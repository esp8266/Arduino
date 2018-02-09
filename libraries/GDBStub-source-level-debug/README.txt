This library contains all files needed to execute source level debugging.
As from v2.0 of esp8266-Arduino this option was not available.

The scope of the instruction is limited to preparing and testing a command-line gdb-debugging session. 
It does not contain an instruction on how to install an IDE for the esp8266 nor the tool chain itself.  
The primary source of the tools and gdbstub-code is  http://gnutoolchains.com/download/. 

Compared to the sources supplied by VisualGDB a few modifications have been made to the sources: some IRAM-instructions have
placed to FLASH, and some assembly-functions have been alignment to 4 bytes (in order to enable the code being called via a long-call).


