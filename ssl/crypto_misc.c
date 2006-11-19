/*
 *  Copyright(C) 2006
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * @file misc.c
 *
 * Some misc. routines to help things out
 */

#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include "crypto.h"
#ifdef CONFIG_WIN32_USE_CRYPTO_LIB
#include "wincrypt.h"
#endif

#define BM_RECORD_OFFSET 	5	/* same as SSL_RECORD_SIZE */

#ifndef WIN32
static int rng_fd = -1;
#elif defined(CONFIG_WIN32_USE_CRYPTO_LIB)
static HCRYPTPROV gCryptProv;
#endif

#if (!defined(CONFIG_USE_DEV_URANDOM) && !defined(CONFIG_WIN32_USE_CRYPTO_LIB))
static uint64_t rng_num;
#endif

static int rng_ref_count;
const char * const unsupported_str = "Error: feature not supported\n";

/**
 * Allocate a new memory buffer 
 */
BUF_MEM buf_new()
{
    BUF_MEM bm;
    bm.pre_data = (uint8_t *)malloc(2048); /* should be enough to start with */
    bm.data = bm.pre_data+BM_RECORD_OFFSET; /* some space at the start */
    bm.max_len = 2048-BM_RECORD_OFFSET;
    bm.index = 0;
    return bm;
}

/**
 * Grow a buffer if necessary
 */
void buf_grow(BUF_MEM *bm, int len)
{
    if (len <= bm->max_len)
    {
        return;
    }

    /* add 1kB just to be sure */
    bm->pre_data = (uint8_t *)realloc(bm->pre_data, len+1024+BM_RECORD_OFFSET); 
    bm->data = bm->pre_data+BM_RECORD_OFFSET;
    bm->max_len = len+1024;
}

/**
 * Free a buffer
 */
void buf_free(BUF_MEM *bm)
{
    free(bm->pre_data);
    bm->pre_data = NULL;
    bm->data = NULL;
}

#ifndef CONFIG_SSL_SKELETON_MODE
/** 
 * Retrieve a file and put it into memory
 * @return The size of the file, or -1 on failure.
 */
int get_file(const char *filename, uint8_t **buf)
{
    int total_bytes = 0;
    int bytes_read = 0; 
    int filesize;
    FILE *stream = fopen(filename, "rb");

    if (stream == NULL)
    {
        return -1;
    }
    
    /* Win CE doesn't support stat() */
    fseek(stream, 0, SEEK_END);
    filesize = ftell(stream);
    *buf = (uint8_t *)malloc(filesize);
    fseek(stream, 0, SEEK_SET);

    do
    {
        bytes_read = fread(*buf+total_bytes, 1, filesize-total_bytes, stream);
        total_bytes += bytes_read;
    } while (total_bytes < filesize && bytes_read > 0);
    
    fclose(stream);
    return filesize;
}
#endif

/**
 * Initialise the Random Number Generator engine.
 * - On Win32 use the platform SDK's crypto engine.
 * - On Linux use /dev/urandom
 * - If none of these work then use a custom RNG.
 */
void RNG_initialize(const uint8_t *seed_buf, int size)
{
    if (rng_ref_count == 0)
    {
#if !defined(WIN32) && defined(CONFIG_USE_DEV_URANDOM)
        if ((rng_fd = open("/dev/urandom", O_RDONLY)) < 0)
        {
            printf(unsupported_str);
            exit(1);
        }
#elif defined(WIN32) && defined(CONFIG_WIN32_USE_CRYPTO_LIB)
        if (!CryptAcquireContext(&gCryptProv, 
                          NULL, NULL, PROV_RSA_FULL, 0))
        {
            printf("%s CryptoLib %x", unsupported_str, GetLastError());
            exit(1);
        }
#else   
        /* help seed with the user's private key - this is a number that 
           should be hard to find, due to the fact that it relies on knowing 
           the private key */
        int i;  

        for (i = 0; i < size/(int)sizeof(uint64_t); i++)
        {
            rng_num ^= *((uint64_t *)&seed_buf[i*sizeof(uint64_t)]);
        }

        srand((long)seed_buf);  /* use the stack ptr as another rnd seed */
#endif
    }

    rng_ref_count++;
}

/**
 * Terminate the RNG engine.
 */
