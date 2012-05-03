#
#  Copyright (c) 2012 Arduino.  All right reserved.
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License, or (at your option) any later version.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
#  See the GNU Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
#

ifeq ("$(VARIANTS)", "")
VARIANTS = arduino_due_x sam3x_ek adk2
#VARIANTS = sam3u_ek sam3s_ek arduino_due_u
endif

SUBMAKE_OPTIONS=--no-builtin-rules --no-builtin-variables --no-print-directory

#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------

all: $(VARIANTS)

.PHONY: sam3s_ek
sam3s_ek:
	@echo ------------------------------------------------------------------------------------
	@echo --- Making sam3s_ek
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_sam3s_ek.mk
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_sam3s_ek.mk
	@echo ------------------------------------------------------------------------------------

.PHONY: sam3u_ek
sam3u_ek:
	@echo ------------------------------------------------------------------------------------
	@echo --- Making sam3u_ek
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_sam3u_ek.mk
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_sam3u_ek.mk
	@echo ------------------------------------------------------------------------------------

.PHONY: sam3x_ek
sam3x_ek:
	@echo ------------------------------------------------------------------------------------
	@echo --- Making sam3x_ek
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_sam3x_ek.mk
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_sam3x_ek.mk
	@echo ------------------------------------------------------------------------------------

.PHONY: arduino_due_u
arduino_due_u:
	@echo ------------------------------------------------------------------------------------
	@echo --- Making arduino_due_u
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_arduino_due_u.mk
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_arduino_due_u.mk
	@echo ------------------------------------------------------------------------------------

.PHONY: arduino_due_x
arduino_due_x:
	@echo ------------------------------------------------------------------------------------
	@echo --- Making arduino_due_x
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_arduino_due_x.mk
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_arduino_due_x.mk
	@echo ------------------------------------------------------------------------------------

.PHONY: adk2
adk2:
	@echo ------------------------------------------------------------------------------------
	@echo --- Making adk2
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_adk2.mk
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_adk2.mk
	@echo ------------------------------------------------------------------------------------

.PHONY: clean
clean:
	@echo ------------------------------------------------------------------------------------
	@echo --- Cleaning sam3s_ek
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_sam3s_ek.mk $@
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_sam3s_ek.mk $@

	@echo --- Cleaning sam3u_ek
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_sam3u_ek.mk $@
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_sam3u_ek.mk $@

	@echo --- Cleaning sam3x_ek
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_sam3x_ek.mk $@
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_sam3x_ek.mk $@

	@echo --- Cleaning arduino_due_u
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_arduino_due_u.mk $@
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_arduino_due_u.mk $@

	@echo --- Cleaning arduino_due_x
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_arduino_due_x.mk $@
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_arduino_due_x.mk $@

	@echo --- Cleaning adk2
	@$(MAKE) DEBUG=1 $(SUBMAKE_OPTIONS) -f libarduino_adk2.mk $@
#	@$(MAKE) $(SUBMAKE_OPTIONS) -f libarduino_adk2.mk $@

	@echo ------------------------------------------------------------------------------------

