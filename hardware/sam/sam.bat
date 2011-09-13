start "libsam" /d"system\libsam\build_gcc" /max "cd"
start "libarduino" /d"cores\sam\build_gcc" /max "cd"
start "libvariant sam3u-ek" /d"variants\sam3u_ek\build_gcc" /max "cd"
start "test" /d"cores\sam\validation\build_gcc" /max "cd"