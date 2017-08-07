
#ifndef UPRINT_H
#define UPRINT_H

#include <stdint.h>

#include "glue.h"
#include "lwipopts.h" // struct ip_info

#if UDEBUG

void dump (const char* what, const char* data, uint16_t len);
void display_ip32 (const char* pre, uint32_t ip);
void display_mac (const uint8_t* mac);
void display_ip_info (const struct ip_info* i);

#else

#define dump(x,y,z) do { (void)0; } while (0)
#define display_ip32(x,y) do { (void)0; } while (0)
#define display_mac(x) do { (void)0; } while (0)
#define display_ip_info(x) do { (void)0; } while (0)

#endif

#endif // BUFPRINT_H
