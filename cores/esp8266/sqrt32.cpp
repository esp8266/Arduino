
#include <coredecls.h>
#include <stdint.h>

extern "C" {

    uint32_t sqrt32(uint32_t n)
    {
        // http://www.codecodex.com/wiki/Calculate_an_integer_square_root#C
        // Another very fast algorithm donated by Tristan.Muntsinger@gmail.com
        // (note: tested across the full 32 bits range, see comment below)

        // 15 iterations (c=1<<15)

        unsigned int c = 0x8000;
        unsigned int g = 0x8000;

        for (;;)
        {
            if (g * g > n)
            {
                g ^= c;
            }
            c >>= 1;
            if (!c)
            {
                return g;
            }
            g |= c;
        }
    }

    /*
        tested with:


        #include <stdio.h>
        #include <stdint.h>
        #include <math.h>

        int main (void)
        {
        for (uint32_t i = 0; ++i; )
        {
            uint32_t sr = sqrt32(i);
            uint32_t ifsr = sqrt(i);

            if (ifsr != sr)
                printf("%d: i%d f%d\n", i, sr, ifsr);

            if (!(i & 0xffffff))
            {
                printf("%i%% (0x%08x)\r", ((i >> 16) * 100) >> 16, i);
                fflush(stdout);
            }
        }

        printf("\n");
        }


    */

};
