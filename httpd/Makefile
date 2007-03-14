#
#  Copyright(C) 2007 Cameron Rich
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
TARGET=../$(STAGE)/axhttpd.exe
TARGET2=../$(STAGE)/htpasswd.exe
else
TARGET=../$(STAGE)/axhttpd
TARGET2=../$(STAGE)/htpasswd
endif

ifdef CONFIG_HTTP_STATIC_BUILD
LIBS=../$(STAGE)/libaxtls.a
else
LIBS=-L../$(STAGE) -laxtls
endif

CFLAGS += -I../ssl

else # win32 build
TARGET=../$(STAGE)/axhttpd.exe
TARGET2=../$(STAGE)/htpasswd.exe

ifdef CONFIG_HTTP_STATIC_BUILD
LIBS=../$(STAGE)/axtls.static.lib ..\\config\\axtls.res
else
LIBS=../$(STAGE)/axtls.lib ..\\config\\axtls.res
endif
endif

ifndef CONFIG_AXHTTPD
web_server:
else

web_server :: $(TARGET)

ifdef CONFIG_HTTP_HAS_AUTHORIZATION
web_server :: $(TARGET2)
endif

OBJ= \
	axhttpd.o \
	proc.o \
	mime_types.o \
	tdate_parse.o

include ../config/makefile.post

ifndef CONFIG_PLATFORM_WIN32

$(TARGET): $(OBJ) ../$(STAGE)/libaxtls.a
	$(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
ifndef CONFIG_DEBUG
ifndef CONFIG_PLATFORM_SOLARIS
	strip --remove-section=.comment $(TARGET)
endif
endif

$(TARGET2): htpasswd.o ../$(STAGE)/libaxtls.a
	$(LD) $(LDFLAGS) -o $@ htpasswd.o $(LIBS)

else    # Win32

OBJ:=$(OBJ:.o=.obj)
%.obj : %.c
	$(CC) $(CFLAGS) $< 

htpasswd.obj : htpasswd.c
	$(CC) $(CFLAGS) $< 
	
$(TARGET): $(OBJ)
	$(LD) $(LDFLAGS) $(LIBS) /out:$@ $(OBJ)

$(TARGET2): htpasswd.obj
	$(LD) $(LDFLAGS) $(LIBS) /out:$@ $<
endif

endif       # CONFIG_AXHTTPD

clean::
	-@rm -f $(TARGET)*

