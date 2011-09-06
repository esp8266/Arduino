# Makefile for compiling libArduino
.SUFFIXES: .o .a .c .s

CHIP=sam3s4
VARIANT=sam3s_ek
TOOLCHAIN=gcc

#-------------------------------------------------------------------------------
# Path
#-------------------------------------------------------------------------------

# Output directories
OUTPUT_PATH = .

# Libraries
PROJECT_BASE_PATH = .
SYSTEM_PATH = ../../../system
CMSIS_BASE_PATH = $(SYSTEM_PATH)/CMSIS/Include
VARIANT_PATH = ../../../variants/sam3s-ek

ifeq ($(CHIP), __SAM3S4C__)
CHIP_NAME=sam3s4c
CHIP_SERIE=sam3s
else ifeq ($(CHIP), __SAM3U4E__)
CHIP_NAME=sam3u4e
CHIP_SERIE=sam3u
else ifeq ($(CHIP), __SAM3N4C__)
CHIP_NAME=sam3n4c
CHIP_SERIE=sam3n
else ifeq ($(CHIP), __SAM3X8H__)
CHIP_NAME=sam3x8h
CHIP_SERIE=sam3xa
else
endif

CMSIS_CHIP_PATH=$(PROJECT_BASE_PATH)/../cmsis/$(CHIP_SERIE)

#-------------------------------------------------------------------------------
# Files
#-------------------------------------------------------------------------------

vpath %.h $(PROJECT_BASE_PATH)/.. $(VARIANT_PATH) $(SYSTEM_PATH) $(CMSIS_BASE_PATH)
vpath %.cpp $(PROJECT_BASE_PATH)

VPATH+=$(PROJECT_BASE_PATH)

INCLUDES = -I$(PROJECT_BASE_PATH)/..
INCLUDES += -I$(VARIANT_PATH)
INCLUDES += -I$(SYSTEM_PATH)
INCLUDES += -I$(SYSTEM_PATH)/libsam
INCLUDES += -I$(CMSIS_BASE_PATH)

#-------------------------------------------------------------------------------
ifdef DEBUG
include debug.mk
else
include release.mk
endif

#-------------------------------------------------------------------------------
# Tools
#-------------------------------------------------------------------------------

include $(TOOLCHAIN).mk

#-------------------------------------------------------------------------------
ifdef DEBUG
OUTPUT_OBJ=debug
OUTPUT_BIN=test_$(TOOLCHAIN)_dbg
LIBS=-Wl,--start-group -lgcc -lc -lchip_$(CHIP)_$(TOOLCHAIN)_dbg -larduino_$(VARIANT)_$(TOOLCHAIN)_dbg -Wl,--end-group
else
OUTPUT_OBJ=release
OUTPUT_BIN=test_$(TOOLCHAIN)_rel
#LIBS=-L../libchip_$(CHIP)_$(TOOLCHAIN)_rel.a -L../arduino_$(VARIANT)_$(TOOLCHAIN)_rel.a
LIBS=-Wl,--start-group -lgcc -lc -lchip_$(CHIP)_$(TOOLCHAIN)_rel -larduino_$(VARIANT)_$(TOOLCHAIN)_rel -lstdc++ -Wl,--end-group
endif

//OUTPUT_PATH=$(OUTPUT_OBJ)_test.elf

LIB_PATH =-L$(PROJECT_BASE_PATH)/..
LIB_PATH+=-L=/lib/thumb2
LIB_PATH+=-L=/../lib/gcc/arm-none-eabi/4.4.1/thumb2

LDFLAGS= -mcpu=cortex-m3 -mthumb -Wl,--cref -Wl,--check-sections -Wl,--gc-sections -Wl,--entry=Reset_Handler -Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align -Wl,--warn-unresolved-symbols

#-------------------------------------------------------------------------------
# CPP source files and objects
#-------------------------------------------------------------------------------
CPP_SRC=$(wildcard $(PROJECT_BASE_PATH)/*.cpp)

CPP_OBJ_TEMP = $(patsubst %.cpp, %.o, $(notdir $(CPP_SRC)))

# during development, remove some files
CPP_OBJ_FILTER=

CPP_OBJ=$(filter-out $(CPP_OBJ_FILTER), $(CPP_OBJ_TEMP))

#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------
all: test

test: $(OUTPUT_BIN)

$(addprefix $(OUTPUT_PATH)/,$(CPP_OBJ)): $(OUTPUT_PATH)/%.o: %.cpp
#	@$(CC) -c $(CPPFLAGS) $< -o $@
	@$(CXX) -c $(CPPFLAGS) $< -o $@

$(OUTPUT_BIN): $(addprefix $(OUTPUT_PATH)/, $(C_OBJ)) $(addprefix $(OUTPUT_PATH)/, $(CPP_OBJ)) $(addprefix $(OUTPUT_PATH)/, $(A_OBJ))
	$(CC) $(LIB_PATH) $(LDFLAGS) -T"$(VARIANT_PATH)/linker_scripts/gcc/flash.ld" -Wl,-Map,$(OUTPUT_PATH)/$@.map -o $(OUTPUT_PATH)/$@.elf $^ $(LIBS)
	$(NM) $(OUTPUT_PATH)/$@.elf >$(OUTPUT_PATH)/$@.elf.txt
	$(OBJCOPY) -O binary $(OUTPUT_PATH)/$@.elf $(OUTPUT_PATH)/$@.bin
	$(SIZE) $^ $(OUTPUT_PATH)/$@.elf


.PHONY: clean
clean:
	@echo --- Cleaning test files
	-@$(RM) $(OUTPUT_PATH)/test.o 1>NUL 2>&1
	-@$(RM) $(OUTPUT_PATH)/$(OUTPUT_BIN).elf 1>NUL 2>&1
	-@$(RM) $(OUTPUT_PATH)/$(OUTPUT_BIN).elf.txt 1>NUL 2>&1
	-@$(RM) $(OUTPUT_PATH)/$(OUTPUT_BIN).bin 1>NUL 2>&1
	-@$(RM) $(OUTPUT_PATH)/$(OUTPUT_BIN).map 1>NUL 2>&1

debug: test
	$(GDB) -x "$(VARIANT_PATH)/debug_scripts/gcc/flash.gdb" -ex "reset" -readnow -se $(OUTPUT_PATH)/$(OUTPUT_BIN).elf
