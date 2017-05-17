Using GDB stub
--------------

-  Add ``#include <GDBStub.h>`` to the sketch
-  Upload the sketch
-  Redirect serial port to TCP port:

   ::

       tcp_serial_redirect.py -p /dev/tty.SLAB_USBtoUART -b 115200 --spy -P 9980 --rts=0 --dtr=0

   Change port and baud rate as necessary. This command requires python
   and pyserial.
-  Observe serial output:

   ::

       nc localhost 9980

-  Once crash happens, close nc and start gdb:

   ::

       xtensa-lx106-elf-gdb /path/to/Sketch.cpp.elf -ex "target remote :9980"

   Or, using the provided gdbcmds file:

   ::

       xtensa-lx106-elf-gdb /path/to/Sketch.cpp.elf -x gdbcmds

-  Use gdb to inspect program state at the point of an exception.

Tips and tricks
---------------

-  Upon including GDBStub.h in the sketch, the target is automatically halted when software WDT fires, with

   ::

       *((int*)0) = 0;

   at the top of ``__wrap_system_restart_local`` in
   core\_esp8266\_postmortem.c.

License
-------

Espressif MIT License. See License file.
