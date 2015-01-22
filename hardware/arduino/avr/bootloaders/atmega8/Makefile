# Makefile for ATmegaBOOT
# E.Lins, 2004-10-14

# program name should not be changed...
PROGRAM    = ATmegaBOOT

PRODUCT=atmega8

# enter the parameters for the UISP isp tool
ISPPARAMS  = -dprog=stk500 -dserial=$(SERIAL) -dspeed=115200


#DIRAVR = /usr/local/avr
DIRAVRBIN = $(DIRAVR)/bin
DIRAVRUTILS = $(DIRAVR)/utils/bin
DIRINC = $(DIRAVR)/include
DIRLIB = $(DIRAVR)/avr/lib


MCU_TARGET = atmega8
LDSECTION  = --section-start=.text=0x1c00
FUSE_L     = 0xdf
FUSE_H     = 0xca
ISPFUSES   = $(DIRAVRBIN)/uisp -dpart=ATmega8 $(ISPPARAMS) --wr_fuse_l=$(FUSE_L) --wr_fuse_h=$(FUSE_H)
ISPFLASH   = $(DIRAVRBIN)/uisp -dpart=ATmega8 $(ISPPARAMS) --erase --upload if=$(PROGRAM).hex -v


OBJ        = $(PROGRAM).o
OPTIMIZE   = -Os -funsigned-char -funsigned-bitfields -fno-inline-small-functions

DEFS       = -DF_CPU=16000000 -DBAUD_RATE=19200
LIBS       =

CC         = $(DIRAVRBIN)/avr-gcc


# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) -D$(PRODUCT) $(DEFS) -I$(DIRINC)
override LDFLAGS       = -Wl,-Map,$(PROGRAM).map,$(LDSECTION)

OBJCOPY        = $(DIRAVRBIN)/avr-objcopy
OBJDUMP        = $(DIRAVRBIN)/avr-objdump
SIZE           = $(DIRAVRBIN)/avr-size

all: $(PROGRAM).elf lst text asm size

isp: $(PROGRAM).hex
	$(ISPFUSES)
	$(ISPFLASH)

$(PROGRAM).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.s
	rm -rf *.o *.elf
	rm -rf *.lst *.map

asm: $(PROGRAM).s

%.s: %.c
	$(CC) -S $(CFLAGS) -g1 $^

lst:  $(PROGRAM).lst

%.lst: %.elf
	$(OBJDUMP) -h -S $< > $@

size: $(PROGRAM).hex
	$(SIZE) $^

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
