TOOLCHAIN_DIR ?= ../../../xtensa-lx106-elf
TOOLS_PATH ?= $(TOOLCHAIN_DIR)/bin/xtensa-lx106-elf-
LWIP_LIB ?= liblwip_src.a
SDK_PATH ?= $(abspath ../../)

BUILD_PATH = build
LWIP_SRCS = $(patsubst %.c,$(BUILD_PATH)/%.o,$(wildcard */*.c)) $(patsubst %.c,$(BUILD_PATH)/%.o,$(wildcard */*/*.c))

LWIP_INCLUDE = -Ibuild -I$(SDK_PATH)/include -I$(SDK_PATH)/lwip/include
BUILD_FLAGS = -c -Os -g -Wpointer-arith -Wno-implicit-function-declaration -Wl,-EL -fno-inline-functions -nostdlib -mlongcalls -mtext-section-literals -falign-functions=4 -MMD -std=gnu99 -ffunction-sections -fdata-sections
BUILD_DEFINES = -D__ets__ -DICACHE_FLASH -U__STRICT_ANSI__ -DLWIP_OPEN_SRC

ifdef WITH_DEBUG_PREFIX_MAP
EXTRA_FLAGS = -fdebug-prefix-map=$(PWD)= -fdebug-prefix-map=$(TOOLCHAIN_DIR)=xtensa-lx106-elf -gno-record-gcc-switches
endif

CC=$(TOOLS_PATH)gcc
AR=$(TOOLS_PATH)ar

$(BUILD_PATH)/%.h:
	@mkdir -p $(dir $@)
	@touch $@

$(BUILD_PATH)/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(BUILD_FLAGS) $(BUILD_DEFINES) $(LWIP_INCLUDE) $(EXTRA_FLAGS) $< -o $@

$(LWIP_LIB): $(BUILD_PATH)/user_config.h $(LWIP_SRCS)
	$(AR) cru $(LWIP_LIB) $(LWIP_SRCS)

all: $(LWIP_LIB)

install: all
	cp -f $(LWIP_LIB) $(SDK_PATH)/lib/$(LWIP_LIB)

release: all
	cp -f $(LWIP_LIB) $(SDK_PATH)/lib/liblwip_gcc.a

clean:
	@rm -rf $(BUILD_PATH) $(LWIP_LIB)
