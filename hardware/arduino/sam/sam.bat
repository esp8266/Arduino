set Path=%ARM_GCC_TOOLCHAIN%
export Path

start "libsam" /d"system\libsam\build_gcc" /max "cd"
start "libarduino" /d"cores\arduino\build_gcc" /max "cd"
rem start "libvariant Arduino Due U" /d"variants\arduino_due_u\build_gcc" /max "cd"
start "libvariant Arduino Due X" /d"variants\arduino_due_x\build_gcc" /max "cd"
rem start "libvariant ADK2" /d"..\..\google\sam\variants\adk2\build_gcc" /max "cd"
rem start "libvariant SAM3X-EK" /d"..\..\atmel\sam\variants\sam3x_ek\build_gcc" /max "cd"
start "test" /d"cores\arduino\validation_usb_host\build_gcc" /max "cd"
