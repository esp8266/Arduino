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

Python needs to be installed on your system.  Either call the script directly, or use specific `python` binary.
Running without any parameters will show a command line help.

::

    $ python tools/boards.txt.py
    $ py -3 tools/boards.txt.py


The Core location depends on your environment.  For example, using Windows:

* for Arduino IDE git installation, it is %USERPROFILE%\Documents\Arduino\hardware\esp8266com\esp8266\

* for Arduino IDE boards manager installation, it is usually %LOCALAPPDATA%\Arduino15\packages\esp8266\hardware\esp8266\VERSION\

* for PlatformIO, it is either %USERPROFILE%\.platformio\packages\framework-arduinoespressif8266 or C:\.platformio


What can I do with it ?
~~~~~~~~~~~~~~~~~~~~~~~

As of today you can:

* in the IDE: change the default serial programming speed of any board

* in the IDE: add new serial programming speed

* increase available flash space by disabling floats in ``*printf`` functions

* change led pin ``LED_BUILTIN`` for the generic boards

* re-create boards.txt file with a different set of boards, or create a boards.local.txt


When do I need to mess with it ?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The board generator is used to automate generation of configuration files
when possible.  It needs to be edited for:

* All information for specific boards.  This is the only place where a new
  board (definition, description) can be updated or added to the existing
  list.

* Memory mapping for ldscripts (flash and filesystem size combinations)


Why is my pull-request failing continuous-integration ?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The generator is able to update a number of files (see list in help), and
global coherency can be checked by the continuous integration facilities.

After a modification in the generator, it is **mandatory** to regenerate all
files and include them in the pull-request.

::

    ./tools/boards.txt.py generate --all
    git add -u -p
    git commit


How to create an abridged boards.txt file
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The list of boards presented by the IDE has gotten quite long. You can reduce
the number of ESP8266 boards used in the IDE with a favorites list. This can
be done by generating a new boards.txt file using the ``--include <file>``
or ``--exclude <file>`` options.

Start by getting a current list of boards supported by boards.txt.py.
This command will write a list of supported board names to favorites.txt.

::

    ./tools/boards.txt.py names > favorites.txt

Edit favorites.txt, keeping only the names you want to keep.

to generate a new abridged boards.txt run:

::

   ./tools/boards.txt.py --include favorites.txt generate --boards


The commands below will overwrite the boards.txt file and omit every board named
in favorites.txt, and the next one will generate a boards.local.txt that only contains boards
named in favorites.txt.

::

    ./tools/boards.txt.py --exclude favorites.txt generate --boards
    ./tools/boards.txt.py --include favorites.txt generate --boards --boards-file boards.local.txt

Additional Notes:

1. Arduino IDE requires at least one board to be specified in boards.txt.

2. Using filtering you could omit our default boards - ``generic`` and ``esp8285``.

3. By default, generators will overwrite existing files. Use ``--output=stdout`` to redirect output to console.

4. Use ``--output=file-with-orig`` to preserve existing files as ``<filename>.orig``.

5. The boards in the boards.txt file will be in the order they were listed in your favorites file, specified by option ``--include <file>``. By default, boards are sorted alphabetically.

`FAQ list :back: <readme.rst>`__
