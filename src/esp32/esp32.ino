/*
    Adapted From: https://randomnerdtutorials.com/esp32-firebase-realtime-database/
*/

#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
#endif

// Include the Firebase ESPClient module
#include <Firebase_ESP_Client.h>

//Provide the token generation process info.
#include "addons/TokenHelper.h"
//Provide the RTDB payload printing info and other helper functions.
#include "addons/RTDBHelper.h"

// Wifi Configuration
#define WIFI_SSID "somerandomnetwork"
#define WIFI_PASSWORD "somerandompassword"

// Firebase Configuration
#define API_KEY "APIKEY"
#define DATABASE_URL "DATABASEURL" 


#define RXD2 16
#define TXD2 17

int incomingByte = 0; // for incoming serial data

// Firebase
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;


String stringValue;
bool signupOK = false;

void setup() {
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);

  Serial.begin(115200);     // Connect to the serial terminal for printing
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD); // Connect to Wifi if possible
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Assign Firebase configuration variables
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Connect to Firebase
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("ok");
    signupOK = true;
  }
  else {
    Serial.printf("%s\n", config.signer.signupError.message.c_str());
  }

  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; //see addons/TokenHelper.h

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  Serial2.flush();
  delay(1000);
}


void loop() {
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > 15000 || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();
    
      if (Serial2.available() > 0) {
      // read the incoming byte:
      String i = Serial2.readString();
      
      char receivedMessage [100] = {0};
      sprintf(receivedMessage, "Data received from nucleo board: %s", i);
      Serial.println(receivedMessage);
      Serial.println("Now querying the firebase database");
      //String i = "";
      //i += c;
      //int i = myByte-48;
      // prints the received data

      char getTypeBuf [100] = {0};
      sprintf(getTypeBuf, "/%s/type", i);
      
      char getNameBuf [100] = {0};
      sprintf(getNameBuf, "/%s/name", i);

      char idMessage [100] = {0};
      sprintf(idMessage, "ID: %s", i);
      Serial.println(idMessage);
      if (Firebase.RTDB.getString(&fbdo, getTypeBuf)) {
        if (fbdo.dataType() == "string") {
          stringValue = fbdo.stringData();
          
          char typeMessage [100] = {0};
          sprintf(typeMessage, "Type: %s", stringValue);
          Serial.println(typeMessage);
        }
        }
        else {
          Serial.println(fbdo.errorReason());
        }


        if (Firebase.RTDB.getString(&fbdo, getNameBuf)) {
        if (fbdo.dataType() == "string") {
          stringValue = fbdo.stringData();
          
          char nameMessage [100] = {0};
          sprintf(nameMessage, "Name: %s", stringValue);
          Serial.println(nameMessage);
          
        }
        }
        else {
          Serial.println(fbdo.errorReason());
        }
        Serial.println("------------------------------------------------------------");
      }
  }
}