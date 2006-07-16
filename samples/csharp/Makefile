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

include ../../config/.config
include ../../config/makefile.conf
include ../../config/makefile.dotnet.conf

all : sample
TARGET=../../$(STAGE)/axssl.csharp.exe
sample : $(TARGET)

$(TARGET): ../../bindings/csharp/axTLS.cs ../../bindings/csharp/axInterface.cs axssl.cs 
ifdef GO_DOT_NET
	csc.exe /nologo /t:exe /out:"`cygpath -w $@`" $(foreach file, $^, "`cygpath -w $(file)`")
else    # use mono to build
	mcs -out:$@ $^

endif    # ARCH

clean::
	-@rm -f $(TARGET)
