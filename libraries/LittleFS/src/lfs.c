// Can't place library in this src/ directory, Arduino will attempt to build the tests/etc.
// Just have a stub here that redirects to the actual source file

#pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#define LFS_NAME_MAX 32
#define LFS_NO_DEBUG
#define LFS_NO_WARN
#define LFS_NO_ERROR

#include "../lib/littlefs/lfs.c"
