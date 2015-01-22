# ----------------------------------------------------------------------------
#         ATMEL Microcontroller Software Support
# ----------------------------------------------------------------------------
# Copyright (c) 2010, Atmel Corporation
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following condition is met:
#
# - Redistributions of source code must retain the above copyright notice,
# this list of conditions and the disclaimer below.
#
# Atmel's name may not be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
# DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# ----------------------------------------------------------------------------

#-------------------------------------------------------------------------------
#        User-modifiable options
#-------------------------------------------------------------------------------

ifeq ('$(CHIP)','')
$(error CHIP not defined)
endif

include sam_series.mk

# fill the needed variables
ifeq ($(CHIP),$(findstring $(CHIP), $(SAM3N)))

BOARD:=SAM3N_EK
SERIES:=sam3n

else ifeq ($(CHIP),$(findstring $(CHIP), $(SAM3S)))

BOARD:=SAM3S_EK
SERIES:=sam3s

else ifeq ($(CHIP),$(findstring $(CHIP), $(SAM3SD8)))

BOARD:=SAM3S_EK2
SERIES:=sam3sd8

else ifeq ($(CHIP),$(findstring $(CHIP), $(SAM3U)))

BOARD:=SAM3U_EK
SERIES:=sam3u

else ifeq ($(CHIP),$(findstring $(CHIP), $(SAM3XA)))

BOARD:=SAM3X_EK
SERIES:=sam3xa

else ifeq ($(CHIP),$(findstring $(CHIP), $(SAM4S)))

BOARD:=SAM4S_EK
SERIES:=sam4s

endif


# Defines which are the available memory targets for the device.
MEMORIES = sram flash

# Optimization level, put in comment for debugging
OPTIMIZATION = -Os

# Output directories
BIN = $(CHIP)_bin
OBJ = $(CHIP)_obj

# Output file basename
OUTPUT_BIN = $(BIN)/cmsis_example_$(BOARD)_$(CHIP)

# GCC toolchain provider
GCC_TOOLCHAIN=gcc_atmel

#-------------------------------------------------------------------------------
#		Tools
#-------------------------------------------------------------------------------

# Toolchain prefix when cross-compiling
CROSS_COMPILE = arm-none-eabi-

CMSIS_ROOT=../../..
CMSIS_PATH=$(CMSIS_ROOT)/CMSIS/Include
SAM_PATH=$(CMSIS_ROOT)/Device/ATMEL
DEVICE_PATH=$(SAM_PATH)/$(SERIES)/source

LIBS = -Wl,--start-group -lgcc -lc -Wl,--end-group

LIB_PATH+=-L=/lib/thumb2
LIB_PATH+=-L"$(realpath $(DEVICE_PATH)/$(GCC_TOOLCHAIN))"

# Compilation tools
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
SIZE = $(CROSS_COMPILE)size
STRIP = $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy
GDB = $(CROSS_COMPILE)gdb
NM = $(CROSS_COMPILE)nm
RM = rm

# Flags
INCLUDES := -I"$(CMSIS_PATH)"
INCLUDES += -I"$(SAM_PATH)"
INCLUDES += -I"$(SAM_PATH)/$(SERIES)/include"

CFLAGS += -Wall -Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int
CFLAGS += -Werror-implicit-function-declaration -Wmain -Wparentheses
CFLAGS += -Wsequence-point -Wreturn-type -Wswitch -Wtrigraphs -Wunused
CFLAGS += -Wuninitialized -Wunknown-pragmas -Wfloat-equal -Wundef
CFLAGS += -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings
CFLAGS += -Wsign-compare -Waggregate-return -Wstrict-prototypes
CFLAGS += -Wmissing-prototypes -Wmissing-declarations
CFLAGS += -Wformat -Wmissing-format-attribute -Wno-deprecated-declarations
CFLAGS += -Wpacked -Wredundant-decls -Wnested-externs -Winline -Wlong-long
CFLAGS += -Wunreachable-code
CFLAGS += -Wcast-align
#CFLAGS += -Wmissing-noreturn
#CFLAGS += -Wconversion

