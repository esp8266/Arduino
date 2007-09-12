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

all : web_server lua

AXTLS_HOME=..

include $(AXTLS_HOME)/config/.config
include $(AXTLS_HOME)/config/makefile.conf

ifndef CONFIG_PLATFORM_WIN32

ifdef CONFIG_PLATFORM_CYGWIN
TARGET=$(AXTLS_HOME)/$(STAGE)/axhttpd.exe
TARGET2=$(AXTLS_HOME)/$(STAGE)/htpasswd.exe
else
TARGET=$(AXTLS_HOME)/$(STAGE)/axhttpd
TARGET2=$(AXTLS_HOME)/$(STAGE)/htpasswd
endif

ifdef CONFIG_HTTP_STATIC_BUILD
LIBS=$(AXTLS_HOME)/$(STAGE)/libaxtls.a
else
LIBS=-L$(AXTLS_HOME)/$(STAGE) -laxtls
endif

ifdef CONFIG_HTTP_BUILD_LUA
lua: kepler-1.1

kepler-1.1:
	@tar xvfz   kepler-1.1-snapshot-20070521-1825.tar.gz
	@cat kepler.patch | patch -p0
	cd kepler-1.1; ./configure --prefix=$(CONFIG_HTTP_LUA_PREFIX) --launcher=cgi --lua-suffix= ; make install
else
lua:
endif

else # win32 build
lua:

TARGET=$(AXTLS_HOME)/$(STAGE)/axhttpd.exe
TARGET2=$(AXTLS_HOME)/$(STAGE)/htpasswd.exe

ifdef CONFIG_HTTP_STATIC_BUILD
LIBS=$(AXTLS_HOME)/$(STAGE)/axtls.static.lib $(AXTLS_HOME)\\config\\axtls.res
else
LIBS=$(AXTLS_HOME)/$(STAGE)/axtls.lib $(AXTLS_HOME)\\config\\axtls.res
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
	tdate_parse.o

include $(AXTLS_HOME)/config/makefile.post

ifndef CONFIG_PLATFORM_WIN32

$(TARGET): $(OBJ) $(AXTLS_HOME)/$(STAGE)/libaxtls.a
	$(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
ifdef CONFIG_STRIP_UNWANTED_SECTIONS
	strip --remove-section=.comment $(TARGET)
endif

$(TARGET2): htpasswd.o $(AXTLS_HOME)/$(STAGE)/libaxtls.a
	$(LD) $(LDFLAGS) -o $@ htpasswd.o $(LIBS)

else    # Win32

OBJ:=$(OBJ:.o=.obj)
%.obj : %.c
	$(CC) $(CFLAGS) $< 

htpasswd.obj : htpasswd.c
	$(CC) $(CFLAGS) $? 
	
$(TARGET): $(OBJ)
	$(LD) $(LDFLAGS) /out:$@ $(LIBS) $?

$(TARGET2): htpasswd.obj
	$(LD) $(LDFLAGS) /out:$@ $(LIBS) $?
endif

endif       # CONFIG_AXHTTPD

clean::
	-@rm -f $(TARGET)*
	-@rm -fr kepler-1.1

