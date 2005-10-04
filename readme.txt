Arduino is an open-source Physical Computing platform based on a simple
i/o board and a development environment that implements the Processing/Wiring
language.

Arduino is an open source project, owned by nobody and supported by many.

The Team is composed of Massimo Banzi, David Cuartielles, Tom Igoe,
David Mellis and Nicholas Zambetti.

Gianluca Martino works with us on many projects providing help, ideas and
taking care of the production.

Yaniv Steiner and Giorgio Olivero have been supporting the project and are
working at using it with the Instant Soup platform.

0002 - 2005.10.05
New build process no longer uses makefiles; now controlled by preferences.txt.
core/ replaced with targets/; can now link against Wiring libraries.
Replaced print() with printString, printInteger, printHex, printByte, etc.
Added menu for selecting serial port speed.
Updated icon.
Bootloader shrunk to less than 1 KB; fuses updated accordingly.
Added serialRead(), serialAvailable(), and delayMicroseconds().

0001 - 2005.08.25 
This is the first released of the unified IDE + language library
it's a terrible hack... but it works. at the moment it's in alpha stage
but it can be used to work.
The processing preprocessor is included but not used.

mbanzi
