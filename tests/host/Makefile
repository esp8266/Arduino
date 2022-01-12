CORE_PATH := $(abspath ../../cores/esp8266)
LIBRARIES_PATH := $(abspath ../../libraries)
common = common
HOST_COMMON_ABSPATH := $(abspath $(common))
FORCE32 ?= 1
OPTZ ?= -Os
V ?= 0
R ?= noexec
TERM ?= xterm
DEFSYM_FS ?= -Wl,--defsym,_FS_start=0x40300000 -Wl,--defsym,_FS_end=0x411FA000 -Wl,--defsym,_FS_page=0x100 -Wl,--defsym,_FS_block=0x2000 -Wl,--defsym,_EEPROM_start=0x411fb000
RANLIB ?= ranlib

MAKEFILE = $(word 1, $(MAKEFILE_LIST))

CXX =  $(shell for i in g++-10  g++-9  g++-8  g++;  do which $$i > /dev/null && { echo $$i; break; } done)
CC  =  $(shell for i in gcc-10  gcc-9  gcc-8  gcc;  do which $$i > /dev/null && { echo $$i; break; } done)
GCOV = $(shell for i in gcov-10 gcov-9 gcov-8 gcov; do which $$i > /dev/null && { echo $$i; break; } done)
$(warning using $(CXX))
ifeq ($(CXX),g++)
CXXFLAGS += -std=gnu++11
else
CXXFLAGS += -std=gnu++17
endif
ifeq ($(CC),gcc)
CFLAGS += -std=gnu11
else
CFLAGS += -std=gnu17
endif

# I wasn't able to build with clang when -coverage flag is enabled, forcing GCC on OS X
ifeq ($(shell uname -s),Darwin)
CC  ?= gcc
CXX ?= g++
endif
GCOV ?= gcov
VALGRIND ?= valgrind
LCOV ?= lcov --gcov-tool $(GCOV)
GENHTML ?= genhtml

ifeq ($(FORCE32),1)
SIZEOFLONG = $(shell echo 'int main(){return sizeof(long);}'|$(CXX) -m32 -x c++ - -o sizeoflong 2>/dev/null && ./sizeoflong; echo $$?; rm -f sizeoflong;)
ifneq ($(SIZEOFLONG),4)
$(warning Cannot compile in 32 bit mode (g++-multilib is missing?), switching to native mode)
else
N32 = 32
M32 = -m32
endif
endif

ifeq ($(N32),32)
$(warning compiling in 32 bits mode)
BINDIR := $(abspath bin32)
else
$(warning compiling in native mode)
BINDIR := $(abspath bin)
endif
OUTPUT_BINARY := $(BINDIR)/host_tests
LCOV_DIRECTORY := $(BINDIR)/../lcov

ifeq ($(V), 0)
VERBC   = @echo "C   $@";
VERBCXX = @echo "C++ $@";
VERBLD  = @echo "LD  $@";
VERBAR  = @echo "AR  $@";
VERBRANLIB = @echo "RANLIB $@";
else
VERBC   =
VERBCXX =
VERBLD  =
VERBAR  =
VERBRANLIB =
endif

$(shell mkdir -p $(BINDIR))

CORE_CPP_FILES := \
	$(addprefix $(abspath $(CORE_PATH))/,\
		debug.cpp \
		StreamSend.cpp \
		Stream.cpp \
		WString.cpp \
		Print.cpp \
		stdlib_noniso.cpp \
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
		Schedule.cpp \
		HardwareSerial.cpp \
		crc32.cpp \
		Updater.cpp \
		time.cpp \
	) \
	$(addprefix $(abspath $(LIBRARIES_PATH)/ESP8266SdFat/src)/, \
		FatLib/FatFile.cpp \
		FatLib/FatFileLFN.cpp \
		FatLib/FatFilePrint.cpp \
		FatLib/FatFileSFN.cpp \
		FatLib/FatFormatter.cpp \
		FatLib/FatName.cpp \
		FatLib/FatVolume.cpp \
		FatLib/FatPartition.cpp \
		common/FmtNumber.cpp \
		common/FsCache.cpp \
		common/FsStructs.cpp \
		common/FsDateTime.cpp \
		common/FsUtf.cpp \
		common/FsName.cpp \
		common/upcase.cpp \
	) \
	$(abspath $(LIBRARIES_PATH)/SDFS/src/SDFS.cpp) \
	$(abspath $(LIBRARIES_PATH)/SD/src/SD.cpp) \

