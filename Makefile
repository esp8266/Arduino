TOOLCHAIN_PREFIX := xtensa-lx106-elf-
CC := $(TOOLCHAIN_PREFIX)gcc
AR := $(TOOLCHAIN_PREFIX)ar
LD := $(TOOLCHAIN_PREFIX)gcc
OBJCOPY := $(TOOLCHAIN_PREFIX)objcopy

MFORCE32 := $(shell $(CC) --help=target | grep mforce-l32)

XTENSA_LIBS ?= $(shell $(CC) -print-sysroot)


OBJ_FILES := \
	crypto/aes.o \
	crypto/bigint.o \
	crypto/hmac.o \
	crypto/md5.o \
	crypto/rc4.o \
	crypto/rsa.o \
	crypto/sha1.o \
	crypto/sha256.o \
	crypto/sha384.o \
	crypto/sha512.o \
	ssl/asn1.o \
	ssl/gen_cert.o \
	ssl/loader.o \
	ssl/os_port.o \
	ssl/p12.o \
	ssl/tls1.o \
	ssl/tls1_clnt.o \
	ssl/tls1_svr.o \
	ssl/x509.o \
	crypto/crypto_misc.o \


CPPFLAGS += -I$(XTENSA_LIBS)/include \
		-Icrypto \
		-Issl \
		-I.

LDFLAGS  += 	-L$(XTENSA_LIBS)/lib \
		-L$(XTENSA_LIBS)/arch/lib \


CFLAGS+=-std=c99 -DESP8266

CFLAGS += -Wall -Os -g -O2 -Wpointer-arith -Wno-implicit-function-declaration -Wl,-EL -fno-inline-functions -nostdlib -mlongcalls -mno-text-section-literals  -D__ets__ -DICACHE_FLASH

ifneq ($(MFORCE32),)
    # Your compiler supports the -mforce-l32 flag which means that 
    # constants can be stored in flash (program) memory instead of SRAM.
    # See: https://www.arduino.cc/en/Reference/PROGMEM
    CFLAGS += -DPROGMEM="__attribute__((aligned(4))) __attribute__((section(\".irom.text\")))" -mforce-l32
endif

BIN_DIR := bin
AXTLS_AR := $(BIN_DIR)/libaxtls.a

all: $(AXTLS_AR)

$(AXTLS_AR): | $(BIN_DIR)

$(AXTLS_AR): $(OBJ_FILES)
	for file in $(OBJ_FILES); do \
		$(OBJCOPY) \
		--rename-section .text=.irom0.text \
		--rename-section .literal=.irom0.literal \
		$$file; \
	done
	$(AR) cru $@ $^

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

clean:
	rm -rf $(OBJ_FILES) $(AXTLS_AR)


.PHONY: all clean
