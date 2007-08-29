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

AXTLS_HOME=../..

include $(AXTLS_HOME)/config/.config
include $(AXTLS_HOME)/config/makefile.conf

all: lib

ifdef CONFIG_PLATFORM_WIN32
TARGET=$(AXTLS_HOME)/$(STAGE)/axtlsp.dll
else
TARGET=$(AXTLS_HOME)/$(STAGE)/libaxtlsp.so
endif

ifneq ($(MAKECMDGOALS), clean)

ifdef CONFIG_PLATFORM_WIN32
PERL5_CORE:=$(shell cygpath -w "$(CONFIG_PERL_CORE)")
else
PERL5_CORE= $(shell perl -e 'use Config; print $$Config{archlib};')/CORE
endif

all: test_perl

test_perl:
	@if ! [ -d "$(PERL5_CORE)" ]; then \
		echo "*** Error: Perl not installed at $(CONFIG_PERL_CORE) - go to " \
        "http://www.cpan.org/authors/id/G/GR/GRAHAMC/SiePerl-5.8.0-bin-1.0-Win32.INSTALL.exe" && exit 1; \
	fi

endif

lib: $(TARGET)
OBJ:=axTLSp_wrap.o
include $(AXTLS_HOME)/config/makefile.post

ifndef CONFIG_PLATFORM_WIN32        # Linux/Unix/Cygwin

#
# Could have used libperl.a, but it increases the library to over 1MB, so just
# use libperl.so. But this needs to be in the shared library path for things to
# work.
#
$(TARGET) : $(OBJ)
	$(LD) $(LDFLAGS) -L$(AXTLS_HOME)/$(STAGE) -L$(PERL5_CORE) $(LDSHARED) -o $@ $(OBJ) -laxtls -lperl
ifdef CONFIG_PLATFORM_CYGWIN
	cd $(AXTLS_HOME)/$(STAGE); ln -sf $(notdir $@) axtlsp.dll
endif
	@install axtlsp.pm $(AXTLS_HOME)/$(STAGE)

CFLAGS += -D_GNU_SOURCE -I$(PERL5_CORE)
else
CFLAGS += /I"$(PERL5_CORE)"
LDFLAGS += $(CONFIG_PERL_LIB) /libpath:"$(PERL5_CORE)" axtls.lib /libpath:"$(AXTLS_HOME)/$(STAGE)"

$(TARGET) : $(OBJ)
	$(LD) $(LDFLAGS) $(LDSHARED) /out:$@ $(OBJ)
	install axtlsp.pm $(AXTLS_HOME)/$(STAGE)
endif # WIN32

clean::
	@rm -f $(TARGET) axtls* *.i axTLSp* *.c .depend $(AXTLS_HOME)/$(STAGE)/axtlsp.pm
