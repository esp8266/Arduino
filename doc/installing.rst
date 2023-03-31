Installing
==========

Boards Manager
--------------

This is the suggested installation method for end users.

Prerequisites
~~~~~~~~~~~~~

-  Internet connection
-  Arduino IDE 1.x or 2.x (https://www.arduino.cc/en/software)
-  (macOS/Linux only) Python ≥3.7 (https://python.org)

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

-  Internet connection
-  Arduino IDE 1.x or 2.x (https://www.arduino.cc/en/software)
-  git (https://git-scm.com)
-  Python ≥3.7 (https://python.org)
-  terminal, console, or command prompt (depending on your OS)
-  **Uninstalling any core version installed via Board Manager**

Instructions - Windows 10
~~~~~~~~~~~~~~~~~~~~~~~~~
- First, make sure you don't already have an ESP8266 core version installed 
  using the Board Manager (see above). If you do, uninstall it from the 
  Board Manager before proceeding. It is also advisable to erase the Arduino15 
  contents.

- Install git for Windows (if not already; see https://git-scm.com/download/win)

-  Open a command prompt (cmd) and go to Arduino default directory. This is typically the
   *sketchbook* directory (usually ``C:\Users\{username}\Documents\Arduino`` where the environment variable ``%USERPROFILE%`` usually contains ``C:\Users\{username}``)
   
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

-  Initialize submodules to fetch external libraries

   .. code:: bash

       cd %USERPROFILE%\Documents\Arduino\hardware\esp8266com\esp8266
       git submodule update --init   
  
  Not doing this step would cause build failure when attempting to include ``SoftwareSerial.h``, ``Ethernet.h``, etc.
  See our `.gitmodules file <https://github.com/esp8266/Arduino/blob/master/.gitmodules>`__ for the full list.
  
-  Download binary tools

   .. code:: bash

       cd tools 
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

-  Initialize submodules to fetch external libraries

   .. code:: bash

       cd esp8266
       git submodule update --init   


  Not doing this step would cause build failure when attempting to include ``SoftwareSerial.h``, ``Ethernet.h``, etc.
  See our `.gitmodules file <https://github.com/esp8266/Arduino/blob/master/.gitmodules>`__ for the full list.

-  Download binary tools

   .. code:: bash
       
       cd tools
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

Maintaining
~~~~~~~~~~~

To keep up with the development branch

.. code:: bash

   git switch --recurse-submodules --discard-changes master
   git pull --recurse-submodules
   cd tools
   python3 get.py

Pull requests
~~~~~~~~~~~~~

To test not yet merged Pull Request, first you have to find its ID number. This is the sequence of digits right after the pull request title.

Open terminal and cd into the directory where the repository was previously cloned. For example, 12345 is the Pull Request ID

.. code:: bash

   git fetch origin pull/12345/head
   git switch --detach --recurse-submodules --discard-changes FETCH_HEAD

When Pull Request updates packaged tools, make sure to also fetch their latest versions.

.. code:: bash

   cd tools
   python3 get.py

To go back to using the development branch

.. code:: bash

   git switch --recurse-submodules --discard-changes master
   git pull --recurse-submodules

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
- `Using Arduino Framework Staging Version <https://docs.platformio.org/en/stable/platforms/espressif8266.html?utm_source=arduino-esp8266#using-arduino-framework-with-staging-version>`__ - install development version of the Core
- `Integration with Cloud and Standalone IDEs <https://docs.platformio.org/en/latest/ide.html?utm_source=arduino-esp8266>`__ - Cloud9, Codeanywhere, Eclipse Che (Codenvy), Atom, CLion, Eclipse, Emacs, NetBeans, Qt Creator, Sublime Text, VIM, Visual Studio, and VSCode
- `Project Examples <https://docs.platformio.org/en/latest/platforms/espressif8266.html?utm_source=arduino-esp8266#examples>`__
