..\wiringlite\bin\uisp -dpart=ATmega8 -dprog=stk500 -dserial=com1 -dspeed=115200 --wr_fuse_l=0xdf --wr_fuse_h=0xc8
..\wiringlite\bin\uisp -dpart=ATmega8 -dprog=stk500 -dserial=com1 -dspeed=115200 --erase --upload --verify if=ATMegaBOOT.hex
