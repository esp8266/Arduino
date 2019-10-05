
#ifndef __LWIPTOOLS
#define __LWIPTOOLS

#include "IPAddress.h"

// reorder WiFi.config() parameters for a esp8266/official Arduino dual-compatibility API

bool ipAddressReorder (const IPAddress& local_ip, const IPAddress& arg1, const IPAddress& arg2, const IPAddress& arg3,
                       IPAddress& gateway, IPAddress& subnet, IPAddress& dns1);

#endif __LWIP_TOOLS
