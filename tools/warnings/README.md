These are the warning options for the compiler at different levels.

Because GCC 10 produces code which crashes when a function is declared
to return a value but doesn't (this is undefined per the C specs), we
cannot warn them if we use "-w" to disable all warnings, and instead have
to delete every warning but "-Wfunction-return"

Generate the "-none" versions with the following command:
````
./tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-gcc --help=warnings -Q | grep '\[enabled\]' | grep -v 'return-type' | awk '{print $1}' | sed 's/-W/-Wno-/' | grep -v = | grep -v -- -f > tools/warnings/none-gcc
./tools/xtensa-lx106-elf/bin/xtensa-lx106-elf-gcc --help=warnings -Q | grep '\[enabled\]' | grep -v 'return-type' | awk '{print $1}' | sed 's/-W/-Wno-/' | grep -v = | grep -v -- -f | egrep -v '(c11-c2x-compat|c90-c99-compat|c99-c11-compat|declaration-after-statement|designated-init|discarded-array-qualifiers|discarded-qualifiers|implicit-int|incompatible-pointer-types|int-conversion|old-style-definition|override-init-side-effects|pointer-to-int-cast)' > tools/warnings/none-g++
````
