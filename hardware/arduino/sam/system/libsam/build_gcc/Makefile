#
#  Copyright (c) 2011 Arduino.  All right reserved.
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

# Makefile for compiling libchip

SUBMAKE_OPTIONS=--no-builtin-rules --no-builtin-variables --no-print-directory

#-------------------------------------------------------------------------------
# Rules
#-------------------------------------------------------------------------------

# libsam_sam3s4c_gcc_rel.a libsam_sam3u4e_gcc_rel.a libsam_sam3x8e_gcc_rel.a libsam_sam3x8h_gcc_rel.a
all: libsam_sam3s4c_gcc_dbg.a libsam_sam3u4e_gcc_dbg.a libsam_sam3x8e_gcc_dbg.a libsam_sam3x8h_gcc_dbg.a

arduino_due_u:
	@echo ------------------------------------------------------------------------------------
	@echo --- Making $@
	@echo ---
	@$(MAKE) CHIP=__SAM3U4E__ $(SUBMAKE_OPTIONS) OUTPUT_BIN=../../../variants/arduino_due_u -f sam3.mk
	@echo ------------------------------------------------------------------------------------

arduino_due_x:
	@echo ------------------------------------------------------------------------------------
	@echo --- Making $@
	@echo ---
	@$(MAKE) CHIP=__SAM3X8E__ $(SUBMAKE_OPTIONS) OUTPUT_BIN=../../../variants/arduino_due_x -f sam3.mk
	@echo ------------------------------------------------------------------------------------

.PHONY: libsam_sam3s4c_gcc_dbg.a
libsam_sam3s4c_gcc_dbg.a:
	@echo ------------------------------------------------------------------------------------
	@echo --- Making $@
	@echo ---
	@$(MAKE) CHIP=__SAM3S4C__ DEBUG=1 $(SUBMAKE_OPTIONS) -f sam3.mk
	@echo ------------------------------------------------------------------------------------

.PHONY: libsam_sam3s4c_gcc_rel.a
libsam_sam3s4c_gcc_rel.a:
	@echo ------------------------------------------------------------------------------------
	@echo --- Making $@
	@echo ---
	@$(MAKE) CHIP=__SAM3S4C__ $(SUBMAKE_OPTIONS) -f sam3.mk
	@echo ------------------------------------------------------------------------------------

.PHONY: libsam_sam3u4e_gcc_dbg.a
libsam_sam3u4e_gcc_dbg.a:
	@echo ------------------------------------------------------------------------------------
	@echo --- Making $@
	@echo ---
	@$(MAKE) CHIP=__SAM3U4E__ DEBUG=1 $(SUBMAKE_OPTIONS) -f sam3.mk
	@echo ------------------------------------------------------------------------------------

.PHONY: libsam_sam3u4e_gcc_rel.a
libsam_sam3u4e_gcc_rel.a:
	@echo ------------------------------------------------------------------------------------
	@echo --- Making $@
	@echo ---
	@$(MAKE) CHIP=__SAM3U4E__ $(SUBMAKE_OPTIONS) -f sam3.mk
	@echo ------------------------------------------------------------------------------------

.PHONY: libsam_sam3x8e_gcc_dbg.a
libsam_sam3x8e_gcc_dbg.a:
	@echo ------------------------------------------------------------------------------------
	@echo --- Making $@
	@echo ---
	@$(MAKE) CHIP=__SAM3X8E__ DEBUG=1 $(SUBMAKE_OPTIONS) -f sam3.mk
	@echo ------------------------------------------------------------------------------------

.PHONY: libsam_sam3x8e_gcc_rel.a
libsam_sam3x8e_gcc_rel.a:
	@echo ------------------------------------------------------------------------------------
	@echo --- Making $@
	@echo ---
	@$(MAKE) CHIP=__SAM3X8E__ $(SUBMAKE_OPTIONS) -f sam3.mk
	@echo ------------------------------------------------------------------------------------

.PHONY: libsam_sam3x8h_gcc_dbg.a
libsam_sam3x8h_gcc_dbg.a:
	@echo ------------------------------------------------------------------------------------
	@echo --- Making $@
	@echo ---
	@$(MAKE) CHIP=__SAM3X8H__ DEBUG=1 $(SUBMAKE_OPTIONS) -f sam3.mk
	@echo ------------------------------------------------------------------------------------

.PHONY: libsam_sam3x8h_gcc_rel.a
libsam_sam3x8h_gcc_rel.a:
	@echo ------------------------------------------------------------------------------------
	@echo --- Making $@
	@echo ---
	@$(MAKE) CHIP=__SAM3X8H__ $(SUBMAKE_OPTIONS) -f sam3.mk
	@echo ------------------------------------------------------------------------------------

.PHONY: clean
clean:
	@echo ------------------------------------------------------------------------------------
	@echo --- Cleaning sam3s4c release and debug
	@$(MAKE) CHIP=__SAM3S4C__ $(SUBMAKE_OPTIONS) -f sam3.mk $@
	@$(MAKE) CHIP=__SAM3S4C__ DEBUG=1 $(SUBMAKE_OPTIONS) -f sam3.mk $@

	@echo --- Cleaning sam3u4e release and debug
	@$(MAKE) CHIP=__SAM3U4E__ $(SUBMAKE_OPTIONS) -f sam3.mk $@
	@$(MAKE) CHIP=__SAM3U4E__ DEBUG=1 $(SUBMAKE_OPTIONS) -f sam3.mk $@

	@echo --- Cleaning sam3x8e release and debug
	@$(MAKE) CHIP=__SAM3X8E__ $(SUBMAKE_OPTIONS) -f sam3.mk $@
	@$(MAKE) CHIP=__SAM3X8E__ DEBUG=1 $(SUBMAKE_OPTIONS) -f sam3.mk $@

	@echo --- Cleaning sam3x8h release and debug
	@$(MAKE) CHIP=__SAM3X8H__ $(SUBMAKE_OPTIONS) -f sam3.mk $@
	@$(MAKE) CHIP=__SAM3X8H__ DEBUG=1 $(SUBMAKE_OPTIONS) -f sam3.mk $@
	@echo ------------------------------------------------------------------------------------


