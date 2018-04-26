SHELL := /bin/bash
V ?= 0
TEST_LIST ?= $(wildcard test_*/*.ino)
ESP8266_CORE_PATH ?= ../..
BUILD_DIR ?= $(PWD)/.build
HARDWARE_DIR ?= $(PWD)/.hardware
ESPTOOL ?= $(ESP8266_CORE_PATH)/tools/esptool/esptool
UPLOAD_PORT ?= $(shell ls /dev/tty* | grep -m 1 -i USB)
UPLOAD_BAUD ?= 921600
UPLOAD_BOARD ?= nodemcu
BS_DIR ?= libraries/BSTest
DEBUG_LEVEL ?= DebugLevel=None____
FQBN ?= esp8266com:esp8266:generic:CpuFrequency=80,FlashFreq=40,FlashMode=dio,UploadSpeed=115200,FlashSize=4M1M,LwIPVariant=v2mss536,ResetMethod=none,Debug=Serial,$(DEBUG_LEVEL)
BUILD_TOOL := $(ARDUINO_IDE_PATH)/arduino-builder
TEST_CONFIG := test_env.cfg
TEST_REPORT_XML := test_report.xml
TEST_REPORT_HTML := test_report.html


ifneq ("$(V)","1")
	SILENT = @
else
	BUILDER_DEBUG_FLAG = -verbose
	RUNNER_DEBUG_FLAG = -d
	UPLOAD_VERBOSE_FLAG = -v
endif


all: count tests test_report

$(TEST_LIST): | virtualenv $(TEST_CONFIG) $(BUILD_DIR) $(HARDWARE_DIR)

tests: $(TEST_LIST)

$(TEST_LIST): LOCAL_BUILD_DIR=$(BUILD_DIR)/$(notdir $@)

$(TEST_LIST):
	@echo Running $(words $(TEST_LIST)) tests
	$(SILENT)mkdir -p $(LOCAL_BUILD_DIR)
ifneq ("$(NO_BUILD)","1")
	@test -n "$(ARDUINO_IDE_PATH)" || (echo "Please export ARDUINO_IDE_PATH" && exit 1)
	@echo Compiling $(notdir $@)
	$(SILENT)$(BUILD_TOOL) -compile -logger=human \
		-libraries "$(PWD)/libraries" \
		-core-api-version="10608" \
		-warnings=none \
		$(BUILDER_DEBUG_FLAG) \
		-build-path $(LOCAL_BUILD_DIR) \
		-tools $(ARDUINO_IDE_PATH)/tools-builder \
		-hardware $(HARDWARE_DIR)\
		-hardware $(ARDUINO_IDE_PATH)/hardware \
		-fqbn=$(FQBN) \
		$@
endif
ifneq ("$(NO_UPLOAD)","1")
	@test -n "$(UPLOAD_PORT)" || (echo "Failed to detect upload port, please export UPLOAD_PORT manually" && exit 1)
	@echo Uploading binary
	$(SILENT)$(ESPTOOL) $(UPLOAD_VERBOSE_FLAG)  \
		-cp $(UPLOAD_PORT) \
		-cb $(UPLOAD_BAUD) \
		-cd $(UPLOAD_BOARD) \
		-cf $(LOCAL_BUILD_DIR)/$(notdir $@).bin
endif
ifneq ("$(NO_RUN)","1")
	@test -n "$(UPLOAD_PORT)" || (echo "Failed to detect upload port, please export UPLOAD_PORT manually" && exit 1)
	@echo Running tests
	$(SILENT)$(ESPTOOL) $(UPLOAD_VERBOSE_FLAG) -cp $(UPLOAD_PORT) -cd $(UPLOAD_BOARD) -cr
	@source $(BS_DIR)/virtualenv/bin/activate && \
		python $(BS_DIR)/runner.py \
			$(RUNNER_DEBUG_FLAG) \
			-p $(UPLOAD_PORT) \
			-n $(basename $(notdir $@)) \
			-o $(LOCAL_BUILD_DIR)/test_result.xml \
			--env-file $(TEST_CONFIG) \
			`test -f $(addsuffix .py, $(basename $@)) && echo "-m $(addsuffix .py, $(basename $@))" || echo ""`
endif

$(TEST_REPORT_XML): $(HARDWARE_DIR) virtualenv
	@$(BS_DIR)/virtualenv/bin/xunitmerge $(shell find $(BUILD_DIR) -name 'test_result.xml' | xargs echo) $(TEST_REPORT_XML)

$(TEST_REPORT_HTML): $(TEST_REPORT_XML) | virtualenv
	@$(BS_DIR)/virtualenv/bin/junit2html $< $@

test_report: $(TEST_REPORT_HTML)
	@echo "Test report generated in $(TEST_REPORT_HTML)"

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(HARDWARE_DIR):
	mkdir -p $(HARDWARE_DIR)/esp8266com
	cd $(HARDWARE_DIR)/esp8266com && ln -s $(realpath $(ESP8266_CORE_PATH)) esp8266

virtualenv:
	@make -C $(BS_DIR) virtualenv

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(HARDWARE_DIR)
	rm -f $(TEST_REPORT_HTML) $(TEST_REPORT_XML)

$(TEST_CONFIG):
	@echo "******    "
	@echo "******    $(TEST_CONFIG) does not exist"
	@echo "******    Create one from $(TEST_CONFIG).template"
	@echo "******    "
	@false

.PHONY: tests all count virtualenv test_report $(TEST_LIST)
