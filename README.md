# Project Description

The objective of this project is to create a cloud-based system for monitoring the temperature in any given environment. It is generic in the sense that it can be embedded in a car, a factory, a hospital or any other environment of choice. The monitoring system will sense the temperature within the environment periodically and these readings will be saved to the cloud (ThinkSpeak). Such data wil will be plotted and it can be aggregated to extract meaningful insights (patterns, statistics, etc.). Additionally, the system would have a critical temperature (it has a default value but it can be overridden by the user) and whenever the temperature exceeds that critical temperature 3 main things will happen:
  1. A record with the exact date, time, current temperature and the critical temperature will be saved to cloud database on Firebase
  2. A buzzer will go off notifying anyone in the vicinity that an action must take place
  3. The user will receive a message from a Telegram Bot notying them with the scenario for a quick response.
  
Besides, the user can get the current temperature reading at any given time using their smart phones by communicating with the Telegram Bot. By default, the critical temperature is set to 30 degrees but it can be changed by sending a simple command to the Telegram Bot.

# Github Repository
https://github.com/ramezmoussa/Cloud-Based-Temperature-Monitoring-System

# Team Members
1. Hany Moussa - 900171833
2. Ramez Moussa - 900171864


# Components
We will be utilising a wide variety of components to provide our generalised Temperature Monitoring System. A list of the hardware components can be found below:
1. STM32L432KC (Microcontroller)
2. Passive Buzzer (for alarming the user when the temperature exceeds the critical temperature)
3. DS3231 RTC Module (for the temperature sensor as well as the real-time clock
4. ~ESP-01 ESP8266~ ESP-32 Serial Wifi Module (for communicating with ThinkSpeak, Firebase and Telegram)
5. Smartphone (for communicating with the system over Telegram)


In terms of software, we will be utilising:
1. Thinkspeak (for real-time plotting of the streamed temeprature values)
2. Firebase (for recording the incidents when the temperature exceeds the critical value)
3. STM32CubeIDE (the IDE for development on the STM32 board)
4. Arduino (the IDE for development on the ESP32 board)
5. Telegram (for the user to communicate with the system)

# Initial Project Design

The DS3231 will be connected to the STM32 over I2C. It will be sending the temperature as well as the time and date periodically to the microcontroller. The STM32 checks if the read temperature is beyond the critical temperature and if so, it activates a buzzer alarming nearby users. Then, these readings would be relayed to the ESP32 board over UART. Next, the readings will be pushed to ThinkSpeak (every 15 seconds due to the free tier limitations) and if the temperature is above the critical temperature, a record of the current incident will be recorded to Firebase.

Another pipeline that would be concurrently running  is the processing of the Telegram messages. Periodically, we check if there are any new messages received by the Telegram Bot and if so, we process them in order. 

![ES-Project-Milestone2-temp drawio](https://user-images.githubusercontent.com/57135988/168786657-2520e7dd-c7fc-4bb8-8471-13d46233bd0f.png)

# First Prototype
For the stage, we made a slight modification which is using ESP32 instead of ESP-8266. We were not able to find ESP-8266 available so we decided to use ESP32 instead because it provides pretty much the same functionality (connecting to Wifi and send/receive from the cloud). In this prototype, we managed to establish communication with the ThinkSpeak channel and save the temperature values to it. The streaming data could be viewed as a plot or as time-series data, which can be used for machine learning / data analytics

The time series data: [thingspeak_timeseries.csv](https://github.com/ramezmoussa/Cloud-Based-Temperature-Monitoring-System/files/8707635/thingspeak_timeseries.csv)


![1c349598-7bd3-4fe3-bdf7-2fe1f66b0cd3](https://user-images.githubusercontent.com/57135988/168796408-b4ffaa14-2361-4312-ac95-be28f9b32f32.jpg)

  
  
We also managed to initialise a Firebase Datastore and succesfully connect to it and write/read records. We used the Firebase-ESP-Client Library to establish such communication. Essentially, we need to provide the APIKey for the Firebase Project as well as the database URL and the library would handle authentication and access operations.


![WhatsApp Image 2022-04-30 at 1 39 19 PM](https://user-images.githubusercontent.com/57135988/166105949-650e6744-3768-498c-b0ba-312a05b6b229.jpeg)
![6e2f8fc8-e8f3-4e4e-9441-90bdab4d8ea3](https://user-images.githubusercontent.com/57135988/167270024-8726fe68-992f-4d98-b799-b4f21210a2af.jpg)

  
In the next milestone, we will be working on communicating data between ESP32 and STM32 over UART. We will also attempt to work on the Telegram Bot.

## The updated Project Design can be seen below (Milestone 1):
![ES-Project-Temp-System-Design drawio](https://user-images.githubusercontent.com/57135988/168784843-17a93176-a0b7-4ef5-bd45-60feb89c94c1.png)

## Updated Components (Milestone 1):
4. ~ESP-01 ESP8266~ ESP-32 Serial Wifi Module

# Second Prototype
In this milestone, we worked on 2 main aspects:

* We established communication between the ESP32 and the nucleboard over UART. We managed to send temeprature values from the nucleoboard to the ESP32 side. We also established communication in the opposite direction by sending the newly set critical temperature readings from ESP32 to STM32.
  
  ![6125b372-fedb-4de6-bf93-2822e1deb575](https://user-images.githubusercontent.com/57135988/168790664-7fa152dc-d728-44ee-a207-a7418e3b9bbc.jpg)

  
* We managed to use a Telegram Bot to send dummy messages. In the next milestone, we will utilise this bot for better accessibility. We used the Universtal Telegram Bot library for this task. It simply requires the BotToken which is the unique identifier for the Bot and also requires a list of the ChatIDs (users) that would be allowed to communicate with the bot.

  
# Final Milestone
In this milestone, we finalised all the missing aspects. Firstly, we utilised the Telegram Bot library to design a bot facilitating communicating between the user and the system.

There are 3 main commands the the bot understands and they are the following:
  1. /help : it provides more information on how to interact with the bot
  2. /getT : it provides the current temperature of the environment at this given moment
  3. /setT <number> : it sets the critical temperature to the provided value by sending it to the STM32 board over UART.
  
Examples of using these commands can be seen in the screenshots below.
  

<img src="https://user-images.githubusercontent.com/57135988/168791398-b52530fc-22a3-4f6f-a94a-58ca210ff3e5.jpg" width="250">  | <img src="https://user-images.githubusercontent.com/57135988/168791412-efbda157-38a0-4e91-89a1-be0175011d35.jpg" width="250">


  
Finally, we integrated all the components with one another on a logical as well as a physical level. Prior to this point, we had all the components functioning independently. Consequently, we had to integrate them with one another to reach our final system. A picture of the physical design can be found below:

| STM      | Component | PIN |
| ----------- | ----------- | ----------- |
| PA7 (I2C3)     | DS3231   | SCL |
| PB4 (I2C3) | DS3231      | SDA |
| PA8 (TIM1-CH1)     | Passive Buzzer   | PWM |
| PA10 (UART1-RX) | ESP32      | IO17 |
| PA9 (UART1-TX) | ESP32      | IO16 |
| PA2 (UART2-RX) | ESP32      | IO12 |
| PA3 (UART2-TX) | ESP32      | IO14 |
----------------------------------------
  
   ![073ec623-eee3-4f76-9e29-5d4731295c4c](https://user-images.githubusercontent.com/57135988/168792044-6e02b177-0867-490c-ab06-0596ca2ac431.jpg)

  
 # Demo
  
 
 
