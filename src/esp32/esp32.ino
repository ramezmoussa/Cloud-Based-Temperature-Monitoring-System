/*
    Adapted From: https://randomnerdtutorials.com/esp32-thingspeak-publish-arduino/
*/

#include <WiFi.h>
#include "ThingSpeak.h"


const char* ssid = "somerandomnetwork";   // your network SSID (name) 
const char* password = "somerandompassword";   // your network password

WiFiClient  client;

unsigned long channel = 1;
const char * myWriteAPIKey = "APIKEY"; //Thinkspeka API Key

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 30000;

// Variable for Data
float data;


void setup() {
  Serial.begin(115200);  //Initializs the UART terminal  
  WiFi.mode(WIFI_STA);   // Initialise the Wifi
  ThingSpeak.begin(client);  // Initialise ThingSpeak
}

// Superloop
void loop() {
  if ((millis() - lastTime) > timerDelay) {
    
    // Connect to Wifi if possible
    if(WiFi.status() != WL_CONNECTED){
      Serial.print("Attempting to connect");
      while(WiFi.status() != WL_CONNECTED){
        WiFi.begin(ssid, password); 
        delay(5000);     
      } 
      Serial.println("\nConnected.");
    }

    // Create a piece of data
    data = 10;
    Serial.print("Data Value: ");
    Serial.println(data);
    
    
    // Send data to Thinkspeak
    int response = ThingSpeak.writeField(channel, 1, data, myWriteAPIKey);

    if(response == 200){
      Serial.println("Channel update successful.");
    }
    else{
      Serial.println("Sending data to Thinkspeak was not successful. Returned error code: " + String(x));
    }
    lastTime = millis();
  }
}