CORE_C_FILES := \
	$(addprefix $(abspath $(CORE_PATH))/,\
		../../libraries/LittleFS/src/lfs.c \
		../../libraries/LittleFS/src/lfs_util.c \
	)

MOCK_CPP_FILES_COMMON := \
	$(addprefix $(abspath $(HOST_COMMON_ABSPATH))/,\
		Arduino.cpp \
		flash_hal_mock.cpp \
		spiffs_mock.cpp \
		littlefs_mock.cpp \
		sdfs_mock.cpp \
		WMath.cpp \
		MockUART.cpp \
		MockTools.cpp \
		MocklwIP.cpp \
		MockDigital.cpp \
	)

MOCK_CPP_FILES := $(MOCK_CPP_FILES_COMMON) \
	$(addprefix $(HOST_COMMON_ABSPATH)/,\
		ArduinoCatch.cpp \
	)

MOCK_CPP_FILES_EMU := $(MOCK_CPP_FILES_COMMON) \
	$(addprefix $(HOST_COMMON_ABSPATH)/,\
		ArduinoMain.cpp \
		ArduinoMainUdp.cpp \
		ArduinoMainSpiffs.cpp \
		ArduinoMainLittlefs.cpp \
		user_interface.cpp \
	)

MOCK_C_FILES := \
	$(addprefix $(HOST_COMMON_ABSPATH)/,\
		md5.c \
		noniso.c \
	)

INC_PATHS += \
	$(addprefix -I, \
		. \
		$(common) \
		$(CORE_PATH) \
	)

