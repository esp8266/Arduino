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

# the efuse should really be 0xf8; since, however, only the lower
# three bits of that byte are used on the atmega168, avrdude gets
# confused if you specify 1's for the higher bits, see:
# http://tinker.it/now/2007/02/24/the-tale-of-avrdude-atmega168-and-extended-bits-fuses/
#
# similarly, the lock bits should be 0xff instead of 0x3f (to
# unlock the bootloader section) and 0xcf instead of 0x0f (to
# lock it), but since the high two bits of the lock byte are
# unused, avrdude would get confused.
ISPFUSES    = avrdude -c $(ISPTOOL) -p m168 -P $(ISPPORT) $(ISPSPEED) -e -u -U lock:w:0x3f:m -U efuse:w:0x00:m -U hfuse:w:0xdf:m -U lfuse:w:0xff:m
ISPFLASH    = avrdude -c $(ISPTOOL) -p m168 -P $(ISPPORT) $(ISPSPEED) -U flash:w:$(PROGRAM).hex -U lock:w:0x0f:m


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
	$(ISPFUSES)
	$(ISPFLASH)

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
