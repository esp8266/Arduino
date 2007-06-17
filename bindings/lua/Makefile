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

include ../../config/.config
include ../../config/makefile.conf

all: lib

ifdef CONFIG_PLATFORM_WIN32
TARGET=../../$(STAGE)/axtlsl.dll
else
TARGET=../../$(STAGE)/axtlsl.so
endif

ifneq ($(MAKECMDGOALS), clean)

lib: $(TARGET)
AXTLS_HOME=../..
SSL_HOME=$(AXTLS_HOME)/ssl
CONFIG_HOME=$(AXTLS_HOME)/config
OBJ:=axTLSl_wrap.o
include ../../config/makefile.post

# there are a few static functions that aren't used
CFLAGS += -funit-at-a-time

$(TARGET) : $(OBJ)
	$(LD) $(LDFLAGS) -L../../$(STAGE) -L$(CONFIG_LUA_CORE)/lib $(LDSHARED) -o $@ $(OBJ) -laxtls -llua

CFLAGS += -I$(CONFIG_HOME) -I$(SSL_HOME) -I $(CONFIG_LUA_CORE)/include
else
CFLAGS += /I"`cygpath -w $(CONFIG_HOME)`" /I"`cygpath -w $(SSL_HOME)`" /I"`cygpath -w $(CONFIG_LUA_CORE)/include`"
LDFLAGS += axtls.lib /libpath:"../../$(STAGE)"

$(TARGET) : $(OBJ)
	$(LD) $(LDFLAGS) $(LDSHARED) /out:$@ $(OBJ)
endif # WIN32

clean::
	@rm -f $(TARGET) *.i axTLSl* .depend
