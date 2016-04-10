TOOLS_PATH = $(abspath ../../../tools)
BUILD_PATH = $(abspath build)
LWIP_INCLUDE = -Ibuild -I$(TOOLS_PATH)/sdk/include -I$(TOOLS_PATH)/sdk/lwip
LWIP_SRCS = $(patsubst %.c,$(BUILD_PATH)/%.o,$(wildcard */*.c)) $(patsubst %.c,$(BUILD_PATH)/%.o,$(wildcard */*/*.c))
LWIP_LIB = $(abspath liblwip_gcc.a)

BUILD_FLAGS = -c -Os -g -Wpointer-arith -Wno-implicit-function-declaration -Wl,-EL -fno-inline-functions -nostdlib -mlongcalls -mtext-section-literals -falign-functions=4 -MMD -std=gnu99 -ffunction-sections -fdata-sections
BUILD_DEFINES = -D__ets__ -DICACHE_FLASH -U__STRICT_ANSI__ -DLWIP_OPEN_SRC

CC=$(TOOLS_PATH)/xtensa-lx106-elf/bin/xtensa-lx106-elf-gcc
AR=$(TOOLS_PATH)/xtensa-lx106-elf/bin/xtensa-lx106-elf-ar

$(BUILD_PATH)/%.h:
	@echo "[CR]" $(notdir $@)
	@mkdir -p $(dir $@)
	@touch $@

$(BUILD_PATH)/%.o: %.c
	@echo "[CC]" $(notdir $@)
	@mkdir -p $(dir $@)
	@$(CC) $(BUILD_FLAGS) $(BUILD_DEFINES) $(LWIP_INCLUDE) $< -o $@

$(LWIP_LIB): $(BUILD_PATH)/user_config.h $(LWIP_SRCS)
	@echo "[AR]" $(notdir $(LWIP_LIB))
	@$(AR) cru $(LWIP_LIB) $(LWIP_SRCS)

all: $(LWIP_LIB)

install: all
	@echo Installing $(notdir $(LWIP_LIB)) to $(TOOLS_PATH)/sdk/lib
	@cp -f $(LWIP_LIB) $(TOOLS_PATH)/sdk/lib/$(notdir $(LWIP_LIB))

deploy: install clean

clean:
	@rm -rf $(BUILD_PATH) $(LWIP_LIB)
