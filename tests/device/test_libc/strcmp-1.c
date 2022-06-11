/*
 * Copyright (c) 2011 ARM Ltd
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the company may not be used to endorse or promote
 *    products derived from this software without specific prior written
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ARM LTD ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ARM LTD BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

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

#define BUFF_SIZE 256

/* The macro LONG_TEST controls whether a short or a more comprehensive test
   of strcmp should be performed.  */
#ifdef LONG_TEST
#ifndef BUFF_SIZE
#define BUFF_SIZE 1024
#endif

#ifndef MAX_BLOCK_SIZE
#define MAX_BLOCK_SIZE 128
#endif

#ifndef MAX_OFFSET
#define MAX_OFFSET 3
#endif

#ifndef MAX_DIFF
#define MAX_DIFF 8
#endif

#ifndef MAX_LEN
#define MAX_LEN 8
#endif

#ifndef MAX_ZEROS
#define MAX_ZEROS 8
#endif
#else /* not defined LONG_TEST */
#ifndef BUFF_SIZE
#define BUFF_SIZE 1024
#endif

#ifndef MAX_BLOCK_SIZE
#define MAX_BLOCK_SIZE 64
#endif

#ifndef MAX_OFFSET
#define MAX_OFFSET 3
#endif

#ifndef MAX_DIFF
#define MAX_DIFF 4
#endif

#ifndef MAX_LEN
#define MAX_LEN 4
#endif

#ifndef MAX_ZEROS
#define MAX_ZEROS 4
#endif
#endif /* not defined LONG_TEST */

#if (MAX_OFFSET >= 26)
#error "MAX_OFFSET >= 26"
#endif
#if (MAX_OFFSET + MAX_BLOCK_SIZE + MAX_DIFF + MAX_LEN + MAX_ZEROS >= BUFF_SIZE)
#error "Buffer overrun: MAX_OFFSET + MAX_BLOCK_SIZE + MAX_DIFF + MAX_LEN + MAX_ZEROS >= BUFF_SIZE."
#endif

#define TOO_MANY_ERRORS 11
static int errors = 0;

const char* testname = "strcmp";

static void print_error(char const* msg, ...)
{
    errors++;
    if (errors == TOO_MANY_ERRORS)
    {
        fprintf(stderr, "Too many errors.\n");
    }
    else if (errors < TOO_MANY_ERRORS)
    {
        va_list ap;
        va_start(ap, msg);
        vfprintf(stderr, msg, ap);
        va_end(ap);
    }
    else
    {
        /* Further errors omitted.  */
    }
}

extern int rand_seed;
void       strcmp_main(void)
{
    /* Allocate buffers to read and write from.  */
    char src[BUFF_SIZE], dest[BUFF_SIZE];

    /* Fill the source buffer with non-null values, reproducible random data. */
    srand(rand_seed);
    int      i, j, zeros;
    unsigned sa;
    unsigned da;
    unsigned n, m, len;
    char*    p;
    int      ret;

    /* Make calls to strcmp with block sizes ranging between 1 and
       MAX_BLOCK_SIZE bytes, aligned and misaligned source and destination.  */
    for (sa = 0; sa <= MAX_OFFSET; sa++)
        for (da = 0; da <= MAX_OFFSET; da++)
            for (n = 1; n <= MAX_BLOCK_SIZE; n++)
            {
                for (m = 1; m < n + MAX_DIFF; m++)
                    for (len = 0; len < MAX_LEN; len++)
                        for (zeros = 1; zeros < MAX_ZEROS; zeros++)
                        {
                            if (n - m > MAX_DIFF)
                                continue;
                            /* Make a copy of the source.  */
                            for (i = 0; i < BUFF_SIZE; i++)
                            {
                                src[i]  = 'A' + (i % 26);
                                dest[i] = src[i];
                            }
                            delay(0);
                            memcpy(dest + da, src + sa, n);

                            /* Make src 0-terminated.  */
                            p = src + sa + n - 1;
                            for (i = 0; i < zeros; i++)
                            {
                                *p++ = '\0';
                            }

                            /* Modify dest.  */
                            p = dest + da + m - 1;
                            for (j = 0; j < (int)len; j++)
                                *p++ = 'x';
                            /* Make dest 0-terminated.  */
                            *p = '\0';

                            ret = strcmp(src + sa, dest + da);

                            /* Check return value.  */
                            if (n == m)
                            {
                                if (len == 0)
                                {
                                    if (ret != 0)
                                    {
                                        print_error("\nFailed: after %s of %u bytes "
                                                    "with src_align %u and dst_align %u, "
                                                    "dest after %d bytes is modified for %d bytes, "
                                                    "return value is %d, expected 0.\n",
                                                    testname, n, sa, da, m, len, ret);
                                    }
                                }
                                else
                                {
                                    if (ret >= 0)
                                        print_error("\nFailed: after %s of %u bytes "
                                                    "with src_align %u and dst_align %u, "
                                                    "dest after %d bytes is modified for %d bytes, "
                                                    "return value is %d, expected negative.\n",
                                                    testname, n, sa, da, m, len, ret);
                                }
                            }
                            else if (m > n)
                            {
                                if (ret >= 0)
                                {
                                    print_error("\nFailed: after %s of %u bytes "
                                                "with src_align %u and dst_align %u, "
                                                "dest after %d bytes is modified for %d bytes, "
                                                "return value is %d, expected negative.\n",
                                                testname, n, sa, da, m, len, ret);
                                }
                            }
                            else /* m < n */
                            {
                                if (len == 0)
                                {
                                    if (ret <= 0)
                                        print_error("\nFailed: after %s of %u bytes "
                                                    "with src_align %u and dst_align %u, "
                                                    "dest after %d bytes is modified for %d bytes, "
                                                    "return value is %d, expected positive.\n",
                                                    testname, n, sa, da, m, len, ret);
                                }
                                else
                                {
                                    if (ret >= 0)
                                        print_error("\nFailed: after %s of %u bytes "
                                                    "with src_align %u and dst_align %u, "
                                                    "dest after %d bytes is modified for %d bytes, "
                                                    "return value is %d, expected negative.\n",
                                                    testname, n, sa, da, m, len, ret);
                                }
                            }
                        }
            }

    /* Check some corner cases.  */
    src[1]  = 'A';
    dest[1] = 'A';
    src[2]  = 'B';
    dest[2] = 'B';
    src[3]  = 'C';
    dest[3] = 'C';
    src[4]  = '\0';
    dest[4] = '\0';

    src[0]  = 0xc1;
    dest[0] = 0x41;
    ret     = strcmp(src, dest);
    if (ret <= 0)
        print_error("\nFailed: expected positive, return %d\n", ret);

    src[0]  = 0x01;
    dest[0] = 0x82;
    ret     = strcmp(src, dest);
    if (ret >= 0)
        print_error("\nFailed: expected negative, return %d\n", ret);

    dest[0] = src[0] = 'D';
    src[3]           = 0xc1;
    dest[3]          = 0x41;
    ret              = strcmp(src, dest);
    if (ret <= 0)
        print_error("\nFailed: expected positive, return %d\n", ret);

    src[3]  = 0x01;
    dest[3] = 0x82;
    ret     = strcmp(src, dest);
    if (ret >= 0)
        print_error("\nFailed: expected negative, return %d\n", ret);

    // printf ("\n");
    if (errors != 0)
    {
        printf("ERROR. FAILED.\n");
        abort();
    }
    // exit (0);
    printf("ok\n");
}
