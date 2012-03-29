set Path=%ARM_GCC_TOOLCHAIN%
export Path

start "libsam" /d"system\libsam\build_gcc" /max "cd"
start "libarduino" /d"cores\sam\build_gcc" /max "cd"
start "libvariant Arduino Due U" /d"variants\arduino_due_u\build_gcc" /max "cd"
start "libvariant Arduino Due X" /d"variants\arduino_due_x\build_gcc" /max "cd"
start "test" /d"cores\sam\validation\build_gcc" /max "cd"