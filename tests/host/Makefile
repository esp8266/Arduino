BINDIR := bin
LCOV_DIRECTORY := lcov
OUTPUT_BINARY := $(BINDIR)/host_tests
CORE_PATH := ../../cores/esp8266
LIBRARIES_PATH := ../../libraries
FORCE32 ?= 1
OPTZ ?= -Os
V ?= 0

MAKEFILE = $(word 1, $(MAKEFILE_LIST))

# I wasn't able to build with clang when -coverage flag is enabled, forcing GCC on OS X
ifeq ($(shell uname -s),Darwin)
CC  ?= gcc
CXX ?= g++
endif
GCOV ?= gcov
VALGRIND ?= valgrind
LCOV ?= lcov
GENHTML ?= genhtml

ifeq ($(FORCE32),1)
SIZEOFLONG = $(shell echo 'int main(){return sizeof(long);}'|$(CXX) -m32 -x c++ - -o sizeoflong 2>/dev/null && ./sizeoflong; echo $$?; rm -f sizeoflong;)
ifneq ($(SIZEOFLONG),4)
$(warning Cannot compile in 32 bit mode, switching to native mode)
else
N32 = 32
M32 = -m32
E32 = .32
endif
endif

ifeq ($(N32),32)
$(warning compiling in 32 bits mode)
else
$(warning compiling in native mode)
endif

ifeq ($(V), 0)
VERBC   = @echo "C   $@";
VERBCXX = @echo "C++ $@";
VERBLD  = @echo "LD  $@";
VERBAR  = @echo "AR  $@";
else
VERBC   =
VERBCXX =
VERBLD  =
VERBAR  =
endif

$(shell mkdir -p $(BINDIR))

CORE_CPP_FILES := $(addprefix $(CORE_PATH)/,\
	StreamString.cpp \
	Stream.cpp \
	WString.cpp \
	Print.cpp \
	FS.cpp \
	spiffs_api.cpp \
	MD5Builder.cpp \
	../../libraries/LittleFS/src/LittleFS.cpp \
	core_esp8266_noniso.cpp \
	spiffs/spiffs_cache.cpp \
	spiffs/spiffs_check.cpp \
	spiffs/spiffs_gc.cpp \
	spiffs/spiffs_hydrogen.cpp \
	spiffs/spiffs_nucleus.cpp \
	libb64/cencode.cpp \
	libb64/cdecode.cpp \
	../../libraries/Schedule/src/Schedule.cpp \
	HardwareSerial.cpp \
	crc32.cpp \
	) \
	$(addprefix $(LIBRARIES_PATH)/ESP8266SdFat/src/, \
		FatLib/FatFile.cpp \
		FatLib/FatFileLFN.cpp \
		FatLib/FatFilePrint.cpp \
		FatLib/FatFileSFN.cpp \
		FatLib/FatVolume.cpp \
		FatLib/FmtNumber.cpp \
		FatLib/StdioStream.cpp \
	) \
	$(LIBRARIES_PATH)/SDFS/src/SDFS.cpp \
	$(LIBRARIES_PATH)/SD/src/SD.cpp

CORE_C_FILES := $(addprefix $(CORE_PATH)/,\
	../../libraries/LittleFS/src/lfs.c \
	../../libraries/LittleFS/src/lfs_util.c \
)

MOCK_CPP_FILES_COMMON := $(addprefix common/,\
	Arduino.cpp \
	flash_hal_mock.cpp \
	spiffs_mock.cpp \
	littlefs_mock.cpp \
	sdfs_mock.cpp \
	WMath.cpp \
	MockUART.cpp \
	MockTools.cpp \
	MocklwIP.cpp \
)

MOCK_CPP_FILES := $(MOCK_CPP_FILES_COMMON) $(addprefix common/,\
	ArduinoCatch.cpp \
)

MOCK_CPP_FILES_EMU := $(MOCK_CPP_FILES_COMMON) $(addprefix common/,\
	ArduinoMain.cpp \
	ArduinoMainUdp.cpp \
	ArduinoMainSpiffs.cpp \
	ArduinoMainLittlefs.cpp \
	user_interface.cpp \
)

