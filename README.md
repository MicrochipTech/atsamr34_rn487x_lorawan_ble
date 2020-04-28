# ATSAMR34_RN487x_LORAWAN_BLE
> “Wireless Made Easy!" - Adding BLE features & provisioning to an existing LoRaWAN Application

<p>
<a href="https://www.microchip.com" target="_blank">
<img border="0" alt="Microchip_logo" src="Doc/Microchip_logo.png" width="200">
</a>
</p>

**This sample code demonstrates how to provision & control a LoRaWAN end-device over a BLE connection between Smartphone App and a RN4870/71 module.**

For more information on Microchip ATSAMR34 LoRa SiP and RN4870/71 BLUETOOTH 5 certified module, visit Microchip webpages: </br>

https://www.microchip.com/design-centers/wireless-connectivity/low-power-wide-area-networks/lora-technology/sam-r34-r35
https://www.microchip.com/wwwproducts/en/rn4870
https://www.microchip.com/wwwproducts/en/rn4871


## Get a Microchip ATSAMR34 Xplained Pro evaluation kit

The ATSAMR34 Xplained Pro evaluation kit (DM320111) is a hardware platform used to evaluate the ATSAMR34 Low Power LoRa® Sub-GHz SiP. It is supported by Atmel Studio 7.0 IDE and a collection of sample codes are available from Advanced Software Framework (ASFv3) since the 3.44.0 release.
The Xplained Pro MCU series evaluation kit includes an on-board Embedded Debugger (EDBG), and no external tools are necessary to program or debug the ATSAMR34.
The kit offers a set of features that enable the user to get started with the ATSAMR34 Low Power LoRa® Sub-GHz SiP peripherals right away, and to understand how to integrate the device in your own design.
The ATSAMR34 Xplained Pro kit contains the following items:
- One ATSAMR34 Xplained Pro </br>
- One external antenna </br>

![](Doc/ATSAMR34Xpro.png)

https://www.microchip.com/DevelopmentTools/ProductDetails/dm320111

## Get a Microchip RN4871-PICTAIL evaluation kit

The RN-4871-PICtail is a development board based on the ultra-compact Bluetooth 5 Low Energy RN4871 module. The RN4871 uses a simple ASCII command interface over the UART. 

![](Doc/rn-4871-pictail.png)

https://www.microchip.com/DevelopmentTools/ProductDetails/PartNO/RN-4871-PICTAIL


![](Doc/wiring_setup.png)

## Get a RN4870/71 click board

Alternatively, you can use a RN4870 or RN4871 click board from Mikroe.

![](Doc/rn4871-click.png)

https://www.mikroe.com/rn4870-click
https://www.mikroe.com/rn4871-click

And use mikroBUS Xplained-Pro board (ATMBUSADAPTER-XPRO).

https://www.microchip.com/Developmenttools/ProductDetails/ATMBUSADAPTER-XPRO

With or without wiring...
![](Doc/setup.png)

## Software

- Download and install Atmel Studio 7.0 IDE. </br>
https://www.microchip.com/mplab/avr-support/atmel-studio-7

- Open Atmel Studio 7.0 IDE. </br>
- Then, you need Advanced Software Framework (ASFv3) v3.45.0 release or upper release. </br>
Install ASFv3 as an extension to Atmel Studio from the menu: Tools -> Extensions and Updates …
- Once the installation is complete, you must restart Atmel Studio. </br>
- Download and install a serial terminal program like Tera Term. </br>
https://osdn.net/projects/ttssh2/releases/

