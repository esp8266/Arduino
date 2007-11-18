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

#include "config.h"

#ifdef CONFIG_GEN_CERTIFICATES
#include <string.h>
#include <stdlib.h>
#include "crypto_misc.h"

/**
 * This file is not completed.
 */

/* OBJECT IDENTIFIER sha1withRSAEncryption (1 2 840 113549 1 1 5) */
static const uint8_t sig_oid[] = 
{
    0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x05
};

/*  OBJECT IDENTIFIER rsaEncryption (1 2 840 113549 1 1 1) */
static const uint8_t rsa_enc_oid[] =
{
    0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01
};

/* INTEGER 65537 */
static const uint8_t pub_key_seq[] = 
{
    0x02, 0x03, 0x01, 0x00, 0x01
};

/* 0x00 + SEQUENCE {
     SEQUENCE {
       OBJECT IDENTIFIER sha1 (1 3 14 3 2 26)
       NULL
       }
     OCTET STRING */
static const uint8_t asn1_sig[] = 
{
    0x30,  0x21, 0x30, 0x09, 0x06, 0x05, 0x2b, 0x0e, 0x03, 0x02,
    0x1a, 0x05, 0x00, 0x04, 0x14 
};

static uint8_t set_gen_length(int len, uint8_t *buf, int *offset)
{
    if (len < 0x80) /* short form */
    {
        buf[(*offset)++] = len;
        return 1;
    }
    else /* long form */
    {
        int i, length_bytes = 0;

        if (len & 0x00FF0000)
            length_bytes = 3;
        else if (len & 0x0000FF00)
            length_bytes = 2;
        else if (len & 0x000000FF)
            length_bytes = 1;
            
        buf[(*offset)++] = 0x80 + length_bytes;

        for (i = length_bytes-1; i >= 0; i--)
        {
            buf[*offset+i] = len & 0xFF;
            len >>= 8;
        }

        *offset += length_bytes;
        return length_bytes+1;
    }
}

static int pre_adjust_with_size(uint8_t type,
        int *seq_offset, uint8_t *buf, int *offset)
{
    buf[(*offset)++] = type;
    *seq_offset = *offset;
    *offset += 4;   /* fill in later */
    return *offset;
}

static void adjust_with_size(int seq_size, int seq_start, 
                uint8_t *buf, int *offset)
{
    uint8_t seq_byte_size; 
    int orig_seq_size = seq_size;
    int orig_seq_start = seq_start;

    seq_size = *offset-seq_size;
    seq_byte_size = set_gen_length(seq_size, buf, &seq_start);

    if (seq_byte_size != 4)
    {
        memmove(&buf[orig_seq_start+seq_byte_size], 
                &buf[orig_seq_size], seq_size);
        *offset -= 4-seq_byte_size;
    }
}

static void gen_serial_number(uint8_t *buf, int *offset)
{
    buf[(*offset)++] = ASN1_INTEGER;
    buf[(*offset)++] = 1;
    buf[(*offset)++] = 0x7F;
}

static void gen_signature_alg(uint8_t *buf, int *offset)
{
    buf[(*offset)++] = ASN1_SEQUENCE;
    set_gen_length(13, buf, offset);
    buf[(*offset)++] = ASN1_OID;
    set_gen_length(sizeof(sig_oid), buf, offset);
    memcpy(&buf[*offset], sig_oid, sizeof(sig_oid));
    *offset += sizeof(sig_oid);
    buf[(*offset)++] = ASN1_NULL;
    buf[(*offset)++] = 0;
}

static int gen_dn(const char *name, uint8_t dn_type, 
                        uint8_t *buf, int *offset)
{
    int ret = X509_OK;
    int name_size = strlen(name);

    if (name_size > 0x70)    /* just too big */
    {
        ret = X509_NOT_OK;
        goto error;
    }

    buf[(*offset)++] = ASN1_SET;
    set_gen_length(9+name_size, buf, offset);
    buf[(*offset)++] = ASN1_SEQUENCE;
    set_gen_length(7+name_size, buf, offset);
    buf[(*offset)++] = ASN1_OID;
    buf[(*offset)++] = 3;
    buf[(*offset)++] = 0x55;
    buf[(*offset)++] = 0x04;
    buf[(*offset)++] = dn_type;
    buf[(*offset)++] = ASN1_PRINTABLE_STR;
    buf[(*offset)++] = name_size;
    strcpy(&buf[*offset], name);
    *offset += name_size;

error:
    return ret;
}

