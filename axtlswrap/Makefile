#
# Copyright (c) 2009, Cameron Rich
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# * Redistributions of source code must retain the above copyright notice,
#   this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# * Neither the name of the axTLS project nor the names of its
#   contributors may be used to endorse or promote products derived
#   from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR 
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
# TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY 
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

all : sslwrap

AXTLS_HOME=..

include $(AXTLS_HOME)/config/.config
include $(AXTLS_HOME)/config/makefile.conf

ifdef CONFIG_PLATFORM_CYGWIN
TARGET=$(AXTLS_HOME)/$(STAGE)/axtlswrap.exe
else # linux
TARGET=$(AXTLS_HOME)/$(STAGE)/axtlswrap
endif

ifdef CONFIG_HTTP_STATIC_BUILD
LIBS=$(AXTLS_HOME)/$(STAGE)/libaxtls.a
else
LIBS=-L$(AXTLS_HOME)/$(STAGE) -laxtls
endif

ifndef CONFIG_AXTLSWRAP
sslwrap:
else
sslwrap : $(TARGET)

OBJ= axtlswrap.o
include $(AXTLS_HOME)/config/makefile.post

$(TARGET): $(OBJ) $(AXTLS_HOME)/$(STAGE)/libaxtls.a
	$(LD) $(LDFLAGS) -o $@ $(OBJ) $(LIBS)
ifdef CONFIG_STRIP_UNWANTED_SECTIONS
	$(STRIP) --remove-section=.comment $(TARGET)
endif

endif   # CONFIG_AXTLSWRAP

clean::
	-@rm -f $(TARGET)*

