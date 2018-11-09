BINARY_DIRECTORY := bin
LCOV_DIRECTORY := lcov
OUTPUT_BINARY := $(BINARY_DIRECTORY)/host_tests
CORE_PATH := ../../cores/esp8266

# I wasn't able to build with clang when -coverage flag is enabled, forcing GCC on OS X
ifeq ($(shell uname -s),Darwin)
CC  ?= gcc
CXX ?= g++
endif
GCOV ?= gcov
VALGRIND ?= valgrind
LCOV ?= lcov
GENHTML ?= genhtml

CORE_CPP_FILES := $(addprefix $(CORE_PATH)/,\
	StreamString.cpp \
	Stream.cpp \
	WString.cpp \
	Print.cpp \
	FS.cpp \
	spiffs_api.cpp \
	pgmspace.cpp \
	MD5Builder.cpp \
)

CORE_C_FILES := $(addprefix $(CORE_PATH)/,\
	core_esp8266_noniso.c \
	spiffs/spiffs_cache.c \
	spiffs/spiffs_check.c \
	spiffs/spiffs_gc.c \
	spiffs/spiffs_hydrogen.c \
	spiffs/spiffs_nucleus.c \
)

MOCK_CPP_FILES_COMMON := $(addprefix common/,\
	Arduino.cpp \
	spiffs_mock.cpp \
	WMath.cpp \
	HardwareSerial.cpp \
)

MOCK_CPP_FILES := $(MOCK_CPP_FILES_COMMON) $(addprefix common/,\
	ArduinoCatch.cpp \
)

MOCK_CPP_FILES_EMU := $(MOCK_CPP_FILES_COMMON) $(addprefix common/,\
	ArduinoMain.cpp \
	user_interface.cpp \
)

MOCK_C_FILES := $(addprefix common/,\
	md5.c \
	noniso.c \
)

