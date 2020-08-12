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

* create an abridged boards.txt file


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


How to create an abridged boards.txt file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The list of boards presented by the IDE has gotten quite long. You can reduce
the ESP8266 boards shown by the IDE to a favorites list. This can
be done by generating a new boards.txt file using the ``--filter <file>``
option.

Start by getting a current list of boards supported by boards.txt.py.
This command will write a list of supported board names to favorites.txt.

::

    ./tools/boards.txt.py --boardnames >favorites.txt

Edit favorites.txt, keeping the name of the boards you want generated in
boards.txt.

to generate a new abridged boards.txt run:

::

   ./tools/boards.txt.py --boardsgen --filter favorites.txt


You can turn the process around by creating a list of boards, you do not want
to be generated. To do this we use the ``--xfilter <file>`` option.

to generate this abridged boards.txt run:

::

    ./tools/boards.txt.py --boardsgen --xfilter favorites.txt


Yet another option, you can split the boards between boards.txt and
boards.local.txt.

The commands below will generate a boards.txt file that omits the boards named
in favorites.txt, and generates a boards.local.txt ( via option ``--boardslocalgen`` ) that only contains boards
named in favorites.txt.

::

    ./tools/boards.txt.py --boardsgen --xfilter favorites.txt
    ./tools/boards.txt.py --boardslocalgen --filter favorites.txt

Additional Notes:

1. The boards.txt file will always contain the generic and esp8285 boards.

2. If boards.txt file exist and no backup copy named boards.txt.orig exist, the current boards.txt will be renamed to boards.txt.orig. Otherwise, the existing boards.txt is over-writen when you generate a new boards.txt file. Similar behavior for when generating a new boards.local.txt.

3. The boards in the boards.txt file will be in the order they were listed in your favorites file, specified by option ``--filter <file>``.

4. It is outside the scope of this document, but you could manually edit any boards.txt file to have fewer boards. One last observation, the Arduino IDE appears to need at least one board in a board.txt file.

`FAQ list :back: <readme.rst>`__
