Arduino Gemma Bootloader
========================

The Arduino Gemma Bootloader is based on the Adafruit Trinket/Gemma Bootloader. In the Arduino Gemma bootloader the USB VID&PID, the Manufacturer name and the Device name parameters are changed.

The source code of the bootloader of the version used at the compile time can be found at the following link: https://github.com/adafruit/Adafruit-Trinket-Gemma-Bootloader/tree/3bc1bb561273535d4d493518a233a3a1fccf6b76

The *'usbconfig.h'* and the *'usbconfig.patch'* files are provided if you want to recompile the bootloader.
You only need to replace the original *'usbconfig.h'* file with this one or patch it with the provided patch file.

**Please note: you cannot use the Arduino USB VID/PID for your own non-Gemma products or projects. Purchase a USB VID for yourself at**  http://www.usb.org/developers/vendor/ 



