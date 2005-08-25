
--- AVRlib "conf" files ---

AVRlib contains many function/code libraries which depend upon particular aspects of the
user's hardware.  The most basic of these is the processor clock rate.  The clock rate defines
dozens of aspects of processor operation from code delays to UART baud rates.

To allow AVRlib to work easily with hardware that may vary from project to project, all
user-configurable parameters of AVRlib are contained in the template configuration files in
this directory.  NOTE that these files are only templates and should be copied, as needed,
to an individual project's code directory and edited to suit that project's hardware.


global.h is the only configuration include file that is common to the entire AVRlib code base.
To use AVRlib libraries, you must copy global.h to your project's code directory and modify
the options inside global.h to match your hardware.)  Each project should have its own global.h.

Other *conf.h files should be copied to your project's code directory as needed.  For example,
if you intend to use the lcd.c library, you will need to copy lcdconf.h and modify it to match
the I/O and LCD configuration of your hardware.

** If you fail to copy the configuration files needed for the AVRlib libraries you use,
the problem will usually exhibit itself as a compile-time error.

