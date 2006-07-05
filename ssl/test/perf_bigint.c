/*
 *  Copyright(C) 2006
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This license is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this license; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * @file perf_bigint.c
 *
 * Some performance testing of bigint.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ssl.h"

/**************************************************************************
 * BIGINT tests 
 *
 **************************************************************************/

int main(int argc, char *argv[])
{
#ifdef CONFIG_SSL_CERT_VERIFICATION
    RSA_CTX *rsa_ctx;
    BI_CTX *ctx;
    bigint *bi_data, *bi_res;
    int diff, res = 1;
    struct timeval tv_old, tv_new;
    const char *plaintext;
    uint8_t compare[MAX_KEY_BYTE_SIZE];
    int i, max_biggie = 10;    /* really crank performance */
    int len; 
    uint8_t *buf;

    /**
     * 512 bit key
     */
    plaintext = /* 64 byte number */
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ*^";

    len = get_file("ssl/test/axTLS.key_512", &buf);
    asn1_get_private_key(buf, len, &rsa_ctx);
    ctx = rsa_ctx->bi_ctx;
    bi_data = bi_import(ctx, (uint8_t *)plaintext, strlen(plaintext));
    bi_res = RSA_public(rsa_ctx, bi_data);
    bi_data = bi_res;   /* reuse again */

    gettimeofday(&tv_old, NULL);
    for (i = 0; i < max_biggie; i++)
    {
        bi_res = RSA_private(rsa_ctx, bi_copy(bi_data));
        if (i < max_biggie-1)
        {
            bi_free(ctx, bi_res);
        }
    }

    gettimeofday(&tv_new, NULL);
    bi_free(ctx, bi_data);

    diff = (tv_new.tv_sec-tv_old.tv_sec)*1000 +
                (tv_new.tv_usec-tv_old.tv_usec)/1000;
    printf("512 bit decrypt time: %dms\n", diff/max_biggie);
    TTY_FLUSH();
    bi_export(ctx, bi_res, compare, 64);
    RSA_free(rsa_ctx);
    free(buf);
    if (memcmp(plaintext, compare, 64) != 0)
        goto end;

    /**
     * 1024 bit key
     */
    plaintext = /* 128 byte number */
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ*^"
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ*^";

    len = get_file("ssl/test/axTLS.key_1024", &buf);
    asn1_get_private_key(buf, len, &rsa_ctx);
    ctx = rsa_ctx->bi_ctx;
    bi_data = bi_import(ctx, (uint8_t *)plaintext, strlen(plaintext));
    bi_res = RSA_public(rsa_ctx, bi_data);
    bi_data = bi_res;   /* reuse again */

    gettimeofday(&tv_old, NULL);
    for (i = 0; i < max_biggie; i++)
    {
        bi_res = RSA_private(rsa_ctx, bi_copy(bi_data));
        if (i < max_biggie-1)
        {
            bi_free(ctx, bi_res);
        }
    }

    gettimeofday(&tv_new, NULL);
    bi_free(ctx, bi_data);

    diff = (tv_new.tv_sec-tv_old.tv_sec)*1000 +
                (tv_new.tv_usec-tv_old.tv_usec)/1000;
    printf("1024 bit decrypt time: %dms\n", diff/max_biggie);
    TTY_FLUSH();
    bi_export(ctx, bi_res, compare, 128);
    RSA_free(rsa_ctx);
    free(buf);
    if (memcmp(plaintext, compare, 128) != 0)
        goto end;

    /**
     * 2048 bit key
     */
    plaintext = /* 256 byte number */
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ*^"
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ*^"
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ*^"
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ*^";

    len = get_file("ssl/test/axTLS.key_2048", &buf);
    asn1_get_private_key(buf, len, &rsa_ctx);
    ctx = rsa_ctx->bi_ctx;
    bi_data = bi_import(ctx, (uint8_t *)plaintext, strlen(plaintext));
    bi_res = RSA_public(rsa_ctx, bi_data);
    bi_data = bi_res;   /* reuse again */

    gettimeofday(&tv_old, NULL);
    for (i = 0; i < max_biggie; i++)
    {
        bi_res = RSA_private(rsa_ctx, bi_copy(bi_data));
        if (i < max_biggie-1)
        {
            bi_free(ctx, bi_res);
        }
    }
    gettimeofday(&tv_new, NULL);
    bi_free(ctx, bi_data);

    diff = (tv_new.tv_sec-tv_old.tv_sec)*1000 +
                (tv_new.tv_usec-tv_old.tv_usec)/1000;
    printf("2048 bit decrypt time: %dms\n", diff/max_biggie);
    TTY_FLUSH();
    bi_export(ctx, bi_res, compare, 256);
    RSA_free(rsa_ctx);
    free(buf);
    if (memcmp(plaintext, compare, 256) != 0)
        goto end;

    /**
     * 4096 bit key
     */
    plaintext = /* 512 byte number */
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ*^"
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ*^"
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ*^"
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ*^"
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ*^"
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ*^"
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ*^"
        "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ*^";

    len = get_file("ssl/test/axTLS.key_4096", &buf);
    asn1_get_private_key(buf, len, &rsa_ctx);
    ctx = rsa_ctx->bi_ctx;
    bi_data = bi_import(ctx, (uint8_t *)plaintext, strlen(plaintext));
    gettimeofday(&tv_old, NULL);
    bi_res = RSA_public(rsa_ctx, bi_data);
    gettimeofday(&tv_new, NULL);
    diff = (tv_new.tv_sec-tv_old.tv_sec)*1000 +
                (tv_new.tv_usec-tv_old.tv_usec)/1000;
    printf("4096 bit encrypt time: %dms\n", diff);
    TTY_FLUSH();
    bi_data = bi_res;   /* reuse again */

    gettimeofday(&tv_old, NULL);
    for (i = 0; i < max_biggie; i++)
    {
        bi_res = RSA_private(rsa_ctx, bi_copy(bi_data));
        if (i < max_biggie-1)
        {
            bi_free(ctx, bi_res);
        }
    }

    gettimeofday(&tv_new, NULL);
    bi_free(ctx, bi_data);

    diff = (tv_new.tv_sec-tv_old.tv_sec)*1000 +
                (tv_new.tv_usec-tv_old.tv_usec)/1000;
    printf("4096 bit decrypt time: %dms\n", diff/max_biggie);
    TTY_FLUSH();
    bi_export(ctx, bi_res, compare, 512);
    RSA_free(rsa_ctx);
    free(buf);
    if (memcmp(plaintext, compare, 512) != 0)
        goto end;

    /* done */
    printf("Bigint performance testing complete\n");
    res = 0;

end:
    return res;
#else
    return 0;
#endif
}