# To reduce application size use only integer printf function.
CFLAGS += -Dprintf=iprintf

# CFLAGS += -mlong-calls -Wall
# CFLAGS += -ansi
CFLAGS += -std=c99 
CFLAGS += --param max-inline-insns-single=500 -mcpu=cortex-m3 -mthumb -ffunction-sections
CFLAGS += -g $(OPTIMIZATION) $(INCLUDES) -D__$(CHIP)__ -DBOARD=$(BOARD)
ASFLAGS = -mcpu=cortex-m3 -mthumb -Wall -g $(OPTIMIZATION) $(INCLUDES) -D__$(CHIP)__ -D__ASSEMBLY__
LDFLAGS= -mcpu=cortex-m3 -mthumb -Wl,--cref -Wl,--check-sections -Wl,--gc-sections -Wl,--entry=Reset_Handler -Wl,--defsym=STACK_SIZE=0x200 -Wl,--unresolved-symbols=report-all -Wl,--warn-common -Wl,--warn-section-align -Wl,--warn-unresolved-symbols
#LD_OPTIONAL=-Wl,--print-gc-sections -Wl,--stats

#-------------------------------------------------------------------------------
#		Files
#-------------------------------------------------------------------------------

# Directories where source files can be found

VPATH += ..
VPATH += $(DEVICE_PATH)
VPATH += $(DEVICE_PATH)/$(GCC_TOOLCHAIN)

# Objects built from C source files
C_OBJECTS += main.o
C_OBJECTS += startup_$(SERIES).o
C_OBJECTS += system_$(SERIES).o

# Append OBJ and BIN directories to output filename
OUTPUT := $(BIN)/$(OUTPUT_BIN)

#-------------------------------------------------------------------------------
#		Rules
#-------------------------------------------------------------------------------

all: $(BIN) $(OBJ) $(MEMORIES)

$(BIN) $(OBJ):
	-@mkdir $@

define RULES
C_OBJECTS_$(1) = $(addprefix $(OBJ)/$(1)_, $(C_OBJECTS))
ASM_OBJECTS_$(1) = $(addprefix $(OBJ)/$(1)_, $(ASM_OBJECTS))

$(1): $$(ASM_OBJECTS_$(1)) $$(C_OBJECTS_$(1))
	$(CC) $(LIB_PATH) $(LDFLAGS) $(LD_OPTIONAL) -T"$(realpath $(DEVICE_PATH)/$(GCC_TOOLCHAIN)/$(CHIP)_$$@.ld)" -Wl,-Map,"$(OUTPUT_BIN)_$$@.map" -o "$(OUTPUT_BIN)_$$@.elf" $$^ $(LIBS)
	@$(NM) "$(OUTPUT_BIN)_$$@.elf" >"$(OUTPUT_BIN)_$$@.elf.txt"
	@$(OBJCOPY) -O binary "$(OUTPUT_BIN)_$$@.elf" "$(OUTPUT_BIN)_$$@.bin"
	@$(SIZE) $$^ "$(OUTPUT_BIN)_$$@.elf"

$$(C_OBJECTS_$(1)): $(OBJ)/$(1)_%.o: %.c Makefile $(OBJ) $(BIN)
	@$(CC) $(CFLAGS) -D$(1) -c -o $$@ $$<

$$(ASM_OBJECTS_$(1)): $(OBJ)/$(1)_%.o: %.S Makefile $(OBJ) $(BIN)
	@$(CC) $(ASFLAGS) -D$(1) -c -o $$@ $$<

debug_$(1): $(1)
	$(GDB) -x "$(CHIP)_$(1).gdb" -ex "reset" -readnow -se "$(OUTPUT_BIN)_$(1).elf"
endef

$(foreach MEMORY, $(MEMORIES), $(eval $(call RULES,$(MEMORY))))

clean:
	-$(RM) -fR $(OBJ) $(BIN)
