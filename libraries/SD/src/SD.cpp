#include "SD.h"

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SD)
SDClass SD;
#endif

void (*__SD__userDateTimeCB)(uint16_t*, uint16_t*) = nullptr;
