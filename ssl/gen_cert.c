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

static const uint8_t pub_key_seq[] = 
{
    0x02, 0x03, 0x01, 0x00, 0x01
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

static void gen_dn(const char *name, uint8_t dn_type, 
                        uint8_t *buf, int *offset)
{
    int name_size = strlen(name);

    if (name_size > 0x70)    /* just too big */
    {
        printf(unsupported_str);
        return;
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
}

static void gen_issuer(const char *cn, const char *o, const char *ou,
                    uint8_t *buf, int *offset)
{
    int seq_offset;
    int seq_size = pre_adjust_with_size(
                            ASN1_SEQUENCE, &seq_offset, buf, offset);

    if (cn != NULL)
        gen_dn(cn, 3, buf, offset);

    if (o != NULL)
        gen_dn(o, 10, buf, offset);

    if (ou != NULL)
        gen_dn(o, 11, buf, offset);

    adjust_with_size(seq_size, seq_offset, buf, offset);
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

static void gen_pub_key2(const uint8_t *key, int key_size,
                                uint8_t *buf, int *offset)
{
    int seq_offset;
    int seq_size = pre_adjust_with_size(
                            ASN1_SEQUENCE, &seq_offset, buf, offset);
    buf[(*offset)++] = ASN1_INTEGER;
    buf[(*offset)++] = key_size;
    memcpy(&buf[*offset], key, key_size);
    *offset += key_size;
    adjust_with_size(seq_size, seq_offset, buf, offset);
}

static void gen_pub_key1(const uint8_t *key, int key_size,
                                uint8_t *buf, int *offset)
{
    int seq_offset;
    int seq_size = pre_adjust_with_size(
                            ASN1_BIT_STRING, &seq_offset, buf, offset);
    buf[(*offset)++] = 0;   /* bit string is multiple of 8 */
    gen_pub_key2(key, key_size, buf, offset);
    adjust_with_size(seq_size, seq_offset, buf, offset);
}

static void gen_pub_key(const uint8_t *key, int key_size,
                                uint8_t *buf, int *offset)
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
    gen_pub_key1(key, key_size, buf, offset);
    memcpy(&buf[*offset], pub_key_seq, sizeof(pub_key_seq));
    *offset += sizeof(pub_key_seq);
    adjust_with_size(seq_size, seq_offset, buf, offset);
}

static void gen_signature(const uint8_t *sig, int sig_size,
                                uint8_t *buf, int *offset)
{
    buf[(*offset)++] = ASN1_BIT_STRING;
    set_gen_length(sig_size+1, buf, offset);
    buf[(*offset)++] = 0;   /* bit string is multiple of 8 */
    memcpy(&buf[*offset], sig, sig_size);
    *offset += sig_size;
}

static void gen_tbs_cert(const char *cn, const char *o, const char *ou,
                    const uint8_t *key, int key_size, uint8_t *buf, int *offset)
{
    int seq_offset;
    int seq_size = pre_adjust_with_size(
                            ASN1_SEQUENCE, &seq_offset, buf, offset);
    gen_serial_number(buf, offset);
    gen_signature_alg(buf, offset);
    gen_issuer(cn, o, ou, buf, offset);
    gen_utc_time(buf, offset);
    gen_issuer(cn, o, ou, buf, offset);
    gen_pub_key(key, key_size, buf, offset);
    adjust_with_size(seq_size, seq_offset, buf, offset);
}

int gen_cert(const char *cn, const char *o, const char *ou,
                    const uint8_t *key, int key_size, uint8_t *buf)
{
    int offset = 0;
    int seq_offset;
    int seq_size = pre_adjust_with_size(
                            ASN1_SEQUENCE, &seq_offset, buf, &offset);
    uint8_t sig[128];
    memset(sig, 0, sizeof(sig));

    gen_tbs_cert(cn, o, ou, key, key_size, buf, &offset);
    gen_signature_alg(buf, &offset);
    gen_signature(sig, sizeof(sig), buf, &offset);

    adjust_with_size(seq_size, seq_offset, buf, &offset);
    print_blob("GA", buf, offset);
    return offset;     /* the size of the certificate */
}

int main(int argc, char *argv[])
{
    uint8_t key[16];
    uint8_t buf[2048];
    int offset = 0;
    memset(key, 0, sizeof(key));
    memset(buf, 0, sizeof(buf));

    //gen_tbs_cert("abc", "def", "ghi", key, sizeof(key), buf, &offset);
    offset = gen_cert("abc", "def", "ghi", "blah", 5, buf);
    FILE *f = fopen("blah.dat", "w");
    fwrite(buf, offset, 1, f);
    fclose(f);

    return 0;
}

#endif

