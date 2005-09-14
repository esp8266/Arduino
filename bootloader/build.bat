avr-gcc -g -Wall -O3 -mmcu=atmega8 -DCRUMB8 -c -o ATmegaBOOT.o ATmegaBOOT.c
avr-gcc -g -Wall -O3 -mmcu=atmega8 -DCRUMB8 -Wl,-Map,ATmegaBOOT.map,--section-start=.text=0x1800 -o ATmegaBOOT.elf ATmegaBOOT.o 
avr-objdump -S -h ATmegaBOOT.elf > ATmegaBOOT.lst
avr-objcopy -j .text -j .data -O ihex ATmegaBOOT.elf ATmegaBOOT.hex
avr-objcopy -j .text -j .data -O binary ATmegaBOOT.elf ATmegaBOOT.bin
avr-objcopy -j .text -j .data -O srec ATmegaBOOT.elf ATmegaBOOT.srec
