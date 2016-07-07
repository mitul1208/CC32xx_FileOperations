# CC32xx_FileOperations
Example Usage of File Operations of cc3200 flash storage file Operations. 

CC3200 - A Wireless MCU integrating high-performance ARM® Cortex™-M4 MCU with on-chip Wi-Fi, internet, and robust security protocols. 
It can be used to develop an entire IoT application with a single IC.

Application Description

It will be sample application for effective usage of simplelink file operation API.
Following will be the operations when you run the application.

1) Build and run application by importing into CCS studio and pluging the CC3200 via USB.  
2) when user will press sw2 button, It will turn on LED incremently and save the status in File.  
3) when user will press sw3 button, It will turn off LED in reverse order and save the status in File.  
4) like volume control it will turn on/off the LEDs.  
5) when user will restart the application. LED will be set to previous state.  

Simplelink API of creating file have bug that it is not returning negative value in if file is already there and instead its replacing it with new file resulting into loss of old file and its content.  

This program remove this vulnerability by utilizing other APIs.
