
#ifndef __COREDECLS_H
#define __COREDECLS_H

#ifdef __cplusplus
extern "C" {
#endif

// TODO: put declarations here, get rid of -Wno-implicit-function-declaration

extern bool timeshift64_is_set;

void esp_yield();
void esp_schedule();
void tune_timeshift64 (uint64_t now_us);
void settimeofday_cb (void (*cb)(void));

// calls to this function must *always* be inlined
void disable_extra4k_at_link_time (void) __attribute__((noinline));

#ifdef __cplusplus
}
#endif

#endif // __COREDECLS_H
