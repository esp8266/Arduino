
#ifndef __COREDECLS_H
#define __COREDECLS_H

#ifdef __cplusplus
extern "C" {
#endif

// TODO: put declarations here, get rid of -Wno-implicit-function-declaration

#include <stddef.h>
#include <stdint.h>
#include <cont.h> // g_pcont declaration

bool esp_is_in_cont();
void esp_suspend_from_cont();
void esp_request_for_cont();

void esp_yield() __attribute__ ((deprecated("use esp_suspend_from_cont()")));
void esp_schedule() __attribute__ ((deprecated("use esp_request_for_cont()")));
bool can_yield() __attribute__ ((deprecated("esp_is_in_cont()")));

void tune_timeshift64 (uint64_t now_us);
void disable_extra4k_at_link_time (void) __attribute__((noinline));
bool sntp_set_timezone_in_seconds(int32_t timezone);
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

#endif

#endif // __COREDECLS_H
