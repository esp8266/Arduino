
#ifndef __COREDECLS_H
#define __COREDECLS_H

#ifdef __cplusplus
extern "C" {
#endif

// TODO: put declarations here, get rid of -Wno-implicit-function-declaration

#include <stddef.h>
#include <stdint.h>
#include <cont.h> // g_pcont declaration

extern bool timeshift64_is_set;

void esp_yield();
void esp_schedule();
void tune_timeshift64 (uint64_t now_us);
void settimeofday_cb (void (*cb)(void));
void disable_extra4k_at_link_time (void) __attribute__((noinline));

uint32_t sqrt32 (uint32_t n);
uint32_t crc32 (const void* data, size_t length, uint32_t crc = 0xffffffff);
uint32_t crc32_P (const void* data, size_t length, uint32_t crc = 0xffffffff);

#ifdef __cplusplus
}
#endif

#endif // __COREDECLS_H
