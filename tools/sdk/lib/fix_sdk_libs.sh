#!/bin/bash
set -e

add_path_ifexist() {
	if [[ -d $1 ]]; then
		export PATH=$( realpath $1 ):$PATH
		return 0
	fi
	return 1
}

if ! which xtensa-lx106-elf-ar | grep "tools/xtensa-lx106-elf/bin" >>/dev/null; then
	add_path_ifexist "../../../xtensa-lx106-elf/bin" || add_path_ifexist "../../xtensa-lx106-elf/bin"
fi
WORK_SPACE=${PWD}

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
	if [[ "$(dd if=$FILE bs=1 count=$LENGTH skip=$ADDRESS status=none | base64 -w0)" = "$EXPECTED" ]]; then
		echo "Patching  $VERSION $1 ..."
		echo $5 | base64 -d | dd of=$FILE bs=1 count=$LENGTH seek=$ADDRESS conv=notrunc
	elif ! [[ "$(dd if=$FILE bs=1 count=$LENGTH skip=$ADDRESS status=none | base64 -w0)" = "$REPLACEWITH" ]]; then
		echo "PATCH FAILED!"
		echo "dd if=$FILE bs=1 count=$LENGTH skip=$ADDRESS status=none | base64 -w0"
		dd if=$FILE bs=1 count=$LENGTH skip=$ADDRESS status=none | hexdump -C
		dd if=$FILE bs=1 count=$LENGTH skip=$ADDRESS status=none | base64 -w0
		echo ""
		exit 1
	fi
}

grepPatchFile() {
	local SDKVER OLDNAME NEWNAME FILES OLDNAME64 NEWNAME64 FILE OFFSET PATTERN
	SDKVER="${1}"
	OLDNAME="${2}"
	NEWNAME="${3}"
	FILES="${4}"
  [[ "${SDKVER:0:9}" != "NONOSDK30" ]] && return
	if [[ -z "${FILES}" ]]; then
		echo "grepPatchFile: bad input: file specification required"
		exit 1
  fi
	if [[ "${#OLDNAME}" != "${#NEWNAME}" ]]; then
		echo "grepPatchFile: bad input: old name ${OLDNAME}(${#OLDNAME}) and new name ${NEWNAME}(${#NEWNAME}) must be the same length."
		exit 1
	fi
	OLDNAME64=( `echo -n "${OLDNAME}" | base64 -w0` )
	NEWNAME64=( `echo -n "${NEWNAME}" | base64 -w0` )

  while read -u3 FILE OFFSET PATTERN; do
		if [[ "${#OLDNAME}" == "${#PATTERN}" ]] && [[ "${OLDNAME}" == "${PATTERN}" ]]; then
			patchFile "$FILE" "$OFFSET" "${#PATTERN}" "${OLDNAME64}" "${NEWNAME64}"
		else
			echo "grepPatchFile: bad parameters FILE=${FILE} OFFSET=${OFFSET} PATTERN=${PATTERN}"
			exit 1
		fi
  done 3< <( grep --with-filename --byte-offset --only-matching --text "${OLDNAME}" $FILES | tr ":" " " )
	return
}

# # xtensa-lx106-elf-ar x libwpa2.a eap.o
if [[ "--shell" == "$1" ]]; then
	# need to poke around a bit
	bash --rcfile <(echo ". ~/.bashrc; cd ${WORK_SPACE}")
	exit 0
fi

if [[ ! -f libmain.a ]]; then
	echo -e "\n\n*** Archive libmain.a is missing ***\n\n"
	exit 1
fi

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
	patchFile "eap.o" "26352" "9" "dlBvcnRGcmVl" "ejJFYXBGcmVl"   # special vPortFree to recover leaked memory
elif [[ ${VERSION} == "NONOSDK22x"* ]]; then
	addSymbol_system_func1 "0x54"
	patchFile "eap.o" "3059" "2" "wAA=" "8CA=" # WPA2-Enterprise patch which replaces a double-free with nop, see #8082
	patchFile "eap.o" "26356" "9" "dlBvcnRGcmVl" "ejJFYXBGcmVl"   # special vPortFree to recover leaked memory
elif [[ ${VERSION} == "NONOSDK3V0" ]]; then
	addSymbol_system_func1 "0x60"
	patchFile "eap.o" "3059" "2" "wAA=" "8CA=" # WPA2-Enterprise patch which replaces a double-free with nop, see #8082
	patchFile "eap.o" "26356" "9" "dlBvcnRGcmVl" "ejJFYXBGcmVl"   # special vPortFree to recover leaked memory
	xtensa-lx106-elf-objcopy --weaken-symbol load_non_32_wide_handler libmain.a
elif [[ ${VERSION} == "NONOSDK300" ]]; then
	addSymbol_system_func1 "0x54"
	patchFile "eap.o" "19204" "9" "dlBvcnRGcmVl" "ejJFYXBGcmVl"   # special vPortFree to recover leaked memory
	# v3.0.0 and up use a non-standard pvPortMalloc.
	# SDK Library global replace
	grepPatchFile "${VERSION}" "pvPortMalloc" "pvEsprMalloc" '*.a' #
	xtensa-lx106-elf-objcopy --weaken-symbol load_non_32_wide_handler libmain.a
elif [[ ${VERSION} == "NONOSDK301" ]]; then
	addSymbol_system_func1 "0x54"
	patchFile "eap.o" "26364" "9" "dlBvcnRGcmVl" "ejJFYXBGcmVl"   # special vPortFree to recover leaked memory
	grepPatchFile "${VERSION}" "pvPortMalloc" "pvEsprMalloc" '*.a'
	xtensa-lx106-elf-objcopy --weaken-symbol load_non_32_wide_handler libmain.a
elif [[ ${VERSION} == "NONOSDK302" ]]; then
	addSymbol_system_func1 "0x54"
	patchFile "eap.o" "26536" "9" "dlBvcnRGcmVl" "ejJFYXBGcmVl"   # special vPortFree to recover leaked memory
	grepPatchFile "${VERSION}" "pvPortMalloc" "pvEsprMalloc" '*.a'
	xtensa-lx106-elf-objcopy --weaken-symbol load_non_32_wide_handler libmain.a
elif [[ ${VERSION} == "NONOSDK303" ]]; then
	addSymbol_system_func1 "0x54"
	patchFile "eap.o" "26536" "9" "dlBvcnRGcmVl" "ejJFYXBGcmVl"   # special vPortFree to recover leaked memory
	grepPatchFile "${VERSION}" "pvPortMalloc" "pvEsprMalloc" '*.a'
	xtensa-lx106-elf-objcopy --weaken-symbol load_non_32_wide_handler libmain.a
elif [[ ${VERSION} == "NONOSDK304" ]]; then
	addSymbol_system_func1 "0x54"
	patchFile "eap.o" "19376" "9" "dlBvcnRGcmVl" "ejJFYXBGcmVl"   # special vPortFree to recover leaked memory
	grepPatchFile "${VERSION}" "pvPortMalloc" "pvEsprMalloc" '*.a'
	xtensa-lx106-elf-objcopy --weaken-symbol load_non_32_wide_handler libmain.a
elif [[ ${VERSION} == "NONOSDK305" ]]; then
	addSymbol_system_func1 "0x54"
	patchFile "eap.o" "67670" "9" "dlBvcnRGcmVl" "ejJFYXBGcmVl"   # special vPortFree to recover leaked memory
	grepPatchFile "${VERSION}" "pvPortMalloc" "pvEsprMalloc" '*.a'
	xtensa-lx106-elf-objcopy --weaken-symbol load_non_32_wide_handler libmain.a
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
