Using Eclipse with Arduino ESP8266
==================================

What to Download
~~~~~~~~~~~~~~~~

-  `arduino IDE <https://www.arduino.cc/en/Main/Software>`__
-  `Eclipse IDE for C/C++
   Developers <http://www.eclipse.org/downloads/packages/eclipse-ide-cc-developers/oxygen3a>`__
-  `Java <http://www.java.com/>`__

Setup Arduino
~~~~~~~~~~~~~

See the
`Readme <https://github.com/esp8266/Arduino#installing-with-boards-manager>`__

Setup Eclipse
~~~~~~~~~~~~~

-  `step 1 <http://www.baeyens.it/eclipse/how_to.shtml#/c>`__
-  `step 2 <http://www.baeyens.it/eclipse/how_to.shtml#/e>`__
-  go to Window --> preferences --> Arduino
-  add as private hardware path the Part to the ESP8266

example private hardware path
                             

::

    Windows: C:\Users\[username]\AppData\Roaming\Arduino15\packages\esp8266\hardware
    Linux: /home/[username]/.arduino15/packages/esp8266/hardware

Eclipse wont build
~~~~~~~~~~~~~~~~~~

if eclipse dont find the path to the Compiler add to the platform.txt
after:

::

    version=1.6.4

this:

::

    runtime.tools.xtensa-lx106-elf-gcc.path={runtime.platform.path}/../../../tools/xtensa-lx106-elf-gcc/1.20.0-26-gb404fb9
    runtime.tools.esptool.path={runtime.platform.path}/../../../tools/esptool/0.4.4

Note: - the path may changed, check the current version. - each update
over the Arduino IDE will remove the fix - may not needed in future if
Eclipse Plugin get an Update