static int gen_issuer(const char *cn, const char *o, const char *ou,
                    uint8_t *buf, int *offset)
{
    int ret = X509_OK;
    int seq_offset;
    int seq_size = pre_adjust_with_size(
                            ASN1_SEQUENCE, &seq_offset, buf, offset);

    /* we need the common name at a minimum */
    if (cn == NULL)
    {
        ret = X509_NOT_OK;
        goto error;
    }

    if ((ret = gen_dn(cn, 3, buf, offset)))
            goto error;

    if (o != NULL)
    {
        if ((ret = gen_dn(o, 10, buf, offset)))
            goto error;
    }

    if (ou != NULL)
    {
        if ((ret = gen_dn(o, 11, buf, offset)))
            goto error;
    }

    adjust_with_size(seq_size, seq_offset, buf, offset);

error:
    return ret;
}

static void gen_utc_time(uint8_t *buf, int *offset)
{
    time_t curr_time = time(NULL);
    struct tm *now_tm = gmtime(&curr_time);

    buf[(*offset)++] = ASN1_SEQUENCE;
    set_gen_length(30, buf, offset);

    now_tm->tm_year -= 100;
    now_tm->tm_mon++;
    buf[(*offset)++] = ASN1_UTC_TIME;
    buf[(*offset)++] = 13;
    buf[(*offset)++] = now_tm->tm_year/10 + '0';
    buf[(*offset)++] = now_tm->tm_year%10 + '0';
    buf[(*offset)++] = now_tm->tm_mon/10 + '0';
    buf[(*offset)++] = now_tm->tm_mon%10 + '0';
    buf[(*offset)++] = now_tm->tm_mday/10 + '0';
    buf[(*offset)++] = now_tm->tm_mday%10 + '0';
    memset(&buf[*offset], '0', 6);
    *offset += 6;
    buf[(*offset)++] = 'Z';
    now_tm->tm_year += 30; /* add 30 years */
    memcpy(&buf[*offset], &buf[*offset-15], 15);
    buf[*offset + 2] = now_tm->tm_year/10 + '0';
    buf[*offset + 3] = now_tm->tm_year%10 + '0';
    *offset += 15;
}

static void gen_pub_key2(const RSA_CTX *rsa_ctx, uint8_t *buf, int *offset)
{
    int seq_offset;
    int pub_key_size = rsa_ctx->num_octets;
    uint8_t *block = (uint8_t *)alloca(pub_key_size);
    int seq_size = pre_adjust_with_size(
                            ASN1_SEQUENCE, &seq_offset, buf, offset);
    buf[(*offset)++] = ASN1_INTEGER;
    bi_export(rsa_ctx->bi_ctx, rsa_ctx->m, block, pub_key_size);
    if (*block & 0x80)  /* make integer positive */
    {
        set_gen_length(pub_key_size+1, buf, offset);
        buf[(*offset)++] = 0;
    }
    else
        set_gen_length(pub_key_size, buf, offset);

    memcpy(&buf[*offset], block, pub_key_size);
    *offset += pub_key_size;
    adjust_with_size(seq_size, seq_offset, buf, offset);
}

static void gen_pub_key1(const RSA_CTX *rsa_ctx, uint8_t *buf, int *offset)
{
    int seq_offset;
    int seq_size = pre_adjust_with_size(
                            ASN1_BIT_STRING, &seq_offset, buf, offset);
    buf[(*offset)++] = 0;   /* bit string is multiple of 8 */
    gen_pub_key2(rsa_ctx, buf, offset);
    adjust_with_size(seq_size, seq_offset, buf, offset);
}

static void gen_pub_key(const RSA_CTX *rsa_ctx, uint8_t *buf, int *offset)
{
    int seq_offset;
    int seq_size = pre_adjust_with_size(
                            ASN1_SEQUENCE, &seq_offset, buf, offset);

    buf[(*offset)++] = ASN1_SEQUENCE;
    set_gen_length(13, buf, offset);
    buf[(*offset)++] = ASN1_OID;
    set_gen_length(sizeof(rsa_enc_oid), buf, offset);
    memcpy(&buf[*offset], rsa_enc_oid, sizeof(rsa_enc_oid));
    *offset += sizeof(rsa_enc_oid);
    buf[(*offset)++] = ASN1_NULL;
    buf[(*offset)++] = 0;
    gen_pub_key1(rsa_ctx, buf, offset);
    memcpy(&buf[*offset], pub_key_seq, sizeof(pub_key_seq));
    *offset += sizeof(pub_key_seq);
    adjust_with_size(seq_size, seq_offset, buf, offset);
}

