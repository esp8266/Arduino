
The "avrproj_make" file in this directory is an important part of the
compiling process for all AVRLib example code.  Unless you are familiar with
writing your own makefiles, it is highly suggested that you use this file
to help compile your own code projects too.

------------------------------------------------------------------------------

To make "avrproj_make" work, you must have the following two environment
variables defined with appropriate values:

AVR = [path to WinAVR/AVR-GCC install directory]
AVRLIB = [path to AVRLib install directory]

For example, if you installed WinAVR in C:\WinAVR, then you should set:

AVR = c:\WinAVR

If you installed/unzipped AVRLib in c:\code\avr\avrlib, then set:

AVRLib = c:\code\avr\avrlib

------------------------------------------------------------------------------

If you are unsure how to set environment variables on your system, check the
installation guides on hubbard.engr.scu.edu/embedded or consult the web.