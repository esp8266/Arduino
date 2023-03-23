Using GDB to Debug Applications
===============================

ESP applications can be debugged using GDB, the GNU debugger, which is
included with the standard IDE installation.  This note will only discuss
the ESP specific steps, so please refer to the
`main GNU GDB documentation
<//sourceware.org/gdb/download/onlinedocs/gdb/index.html>`__.

Note that as of 2.5.0, the toolchain moved from the ESPRESSIF patched,
closed-source version of GDB to the main GNU version.  The debugging
formats are different, so please be sure to use only the latest Arduino
toolchain GDB executable.

CLI and IDE Note
----------------

Because the Arduino IDE doesn't support interactive debugging, the following
sections describe debugging using the command line.  Other IDEs which use
GDB in their debug backends should work identically, but you may need to
edit their configuration files or options to enable the remote serial
debugging required and to set the standard options.  PRs are happily
accepted for updates to this document with additional IDEs!


Preparing your application for GDB
----------------------------------

Applications need to be changed to enable GDB debugging support.  This
change will add 2-3KB of flash and around 700 bytes of IRAM usage, but
should not affect operation of the application.

In your main ``sketch.ino`` file, add the following line to the top of
the application:

.. code:: cpp

    #include <GDBStub.h>

And in the ``void setup()`` function ensure the serial port is initialized
and call ``gdbstub_init()``:

.. code:: cpp

    Serial.begin(115200);
    gdbstub_init();

Rebuild and reupload your application and it should run exactly as before.


Starting a Debug Session
------------------------

Once your application is running, the process to attach a debugger is
quite simple:
. Close the Arduino Serial Monitor
. Locate Application.ino.elf File
. Open a Command Prompt and Start GDB
. Apply the GDB configurations
. Attach the Debugger
. Debug Away!


Close the Arduino Serial Monitor
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Because GDB needs full control of the serial port, you will need to close
any Arduino Serial Monitor windows you may have open.  Otherwise GDB will
report an error while attempting to debug.

Locate Application.ino.elf File
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In order for GDB to debug your application, you need to locate the compiled
ELF format version of it (which includes needed debug symbols). 

Under Linux these files are stored in ``/tmp/arduino_build_*`` and the following command will help locate the right file for your app:

.. code:: bash

    find /tmp -name "*.elf" -print

Under Windows these files are stored in ``%userprofile%\AppData\Local\Temp\arduino_build_*`` and the following command will help locate the right file for your app:

.. code:: bash

    dir %userprofile%\appdata\*.elf /s/b

Note the full path of ELF file that corresponds to your sketch name, it will
be needed later once GDB is started.


Open a Command Prompt and Start GDB
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Open a terminal or ``CMD`` prompt and navigate to the proper ESP8266 toolchain
directory.

Linux

.. code:: bash

    ~/.arduino15/packages/esp8266/tools/xtensa-lx106-elf-gcc/2.5.0-4-b40a506/bin/xtensa-lx106-elf-gdb

Windows (Using Board Manager version)

.. code:: bash

    %userprofile%\AppData\Local\Arduino15\packages\esp8266\tools\xtensa-lx106-elf-gcc\2.5.0-3-20ed2b9\bin\xtensa-lx106-elf-gdb.exe

Windows (Using Git version)

.. code:: bash

    %userprofile%\Documents\Arduino\hardware\esp8266com\esp8266\tools\xtensa-lx106-elf\bin\xtensa-lx106-elf-gdb.exe

Please note the proper GDB name is "xtensa-lx106-elf-gdb".  If you accidentally
run "gdb" you may start your own operating system's GDB, which will not know how
to talk to the ESP8266.

Apply the GDB Configurations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

At the ``(gdb)`` prompt, enter the following options to configure GDB for the
ESP8266 memory map and configuration:

.. code:: bash

    set remote hardware-breakpoint-limit 1
    set remote hardware-watchpoint-limit 1
    set remote interrupt-on-connect on
    set remote kill-packet off
    set remote symbol-lookup-packet off
    set remote verbose-resume-packet off
    mem 0x20000000 0x3fefffff ro cache
    mem 0x3ff00000 0x3fffffff rw
    mem 0x40000000 0x400fffff ro cache
    mem 0x40100000 0x4013ffff rw cache
    mem 0x40140000 0x5fffffff ro cache
    mem 0x60000000 0x60001fff rw
    set serial baud 115200

