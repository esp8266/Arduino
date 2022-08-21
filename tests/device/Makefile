SHELL := /bin/bash
V ?= 0
ESP8266_CORE_PATH ?= $(realpath ../..)
BUILD_DIR ?= $(PWD)/.build
HARDWARE_DIR ?= $(PWD)/.hardware
PYTHON ?= python3
ESPTOOL ?= $(PYTHON) $(ESP8266_CORE_PATH)/tools/esptool/esptool.py
MKSPIFFS ?= $(ESP8266_CORE_PATH)/tools/mkspiffs/mkspiffs
UPLOAD_PORT ?= $(shell ls /dev/tty* | grep -m 1 -i USB)
UPLOAD_BAUD ?= 460800
UPLOAD_BOARD ?= nodemcu
BS_DIR ?= libraries/BSTest
DEBUG_LEVEL ?= lvl=None____
FQBN ?= esp8266com:esp8266:generic:xtal=160,FlashFreq=40,FlashMode=dio,baud=115200,eesz=4M1M,ip=lm2f,ResetMethod=nodemcu,dbg=Serial,$(DEBUG_LEVEL)
BUILD_TOOL := $(ARDUINO_IDE_PATH)/arduino-builder
TEST_CONFIG := test_env.cfg
TEST_REPORT_XML := test_report.xml
TEST_REPORT_HTML := test_report.html

ifeq ("$(MOCK)", "1")
# To enable a test for mock testing, just rename dir+files to '*_sw_*'
TEST_LIST ?= $(wildcard test_sw_*/*.ino)
else
TEST_LIST ?= $(wildcard test_*/*.ino)
endif

ifneq ("$(V)","1")
	SILENT = @
	REDIR = >& /dev/null
else
	BUILDER_DEBUG_FLAG = -verbose
	RUNNER_DEBUG_FLAG = -d
	#UPLOAD_VERBOSE_FLAG = -v
endif

help:
	@echo
	@echo 'make list                    - show list of tests'
	@echo 'make sometest/sometest.ino   - run one test'
	@echo 'make all                     - run all tests'
	@echo 'make MOCK=1 all              - run all emulation-on-host compatible tests'
	@echo 'variables needed: $$ARDUINO_IDE_PATH $$ESP8266_CORE_PATH'
	@echo 'make options: V=1 NO_BUILD=1 NO_UPLOAD=1 NO_RUN=1 MOCK=1'
	@echo

list: showtestlist

all: count tests test_report

$(TEST_LIST): | virtualenv $(TEST_CONFIG) $(BUILD_DIR) $(HARDWARE_DIR)

tests: showtestlist $(TEST_LIST)

showtestlist:
	@echo "-------------------------------- test list:"
	@echo $(TEST_LIST)
	@echo "--------------------------------"

$(TEST_LIST): LOCAL_BUILD_DIR=$(BUILD_DIR)/$(notdir $@)

$(TEST_LIST):
	@echo "--------------------------------"
	@echo "Running test '$@' of $(words $(TEST_LIST)) tests"
	$(SILENT)mkdir -p $(LOCAL_BUILD_DIR)
ifeq ("$(MOCK)", "1")
	@echo Compiling $(notdir $@)
	(cd ../host; make D=$(V) ULIBDIRS=../device/libraries/BSTest ../device/$(@:%.ino=%))
	$(SILENT)$(BS_DIR)/virtualenv/bin/python \
		$(BS_DIR)/runner.py \
			$(RUNNER_DEBUG_FLAG) \
			-e "$(ESP8266_CORE_PATH)/tests/host/bin/$(@:%.ino=%)" \
			-n $(basename $(notdir $@)) \
			-o $(LOCAL_BUILD_DIR)/test_result.xml \
			--env-file $(TEST_CONFIG) \
			`test -f $(addsuffix .py, $(basename $@)) && echo "-m $(addsuffix .py, $(basename $@))" || echo ""`
