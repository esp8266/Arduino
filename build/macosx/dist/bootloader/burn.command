#!/bin/sh

# fixmybootloader.command  25.06.2005 mbanzi
# 
# Arduino project http://arduino.berlios.de
#
# quick and dirty script to set the proper fuses and lock bits
# while loading the bootloader onto a brand new arduino board
#
# very useful also when for some reasons the bootloader disappears
# 
# TODO: cleanup and make it more user friendly
# TODO: move this (and related) scripts to platform dist/ dirs
#
# expects an STK500 compatible programmer on the specified serial port
# if you use the parallel port programmer you need to change the dprog
# parametre
# 

BINDIR=../../tools/avr/bin
PORT=/dev/tty.USA19QW1b1P1.1

# unlock bootloader segment
$BINDIR/uisp -dpart=ATmega8 -dprog=stk500 -dserial=$PORT -dspeed=115200 --wr_lock=0xFF

# set fuses
# bootloader size of 512 words; from 0xE00-0xFFF
# clock speed of 16 MHz, external quartz
$BINDIR/uisp -dpart=ATmega8 -dprog=stk500 -dserial=$PORT -dspeed=115200 --wr_fuse_l=0xdf --wr_fuse_h=0xca

# upload bootloader
$BINDIR/uisp -dpart=ATmega8 -dprog=stk500 -dserial=$PORT -dspeed=115200 --erase --upload if=ATMegaBOOT.hex -v

# lock bootloader segment
$BINDIR/uisp -dpart=ATmega8 -dprog=stk500 -dserial=$PORT -dspeed=115200 --wr_lock=0xCF