Now tell GDB where your compiled ELF file is located:

.. code:: bash

    file /tmp/arduino_build_257110/sketch_dec26a.ino.elf

Attach the Debugger
~~~~~~~~~~~~~~~~~~~

Once GDB has been configured properly and loaded your debugging symbols, connect
it to the ESP with the command (replace the ttyUSB0 or COM9 with your ESP's serial
port):

.. code:: bash

    target remote /dev/ttyUSB0

or

.. code:: bash

    target remote \\.\COM9

At this point GDB will send a stop the application on the ESP8266 and you can
begin setting a breakpoint (``break loop``) or any other debugging operation.


Example Debugging Session
-------------------------

Create a new sketch and paste the following code into it:

.. code:: cpp

    #include <GDBStub.h>
    
    void setup() {
      Serial.begin(115200);
      gdbstub_init();
      Serial.printf("Starting...\n");
    }
    
    void loop() {
      static uint32_t cnt = 0;
      Serial.printf("%d\n", cnt++);
      delay(100);
    }

Save it and then build and upload to your ESP8266.  On the Serial monitor you
should see something like

.. code:: bash

    1
    2
    3
    ....


Now close the Serial Monitor.

Open a command prompt and find the ELF file:

.. code:: bash

    earle@server:~$ find /tmp -name "*.elf" -print
    /tmp/arduino_build_257110/testgdb.ino.elf
    /tmp/arduino_build_531411/listfiles.ino.elf
    /tmp/arduino_build_156712/SDWebServer.ino.elf

In this example there are multiple ``elf`` files found, but we only care about
the one we just built, ``testgdb.ino.elf``.

Open up the proper ESP8266-specific GDB

.. code:: bash

    earle@server:~$ ~/.arduino15/packages/esp8266/hardware/xtensa-lx106-elf/bin/xtensa-lx106-elf-gdb
    GNU gdb (GDB) 8.2.50.20180723-git
    Copyright (C) 2018 Free Software Foundation, Inc.
    License GPLv3+: GNU GPL version 3 or later <https://gnu.org/licenses/gpl.html>
    This is free software: you are free to change and redistribute it.
    There is NO WARRANTY, to the extent permitted by law.
    Type "show copying" and "show warranty" for details.
    This GDB was configured as "--host=x86_64-linux-gnu --target=xtensa-lx106-elf".
    Type "show configuration" for configuration details.
    For bug reporting instructions, please see:
        <https://www.gnu.org/software/gdb/bugs/>.
    Find the GDB manual and other documentation resources online at:
        <https://www.gnu.org/software/gdb/documentation/>.

    For help, type "help".
    Type "apropos word" to search for commands related to "word".
    (gdb) 

We're now at the GDB prompt, but nothing has been set up for the ESP8266
and no debug information has been loaded.  Cut-and-paste the setup options:

.. code:: bash

    (gdb) set remote hardware-breakpoint-limit 1
    (gdb) set remote hardware-watchpoint-limit 1
    (gdb) set remote interrupt-on-connect on
    (gdb) set remote kill-packet off
    (gdb) set remote symbol-lookup-packet off
    (gdb) set remote verbose-resume-packet off
    (gdb) mem 0x20000000 0x3fefffff ro cache
    (gdb) mem 0x3ff00000 0x3fffffff rw
    (gdb) mem 0x40000000 0x400fffff ro cache
    (gdb) mem 0x40100000 0x4013ffff rw cache
    (gdb) mem 0x40140000 0x5fffffff ro cache
    (gdb) mem 0x60000000 0x60001fff rw
    (gdb) set serial baud 115200
    (gdb) 

And tell GDB where the debugging info ELF file is located:

.. code:: bash

    (gdb) file /tmp/arduino_build_257110/testgdb.ino.elf
    Reading symbols from /tmp/arduino_build_257110/testgdb.ino.elf...done.

