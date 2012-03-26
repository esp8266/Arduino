#*******************************************************
#
#  Connect to J-Link and debug application in sram on SAM3U.
#

# Define 'reset' command
define reset

# Connect to the J-Link gdb server
target remote localhost:2331

# Reset the chip to get to a known state
monitor reset

# Load the program
load

# Reset peripheral (RSTC_CR)
set *0x400e1200 = 0xA5000004

# Initialize PC and stack pointer
mon reg sp=(0x20000000)
#set *0x20000004 = *0x20000004 & 0xFFFFFFFE
mon reg pc=(0x20000004)

info reg

# End of 'reset' command
end