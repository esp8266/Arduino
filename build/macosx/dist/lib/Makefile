# Arduino Makefile (for Mac OS X)
# Nick Zambetti and David A. Mellis
# $Id: makefile.osx,v 1.6 2005/05/23 17:19:56 mellis Exp $

# By default, this makefile uses the serial device specified by
# Wiring (either in the Tools | Serial Port menu or the Wiring
# preferences file in the sketchbook directory) and passed as
# an argument to make.  To override this value, uncomment the
# following line and change the value to the desired device.
# SERIAL=/dev/tty.usbserial-1B1

# The Wiring Lite IDE runs the "compile" rule when you click the play button.
compile:
	rm -rf tmp
	mkdir tmp
	cat ../core/wiringlite.inc > tmp/prog.c
	cat build/*.c >> tmp/prog.c
	cp ../core/* tmp/
	../tools/gnumake -s -C tmp

# The IDE runs the "program" rule when you hit the export button.
# The string after the colon determines the method used to 
# program the microcontroller: program-using-bootloader or
# program-with-isp.
program: program-using-bootloader

# This rule is for uploading code using a bootloader already on
# the microcontroller.  It should run at the baud rate specified
# in the bootloader's code, current 9600 baud.
program-using-bootloader:
	../../tools/avr/bin/uisp -dprog=stk500 -dspeed=9600 -dserial=${SERIAL} -dpart=ATmega8 if=tmp/prog.hex --upload

# This rule is for uploading code using an in-system programmer,
# e.g. the one sold by Atmel.  In this case, you want to erase the
# microcontroller, upload the code, and then verify it.
# Atmel's isp follows the stk500 protocol and works at 115200 baud.
program-with-isp:
	../../tools/avr/bin/uisp -dprog=stk500 -dspeed=115200 -dserial=${SERIAL} -dpart=ATmega8 if=tmp/prog.hex --erase --upload --verify
