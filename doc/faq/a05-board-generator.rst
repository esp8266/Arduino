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
    C:\...> py -3 tools\boards.txt.py

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
files and include them in the pull-request.

::

    ./tools/boards.txt.py generate --all
    git add -u
    git commit


How to create an abridged boards.txt file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The list of boards presented by the IDE has gotten quite long. You can reduce
the ESP8266 boards shown by the IDE to a favorites list. This can
be done by generating a new boards.txt file using the ``--include <file>``
or ``--exclude <file>`` options.

Start by getting a current list of boards supported by boards.txt.py.
This command will write a list of supported board names to favorites.txt.

::

    ./tools/boards.txt.py names > favorites.txt

Edit favorites.txt, keeping the name of the boards you want generated in
boards.txt.

to generate a new abridged boards.txt run:

::

   ./tools/boards.txt.py --include favorites.txt generate --boards


The commands below will generate a boards.txt file that omits the boards named
in favorites.txt, and generates a boards.local.txt that only contains boards
named in favorites.txt.

::

    ./tools/boards.txt.py --exclude favorites.txt generate --boards
    ./tools/boards.txt.py --include favorites.txt generate --boards --boards-file boards.local.txt

Additional Notes:

1. Arduino IDE requires at least one board to be specified in boards.txt.

2. Using filter exclude you could omit ``generic`` and ``esp8285`` boards.

3. To avoid modifying any files, use ``--output=stdout`` and see the result in console output.

4. Default ``--output=file`` behaviour is to simply over-write the target file(s). Use ``--output=file-with-orig`` to preserve them as ``<filename>.orig``.

5. The boards in the boards.txt file will be in the order they were listed in your favorites file, specified by option ``--include <file>``. By default, boards are sorted alphabetically.

`FAQ list :back: <readme.rst>`__
