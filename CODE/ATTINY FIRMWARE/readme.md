It is now possible to change the ON_ an OFF_ THRESHOLDS via a serial terminal. You need a FTDI USB<->SERIAL bridge like [this FTDI cable from Adafruit](https://www.adafruit.com/product/70)

To add this functionality, you must reprogram the ATTINY85

If you don't have a [Sparkfun Tiny ISP Programmer](https://www.sparkfun.com/products/11801)

Use an Arduino to program the ATTINY85

http://highlowtech.org/?p=1695

Afterwards, connect your FTDI cable to the RX, TX and GND pins in the image.

|WW  | FTDI|
|----|-----|
|TX  | RX|
|RX  | TX|
|GND | GND|

![img](./WWII-SERIAL.jpg)

Serial connection parameters are 9600 baud, 8-N-1

A command string starts with the $ character. This is followed with a command type character character. `>` is the command type character to set the ON_THRESHOLD voltage. `<` is the command type character to set the OFF_THRESHOLD. This is followed with a numeric value 0-9999 which represents the desired voltage in millivolts (mV). 1000mV = 1V. The command string is terminated by CR/LF.

A command string looks like this:

$>4200

This sets the ON_THRESHOLD to 4.2V

$<3700

This sets the OFF_THRESHOLD to 3.7V
