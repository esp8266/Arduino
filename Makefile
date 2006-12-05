#
#  Copyright(C) 2006 Cameron Rich
#
#  This license is free software; you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#
#  This license is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public License
#  along with this license; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#

-include config/.config

ifneq ($(strip $(HAVE_DOT_CONFIG)),y)
all: menuconfig
else
all: target
endif

include config/makefile.conf

target : $(STAGE) $(TARGET)

# VERSION has to come from the command line
RELEASE=axTLS-$(VERSION)

# standard version
target:
	$(MAKE) -C ssl
ifdef CONFIG_AXHTTPD
	$(MAKE) -C httpd
endif
ifdef CONFIG_BINDINGS
	$(MAKE) -C bindings
endif
ifdef CONFIG_SAMPLES
	$(MAKE) -C samples
endif

$(STAGE) :
	@mkdir -p $(STAGE)

$(PREFIX) :
	@mkdir -p $(PREFIX)/lib
	@mkdir -p $(PREFIX)/bin

release:
	$(MAKE) -C config/scripts/config clean
	-$(MAKE) clean
	-@rm config/*.msi config/*.back.aip config/config.h config/.config*
	@rm -fr $(STAGE)
	cd ../; tar cvfz $(RELEASE).tar.gz --wildcards-match-slash --exclude .svn axTLS; cd -;

docs:
	$(MAKE) -C docsrc doco

# build the Win32 demo release version
win32_demo:
	-@rm -fr ../axTLS.release_test > /dev/null 2>&1
	$(MAKE) win32releaseconf

install: $(PREFIX) all
	cp --no-dereference $(STAGE)/libax* $(PREFIX)/lib
	chmod 755 $(PREFIX)/lib/libax* 
	-@install -m 755 $(STAGE)/ax* $(PREFIX)/bin > /dev/null 2>&1
	-@install -m 755 $(STAGE)/axtlsp.pm `perl -e 'use Config; print $$Config{installarchlib};'` > /dev/null 2>&1
	-@install -m 755 $(STAGE)/axhttpd* $(PREFIX)/bin > /dev/null 2>&1

installclean:
	-@rm $(PREFIX)/lib/libax* > /dev/null 2>&1
	-@rm $(PREFIX)/bin/ax* > /dev/null 2>&1
	-@rm $(PREFIX)/bin/axhttpd* > /dev/null 2>&1
	-@rm `perl -e 'use Config; print $$Config{installarchlib};'`/axtlsp.pm > /dev/null 2>&1

test:
	cd $(STAGE); ssltest; ../ssl/test/test_axssl.sh; cd -;

# tidy up things
clean::
	@cd ssl; $(MAKE) clean
	@cd httpd; $(MAKE) clean
	@cd samples; $(MAKE) clean
	@cd docsrc; $(MAKE) clean
	@cd bindings; $(MAKE) clean

# ---------------------------------------------------------------------------
# mconf stuff
# ---------------------------------------------------------------------------

CONFIG_CONFIG_IN = config/Config.in
CONFIG_DEFCONFIG = config/defconfig

config/scripts/config/conf: config/scripts/config/Makefile
	$(MAKE) -C config/scripts/config conf
	-@if [ ! -f config/.config ] ; then \
		cp $(CONFIG_DEFCONFIG) config/.config; \
	fi

config/scripts/config/mconf: config/scripts/config/Makefile
	$(MAKE) -C config/scripts/config ncurses conf mconf
	-@if [ ! -f config/.config ] ; then \
		cp $(CONFIG_DEFCONFIG) .config; \
	fi

cleanconf:
	$(MAKE) -C config/scripts/config clean
	@rm -f config/.config

menuconfig: config/scripts/config/mconf
	@./config/scripts/config/mconf $(CONFIG_CONFIG_IN)

config: config/scripts/config/conf
	@./config/scripts/config/conf $(CONFIG_CONFIG_IN)

oldconfig: config/scripts/config/conf
	@./config/scripts/config/conf -o $(CONFIG_CONFIG_IN)

default: config/scripts/config/conf
	@./config/scripts/config/conf -d $(CONFIG_CONFIG_IN) > /dev/null
	$(MAKE)

randconfig: config/scripts/config/conf
	@./config/scripts/config/conf -r $(CONFIG_CONFIG_IN)

allnoconfig: config/scripts/config/conf
	@./config/scripts/config/conf -n $(CONFIG_CONFIG_IN)

allyesconfig: config/scripts/config/conf
	@./config/scripts/config/conf -y $(CONFIG_CONFIG_IN)

# The special win32 release configuration
win32releaseconf: config/scripts/config/conf
	@./config/scripts/config/conf -D config/win32config $(CONFIG_CONFIG_IN) > /dev/null
	$(MAKE)

# The special linux release configuration
linuxconf: config/scripts/config/conf
	@./config/scripts/config/conf -D config/linuxconfig $(CONFIG_CONFIG_IN) > /dev/null
	$(MAKE)
