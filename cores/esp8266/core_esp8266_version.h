
#ifndef __CORE_ESP8266_VERSION_H
#define __CORE_ESP8266_VERSION_H

#define CORE_ESP8266_MAJOR    (2)
#define CORE_ESP8266_MINOR    (4)
#define CORE_ESP8266_REVISION (2)
#define CORE_ESP8266_RC	      (0)
#define CORE_ESP8266_DEVEL    (1)

#define CORE_ESP8266_VERSION  ((CORE_ESP8266_MAJOR*10000) + (CORE_ESP8266_MINOR*1000) + (CORE_ESP8266_REVISION*100) + (CORE_ESP8266_RC*10) + (CORE_ESP8266_DEVEL))

// CORE_ESP8266_VERSION:
// 2.4.3-dev  (after 2.4.2 release), or            24201
// 2.5.0-dev  (after 2.4.2 release)                24201
// 2.5.0-rc1  first release candidate              24210
// 2.5.0-rc1+ dev after first release candidate    24211
// 2.5.0-rc2  second release candidate             24220
// 2.5.0      release                              25000
// 2.5.1-dev                                       25001
// 2.5.1-rc1                                       25010
// 2.5.1      release                              25100

// for example:
// 24211 should read as:
// "dev after first RC after 2.4.2" (for 2.4.3 or 2.5.0)
// CORE_ESP8266_DEVEL must be increased whenever API changes 24201 -> 24202 -> 24203
// so one can always compare (#if CORE_ESP8266_VERSION < 24201 // previous api...)

#endif // __CORE_ESP8266_ESP8266_VERSION_H
