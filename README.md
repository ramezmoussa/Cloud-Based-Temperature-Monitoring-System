# Project Description
The objective of this project is to provide an identification system to be used in healthcare settings. The system will be able to identify any RFID-tagged elements within the environment and provide all the necessary information regarding these elements. We are planning to not only identify medical equipment, but also the patients, the rooms and potentially the doctors. Essentially, the system does not have to be specific to one type of ‘elements’ but rather it can be generalised to identify any present tagged-element within its environment. However, the information provided will vary significantly depending on the type of that element as the information provided for a piece of equipment will be quite different from that of a newly born baby.

# Github Repository
https://github.com/ramezmoussa/Healthcare-Identification-System

# Team Members
1. Hany Moussa - 900171833
2. Ramez Moussa - 900171864


# Components
We will be utilising a wide variety of components to provide our generalised healthcare identification system. A list of the hardware components can be found below:
1. STM32L432KC (Microcontroller)
2. ~RC522~ PN532 RFID Reader/Writer Module (updated while working on the secondprototype)
3. RFID tags 
4. ~HC-06 bluetooth module~ (not needed anymore as we will use Telegram instead)
5. ~ESP-01 ESP8266~ ESP-32 Serial Wifi Module (updated while working on the first prototype)
6. Cellphone on which the information will be displayed via bluetooth


In terms of software, we will be utilising:
1. ~Thinkspeak~ (Firebase) (updated while working on the first prototype)
2. STM32CubeIDE (the IDE for the development)
3. ~A bluetooth serial terminal on the cellphone to be used~ Telegram (updated while working on the second prototype)

# Initial Project Design
Essentially, the RFID reader will be connected to the microcontroller using SPI. When a tag is read, its ID will be then used to query the database on Thinkspeak, which will be reached using the Wifi module. That API request will return the data related to that element, be it a patient, a device or a doctor. Once that data is returned, it will be sent to the Cellphone using the HC-06 bluetooth module. Moreover, we will be keeping track of the frequency of accessing the different elements, their maintenance status, etc. Furthermore, actions will be taken when necessary. For example, notification messages can be utilised when a medical equipment reaches its maintenance deadline. An initial block diagram of the system design can be found below:

![ES-Project-Block-Diagram drawio (4)](https://user-images.githubusercontent.com/57135988/164934623-6baf4c5f-a7db-45e4-a910-724c485d3158.png)


# First Prototype
For the stage, we made a slight modification which is using ESP32 instead of ESP-8266. We were not able to find ESP-8266 available so we decided to use ESP32 instead because it provides pretty much the same functionality (connecting to Wifi and send/receive from the cloud). We also decided to change our cloud platform to be Firebase instead of Thinkspeak. We realised that Thinkspeak is supported the most with monitoring and/or recording data into the cloud but it is not very well suited for a database-like system. Instead, we will be using Firebase as it has an available dataset feature that would allow us to read and/or write to the database using our microcontroller.

## The updated Project Design can be seen below (Milestone 1):

![ES-Project-Block-Diagram drawio](https://user-images.githubusercontent.com/57135988/166105890-42e031b8-7e6a-409e-9940-051029782674.png)

## Updated Components (Milestone 1):
5. ~ESP-01 ESP8266~ ESP-32 Serial Wifi Module

## Updated Software (Milestone 1):
1. ~Thinkspeak~ Firebase


By this stage, we managed to setup ESP32 to connect to Wifi as well as reach Firebase. We created a datastore on Firebase that we will be accessing and examples of accessing present data is shown in the screenshot below.

![WhatsApp Image 2022-04-30 at 1 39 19 PM](https://user-images.githubusercontent.com/57135988/166105949-650e6744-3768-498c-b0ba-312a05b6b229.jpeg)

For the next milestone, we will attempt to use the RFID module to read the IDs from the tags, send them to the nucleoboard over SPI. Then, we will send that data to the ESP32 board over UART. Finallly, we will use that received data to query Firebase and get the data related to that ID. 

For the final milestone, we will utilise ESP32 and/or the nucleoboard to send the read data to the user using Bluetooth. Potentially, we can also utilise a Telegram and/or WhatsApp bots to send the data using that as well. 


# Second Prototype
In this milestone, we worked on 3 main aspects:
* We attempted to utilise the RC522 RFID module to read the RFID tags. Unfortunately, after various attempts we were not successful at interfacing that module with our nucleoboard. We experimented with various libraries but reading the tags was not possible using any of them. We suspect we have one of two potential issues: 1- The libraries might not be compatible for our nucleoboard (they were developed for stm32f1xx series). 2- The module we are using might be faulty. We also experimented with interfacing the RFID module with the ESP32 board and our attempts were not successful. As a result, we will be attempting to use a different RFID module, namely PN532, in the final milestone.

* We established communication between the ESP32 and the nucleboard over UART. We managed to send mock IDs from the nucleoboard and receive them from the ESP32 side.

* We used the received IDs on the ESP32 board to query Firebase and access the data related to our unique IDs.

* We managed to use a Telegram Bot to send dummy messages. In the next milestone, we will utilise this bot to send the data from Firebase to the user for display.

![6e2f8fc8-e8f3-4e4e-9441-90bdab4d8ea3](https://user-images.githubusercontent.com/57135988/167270024-8726fe68-992f-4d98-b799-b4f21210a2af.jpg)


## The updated Project Design can be seen below (Milestone 2):
![ES-Project-Block-Diagram drawio (2)](https://user-images.githubusercontent.com/57135988/167269972-b14e1949-12a8-4df2-bf56-fb4e4d7b9596.png)


## Updated Components (Milestone 2):
2. ~RC522~ PN532 RFID Reader/Writer Module
4. ~HC-06 bluetooth module~ (not needed anymore as we will use Telegram instead)


## Updated Software (Milestone 2):
3. ~A bluetooth serial terminal on the cellphone to be used~ Telegram

In the final milestone we will work on the following:
* Expand the database fields to be more representative instead of the dummy fields we are utilising now
* Use the RFID module to read IDs properly instead of creating mock IDs
* Integrate the Telegram Bot with the other components to be used for displaying the results.
