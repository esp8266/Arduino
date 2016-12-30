/*
 * Copyright (c) 2007-2016, Cameron Rich
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
 * @file crypto_misc.h
 */

#ifndef HEADER_CRYPTO_MISC_H
#define HEADER_CRYPTO_MISC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "crypto.h"
#include "bigint.h"

/**************************************************************************
 * X509 declarations 
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
#define X509_MAX_CERTS                      -10
#define X509_VFY_ERROR_BASIC_CONSTRAINT     -11

/*
 * The Distinguished Name
 */
#define X509_NUM_DN_TYPES                   6
#define X509_COMMON_NAME                    0
#define X509_ORGANIZATION                   1
#define X509_ORGANIZATIONAL_UNIT            2
#define X509_LOCATION                       3
#define X509_COUNTRY                        4
#define X509_STATE                          5

/*
 * Key Usage bits
 */
#define IS_SET_KEY_USAGE_FLAG(A, B)          (A->key_usage & B)

#define KEY_USAGE_DIGITAL_SIGNATURE         0x0080
#define KEY_USAGE_NON_REPUDIATION           0x0040
#define KEY_USAGE_KEY_ENCIPHERMENT          0x0020
#define KEY_USAGE_DATA_ENCIPHERMENT         0x0010
#define KEY_USAGE_KEY_AGREEMENT             0x0008
#define KEY_USAGE_KEY_CERT_SIGN             0x0004
#define KEY_USAGE_CRL_SIGN                  0x0002
#define KEY_USAGE_ENCIPHER_ONLY             0x0001
#define KEY_USAGE_DECIPHER_ONLY             0x8000

struct _x509_ctx
{
    char *ca_cert_dn[X509_NUM_DN_TYPES];
    char *cert_dn[X509_NUM_DN_TYPES];
    char **subject_alt_dnsnames;
    time_t not_before;
    time_t not_after;
    uint8_t *signature;
    RSA_CTX *rsa_ctx;
    bigint *digest;
    uint8_t *fingerprint;
    uint8_t *spki_sha256;
    uint16_t sig_len;
    uint8_t sig_type;
    bool basic_constraint_present;
    bool basic_constraint_is_critical;
    bool key_usage_present;
    bool key_usage_is_critical;
    bool subject_alt_name_present;
    bool subject_alt_name_is_critical;
    bool basic_constraint_cA;
    int basic_constraint_pathLenConstraint;
    uint32_t key_usage;
    struct _x509_ctx *next;
};

typedef struct _x509_ctx X509_CTX;

#ifdef CONFIG_SSL_CERT_VERIFICATION
typedef struct 
{
    X509_CTX *cert[CONFIG_X509_MAX_CA_CERTS];
} CA_CERT_CTX;
#endif

int x509_new(const uint8_t *cert, int *len, X509_CTX **ctx);
void x509_free(X509_CTX *x509_ctx);
#ifdef CONFIG_SSL_CERT_VERIFICATION
int x509_verify(const CA_CERT_CTX *ca_cert_ctx, const X509_CTX *cert, 
        int *pathLenConstraint);
#endif
#ifdef CONFIG_SSL_FULL_MODE
void x509_print(const X509_CTX *cert, CA_CERT_CTX *ca_cert_ctx);
const char * x509_display_error(int error);
#endif

/**************************************************************************
 * ASN1 declarations 
 **************************************************************************/
#define ASN1_BOOLEAN            0x01
#define ASN1_INTEGER            0x02
#define ASN1_BIT_STRING         0x03
#define ASN1_OCTET_STRING       0x04
#define ASN1_NULL               0x05
#define ASN1_PRINTABLE_STR2     0x0C
#define ASN1_OID                0x06
#define ASN1_PRINTABLE_STR2     0x0C
#define ASN1_PRINTABLE_STR      0x13
#define ASN1_TELETEX_STR        0x14
#define ASN1_IA5_STR            0x16
#define ASN1_UTC_TIME           0x17
#define ASN1_GENERALIZED_TIME   0x18
#define ASN1_UNICODE_STR        0x1e
#define ASN1_SEQUENCE           0x30
#define ASN1_CONTEXT_DNSNAME	0x82
#define ASN1_SET                0x31
#define ASN1_V3_DATA			0xa3
#define ASN1_IMPLICIT_TAG       0x80
#define ASN1_CONTEXT_DNSNAME	0x82
#define ASN1_EXPLICIT_TAG       0xa0
#define ASN1_V3_DATA			0xa3

#define SIG_TYPE_MD5            0x04
#define SIG_TYPE_SHA1           0x05
#define SIG_TYPE_SHA256         0x0b
#define SIG_TYPE_SHA384         0x0c
#define SIG_TYPE_SHA512         0x0d

uint32_t get_asn1_length(const uint8_t *buf, int *offset);
int asn1_get_private_key(const uint8_t *buf, int len, RSA_CTX **rsa_ctx);
int asn1_next_obj(const uint8_t *buf, int *offset, int obj_type);
int asn1_skip_obj(const uint8_t *buf, int *offset, int obj_type);
int asn1_get_big_int(const uint8_t *buf, int *offset, uint8_t **object);
int asn1_get_int(const uint8_t *buf, int *offset, int32_t *val);
int asn1_get_bool(const uint8_t *buf, int *offset, bool *val);
int asn1_get_bit_string_as_int(const uint8_t *buf, int *offset, uint32_t *val);
int asn1_version(const uint8_t *cert, int *offset, int *val);
int asn1_validity(const uint8_t *cert, int *offset, X509_CTX *x509_ctx);
int asn1_name(const uint8_t *cert, int *offset, char *dn[]);
int asn1_public_key(const uint8_t *cert, int *offset, X509_CTX *x509_ctx);
#ifdef CONFIG_SSL_CERT_VERIFICATION
int asn1_signature(const uint8_t *cert, int *offset, X509_CTX *x509_ctx);
int asn1_compare_dn(char * const dn1[], char * const dn2[]);
int asn1_is_subject_alt_name(const uint8_t *cert, int offset);
int asn1_is_basic_constraints(const uint8_t *cert, int offset);
int asn1_is_key_usage(const uint8_t *cert, int offset);
bool asn1_is_critical_ext(const uint8_t *buf, int *offset);
#endif /* CONFIG_SSL_CERT_VERIFICATION */
int asn1_signature_type(const uint8_t *cert, 
                                int *offset, X509_CTX *x509_ctx);

/**************************************************************************
 * MISC declarations 
 **************************************************************************/
#define SALT_SIZE               8

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
