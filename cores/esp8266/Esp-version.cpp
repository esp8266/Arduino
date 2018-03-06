
#include <Arduino.h>
#include <user_interface.h>
#include <core_version.h>
#include <lwip/init.h>      // LWIP_VERSION_*
#include <lwipopts.h>       // LWIP_HASH_STR (lwip2)

#define STRHELPER(x) #x
#define STR(x) STRHELPER(x) // stringifier

#ifdef NDEBUG

String EspClass::getFullVersion()
{
    return String();
}

#else // !NDEBUG

static const char arduino_esp8266_git_ver [] PROGMEM = STR(ARDUINO_ESP8266_GIT_DESC);
#if LWIP_VERSION_MAJOR != 1
static const char lwip2_version [] PROGMEM = "/lwIP:" STR(LWIP_VERSION_MAJOR) "." STR(LWIP_VERSION_MINOR) "." STR(LWIP_VERSION_REVISION);
#endif

String EspClass::getFullVersion()
{
    return   String(F("SDK:")) + system_get_sdk_version()
           + F("/Core:") + FPSTR(arduino_esp8266_git_ver)
#if LWIP_VERSION_MAJOR == 1
           + F("/lwIP:") + String(LWIP_VERSION_MAJOR) + "." + String(LWIP_VERSION_MINOR) + "." + String(LWIP_VERSION_REVISION)
#else
           + FPSTR(lwip2_version)
#endif
#if LWIP_VERSION_IS_DEVELOPMENT
             + F("-dev")
#endif
#if LWIP_VERSION_IS_RC
             + F("rc") + String(LWIP_VERSION_RC)
#endif
#ifdef LWIP_HASH_STR
             + "(" + F(LWIP_HASH_STR) + ")"
#endif
           ;
}

#endif // !NDEBUG
