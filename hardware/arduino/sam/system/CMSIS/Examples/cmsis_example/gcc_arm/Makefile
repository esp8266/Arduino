# ----------------------------------------------------------------------------
#         ATMEL Microcontroller Software Support 
# ----------------------------------------------------------------------------
# Copyright (c) 2010, Atmel Corporation
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following condition is met:
#
# - Redistributions of source code must retain the above copyright notice,
# this list of conditions and the disclaimer below.
#
# Atmel's name may not be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
# DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
# OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
# EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# ----------------------------------------------------------------------------

# make inner variables
.DEFAULT_GOAL := all

# custom variables
BUILD_NUMBER := 
BUILDS := 
CLEANS :=

# Build macro
define BUILD_SERIES

# output test number information
$(info Defining cmsis_example_$(1)_build and cmsis_example_$(1)_clean)

# add the incoming targets to global targets
BUILDS += cmsis_example_$(1)_build
CLEANS += cmsis_example_$(1)_clean
BUILD_NUMBER += x

.PHONY: cmsis_example_$(1)_build
cmsis_example_$(1)_build:
	@echo --- 
	@echo --- 
	@echo --- Making $(1)
	@echo --- 
	@$(MAKE) CHIP=$(1) -f cmsis_example.mk
	
.PHONY: cmsis_example_$(1)_clean
cmsis_example_$(1)_clean:
	@echo --- 
	@echo --- 
	@echo --- Cleaning $(1)
	@echo --- 
	@$(MAKE) CHIP=$(1) clean -f cmsis_example.mk
endef

# define SAM series
include sam_series.mk

$(foreach SERIES, $(SAM_SERIES), $(eval $(call BUILD_SERIES,$(SERIES))))

# output test number information
$(info Number of devices to be tested $(words $(BUILD_NUMBER)) /  $(words $(SAM_SERIES)))

all: $(BUILDS)

clean: $(CLEANS)
