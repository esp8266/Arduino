# Makefile for compiling libboard
.SUFFIXES: .o .a .c .s

CHIP=sam3s4
BOARD=sam3s_ek
LIBNAME=libboard
TOOLCHAIN=gcc

#-------------------------------------------------------------------------------
# Path
#-------------------------------------------------------------------------------

# Output directories
OUTPUT_BIN = ../lib

# Libraries
PROJECT_BASE_PATH = ..

#-------------------------------------------------------------------------------
# Files
#-------------------------------------------------------------------------------

vpath %.h $(PROJECT_BASE_PATH)/include
vpath %.c $(PROJECT_BASE_PATH)/source
vpath %.s $(PROJECT_BASE_PATH)/source

VPATH+=$(PROJECT_BASE_PATH)/source

INCLUDES = -I$(PROJECT_BASE_PATH)
INCLUDES += -I$(PROJECT_BASE_PATH)/include
INCLUDES += -I$(PROJECT_BASE_PATH)/../libchip_sam3s

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
OUTPUT_LIB=$(LIBNAME)_$(BOARD)_$(TOOLCHAIN)_dbg.a
else
OUTPUT_OBJ=release
OUTPUT_LIB=$(LIBNAME)_$(BOARD)_$(TOOLCHAIN)_rel.a
endif

OUTPUT_PATH=$(OUTPUT_OBJ)_$(BOARD)

#-------------------------------------------------------------------------------
# C source files and objects
#-------------------------------------------------------------------------------
C_SRC=$(wildcard $(PROJECT_BASE_PATH)/source/*.c)

C_OBJ_TEMP = $(patsubst %.c, %.o, $(notdir $(C_SRC)))

# during development, remove some files
C_OBJ_FILTER=

C_OBJ=$(filter-out $(C_OBJ_FILTER), $(C_OBJ_TEMP))

#-------------------------------------------------------------------------------
# Assembler source files and objects
#-------------------------------------------------------------------------------
A_SRC=$(wildcard $(PROJECT_BASE_PATH)/source/*.s)

A_OBJ_TEMP=$(patsubst %.s, %.o, $(notdir $(A_SRC)))

# during development, remove some files
A_OBJ_FILTER=

A_OBJ=$(filter-out $(A_OBJ_FILTER), $(A_OBJ_TEMP))

#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------
all: $(BOARD)

$(BOARD): create_output $(OUTPUT_LIB)

.PHONY: create_output
create_output:
	@echo --- Preparing $(BOARD) files $(OUTPUT_PATH) $(OUTPUT_BIN) 
#	@echo -------------------------
#	@echo *$(C_SRC)
#	@echo -------------------------
#	@echo *$(C_OBJ)
#	@echo -------------------------
#	@echo *$(addprefix $(OUTPUT_PATH)/, $(C_OBJ))
#	@echo -------------------------
#	@echo *$(A_SRC)
#	@echo -------------------------

	-@mkdir $(subst /,$(SEP),$(OUTPUT_BIN)) 1>NUL 2>&1
	-@mkdir $(OUTPUT_PATH) 1>NUL 2>&1

$(addprefix $(OUTPUT_PATH)/,$(C_OBJ)): $(OUTPUT_PATH)/%.o: %.c
	@$(CC) -c $(CFLAGS) $< -o $@

$(addprefix $(OUTPUT_PATH)/,$(A_OBJ)): $(OUTPUT_PATH)/%.o: %.s
	@$(AS) -c $(ASFLAGS) $< -o $@

$(OUTPUT_LIB): $(addprefix $(OUTPUT_PATH)/, $(C_OBJ)) $(addprefix $(OUTPUT_PATH)/, $(A_OBJ))
	@$(AR) -r "$(OUTPUT_BIN)/$@" $^
	@$(NM) "$(OUTPUT_BIN)/$@" > "$(OUTPUT_BIN)/$@.txt"

.PHONY: clean
clean:
	@echo --- Cleaning $(BOARD) files [$(OUTPUT_PATH)$(SEP)*.o]
	-@$(RM) $(OUTPUT_PATH) 1>NUL 2>&1
	-@$(RM) $(OUTPUT_BIN)/$(OUTPUT_LIB) 1>NUL 2>&1
	-@$(RM) $(OUTPUT_BIN)/$(OUTPUT_LIB).txt 1>NUL 2>&1

# dependencies
$(addprefix $(OUTPUT_PATH)/,$(C_OBJ)): $(OUTPUT_PATH)/%.o: $(PROJECT_BASE_PATH)/board.h $(wildcard $(PROJECT_BASE_PATH)/include/*.h)
