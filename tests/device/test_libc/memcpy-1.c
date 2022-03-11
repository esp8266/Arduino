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
#define BUFF_SIZE 512

#ifndef BUFF_SIZE
#define BUFF_SIZE 1024
#endif

#ifndef START_COPY
#define START_COPY 256
#endif

#ifndef MAX_BLOCK_SIZE
#define MAX_BLOCK_SIZE 128
#endif

#ifndef MAX_OFFSET
#define MAX_OFFSET 3
#endif

#if (START_COPY + MAX_OFFSET + MAX_BLOCK_SIZE >= BUFF_SIZE)
#error "Buffer overrun: START_COPY + MAX_OFFSET + MAX_BLOCK_SIZE >= BUFF_SIZE."
#endif

#define TOO_MANY_ERRORS 11
static int errors = 0;

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
void       memcpy_main(void)
{
    /* Allocate buffers to read and write from.  */
    char src[BUFF_SIZE], dest[BUFF_SIZE], backup_src[BUFF_SIZE];

    /* Fill the source buffer with non-null values, reproducible random data. */
    srand(rand_seed);
    int      i, j;
    unsigned sa;
    unsigned da;
    unsigned n;
    for (i = 0; i < BUFF_SIZE; i++)
    {
        src[i]        = (char)rand() | 1;
        backup_src[i] = src[i];
    }

    /* Make calls to memcpy with block sizes ranging between 1 and
       MAX_BLOCK_SIZE bytes, aligned and misaligned source and destination.  */
    for (sa = 0; sa <= MAX_OFFSET; sa++)
        for (da = 0; da <= MAX_OFFSET; da++)
            for (n = 1; n <= MAX_BLOCK_SIZE; n++)
            {
                // printf (".");
                /* Zero dest so we can check it properly after the copying.  */
                for (j = 0; j < BUFF_SIZE; j++)
                    dest[j] = 0;

                void* ret = memcpy(dest + START_COPY + da, src + sa, n);

                /* Check return value.  */
                if (ret != (dest + START_COPY + da))
                    print_error("\nFailed: wrong return value in memcpy of %u bytes "
                                "with src_align %u and dst_align %u. "
                                "Return value and dest should be the same"
                                "(ret is %p, dest is %p)\n",
                                n, sa, da, ret, dest + START_COPY + da);

                /* Check that content of the destination buffer
                   is the same as the source buffer, and
                   memory outside destination buffer is not modified.  */
                for (j = 0; j < BUFF_SIZE; j++)
                    if ((unsigned)j < START_COPY + da)
                    {
                        if (dest[j] != 0)
                            print_error("\nFailed: after memcpy of %u bytes "
                                        "with src_align %u and dst_align %u, "
                                        "byte %u before the start of dest is not 0.\n",
                                        n, sa, da, START_COPY - j);
                    }
                    else if ((unsigned)j < START_COPY + da + n)
                    {
                        i = j - START_COPY - da;
                        if (dest[j] != (src + sa)[i])
                            print_error("\nFailed: after memcpy of %u bytes "
                                        "with src_align %u and dst_align %u, "
                                        "byte %u in dest and src are not the same.\n",
                                        n, sa, da, i);
                    }
                    else if (dest[j] != 0)
                    {
                        print_error("\nFailed: after memcpy of %u bytes "
                                    "with src_align %u and dst_align %u, "
                                    "byte %u after the end of dest is not 0.\n",
                                    n, sa, da, j - START_COPY - da - n);
                    }

                /* Check src is not modified.  */
                for (j = 0; j < BUFF_SIZE; j++)
                    if (src[i] != backup_src[i])
                        print_error("\nFailed: after memcpy of %u bytes "
                                    "with src_align %u and dst_align %u, "
                                    "byte %u of src is modified.\n",
                                    n, sa, da, j);
            }

    if (errors != 0)
        abort();

    printf("ok\n");
}
