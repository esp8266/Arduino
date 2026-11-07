#pragma once

#include "spiffs_api.h"
#include <FS.h>

namespace fs {

class SPIFFSConfig : public FSConfig
{
public:
    static constexpr uint32_t FSId = 0x53504946;
    SPIFFSConfig(bool autoFormat = true) : FSConfig(FSId, autoFormat) { }

    // Inherit _type and _autoFormat
    // nothing yet, enableTime TBD when SPIFFS has metadate
};

}  // namespace fs

using fs::SPIFFSConfig;

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SPIFFS)
extern fs::FS SPIFFS __attribute__((deprecated("SPIFFS has been deprecated. Please consider moving to LittleFS or other filesystems.")));
#endif