Now, connect to the running ESP8266:

.. code:: bash

    (gdb)     target remote /dev/ttyUSB0
    Remote debugging using /dev/ttyUSB0
    0x40000f68 in ?? ()
    (gdb)

Don't worry that GDB doesn't know what is at our present address, we broke
into the code at a random spot and we could be in an interrupt, in the
ROM, or elsewhere.  The important bit is that we're now connected and
two things will now happen: we can debug, and the app's regular serial
output will be displayed on the GDB console..

Continue the running app to see the serial output:

.. code:: bash

    (gdb) cont
    Continuing.
    74
    75
    76
    77
    ...

The app is back running and we can stop it at any time using ``Ctrl-C``:

.. code:: bash 

    113
    ^C
    Program received signal SIGINT, Interrupt.
    0x40000f68 in ?? ()
    (gdb) 

At this point we can set a breakpoint on the main ``loop()`` and restart
to get into our own code:

.. code:: bash

    (gdb) break loop
    Breakpoint 1 at 0x40202e33: file /home/earle/Arduino/sketch_dec26a/sketch_dec26a.ino, line 10.
    (gdb) cont
    Continuing.
    Note: automatically using hardware breakpoints for read-only addresses.
    bcn_timout,ap_probe_send_start
    
    Breakpoint 1, loop () at /home/earle/Arduino/sketch_dec26a/sketch_dec26a.ino:10
    10	void loop()
    (gdb) 

Let's examine the local variable:

.. code:: bash

    (gdb) next
    loop () at /home/earle/Arduino/sketch_dec26a/sketch_dec26a.ino:13
    13      Serial.printf("%d\n", cnt++);
    (gdb) print cnt
    $1 = 114
    (gdb) 

And change it:

.. code:: bash

    $2 = 114
    (gdb) set cnt = 2000
    (gdb) print cnt
    $3 = 2000
    (gdb) 

And restart the app and see our changes take effect:

.. code:: bash

    (gdb) cont
    Continuing.
    2000
    Breakpoint 1, loop () at /home/earle/Arduino/sketch_dec26a/sketch_dec26a.ino:10
    10	void loop() {
    (gdb) cont
    Continuing.
    2001
    Breakpoint 1, loop () at /home/earle/Arduino/sketch_dec26a/sketch_dec26a.ino:10
    10	void loop() {
    (gdb) 

Looks like we left the breakpoint on loop(), let's get rid of it and try again:

.. code:: bash

    (gdb) delete
    Delete all breakpoints? (y or n) y
    (gdb) cont
    Continuing.
    2002
    2003
    2004
    2005
    2006
    ....

At this point we can exit GDB with ``quit`` or do further debugging.


ESP8266 Hardware Debugging Limitations
--------------------------------------

The ESP8266 only supports a single hardware breakpoint and a single
hardware data watchpoint.  This means only one breakpoint in user code
is allowed at any time.  Consider using the ``thb`` (temporary hardware
breakpoint) command in GDB while debugging instead of the more common
``break`` command, since ``thb`` will remove the breakpoint once it is
reached automatically and save you some trouble.

Because of the single hardware breakpoint limitation, you must pay careful
attention to the output from ``gdb`` when you set a breakpoint. If your
breakpoint expression matches multiple locations, as in this example:

.. code:: bash

    (gdb) break loop
    Breakpoint 1 at 0x40202c84: loop. (2 locations)

Then you will be unable to ``continue``:

.. code:: bash

    (gdb) cont
    Continuing.
    Note: automatically using hardware breakpoints for read-only addresses.
    Warning:
    Cannot insert hardware breakpoint 1.
    Could not insert hardware breakpoints:
    You may have requested too many hardware breakpoints/watchpoints.

You can resolve this situation by deleting the previous breakpoint and
using a more specific breakpoint expression:

.. code:: bash

    (gdb) delete
    Delete all breakpoints? (y or n) y
    (gdb) break mysketch.ino:loop
    Breakpoint 2 at 0x40202c84: file .../mysketch.ino, line 113.
