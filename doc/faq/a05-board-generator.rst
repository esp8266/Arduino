:orphan:

Board generator
---------------

The board generator is a python script originally intended to ease the
Arduino IDE's `boards.txt` configuration file about the multitude of
available boards, especially when common parameters have to be updated for
all of them.

This script is also used to manage uncommon options that are currently not
available in the IDE menu.

-  `How can I run the script ? <#how-can-i-run-the-script>`__
-  `What can I do with it ? <#what-can-i-do-with-it>`__
-  `When do I need to update it ? <#when-do-i-need-to-mess-with-it>`__
-  `Why is my pull-request failing continuous-integration ? <#why-is-my-pull-request-failing-continuous-integration>`__

How can I run the script ?
~~~~~~~~~~~~~~~~~~~~~~~~~~

Python needs to be installed on your system.

The script is located in the ``tools`` subdirectory of the core's root installation.
It needs to be run from the root directory,

::

    $ tools/boards.txt.py

::

    C:\...> tools\boards.txt.py
    C:\...> python tools\boards.txt.py

Running without parameters will show the command line help.  They are
generally self-explanatory.  Running with the parameters will show no output but will generate a new boards.txt file (and a backup boards.txt.orig).

The core root directory varies depending on your development environment.  In Windows, core root is found under your home directory; for Arduino it is in AppData\Local\Arduino15\packages\esp8266\hardware\esp8266\2.4.2\ for PlatformIO it is in .platformio\packages\framework-arduinoespressif8266.


What can I do with it ?
~~~~~~~~~~~~~~~~~~~~~~~

As of today you can:

* in the IDE: change the default serial programming speed of any board

* in the IDE: add new serial programming speed

* increase available flash space by disabling floats in ``*printf`` functions

* change led pin ``LED_BUILTIN`` for the two generic boards

* change the default lwIP version (1.4 or 2)


When do I need to mess with it ?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The board generator is used to automate generation of configuration files
when possible.  It needs to be edited for:

* All information for specific boards.  This is the only place where a new
  board (definition, description) can be updated or added to the existing
  list.

* Memory mapping for ldscripts (flash and spiffs size combinations)


Why is my pull-request failing continuous-integration ?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The generator is able to update a number of files (see list in help), and
global coherency can be checked by the continuous integration facilities.

After a modification in the generator, it is **mandatory** to regenerate all
files (option ``--allgen``) and add them in the pull-request.


`FAQ list :back: <readme.rst>`__
