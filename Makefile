include Makefile.local

TOOLCHAIN_PREFIX := xtensa-lx106-elf-
CC := $(TOOLCHAIN_PREFIX)gcc
AR := $(TOOLCHAIN_PREFIX)ar
LD := $(TOOLCHAIN_PREFIX)gcc
OBJCOPY := $(TOOLCHAIN_PREFIX)objcopy


XTENSA_LIBS ?= $(shell $(CC) -print-sysroot)


OBJ_FILES := \
	crypto/aes.o \
	crypto/bigint.o \
	crypto/hmac.o \
	crypto/md2.o \
	crypto/md5.o \
	crypto/rc4.o \
	crypto/rsa.o \
	crypto/sha1.o \
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
		-I$(SDK_BASE)/include \
		-Icrypto \
		-Issl

LDFLAGS  += 	-L$(XTENSA_LIBS)/lib \
		-L$(XTENSA_LIBS)/arch/lib \
		-L$(SDK_BASE)/lib

CFLAGS+=-std=c99 -DESP8266

CFLAGS += -Os -g -O2 -Wpointer-arith -Wno-implicit-function-declaration -Wl,-EL -fno-inline-functions -nostdlib -mlongcalls -mno-text-section-literals  -D__ets__ -DICACHE_FLASH
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
	rm -rf $(OBJ_FILES) $(LWIP_AR)


.PHONY: all clean
