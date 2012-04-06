AVR Toolchain for 64 bit Linux (x86_64-linux-gnu)

This toolchain was created by extracting the Debian gcc-avr and binutils-avr
packages from Ubuntu 9.04 (Jaunty Jackalope).  These versions closely match
the toolchain Arduino bundles on Mac and Windows.

AVR libc 1.6.4 was compiled compiled from source on Jaunty and then and
copied to the avr/lib/avr directory.  The eeprom.h file was replaced by
Arduino's modified copy.

Ubuntu libraries libgmp and libmpfr were copied to avr/lib.  The main
gcc binaries avr-gcc, avr-g++ and avr-cpp were moved to avr/bin.gcc, and
scripts were placed in the avr/bin directory to run these with
LD_LIBRARY_PATH set to use libgmp and libmpfr from the avr/lib directory.

A patch was applied to the Arduino IDE to use this toolchain if the
tools/avr/bin directory is present.  If this directory is removed, Arduino
will attempt to use the AVR toolchain provided by your system.

Finally, this readme file was added.
