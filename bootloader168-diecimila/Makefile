# Makefile for ATmegaBOOT
# E.Lins, 18.7.2005
# $Id$

# program name should not be changed...
PROGRAM    = ATmegaBOOT_168

# enter the target CPU frequency
AVR_FREQ   = 16000000L

# enter the parameters for the avrdude isp tool
ISPTOOL	   = stk500v2
ISPPORT	   = usb
ISPSPEED   = -b 115200

MCU_TARGET = atmega168
LDSECTION  = --section-start=.text=0x3800
ISPFUSES    = avrdude -c $(ISPTOOL) -p m168 -P $(ISPPORT) $(ISPSPEED) -u -U efuse:w:0xf8:m -U hfuse:w:0xd7:m -U lfuse:w:0xaf:m
ISPFLASH    = avrdude -c $(ISPTOOL) -p m168 -P $(ISPPORT) $(ISPSPEED) -V -U flash:w:$(PROGRAM).hex


OBJ        = $(PROGRAM).o
OPTIMIZE   = -O2

DEFS       =
LIBS       =

CC         = avr-gcc


# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) -DF_CPU=$(AVR_FREQ) $(DEFS)
override LDFLAGS       = -Wl,-Map,$(PROGRAM).map,$(LDSECTION)

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

all: $(PROGRAM).elf lst text

isp: $(PROGRAM).hex
	$(ISPFLASH)
#	$(ISPFUSES)

$(PROGRAM).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o *.elf *.lst *.map *.sym *.lss *.eep
	rm -rf $(PROGRAM).hex $(PROGRAM).srec $(PROGRAM).bin

lst:  $(PROGRAM).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

# Rules for building the .text rom images

text: hex bin srec

hex:  $(PROGRAM).hex
bin:  $(PROGRAM).bin
srec: $(PROGRAM).srec

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@
