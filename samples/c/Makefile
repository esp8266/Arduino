#
#  Copyright(C) 2006 Cameron Rich
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

AXTLS_HOME=../..

include $(AXTLS_HOME)/config/.config
include $(AXTLS_HOME)/config/makefile.conf

ifndef CONFIG_PLATFORM_WIN32

ifdef CONFIG_PLATFORM_CYGWIN
TARGET=$(AXTLS_HOME)/$(STAGE)/axssl.exe
else
TARGET=$(AXTLS_HOME)/$(STAGE)/axssl
endif   # cygwin

LIBS=$(AXTLS_HOME)/$(STAGE)
else
TARGET=$(AXTLS_HOME)/$(STAGE)/axssl.exe
endif

ifndef CONFIG_C_SAMPLES
sample:

else
sample : $(TARGET)
OBJ= axssl.o
include $(AXTLS_HOME)/config/makefile.post

ifndef CONFIG_PLATFORM_WIN32

$(TARGET): $(OBJ) $(LIBS)/libaxtls.a
	$(LD) $(LDFLAGS) -o $@ $(OBJ) -L$(LIBS) -laxtls 
ifdef CONFIG_STRIP_UNWANTED_SECTIONS
	strip --remove-section=.comment $(TARGET)
endif   # use strip
else    # Win32

$(TARGET): $(OBJ)
	$(LD) $(LDFLAGS) $(AXTLS_HOME)/config/axtls.res /out:$@ $^ /libpath:"$(AXTLS_HOME)/$(STAGE)" axtls.lib
endif

endif    # CONFIG_C_SAMPLES

clean::
	-@rm -f $(AXTLS_HOME)/$(STAGE)/axssl*

