#include "SD.h"

static_assert(__builtin_strcmp(SDClassFileMode(FILE_READ), "r") == 0, "");
static_assert(__builtin_strcmp(SDClassFileMode(FILE_WRITE), "a+") == 0, "");

static_assert(__builtin_strcmp(SDClassFileMode(O_RDONLY), "r") == 0, "");
static_assert(__builtin_strcmp(SDClassFileMode(O_WRONLY), "w+") == 0, "");
static_assert(__builtin_strcmp(SDClassFileMode(O_RDWR), "w+") == 0, "");
static_assert(__builtin_strcmp(SDClassFileMode(O_WRONLY | O_APPEND), "a") == 0, "");
static_assert(__builtin_strcmp(SDClassFileMode(O_RDWR | O_APPEND), "a+") == 0, "");

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SD)
SDClass SD;
#endif

void (*__SD__userDateTimeCB)(uint16_t*, uint16_t*) = nullptr;
