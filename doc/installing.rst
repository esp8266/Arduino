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
-  Python 3 interpreter (Mac/Linux only, Windows installation supplies its own)

Instructions
~~~~~~~~~~~~

-  Start Arduino and open Preferences window.
-  Enter
   ``https://arduino.esp8266.com/stable/package_esp8266com_index.json``
   into *Additional Board Manager URLs* field. You can add multiple
   URLs, separating them with commas.
-  Open Boards Manager from Tools > Board menu and find *esp8266*
   platform.
-  Select the version you need from a drop-down box.
-  Click *install* button.
-  Don't forget to select your ESP8266 board from Tools > Board menu
   after installation.

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
-  Python 3.x (https://python.org)
-  terminal, console, or command prompt (depending on your OS)
-  Internet connection
-  Uninstalling any core version installed via Board Manager

Instructions - Windows 10
~~~~~~~~~~~~~~~~~~~~~~~~~
- First, make sure you don't already have an ESP8266 core version installed 
  using the Board Manager (see above). If you do, uninstall it from the 
  Board Manager before proceeding. It is also advisable to erase the Arduino15 
  contents.

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
       python3 get.py

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

-  First, make sure you don't already have an ESP8266 core version installed 
   using the Board Manager (see above). If you do, uninstall it from the 
   Board Manager before proceeding. It is also advisable to erase the .arduino15 (Linux)
   or Arduino15 (MacOS) contents.

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
       python3 get.py

   If you get an error message stating that python3 is not found, you will need to install it (most modern UNIX-like OSes provide Python 3 as
   part of the default install).  To install you will need to use ``sudo yum install python3``, ``sudo apt install python3``, or ``brew install python3``
   as appropriate.  On the Mac you may get an error message like:

   .. code:: bash

       python3 get.py
       Platform: x86_64-apple-darwin
       Downloading python3-macosx-placeholder.tar.gz
       Traceback (most recent call last):
         File "/Library/Frameworks/Python.framework/Versions/3.7/lib/python3.7/urllib/request.py", line 1317, in do_open
           encode_chunked=req.has_header('Transfer-encoding'))
         ...
         File "/Library/Frameworks/Python.framework/Versions/3.7/lib/python3.7/ssl.py", line 1117, in do_handshake
           self._sslobj.do_handshake()
       ssl.SSLCertVerificationError: [SSL: CERTIFICATE_VERIFY_FAILED] certificate verify failed: unable to get local issuer certificate (_ssl.c:1056)

    This is because Homebrew on the Mac does not always install the required SSL certificates by default.  Install them manually (adjust the Python 3.7 as needed) with:

    .. code:: bash

        cd "/Applications/Python 3.7/" && sudo "./Install Certificates.command"


-  Restart Arduino

-  When later updating your local library, goto the esp8266 directory and do a git pull

   .. code:: bash

       cd hardware\esp8266com\esp8266
       git status
       git pull

Using PlatformIO
----------------

`PlatformIO <https://platformio.org?utm_source=arduino-esp8266>`__
is an open source ecosystem for IoT development with a cross-platform
build system, a library manager, and full support for Espressif
(ESP8266) development. It works on the following popular host operating
systems: macOS, Windows, Linux 32/64, and Linux ARM (like Raspberry Pi,
BeagleBone, CubieBoard).

- `What is PlatformIO? <https://docs.platformio.org/en/latest/what-is-platformio.html?utm_source=arduino-esp8266>`__
- `PlatformIO IDE <https://platformio.org/platformio-ide?utm_source=arduino-esp8266>`__
- `PlatformIO Core <https://docs.platformio.org/en/latest/core.html?utm_source=arduino-esp8266>`__ (command line tool)
- `Advanced usage <https://docs.platformio.org/en/latest/platforms/espressif8266.html?utm_source=arduino-esp8266>`__ - custom settings, uploading to LittleFS, Over-the-Air (OTA), staging version
- `Integration with Cloud and Standalone IDEs <https://docs.platformio.org/en/latest/ide.html?utm_source=arduino-esp8266>`__ - Cloud9, Codeanywhere, Eclipse Che (Codenvy), Atom, CLion, Eclipse, Emacs, NetBeans, Qt Creator, Sublime Text, VIM, Visual Studio, and VSCode
- `Project Examples <https://docs.platformio.org/en/latest/platforms/espressif8266.html?utm_source=arduino-esp8266#examples>`__
