#
#  Copyright (c) 2011 Arduino.  All right reserved.
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#  See the GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#

# Makefile for compiling libArduino
.SUFFIXES: .o .a .c .s

# putting default variant
ifeq ("$(VARIANT)", "")
#VARIANT=sam3s_ek
#VARIANT=sam3u_ek
VARIANT=arduino_due_x
endif

ifeq ("$(VARIANT)", "sam3s_ek")
CHIP=__SAM3S4C__
VARIANT_PATH = ../../../../atmel/sam/variants/$(VARIANT)
else ifeq ("$(VARIANT)", "sam3u_ek")
CHIP=__SAM3U4E__
VARIANT_PATH = ../../../../atmel/sam/variants/$(VARIANT)
else ifeq ("$(VARIANT)", "sam3x_ek")
CHIP=__SAM3X8H__
VARIANT_PATH = ../../../../atmel/sam/variants/$(VARIANT)
else ifeq ("$(VARIANT)", "arduino_due_u")
CHIP=__SAM3U4E__
VARIANT_PATH = ../../../../variants/$(VARIANT)
else ifeq ("$(VARIANT)", "arduino_due_x")
CHIP=__SAM3X8E__
VARIANT_PATH = ../../../../variants/$(VARIANT)
endif

TOOLCHAIN=gcc

#-------------------------------------------------------------------------------
# Path
#-------------------------------------------------------------------------------

# Libraries
PROJECT_BASE_PATH = ./..
SYSTEM_PATH = ../../../../system

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
else ifeq ($(CHIP), __SAM3X8E__)
CHIP_NAME=sam3x8e
CHIP_SERIE=sam3xa
else
endif

CMSIS_ROOT_PATH = $(SYSTEM_PATH)/CMSIS
CMSIS_ARM_PATH=$(CMSIS_ROOT_PATH)/CMSIS/Include
CMSIS_ATMEL_PATH=$(CMSIS_ROOT_PATH)/Device/ATMEL
CMSIS_CHIP_PATH=$(CMSIS_ROOT_PATH)/Device/ATMEL/$(CHIP_SERIE)

ARDUINO_CORE_PATH=$(PROJECT_BASE_PATH)/..
ARDUINO_USB_PATH=$(PROJECT_BASE_PATH)/../USB
ARDUINO_USB_HOST_PATH=$(PROJECT_BASE_PATH)/../../../system/USBHost

# Output directories
OUTPUT_PATH = debug_$(VARIANT)

#-------------------------------------------------------------------------------
# Files
#-------------------------------------------------------------------------------

vpath %.h $(PROJECT_BASE_PATH)/.. $(VARIANT_PATH) $(SYSTEM_PATH) $(CMSIS_ARM_PATH)
vpath %.cpp $(PROJECT_BASE_PATH)

VPATH+=$(PROJECT_BASE_PATH)

INCLUDES = -I$(PROJECT_BASE_PATH)/..
INCLUDES += -I$(VARIANT_PATH)
#INCLUDES += -I$(VARIANT_PATH)/..
#INCLUDES += -I$(SYSTEM_PATH)
INCLUDES += -I$(SYSTEM_PATH)/libsam
INCLUDES += -I$(CMSIS_ARM_PATH)
INCLUDES += -I$(CMSIS_ATMEL_PATH)
INCLUDES += -I$(CMSIS_CHIP_PATH)
INCLUDES += -I$(ARDUINO_USB_PATH)
INCLUDES += -I$(ARDUINO_USB_HOST_PATH)

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
LIBS_POSTFIX=dbg
else
OUTPUT_OBJ=release
LIBS_POSTFIX=rel
endif

OUTPUT_BIN=test_$(TOOLCHAIN)_$(LIBS_POSTFIX)
LIBS=-Wl,--start-group -lgcc -lc -lstdc++ -lsam_$(CHIP_NAME)_$(TOOLCHAIN)_$(LIBS_POSTFIX) -larduino_$(VARIANT)_$(TOOLCHAIN)_$(LIBS_POSTFIX) -lvariant_$(VARIANT)_$(TOOLCHAIN)_$(LIBS_POSTFIX) -Wl,--end-group

LIB_PATH =-L$(PROJECT_BASE_PATH)/..
LIB_PATH+=-L=/lib/thumb2
#LIB_PATH+=-L=/../lib/gcc/arm-none-eabi/4.5.2/thumb2

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

