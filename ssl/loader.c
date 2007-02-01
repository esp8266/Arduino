/*
 *  Copyright(C) 2006 Cameron Rich
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
 * Load certificates/keys into memory. These can be in many different formats.
 * PEM support and other formats can be processed here.
 *
 * The PEM private keys may be optionally encrypted with AES128 or AES256. 
 * The encrypted PEM keys were generated with something like:
 *
 * openssl genrsa -aes128 -passout pass:abcd -out axTLS.key_aes128.pem 512
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "ssl.h"

static int do_obj(SSLCTX *ssl_ctx, int obj_type, 
                    SSLObjLoader *ssl_obj, const char *password);
#ifdef CONFIG_SSL_HAS_PEM
static int ssl_obj_PEM_load(SSLCTX *ssl_ctx, int obj_type, 
                        SSLObjLoader *ssl_obj, const char *password);
#endif

/*
 * Load a file into memory that is in binary DER (or ascii PEM) format.
 */
EXP_FUNC int STDCALL ssl_obj_load(SSLCTX *ssl_ctx, int obj_type, 
                            const char *filename, const char *password)
{
#ifndef CONFIG_SSL_SKELETON_MODE
    static const char * const begin = "-----BEGIN";
    int ret = SSL_OK;
    SSLObjLoader *ssl_obj = NULL;

    if (filename == NULL)
    {
        ret = SSL_ERROR_INVALID_KEY;
        goto error;
    }

    ssl_obj = (SSLObjLoader *)calloc(1, sizeof(SSLObjLoader));
    ssl_obj->len = get_file(filename, &ssl_obj->buf);

    if (ssl_obj->len <= 0)
    {
        ret = SSL_ERROR_INVALID_KEY;
        goto error;
    }

    /* is the file a PEM file? */
    if (strncmp(ssl_obj->buf, begin, strlen(begin)) == 0)
    {
#ifdef CONFIG_SSL_HAS_PEM
        ret = ssl_obj_PEM_load(ssl_ctx, obj_type, ssl_obj, password);
#else
        printf(unsupported_str);
        ret = SSL_ERROR_NOT_SUPPORTED;
#endif
    }
    else
        ret = do_obj(ssl_ctx, obj_type, ssl_obj, password);

error:
    ssl_obj_free(ssl_obj);
    return ret;
#else
    printf(unsupported_str);
    return SSL_ERROR_NOT_SUPPORTED;
#endif /* CONFIG_SSL_SKELETON_MODE */
}

/*
 * Transfer binary data into the object loader.
 */
EXP_FUNC int STDCALL ssl_obj_memory_load(SSLCTX *ssl_ctx, int mem_type, 
        const uint8_t *data, int len, const char *password)
{
    int ret;

    SSLObjLoader *ssl_obj = (SSLObjLoader *)calloc(1, sizeof(SSLObjLoader));
    ssl_obj->buf = (uint8_t *)malloc(len);
    memcpy(ssl_obj->buf, data, len);
    ssl_obj->len = len;
    ret = do_obj(ssl_ctx, mem_type, ssl_obj, password);
    ssl_obj_free(ssl_obj);
    return ret;
}

/*
 * Actually work out what we are doing 
 */
static int do_obj(SSLCTX *ssl_ctx, int obj_type, 
                    SSLObjLoader *ssl_obj, const char *password)
{
    int ret = SSL_OK;

    switch (obj_type)
    {
        case SSL_OBJ_RSA_KEY:
            ret = add_private_key(ssl_ctx, ssl_obj);
            break;

        case SSL_OBJ_X509_CERT:
            ret = add_cert(ssl_ctx, ssl_obj->buf, ssl_obj->len);
            break;

#ifdef CONFIG_SSL_CERT_VERIFICATION
        case SSL_OBJ_X509_CACERT:
            ret = add_cert_auth(ssl_ctx, ssl_obj->buf, ssl_obj->len);
            break;
#endif

#ifdef CONFIG_SSL_USE_PKCS12
        case SSL_OBJ_PKCS8:
            ret = pkcs8_decode(ssl_ctx, ssl_obj, password);
            break;

        case SSL_OBJ_PKCS12:
            ret = pkcs12_decode(ssl_ctx, ssl_obj, password);
            break;
#endif
        default:
            printf(unsupported_str);
            ret = SSL_ERROR_NOT_SUPPORTED;
            break;
    }

    return ret;
}

/*
 * Clean up our mess.
 */
