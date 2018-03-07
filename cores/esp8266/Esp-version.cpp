/*
 Esp.cpp - ESP8266-specific APIs
 Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <Arduino.h>
#include <user_interface.h>
#include <core_version.h>
#include <lwip/init.h>      // LWIP_VERSION_*
#include <lwipopts.h>       // LWIP_HASH_STR (lwip2)

#define STRHELPER(x) #x
#define STR(x) STRHELPER(x) // stringifier

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