test: create_output libsam_$(CHIP_NAME)_$(TOOLCHAIN)_$(LIBS_POSTFIX).a libarduino_$(VARIANT)_$(TOOLCHAIN)_$(LIBS_POSTFIX).a libvariant_$(VARIANT)_$(TOOLCHAIN)_$(LIBS_POSTFIX).a $(OUTPUT_BIN)


.PHONY: create_output
create_output:
	@echo --- Preparing $(VARIANT) files in $(OUTPUT_PATH) $(OUTPUT_BIN)
#	@echo -------------------------
#	@echo *$(INCLUDES)
#	@echo -------------------------
#	@echo *$(C_SRC)
#	@echo -------------------------
#	@echo *$(C_OBJ)
#	@echo -------------------------
#	@echo *$(addprefix $(OUTPUT_PATH)/, $(C_OBJ))
#	@echo -------------------------
#	@echo *$(CPP_SRC)
#	@echo -------------------------
#	@echo *$(CPP_OBJ)
#	@echo -------------------------
#	@echo *$(addprefix $(OUTPUT_PATH)/, $(CPP_OBJ))
#	@echo -------------------------
#	@echo *$(A_SRC)
#	@echo -------------------------

	-@mkdir $(OUTPUT_PATH) 1>NUL 2>&1

$(addprefix $(OUTPUT_PATH)/,$(CPP_OBJ)): $(OUTPUT_PATH)/%.o: %.cpp
#	@"$(CC)" -c $(CPPFLAGS) $< -o $@
	@"$(CXX)" -c $(CPPFLAGS) $< -o $@
#	@"$(CXX)" -v -c $(CPPFLAGS) $< -o $@

$(OUTPUT_BIN): $(addprefix $(OUTPUT_PATH)/, $(C_OBJ)) $(addprefix $(OUTPUT_PATH)/, $(CPP_OBJ)) $(addprefix $(OUTPUT_PATH)/, $(A_OBJ))
	@"$(CC)" $(LIB_PATH) $(LDFLAGS) -T"$(VARIANT_PATH)/linker_scripts/gcc/flash.ld" -Wl,-Map,$(OUTPUT_PATH)/$@.map -o $(OUTPUT_PATH)/$@.elf $^ $(LIBS)
#	@"$(CC)" $(LIB_PATH) $(LDFLAGS) -T"$(VARIANT_PATH)/linker_scripts/gcc/sram.ld" -Wl,-Map,$(OUTPUT_PATH)/$@.map -o $(OUTPUT_PATH)/$@.elf $^ $(LIBS)
	@"$(NM)" $(OUTPUT_PATH)/$@.elf >$(OUTPUT_PATH)/$@.elf.txt
	@"$(OBJCOPY)" -O binary $(OUTPUT_PATH)/$@.elf $(OUTPUT_PATH)/$@.bin
	$(SIZE) $^ $(OUTPUT_PATH)/$@.elf

.PHONY: clean
clean:
	@echo --- Cleaning test files
	-@$(RM) $(OUTPUT_PATH) 1>NUL 2>&1

#	-$(RM) $(OUTPUT_PATH)/test.o
#	-$(RM) $(OUTPUT_PATH)/$(OUTPUT_BIN).elf
#	-$(RM) $(OUTPUT_PATH)/$(OUTPUT_BIN).elf.txt
#	-$(RM) $(OUTPUT_PATH)/$(OUTPUT_BIN).bin
#	-$(RM) $(OUTPUT_PATH)/$(OUTPUT_BIN).map

debug: test
	@"$(GDB)" -x "$(VARIANT_PATH)/debug_scripts/gcc/$(VARIANT)_flash.gdb" -ex "reset" -readnow -se $(OUTPUT_PATH)/$(OUTPUT_BIN).elf
#	@"$(GDB)" -w -x "$(VARIANT_PATH)/debug_scripts/gcc/$(VARIANT)_sram.gdb" -ex "reset" -readnow -se $(OUTPUT_PATH)/$(OUTPUT_BIN).elf

libsam_$(CHIP_NAME)_$(TOOLCHAIN)_$(LIBS_POSTFIX).a:
	@echo Building $@
	@$(MAKE) -C $(SYSTEM_PATH)/libsam/build_gcc -f Makefile $@

libarduino_$(VARIANT)_$(TOOLCHAIN)_$(LIBS_POSTFIX).a:
	@echo Building $@
	$(MAKE) -C $(ARDUINO_CORE_PATH)/build_gcc -f Makefile $(VARIANT)

libvariant_$(VARIANT)_$(TOOLCHAIN)_$(LIBS_POSTFIX).a:
	@echo Building $@
	$(MAKE) -C $(VARIANT_PATH)/build_gcc -f Makefile $(VARIANT)

