BINARY_DIRECTORY := bin
OUTPUT_BINARY := $(BINARY_DIRECTORY)/host_tests
CORE_PATH := ../../cores/esp8266

# I wasn't able to build with clang when -coverage flag is enabled, forcing GCC on OS X
ifeq ($(shell uname -s),Darwin)
CC  := gcc
CXX := g++
endif
GCOV ?= gcov

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

MOCK_CPP_FILES := $(addprefix common/,\
	Arduino.cpp \
	spiffs_mock.cpp \
	WMath.cpp \
)

MOCK_C_FILES := $(addprefix common/,\
	md5.c \
)

INC_PATHS += $(addprefix -I, \
	common \
	$(CORE_PATH) \
)

TEST_CPP_FILES := \
	fs/test_fs.cpp \
	core/test_pgmspace.cpp \
	core/test_md5builder.cpp \


CXXFLAGS += -std=c++11 -Wall -coverage -O0 -fno-common
CFLAGS += -std=c99 -Wall -coverage -O0 -fno-common
LDFLAGS += -coverage -O0

remduplicates = $(strip $(if $1,$(firstword $1) $(call remduplicates,$(filter-out $(firstword $1),$1))))

C_SOURCE_FILES = $(MOCK_C_FILES) $(CORE_C_FILES)
CPP_SOURCE_FILES = $(MOCK_CPP_FILES) $(CORE_CPP_FILES) $(TEST_CPP_FILES)
C_OBJECTS = $(C_SOURCE_FILES:.c=.c.o)

CPP_OBJECTS_CORE = $(MOCK_CPP_FILES:.cpp=.cpp.o) $(CORE_CPP_FILES:.cpp=.cpp.o)
CPP_OBJECTS_TESTS = $(TEST_CPP_FILES:.cpp=.cpp.o)

CPP_OBJECTS = $(CPP_OBJECTS_CORE) $(CPP_OBJECTS_TESTS)

OBJECTS = $(C_OBJECTS) $(CPP_OBJECTS)
COVERAGE_FILES = $(OBJECTS:.o=.gc*)

all: build-info $(OUTPUT_BINARY) test gcov

test: $(OUTPUT_BINARY)
	$(OUTPUT_BINARY)

clean: clean-objects clean-coverage
	rm -rf $(BINARY_DIRECTORY)

clean-objects:
	rm -rf $(OBJECTS)

clean-coverage:
	rm -rf $(COVERAGE_FILES) *.gcov

gcov: test
	find $(CORE_PATH) -name "*.gcno" -exec $(GCOV) -r -pb {} +

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

$(C_OBJECTS): %.c.o: %.c
	$(CC) $(CFLAGS) $(INC_PATHS) -c -o $@ $<

$(CPP_OBJECTS): %.cpp.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INC_PATHS) -c -o $@ $<

$(BINARY_DIRECTORY)/core.a: $(C_OBJECTS) $(CPP_OBJECTS_CORE)
	ar -rcu $@ $(C_OBJECTS) $(CPP_OBJECTS_CORE)
	ranlib -c $@

$(OUTPUT_BINARY): $(BINARY_DIRECTORY) $(CPP_OBJECTS_TESTS) $(BINARY_DIRECTORY)/core.a
	$(CXX) $(LDFLAGS) $(CPP_OBJECTS_TESTS) $(BINARY_DIRECTORY)/core.a $(LIBS) -o $(OUTPUT_BINARY)
