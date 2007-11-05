/*
 * Copyright (c) 2007, Cameron Rich
 * 
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, 
 *   this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright notice, 
 *   this list of conditions and the following disclaimer in the documentation 
 *   and/or other materials provided with the distribution.
 * * Neither the name of the axTLS project nor the names of its contributors 
 *   may be used to endorse or promote products derived from this software 
 *   without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * @file crypto.h
 */

#ifndef HEADER_CRYPTO_H
#define HEADER_CRYPTO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "os_port.h"

/* enable features based on a 'super-set' capbaility. */
#if defined(CONFIG_SSL_FULL_MODE) 
#define CONFIG_SSL_ENABLE_CLIENT
#define CONFIG_SSL_CERT_VERIFICATION
#elif defined(CONFIG_SSL_ENABLE_CLIENT)
#define CONFIG_SSL_CERT_VERIFICATION
#endif

/**************************************************************************
 * AES declarations 
 **************************************************************************/

#define AES_MAXROUNDS			14
#define AES_BLOCKSIZE           16
#define AES_IV_SIZE             16

typedef struct aes_key_st 
{
    uint16_t rounds;
    uint16_t key_size;
    uint32_t ks[(AES_MAXROUNDS+1)*8];
    uint8_t iv[AES_IV_SIZE];
} AES_CTX;

typedef enum
{
    AES_MODE_128,
    AES_MODE_256
} AES_MODE;

void AES_set_key(AES_CTX *ctx, const uint8_t *key, 
        const uint8_t *iv, AES_MODE mode);
void AES_cbc_encrypt(AES_CTX *ctx, const uint8_t *msg, 
        uint8_t *out, int length);
void AES_cbc_decrypt(AES_CTX *ks, const uint8_t *in, uint8_t *out, int length);
void AES_convert_key(AES_CTX *ctx);

/**************************************************************************
 * RC4 declarations 
 **************************************************************************/

typedef struct 
{
    uint8_t x, y, m[256];
} RC4_CTX;

void RC4_setup(RC4_CTX *s, const uint8_t *key, int length);
void RC4_crypt(RC4_CTX *s, const uint8_t *msg, uint8_t *data, int length);

/**************************************************************************
 * SHA1 declarations 
 **************************************************************************/

#define SHA1_SIZE   20

/*
 *  This structure will hold context information for the SHA-1
 *  hashing operation
 */
typedef struct 
{
    uint32_t Intermediate_Hash[SHA1_SIZE/4]; /* Message Digest */
    uint32_t Length_Low;            /* Message length in bits */
    uint32_t Length_High;           /* Message length in bits */
    uint16_t Message_Block_Index;   /* Index into message block array   */
    uint8_t Message_Block[64];      /* 512-bit message blocks */
} SHA1_CTX;

void SHA1_Init(SHA1_CTX *);
void SHA1_Update(SHA1_CTX *, const uint8_t * msg, int len);
void SHA1_Final(uint8_t *digest, SHA1_CTX *);

/**************************************************************************
 * MD2 declarations 
 **************************************************************************/

#define MD2_SIZE 16

typedef struct
{
    unsigned char cksum[16];    /* checksum of the data block */
    unsigned char state[48];    /* intermediate digest state */
    unsigned char buffer[16];   /* data block being processed */
    int left;                   /* amount of data in buffer */
} MD2_CTX;

EXP_FUNC void STDCALL MD2_Init(MD2_CTX *ctx);
EXP_FUNC void STDCALL MD2_Update(MD2_CTX *ctx, const uint8_t *input, int ilen);
EXP_FUNC void STDCALL MD2_Final(uint8_t *digest, MD2_CTX *ctx);

/**************************************************************************
 * MD5 declarations 
 **************************************************************************/

#define MD5_SIZE    16

typedef struct 
{
  uint32_t state[4];        /* state (ABCD) */
  uint32_t count[2];        /* number of bits, modulo 2^64 (lsb first) */
  uint8_t buffer[64];       /* input buffer */
} MD5_CTX;

EXP_FUNC void STDCALL MD5_Init(MD5_CTX *);
EXP_FUNC void STDCALL MD5_Update(MD5_CTX *, const uint8_t *msg, int len);
EXP_FUNC void STDCALL MD5_Final(uint8_t *digest, MD5_CTX *);

/**************************************************************************
 * HMAC declarations 
 **************************************************************************/
void hmac_md5(const uint8_t *msg, int length, const uint8_t *key, 
        int key_len, uint8_t *digest);
void hmac_sha1(const uint8_t *msg, int length, const uint8_t *key, 
        int key_len, uint8_t *digest);



#ifdef __cplusplus
}
#endif

#endif 
