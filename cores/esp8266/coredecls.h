
#ifndef __COREDECLS_H
#define __COREDECLS_H

#ifdef __cplusplus
extern "C" {
#endif

// TODO: put declarations here, get rid of -Wno-implicit-function-declaration

#include <stddef.h>
#include <stdint.h>
#include <cont.h> // g_pcont declaration

bool can_yield();
void esp_yield();
void esp_delay(unsigned long ms);
void esp_schedule();
void tune_timeshift64 (uint64_t now_us);
void disable_extra4k_at_link_time (void) __attribute__((noinline));
bool sntp_set_timezone_in_seconds(int32_t timezone);
void __disableWiFiAtBootTime (void) __attribute__((noinline));
void __real_system_restart_local() __attribute__((noreturn));

uint32_t sqrt32 (uint32_t n);
uint32_t crc32 (const void* data, size_t length, uint32_t crc = 0xffffffff);

#ifdef __cplusplus
}

#include <functional>

using BoolCB = std::function<void(bool)>;
using TrivialCB = std::function<void()>;

void settimeofday_cb (const BoolCB& cb);
void settimeofday_cb (const TrivialCB& cb);

inline void esp_yield(const std::function<bool()>& blocked) {
    do {
        esp_yield();
    } while (blocked());
}

void esp_delay(const uint32_t timeout_ms, const std::function<bool()>& blocked, const uint32_t intvl_ms);

inline void esp_delay(const uint32_t timeout_ms, const std::function<bool()>& blocked) {
    esp_delay(timeout_ms, blocked, timeout_ms);
}

#endif // __cplusplus

#endif // __COREDECLS_H
