#ifndef ARD_DEBUG_H
#define ARD_DEBUG_H

#include <stddef.h>

extern "C" size_t ets_printf(const char*, ...);
#define DEBUGV ets_printf


#endif//ARD_DEBUG_H
