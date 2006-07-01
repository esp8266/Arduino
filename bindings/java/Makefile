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
include ../../config/makefile.java.conf

all: lib jar

JAR=../../axtls.jar

ifdef CONFIG_PLATFORM_WIN32
TARGET=../../axtlsj.dll
else
TARGET=../../libaxtlsj.so
endif

lib: $(TARGET)
axTLSj_wrap.o : axTLSj_wrap.c

JAVA_FILES= \
	axtlsjJNI.java \
    axtlsjConstants.java \
    axtlsj.java \
    SSLReadHolder.java \
    SSL.java \
    SSLUtil.java \
    SSLCTX.java \
    SSLServer.java \
    SSLClient.java

OBJ=axTLSj_wrap.o

AXOLOTLS_HOME=../..
SSL_HOME=$(AXOLOTLS_HOME)/ssl
CONFIG_HOME=$(AXOLOTLS_HOME)/config
JAVA_CLASSES:=$(JAVA_FILES:%.java=classes/axTLSj/%.class)

ifdef CONFIG_PLATFORM_WIN32
CFLAGS += /I"$(shell cygpath -w $(SSL_HOME))"
CFLAGS += /I"$(shell cygpath -w $(CONFIG_HOME))"
LDFLAGS += axtls.lib /libpath:"../../"

include ../../config/makefile.post

$(TARGET) : $(OBJ)
	$(LD) $(LDFLAGS) $(LDSHARED) /out:$@ $(OBJ)
else    # Not Win32

ifdef CONFIG_PLATFORM_CYGWIN
SSL_HOME:=$(shell cygpath -u $(SSL_HOME))
CONFIG_HOME:=$(shell cygpath -u $(CONFIG_HOME))
endif

CFLAGS += -I$(SSL_HOME)
CFLAGS += -I$(CONFIG_HOME)

$(TARGET) : $(OBJ)
	$(LD) $(LDFLAGS) -L ../../ $(LDSHARED) -o $@ $(OBJ) -laxtls 
endif

jar: $(OBJ) $(JAR)

# if we are doing the samples then defer creating the jar until then
$(JAR): $(JAVA_CLASSES)
ifndef CONFIG_JAVA_SAMPLES
	jar cvf $@ -C classes .
else
	@if [ ! -f $(JAR) ]; then touch $(JAR); fi
endif

classes/axTLSj/%.class : %.java
	javac -d classes -classpath classes $^

clean::
	@rm -f $(JAR) $(TARGET) SWIG* axtls* *.i *.c 
	@rm -fr classes/*

