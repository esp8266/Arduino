
#include <coredecls.h>
#include <stdint.h>

uint32_t isqrt32 (uint32_t n)
{
    // http://www.codecodex.com/wiki/Calculate_an_integer_square_root#C

    unsigned int c = 0x8000;
    unsigned int g = 0x8000;

    for(;;)
    {
        if (g*g > n)
            g ^= c;
        c >>= 1;
        if (!c)
            return g;
        g |= c;
    }
}
