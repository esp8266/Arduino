#ifndef HEADER_ASN1_H
#define HEADER_ASN1_H
#include "x509.h"
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
int asn1_signature(const uint8_t *cert, int *offset, X509_CTX *x509_ctx);
int asn1_compare_dn(char * const dn1[], char * const dn2[]);
int asn1_is_subject_alt_name(const uint8_t *cert, int offset);
int asn1_is_basic_constraints(const uint8_t *cert, int offset);
int asn1_is_key_usage(const uint8_t *cert, int offset);
bool asn1_is_critical_ext(const uint8_t *buf, int *offset);
int asn1_signature_type(const uint8_t *cert, int *offset, X509_CTX *x509_ctx);
#endif
