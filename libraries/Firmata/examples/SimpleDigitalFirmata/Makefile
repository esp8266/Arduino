# Arduino makefile
#
# This makefile allows you to build sketches from the command line
# without the Arduino environment (or Java).
#
# The Arduino environment does preliminary processing on a sketch before
# compiling it.  If you're using this makefile instead, you'll need to do
# a few things differently:
#
#   - Give your program's file a .cpp extension (e.g. foo.cpp).
#
#   - Put this line at top of your code: #include <WProgram.h>
#
#   - Write prototypes for all your functions (or define them before you
#     call them).  A prototype declares the types of parameters a
#     function will take and what type of value it will return.  This
#     means that you can have a call to a function before the definition
#     of the function.  A function prototype looks like the first line of
#     the function, with a semi-colon at the end.  For example:
#     int digitalRead(int pin);
#
# Instructions for using the makefile:
#
#  1. Copy this file into the folder with your sketch.
#
#  2. Below, modify the line containing "TARGET" to refer to the name of
#     of your program's file without an extension (e.g. TARGET = foo).
#
#  3. Modify the line containg "ARDUINO" to point the directory that
#     contains the Arduino core (for normal Arduino installations, this
#     is the hardware/cores/arduino sub-directory).
#
#  4. Modify the line containing "PORT" to refer to the filename
#     representing the USB or serial connection to your Arduino board
#     (e.g. PORT = /dev/tty.USB0).  If the exact name of this file
#     changes, you can use * as a wildcard (e.g. PORT = /dev/tty.USB*).
#
#  5. At the command line, change to the directory containing your
#     program's file and the makefile.
#
#  6. Type "make" and press enter to compile/verify your program.
#
#  7. Type "make upload", reset your Arduino board, and press enter  to
#     upload your program to the Arduino board.
#
# $Id: Makefile,v 1.7 2007/04/13 05:28:23 eighthave Exp $

PORT = /dev/tty.usbserial-*
TARGET := $(shell pwd | sed 's|.*/\(.*\)|\1|')
ARDUINO = /Applications/arduino
ARDUINO_SRC = $(ARDUINO)/hardware/cores/arduino
ARDUINO_LIB_SRC = $(ARDUINO)/hardware/libraries
INCLUDE = -I$(ARDUINO_SRC) -I$(ARDUINO)/hardware/tools/avr/avr/include \
	-I$(ARDUINO_LIB_SRC)/EEPROM \
	-I$(ARDUINO_LIB_SRC)/Firmata \
	-I$(ARDUINO_LIB_SRC)/Servo \
	-I$(ARDUINO_LIB_SRC)
SRC = $(wildcard $(ARDUINO_SRC)/*.c)
CXXSRC = applet/$(TARGET).cpp $(ARDUINO_SRC)/HardwareSerial.cpp \
	$(ARDUINO_LIB_SRC)/EEPROM/EEPROM.cpp \
	$(ARDUINO_LIB_SRC)/Firmata/Firmata.cpp \
	$(ARDUINO_LIB_SRC)/Servo/Servo.cpp \
	$(ARDUINO_SRC)/WMath.cpp
HEADERS = $(wildcard $(ARDUINO_SRC)/*.h) $(wildcard $(ARDUINO_LIB_SRC)/*/*.h)

MCU = atmega168
#MCU = atmega8
F_CPU = 16000000
FORMAT = ihex
UPLOAD_RATE = 19200

# Name of this Makefile (used for "make depend").
MAKEFILE = Makefile

# Debugging format.
# Native formats for AVR-GCC's -g are stabs [default], or dwarf-2.
# AVR (extended) COFF requires stabs, plus an avr-objcopy run.
DEBUG = stabs

OPT = s

# Place -D or -U options here
CDEFS = -DF_CPU=$(F_CPU)
CXXDEFS = -DF_CPU=$(F_CPU)

# Compiler flag to set the C Standard level.
# c89   - "ANSI" C
# gnu89 - c89 plus GCC extensions
# c99   - ISO C99 standard (not yet fully implemented)
# gnu99 - c99 plus GCC extensions
CSTANDARD = -std=gnu99
CDEBUG = -g$(DEBUG)
CWARN = -Wall -Wstrict-prototypes
CTUNING = -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums
#CEXTRA = -Wa,-adhlns=$(<:.c=.lst)

CFLAGS = $(CDEBUG) $(CDEFS) $(INCLUDE) -O$(OPT) $(CWARN) $(CSTANDARD) $(CEXTRA)
CXXFLAGS = $(CDEFS) $(INCLUDE) -O$(OPT)
#ASFLAGS = -Wa,-adhlns=$(<:.S=.lst),-gstabs 
LDFLAGS = 


# Programming support using avrdude. Settings and variables.
AVRDUDE_PROGRAMMER = stk500
AVRDUDE_PORT = $(PORT)
AVRDUDE_WRITE_FLASH = -U flash:w:applet/$(TARGET).hex
AVRDUDE_FLAGS = -F -p $(MCU) -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) \
  -b $(UPLOAD_RATE) -q -V

# Program settings
CC = avr-gcc
CXX = avr-g++
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size
NM = avr-nm
AVRDUDE = avrdude
REMOVE = rm -f
MV = mv -f

# Define all object files.
OBJ = $(SRC:.c=.o) $(CXXSRC:.cpp=.o) $(ASRC:.S=.o)

# Define all listing files.
LST = $(ASRC:.S=.lst) $(CXXSRC:.cpp=.lst) $(SRC:.c=.lst)

