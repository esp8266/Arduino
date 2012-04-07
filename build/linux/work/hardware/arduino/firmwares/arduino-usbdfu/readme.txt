To setup the project and program an ATMEG8U2 with the Arduino USB DFU bootloader:
1. unpack the source into LUFA's Bootloader directory
2. set ARDUINO_MODEL_PID in the makefile as appropriate 
3. do "make clean; make; make program"

Check that the board enumerates as either "Arduino Uno DFU" or "Arduino Mega 2560 DFU".  Test by uploading the Arduino-usbserial application firmware (see instructions in Arduino-usbserial directory)

