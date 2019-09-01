#!/bin/bash
set -e

export PATH=../../xtensa-lx106-elf/bin:$PATH

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
xtensa-lx106-elf-ar r libmain.a eagle_lwip_if.o user_interface.o 
rm -f eagle_lwip_if.o user_interface.o

# Replace use of ROM ets_intr_(un)lock with nestable ets_intr_(un)lock_nest
for f in libmain.a libpp.a libnet80211.a; do
  xtensa-lx106-elf-objcopy  --redefine-sym ets_intr_lock=ets_intr_lock_nest $f;
  xtensa-lx106-elf-objcopy  --redefine-sym ets_intr_unlock=ets_intr_unlock_nest $f;
done