void ssl_obj_free(SSLObjLoader *ssl_obj)
{
    if (ssl_obj)
    {
        free(ssl_obj->buf);
        free(ssl_obj);
    }
}

/*
 * Support for PEM encoded keys/certificates.
 */
#ifdef CONFIG_SSL_HAS_PEM

#define NUM_PEM_TYPES               3
#define IV_SIZE                     16
#define IS_RSA_PRIVATE_KEY          0
#define IS_ENCRYPTED_PRIVATE_KEY    1
#define IS_CERTIFICATE              2

/* base64 to binary lookup table */
static const uint8_t map[128] = 
{
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255,  62, 255, 255, 255,  63,
    52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,
    255, 254, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,
    7,   8,   9,  10,  11,  12,  13,  14,  15,  16,  17,  18,
    19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255,
    255,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,
    37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
    49,  50,  51, 255, 255, 255, 255, 255
};

static const char * const begins[NUM_PEM_TYPES] =
{
    "-----BEGIN RSA PRIVATE KEY-----",
    "-----BEGIN ENCRYPTED PRIVATE KEY-----",
    "-----BEGIN CERTIFICATE-----",
};

static const char * const ends[NUM_PEM_TYPES] =
{
    "-----END RSA PRIVATE KEY-----",
    "-----END ENCRYPTED PRIVATE KEY-----",
    "-----END CERTIFICATE-----",
};

static const char * const aes_str[2] =
{
    "DEK-Info: AES-128-CBC,",
    "DEK-Info: AES-256-CBC," 
};

static int base64_decode(const uint8_t *in,  int len,
                    uint8_t *out, int *outlen)
{
    int g, t, x, y, z;
    uint8_t c;
    int ret = -1;

    g = 3;
    for (x = y = z = t = 0; x < len; x++) 
    {
        if ((c = map[in[x] & 0x7F]) == 0xff)
            continue;

        if (c == 254)   /* this is the end... */
        {
            c = 0;

            if (--g < 0) 
                goto error;
        } 
        else if (g != 3) /* only allow = at end */
            goto error;

        t = (t<<6) | c;

        if (++y == 4) 
        {
            out[z++] = (uint8_t)((t>>16)&255);

            if (g > 1) 
                out[z++] = (uint8_t)((t>>8)&255);

            if (g > 2) 
                out[z++] = (uint8_t)(t&255);

            y = t = 0;
        }
    }

    if (y != 0) 
        goto error;

    *outlen = z;
    ret = 0;

error:
#ifdef CONFIG_SSL_FULL_MODE
    if (ret < 0)
        printf("Error: Invalid base64 file\n");
#endif
    return ret;
}

/**
 * Take a base64 blob of data and decrypt it (using AES) into its 
 * proper ASN.1 form.
 */
static int pem_decrypt(const uint8_t *where, const uint8_t *end,
                        const char *password, SSLObjLoader *ssl_obj)
{
    int ret = -1;
    int is_aes_256 = 0;
    uint8_t *start = NULL;
    uint8_t iv[IV_SIZE];
    int i, pem_size;
    MD5_CTX md5_ctx;
    AES_CTX aes_ctx;
    uint8_t key[32];        /* AES256 size */

    if (password == NULL)
    {
#ifdef CONFIG_SSL_FULL_MODE
        printf("Error: need a password for this PEM file\n");
#endif
        goto error;
    }

    if ((start = (uint8_t *)strstr(
                    (const char *)where, aes_str[0])))          /* AES128? */
    {
        start += strlen(aes_str[0]);
    }
    else if ((start = (uint8_t *)strstr(
                    (const char *)where, aes_str[1])))          /* AES256? */
    {
        is_aes_256 = 1;
        start += strlen(aes_str[1]);
    }
    else 
    {
#ifdef CONFIG_SSL_FULL_MODE
        printf("Error: Unsupported password cipher\n");
#endif
        goto error;
    }

    /* convert from hex to binary - assumes uppercase hex */
    for (i = 0; i < IV_SIZE; i++)
    {
        uint8_t c = *start++ - '0';
        iv[i] = (c > 9 ? c + '0' - 'A' + 10 : c) << 4;
        c = *start++ - '0';
        iv[i] += (c > 9 ? c + '0' - 'A' + 10 : c);
    }

    while (*start == '\r' || *start == '\n')
        start++;

    /* turn base64 into binary */
    pem_size = (int)(end-start);
    if (base64_decode(start, pem_size, ssl_obj->buf, &ssl_obj->len) != 0)
        goto error;

    /* work out the key */
    MD5Init(&md5_ctx);
    MD5Update(&md5_ctx, (const uint8_t *)password, strlen(password));
    MD5Update(&md5_ctx, iv, SALT_SIZE);
    MD5Final(&md5_ctx, key);

    if (is_aes_256)
    {
        MD5Init(&md5_ctx);
        MD5Update(&md5_ctx, key, MD5_SIZE);
        MD5Update(&md5_ctx, (const uint8_t *)password, strlen(password));
        MD5Update(&md5_ctx, iv, SALT_SIZE);
        MD5Final(&md5_ctx, &key[MD5_SIZE]);
    }

    /* decrypt using the key/iv */
    AES_set_key(&aes_ctx, key, iv, is_aes_256 ? AES_MODE_256 : AES_MODE_128);
    AES_convert_key(&aes_ctx);
    AES_cbc_decrypt(&aes_ctx, ssl_obj->buf, ssl_obj->buf, ssl_obj->len);
    ret = 0;

error:
    return ret; 
}