static void gen_signature(const RSA_CTX *rsa_ctx, const uint8_t *sha_dgst, 
                        uint8_t *buf, int *offset)
{
    uint8_t *enc_block = (uint8_t *)alloca(rsa_ctx->num_octets);
    uint8_t *block = (uint8_t *)alloca(sizeof(asn1_sig) + SHA1_SIZE);
    int sig_size;

    /* add the digest as an embedded asn.1 sequence */
    memcpy(block, asn1_sig, sizeof(asn1_sig));
    memcpy(&block[sizeof(asn1_sig)], sha_dgst, SHA1_SIZE);

    sig_size = RSA_encrypt(rsa_ctx, block, 
                            sizeof(asn1_sig) + SHA1_SIZE, enc_block, 1);

    buf[(*offset)++] = ASN1_BIT_STRING;
    set_gen_length(sig_size+1, buf, offset);
    buf[(*offset)++] = 0;   /* bit string is multiple of 8 */
    memcpy(&buf[*offset], enc_block, sig_size);
    *offset += sig_size;
}

static int gen_tbs_cert(const char *cn, const char *o, const char *ou,
                    const RSA_CTX *rsa_ctx, uint8_t *buf, int *offset,
                    uint8_t *sha_dgst)
{
    int ret = X509_OK;
    SHA1_CTX sha_ctx;
    int seq_offset;
    int seq_size = pre_adjust_with_size(
                        ASN1_SEQUENCE, &seq_offset, buf, offset);
    int begin_tbs = *offset;

    gen_serial_number(buf, offset);
    gen_signature_alg(buf, offset);
    if ((ret = gen_issuer(cn, o, ou, buf, offset)))
        goto error;

    gen_utc_time(buf, offset);
    if ((ret = gen_issuer(cn, o, ou, buf, offset)))
        goto error;

    gen_pub_key(rsa_ctx, buf, offset);

    SHA1_Init(&sha_ctx);
    SHA1_Update(&sha_ctx, &buf[begin_tbs], *offset-begin_tbs);
    SHA1_Final(sha_dgst, &sha_ctx);
    adjust_with_size(seq_size, seq_offset, buf, offset);

error:
    return ret;
}

int gen_cert(const char *cn, const char *o, const char *ou,
                    const RSA_CTX *rsa_ctx, uint8_t *buf, int *cert_size)
{
    int ret = X509_OK;
    int offset = 0;
    int seq_offset;
    uint8_t sha_dgst[SHA1_SIZE];
    int seq_size = pre_adjust_with_size(
                            ASN1_SEQUENCE, &seq_offset, buf, &offset);

    if ((ret = gen_tbs_cert(cn, o, ou, rsa_ctx, buf, &offset, sha_dgst)))
        goto error;

    gen_signature_alg(buf, &offset);
    gen_signature(rsa_ctx, sha_dgst, buf, &offset);

    adjust_with_size(seq_size, seq_offset, buf, &offset);
    *cert_size = offset;
error:
    return ret;
}

int main(int argc, char *argv[])
{
    int ret = X509_OK;
    uint8_t *key_buf = NULL;
    RSA_CTX *rsa_ctx = NULL;
    uint8_t buf[2048];
    int cert_size;
    FILE *f;

    int len = get_file("../ssl/test/axTLS.key_512", &key_buf);
    if ((ret = asn1_get_private_key(key_buf, len, &rsa_ctx)))
        goto error;

    if ((ret = gen_cert("abc", "def", "ghi", rsa_ctx, buf, &cert_size)))
        goto error;

    f = fopen("blah.dat", "w");
    fwrite(buf, cert_size, 1, f);
    fclose(f);
error:
    free(key_buf);
    RSA_free(rsa_ctx);

    if (ret)
        printf("Some cert generation issue\n");

    return ret;
}

#endif

