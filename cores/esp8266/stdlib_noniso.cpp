
#include "stdlib_noniso.h"

// fill backwards
char* ulltoa(unsigned long long val, char* str, size_t slen, uint_fast8_t radix)
{
    str += --slen;
    *str = 0;
    do
    {
        auto n = val % radix;
        val /= radix;
        *--str = n + ((n > 9) ? ('a' - 10) : '0');
    } while (--slen && val);
    return val? nullptr: str;
}

char* lltoa (long long val, char* str, size_t slen, uint_fast8_t radix)
{
    bool neg;
    if (val < 0)
    {
        val = -val;
        neg = true;
    }
    else
    {
        neg = false;
    }
    char* ret = ulltoa(val, str, slen, radix);
    if (neg)
    {
        if (ret == str)
            return nullptr;
        *--ret = '-';
    }
    return ret;
}
