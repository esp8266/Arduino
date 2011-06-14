# Makefile for ATmegaBOOT
# E.Lins, 18.7.2005
# $Id$
#
# Instructions
#
# To make bootloader .hex file:
# make diecimila
# make lilypad
# make ng
# etc...
#
# To burn bootloader .hex file:
# make diecimila_isp
# make lilypad_isp
# make ng_isp
# etc...

# program name should not be changed...
PROGRAM    = optiboot

# enter the parameters for the avrdude isp tool
ISPTOOL	   = stk500v2
ISPPORT	   = usb
ISPSPEED   = -b 115200

MCU_TARGET = atmega168
LDSECTION  = --section-start=.text=0x3e00

# the efuse should really be 0xf8; since, however, only the lower
# three bits of that byte are used on the atmega168, avrdude gets
# confused if you specify 1's for the higher bits, see:
# http://tinker.it/now/2007/02/24/the-tale-of-avrdude-atmega168-and-extended-bits-fuses/
#
# similarly, the lock bits should be 0xff instead of 0x3f (to
# unlock the bootloader section) and 0xcf instead of 0x0f (to
# lock it), but since the high two bits of the lock byte are
# unused, avrdude would get confused.

ISPFUSES    = avrdude -c $(ISPTOOL) -p $(MCU_TARGET) -P $(ISPPORT) $(ISPSPEED) \
-e -u -U lock:w:0x3f:m -U efuse:w:0x$(EFUSE):m -U hfuse:w:0x$(HFUSE):m -U lfuse:w:0x$(LFUSE):m
ISPFLASH    = avrdude -c $(ISPTOOL) -p $(MCU_TARGET) -P $(ISPPORT) $(ISPSPEED) \
-U flash:w:$(PROGRAM)_$(TARGET).hex -U lock:w:0x0f:m

STK500 = "C:\Program Files\Atmel\AVR Tools\STK500\Stk500.exe"
STK500-1 = $(STK500) -e -d$(MCU_TARGET) -pf -vf -if$(PROGRAM)_$(TARGET).hex \
-lFF -LFF -f$(HFUSE)$(LFUSE) -EF8 -ms -q -cUSB -I200kHz -s -wt
STK500-2 = $(STK500) -d$(MCU_TARGET) -ms -q -lCF -LCF -cUSB -I200kHz -s -wt

OBJ        = $(PROGRAM).o
OPTIMIZE = -Os -fno-inline-small-functions -fno-split-wide-types -mshort-calls

DEFS       = 
LIBS       =

CC         = avr-gcc

# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) -DF_CPU=$(AVR_FREQ) $(DEFS)
override LDFLAGS       = -Wl,$(LDSECTION) -Wl,--relax -nostartfiles

OBJCOPY        = avr-objcopy
OBJDUMP        = avr-objdump

# 20MHz clocked platforms
#
# These are capable of 230400 baud, or 115200 baud on PC (Arduino Avrdude issue)
#

pro20: TARGET = pro_20mhz
pro20: CFLAGS += '-DLED_START_FLASHES=3' '-DBAUD_RATE=115200'
pro20: AVR_FREQ = 20000000L
pro20: $(PROGRAM)_pro_20mhz.hex
pro20: $(PROGRAM)_pro_20mhz.lst

pro20_isp: pro20
pro20_isp: TARGET = pro_20mhz
pro20_isp: HFUSE = DD # 2.7V brownout
pro20_isp: LFUSE = C6 # Full swing xtal (20MHz) 258CK/14CK+4.1ms
pro20_isp: EFUSE = 02 # 512 byte boot
pro20_isp: isp

# 16MHz clocked platforms
#
# These are capable of 230400 baud, or 115200 baud on PC (Arduino Avrdude issue)
#

pro16: TARGET = pro_16MHz
pro16: CFLAGS += '-DLED_START_FLASHES=3' '-DBAUD_RATE=115200'
pro16: AVR_FREQ = 16000000L
pro16: $(PROGRAM)_pro_16MHz.hex
pro16: $(PROGRAM)_pro_16MHz.lst

pro16_isp: pro16
pro16_isp: TARGET = pro_16MHz
pro16_isp: HFUSE = DD # 2.7V brownout
pro16_isp: LFUSE = C6 # Full swing xtal (20MHz) 258CK/14CK+4.1ms
pro16_isp: EFUSE = 02 # 512 byte boot
pro16_isp: isp

# Diecimila and NG use identical bootloaders
#
diecimila: TARGET = diecimila
diecimila: CFLAGS += '-DLED_START_FLASHES=3' '-DBAUD_RATE=115200'
diecimila: AVR_FREQ = 16000000L 
diecimila: $(PROGRAM)_diecimila.hex
diecimila: $(PROGRAM)_diecimila.lst

diecimila_isp: diecimila
diecimila_isp: TARGET = diecimila
diecimila_isp: HFUSE = DD # 2.7V brownout
diecimila_isp: LFUSE = FF # Low power xtal (16MHz) 16KCK/14CK+65ms
diecimila_isp: EFUSE = 02 # 512 byte boot
diecimila_isp: isp

atmega328: TARGET = atmega328
atmega328: MCU_TARGET = atmega328p
atmega328: CFLAGS += '-DLED_START_FLASHES=3' '-DBAUD_RATE=115200'
atmega328: AVR_FREQ = 16000000L
atmega328: LDSECTION  = --section-start=.text=0x7e00
atmega328: $(PROGRAM)_atmega328.hex
atmega328: $(PROGRAM)_atmega328.lst

