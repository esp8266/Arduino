/*
 *  Copyright(C) 2006 Cameron Rich
 *
 *  This library is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
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
 * @file crypto.h
 */

#ifndef HEADER_CRYPTO_H
#define HEADER_CRYPTO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "bigint.h"

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
    int x, y, m[256];
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
    uint32_t Intermediate_Hash[SHA1_SIZE/4]; /* Message Digest  */
    uint32_t Length_Low;            /* Message length in bits      */
    uint32_t Length_High;           /* Message length in bits      */
    uint16_t Message_Block_Index;   /* Index into message block array   */
    uint8_t Message_Block[64];      /* 512-bit message blocks      */
} SHA1_CTX;

void SHA1Init(SHA1_CTX *);
void SHA1Update(SHA1_CTX *, const uint8_t * msg, int len);
void SHA1Final(SHA1_CTX *, uint8_t *digest);

/**************************************************************************
 * MD5 declarations 
 **************************************************************************/

/* MD5 context. */

#define MD5_SIZE    16

typedef struct 
{
  uint32_t state[4];        /* state (ABCD) */
  uint32_t count[2];        /* number of bits, modulo 2^64 (lsb first) */
  uint8_t buffer[64];       /* input buffer */
} MD5_CTX;

EXP_FUNC void STDCALL MD5Init(MD5_CTX *);
EXP_FUNC void STDCALL MD5Update(MD5_CTX *, const uint8_t *msg, int len);
EXP_FUNC void STDCALL MD5Final(MD5_CTX *, uint8_t *digest);

/**************************************************************************
 * HMAC declarations 
 **************************************************************************/
void hmac_md5(const uint8_t *msg, int length, const uint8_t *key, 
        int key_len, uint8_t *digest);
void hmac_sha1(const uint8_t *msg, int length, const uint8_t *key, 
        int key_len, uint8_t *digest);

/**************************************************************************
 * RNG declarations 
 **************************************************************************/
EXP_FUNC void STDCALL RNG_initialize(const uint8_t *seed_buf, int size);
EXP_FUNC void STDCALL RNG_terminate(void);
EXP_FUNC void STDCALL get_random(int num_rand_bytes, uint8_t *rand_data);
void get_random_NZ(int num_rand_bytes, uint8_t *rand_data);

/**************************************************************************
 * RSA declarations 
 **************************************************************************/

typedef struct 
{
    bigint *m;              /* modulus */
    bigint *e;              /* public exponent */
    bigint *d;              /* private exponent */
#ifdef CONFIG_BIGINT_CRT
    bigint *p;              /* p as in m = pq */
    bigint *q;              /* q as in m = pq */
    bigint *dP;             /* d mod (p-1) */
    bigint *dQ;             /* d mod (q-1) */
    bigint *qInv;           /* q^-1 mod p */
#endif
    int num_octets;
    BI_CTX *bi_ctx;
} RSA_CTX;

void RSA_priv_key_new(RSA_CTX **rsa_ctx, 
        const uint8_t *modulus, int mod_len,
        const uint8_t *pub_exp, int pub_len,
        const uint8_t *priv_exp, int priv_len
#ifdef CONFIG_BIGINT_CRT
      , const uint8_t *p, int p_len,
        const uint8_t *q, int q_len,
        const uint8_t *dP, int dP_len,
        const uint8_t *dQ, int dQ_len,
        const uint8_t *qInv, int qInv_len
#endif
        );
void RSA_pub_key_new(RSA_CTX **rsa_ctx, 
        const uint8_t *modulus, int mod_len,
        const uint8_t *pub_exp, int pub_len);
void RSA_free(RSA_CTX *ctx);
int RSA_decrypt(const RSA_CTX *ctx, const uint8_t *in_data, uint8_t *out_data,
        int is_decryption);
bigint *RSA_private(const RSA_CTX *c, bigint *bi_msg);
#ifdef CONFIG_SSL_CERT_VERIFICATION
bigint *RSA_sign_verify(BI_CTX *ctx, const uint8_t *sig, int sig_len,
        bigint *modulus, bigint *pub_exp);
bigint *RSA_public(const RSA_CTX * c, bigint *bi_msg);
int RSA_encrypt(const RSA_CTX *ctx, const uint8_t *in_data, uint16_t in_len, 
        uint8_t *out_data, int is_signing);
