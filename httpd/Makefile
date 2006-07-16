#
#  Copyright(C) 2006
#
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

all : web_server

include ../config/.config
include ../config/makefile.conf

ifndef CONFIG_PLATFORM_WIN32

ifdef CONFIG_PLATFORM_CYGWIN
TARGET=../$(STAGE)/awhttpd.exe
else
TARGET=../$(STAGE)/awhttpd
endif

ifdef CONFIG_HTTP_STATIC_BUILD
LIBS=../$(STAGE)/libaxtls.a
else
LIBS=-L../$(STAGE) -laxtls
endif

CFLAGS += -I../ssl

else # win32 build
TARGET=../$(STAGE)/awhttpd.exe

ifdef CONFIG_HTTP_STATIC_BUILD
LIBS=../$(STAGE)/axtls.static.lib ..\\config\\axtls.res
else
LIBS=../$(STAGE)/axtls.lib ..\\config\\axtls.res
endif
endif

ifndef CONFIG_AWHTTPD
web_server:
else

untar_web_server: awhttpd/Makefile

awhttpd/Makefile:
	tar xvf awhttpd-3.0.7.tar
	cat awhttpd.patch | patch -p0

web_server : $(TARGET)

OBJ= \
    cgi.o \
	conn.o \
	main.o \
	net.o \
	proc.o \
	socket.o \
	errors.o \
	misc.o \
	urldecode.o \
	mime_types.o \
	index.o \
	urlencode.o \
    permcheck.o \
	conf.o

%.o : awhttpd/%.c ../config/.config
	$(CC) -c $(CFLAGS) $< 

ifndef CONFIG_PLATFORM_WIN32

$(TARGET): $(OBJ)
ifdef CONFIG_HTTP_NO_SSL
	$(LD) $(LDFLAGS) -o $@ $(OBJ)
endif
	$(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
ifndef CONFIG_DEBUG
ifndef CONFIG_PLATFORM_SOLARIS
	strip --remove-section=.comment $(TARGET)
endif
endif
else    # Win32

OBJ:=$(OBJ:.o=.obj)
%.obj : awhttpd/%.c
	$(CC) $(CFLAGS) $< 

$(TARGET): $(OBJ)
ifdef CONFIG_HTTP_NO_SSL
	$(LD) $(LDFLAGS) /out:$@ $(OBJ)
endif
	$(LD) $(LDFLAGS) $(LIBS) /out:$@ $(OBJ)
endif

endif       # CONFIG_AWHTTPD

clean::
	-@rm -f $(TARGET)*
	-@rm -fr awhttpd

