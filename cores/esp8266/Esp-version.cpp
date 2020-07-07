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
#include <lwipopts.h>       // LWIP_HASH_STR (lwip2)
#include <bearssl/bearssl_git.h>  // BEARSSL_GIT short hash

#define STRHELPER(x) #x
#define STR(x) STRHELPER(x) // stringifier

static const char arduino_esp8266_git_ver [] PROGMEM = "/Core:" STR(ARDUINO_ESP8266_GIT_DESC) "=";
#if LWIP_IPV6
static const char lwip_version [] PROGMEM = "/lwIP:IPv6+" LWIP_HASH_STR;
#else
static const char lwip_version [] PROGMEM = "/lwIP:" LWIP_HASH_STR;
#endif
static const char bearssl_version [] PROGMEM = "/BearSSL:" STR(BEARSSL_GIT);

String EspClass::getFullVersion() {
    String s(F("SDK:"));
    s.reserve(127);

    s += system_get_sdk_version();
    s += FPSTR(arduino_esp8266_git_ver);
    s += String(esp8266::coreVersionNumeric());
#if LWIP_VERSION_MAJOR == 1
    s += F("/lwIP:");
    s += LWIP_VERSION_MAJOR;
    s += '.';
    s += LWIP_VERSION_MINOR;
    s += '.';
    s += LWIP_VERSION_REVISION;
#if LWIP_VERSION_IS_DEVELOPMENT
    s += F("-dev");
#endif
#if LWIP_VERSION_IS_RC
    s += F("rc");
    s += String(LWIP_VERSION_RC);
#endif
#else // LWIP_VERSION_MAJOR != 1
    s += FPSTR(lwip_version);
#endif // LWIP_VERSION_MAJOR != 1
    s += FPSTR(bearssl_version);

    return s;
}