/**
 * Take a base64 blob of data and turn it into its proper ASN.1 form.
 */
static int new_pem_obj(SSLCTX *ssl_ctx, int is_cacert, uint8_t *where, 
        int remain, const char *password)
{
    int ret = SSL_OK;
    SSLObjLoader *ssl_obj = NULL;
    int i, pem_size, obj_type;
    uint8_t *start = NULL, *end = NULL;

    for (i = 0; i < NUM_PEM_TYPES; i++)
    {
        if ((start = (uint8_t *)strstr((const char *)where, begins[i])) &&
                (end = (uint8_t *)strstr((const char *)where, ends[i])))
        {
            remain -= (int)(end-start);
            start += strlen(begins[i]);
            pem_size = (int)(end-start);
            ssl_obj = (SSLObjLoader *)calloc(1, sizeof(SSLObjLoader));

            /* 4/3 bigger than what we need but so what */
            ssl_obj->buf = (uint8_t *)calloc(1, pem_size);

            if (i == IS_RSA_PRIVATE_KEY && 
                        strstr((const char *)start, "Proc-Type:") && 
                        strstr((const char *)start, "4,ENCRYPTED"))
            {
                /* check for encrypted PEM file */
                if ((pem_size = pem_decrypt(start, end, password, ssl_obj)) < 0)
                    goto error;
            }
            else if (base64_decode(start, pem_size, 
                        ssl_obj->buf, &ssl_obj->len) != 0)
                goto error;

            switch (i)
            {
                case IS_RSA_PRIVATE_KEY:
                    obj_type = SSL_OBJ_RSA_KEY;
                    break;

                case IS_ENCRYPTED_PRIVATE_KEY:
                    obj_type = SSL_OBJ_PKCS8;
                    break;

                case IS_CERTIFICATE:
                    obj_type = is_cacert ?  
                                    SSL_OBJ_X509_CACERT : SSL_OBJ_X509_CERT;
                    break;
            }

            /* In a format we can now understand - so process it */
            if ((ret = do_obj(ssl_ctx, obj_type, ssl_obj, password)))
                goto error;

            end += strlen(ends[i]);
            remain -= strlen(ends[i]);
            while (remain > 0 && (*end == '\r' || *end == '\n'))
            {
                end++;
                remain--;
            }

            break;
        }
    }

    if (i == NUM_PEM_TYPES)
        goto error;

    /* more PEM stuff to process? */
    if (remain)
        ret = new_pem_obj(ssl_ctx, is_cacert, end, remain, password);

error:
    ssl_obj_free(ssl_obj);
    return ret;
}

/*
 * Load a file into memory that is in ASCII PEM format.
 */
static int ssl_obj_PEM_load(SSLCTX *ssl_ctx, int obj_type, 
                        SSLObjLoader *ssl_obj, const char *password)
{
    uint8_t *start;

    /* add a null terminator */
    ssl_obj->len++;
    ssl_obj->buf = (uint8_t *)realloc(ssl_obj->buf, ssl_obj->len);
    ssl_obj->buf[ssl_obj->len-1] = 0;
    start = ssl_obj->buf;
    return new_pem_obj(ssl_ctx, obj_type == SSL_OBJ_X509_CACERT,
                                start, ssl_obj->len, password);
}
#endif /* CONFIG_SSL_HAS_PEM */
