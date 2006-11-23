Arduino is an open-source physical computing platform based on a simple i/o
board and a development environment that implements the Processing/Wiring
language. Arduino can be used to develop stand-alone interactive objects or
can be connected to software on your computer (e.g. Flash, Processing, MaxMSP).
The boards can be assembled by hand or purchased preassembled; the open-source
IDE can be downloaded for free.

For more information, see the website at: http://www.arduino.cc/
or the forums at: http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl

To report a bug or a make a suggestions, go to:
[hardware] http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?board=hwbugs
[software] http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?board=swbugs

INSTALLATION
Detailed instructions are in reference/howto.html.

If you are using a USB Arduino, you will need to install the drivers for the
FTDI chip on the board. These can be found in the drivers/ directory.

* On Windows, you will need to unzip FTDI USB Drivers.zip. Then plug in the
  Arduino board and point the Windows Add Hardware wizard to the FTDI USB
  Drivers directory.

* On the Mac, mount the FTDIUSBSerialDriver_v2_1_6.dmg (on PPC Macs) or the
  FTDIUSBSerialDriver_v2_2_6_Intel.dmg (on Intel Macs) disk image and run the
  included FTDIUSBSerialDriver.pkg.  Also run the macosx_setup.command (after
  moving the Arduino distribution to your /Applications folder). It corrects
  permission on a few files for use with the serial port and will prompt you
  for your password. You may need to reboot after running this script.

CREDITS
Arduino is an open source project, owned by nobody and supported by many.

The Arduino team is composed of Massimo Banzi, David Cuartielles, Tom Igoe,
Gianluca Martino, and David A. Mellis.

Nicholas Zambetti has contributed from the beginning.

Yaniv Steiner and Giorgio Olivero have been supporting the project and are
working at using it with the Instant Soup platform.

Arduino uses the GNU avr-gcc toolchain, uisp, avr-libc, avrlib, and code
from Processing and Wiring.

UPDATES

0007

Replaced avr-lib's uart routines with custom code for handling serial
communication and modified C++ serial commands to call the C serial commands,
saving ~1 KB of space; the code may behave slightly differently in border
cases (e.g. non-standard speeds, or on overflow).
Adding Sonar library for controlling Parallax Ultrasonic PING))) sensors.
Defining binary constants: e.g. B1010 is 6.
Digital pins 0 and 1 can be used for i/o until a call to Serial.begin().

0006 - 2006.10.21

Mac version no longer requires Java 1.5, meaning it should run on 10.3.9.
Added support for analog inputs 6 and 7 and pwm on pins 5 and 6 on the
on the ATmega168 used in the Arduino Mini (extra analog inputs not available
in DIP ATmega168s).
You now select the baud rate for the serial monitor from within the editor
status bar when the serial monitor is running instead of from the Tools menu.
Pressing enter within the serial monitor edit box no longer appends a newline
to the message sent to the board.
Included the Wire (TWI) library from Wiring.
Updated the reference.

0005 - 2006.09.26

Applied patch from Hans Steiner to improve Linux support by searching for avr
tools in the user's path instead of expecting them at a fixed location.
Added an upload.verbose preference for help in debugging.
ATmega168 support!
New Wiring-compatible randomSeed(), random(max) and random(min, max) functions
(except operating on longs instead of floats).
Fixed bug that sometimes caused uploading of old versions of a sketch.
Serial monitor nows include an interface to send messages to the Arduino
board.  Pressing return appends a newline, pushing the send button doesn't.
Now displaying "burning bootloader..." and "compiling..." status messages.

0004 - 2006.04.26

Main sketch is now compiled as C++ (instead of C).
Updated avr toolchain.
printInteger(), printHex(), etc. now handle longs.
millis() fixed (now overflows after days, not minutes)
Fixed path to java in Windows run.bat.
Added Matrix and Sprite libraries (written with Nicholas Zambetti).
PWM now working on pin 11 (in addition to pins 9 and 10).
Slowed PWM frequency (on all three PWM pins) to 1KHz.
Now give an error if compiled sketch is too big.
Fixed abs(), min(), max(), and constrain() macros.
Added menu items to the IDE to burn bootloader.
Now display binary sketch size on upload, and give error if too big.
Added C++ serial library.
Resynced with Processing/Wiring IDE code (improved auto-format, faster logging
to serial monitor console, other bug fixes)
New library system.
Updated to latest version of the RXTX serial library; Mac users will need to
rerun macosx_setup.command.

0003 - 2006.01.16

API Changes
Reversed the analog input pins to correspond to newer boards.  This means
a call, for example, to analogRead(0) must be changed to analogRead(5) in
order to read the same physical pin.
Added a printNewline() function (which sends '\n' = ASCII 10).

New Stuff
Reference is included (features native to C not yet documented).
Serial monitor added (click the toolbar button to turn it on or off).  Baud
rate is controlled by the Serial Monitor Baud Rate Menu, defaults to 9600.  Icon
and implementation from Wiring.
Serial port menu now automatically refreshes when opened.
New blue color scheme and icons courtesy of Noah Shibley (colors are hardcoded
into the source to ensure consistency with image files).
Keyspan and FTDI USB drivers included with Mac and Windows distributions.

Bug Fixes
millis() now updates every millisecond instead of every second.
Bootloader included with Windows distribution (it was already in the Mac dist).
Updated icon of the Windows executable.
Now flushing the serial port before uploading (should fix some errors).
Improved accuracy of the delayMicroseconds() function.

Other
Upload rate no longer selectable from a menu within the IDE.  Instead, edit the
serial.download_rate item in the preferences.txt file.
Created Xcode project for building Arduino on the Mac (doesn't yet regenerate
the grammar files or package the distribution); active target should be "App".
Removed unused or unimplemented items from menus.

0002 - 2005.10.05

New build process no longer uses makefiles; now controlled by preferences.txt.
core/ replaced with targets/; can now link against Wiring libraries.
Replaced print() with printString, printInteger, printHex, printByte, etc.
Added menu for selecting serial port speed.
Updated icon.
Bootloader shrunk to less than 1 KB; fuses updated accordingly.
Added serialRead(), serialAvailable(), and delayMicroseconds().

0001 - 2005.08.25 

This is the first released of the unified IDE + language library
it's a terrible hack... but it works. at the moment it's in alpha stage
but it can be used to work.
The processing preprocessor is included but not used.
