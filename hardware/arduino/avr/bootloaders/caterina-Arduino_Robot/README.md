Building the bootloader for the Arduino Robot
=============================================

The Arduino Robot has two boards featuring the atmega32U4 processor from Atmel. Each one of them is identified as a different board at the USB level and has a different bootloader.

The Arduino Robot Control board has the USB identifier 0x0038. This is the value configured by default in the Makefile. 

The Arduino Robot Motor board has the USB identifier 0x0039. If you want to compile/upload this version of the bootloader, you will have to edit the Makefile, comment away the like dedicated to the PID and uncomment the one that configures such variable accordingly.

The general conditions for using these bootloaders require downloading a specific version of LUFA as explained here:

1. Download the LUFA-111009 file (http://fourwalledcubicle.com/blog/2011/10/lufa-111009-released/).
2. Extract that file directly to the Caterina-Arduino_Robot bootloader directory.
3. Open a command prompt in the Caterina-Arduino_Robot bootloader directory.
4. Type 'make'.
5. Enjoy!

Programming the bootloader for one of the Arduino Robot boards
1. Open a command prompt in the Caterina-Arduino_Robot folder.
2. Connect your programmer- use a 2x3 .1" header, pressed against the programming vias.
3. Edit the make file for it to include the right programmer (e.g. in my lab I have AVRMKII and USBTINY ISP)
4. Type 'make program' into the command prompt.

Differences between this bootoloader and the standard one for Leonardo boards
=============================================================================

This bootloader is different from the one on the standard Leonardo boards. To enter the bootloader, you need to double click the reset button. You need to click twice in less that 3/4 of a second (easy uh?). This bootloader, designed in the first place for the LilypadUSB, seems to be optimal for situations when users are e.g. using their robots in soccer competitions where they make direct manipulation of the board as it runs.
