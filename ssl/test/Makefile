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
performance: ../../perf_bigint
ssltesting: ../../ssltest
LIBS=../../libaxtls.a
CFLAGS += -I../../ssl -I../../config

../../perf_bigint: perf_bigint.o $(LIBS)
	$(CC) $(LDFLAGS) -o $@ $^

../../ssltest: ssltest.o $(LIBS)
	$(CC) $(LDFLAGS) -o $@ -lpthread $^
else
performance: ../../perf_bigint.exe
ssltesting: ../../ssltest.exe
CFLAGS += /I".." /I"../../config"

%.obj : %.c
	$(CC) $(CFLAGS) $< 

OBJLIST=..\aes.obj ..\asn1.obj ..\bigint.obj ..\crypto_misc.obj ..\hmac.obj \
        ..\md5.obj ..\loader.obj ..\p12.obj ..\os_port.obj ..\rc4.obj \
        ..\rsa.obj ..\sha1.obj ..\tls1.obj ..\tls1_clnt.obj ..\tls1_svr.obj

../../perf_bigint.exe: perf_bigint.obj $(OBJLIST)
	$(LD) $(LDFLAGS) /out:$@ $^

../../ssltest.exe: ssltest.obj $(OBJLIST)
	$(LD) $(LDFLAGS) /out:$@ $^
endif

clean::
	-@rm -f ../../perf_bigint* ../../ssltest*

include ../../config/makefile.post
