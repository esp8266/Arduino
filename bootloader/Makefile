# Makefile for ATmegaBOOT
# E.Lins, 2004-10-14

# program name should not be changed...
PROGRAM    = ATmegaBOOT

# enter the product name for which you want to build the bootloader/monitor
# currently CRUMB8, CRUMB128 and PROBOMEGA128 are supported
PRODUCT    = CRUMB8

# enter the parameters for the UISP isp tool
#ISPPARAMS  = -dprog=stk200 -dlpt=0x378
ISPPARAMS  = -dprog=stk500 -dserial=/dev/com1 -dspeed=115200


############################################################
# You should not have to change anything below here.
############################################################

#DIRAVR = /usr/local/avr
DIRAVRBIN = $(DIRAVR)/bin
DIRAVRUTILS = $(DIRAVR)/utils/bin
DIRINC = .
DIRLIB = $(DIRAVR)/avr/lib




ifeq ($(PRODUCT),CRUMB8)
MCU_TARGET = atmega8
LDSECTION  = --section-start=.text=0x1800
FUSE_L     = 0xdf
FUSE_H     = 0xc8
ISPFUSES   = $(DIRAVRBIN)/uisp -dpart=ATmega8 $(ISPPARAMS) --wr_fuse_l=$(FUSE_L) --wr_fuse_h=$(FUSE_H)
ISPFLASH   = $(DIRAVRBIN)/uisp -dpart=ATmega8 $(ISPPARAMS) --erase --upload if=$(PROGRAM).hex -v
endif

ifeq ($(PRODUCT),CRUMB128)
MCU_TARGET = atmega128
LDSECTION  = --section-start=.text=0x1E000
FUSE_L     = 0xdf
FUSE_H     = 0xc8
FUSE_E     = 0xff
ISPFUSES   = ./uisp -dpart=ATmega128 $(ISPPARAMS) --wr_fuse_l=$(FUSE_L) --wr_fuse_h=$(FUSE_H) --wr_fuse_e=$(FUSE_E)
ISPFLASH   = ./uisp -dpart=ATmega128 $(ISPPARAMS) --erase --upload if=$(PROGRAM).hex -v
endif

ifeq ($(PRODUCT),PROBOMEGA128)
MCU_TARGET = atmega128
LDSECTION  = --section-start=.text=0x1E000
FUSE_L     = 0xdf
FUSE_H     = 0xc8
FUSE_E     = 0xff
ISPFUSES   = ./uisp -dpart=ATmega128 $(ISPPARAMS) --wr_fuse_l=$(FUSE_L) --wr_fuse_h=$(FUSE_H) --wr_fuse_e=$(FUSE_E)
ISPFLASH   = ./uisp -dpart=ATmega128 $(ISPPARAMS) --erase --upload if=$(PROGRAM).hex -v
endif


OBJ        = $(PROGRAM).o
OPTIMIZE   = -O3

DEFS       =
LIBS       =

CC             = $(DIRAVRBIN)/avr-gcc


# Override is only needed by avr-lib build system.

override CFLAGS        = -g -Wall $(OPTIMIZE) -mmcu=$(MCU_TARGET) -D$(PRODUCT) $(DEFS)
override LDFLAGS       = -Wl,-Map,$(PROGRAM).map,$(LDSECTION)

OBJCOPY        = $(DIRAVRBIN)/avr-objcopy
OBJDUMP        = $(DIRAVRBIN)/avr-objdump

all: $(PROGRAM).elf lst text

isp: $(PROGRAM).hex
	$(ISPFUSES)
	$(ISPFLASH)

$(PROGRAM).elf: $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -rf *.o *.elf
	rm -rf *.lst *.map

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
