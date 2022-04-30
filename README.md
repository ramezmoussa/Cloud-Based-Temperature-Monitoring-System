# Healthcare-Identification-System

# Contributers
1. [Hany Moussa](https://github.com/hanymoussa)
2. [Ramez Moussa](https://github.com/ramezmoussa)

# Project Description
The objective of this project is to provide an identification system to be used in healthcare settings. The system will be able to identify any RFID-tagged elements within the environment and provide all the necessary information regarding these elements. We are planning to not only identify medical equipment, but also the patients, the rooms and potentially the doctors. Essentially, the system does not have to be specific to one type of ‘elements’ but rather it can be generalised to identify any present tagged-element within its environment. However, the information provided will vary significantly depending on the type of that element as the information provided for a piece of equipment will be quite different from that of a newly born baby.

# Components
We will be utilising a wide variety of components to provide our generalised healthcare identification system. A list of the hardware components can be found below:
1. STM32L432KC (Microcontroller)
2. RC522 RFID Reader/Writer Module
3. RFID tags 
4. HC-06 bluetooth module
5. ESP-01 ESP8266 Serial Wifi Module
6. Cellphone on which the information will be displayed via bluetooth

In terms of software, we will be utilising:
1. Thinkspeak (an IoT platform on which we will be storing our database)
2. STM32CubeIDE (the IDE for the development)
3. A bluetooth serial terminal on the cellphone to be used

# Initial Project Design
Essentially, the RFID reader will be connected to the microcontroller using SPI. When a tag is read, its ID will be then used to query the database on Thinkspeak, which will be reached using the Wifi module. That API request will return the data related to that element, be it a patient, a device or a doctor. Once that data is returned, it will be sent to the Cellphone using the HC-06 bluetooth module. Moreover, we will be keeping track of the frequency of accessing the different elements, their maintenance status, etc. Furthermore, actions will be taken when necessary. For example, notification messages can be utilised when a medical equipment reaches its maintenance deadline. An initial block diagram of the system design can be found below:

![ES-Project-Block-Diagram drawio (4)](https://user-images.githubusercontent.com/57135988/164934623-6baf4c5f-a7db-45e4-a910-724c485d3158.png)


# First Prototype
For the stage, we made a slight modification which is using ESP32 instead of ESP-8266. We were not able to find ESP-8266 available so we decided to use ESP32 instead because it provides pretty much the same functionality (connecting to Wifi and send/receive from the cloud). We also decided to change our cloud platform to be Firebase instead of Thinkspeak. We realised that Thinkspeak is supported the most with monitoring and/or recording data into the cloud but it is not very well suited for a database-like system. Instead, we will be using Firebase as it has an available dataset feature that would allow us to read and/or write to the database using our microcontroller.

By this stage, we managed to setup ESP32 to connect to Wifi as well as reach Firebase. We created a datastore on Firebase that we will be accessing and an example of accessing a present document is shown below.

For the next prototype, we hope we will successfully send the data read from Firebase to the Nucleoboard using UART and from the Nucleoboard, we will send that data to a mobile phone using a UART-connected Bluetooth module 