atmega328_isp: atmega328
atmega328_isp: TARGET = atmega328
atmega328_isp: MCU_TARGET = atmega328p
atmega328_isp: HFUSE = DE # 512 byte boot
atmega328_isp: LFUSE = FF # Low power xtal (16MHz) 16KCK/14CK+65ms
atmega328_isp: EFUSE = 05 # 2.7V brownout
atmega328_isp: isp

# 8MHz clocked platforms
#
# These are capable of 115200 baud
#

lilypad: TARGET = lilypad
lilypad: CFLAGS += '-DLED_START_FLASHES=3' '-DSOFT_UART' '-DBAUD_RATE=115200'
lilypad: AVR_FREQ = 8000000L
lilypad: $(PROGRAM)_lilypad.hex
lilypad: $(PROGRAM)_lilypad.lst

lilypad_isp: lilypad
lilypad_isp: TARGET = lilypad
lilypad_isp: HFUSE = DD # 2.7V brownout
lilypad_isp: LFUSE = E2 # Internal 8MHz osc (8MHz) Slow rising power
lilypad_isp: EFUSE = 02 # 512 byte boot
lilypad_isp: isp

lilypad_resonator: TARGET = lilypad_resonator
lilypad_resonator: CFLAGS += '-DLED_START_FLASHES=3' '-DSOFT_UART' '-DBAUD_RATE=115200'
lilypad_resonator: AVR_FREQ = 8000000L
lilypad_resonator: $(PROGRAM)_lilypad_resonator.hex
lilypad_resonator: $(PROGRAM)_lilypad_resonator.lst

lilypad_resonator_isp: lilypad_resonator
lilypad_resonator_isp: TARGET = lilypad_resonator
lilypad_resonator_isp: HFUSE = DD # 2.7V brownout
lilypad_resonator_isp: LFUSE = C6 # Full swing xtal (20MHz) 258CK/14CK+4.1ms
lilypad_resonator_isp: EFUSE = 02 # 512 byte boot
lilypad_resonator_isp: isp

pro8: TARGET = pro_8MHz
pro8: CFLAGS += '-DLED_START_FLASHES=3' '-DSOFT_UART' '-DBAUD_RATE=115200'
pro8: AVR_FREQ = 8000000L
pro8: $(PROGRAM)_pro_8MHz.hex
pro8: $(PROGRAM)_pro_8MHz.lst

pro8_isp: pro8
pro8_isp: TARGET = pro_8MHz
pro8_isp: HFUSE = DD # 2.7V brownout
pro8_isp: LFUSE = C6 # Full swing xtal (20MHz) 258CK/14CK+4.1ms
pro8_isp: EFUSE = 02 # 512 byte boot
pro8_isp: isp

atmega328_pro8: TARGET = atmega328_pro_8MHz
atmega328_pro8: MCU_TARGET = atmega328p
atmega328_pro8: CFLAGS += '-DLED_START_FLASHES=3' '-DBAUD_RATE=115200'
atmega328_pro8: AVR_FREQ = 8000000L
atmega328_pro8: LDSECTION  = --section-start=.text=0x7e00
atmega328_pro8: $(PROGRAM)_atmega328_pro_8MHz.hex
atmega328_pro8: $(PROGRAM)_atmega328_pro_8MHz.lst

atmega328_pro8_isp: atmega328_pro8
atmega328_pro8_isp: TARGET = atmega328_pro_8MHz
atmega328_pro8_isp: MCU_TARGET = atmega328p
atmega328_pro8_isp: HFUSE = DE # 512 byte boot
atmega328_pro8_isp: LFUSE = FF # Low power xtal (16MHz) 16KCK/14CK+65ms
atmega328_pro8_isp: EFUSE = 05 # 2.7V brownout
atmega328_pro8_isp: isp

# 1MHz clocked platforms
#
# These are capable of 9600 baud
#

luminet: TARGET = luminet
luminet: MCU_TARGET = attiny84
luminet: CFLAGS += '-DLED_START_FLASHES=3' '-DSOFT_UART' '-DBAUD_RATE=9600'
luminet: CFLAGS += '-DVIRTUAL_BOOT_PARTITION'
luminet: AVR_FREQ = 1000000L
luminet: LDSECTION  = --section-start=.text=0x1d00
luminet: $(PROGRAM)_luminet.hex
luminet: $(PROGRAM)_luminet.lst

luminet_isp: luminet
luminet_isp: TARGET = luminet
luminet_isp: MCU_TARGET = attiny84
luminet_isp: HFUSE = DF # Brownout disabled
luminet_isp: LFUSE = 62 # 1MHz internal oscillator, slowly rising power
luminet_isp: EFUSE = FE # Self-programming enable
luminet_isp: isp

isp: $(TARGET)
	$(ISPFUSES)
	$(ISPFLASH)

isp-stk500: $(PROGRAM)_$(TARGET).hex
	$(STK500-1)
	$(STK500-2)

%.elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o *.elf *.lst *.map *.sym *.lss *.eep *.srec *.bin *.hex

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

%.hex: %.elf
	$(OBJCOPY) -j .text -j .data -O ihex $< $@

%.srec: %.elf
	$(OBJCOPY) -j .text -j .data -O srec $< $@

%.bin: %.elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@
