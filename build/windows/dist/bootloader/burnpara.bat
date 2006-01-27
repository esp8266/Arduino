@REM burnpara.bat
@REM David A. Mellis
@REM 27 January 2006
@REM Burns bootloader onto Arduino board with a parallel port programmer.

REM Before running this for the first time, you'll need to install giveio.
REM Unzip drivers\giveio.zip, then run ginstall.bat.

@REM Need to be in directory with cygwin dll's.
cd ..

tools\avr\bin\uisp -dpart=ATmega8 -dprog=dapa -dlpt=0x378 --wr_lock=0xFF
tools\avr\bin\uisp -dpart=ATmega8 -dprog=dapa -dlpt=0x378 --wr_fuse_l=0xdf --wr_fuse_h=0xca
tools\avr\bin\uisp -dpart=ATmega8 -dprog=dapa -dlpt=0x378 --erase --upload --verify if=bootloader\ATMegaBOOT.hex
tools\avr\bin\uisp -dpart=ATmega8 -dprog=dapa -dlpt=0x378 --wr_lock=0xCF

@REM Return to bootloader directory.
cd bootloader
