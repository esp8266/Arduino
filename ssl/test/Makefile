#
#  Copyright(C) 2006 Cameron Rich
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

all:

AXTLS_HOME=../..

include $(AXTLS_HOME)/config/.config
include $(AXTLS_HOME)/config/makefile.conf

ifdef CONFIG_PERFORMANCE_TESTING
all: performance
endif

ifdef CONFIG_SSL_TEST
all: ssltesting
endif

include $(AXTLS_HOME)/config/makefile.post

ifndef CONFIG_PLATFORM_WIN32
performance: $(AXTLS_HOME)/$(STAGE)/perf_bigint
ssltesting: $(AXTLS_HOME)/$(STAGE)/ssltest
LIBS=$(AXTLS_HOME)/$(STAGE)

$(AXTLS_HOME)/$(STAGE)/perf_bigint: perf_bigint.o $(LIBS)/libaxtls.a
	$(CC) $(LDFLAGS) -o $@ $^ -L $(LIBS) -laxtls

$(AXTLS_HOME)/$(STAGE)/ssltest: ssltest.o $(LIBS)/libaxtls.a
	$(CC) $(LDFLAGS) -o $@ $^ -lpthread -L $(LIBS) -laxtls
else
performance: $(AXTLS_HOME)/$(STAGE)/perf_bigint.exe
ssltesting: $(AXTLS_HOME)/$(STAGE)/ssltest.exe

CRYPTO_PATH="$(AXTLS_INCLUDE)crypto\\"
AXTLS_SSL_PATH="$(AXTLS_INCLUDE)ssl\\"

CRYPTO_OBJ=\
	$(CRYPTO_PATH)aes.obj \
	$(CRYPTO_PATH)hmac.obj \
	$(CRYPTO_PATH)md5.obj \
	$(CRYPTO_PATH)rc4.obj \
	$(CRYPTO_PATH)sha1.obj

OBJ=\
	$(AXTLS_SSL_PATH)asn1.obj \
	$(AXTLS_SSL_PATH)x509.obj \
	$(AXTLS_SSL_PATH)bigint.obj \
	$(AXTLS_SSL_PATH)crypto_misc.obj \
	$(AXTLS_SSL_PATH)os_port.obj \
	$(AXTLS_SSL_PATH)loader.obj \
	$(AXTLS_SSL_PATH)openssl.obj \
	$(AXTLS_SSL_PATH)p12.obj \
	$(AXTLS_SSL_PATH)rsa.obj \
	$(AXTLS_SSL_PATH)tls1.obj \
	$(AXTLS_SSL_PATH)tls1_svr.obj \
	$(AXTLS_SSL_PATH)tls1_clnt.obj

$(AXTLS_HOME)/$(STAGE)/perf_bigint.exe: perf_bigint.obj
	$(LD) $(LDFLAGS) /out:$@ $? $(CRYPTO_OBJ) $(OBJ)

$(AXTLS_HOME)/$(STAGE)/ssltest.exe: ssltest.obj
	$(LD) $(LDFLAGS) /out:$@ $? $(CRYPTO_OBJ) $(OBJ)
endif

clean::
	-@rm -f $(AXTLS_HOME)/$(STAGE)/perf_bigint* $(AXTLS_HOME)/$(STAGE)/ssltest*

