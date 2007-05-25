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

include ../config/.config
include ../config/makefile.conf

ifdef CONFIG_CSHARP_BINDINGS
all: csharp/axInterface.cs
endif

ifdef CONFIG_VBNET_BINDINGS
all: vbnet/axInterface.vb
endif

ifdef CONFIG_JAVA_BINDINGS
all: java/axtlsj.java
endif

ifdef CONFIG_PERL_BINDINGS
all: perl/axTLSp_wrap.c
endif

ifdef CONFIG_LUA_BINDINGS
all: lua/axTLSl_wrap.c
endif

csharp/axInterface.cs: ../ssl/ssl.h
	@perl ./generate_interface.pl -csharp

vbnet/axInterface.vb: ../ssl/ssl.h
	@perl ./generate_interface.pl -vbnet

java/axTLSj.i: ../ssl/ssl.h
	@perl ./generate_SWIG_interface.pl -java

java/axtlsj.java: java/axTLSj.i $(wildcard java/SSL*.java)
	@cd java; swig -java -package axTLSj axTLSj.i; $(MAKE)

perl/axTLSp.i: ../ssl/ssl.h
	@perl ./generate_SWIG_interface.pl -perl

perl/axTLSp_wrap.c: perl/axTLSp.i
	@cd perl; swig -perl5 axTLSp.i; $(MAKE)

lua/axTLSl.i: ../ssl/ssl.h
	@perl ./generate_SWIG_interface.pl -lua

lua/axTLSl_wrap.c: lua/axTLSl.i
	@cd lua; swig -lua axTLSl.i; $(MAKE)

clean::
	$(MAKE) -C csharp clean
	$(MAKE) -C vbnet clean
	$(MAKE) -C java clean
	$(MAKE) -C perl clean
	$(MAKE) -C lua clean

