Arduino Uno and Mega 2560 Firmwares for the ATmega8U2

This directory contains the firmwares used on the ATmega8U2 on the Arduino
Uno and Arduino Mega 2560.  The arduino-usbdfu directory contains the DFU
bootloader on the 8U2; the arduino-usbserial directory contains the actual
usb to serial firmware.  Both should be compiled against LUFA 100807.  The
two .hex files in this directory combine the dfu and serial firmwares into
a single file to burn onto the 8U2.  

To burn (Uno):
avrdude -p at90usb82 -F -P usb -c avrispmkii -U flash:w:UNO-dfu_and_usbserial_combined.hex -U lfuse:w:0xFF:m -U hfuse:w:0xD9:m -U efuse:w:0xF4:m -U lock:w:0x0F:m

To burn (Mega 2560):
avrdude -p at90usb82 -F -P usb -c avrispmkii -U flash:w:MEGA-dfu_and_usbserial_combined.hex -U lfuse:w:0xFF:m -U hfuse:w:0xD9:m -U efuse:w:0xF4:m -U lock:w:0x0F:m