XTENSA_TOOLCHAIN ?= ../../tools/xtensa-lx106-elf/bin/
ESPTOOL ?= ../../tools/esptool/esptool

BIN_DIR := ./
TARGET_DIR := ./

TARGET_OBJ_FILES := \
	eboot.o \
	eboot_command.o

TARGET_OBJ_PATHS := $(addprefix $(TARGET_DIR)/,$(TARGET_OBJ_FILES))

UZLIB_PATH := ../../tools/sdk/uzlib/src
UZLIB_FLAGS := -DRUNTIME_BITS_TABLES

CC := $(XTENSA_TOOLCHAIN)xtensa-lx106-elf-gcc
CXX := $(XTENSA_TOOLCHAIN)xtensa-lx106-elf-g++
AR := $(XTENSA_TOOLCHAIN)xtensa-lx106-elf-ar
LD := $(XTENSA_TOOLCHAIN)xtensa-lx106-elf-gcc
OBJDUMP := $(XTENSA_TOOLCHAIN)xtensa-lx106-elf-objdump

INC += -I../../tools/sdk/include -I../../tools/sdk/uzlib/src

CFLAGS += -std=gnu99

CFLAGS += -Os -fcommon -g -Wall -Wpointer-arith -Wno-implicit-function-declaration -Wl,-EL -fno-inline-functions -nostdlib -mlongcalls -mno-text-section-literals -ffunction-sections -fdata-sections -free -fipa-pta

CFLAGS += $(INC)

CFLAGS += $(UZLIB_FLAGS)

LDFLAGS	+= -nostdlib -Wl,--no-check-sections -Wl,--gc-sections -umain -Wl,-Map,$(@:.elf=.map)

LD_SCRIPT := -Teboot.ld

APP_OUT := eboot.elf
APP_AR  := eboot.a
APP_FW  := eboot.bin


all: $(APP_OUT)

tinflate.o: $(UZLIB_PATH)/tinflate.c $(UZLIB_PATH)/uzlib.h $(UZLIB_PATH)/uzlib_conf.h
	$(CC) $(CFLAGS) -c -o tinflate.o $(UZLIB_PATH)/tinflate.c

tinfgzip.o: $(UZLIB_PATH)/tinfgzip.c $(UZLIB_PATH)/uzlib.h $(UZLIB_PATH)/uzlib_conf.h
	$(CC) $(CFLAGS) -c -o tinfgzip.o $(UZLIB_PATH)/tinfgzip.c

$(APP_AR): $(TARGET_OBJ_PATHS) tinflate.o tinfgzip.o
	$(AR) cru $@ $^

$(APP_OUT): $(APP_AR) eboot.ld | Makefile
	$(LD) $(LD_SCRIPT) $(LDFLAGS) -Wl,--start-group -Wl,--whole-archive $(APP_AR) -Wl,--end-group -o $@

clean:
	rm -f *.o
	rm -f $(APP_AR)
	rm -f $(APP_OUT)
	rm -f *.map


.PHONY: all clean default
