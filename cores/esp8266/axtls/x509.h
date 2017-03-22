#ifndef HEADER_X509_H
#define HEADER_X509_H

#define CONFIG_X509_MAX_CA_CERTS 1

#include "rsa.h"
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

typedef struct
{
    X509_CTX *cert[CONFIG_X509_MAX_CA_CERTS];
} CA_CERT_CTX;

int x509_new(const uint8_t *cert, int *len, X509_CTX **ctx);
void x509_free(X509_CTX *x509_ctx);
int x509_verify(const CA_CERT_CTX *ca_cert_ctx, const X509_CTX *cert, int *pathLenConstraint);
#endif
