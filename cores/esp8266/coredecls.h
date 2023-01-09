
#ifndef __COREDECLS_H
#define __COREDECLS_H

#include "core_esp8266_features.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO: put declarations here, get rid of -Wno-implicit-function-declaration

#include <stddef.h>
#include <stdint.h>
#include <cont.h> // g_pcont declaration

bool can_yield();
void esp_suspend();
void esp_delay(unsigned long ms);
void esp_schedule();
void esp_yield();
void tune_timeshift64 (uint64_t now_us);
bool sntp_set_timezone_in_seconds(int32_t timezone);

void disable_extra4k_at_link_time (void) __attribute__((noinline));
void enable_wifi_enterprise_patch(void) __attribute__((noinline));
void __disableWiFiAtBootTime (void) __attribute__((noinline));
void __real_system_restart_local() __attribute__((noreturn));

uint32_t sqrt32(uint32_t n);
uint32_t crc32(const void* data, size_t length, uint32_t crc);

#ifdef __cplusplus
}

#include <functional>

using BoolCB = std::function<void(bool)>;
using TrivialCB = std::function<void()>;

void settimeofday_cb (BoolCB&& cb);
void settimeofday_cb (const BoolCB& cb);
void settimeofday_cb (const TrivialCB& cb);

// This overload of esp_suspend() performs the blocked callback whenever it is resumed,
// and if that returns true, it immediately suspends again.
template <typename T>
inline void esp_suspend(T&& blocked) {
    do {
        esp_suspend();
    } while (blocked());
}

// Try to delay until timeout_ms has expired since start_ms.
// Returns true if timeout_ms has completely expired on entry.
// Otherwise returns false after delaying for the relative
// remainder of timeout_ms, or an absolute intvl_ms, whichever is shorter.
// The delay may be asynchronously cancelled, before that timeout is reached.
bool esp_try_delay(const uint32_t start_ms, const uint32_t timeout_ms, const uint32_t intvl_ms);

// This overload of esp_delay() delays for a duration of at most timeout_ms milliseconds.
// Whenever it is resumed, as well as every intvl_ms millisconds, it performs
// the blocked callback, and if that returns true, it keeps delaying for the remainder
// of the original timeout_ms period.
template <typename T>
inline void esp_delay(const uint32_t timeout_ms, T&& blocked, const uint32_t intvl_ms) {
    const auto start_ms = millis();
    while (!esp_try_delay(start_ms, timeout_ms, intvl_ms) && blocked()) {
    }
}

// This overload of esp_delay() delays for a duration of at most timeout_ms milliseconds.
// Whenever it is resumed, it performs the blocked callback, and if that returns true,
// it keeps delaying for the remainder of the original timeout_ms period.
template <typename T>
inline void esp_delay(const uint32_t timeout_ms, T&& blocked) {
    esp_delay(timeout_ms, std::forward<T>(blocked), timeout_ms);
}

#endif // __cplusplus

#endif // __COREDECLS_H
