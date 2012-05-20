#
#  Copyright (c) 2012 Arduino.  All right reserved.
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

CHIP=__SAM3X8H__
VARIANT=sam3x_ek
LIBNAME=libarduino_$(VARIANT)
TOOLCHAIN=gcc

#-------------------------------------------------------------------------------
# Path
#-------------------------------------------------------------------------------

# Output directories
OUTPUT_BIN = ..

# Libraries
PROJECT_BASE_PATH = ..
PROJECT_BASE_PATH_USB = ../USB
SYSTEM_PATH = ../../../system
CMSIS_ROOT_PATH = $(SYSTEM_PATH)/CMSIS
CMSIS_ARM_PATH=$(CMSIS_ROOT_PATH)/CMSIS/Include
CMSIS_ATMEL_PATH=$(CMSIS_ROOT_PATH)/Device/ATMEL
CMSIS_CHIP_PATH=$(CMSIS_ROOT_PATH)/Device/ATMEL/$(CHIP_SERIE)
VARIANT_PATH = ../../../../../atmel/sam/variants/$(VARIANT)

#-------------------------------------------------------------------------------
# Files
#-------------------------------------------------------------------------------

vpath %.h $(PROJECT_BASE_PATH) $(PROJECT_BASE_PATH_USB) $(SYSTEM_PATH) $(VARIANT_PATH)
vpath %.c $(PROJECT_BASE_PATH) $(PROJECT_BASE_PATH_USB) $(VARIANT_PATH)
vpath %.cpp $(PROJECT_BASE_PATH) $(PROJECT_BASE_PATH_USB)

VPATH+=$(PROJECT_BASE_PATH)

INCLUDES =
INCLUDES += -I$(PROJECT_BASE_PATH)
INCLUDES += -I$(PROJECT_BASE_PATH_USB)
INCLUDES += -I$(VARIANT_PATH)
INCLUDES += -I$(CMSIS_ARM_PATH)
INCLUDES += -I$(CMSIS_ATMEL_PATH)
INCLUDES += -I$(SYSTEM_PATH)
INCLUDES += -I$(SYSTEM_PATH)/libsam

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

CFLAGS += -DUSB_VID=0x2341 -DUSB_PID=0xcafe
CPPFLAGS += -DUSB_VID=0x2341 -DUSB_PID=0xcafe

#-------------------------------------------------------------------------------
ifdef DEBUG
OUTPUT_OBJ=debug
OUTPUT_LIB=$(LIBNAME)_$(TOOLCHAIN)_dbg.a
else
OUTPUT_OBJ=release
OUTPUT_LIB=$(LIBNAME)_$(TOOLCHAIN)_rel.a
endif

OUTPUT_PATH=$(OUTPUT_OBJ)_$(VARIANT)

#-------------------------------------------------------------------------------
# C source files and objects
#-------------------------------------------------------------------------------
C_SRC=$(wildcard $(PROJECT_BASE_PATH)/*.c $(PROJECT_BASE_PATH_USB)/*.c)

C_OBJ_TEMP = $(patsubst %.c, %.o, $(notdir $(C_SRC)))

# during development, remove some files
C_OBJ_FILTER=

C_OBJ=$(filter-out $(C_OBJ_FILTER), $(C_OBJ_TEMP))

#-------------------------------------------------------------------------------
# CPP source files and objects
#-------------------------------------------------------------------------------
CPP_SRC=$(wildcard $(PROJECT_BASE_PATH)/*.cpp $(PROJECT_BASE_PATH_USB)/*.cpp)

CPP_OBJ_TEMP = $(patsubst %.cpp, %.o, $(notdir $(CPP_SRC)))

# during development, remove some files
CPP_OBJ_FILTER=

CPP_OBJ=$(filter-out $(CPP_OBJ_FILTER), $(CPP_OBJ_TEMP))

#-------------------------------------------------------------------------------
# Assembler source files and objects
#-------------------------------------------------------------------------------
A_SRC=$(wildcard $(PROJECT_BASE_PATH)/*.s)

A_OBJ_TEMP=$(patsubst %.s, %.o, $(notdir $(A_SRC)))

# during development, remove some files
A_OBJ_FILTER=

A_OBJ=$(filter-out $(A_OBJ_FILTER), $(A_OBJ_TEMP))

#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------
all: $(VARIANT)

$(VARIANT): create_output $(OUTPUT_LIB)

.PHONY: create_output
create_output:
	@echo ------------------------------------------------------------------------------------
	@echo --- Preparing $(VARIANT) files in $(OUTPUT_PATH) $(OUTPUT_BIN)
	@echo ------------------------------------------------------------------------------------
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
	@echo ------------------------------------------------------------------------------------

$(addprefix $(OUTPUT_PATH)/,$(C_OBJ)): $(OUTPUT_PATH)/%.o: %.c
#	"$(CC)" -v -c $(CFLAGS) $< -o $@
	@"$(CC)" -c $(CFLAGS) $< -o $@

#$(addprefix $(OUTPUT_PATH)/,$(CPP_OBJ)): $(OUTPUT_PATH)/%.d: %.o
#	"$(CC)" -M -MF $@.d -c $(CPPFLAGS) $<

$(addprefix $(OUTPUT_PATH)/,$(CPP_OBJ)): $(OUTPUT_PATH)/%.o: %.cpp
#	"$(CC)" -xc++ -c $(CPPFLAGS) $< -o $@
	@"$(CC)" -xc++ -c $(CPPFLAGS) $< -o $@

$(addprefix $(OUTPUT_PATH)/,$(A_OBJ)): $(OUTPUT_PATH)/%.o: %.s
	@"$(AS)" -c $(ASFLAGS) $< -o $@

$(OUTPUT_LIB): $(addprefix $(OUTPUT_PATH)/, $(C_OBJ)) $(addprefix $(OUTPUT_PATH)/, $(CPP_OBJ)) $(addprefix $(OUTPUT_PATH)/, $(A_OBJ))
	@echo ------------------------------------------------------------------------------------
	@"$(AR)" -r "$(OUTPUT_BIN)/$@" $^
	@"$(NM)" "$(OUTPUT_BIN)/$@" > "$(OUTPUT_BIN)/$@.txt"
	@echo ------------------------------------------------------------------------------------


.PHONY: clean
clean:
	@echo --- Cleaning $(VARIANT) files [$(OUTPUT_PATH)$(SEP)*.o]
	-@$(RM) $(OUTPUT_PATH) 1>NUL 2>&1
	-@$(RM) $(OUTPUT_BIN)/$(OUTPUT_LIB) 1>NUL 2>&1
