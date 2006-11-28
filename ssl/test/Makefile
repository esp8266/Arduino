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

include ../../config/.config
include ../../config/makefile.conf

ifdef CONFIG_PERFORMANCE_TESTING
all: performance
endif

ifdef CONFIG_SSL_TEST
all: ssltesting
endif

ifndef CONFIG_PLATFORM_WIN32
performance: ../../$(STAGE)/perf_bigint
ssltesting: ../../$(STAGE)/ssltest
LIBS=../../$(STAGE)
CFLAGS += -I../../ssl -I../../config

../../$(STAGE)/perf_bigint: perf_bigint.o $(LIBS)/libaxtls.a
	$(CC) $(LDFLAGS) -o $@ $^ -L $(LIBS) -laxtls

../../$(STAGE)/ssltest: ssltest.o $(LIBS)/libaxtls.a
	$(CC) $(LDFLAGS) -o $@ $^ -lpthread -L $(LIBS) -laxtls
else
performance: ../../$(STAGE)/perf_bigint.exe
ssltesting: ../../$(STAGE)/ssltest.exe
CFLAGS += /I".." /I"../../config"

%.obj : %.c
	$(CC) $(CFLAGS) $< 

OBJLIST=..\aes.obj ..\asn1.obj ..\bigint.obj ..\crypto_misc.obj ..\hmac.obj \
        ..\md5.obj ..\loader.obj ..\p12.obj ..\os_port.obj ..\rc4.obj \
        ..\rsa.obj ..\sha1.obj ..\tls1.obj ..\tls1_clnt.obj ..\tls1_svr.obj

../../$(STAGE)/perf_bigint.exe: perf_bigint.obj $(OBJLIST)
	$(LD) $(LDFLAGS) /out:$@ $^

../../$(STAGE)/ssltest.exe: ssltest.obj $(OBJLIST)
	$(LD) $(LDFLAGS) /out:$@ $^
endif

clean::
	-@rm -f ../../$(STAGE)/perf_bigint* ../../$(STAGE)/ssltest*

include ../../config/makefile.post
