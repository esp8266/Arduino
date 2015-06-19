#ifndef ARD_DEBUG_H
#define ARD_DEBUG_H

#include <stddef.h>
// #define DEBUGV(...) ets_printf(__VA_ARGS__)
#define DEBUGV(...)

#ifdef __cplusplus
void hexdump(uint8_t *mem, uint32_t len, uint8_t cols = 16);
#else
void hexdump(uint8_t *mem, uint32_t len, uint8_t cols);
#endif

#endif//ARD_DEBUG_H
