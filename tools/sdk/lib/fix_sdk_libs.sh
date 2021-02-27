#!/bin/bash
set -e

export PATH=../../xtensa-lx106-elf/bin:$PATH
VERSION=$(basename ${PWD})

addSymbol_system_func1() {
	ADDRESS=$1
	xtensa-lx106-elf-objcopy --add-symbol system_func1=.irom0.text:${ADDRESS},function,global user_interface.o
}


# Remove mem_manager.o from libmain.a to use custom heap implementation,
# and time.o to fix redefinition of time-related functions:
xtensa-lx106-elf-ar d libmain.a mem_manager.o
xtensa-lx106-elf-ar d libmain.a time.o

# Rename `hostname` and `default_hostname` symbols:
xtensa-lx106-elf-ar x libmain.a eagle_lwip_if.o user_interface.o
xtensa-lx106-elf-objcopy --redefine-sym hostname=wifi_station_hostname user_interface.o 
xtensa-lx106-elf-objcopy --redefine-sym hostname=wifi_station_hostname eagle_lwip_if.o 
xtensa-lx106-elf-objcopy --redefine-sym default_hostname=wifi_station_default_hostname user_interface.o 
xtensa-lx106-elf-objcopy --redefine-sym default_hostname=wifi_station_default_hostname eagle_lwip_if.o 

if [[ ${VERSION} == "NONOSDK221" ]]; then
	addSymbol_system_func1 "0x60"
elif [[ ${VERSION} == "NONOSDK22x"* ]]; then
	addSymbol_system_func1 "0x54"
elif [[ ${VERSION} == "NONOSDK3"* ]]; then
	addSymbol_system_func1 "0x60"
else
	echo "WARN: Unknown address for system_func1() called by system_restart_local()"
fi

xtensa-lx106-elf-ar r libmain.a eagle_lwip_if.o user_interface.o 
rm -f eagle_lwip_if.o user_interface.o
