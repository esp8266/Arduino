SHELL := /bin/bash

ESP8266_CORE_PATH ?= $(shell git rev-parse --show-toplevel)

# arduino-cli core/ & sketch/ build and cache directories
# by default, share build location with every other sketch
CACHE_DIR ?= $(HOME)/.cache/arduino
ifneq ("$(findstring $(ESP8266_CORE_PATH),$(CACHE_DIR))", "")
$(warning "CACHE_DIR is located in ESP8266_CORE_PATH, core.a caching will be disabled")
endif

# binaries (compile --output-dir=...) & test output goes in here.
BUILD_DIR ?= $(PWD)/build

# where to look for BSTest scripts
BS_DIR ?= $(PWD)/libraries/BSTest

PYTHON ?= python3
ESPTOOL ?= $(PYTHON) $(ESP8266_CORE_PATH)/tools/esptool/esptool.py

VENV_PYTHON ?= $(BS_DIR)/virtualenv/bin/python
VENV_JUNIT2HTML ?= $(BS_DIR)/virtualenv/bin/junit2html

MKFS ?= $(ESP8266_CORE_PATH)/tools/mklittlefs/mklittlefs

UPLOAD_PORT ?= $(shell ls /dev/tty* | grep -m 1 -i USB)
UPLOAD_BAUD ?= 460800

TEST_BAUD ?= 115200
BUILD_TOOL ?= arduino-cli

BUILD_BOARD ?= generic
BUILD_CPU ?= 160
BUILD_SIZE ?= 4M1M
BUILD_LWIP ?= lm2f
BUILD_EXTRA ?= ,dbg=Serial,lvl=CORE

FQBN ?= esp8266com:esp8266:$(BUILD_BOARD):xtal=$(BUILD_CPU),baud=$(TEST_BAUD),eesz=$(BUILD_SIZE),ip=$(BUILD_LWIP)$(BUILD_EXTRA)

TEST_CONFIG := test_env.cfg
TEST_RESULT_XML := test_result.xml
TEST_REPORT_XML := test_report.xml
TEST_REPORT_HTML := test_report.html

