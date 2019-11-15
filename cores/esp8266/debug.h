#ifndef ARD_DEBUG_H
#define ARD_DEBUG_H

#include <stddef.h>
#include <stdint.h>

#ifdef DEBUG_ESP_CORE
#define DEBUGV(fmt, ...) ::printf((PGM_P)PSTR(fmt), ## __VA_ARGS__)
#endif

#ifndef DEBUGV
#define DEBUGV(...) do { (void)0; } while (0)
#endif

#ifdef __cplusplus
void hexdump(const void *mem, uint32_t len, uint8_t cols = 16);
#else
void hexdump(const void *mem, uint32_t len, uint8_t cols);
#endif

#ifdef __cplusplus
extern "C" {
#endif

void __panic_func(const char* file, int line, const char* func) __attribute__((noreturn));
#define panic() __panic_func(PSTR(__FILE__), __LINE__, __func__)

#ifdef DEBUG_ESP_CORE
extern const char* overrideme PROGMEM;
#define IAMSLOW() \
    do { \
        static bool once = false; \
        if (!once) { \
            once = true; \
            DEBUGV(overrideme, (PGM_P)PSTR(__FUNCTION__)); \
        } \
    } while (0)
#else
#define IAMSLOW() do { (void)0; } while (0)
#endif

#ifdef __cplusplus
}
#endif


#endif//ARD_DEBUG_H
