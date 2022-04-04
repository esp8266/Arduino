#!/bin/bash
set -e

export PATH=../../../xtensa-lx106-elf/bin:$PATH
VERSION=$(basename ${PWD})

addSymbol_system_func1() {
	if ! xtensa-lx106-elf-nm  user_interface.o | grep -q " T system_func1"; then # Don't add symbol if it already exists
		ADDRESS=$1
		xtensa-lx106-elf-objcopy --add-symbol system_func1=.irom0.text:${ADDRESS},function,global user_interface.o
	fi
}

patchFile() {
	FILE=$1
	ADDRESS=$2 # DO NOT PASS AS HEX!
	LENGTH=$3 # DO NOT PASS AS HEX!
	EXPECTED=$4
	REPLACEWITH=$5
	if [[ "$(dd if=eap.o bs=1 count=$LENGTH skip=$ADDRESS status=none | base64 -w0)" = "$EXPECTED" ]]; then
		echo "Patching $1..."
		echo $5 | base64 -d | dd of=eap.o bs=1 count=$LENGTH seek=$ADDRESS conv=notrunc
	elif ! [[ "$(dd if=eap.o bs=1 count=$LENGTH skip=$ADDRESS status=none | base64 -w0)" = "$REPLACEWITH" ]]; then
		echo "PATCH FAILED!"
		exit 0
	fi
}

# Remove mem_manager.o from libmain.a to use custom heap implementation,
# and time.o to fix redefinition of time-related functions:
xtensa-lx106-elf-ar d libmain.a mem_manager.o
xtensa-lx106-elf-ar d libmain.a time.o

# Patch WPA2-Enterprise double-free
xtensa-lx106-elf-ar x libwpa2.a eap.o
eapcs=$(sha256sum eap.o | awk '{print $1}')

# Rename `hostname` and `default_hostname` symbols:
xtensa-lx106-elf-ar x libmain.a eagle_lwip_if.o user_interface.o
lwipcs=$(sha256sum eagle_lwip_if.o | awk '{print $1}')
uics=$(sha256sum user_interface.o | awk '{print $1}')
xtensa-lx106-elf-objcopy --redefine-sym hostname=wifi_station_hostname user_interface.o
xtensa-lx106-elf-objcopy --redefine-sym hostname=wifi_station_hostname eagle_lwip_if.o
xtensa-lx106-elf-objcopy --redefine-sym default_hostname=wifi_station_default_hostname user_interface.o
xtensa-lx106-elf-objcopy --redefine-sym default_hostname=wifi_station_default_hostname eagle_lwip_if.o

if [[ ${VERSION} == "NONOSDK221" ]]; then
	addSymbol_system_func1 "0x60"
	patchFile "eap.o" "3055" "2" "wAA=" "8CA=" # WPA2-Enterprise patch which replaces a double-free with nop, see #8082
elif [[ ${VERSION} == "NONOSDK22x"* ]]; then
	addSymbol_system_func1 "0x54"
	patchFile "eap.o" "3059" "2" "wAA=" "8CA=" # WPA2-Enterprise patch which replaces a double-free with nop, see #8082
elif [[ ${VERSION} == "NONOSDK3"* ]]; then
	addSymbol_system_func1 "0x60"
	patchFile "eap.o" "3059" "2" "wAA=" "8CA=" # WPA2-Enterprise patch which replaces a double-free with nop, see #8082
else
	echo "WARN: Unknown address for system_func1() called by system_restart_local()"
fi

if [[ $(sha256sum eap.o | awk '{print $1}') != $eapcs ]]; then
	xtensa-lx106-elf-ar r libwpa2.a eap.o
fi
if [[ $(sha256sum user_interface.o | awk '{print $1}') != $uics || $(sha256sum eagle_lwip_if.o | awk '{print $1}') != $lwipcs ]]; then
	xtensa-lx106-elf-ar r libmain.a eagle_lwip_if.o user_interface.o
fi
rm -f eagle_lwip_if.o user_interface.o eap.o

