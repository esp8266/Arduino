## Using GDB stub

- Add `#include <GDBStub.h>` to the sketch
- Upload the sketch
- Redirect serial port to TCP port:
```
    tcp_serial_redirect.py -p /dev/tty.SLAB_USBtoUART -b 115200 --spy -P 9980 --rts=0 --dtr=0
```
Change port and baud rate as necessary. This command requires python and pyserial.
- Observe serial output:
```
    nc localhost 9980
```
- When crash happens, `Trap %d: pc=%p va=%p` line will appear in serial output.
- Close nc and start gdb:
```
    xtensa-lx106-elf-gdb /path/to/Sketch.cpp.elf -ex "target remote :9980"
```
- Use gdb to inspect program state at the point of an exception.

## Tips and tricks

- To halt the target when software WDT fires, add
```
    ((int*)0) = 0;
```
at the top of `__wrap_system_restart_local` in core_esp8266_postmortem.c.

## License

GDB Server stub by Marko Mikulicic was taken from Cesanta's smart.js

https://github.com/cesanta/smart.js

Copyright (c) 2013-2014 Cesanta Software Limited
All rights reserved

This software is dual-licensed: you can redistribute it and/or modify
it under the terms of the GNU General Public License version 2 as
published by the Free Software Foundation. For the terms of this
license, see <http://www.gnu.org/licenses>.

You are free to use this software under the terms of the GNU General
Public License, but WITHOUT ANY WARRANTY; without even the implied
warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU General Public License for more details.

Alternatively, you can license this software under a commercial
license, as set out in <https://www.cesanta.com/license>.
