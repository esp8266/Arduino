// This include file is a hack to ensure backward compatibility with
// pre 3.0.0 versions of the core.  There was a *lowercase* "i2s.h"
// header which was in this directory, now renamed to "core_esp82i66s.h"
// But, the I2S class has a header, "I2S.h" in uppercase.  On Linux
// the two names are different, but on Windows it's case-insensitive
// so the names conflict.
//
// Avoid the issue by preserving the old i2s.h file and have it redirect
// to I2S.h which will give the ESP8266-specific functions as well as
// the generic I2S class.

#include "../../libraries/I2S/src/I2S.h"
