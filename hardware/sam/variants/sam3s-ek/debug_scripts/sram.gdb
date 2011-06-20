#*************************************************
#
#  Connect to J-Link and debug application in sram on SAM3S
#
# Note:
#     First, users should modify Step1 and Step2 according to their project,
#     then do Step3.

# Step1: Connect to the J-Link gdb server
define reset
target remote localhost:2331
monitor reset

# Step2: Load file(eg. getting-started project)
load

# Step3: Reset peripheral  (RSTC_CR)
set *0x400e1400 = 0xA5000004

# Step4: Initializing PC and stack pointer
# Modify pc value to even before writing pc register
mon reg sp=(0x20000000)
set *0x20000004 = *0x20000004 & 0xFFFFFFFE
mon reg pc=(0x20000004)
info reg

end
