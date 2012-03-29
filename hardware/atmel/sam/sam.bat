set Path=%ARM_GCC_TOOLCHAIN%
export Path

start "libsam" /d"..\..\arduino\sam\system\libsam\build_gcc" /max "cd"
start "libarduino" /d"..\..\arduino\sam\cores\sam\build_gcc" /max "cd"
start "libvariant sam3s-ek" /d"variants\sam3s_ek\build_gcc" /max "cd"
start "libvariant sam3u-ek" /d"variants\sam3u_ek\build_gcc" /max "cd"
start "libvariant sam3x-ek" /d"variants\sam3x_ek\build_gcc" /max "cd"
start "test" /d"..\..\arduino\sam\cores\sam\validation\build_gcc" /max "cd"