#
#  Copyright(C) 2007 Cameron Rich
#
#  This library is free software; you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

AXTLS_HOME=..

include $(AXTLS_HOME)/config/.config
include $(AXTLS_HOME)/config/makefile.conf

all: libs
ifdef CONFIG_PERFORMANCE_TESTING
	$(MAKE) -C test
else
ifdef CONFIG_SSL_TEST
	$(MAKE) -C test
endif
endif

ifndef CONFIG_PLATFORM_WIN32
TARGET1=$(AXTLS_HOME)/$(STAGE)/libaxtls.a
BASETARGET=libaxtls.so
CRYPTO_PATH=$(AXTLS_HOME)/crypto/
ifdef CONFIG_PLATFORM_CYGWIN
TARGET2=$(AXTLS_HOME)/$(STAGE)/libaxtls.dll.a
else
TARGET2=$(AXTLS_HOME)/$(STAGE)/$(LIBMINOR)
endif

# shared library major/minor numbers
LIBMAJOR=$(BASETARGET).1
LIBMINOR=$(BASETARGET).1.2
else
TARGET1=$(AXTLS_HOME)/axtls.lib
TARGET2=$(AXTLS_HOME)/$(STAGE)/axtls.dll
STATIC_LIB=$(AXTLS_HOME)/$(STAGE)/axtls.static.lib
CRYPTO_PATH=$(AXTLS_HOME)\\crypto\\
endif

libs: $(TARGET1) $(TARGET2)

CRYPTO_OBJ=\
	$(CRYPTO_PATH)aes.o \
	$(CRYPTO_PATH)hmac.o \
	$(CRYPTO_PATH)md5.o \
	$(CRYPTO_PATH)rc4.o \
	$(CRYPTO_PATH)sha1.o

OBJ=\
	asn1.o \
	x509.o \
	bigint.o \
	crypto_misc.o \
	os_port.o \
	loader.o \
	openssl.o \
	p12.o \
	rsa.o \
	tls1.o \
	tls1_svr.o \
	tls1_clnt.o

include $(AXTLS_HOME)/config/makefile.post

ifndef CONFIG_PLATFORM_WIN32     # Linux/Unix/Cygwin

$(TARGET1) : $(OBJ)
	$(AR) -r $@ $(CRYPTO_OBJ) $(OBJ)

$(TARGET2) : $(OBJ)
ifndef CONFIG_PLATFORM_CYGWIN
	$(LD) $(LDFLAGS) $(LDSHARED) -Wl,-soname,$(LIBMAJOR) -o $(AXTLS_HOME)/$(STAGE)/$(LIBMINOR) $(CRYPTO_OBJ) $(OBJ)
	cd $(AXTLS_HOME)/$(STAGE); ln -sf $(LIBMINOR) $(LIBMAJOR); ln -sf $(LIBMAJOR) $(BASETARGET); cd -
else
	$(LD) $(LDFLAGS) $(LDSHARED) -o $(AXTLS_HOME)/$(STAGE)/cygaxtls.dll \
    -Wl,--out-implib=$(AXTLS_HOME)/$(STAGE)/libaxtls.dll.a \
    -Wl,--export-all-symbols \
    -Wl,--enable-auto-import $(CRYPTO_OBJ) $(OBJ)
endif

else  # Win32
CRYPTO_OBJ:=$(CRYPTO_OBJ:.o=.obj)

$(TARGET1) : $(OBJ)
	$(AR) /out:$@ $(CRYPTO_OBJ) $(OBJ)

$(TARGET2) : $(OBJ)
	cp $(TARGET1) $(STATIC_LIB)
	$(LD) $(LDFLAGS) $(LDSHARED) /out:$@ $(CRYPTO_OBJ) $(OBJ)

endif    

clean::
	$(MAKE) -C test clean
	-@rm -f $(AXTLS_HOME)/$(STAGE)/* *.a $(TARGET1) $(TARGET2)

