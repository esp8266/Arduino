#!/bin/bash

"/c/Program Files (x86)/Arduino/arduino-builder" -compile -logger=human -libraries "/c/Users/earle/Documents/Arduino/hardware/esp8266com/esp8266/tests/rig/waveform/libraries" -core-api-version=10608 -warnings=all -build-path ".build" -tools "/c/Program Files (x86)/Arduino/tools-builder" -hardware "/c/Users/earle/Documents/Arduino/hardware/esp8266com/esp8266/../.." -hardware "/c/Program Files (x86)/Arduino/hardware" -fqbn="esp8266com:esp8266:d1_mini:xtal=80,vt=flash,exception=legacy,ssl=all,eesz=4M3M,ip=lm2f,dbg=Disabled,lvl=None____,wipe=none,baud=921600" device/waveform_sut.ino

"/c/Users/earle/Documents/Arduino/hardware/esp8266com/esp8266/tools/python3/python3" "/c/Users/earle/Documents/Arduino/hardware/esp8266com/esp8266/tools/upload.py" --chip esp8266 --port COM6 --baud 921600 --before default_reset --after hard_reset write_flash 0x0 .build/waveform_sut.ino.bin

