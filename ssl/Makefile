#
#  Copyright(C) 2006
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
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

include ../config/.config
include ../config/makefile.conf

all: libs
ifdef CONFIG_PERFORMANCE_TESTING
	$(MAKE) -C test
else
ifdef CONFIG_SSL_TEST
	$(MAKE) -C test
endif
endif

ifndef CONFIG_PLATFORM_WIN32
TARGET1=../libaxtls.a
TARGET2=../libaxtls.so
else
TARGET1=../axtls.lib
TARGET2=../axtls.dll
STATIC_LIB=../axtls.static.lib
endif

libs: $(TARGET1) $(TARGET2)

OBJ=\
    aes.o \
	asn1.o \
	bigint.o \
	crypto_misc.o \
	hmac.o \
	loader.o \
	md5.o \
	p12.o \
	rsa.o \
	rc4.o \
	sha1.o \
	tls1.o \
	tls1_svr.o \
	tls1_clnt.o

ifdef CONFIG_PLATFORM_WIN32
OBJ+=os_port.o
endif

include ../config/makefile.post

ifndef CONFIG_PLATFORM_WIN32

$(TARGET1) : $(OBJ)
	$(AR) -r $@ $(OBJ)

$(TARGET2) : $(OBJ)
	$(LD) $(LDFLAGS) $(LDSHARED) -o $@ $(OBJ)

else  # Win32

$(TARGET1) : $(OBJ)
	$(AR) /out:$@ $(OBJ)

$(TARGET2) : $(OBJ)
	cp $(TARGET1) $(STATIC_LIB)
	$(LD) $(LDFLAGS) $(LDSHARED) /out:$@ $(OBJ)

endif    

clean::
	$(MAKE) -C test clean
	-@rm -f *.pch ../*.so ../*.a ../*.dll ../*.lib ../*.exp ../*.pdb ../*.ilk

