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

static int do_obj(SSL_CTX *ssl_ctx, int obj_type, 
                    SSLObjLoader *ssl_obj, const char *password);
#ifdef CONFIG_SSL_HAS_PEM
static int ssl_obj_PEM_load(SSL_CTX *ssl_ctx, int obj_type, 
                        SSLObjLoader *ssl_obj, const char *password);
#endif

/*
 * Load a file into memory that is in binary DER (or ascii PEM) format.
 */
EXP_FUNC int STDCALL ssl_obj_load(SSL_CTX *ssl_ctx, int obj_type, 
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
    if (strncmp((char *)ssl_obj->buf, begin, strlen(begin)) == 0)
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
EXP_FUNC int STDCALL ssl_obj_memory_load(SSL_CTX *ssl_ctx, int mem_type, 
        const uint8_t *data, int len, const char *password)
{
    int ret;

    SSLObjLoader *ssl_obj;
    ssl_obj = (SSLObjLoader *)calloc(1, sizeof(SSLObjLoader));
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
static int do_obj(SSL_CTX *ssl_ctx, int obj_type, 
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

/**
 * Take a base64 blob of data and decrypt it (using AES) into its 
 * proper ASN.1 form.
 */
static int pem_decrypt(const char *where, const char *end,
                        const char *password, SSLObjLoader *ssl_obj)
{
    int ret = -1;
    int is_aes_256 = 0;
    char *start = NULL;
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

    if ((start = strstr((const char *)where, aes_str[0])))         /* AES128? */
    {
        start += strlen(aes_str[0]);
    }
    else if ((start = strstr((const char *)where, aes_str[1])))    /* AES256? */
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
        char c = *start++ - '0';
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
    MD5_Init(&md5_ctx);
    MD5_Update(&md5_ctx, (const uint8_t *)password, strlen(password));
    MD5_Update(&md5_ctx, iv, SALT_SIZE);
    MD5_Final(key, &md5_ctx);

    if (is_aes_256)
    {
        MD5_Init(&md5_ctx);
        MD5_Update(&md5_ctx, key, MD5_SIZE);
        MD5_Update(&md5_ctx, (const uint8_t *)password, strlen(password));
        MD5_Update(&md5_ctx, iv, SALT_SIZE);
        MD5_Final(&key[MD5_SIZE], &md5_ctx);
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
static int new_pem_obj(SSL_CTX *ssl_ctx, int is_cacert, char *where, 
                    int remain, const char *password)
{
    int ret = SSL_OK;
    SSLObjLoader *ssl_obj = NULL;
    int i, pem_size, obj_type;
    char *start = NULL, *end = NULL;

    for (i = 0; i < NUM_PEM_TYPES; i++)
    {
        if ((start = strstr(where, begins[i])) &&
                (end = strstr(where, ends[i])))
        {
            remain -= (int)(end-start);
            start += strlen(begins[i]);
            pem_size = (int)(end-start);

            ssl_obj = (SSLObjLoader *)calloc(1, sizeof(SSLObjLoader));

            /* 4/3 bigger than what we need but so what */
            ssl_obj->buf = (uint8_t *)calloc(1, pem_size);

            if (i == IS_RSA_PRIVATE_KEY && 
                        strstr(start, "Proc-Type:") && 
                        strstr(start, "4,ENCRYPTED"))
            {
                /* check for encrypted PEM file */
                if (pem_decrypt(start, end, password, ssl_obj) < 0)
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

                default:
                    goto error;
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
static int ssl_obj_PEM_load(SSL_CTX *ssl_ctx, int obj_type, 
                        SSLObjLoader *ssl_obj, const char *password)
{
    char *start;

    /* add a null terminator */
    ssl_obj->len++;
    ssl_obj->buf = (uint8_t *)realloc(ssl_obj->buf, ssl_obj->len);
    ssl_obj->buf[ssl_obj->len-1] = 0;
    start = (char *)ssl_obj->buf;
    return new_pem_obj(ssl_ctx, obj_type == SSL_OBJ_X509_CACERT,
                                start, ssl_obj->len, password);
}
#endif /* CONFIG_SSL_HAS_PEM */