INC_PATHS := $(addprefix -I,\
	common \
	$(CORE_PATH) \
	$(shell echo ../../libraries/*/src) \
	$(shell echo ../../libraries/*) \
	../../tools/sdk/include \
)

INC_PATHS += -DLWIP_OPEN_SRC -DTCP_MSS=536 -DLWIP_FEATURES=1 \
	$(addprefix -I,\
		../../tools/sdk/lwip2/include \
	)

TEST_CPP_FILES := \
	fs/test_fs.cpp \
	core/test_pgmspace.cpp \
	core/test_md5builder.cpp \
	core/test_string.cpp

PREINCLUDES := \
	-include common/mock.h \
	-include common/c_types.h \

CXXFLAGS += -std=c++11 -Wall -Werror -coverage -O0 -fno-common -g
CFLAGS += -std=c99 -Wall -Werror -coverage -O0 -fno-common -g
LDFLAGS += -coverage -O0
VALGRINDFLAGS += --leak-check=full --track-origins=yes --error-limit=no --show-leak-kinds=all --error-exitcode=999

remduplicates = $(strip $(if $1,$(firstword $1) $(call remduplicates,$(filter-out $(firstword $1),$1))))

C_SOURCE_FILES = $(MOCK_C_FILES) $(CORE_C_FILES)
CPP_SOURCE_FILES = $(MOCK_CPP_FILES) $(CORE_CPP_FILES) $(TEST_CPP_FILES)
C_OBJECTS = $(C_SOURCE_FILES:.c=.c.o)

CPP_OBJECTS_CORE = $(MOCK_CPP_FILES:.cpp=.cpp.o) $(CORE_CPP_FILES:.cpp=.cpp.o)
CPP_OBJECTS_TESTS = $(TEST_CPP_FILES:.cpp=.cpp.o)

CPP_OBJECTS = $(CPP_OBJECTS_CORE) $(CPP_OBJECTS_TESTS)

OBJECTS = $(C_OBJECTS) $(CPP_OBJECTS)
COVERAGE_FILES = $(OBJECTS:.o=.gc*)

all: build-info $(OUTPUT_BINARY) valgrind test gcov

test: $(OUTPUT_BINARY)
	$(OUTPUT_BINARY)

clean: clean-objects clean-coverage
	rm -rf $(BINARY_DIRECTORY) examples

clean-objects:
	rm -rf $(OBJECTS)

clean-coverage:
	rm -rf $(COVERAGE_FILES) $(LCOV_DIRECTORY) *.gcov

gcov: test
	find $(CORE_PATH) -name "*.gcno" -exec $(GCOV) -r -pb {} +

valgrind: $(OUTPUT_BINARY)
	mkdir -p $(LCOV_DIRECTORY)
	$(LCOV) --directory ../../cores/esp8266/ --zerocounters
	$(VALGRIND) $(VALGRINDFLAGS) $(OUTPUT_BINARY)
	$(LCOV) --directory $(CORE_PATH) --capture --output-file $(LCOV_DIRECTORY)/app.info
	$(GENHTML) $(LCOV_DIRECTORY)/app.info -o $(LCOV_DIRECTORY)

build-info:
	@echo "-------- build tools info --------"
	@echo "CC: " $(CC)
	$(CC) -v
	@echo "CXX: " $(CXX)
	$(CXX) -v
	@echo "GCOV: " $(GCOV)
	$(GCOV) -v
	@echo "----------------------------------"

$(BINARY_DIRECTORY):
	mkdir -p $@

#$(C_OBJECTS): %.c.o: %.c
%.c.o: %.c
	$(CC) $(PREINCLUDES) $(CFLAGS) $(INC_PATHS) -c -o $@ $<

#$(CPP_OBJECTS): %.cpp.o: %.cpp
%.cpp.o: %.cpp
	$(CXX) $(PREINCLUDES) $(CXXFLAGS) $(INC_PATHS) -c -o $@ $<

$(BINARY_DIRECTORY)/core.a: $(C_OBJECTS) $(CPP_OBJECTS_CORE)
	ar -rcu $@ $(C_OBJECTS) $(CPP_OBJECTS_CORE)
	ranlib -c $@

$(OUTPUT_BINARY): $(BINARY_DIRECTORY) $(CPP_OBJECTS_TESTS) $(BINARY_DIRECTORY)/core.a
	$(CXX) $(LDFLAGS) $(CPP_OBJECTS_TESTS) $(BINARY_DIRECTORY)/core.a $(LIBS) -o $(OUTPUT_BINARY)

#################################################
# building ino sources
-include examples/Makefile.inc.ino

MOCK_ARDUINO_LIBS := $(addprefix ../../libraries/ESP8266WiFi/src/,\
	ESP8266WiFi.cpp \
	ESP8266WiFiAP.cpp \
	ESP8266WiFiGeneric.cpp \
	ESP8266WiFiMulti.cpp \
	ESP8266WiFiSTA-WPS.cpp \
	ESP8266WiFiSTA.cpp \
	ESP8266WiFiScan.cpp \
	WiFiServer.cpp \
	WiFiClient.cpp \
)
#	WiFiUdp.cpp \

MOCK_ARDUINO_LIBS += $(addprefix ../../libraries/ESP8266WiFi/src/,\
)

MOCK_ARDUINO_LIBS += $(addprefix $(CORE_PATH)/,\
	IPAddress.cpp \
)

CPP_OBJECTS_CORE_EMU_SOURCE = \
	$(MOCK_CPP_FILES_EMU) \
	$(CORE_CPP_FILES) \
	$(MOCK_ARDUINO_LIBS) \

CPP_OBJECTS_CORE_EMU = $(CPP_OBJECTS_CORE_EMU_SOURCE:.cpp=.cpp.o)

common/fullcore.a: $(C_OBJECTS) $(CPP_OBJECTS_CORE_EMU)
	ar -rcu $@ $^
	ranlib -c $@

%.ino-host: %.ino-host.o common/fullcore.a
	$(CXX) $(LDFLAGS) $^ -o $@

%.ino-host.o: %.ino-host.cpp
	$(CXX) $(PREINCLUDES) $(CXXFLAGS) $(INC_PATHS) -c -o $@ $<

.PRECIOUS: %-host.cpp
%.ino-host.cpp: %.ino
	cp $< $@

#################################################
# Makefile.inc.ino generation

.PHONY: examples

# default "examples" rule (overridden in Makefile.inc.ino when built)
examples:
	# copy examples locally
	rm -f Makefile.inc.ino
	all=""; \
	for dir in ../../libraries/*/$@; do \
		exampledir=$${dir%/*}; \
		exampledirname=$${exampledir##*/}; \
		for subdir in $$dir/*; do \
			exname=$${subdir##*/}; \
			mkdir -p $@/$$exampledirname/$$exname; \
			(cd $$subdir && tar cf - .) | (cd $@/$$exampledirname/$$exname; tar xf -); \
			echo $@/$$exampledirname/$$exname/$$exname-host: $@/$$exampledirname/$$exname/$$exname.ino >> $@/Makefile.inc.ino; \
			all="$$all $@/$$exampledirname/$$exname/$$exname.ino-host"; \
		done; \
	done; \
	echo "$@: $$all" >> $@/Makefile.inc.ino
	$(MAKE) $@