void RNG_terminate(void)
{
    if (--rng_ref_count == 0)
    {
#ifndef WIN32
        close(rng_fd);
#elif defined(CONFIG_WIN32_USE_CRYPTO_LIB)
        CryptReleaseContext(gCryptProv, 0);
#endif
    }
}

/**
 * Set a series of bytes with a random number. Individual bytes can be 0
 */
void get_random(int num_rand_bytes, uint8_t *rand_data)
{   
#if !defined(WIN32) && defined(CONFIG_USE_DEV_URANDOM)
    /* use the Linux default */
    read(rng_fd, rand_data, num_rand_bytes);    /* read from /dev/urandom */
#elif defined(WIN32) && defined(CONFIG_WIN32_USE_CRYPTO_LIB)
    /* use Microsoft Crypto Libraries */
    CryptGenRandom(gCryptProv, num_rand_bytes, rand_data);
#else   /* nothing else to use, so use a custom RNG */
    /* The method we use when we've got nothing better. Use RC4, time 
       and a couple of random seeds to generate a random sequence */
    RC4_CTX rng_ctx;
    struct timeval tv;
    uint64_t big_num1, big_num2;

    gettimeofday(&tv, NULL);    /* yes I know we shouldn't do this */

    /* all numbers by themselves are pretty simple, but combined should 
     * be a challenge */
    big_num1 = (uint64_t)tv.tv_sec*(tv.tv_usec+1); 
    big_num2 = (uint64_t)rand()*big_num1;
    big_num1 ^= rng_num;

    memcpy(rand_data, &big_num1, sizeof(uint64_t));
    if (num_rand_bytes > sizeof(uint64_t))
    {
        memcpy(&rand_data[8], &big_num2, sizeof(uint64_t));
    }

    if (num_rand_bytes > 16)
    {
        /* clear rest of data */
        memset(&rand_data[16], 0, num_rand_bytes-16); 
    }

    RC4_setup(&rng_ctx, rand_data, 16); /* use as a key */
    RC4_crypt(&rng_ctx, rand_data, rand_data, num_rand_bytes);
    
    /* use last 8 bytes for next time */
    memcpy(&rng_num, &rand_data[num_rand_bytes-8], sizeof(uint64_t));    
#endif
}

/**
 * Set a series of bytes with a random number. Individual bytes are not zero.
 */
void get_random_NZ(int num_rand_bytes, uint8_t *rand_data)
{
    int i;
    get_random(num_rand_bytes, rand_data);

    for (i = 0; i < num_rand_bytes; i++)
    {
        while (rand_data[i] == 0)  /* can't be 0 */
        {
            rand_data[i] = (uint8_t)(rand());
        }
    }
}

/**
 * Some useful diagnostic routines
 */
#if defined(CONFIG_SSL_FULL_MODE) || defined(CONFIG_DEBUG)
int hex_finish;
int hex_index;

static void print_hex_init(int finish)
{
    hex_finish = finish;
    hex_index = 0;
}

static void print_hex(uint8_t hex)
{
    static int column;

    if (hex_index == 0)
    {
        column = 0;
    }

    printf("%02x ", hex);
    if (++column == 8)
    {
        printf(": ");
    }
    else if (column >= 16)
    {
        printf("\n");
        column = 0;
    }

    if (++hex_index >= hex_finish && column > 0)
    {
        printf("\n");
    }
}

/**
 * Spit out a blob of data for diagnostics. The data is is a nice column format
 * for easy reading.
 *
 * @param format   [in]    The string (with possible embedded format characters)
 * @param size     [in]    The number of numbers to print
 * @param data     [in]    The start of data to use
 * @param ...      [in]    Any additional arguments
 */
void print_blob(const char *format, 
        const uint8_t *data, int size, ...)
{
    int i;
    char tmp[80];
    va_list(ap);

    va_start(ap, size);
    sprintf(tmp, "%s\n", format);
    vprintf(tmp, ap);
    print_hex_init(size);
    for (i = 0; i < size; i++)
    {
        print_hex(data[i]);
    }

    va_end(ap);
    TTY_FLUSH();
}
#elif defined(WIN32)
/* VC6.0 doesn't handle variadic macros */
void print_blob(const char *format, const unsigned char *data,
        int size, ...) {}
#endif

