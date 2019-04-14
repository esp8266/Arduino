Installing
==========

Boards Manager
--------------

This is the suggested installation method for end users.

Prerequisites
~~~~~~~~~~~~~

-  Arduino 1.6.8, get it from `Arduino
   website <https://www.arduino.cc/en/Main/OldSoftwareReleases#previous>`__.
-  Internet connection

Instructions
~~~~~~~~~~~~

-  Start Arduino and open Preferences window.
-  Enter
   ``http://arduino.esp8266.com/stable/package_esp8266com_index.json``
   into *Additional Board Manager URLs* field. You can add multiple
   URLs, separating them with commas.
-  Open Boards Manager from Tools > Board menu and find *esp8266*
   platform.
-  Select the version you need from a drop-down box.
-  Click *install* button.
-  Don't forget to select your ESP8266 board from Tools > Board menu
   after installation.

You may optionally use *staging* boards manager package link:
``http://arduino.esp8266.com/staging/package_esp8266com_index.json``.
This may contain some new features, but at the same time, some things
might be broken.

For more information on the Arduino Board Manager, see:

- https://www.arduino.cc/en/guide/cores

Using git version
-----------------

This is the suggested installation method for contributors and library
developers.

Prerequisites
~~~~~~~~~~~~~

-  Arduino 1.6.8 (or newer, current working version is 1.8.5)
-  git
-  Python 2.7 (http://python.org)
-  terminal, console, or command prompt (depending on your OS)
-  Internet connection

Instructions - Windows 10
~~~~~~~~~~~~~~~~~~~~~~~~~
- First, make sure you don't already have the ESP8266 library installed using the Board Manager (see above)

- Install git for Windows (if not already; see https://git-scm.com/download/win)

-  Open a command prompt (cmd) and go to Arduino default directory. This is typically the
   *sketchbook* directory (usually ``C:\users\{username}\Documents\Arduino`` where the environment variable ``%USERPROFILE%`` usually contains ``C:\users\{username}``)
   
-  Clone this repository into hardware/esp8266com/esp8266 directory.

   .. code:: bash
      
       cd %USERPROFILE%\Documents\Arduino\
       if not exist hardware mkdir hardware
       cd hardware
       if not exist esp8266com mkdir esp8266com
       cd esp8266com
       git clone https://github.com/esp8266/Arduino.git esp8266

   You should end up with the following directory structure in
   
   ``C:\Users\{your username}\Documents\``

   .. code:: bash

       Arduino
       |
       --- libraries
       --- hardware
           |
           --- esp8266com
               |
               --- esp8266
                   |
                   --- bootloaders
                   --- cores
                   --- doc
                   --- libraries
                   --- package
                   --- tests
                   --- tools
                   --- variants
                   --- platform.txt
                   --- programmers.txt
                   --- README.md
                   --- boards.txt
                   --- LICENSE

-  Initialize the submodules

   .. code:: bash

       cd %USERPROFILE%\Documents\Arduino\hardware\esp8266com\esp8266
       git submodule update --init   
  
  If error messages about missing files related to ``SoftwareSerial`` are encountered during the build process, it should be because this step was missed and is required.
  
-  Download binary tools

   .. code:: bash

       cd esp8266/tools
       python get.py

-  Restart Arduino

- If using the Arduino IDE for Visual Studio (https://www.visualmicro.com/), be sure to click Tools - Visual Micro - Rescan Toolchains and Libraries 

-  When later updating your local library, goto the esp8266 directory and do a git pull

   .. code:: bash

       cd %USERPROFILE%\Documents\Arduino\hardware\esp8266com\esp8266
       git status
       git pull

Note that you could, in theory install in ``C:\Program Files (x86)\Arduino\hardware`` however this has security implications, not to mention the directory often gets blown away when re-installing Arduino IDE. It does have the benefit (or drawback, depending on your perspective) - of being available to all users on your PC that use Arduino.


Instructions - Other OS
~~~~~~~~~~~~~~~~~~~~~~~

-  Open the console and go to Arduino directory. This can be either your
   *sketchbook* directory (usually ``<Documents>/Arduino``), or the
   directory of Arduino application itself, the choice is up to you.
-  Clone this repository into hardware/esp8266com/esp8266 directory.
   Alternatively, clone it elsewhere and create a symlink, if your OS
   supports them.

   .. code:: bash

       cd hardware
       mkdir esp8266com
       cd esp8266com
       git clone https://github.com/esp8266/Arduino.git esp8266

   You should end up with the following directory structure:

   .. code:: bash

       Arduino
       |
       --- hardware
           |
           --- esp8266com
               |
               --- esp8266
                   |
                   --- bootloaders
                   --- cores
                   --- doc
                   --- libraries
                   --- package
                   --- tests
                   --- tools
                   --- variants
                   --- platform.txt
                   --- programmers.txt
                   --- README.md
                   --- boards.txt
                   --- LICENSE

-  Initialize the submodules

   .. code:: bash

       cd esp8266
       git submodule update --init   
  
  If error messages about missing files related to ``SoftwareSerial`` are encountered during the build process, it should be because this step was missed and is required.

-  Download binary tools

   .. code:: bash

       cd esp8266/tools
       python get.py

-  Restart Arduino

- When later updating your local library, goto the esp8266 directory and do a git pull

   .. code:: bash

       cd hardware\esp8266com\esp8266
       git status
       git pull
