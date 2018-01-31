# HeadLight with ATtiny13 core
#### Vexilurz (c)
This repository for create flashlight using ATtiny13 (DIY).
* Idea from: [https://geektimes.ru/post/255004/](https://geektimes.ru/post/255004/)
* For ATtiny13 support in Arduino IDE: [https://github.com/MCUdude/MicroCore](https://github.com/MCUdude/MicroCore)
* Library for interrupts support: [https://github.com/NicoHood/PinChangeInterrupt](https://github.com/NicoHood/PinChangeInterrupt)

<img src="https://habrastorage.org/files/64d/229/621/64d22962193c4e70a197986030ec306a.jpg" width="600">

## How to program ATtiny13 via "Arduino as ISP":
[In Russian](https://drive.google.com/open?id=1-w3zYSKzwpRuDV7iiXSNdWmjPVqMFLKMCIAcvBqBYi4)
* [Install ATtiny13 board support using MicroCore](https://github.com/MCUdude/MicroCore#how-to-install)
* Load "ArduinoISP" example sketch
* Remove comment in line `#define USE_OLD_STYLE_WIRING`
* Swap comments on lines `#define SPI_CLOCK`
* Define baudrate 250 instead 19200 `#define BAUDRATE 250`
* Program Arduino
* Connect ATtiny13 to the Arduino like in picture below 
* Choose ATtiny13 in Arduino IDE boards
* Choose **Arduino as ISP (slow)** programmer
* Burn bootloader
* Return baudrate to the programmer (19200 instead 250)
* Now you can program ATtiny13 via **Arduino as ISP** (not slow!) programmer
* (Don't forget to install [PinChangeInterrupt](https://github.com/NicoHood/PinChangeInterrupt) library for this project!)
<img src="http://msyst.ru/images/attiny_arduino_connect.png" width="468">

## ATtiny13 pinout
This diagram shows the pinout and the peripherals of ATtiny13.
<b>Click to enlarge:</b> 
</br> </br>
<img src="http://i.imgur.com/JsbguPV.jpg" width="800">