INC_PATHS += \
	$(addprefix -I,\
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
	core/test_Print.cpp \
	core/test_Updater.cpp

PREINCLUDES := \
	-include $(common)/mock.h \
	-include $(common)/c_types.h \

ifneq ($(D),)
OPTZ=-O0
DEBUG += -DDEBUG_ESP_PORT=Serial
DEBUG += -DDEBUG_ESP_SSL -DDEBUG_ESP_TLS_MEM -DDEBUG_ESP_HTTP_CLIENT -DDEBUG_ESP_HTTP_SERVER -DDEBUG_ESP_CORE -DDEBUG_ESP_WIFI -DDEBUG_ESP_HTTP_UPDATE -DDEBUG_ESP_UPDATER -DDEBUG_ESP_OTA -DDEBUG_ESP_MDNS
endif

FLAGS += $(DEBUG) -Wall $(OPTZ) -fno-common -g $(M32)
FLAGS += -fstack-protector-all
FLAGS += -DHTTPCLIENT_1_1_COMPATIBLE=0
FLAGS += -DLWIP_IPV6=0
FLAGS += -DHOST_MOCK=1
FLAGS += -DNONOSDK221=1
FLAGS += -DF_CPU=80000000
FLAGS += $(MKFLAGS)
FLAGS += -Wimplicit-fallthrough=2 # allow "// fall through" comments to stop spurious warnings
FLAGS += $(USERCFLAGS)
CXXFLAGS += -fno-rtti $(FLAGS) -funsigned-char
CFLAGS += $(FLAGS) -funsigned-char
LDFLAGS += $(OPTZ) -g $(M32)
LDFLAGS += $(USERLDFLAGS)
VALGRINDFLAGS += --leak-check=full --track-origins=yes --error-limit=no --show-leak-kinds=all --error-exitcode=999
CXXFLAGS += -Wno-error=format-security # cores/esp8266/Print.cpp:42:24:   error: format not a string literal and no format arguments [-Werror=format-security] -- (os_printf_plus(not_the_best_way))
#CXXFLAGS += -Wno-format-security      # cores/esp8266/Print.cpp:42:40: warning: format not a string literal and no format arguments [-Wformat-security] -- (os_printf_plus(not_the_best_way))

remduplicates = $(strip $(if $1,$(firstword $1) $(call remduplicates,$(filter-out $(firstword $1),$1))))

C_SOURCE_FILES = $(MOCK_C_FILES) $(CORE_C_FILES)
CPP_SOURCE_FILES = $(MOCK_CPP_FILES) $(CORE_CPP_FILES) $(TEST_CPP_FILES)
C_OBJECTS = $(C_SOURCE_FILES:.c=.c.o)

CPP_OBJECTS_CORE = $(MOCK_CPP_FILES:.cpp=.cpp.o) $(CORE_CPP_FILES:.cpp=.cpp.o)
CPP_OBJECTS_TESTS = $(TEST_CPP_FILES:.cpp=.cpp.o)

CPP_OBJECTS = $(CPP_OBJECTS_CORE) $(CPP_OBJECTS_TESTS)

OBJECTS = $(C_OBJECTS) $(CPP_OBJECTS)
COVERAGE_FILES = $(OBJECTS:.o=.gc*)

.PHONY: all
all: help

.PHONY: CI
CI:					# run CI
	$(MAKE) -f $(MAKEFILE) MKFLAGS="-Werror --coverage" LDFLAGS="--coverage" FORCE32=0 OPTZ=-O0 doCI

.PHONY: doCI
doCI: build-info $(OUTPUT_BINARY) valgrind test gcov

test: $(OUTPUT_BINARY)			# run host test for CI
	$(OUTPUT_BINARY)

.PHONY: clean
clean: clean-lcov clean-objects

.PHONY: clean-lcov
clean-lcov:
	rm -rf $(LCOV_DIRECTORY)

.PHONY: clean-objects
clean-objects:
	rm -rf bin bin32

.PHONY: test
gcov: test				# run coverage for CI
	( mkdir -p $(BINDIR)/gcov; cd $(BINDIR)/gcov; find . -name "*.gcno" -exec $(GCOV) -s ../.. -r -pb {} + )

.PHONY: valgrind
valgrind: $(OUTPUT_BINARY)
	mkdir -p $(LCOV_DIRECTORY)
	$(LCOV) --directory $(BINDIR) --zerocounters
	( cd $(LCOV_DIRECTORY); $(VALGRIND) $(VALGRINDFLAGS) $(OUTPUT_BINARY) )
	$(LCOV) --directory $(BINDIR) --capture --output-file $(LCOV_DIRECTORY)/app.info
	-$(GENHTML) $(LCOV_DIRECTORY)/app.info -o $(LCOV_DIRECTORY)

.PHONY: build-info
build-info:				# show toolchain version
	@echo "-------- build tools info --------"
	@echo "CC: " $(CC)
	$(CC) -v
	@echo "CXX: " $(CXX)
	$(CXX) -v
	@echo "CFLAGS: " $(CFLAGS)
	@echo "CXXFLAGS: " $(CXXFLAGS)
	@echo "----------------------------------"

include $(shell find $(BINDIR) -name "*.d" -print)

.SUFFIXES:

.PRECIOUS: %.c.o

$(BINDIR)/%.c.o: %.c
	@mkdir -p $(dir $@)
	$(VERBC) $(CC) $(PREINCLUDES) $(CFLAGS) $(INC_PATHS) -MD -MF $@.d -c -o $@ $<

%.c.o: %.c
	$(VERBC) $(CC) $(PREINCLUDES) $(CFLAGS) $(INC_PATHS) -MD -MF $@.d -c -o $@ $<

.PRECIOUS: %.cpp.o

$(BINDIR)/%.cpp.o: %.cpp
	@mkdir -p $(dir $@)
	$(VERBCXX) $(CXX) $(PREINCLUDES) $(CXXFLAGS) $(INC_PATHS) -MD -MF $@.d -c -o $@ $<

%.cpp.o: %.cpp
	$(VERBCXX) $(CXX) $(PREINCLUDES) $(CXXFLAGS) $(INC_PATHS) -MD -MF $@.d -c -o $@ $<

$(BINDIR)/core.a: $(C_OBJECTS:%=$(BINDIR)/%) $(CPP_OBJECTS_CORE:%=$(BINDIR)/%)
	$(AR) rc $@ $^
	$(RANLIB) $@

$(OUTPUT_BINARY): $(CPP_OBJECTS_TESTS:%=$(BINDIR)/%) $(BINDIR)/core.a
	$(VERBLD) $(CXX) $(DEFSYM_FS) $(LDFLAGS) $^ -o $@

#################################################
# building ino sources

ARDUINO_LIBS := \
	$(addprefix $(CORE_PATH)/,\
		IPAddress.cpp \
		Updater.cpp \
		base64.cpp \
		LwipIntf.cpp \
		LwipIntfCB.cpp \
		debug.cpp \
	) \
	$(addprefix $(abspath ../../libraries/ESP8266WiFi/src)/,\
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

OPT_ARDUINO_LIBS ?= \
	$(addprefix $(abspath ../../libraries)/,\
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
		Hash/src/Hash.cpp \
	)

MOCK_ARDUINO_LIBS := \
    $(addprefix $(HOST_COMMON_ABSPATH)/,\
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
USERLIBDIRS = $(shell test -z "$(ULIBPATHS)" || for d in $(ULIBPATHS); do for dd in $$d $$d/src $$d/src/libmad; do test -d $$dd && { echo -I$$dd; echo "userlib: using directory '$$dd'" 1>&2; } done; done)
USERLIBSRCS = $(shell test -z "$(ULIBPATHS)" || for d in $(ULIBPATHS); do for ss in $$d/*.cpp $$d/src/*.cpp $$d/src/libmad/*.c; do test -r $$ss && echo $$ss; done; done)
INC_PATHS += $(USERLIBDIRS)
INC_PATHS += -I$(INODIR)/..
CPP_OBJECTS_CORE_EMU = $(CPP_SOURCES_CORE_EMU:.cpp=.cpp.o) $(USERLIBSRCS:.cpp=.cpp.o) $(USERCXXSOURCES:.cpp=.cpp.o)
C_OBJECTS_CORE_EMU = $(USERCSOURCES:.c=.c.o)

FULLCORE_OBJECTS = $(C_OBJECTS) $(CPP_OBJECTS_CORE_EMU) $(C_OBJECTS_CORE_EMU)
FULLCORE_OBJECTS_ISOLATED = $(FULLCORE_OBJECTS:%.o=$(BINDIR)/%.o)

$(BINDIR)/fullcore.a: $(FULLCORE_OBJECTS_ISOLATED)
	$(VERBAR) $(AR) rc $@ $^
	$(VERBRANLIB) $(RANLIB) $@

ifeq ($(INO),)

%:
	make INO=$@.ino $(BINDIR)/$(abspath $@)

else

%: %.ino.cpp.o $(BINDIR)/fullcore.a FORCE
	$(VERBLD) $(CXX) $(LDFLAGS) $< $(BINDIR)/fullcore.a $(LIBSSL) -o $@
	mkdir -p $(BINDIR)/$(lastword $(subst /, ,$@))
	ln -sf $@ $(BINDIR)/$(lastword $(subst /, ,$@))
	@echo "----> $(BINDIR)/$(lastword $(subst /, ,$@))/$(lastword $(subst /, ,$@)) <----"
	@[ "$(R)" = noexec ] && echo '(not running it, use `make R="[<options>]" ...` for valgrind+gdb)' || $(dir $(MAKEFILE))/valgdb $@ $(R)

FORCE:

endif

$(BINDIR)/$(abspath $(INO)).cpp: $(INO)
	@# arduino builder would come around here - .ino -> .ino.cpp
	@mkdir -p $(dir $@); \
	( \
		for i in $(dir $<)/*.ino; do \
			echo "#include \"$$i\""; \
		done; \
	) > $@
	
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
	
