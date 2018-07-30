:orphan:

Board generator
---------------

-  `How can I run the script ? <#how-can-i-run-the-script>`__
-  `What can I do with it ? <#what-can-i-do-with-it>`__
-  `When do I need to update it ? <#when-do-i-need-to-mess-with-it>`__
-  `Why is my pull-request failing continuous-integration ? <#why-is-my-pull-request-failing-continuous-integration ?>`__

How can I run the script ?
~~~~~~~~~~~~~~~~~~~~~~~~~~

Python 2 needs to be installed on your system.

The script is located in the ``tools`` subdirectory of the core's root installation.
It needs to be run from the root directory,

:: 

    $ tools/boards.txt.py

::

    C:\...> tools\boards.txt.py

Running without parameters will show the command line help.  They are
generally self-explanatory.


What can I do with it ?
~~~~~~~~~~~~~~~~~~~~~~~

As of today you can:

* in the IDE: change the default serial programming speed of any board

* in the IDE: add new serial programming speed

* increase available flash space by disabling floats in ``*printf`` functions

* enable WPS which is now disabled by default (at the cost of a smaller heap by 4KB)

* change led pin ``LED_BUILTIN`` for the two generic boards

* change the default lwIP version (1.4 or 2)


When do I need to mess with it ?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The board generator is used to automate configuration files generatiom when it is
possible to. It needs to be edited for:

* All the specific board informations.  This is the only place where a new
  board (definition, description) can be updated or added to the existing
  list.
  
* Memory mapping for ldscripts (flash and spiffs size combinations)


Why is my pull-request failing continuous-integration ?
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The generator is able to update a number of files (see list in help), and
global coherency can be checked by the continuous integration facilities.

After a modification in the generator, it is **mandatory** to regenerate all
files (option ``--allgen``) and add them in the pull-request.


`FAQ list :back: <readme.rst>`__
