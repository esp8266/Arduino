#pragma once

#include "spiffs_api.h"

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SPIFFS)
extern fs::FS SPIFFS __attribute__((deprecated("SPIFFS has been deprecated. Please consider moving to LittleFS or other filesystems.")));
#endif
