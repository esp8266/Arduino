REM TODO: need way to specify serial port.

cd ..
tools\avr\bin\uisp -dpart=ATmega8 -dprog=stk500 -dserial=com1 -dspeed=115200 --wr_lock=0xFF
tools\avr\bin\uisp -dpart=ATmega8 -dprog=stk500 -dserial=com1 -dspeed=115200 --wr_fuse_l=0xdf --wr_fuse_h=0xca
tools\avr\bin\uisp -dpart=ATmega8 -dprog=stk500 -dserial=com1 -dspeed=115200 --erase --upload --verify if=bootloader\ATMegaBOOT.hex
tools\avr\bin\uisp -dpart=ATmega8 -dprog=stk500 -dserial=com1 -dspeed=115200 --wr_lock=0xCF
cd bootloader