Note: ASFv3 is an MCU software library providing a large collection of embedded software for AVR® and SAM flash MCUs and Wireless devices. ASFv3 is configured by the ASF Wizard in Atmel Studio 7.0 (installed as an extension to Studio). ASFv3 is also available as a standalone (.zip) with the same content as Studio extension (https://www.microchip.com/mplab/avr-support/advanced-software-framework).

Important:
Until the next Atmel Studio IDE release, you have to manually install the Device Part Pack for developing with SAMR34/R35 on Atmel Studio 7.0 IDE.
(all products released in between IDE releases of Atmel Studio should be manually added by user to develop applications).
- Go to Tools -> Device Pack Manager </br>
- Check for Updates </br>
- Search for SAMR34 and click install </br>
- Repeat the same for SAMR35 </br>
- Restart Atmel Studio 7.0 IDE </br>

## Hardware Setup

To demonstrate BLE applications as well as LoRaWAN end-device provisioning, follow the connection below :

![](Doc/wiring.png)
![](Doc/samr34xpro_ext1_header.png)
![](Doc/rn4871-pictail_interface.png)
![](Doc/rn4871-click_pinout.png)

- In order to connect in LoRaWAN, you will need to be in range of your gateway. If you don’t have gateway, check the world map (https://www.thethingsnetwork.org/map) to see if your local community has gateway deployed around your location.
- Plug the antennas and always make sure you have the antennas plugged to your boards before powering it up. </br>
- Connect the ATSAMR34-Xpro board to the PC through the micro-USB cables. </br>
USB cable must be connected to the EDBG USB connectors of the ATSAMR34 kit. </br>
The USB ports powers the board. 
- Connect the FTDI cable the host PC. A COM port will be mounted. This port mounted will be used to communicate with the kit. </br>
- Launch Tera Term program and configure the serial ports mounted with: 115200 bps, 8/N/1 </br>

## Mobile Apps

This demo requires:
- Microchip Bluetooth Data (MBD) Apps for mobile
</br>
For Google device: https://play.google.com/store/apps/details?id=com.microchip.bluetooth.data&hl=en
</br>
For Apple device: https://apps.apple.com/us/app/microchip-bluetooth-data/id1319166097
</br>
- Microchip Bluetooth Smart Data
</br>
For Apple device: https://apps.apple.com/fr/app/bluetooth-smart-data/id1004033562
</br>
or
- BLE Terminal
</br>
For Google device: https://play.google.com/store/apps/details?id=de.kai_morich.serial_bluetooth_terminal&hl=en
</br>
For Apple device: https://apps.apple.com/fr/app/ble-terminal-hm-10/id1398703795
</br>
This demo is using QRbot apps to scan a QR code. This QRcode contains the LoRaWAN provisioning information.
</br>
QRbot is available for Google device: https://play.google.com/store/apps/details?id=net.qrbot&hl=en
</br>
QRbot is available for Apple device: https://apps.apple.com/cy/app/qrbot/id1048473097

## TTN Provisioning

- Log in to your TTN account
- Go to the Console
- Create an Application and note the AppEUI generated by TTN
- From the application created, add a Device
- For the DevEUI, use the MAC address written in the back of the SAMR34 Xplained Pro board.
![](Doc/ATSAMR34Xpro_Back.png)
- Let TTN generate the AppKey

![](Doc/TTN_Device_Provisioning.png)

When the device is created, note carrefully the OTAA credentials (DevEUI, AppEUI, AppKey). 
You will needed later to generate the QR Code.

You have done the provisioning of the end-device within the TTN Console.


## Generating QR Code for End-device provisioning

A QR Code will be used to provision the LoRaWAN OTAA parameters (DevEUI, AppEUI, AppKey) over BLE.

To be recognized by the SAMR34 sample application, the QR code must contains a string of characters which respect the following format:

![](Doc/mac_set_otaa.png)

Create your own QR Code from: https://www.unitag.io/qrcode
- As QR Code Type: select Other and Text
- In the textbox, type your own string of characters with the help of your OTAA credentials
- Click confirm to generate your own QR Code
![](Doc/QR_Code_Generator.png)

You can download and print your QR Code.

From your smartphone, Open the QRbot apps and scan your QR Code. 

![](Doc/QRbot_scan1.png)

You should retrieve the "mac set otaa" command with your credentials.
Copy the text.

![](Doc/QRbot_scan2.png)

## BLE + LoRaWAN in a single Application

This project integrates the Microchip LoRaWAN Stack (MLS) Software API which provide an interface to the different software modules. </br></br>
This application is based on LoRaWAN Mote Application generated from ASFv3 and already contains drivers required to interface the RN4870/71 module.

It implements the Transparent UART service. This service exposes the characteristics to transfer serial data on both direction.

From the menu, you have the choice of the application:

![](Doc/ble_app_menu.png)

## Tutorial

- Start Atmel Studio 7 IDE
- Open this project
- From the Atmel Studio menu, go to: Build -> Build Solution
- Flash the firmware on the ATSAMR34 Xplained Pro board
- To get console logs through EDBG interface, open a Tera Term session with 115200 bps 8N1 configuration.
- Reset the board
- The application will start and should expose a menu if the interface with RN4870/71 module is correct</br>

```
Try to initialize BLE device ...

===============================================
Menu: select a BLE application from the list
===============================================
1) Serial bridge
2) Beacon mode - https://www.microchip.com URL
3) Transparent chat mode
4) Scan mode
5) Search mode
6) Search and connect
7) Sleep mode
8) Wake-up
9) LoRaWAN provisioning
-----------------------------------------------
>>
```

- From here, simply select a BLE application you want to be executed</br>
- And follow the instructions displayed on the console

## Serial bridge

![](Doc/ble_app_serial_bridge.png)

## Beacon mode

![](Doc/ble_app_beacon.png)
![](Doc/MBD.png)
![](Doc/beacon_ranging.png)

## Transparent chat mode

![](Doc/ble_app_transparent_uart.png)
![](Doc/SmartData_AppStore.png)
![](Doc/smart_data.png)
![](Doc/transparent_uart_1.png)
![](Doc/transparent_uart_2.png)

## Scan mode

![](Doc/ble_app_scan.png)

## Search mode

![](Doc/ble_app_search.png)

## Search and connect

![](Doc/ble_app_search_and_connect.png)

## Sleep mode

![](Doc/ble_app_sleep.png)

## Wake-up 

![](Doc/ble_app_wakeup.png)

## LoRaWAN provisioning

![](Doc/ble_app_lorawan_provisioning_1.png)
![](Doc/SmartData_AppStore.png)
![](Doc/smart_data.png)
![](Doc/lorawan_provisioning_1.png)
![](Doc/ble_app_lorawan_provisioning_2.png)
![](Doc/lorawan_provisioning_2.png)

- From here, the SAMR34 will automatically send a Join Request message to activate the device within the LoRaWAN nettwork
- If the activation succeed, you will be notified by a "Joining Successful" message on the BLE apps
- Go back to the TTN Console, <your application> and observe the Application Data of your device
- You should see the Join procedure
![](Doc/TTN_join_success.png)
- Go back to the TTN Console and observe the data received in HEX
![](Doc/TTN_data_received.png)
