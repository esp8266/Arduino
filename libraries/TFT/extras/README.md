TFT Library
============

An Arduino library for the Arduino TFT LCD screen.

This library enables an Arduino board to communicate with an Arduino TFT LCD screen. It simplifies the process for drawing shapes, lines, images, and text to the screen.
The Arduino TFT library extends the Adafruit GFX, and Adafruit ST7735 libraries that it is based on. The GFX library is responsible for the drawing routines, while the ST7735 library is specific to the screen on the Arduino GTFT. The Arduino specific additions were designed to work as similarly to the Processing API as possible.

Onboard the screen is a SD card slot, which can be used through the SD library.

The TFT library relies on the SPI library for communication with the screen and SD card, and needs to be included in all sketches.

https://github.com/adafruit/Adafruit-GFX-Library
https://github.com/adafruit/Adafruit-ST7735-Library
http://www.arduino.cc/en/Reference/SD
http://www.arduino.cc/en/Reference/SPI

http://www.arduino.cc/en/Reference/TFTLibrary