TEST_LIST ?= $(wildcard test_*/*.ino)
BUILD_FLAGS ?=
BS_FLAGS ?=

# To enable a test for mock testing, just rename dir+files to '*_sw_*'
ifeq ("$(MOCK)", "1")
	TEST_LIST := $(filter test_sw_%.ino,$(TEST_LIST))
	NO_UPLOAD := 1
	NO_RUN := 1
endif

# To enable verbose mode, call `make V=1` ...
V ?= 0
ifeq ("$(V)", "1")
	BUILD_FLAGS += --verbose
	BS_FLAGS += --debug
endif

# ${sketch}.py helper script when building locally
mock_script = \
	`test -f $(addsuffix .py, $(basename $(1))) && echo "--mock $(addsuffix .py, $(basename $(1)))" || echo ""`

help:
	@echo
	@echo 'make list                    - show list of tests'
	@echo 'make sometest/sometest.ino   - run one test'
	@echo 'make all                     - run all tests'
	@echo 'make MOCK=1 all              - run all emulation-on-host compatible tests'
	@echo 'make options: V=1 NO_BUILD=1 NO_UPLOAD=1 NO_RUN=1 MOCK=1'
	@echo

list: showtestlist

all: count tests test_report

$(TEST_LIST): | virtualenv $(TEST_CONFIG) $(CACHE_DIR) $(BUILD_DIR) $(HARDWARE_DIR)

.NOTPARALLEL: $(TEST_LIST)

tests: showtestlist $(TEST_LIST)

showtestlist:
	@echo "-------------------------------- test list:"
	@printf '%s\n' $(TEST_LIST)
	@echo "--------------------------------"

$(TEST_LIST): LOCAL_BUILD_DIR=$(BUILD_DIR)/$(notdir $@)/
$(TEST_LIST): LOCAL_TEST_RESULT_XML=$(LOCAL_BUILD_DIR)/$(TEST_RESULT_XML)
$(TEST_LIST): LOCAL_DATA_DIR=$(LOCAL_BUILD_DIR)/data/
$(TEST_LIST): LOCAL_DATA_IMG=$(LOCAL_BUILD_DIR)/data.img

define build-arduino
	export ARDUINO_BUILD_CACHE_PATH="$(CACHE_DIR)"; \
		$(BUILD_TOOL) config dump; \
		$(BUILD_TOOL) compile \
			$(BUILD_FLAGS) \
			--libraries "$(PWD)/libraries" \
			--output-dir "$(LOCAL_BUILD_DIR)" \
			--warnings=all \
			--fqbn=$(FQBN) \
			$@
endef

define build-mock
	(cd $(ESP8266_CORE_PATH)/test/host; \
		$(MAKE) D=$(V) ULIBDIRS=$(PWD)/libraries/BSTest $(PWD)/$(@:%.ino=%))
	$(VENV_PYTHON) $(BS_DIR)/runner.py \
		$(BS_FLAGS) \
		--name $(basename $(notdir $@)) \
		--output $(LOCAL_TEST_RESULT_XML) \
		--env-file $(TEST_CONFIG) \
		$(call mock_script,$@) \
		executable "$(ESP8266_CORE_PATH)/tests/host/bin/$(@:%.ino=%)" || echo ""`
endef

define upload-data
	@test -n "$(UPLOAD_PORT)" \
		|| (echo "Failed to detect upload port, please export UPLOAD_PORT manually" && exit 1)
	@test ! \( -d $(dir $@)/data/ \) -a \( -e $(dir $@)/make_data.py \) && \
		(cd $(dir $@) && ./make_data.py ) || echo "Filesystem creation skipped"
	@test -d $(dir $@)/data/ && ( \
		$(MKFS) \
			--create $(LOCAL_DATA_DIR) \
			--size 0xFB000 \
			--block 8192 \
			--page 256 \
			$(LOCAL_DATA_IMG) && \
		$(ESPTOOL) \
			--chip esp8266 \
			--port $(UPLOAD_PORT) \
			--baud $(UPLOAD_BAUD) \
			--after no_reset \
			write_flash 0x300000 $(LOCAL_DATA_IMG) ) \
		&& (echo "Uploaded filesystem") \
		|| (echo "Filesystem upload skipped")
endef

define upload-binary
	@test -n "$(UPLOAD_PORT)" \
		|| (echo "Failed to detect upload port, please export UPLOAD_PORT manually" && exit 1)
	$(ESPTOOL) \
		--chip esp8266 \
		--port $(UPLOAD_PORT) \
		--baud $(UPLOAD_BAUD) \
		--after no_reset \
		write_flash 0x0 $(LOCAL_BUILD_DIR)/$(notdir $@).bin
endef

define run-test
	@test -n "$(UPLOAD_PORT)" \
		|| (echo "Failed to detect upload port, please export UPLOAD_PORT manually" && exit 1)
	@echo Running tests
	$(ESPTOOL) \
		--chip esp8266 \
		--port $(UPLOAD_PORT) \
		--baud $(UPLOAD_BAUD) \
		read_flash_status # reset via implicit stub reboot
	$(VENV_PYTHON) $(BS_DIR)/runner.py \
		$(BS_FLAGS) \
		--name $(basename $(notdir $@)) \
		--output $(LOCAL_TEST_RESULT_XML) \
		--env-file $(TEST_CONFIG) \
		$(call mock_script,$@) \
		port $(UPLOAD_PORT) \
		--baudrate $(TEST_BAUD)
endef

$(TEST_LIST):
	@echo "--------------------------------"
	@echo "Running test '$@' of $(words $(TEST_LIST)) tests"
	mkdir -p $(LOCAL_BUILD_DIR)
	mkdir -p $(CACHE_DIR)
ifneq ("$(NO_BUILD)","1")
	@echo Building $(notdir $@)
ifeq ("$(MOCK)", "1")
	$(build-mock)
else
	$(build-arduino)
endif
endif
ifneq ("$(NO_UPLOAD)","1")
	$(upload-filesystem)
	$(upload-binary)
endif
ifneq ("$(NO_RUN)","1")
	$(run-test)
endif

$(TEST_REPORT_XML): virtualenv
	$(BS_DIR)/xunitmerge \
		$(shell find $(BUILD_DIR) -name '$(TEST_RESULT_XML)' | xargs echo) \
		$(TEST_REPORT_XML)

$(TEST_REPORT_HTML): $(TEST_REPORT_XML) | virtualenv
	$(VENV_JUNIT2HTML) $< $@

test_report: $(TEST_REPORT_HTML)
	@echo "Test report generated in $(TEST_REPORT_HTML)"

$(CACHE_DIR):
	@mkdir -p $@

$(BUILD_DIR):
	@mkdir -p $@

virtualenv:
	@make -C $(BS_DIR) PYTHON=$(PYTHON) virtualenv

clean:
	rm -rf $(CACHE_DIR)
	rm -rf $(BUILD_DIR)
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
