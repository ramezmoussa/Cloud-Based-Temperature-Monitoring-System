
#include <WiFi.h>
#include "ThingSpeak.h"
#include <Arduino.h>
#include <Firebase_ESP_Client.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <stdio.h>

float critical_temp = 30;

// Initialize Telegram BOT
#define BOTtoken "###############################"  // your Bot Token (Get from Botfather)

// Use @myidbot to find out the chat ID of an individual or a group
// Also note that you need to click "start" on a bot before it can
// message you
#define CHAT_ID "#########"

WiFiClientSecure client;
WiFiClient thingSpeakClient;

UniversalTelegramBot bot(BOTtoken, client);

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

float temperature;

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i = 0; i < numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID) {
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }

    // Print the received message
    String text = bot.messages[i].text;
    const char* charText = text.c_str();

    Serial.println(text);

    String from_name = bot.messages[i].from_name;


    if (text == "/help") {
      String msg = "Welcome, " + from_name + ".\n";
      msg += "This Bot is responsible for monitoring the temperature of the system at any given time.\n\n";
      msg += "For help you can use the /help command\n\n";
      msg += "You can check the current temperature by using the command /getT \n\n";
      msg += "Additionally, by default, if the temperature exceeds 30 degrees, you will receive a notification.\n\n";
      msg += "You can change this default value to any value you prefer using the command /setT <number>, for example /setT 32.5 would set the critical temperature to 32.5\n\n";
      bot.sendMessage(chat_id, msg);
    }

    else if (text == "/getT") {
      char tempString[10];
      sprintf(tempString, "%f", temperature);
      String msg = "Welcome, " + from_name + ".\n";
      msg += "The current Temperature of the system is: ";
      msg += tempString;
      bot.sendMessage(chat_id,  msg);
    }

    else if (strncmp(charText, "/setT", 4) == 0)
    {
      String msg = "Welcome, " + from_name + ".\n";
      msg += "The Critical Temperature has been changed successfully to: ";
      critical_temp = atof(charText + 5);


      char setTempString[10];
      sprintf(setTempString, "%.2f", critical_temp);
      Serial1.print(setTempString);

      Serial.println("New Critical Temp");
      Serial.println(critical_temp);
      char tempString[10];
      sprintf(tempString, "%f", critical_temp);
      msg += tempString;
      msg += "\n\n";
      bot.sendMessage(chat_id, msg);
    }

    else
    {
      char tempString[50] = "Invalid Command. Use /help for more details";
      //    sprintf(tempString, "The current temperature of the system is: %f C.\n\n", temperature) ;
      String msg = "Welcome, " + from_name + ".\n";
      msg += tempString;
      bot.sendMessage(chat_id, tempString, "");
    }


  }
}

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Insert Firebase project API Key
#define API_KEY "###############################"

// Insert RTDB URLefine the RTDB URL */
#define DATABASE_URL "https://embeddedsystemsprojectdb-default-rtdb.europe-west1.firebasedatabase.app/"

//Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;
String stringValue;
bool signupOK = false;


const char* ssid = "wifi-network";   // your network SSID (name)
const char* password = "password";   // your network password

unsigned long myChannelNumber = 1;
const char * myThingSpeakWriteAPIKey = "############";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

// Variable to hold temperature readings
float temperatureC;
//uncomment if you want to get temperature in Fahrenheit
//float temperatureF;


String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
int day, month, year, second, minute, hour;


int lastRan = millis();

#define RXD2 16
#define TXD2 17


