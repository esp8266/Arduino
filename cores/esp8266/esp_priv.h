
#ifndef __ESP_PRIV
#define __ESP_PRIV

#if defined(CORE_MOCK)

constexpr bool __byteAddressable(const void* addr)
{
    (void)addr;
    return true;
}

#else // on hardware

#include <sys/config.h>

// returns true when addr can be used without "pgm_" functions or non32xfer service
constexpr bool __byteAddressable(const void* addr)
{
    return addr < (const void*)(XCHAL_DATARAM0_VADDR + XCHAL_DATARAM0_SIZE);
}

#endif // on hardware

#endif // __ESP_PRIV