MOCK_C_FILES := $(addprefix common/,\
	md5.c \
	noniso.c \
)

INC_PATHS += $(addprefix -I, \
	. \
	common \
	$(CORE_PATH) \
)

INC_PATHS += $(addprefix -I,\
	$(shell echo ../../libraries/*/src) \
	$(shell echo ../../libraries/*) \
	../../tools/sdk/include \
	../../tools/sdk/lwip2/include \
)

TEST_CPP_FILES := \
	fs/test_fs.cpp \
	core/test_pgmspace.cpp \
	core/test_md5builder.cpp \
	core/test_string.cpp \
	core/test_PolledTimeout.cpp \
	core/test_Print.cpp

PREINCLUDES := \
	-include common/mock.h \
	-include common/c_types.h \

ifneq ($(D),)
OPTZ=-O0
DEBUG += -DDEBUG_ESP_PORT=Serial
DEBUG += -DDEBUG_ESP_SSL -DDEBUG_ESP_TLS_MEM -DDEBUG_ESP_HTTP_CLIENT -DDEBUG_ESP_HTTP_SERVER -DDEBUG_ESP_CORE -DDEBUG_ESP_WIFI -DDEBUG_ESP_HTTP_UPDATE -DDEBUG_ESP_UPDATER -DDEBUG_ESP_OTA -DDEBUG_ESP_MDNS
endif

FLAGS += $(DEBUG) -Wall $(OPTZ) -fno-common -g $(M32)
FLAGS += -DHTTPCLIENT_1_1_COMPATIBLE=0
FLAGS += -DLWIP_IPV6=0
FLAGS += -DHOST_MOCK=1
FLAGS += -DNONOSDK221=1
FLAGS += $(MKFLAGS)
FLAGS += -Wimplicit-fallthrough=2 # allow "// fall through" comments to stop spurious warnings
CXXFLAGS += -std=c++11 -fno-rtti $(FLAGS) -funsigned-char
CFLAGS += -std=c99 $(FLAGS) -funsigned-char
LDFLAGS += -coverage $(OPTZ) -g $(M32)
VALGRINDFLAGS += --leak-check=full --track-origins=yes --error-limit=no --show-leak-kinds=all --error-exitcode=999
CXXFLAGS += -Wno-error=format-security # cores/esp8266/Print.cpp:42:24:   error: format not a string literal and no format arguments [-Werror=format-security] -- (os_printf_plus(not_the_best_way))
#CXXFLAGS += -Wno-format-security      # cores/esp8266/Print.cpp:42:40: warning: format not a string literal and no format arguments [-Wformat-security] -- (os_printf_plus(not_the_best_way))

remduplicates = $(strip $(if $1,$(firstword $1) $(call remduplicates,$(filter-out $(firstword $1),$1))))

C_SOURCE_FILES = $(MOCK_C_FILES) $(CORE_C_FILES)
CPP_SOURCE_FILES = $(MOCK_CPP_FILES) $(CORE_CPP_FILES) $(TEST_CPP_FILES)
C_OBJECTS = $(C_SOURCE_FILES:.c=.c$(E32).o)

CPP_OBJECTS_CORE = $(MOCK_CPP_FILES:.cpp=.cpp$(E32).o) $(CORE_CPP_FILES:.cpp=.cpp$(E32).o)
CPP_OBJECTS_TESTS = $(TEST_CPP_FILES:.cpp=.cpp$(E32).o)

CPP_OBJECTS = $(CPP_OBJECTS_CORE) $(CPP_OBJECTS_TESTS)

OBJECTS = $(C_OBJECTS) $(CPP_OBJECTS)
COVERAGE_FILES = $(OBJECTS:.o=.gc*)

all: help

CI:					# run CI
	$(MAKE) -f $(MAKEFILE) MKFLAGS="-Werror -coverage" FORCE32=0 OPTZ=-O0 doCI

doCI: build-info $(OUTPUT_BINARY) valgrind test gcov

test: $(OUTPUT_BINARY)			# run host test for CI
	$(OUTPUT_BINARY)

clean:
	make FORCE32=0 cleanarch; make FORCE32=1 cleanarch

cleanarch: clean-objects clean-coverage	# clean everything
	rm -rf $(BINDIR)

clean-objects:
	rm -rf $(C_OBJECTS) $(CPP_OBJECTS_CORE) $(CPP_OBJECTS_CORE_EMU) $(CPP_OBJECTS_TESTS)

clean-coverage:
	rm -rf $(COVERAGE_FILES) $(LCOV_DIRECTORY) *.gcov

gcov: test				# run coverage for CI
	find $(CORE_PATH) -name "*.gcno" -exec $(GCOV) -r -pb {} +

valgrind: $(OUTPUT_BINARY)
	mkdir -p $(LCOV_DIRECTORY)
	$(LCOV) --directory ../../cores/esp8266/ --zerocounters
	$(VALGRIND) $(VALGRINDFLAGS) $(OUTPUT_BINARY)
	$(LCOV) --directory $(CORE_PATH) --capture --output-file $(LCOV_DIRECTORY)/app.info
	-$(GENHTML) $(LCOV_DIRECTORY)/app.info -o $(LCOV_DIRECTORY)

build-info:				# show toolchain version
	@echo "-------- build tools info --------"
	@echo "CC: " $(CC)
	$(CC) -v
	@echo "CXX: " $(CXX)
	$(CXX) -v
	@echo "GCOV: " $(GCOV)
	$(GCOV) -v
	@echo "----------------------------------"

-include $(BINDIR)/.*.d
.SUFFIXES:

%.c$(E32).o: %.c
	$(VERBC) $(CC) $(PREINCLUDES) $(CFLAGS) $(INC_PATHS) -MD -MF $(BINDIR)/.$(notdir $<).d -c -o $@ $<

.PRECIOUS: %.cpp$(E32).o
%.cpp$(E32).o: %.cpp
	$(VERBCXX) $(CXX) $(PREINCLUDES) $(CXXFLAGS) $(INC_PATHS) -MD -MF $(BINDIR)/.$(notdir $<).d -c -o $@ $<

$(BINDIR)/core.a: $(C_OBJECTS) $(CPP_OBJECTS_CORE)
	ar -rcu $@ $^
	ranlib -c $@

$(OUTPUT_BINARY): $(CPP_OBJECTS_TESTS) $(BINDIR)/core.a
	$(VERBLD) $(CXX) $(LDFLAGS) $^ -o $@

#################################################
# building ino sources

ARDUINO_LIBS := \
	$(addprefix $(CORE_PATH)/,\
		IPAddress.cpp \
		Updater.cpp \
		base64.cpp \
	) \
	$(addprefix ../../libraries/ESP8266WiFi/src/,\
		ESP8266WiFi.cpp \
		ESP8266WiFiAP.cpp \
		ESP8266WiFiGeneric.cpp \
		ESP8266WiFiMulti.cpp \
		ESP8266WiFiSTA-WPS.cpp \
		ESP8266WiFiSTA.cpp \
		ESP8266WiFiScan.cpp \
		WiFiClient.cpp \
		WiFiUdp.cpp \
		WiFiClientSecureBearSSL.cpp \
		WiFiServerSecureBearSSL.cpp \
		BearSSLHelpers.cpp \
		CertStoreBearSSL.cpp \
	)

OPT_ARDUINO_LIBS ?= $(addprefix ../../libraries/,\
	$(addprefix ESP8266WebServer/src/,\
		detail/mimetable.cpp \
	) \
	$(addprefix ESP8266mDNS/src/,\
		LEAmDNS.cpp \
		LEAmDNS_Control.cpp \
		LEAmDNS_Helpers.cpp \
		LEAmDNS_Structs.cpp \
		LEAmDNS_Transfer.cpp \
		ESP8266mDNS.cpp \
	) \
	ArduinoOTA/ArduinoOTA.cpp \
	DNSServer/src/DNSServer.cpp \
	ESP8266AVRISP/src/ESP8266AVRISP.cpp \
	ESP8266HTTPClient/src/ESP8266HTTPClient.cpp \
)

MOCK_ARDUINO_LIBS := $(addprefix common/,\
	ClientContextSocket.cpp \
	ClientContextTools.cpp \
	MockWiFiServerSocket.cpp \
	MockWiFiServer.cpp \
	UdpContextSocket.cpp \
	HostWiring.cpp \
	MockEsp.cpp \
	MockEEPROM.cpp \
	MockSPI.cpp \
	strl.cpp \
)

CPP_SOURCES_CORE_EMU = \
	$(MOCK_CPP_FILES_EMU) \
	$(CORE_CPP_FILES) \
	$(MOCK_ARDUINO_LIBS) \
	$(OPT_ARDUINO_LIBS) \
	$(ARDUINO_LIBS) \

LIBSSLFILE = ../../tools/sdk/ssl/bearssl/build$(N32)/libbearssl.a
ifeq (,$(wildcard $(LIBSSLFILE)))
LIBSSL =
else
LIBSSL = $(LIBSSLFILE)
endif
ssl:							# download source and build BearSSL
	cd ../../tools/sdk/ssl && make native$(N32)

ULIBPATHS = $(shell echo $(ULIBDIRS) | sed 's,:, ,g')
USERLIBDIRS = $(shell test -z "$(ULIBPATHS)" || for d in $(ULIBPATHS); do for dd in $$d $$d/src; do test -d $$dd && { echo -I$$dd; echo "userlib: using directory '$$dd'" 1>&2; } done; done)
USERLIBSRCS = $(shell test -z "$(ULIBPATHS)" || for d in $(ULIBPATHS); do for ss in $$d/*.cpp $$d/src/*.cpp; do test -r $$ss && echo $$ss; done; done)
INC_PATHS += $(USERLIBDIRS)
INC_PATHS += -I$(INODIR)/..
CPP_OBJECTS_CORE_EMU = $(CPP_SOURCES_CORE_EMU:.cpp=.cpp$(E32).o) $(USERLIBSRCS:.cpp=.cpp$(E32).o)

bin/fullcore.a: $(C_OBJECTS) $(CPP_OBJECTS_CORE_EMU)
	$(VERBAR) ar -rcu $@ $^
	$(VERBAR) ranlib -c $@

%: %.ino.cpp$(E32).o bin/fullcore.a
	$(VERBLD) $(CXX) $(LDFLAGS) $< bin/fullcore.a $(LIBSSL) -o $@
	@echo "----> $@ <----"

#################################################
# are we in primary make call ?
ifeq ($(INO),)

%: %.ino
	@# recursive 'make' with paths
	$(MAKE) -f $(MAKEFILE) MKFLAGS=-Wextra INODIR=$(dir $@) INO=$(notdir $@) $(BINDIR)/$(notdir $@)/$(notdir $@)
	@# see below the new build rule with fixed output path outside from core location

#####################
# recursive call on ino target
else

$(BINDIR)/$(INO)/$(INO).ino.cpp:
	@# arduino builder would come around here (.ino -> .ino.cpp)
	@mkdir -p $(BINDIR)/$(INO); \
	( \
		echo "#include <$(INODIR)/$(INO).ino>"; \
		for i in $(INODIR)/*.ino; do \
			test "$$i" = $(INODIR)/$(INO).ino || echo "#include \"$$i\""; \
		done; \
	) > $(BINDIR)/$(INO)/$(INO).ino.cpp
	
endif # recursive
#####################

#################################################

.PHONY: list
list:							# show core example list
	@for dir in ../../libraries/*/examples/* \
	            ../../libraries/*/examples/*/*; do \
		test -d $$dir || continue; \
		examplename=$${dir##*/}; \
		test -f $${dir}/$${examplename}.ino || continue; \
		echo $${dir}/$${examplename}; \
	done | sort; \

#################################################
# help

.PHONY: help
help:
	@cat README.txt
	@echo ""
	@echo "Make rules:"
	@echo ""
	@sed -rne 's,([^: \t]*):[^=#]*#[\t ]*(.*),\1 - \2,p' $(MAKEFILE)
	@echo ""
	
