#
# $Id: Makefile,v 1.1 2002/12/18 22:35:38 joerg_wunsch Exp $
#
CC= avr-gcc
#MCU=atmega8
#MCU=atmega16
#MCU=atmega32
#MCU=atmega163
#MCU=atmega323
MCU=atmega128

CFLAGS=  -O -g -Wall -ffreestanding -mmcu=$(MCU)

.SUFFIXES: .s .bin .out .hex .srec

.c.s:
	$(CC) $(CFLAGS) -S $<

.S.o:
	$(CC) $(ASFLAGS) -c $<

.o.out:
	$(CC) $(CFLAGS) -o $@ $<

.out.bin:
	avr-objcopy -O binary $< $@

.out.hex:
	avr-objcopy -O ihex $< $@

.out.srec:
	avr-objcopy -O srec $< $@

all:	twitest.bin

OBJS=twitest.o
twitest.out: $(OBJS)
	$(CC) -o twitest.out $(CFLAGS) $(LDFLAGS) $(OBJS) $(LDLIBS)

clean:
	rm -f *~ *.out *.bin *.hex *.srec *.s *.o *.pdf *core