# Combine all necessary flags and optional flags.
# Add target processor to flags.
ALL_CFLAGS = -mmcu=$(MCU) -I. $(CFLAGS)
ALL_CXXFLAGS = -mmcu=$(MCU) -I. $(CXXFLAGS)
ALL_ASFLAGS = -mmcu=$(MCU) -I. -x assembler-with-cpp $(ASFLAGS)


# Default target.
all: build

build: applet/$(TARGET).hex

eep: applet/$(TARGET).eep
lss: applet/$(TARGET).lss 
sym: applet/$(TARGET).sym


# Convert ELF to COFF for use in debugging / simulating in AVR Studio or VMLAB.
COFFCONVERT=$(OBJCOPY) --debugging \
--change-section-address .data-0x800000 \
--change-section-address .bss-0x800000 \
--change-section-address .noinit-0x800000 \
--change-section-address .eeprom-0x810000 


coff: applet/$(TARGET).elf
	$(COFFCONVERT) -O coff-avr applet/$(TARGET).elf applet/$(TARGET).cof


extcoff: applet/$(TARGET).elf
	$(COFFCONVERT) -O coff-ext-avr applet/$(TARGET).elf applet/$(TARGET).cof


.SUFFIXES: .elf .hex .eep .lss .sym .pde

.elf.hex:
	$(OBJCOPY) -O $(FORMAT) -R .eeprom $< $@

.elf.eep:
	-$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom="alloc,load" \
	--change-section-lma .eeprom=0 -O $(FORMAT) $< $@

# Create extended listing file from ELF output file.
.elf.lss:
	$(OBJDUMP) -h -S $< > $@

# Create a symbol table from ELF output file.
.elf.sym:
	$(NM) -n $< > $@


# Compile: create object files from C++ source files.
.cpp.o: $(HEADERS)
	$(CXX) -c $(ALL_CXXFLAGS) $< -o $@ 

# Compile: create object files from C source files.
.c.o: $(HEADERS)
	$(CC)  -c $(ALL_CFLAGS) $< -o $@ 


# Compile: create assembler files from C source files.
.c.s:
	$(CC) -S $(ALL_CFLAGS) $< -o $@


# Assemble: create object files from assembler source files.
.S.o:
	$(CC) -c $(ALL_ASFLAGS) $< -o $@



applet/$(TARGET).cpp: $(TARGET).pde
	test -d applet || mkdir applet
	echo '#include "WProgram.h"' > applet/$(TARGET).cpp
	echo '#include "avr/interrupt.h"' >> applet/$(TARGET).cpp
	sed -n 's|^\(void .*)\).*|\1;|p' $(TARGET).pde | grep -v 'setup()' | \
		grep -v 'loop()' >> applet/$(TARGET).cpp
	cat $(TARGET).pde >> applet/$(TARGET).cpp
	cat $(ARDUINO_SRC)/main.cxx >> applet/$(TARGET).cpp

# Link: create ELF output file from object files.
applet/$(TARGET).elf: applet/$(TARGET).cpp $(OBJ)
	$(CC) $(ALL_CFLAGS) $(OBJ) --output $@ $(LDFLAGS)

pd_close_serial:
	echo 'close;' | /Applications/Pd-extended.app/Contents/Resources/bin/pdsend 34567 || true

# Program the device.  
upload: applet/$(TARGET).hex
	$(AVRDUDE) $(AVRDUDE_FLAGS) $(AVRDUDE_WRITE_FLASH)


pd_test: build pd_close_serial upload

# Target: clean project.
clean:
	$(REMOVE) -- applet/$(TARGET).hex applet/$(TARGET).eep \
	applet/$(TARGET).cof applet/$(TARGET).elf $(TARGET).map \
	applet/$(TARGET).sym applet/$(TARGET).lss applet/$(TARGET).cpp \
	$(OBJ) $(LST) $(SRC:.c=.s) $(SRC:.c=.d) $(CXXSRC:.cpp=.s) $(CXXSRC:.cpp=.d)
	rmdir -- applet

depend:
	if grep '^# DO NOT DELETE' $(MAKEFILE) >/dev/null; \
	then \
		sed -e '/^# DO NOT DELETE/,$$d' $(MAKEFILE) > \
			$(MAKEFILE).$$$$ && \
		$(MV) $(MAKEFILE).$$$$ $(MAKEFILE); \
	fi
	echo '# DO NOT DELETE THIS LINE -- make depend depends on it.' \
		>> $(MAKEFILE); \
	$(CC) -M -mmcu=$(MCU) $(CDEFS) $(INCLUDE) $(SRC) $(ASRC) >> $(MAKEFILE)

.PHONY:	all build eep lss sym coff extcoff clean depend pd_close_serial pd_test

# for emacs
etags:
	make etags_`uname -s`
	etags *.pde \
		$(ARDUINO_SRC)/*.[ch] \
		$(ARDUINO_SRC)/*.cpp \
		$(ARDUINO_LIB_SRC)/*/*.[ch] \
		$(ARDUINO_LIB_SRC)/*/*.cpp \
		$(ARDUINO)/hardware/tools/avr/avr/include/avr/*.[ch] \
		$(ARDUINO)/hardware/tools/avr/avr/include/*.[ch]

etags_Darwin:
#	etags -a 

etags_Linux:
#	etags -a /usr/include/*.h linux/input.h /usr/include/sys/*.h

etags_MINGW:
#	etags -a /usr/include/*.h /usr/include/sys/*.h 



