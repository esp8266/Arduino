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

all : sample

include ../../config/.config
include ../../config/makefile.conf

ifndef CONFIG_PLATFORM_WIN32

ifdef CONFIG_PLATFORM_CYGWIN
TARGET=../../axssl.exe
else
TARGET=../../axssl
endif   # cygwin

LIBS=../../libaxtls.a
CFLAGS += -I../../ssl -I../../config
else
TARGET=../../axssl.exe
LIBS=../../axtls.lib
CFLAGS += /I"..\..\ssl" /I"..\..\config"
endif

ifndef CONFIG_C_SAMPLES
sample:

else
sample : $(TARGET)
OBJ= axssl.o
include ../../config/makefile.post

ifndef CONFIG_PLATFORM_WIN32

$(TARGET): $(OBJ) $(LIBS)
	$(LD) $(LDFLAGS) -o $@ $^
ifndef CONFIG_DEBUG
ifndef CONFIG_PLATFORM_SOLARIS
	strip --remove-section=.comment $(TARGET)
endif   # SOLARIS
endif   # CONFIG_DEBUG
else    # Win32

$(TARGET): $(OBJ) $(LIBS)
	$(LD) $(LDFLAGS) ..\..\config\axtls.res /out:$@ $^
endif

endif    # CONFIG_C_SAMPLES

clean::
	-@rm -f ../../axssl*

