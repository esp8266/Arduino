
#ifndef __CORE_ESP8266_VERSION_H
#define __CORE_ESP8266_VERSION_H

#define CORE_ESP8266_MAJOR    2
#define CORE_ESP8266_MINOR    4
#define CORE_ESP8266_REVISION 2
#define CORE_ESP8266_CBNR     0
#define CORE_ESP8266_DEVEL    1

// CBNR = candidate before next release
// CORE_ESP8266_DEVEL must be increased whenever API changes (api signature update, or api addition, or api removal)

#define CORE_ESP8266_VERSION  ((CORE_ESP8266_MAJOR*10000) + (CORE_ESP8266_MINOR*1000) + (CORE_ESP8266_REVISION*100) + (CORE_ESP8266_CBNR*10) + (CORE_ESP8266_DEVEL))

// CORE_ESP8266_VERSION:
// 2.4.3-dev  (after 2.4.2 release), or            24201
// 2.5.0-dev  (after 2.4.2 release)                24201
// 2.5.0-rc1  first candidate before next release  24210
// 2.5.0-rc1+ dev after first candidate            24211
// 2.5.0-rc2  second candidate                     24220
// 2.5.0      release                              25000
// 2.5.1-dev                                       25001
// 2.5.1-rc1                                       25010
// 2.5.1      release                              25100

// for example:
// 24211 should read as:
// "dev after first candidate before next release after 2.4.2" (for either pre-2.4.3 or pre-2.5.0)

#endif // __CORE_ESP8266_ESP8266_VERSION_H