void RSA_print(const RSA_CTX *ctx);
#endif

/**************************************************************************
 * ASN1 declarations 
 **************************************************************************/
#define X509_OK                             0
#define X509_NOT_OK                         -1
#define X509_VFY_ERROR_NO_TRUSTED_CERT      -2
#define X509_VFY_ERROR_BAD_SIGNATURE        -3      
#define X509_VFY_ERROR_NOT_YET_VALID        -4
#define X509_VFY_ERROR_EXPIRED              -5
#define X509_VFY_ERROR_SELF_SIGNED          -6
#define X509_VFY_ERROR_INVALID_CHAIN        -7
#define X509_VFY_ERROR_UNSUPPORTED_DIGEST   -8
#define X509_INVALID_PRIV_KEY               -9

/*
 * The Distinguished Name
 */
#define X509_NUM_DN_TYPES                   3
#define X509_COMMON_NAME                    0
#define X509_ORGANIZATION                   1
#define X509_ORGANIZATIONAL_TYPE            2

#define ASN1_INTEGER            0x02
#define ASN1_BIT_STRING         0x03
#define ASN1_OCTET_STRING       0x04
#define ASN1_NULL               0x05
#define ASN1_OID                0x06
#define ASN1_PRINTABLE_STR      0x13
#define ASN1_TELETEX_STR        0x14
#define ASN1_IA5_STR            0x16
#define ASN1_UTC_TIME           0x17
#define ASN1_SEQUENCE           0x30
#define ASN1_SET                0x31
#define ASN1_IMPLICIT_TAG       0x80
#define ASN1_EXPLICIT_TAG       0xa0

#define SALT_SIZE               8

struct _x509_ctx
{
    char *ca_cert_dn[X509_NUM_DN_TYPES];
    char *cert_dn[X509_NUM_DN_TYPES];
#if defined(_WIN32_WCE)
    long not_before;
    long not_after;
#else
    time_t not_before;
    time_t not_after;
#endif
    uint8_t *signature;
    uint16_t sig_len;
    uint8_t sig_type;
    RSA_CTX *rsa_ctx;
    bigint *digest;
    struct _x509_ctx *next;
};

typedef struct _x509_ctx X509_CTX;

#ifdef CONFIG_SSL_CERT_VERIFICATION
typedef struct 
{
    X509_CTX *cert[CONFIG_X509_MAX_CA_CERTS];
} CA_CERT_CTX;
#endif

int asn1_get_private_key(const uint8_t *buf, int len, RSA_CTX **rsa_ctx);
int asn1_next_obj(const uint8_t *buf, int *offset, int obj_type);
int asn1_skip_obj(const uint8_t *buf, int *offset, int obj_type);
int asn1_get_int(const uint8_t *buf, int *offset, uint8_t **object);
int x509_new(const uint8_t *cert, int *len, X509_CTX **ctx);
void x509_free(X509_CTX *x509_ctx);
#ifdef CONFIG_SSL_CERT_VERIFICATION
int x509_verify(const CA_CERT_CTX *ca_cert_ctx, const X509_CTX *cert);
const uint8_t *x509_get_signature(const uint8_t *asn1_signature, int *len);
#endif
#ifdef CONFIG_SSL_FULL_MODE
void x509_print(CA_CERT_CTX *ca_cert_ctx, const X509_CTX *cert);
void x509_display_error(int error);
#endif

/**************************************************************************
 * MISC declarations 
 **************************************************************************/

extern const char * const unsupported_str;

typedef void (*crypt_func)(void *, const uint8_t *, uint8_t *, int);
typedef void (*hmac_func)(const uint8_t *msg, int length, const uint8_t *key, 
        int key_len, uint8_t *digest);

int get_file(const char *filename, uint8_t **buf);

#if defined(CONFIG_SSL_FULL_MODE) || defined(WIN32) || defined(CONFIG_DEBUG)
EXP_FUNC void STDCALL print_blob(const char *format, const uint8_t *data, int size, ...);
#else
    #define print_blob(...)
#endif

EXP_FUNC int STDCALL base64_decode(const char *in,  int len,
                    uint8_t *out, int *outlen);

#ifdef __cplusplus
}
#endif

#endif 
