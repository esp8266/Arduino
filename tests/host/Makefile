OBJECT_DIRECTORY := obj
BINARY_DIRECTORY := bin
OUTPUT_BINARY := $(BINARY_DIRECTORY)/host_tests
CORE_PATH := ../../cores/esp8266

CORE_CPP_FILES := $(addprefix $(CORE_PATH)/,\
	StreamString.cpp \
	Stream.cpp \
	WString.cpp \
	Print.cpp \
	FS.cpp \
	spiffs_api.cpp \
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

INC_PATHS += $(addprefix -I, \
	common \
	$(CORE_PATH) \
)

TEST_CPP_FILES := \
	fs/test_fs.cpp \

CXXFLAGS += -std=c++11 -Wall
CFLAGS += -std=c99 -Wall

remduplicates = $(strip $(if $1,$(firstword $1) $(call remduplicates,$(filter-out $(firstword $1),$1))))

C_SOURCE_FILES = $(CORE_C_FILES)
CPP_SOURCE_FILES = $(MOCK_CPP_FILES) $(CORE_CPP_FILES) $(TEST_CPP_FILES)
C_SOURCE_FILE_NAMES = $(notdir $(C_SOURCE_FILES))
C_PATHS = $(call remduplicates, $(dir $(C_SOURCE_FILES) ) )
C_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(C_SOURCE_FILE_NAMES:.c=.c.o) )

CPP_SOURCE_FILE_NAMES = $(notdir $(CPP_SOURCE_FILES))
CPP_PATHS = $(call remduplicates, $(dir $(CPP_SOURCE_FILES) ))
CPP_OBJECTS = $(addprefix $(OBJECT_DIRECTORY)/, $(CPP_SOURCE_FILE_NAMES:.cpp=.cpp.o) )

$(warning $(call remduplicates, $(dir $(CPP_SOURCE_FILES) )))
vpath %.c $(C_PATHS)
vpath %.cpp $(CPP_PATHS)

OBJECTS = $(C_OBJECTS) $(CPP_OBJECTS)

all: $(OUTPUT_BINARY) test

test: $(OUTPUT_BINARY)
	$(OUTPUT_BINARY)

$(OBJECT_DIRECTORY):
	mkdir -p $@

$(BINARY_DIRECTORY):
	mkdir -p $@

$(C_OBJECTS): $(OBJECT_DIRECTORY)/%.c.o: %.c
	$(CC) $(CFLAGS) $(INC_PATHS) -c -o $@ $<

$(CPP_OBJECTS): $(OBJECT_DIRECTORY)/%.cpp.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INC_PATHS) -c -o $@ $<


# Link
$(OUTPUT_BINARY): $(BINARY_DIRECTORY) $(OBJECT_DIRECTORY) $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $(OUTPUT_BINARY)
