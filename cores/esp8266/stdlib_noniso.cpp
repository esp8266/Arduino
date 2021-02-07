
#include "stdlib_noniso.h"

// fill backwards
char* ulltoa(unsigned long long val, char* str, int slen, unsigned long long radix)
{
    str += --slen;
    *str = 0;
    do
    {
#if 1
        // using div and mod in a single call
        // String(ULLMAX_LONG, 10) => 354us
        unsigned long long mod;
        val = udivmod(val, radix, mod);
#else
        // String(ULLMAX_LONG, 10) => 374us
        auto mod = val % radix;
        val /= radix;
#endif
        *--str = mod + ((mod > 9) ? ('a' - 10) : '0');
    } while (--slen && val);
    return val? nullptr: str;
}

char* lltoa (long long val, char* str, int slen, long long radix)
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
