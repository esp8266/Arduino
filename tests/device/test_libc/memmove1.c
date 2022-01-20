/* A minor test-program for memmove.
   Copyright (C) 2005 Axis Communications.
   All rights reserved.

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

   2. Neither the name of Axis Communications nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY AXIS COMMUNICATIONS AND ITS CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL AXIS
   COMMUNICATIONS OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
   INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
   SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
   STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
   IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.  */

/* Test moves of 0..MAX bytes; overlapping-src-higher,
   overlapping-src-lower and non-overlapping.  The overlap varies with
   1..N where N is the size moved.  This means an order of MAX**2
   iterations.  The size of an octet may seem appropriate for MAX and
   makes an upper limit for simple testing.  For the CRIS simulator,
   making this 256 added 90s to the test-run (2GHz P4) while 64 (4s) was
   enough to spot the bugs that had crept in, hence the number chosen.  */
#define MAX 64

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define memcmp memcmp_P
#define memcpy memcpy_P
#define memmem memmem_P
#define memchr memchr_P
#define strcat strcat_P
#define strncat strncat_P
#define strcpy strcpy_P
#define strncpy strncpy_P
#define strlen strlen_P
#define strnlen strnlen_P
#define strcmp strcmp_P
#define strncmp strncmp_P

#define TOO_MANY_ERRORS 11
int errors = 0;

#define DEBUGP                              \
    if (errors == TOO_MANY_ERRORS)          \
        printf("Further errors omitted\n"); \
    else if (errors < TOO_MANY_ERRORS)      \
    printf

/* A safe target-independent memmove.  */

void mymemmove(unsigned char* dest, unsigned char* src, size_t n)
{
    if ((src <= dest && src + n <= dest)
        || src >= dest)
        while (n-- > 0)
            *dest++ = *src++;
    else
    {
        dest += n;
        src += n;
        while (n-- > 0)
            *--dest = *--src;
    }
}

/* It's either the noinline attribute or forcing the test framework to
   pass -fno-builtin-memmove.  */
void xmemmove(unsigned char* dest, unsigned char* src, size_t n)
    __attribute__((__noinline__));

void xmemmove(unsigned char* dest, unsigned char* src, size_t n)
{
    void* retp;
    retp = memmove(dest, src, n);

    if (retp != dest)
    {
        errors++;
        DEBUGP("memmove of n bytes returned %p instead of dest=%p\n",
            retp, dest);
    }
}

/* Fill the array with something we can associate with a position, but
   not exactly the same as the position index.  */

void fill(unsigned char dest[MAX * 3])
{
    size_t i;
    for (i = 0; i < MAX * 3; i++)
        dest[i] = (10 + i) % MAX;
}

void memmove_main(void)
{
    size_t i;
    int errors = 0;

    /* Leave some room before and after the area tested, so we can detect
     overwrites of up to N bytes, N being the amount tested.  If you
     want to test using valgrind, make these malloced instead.  */
    unsigned char from_test[MAX * 3];
    unsigned char to_test[MAX * 3];
    unsigned char from_known[MAX * 3];
    unsigned char to_known[MAX * 3];

    /* Non-overlap.  */
    for (i = 0; i < MAX; i++)
    {
        /* Do the memmove first before setting the known array, so we know
         it didn't change any of the known array.  */
        fill(from_test);
        fill(to_test);
        xmemmove(to_test + MAX, 1 + from_test + MAX, i);

        fill(from_known);
        fill(to_known);
        mymemmove(to_known + MAX, 1 + from_known + MAX, i);

        if (memcmp(to_known, to_test, sizeof(to_known)) != 0)
        {
            errors++;
            DEBUGP("memmove failed non-overlap test for %d bytes\n", i);
        }
    }

    /* Overlap-from-before.  */
    for (i = 0; i < MAX; i++)
    {
        size_t j;
        for (j = 0; j < i; j++)
        {
            fill(to_test);
            xmemmove(to_test + MAX * 2 - i, to_test + MAX * 2 - i - j, i);

            fill(to_known);
            mymemmove(to_known + MAX * 2 - i, to_known + MAX * 2 - i - j, i);

            if (memcmp(to_known, to_test, sizeof(to_known)) != 0)
            {
                errors++;
                DEBUGP("memmove failed for %d bytes,"
                       " with src %d bytes before dest\n",
                    i, j);
            }
        }
    }

    /* Overlap-from-after.  */
    for (i = 0; i < MAX; i++)
    {
        size_t j;
        for (j = 0; j < i; j++)
        {
            fill(to_test);
            xmemmove(to_test + MAX, to_test + MAX + j, i);

            fill(to_known);
            mymemmove(to_known + MAX, to_known + MAX + j, i);

            if (memcmp(to_known, to_test, sizeof(to_known)) != 0)
            {
                errors++;
                DEBUGP("memmove failed when moving %d bytes,"
                       " with src %d bytes after dest\n",
                    i, j);
            }
        }
    }

    if (errors != 0)
        abort();
    printf("ok\n");
}
