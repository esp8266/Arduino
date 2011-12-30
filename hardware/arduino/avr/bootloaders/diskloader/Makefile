###############################################################################
# Makefile for DiskLoader
###############################################################################

## General Flags
PROJECT = DiskLoader
TARGET = DiskLoader.elf
CC = avr-gcc

# BOARD2
MCU = atmega32u4
AVR_FREQ   = 16000000L

# Specify the Arduino model using the assigned PID.  This is used by Descriptors.c
#   to set PID and product descriptor string
# Arduino Leonardo PID
ARDUINO_MODEL_PID = 0x0034
# Arduino Micro PID
#ARDUINO_MODEL_PID = 0x0035

# Change if your programmer is different
AVRDUDE_PROGRAMMER = avrispmkII
AVRDUDE_PORT = usb	   

# program name should not be changed...
PROGRAM    = DiskLoader

AVRDUDE = avrdude
AVRDUDE_FLAGS = -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) -p $(MCU)

## Options common to compile, link and assembly rules
COMMON = -mmcu=$(MCU)

override CFLAGS = -g -Wall -Os -mmcu=$(MCU) -DF_CPU=$(AVR_FREQ) -DARDUINO_MODEL_PID=$(ARDUINO_MODEL_PID) $(DEFS) -ffunction-sections -gdwarf-2 -fdata-sections -fno-split-wide-types

## Assembly specific flags
ASMFLAGS = $(COMMON)
ASMFLAGS += $(CFLAGS)
ASMFLAGS += -x assembler-with-cpp -Wa,-gdwarf2

## Linker flags
LDFLAGS = $(COMMON)
LDFLAGS += -Wl,-gc-sections,-Map=DiskLoader.map,--section-start=.text=0x7800,--relax
LDFLAGS += -nodefaultlibs -nostartfiles


## Intel Hex file production flags
HEX_EEPROM_FLAGS = -j .eeprom
HEX_EEPROM_FLAGS += --set-section-flags=.eeprom="alloc,load"
HEX_EEPROM_FLAGS += --change-section-lma .eeprom=0 --no-change-warnings

## Objects explicitly added by the user
LINKONLYOBJECTS = 

MODULES   := .
SRC_DIR   := $(addprefix src/,$(MODULES))
BUILD_DIR := $(addprefix build/,$(MODULES))

SRC       := $(foreach sdir,$(SRC_DIR),$(wildcard $(sdir)/*.cpp))
OBJ       := $(patsubst src/%.cpp,build/%.o,$(SRC))
DEP		  := $(OBJ:%.o=%.d)
INCLUDES  := $(addprefix -I,$(SRC_DIR))

vpath %.cpp $(SRC_DIR)

.PHONY: all checkdirs clean

all: checkdirs $(TARGET) DiskLoader.hex DiskLoader.lss size

-include $(DEP)

checkdirs: $(BUILD_DIR)

$(BUILD_DIR):
	@mkdir -p $@

clean:
	@rm -rf build/
	@rm -f *.hex
	@rm -f *.elf
	@rm -f *.lss
	@rm -f *.map

define make-goal
$1/%.o: %.cpp
	$(CC) $(INCLUDES) $(CFLAGS) -c $$< -MD -o $$@
endef

$(foreach bdir,$(BUILD_DIR),$(eval $(call make-goal,$(bdir))))

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $(LINKONLYOBJECTS) $(LIBDIRS) $(LIBS) $^ -o $@

%.hex: $(TARGET)
	avr-objcopy -O ihex $(HEX_FLASH_FLAGS)  $< $@
	
%.lss: $(TARGET)
	avr-objdump -h -S $< > $@

size: $(TARGET)
	@echo
#	@avr-size -C --mcu=${MCU} ${TARGET}.elf

program: $(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) -B 5 -u -U flash:w:$(TARGET).hex