else
ifneq ("$(NO_BUILD)","1")
	@test -n "$(ARDUINO_IDE_PATH)" || (echo "Please export ARDUINO_IDE_PATH" && exit 1)
	@echo Compiling $(notdir $@)
	@rm -f $(LOCAL_BUILD_DIR)/build.options.json
	$(SILENT)$(BUILD_TOOL) -compile -logger=human \
		-libraries "$(PWD)/libraries" \
		-core-api-version="10608" \
		-warnings=all \
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
	@test -e $(dir $@)/make_spiffs.py && ( \
		echo "Generating and uploading SPIFFS" && \
		(cd $(dir $@) && $(PYTHON) ./make_spiffs.py $(REDIR) ) && \
		$(MKSPIFFS) --create $(dir $@)data/ --size 0xFB000 \
			--block 8192 --page 256 $(LOCAL_BUILD_DIR)/spiffs.img $(REDIR) && \
		$(ESPTOOL) $(UPLOAD_VERBOSE_FLAG) \
			--chip esp8266 \
			--port $(UPLOAD_PORT) \
			--baud $(UPLOAD_BAUD) \
			--after no_reset \
			write_flash 0x300000 $(LOCAL_BUILD_DIR)/spiffs.img $(REDIR) ) \
			|| (echo "No SPIFFS to upload")
	@echo Uploading binary
	$(SILENT)$(ESPTOOL) $(UPLOAD_VERBOSE_FLAG)  \
		--chip esp8266 \
		--port $(UPLOAD_PORT) \
		--baud $(UPLOAD_BAUD) \
		--after no_reset \
		write_flash 0x0 $(LOCAL_BUILD_DIR)/$(notdir $@).bin $(REDIR) # no reset
endif
ifneq ("$(NO_RUN)","1")
	@test -n "$(UPLOAD_PORT)" || (echo "Failed to detect upload port, please export UPLOAD_PORT manually" && exit 1)
	@echo Running tests
	$(SILENT)$(ESPTOOL) $(UPLOAD_VERBOSE_FLAG)  \
		--chip esp8266 \
		--port $(UPLOAD_PORT) \
		--baud $(UPLOAD_BAUD) \
		read_flash_status $(REDIR) # reset
	$(SILENT)$(BS_DIR)/virtualenv/bin/python \
		$(BS_DIR)/runner.py \
			$(RUNNER_DEBUG_FLAG) \
			-p $(UPLOAD_PORT) \
			-n $(basename $(notdir $@)) \
			-o $(LOCAL_BUILD_DIR)/test_result.xml \
			--env-file $(TEST_CONFIG) \
			`test -f $(addsuffix .py, $(basename $@)) && echo "-m $(addsuffix .py, $(basename $@))" || echo ""`
endif
endif

$(TEST_REPORT_XML): $(HARDWARE_DIR) virtualenv
	$(SILENT)$(BS_DIR)/xunitmerge $(shell find $(BUILD_DIR) -name 'test_result.xml' | xargs echo) $(TEST_REPORT_XML)

$(TEST_REPORT_HTML): $(TEST_REPORT_XML) | virtualenv
	$(SILENT)$(BS_DIR)/virtualenv/bin/junit2html $< $@

test_report: $(TEST_REPORT_HTML)
	@echo "Test report generated in $(TEST_REPORT_HTML)"

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(HARDWARE_DIR):
	mkdir -p $(HARDWARE_DIR)/esp8266com
	cd $(HARDWARE_DIR)/esp8266com && ln -s $(realpath $(ESP8266_CORE_PATH)) esp8266

virtualenv:
	@make -C $(BS_DIR) PYTHON=$(PYTHON) virtualenv

clean:
	rm -rf $(BUILD_DIR)
	rm -rf $(HARDWARE_DIR)
	rm -rf $(BS_DIR)/virtualenv
	rm -f $(TEST_REPORT_HTML) $(TEST_REPORT_XML)

distclean: clean
	rm -rf libraries/BSTest/virtualenv/
	find . -name "*pyc" -exec rm -f {} \;

$(TEST_CONFIG):
	@echo "******    "
	@echo "******    $(TEST_CONFIG) does not exist"
	@echo "******    Create one from $(TEST_CONFIG).template"
	@echo "******    "
	@false

.PHONY: tests all count virtualenv test_report $(TEST_LIST)
