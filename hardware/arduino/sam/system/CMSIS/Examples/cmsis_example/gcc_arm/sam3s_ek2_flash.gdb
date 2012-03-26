#*******************************************************
#
#  Connect to J-Link and debug application in flash on SAM3S.
#

# Define 'reset' command
define reset

# Connect to the J-Link gdb server
target remote localhost:2331

# Reset the chip to get to a known state
monitor reset

# Select flash device
monitor flash device = AT91SAM3S4C

# Enable flash download and flash breakpoints
monitor flash download = 1

# Load the program
load

# Reset peripheral (RSTC_CR)
set *0x400e1400 = 0xA5000004

# Initialize PC and stack pointer
mon reg sp=(0x400000)
#set *0x400004 = *0x400004 & 0xFFFFFFFE
mon reg pc=(0x400004)

info reg

# End of 'reset' command
end
