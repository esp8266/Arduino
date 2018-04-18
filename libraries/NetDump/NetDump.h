
#ifndef __NETDUMP_H
#define __NETDUMP_H

#include <Print.h>

bool netDump_is_ARP (const char* data);
bool netDump_is_IPv4 (const char* data);
bool netDump_is_IPv6 (const char* data);

void netDump    (Print& out, const char* data, size_t size);
void netDumpHex (Print& out, const char* data, size_t size, bool show_hex = true, bool show_ascii = true, size_t per_line = 16);

#endif // __NETDUMP_H
