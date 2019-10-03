XTENSA_TOOLCHAIN ?= ../../tools/xtensa-lx106-elf/bin/
ESPTOOL ?= ../../tools/esptool/esptool

BIN_DIR := ./
TARGET_DIR := ./

TARGET_OBJ_FILES := \
	eboot.o \
	eboot_command.o \


TARGET_OBJ_PATHS := $(addprefix $(TARGET_DIR)/,$(TARGET_OBJ_FILES))

CC := $(XTENSA_TOOLCHAIN)xtensa-lx106-elf-gcc
CXX := $(XTENSA_TOOLCHAIN)xtensa-lx106-elf-g++
AR := $(XTENSA_TOOLCHAIN)xtensa-lx106-elf-ar
LD := $(XTENSA_TOOLCHAIN)xtensa-lx106-elf-gcc
OBJDUMP := $(XTENSA_TOOLCHAIN)xtensa-lx106-elf-objdump

INC += -I../../tools/sdk/include
CFLAGS += -std=gnu99 -DEBOOT_ENABLE_FLASH_STORAGE #-DEBOOT_ENABLE_SERIAL_DEBUG

CFLAGS += -O0 -g -Wpointer-arith -Wno-implicit-function-declaration -Wl,-EL -fno-inline-functions -nostdlib -mlongcalls -mno-text-section-literals

CFLAGS += $(INC)

LDFLAGS	+= -nostdlib -Wl,--no-check-sections -umain

LD_SCRIPT := -Teboot.ld

APP_OUT:= eboot.elf
APP_AR := eboot.a
APP_FW := eboot.bin

all: $(APP_FW)

$(APP_AR): $(TARGET_OBJ_PATHS)
	$(AR) cru $@ $^


$(APP_OUT): $(APP_AR)
	$(LD) $(LD_SCRIPT) $(LDFLAGS) -Wl,--start-group -Wl,--whole-archive $(APP_AR) -Wl,--end-group -o $@

$(APP_FW): $(APP_OUT)
	$(ESPTOOL) -vvv -eo $(APP_OUT) -bo $@ -bs .text -bs .data -bs .rodata -bc -ec || true


clean:
	rm -f *.o
	rm -f $(APP_AR)
	rm -f $(APP_OUT)


.PHONY: all clean default