void setup() {
  inputString.reserve(200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  Serial.begin(115200); // opens serial port, sets data rate to 115200 bps
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }


  bot.sendMessage(CHAT_ID, "Bot started up for more information, use the /help command", "");
  String msg = "Welcome!! .\n";
  msg += "This Bot is responsible for monitoring the temperature of the system at any given time.\n\n";
  msg += "For help you can use the /help command\n\n";
  msg += "You can check the current temperature by using the command /getT \n\n";
  msg += "Additionally, by default, if the temperature exceeds 30 degrees, you will receive a notification.\n\n";
  msg += "You can change this default value to any value you prefer using the command /setT <number>, for example /setT 32.5 would set the critical temperature to 32.5\n\n";
  bot.sendMessage(CHAT_ID, msg);

  ThingSpeak.begin(thingSpeakClient);  // Initialize ThingSpeak


  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  /* Sign up */
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("\nok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial1.begin(115200, SERIAL_8N1, 14, 12);
  delay(1000);
}

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {

    parseDataFromSTM32();

    // Connect or reconnect to WiFi
    if (WiFi.status() != WL_CONNECTED) {
      Serial.print("Attempting to connect");
      while (WiFi.status() != WL_CONNECTED) {
        WiFi.begin(ssid, password);
        delay(5000);
      }
      Serial.println("\nConnected.");
    }

    // Write to ThingSpeak. There are up to 8 fields in a channel, allowing you to store up to 8 different
    // pieces of information in a channel.  Here, we write to field 1.
    int x = ThingSpeak.writeField(myChannelNumber, 1, temperature, myThingSpeakWriteAPIKey);
    Serial.println(temperature);
    if (x == 200) {
      Serial.println("Channel update successful.");
    }
    else {
      Serial.println("Problem updating channel. HTTP error code " + String(x));
    }

    if (temperature >= critical_temp)
    {
      if (Firebase.ready() && signupOK) {
        sendDataPrevMillis = millis();
        //YYYY-MM-DD_HH:MM:SS
        char datetime_entry [100] = {0};
        sprintf(datetime_entry, "%d-%d-%d_%d:%d:%d/Temperature/float", year, month, day, hour, minute, second);
        if (Firebase.RTDB.setFloat(&fbdo, datetime_entry, temperature)) {
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
          Serial.println("TYPE: " + fbdo.dataType());
        }
        else {
          Serial.println("FAILED");
          Serial.println("REASON: " + fbdo.errorReason());
        }


        sprintf(datetime_entry, "%d-%d-%d_%d:%d:%d/Critical_Temperature/float", year, month, day, hour, minute, second);
        if (Firebase.RTDB.setFloat(&fbdo, datetime_entry, critical_temp)) {
          Serial.println("PASSED");
          Serial.println("PATH: " + fbdo.dataPath());
          Serial.println("TYPE: " + fbdo.dataType());
        }
        else {
          Serial.println("FAILED");
          Serial.println("REASON: " + fbdo.errorReason());
        }

      }

      //alarm user every 30 s
      if (millis() - lastRan > 30000)
      {
        lastRan = millis();
        String msg = "WARNING!\n The temperature has exceeded the critical value. The current temperature is: ";
        char tempString[10];
        sprintf(tempString, "%f", temperature);
        msg += tempString;
        msg += "\n\nThe set critical temperature is: ";

        sprintf(tempString, "%f", critical_temp);
        msg += tempString;
        msg += "\n\n";
        bot.sendMessage(CHAT_ID, msg);
      }
    }


    if (millis() > lastTimeBotRan + botRequestDelay)  {
      int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

      while (numNewMessages) {
        Serial.println("got response");
        handleNewMessages(numNewMessages);
        numNewMessages = bot.getUpdates(bot.last_message_received + 1);
      }
      lastTimeBotRan = millis();
    }



    // clear the string:
    inputString = "";
    stringComplete = false;

    day = 0; month = 0; year = 0; hour = 0; minute = 0; second = 0; temperature = 0;
  }

  if (Serial2.available() > 0)
  {
    char c = Serial2.read();
    if (c == '#')
    {
      stringComplete = true;
    }
    else if (c == '~')
      inputString = "";
    else
    {
      inputString += c;
    }
  }

}



void parseDataFromSTM32()
{
  char temp[2];
  memcpy(temp, &inputString[11], 2);
  day = atoi(temp);
  memcpy(temp, &inputString[14], 2);
  month = atoi(temp);
  memcpy(temp, &inputString[19], 2);
  year = atoi(temp);

  memcpy(temp, &inputString[22], 2);
  hour = atoi(temp);
  memcpy(temp, &inputString[25], 2);
  minute = atoi(temp);
  memcpy(temp, &inputString[28], 2);
  second = atoi(temp);

  char floatTemp[5];
  memcpy(floatTemp, &inputString[44], 5);
  temperature = atof(floatTemp);

  char printBuffer[50] = {0};

  // Uncomment for debugging
  // sprintf(printBuffer, "~Timestamp: %d/%d/20%d %d:%d:%d Temperature: %f# \r\n", day, month, year, hour, minute, second,temperature);
  // Serial.println(printBuffer);

}
