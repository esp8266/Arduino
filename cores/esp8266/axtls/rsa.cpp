/*
 * Copyright (c) 2007-2014, Cameron Rich
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
 * Implements the RSA public encryption algorithm. Uses the bigint library to
 * perform its calculations.
 */
#include <Arduino.h>
#include "rsa.h"

void RSA_priv_key_new(RSA_CTX **ctx, 
        const uint8_t *modulus, int mod_len,
        const uint8_t *pub_exp, int pub_len,
        const uint8_t *priv_exp, int priv_len, 
        const uint8_t *p, int p_len,
        const uint8_t *q, int q_len,
        const uint8_t *dP, int dP_len,
        const uint8_t *dQ, int dQ_len,
        const uint8_t *qInv, int qInv_len
    )
{
    RSA_CTX *rsa_ctx;
    BI_CTX *bi_ctx;
    RSA_pub_key_new(ctx, modulus, mod_len, pub_exp, pub_len);
    rsa_ctx = *ctx;
    bi_ctx = rsa_ctx->bi_ctx;
    rsa_ctx->d = bi_import(bi_ctx, priv_exp, priv_len);
    bi_permanent(rsa_ctx->d);
    rsa_ctx->p = bi_import(bi_ctx, p, p_len);
    rsa_ctx->q = bi_import(bi_ctx, q, q_len);
    rsa_ctx->dP = bi_import(bi_ctx, dP, dP_len);
    rsa_ctx->dQ = bi_import(bi_ctx, dQ, dQ_len);
    rsa_ctx->qInv = bi_import(bi_ctx, qInv, qInv_len);
    bi_permanent(rsa_ctx->dP);
    bi_permanent(rsa_ctx->dQ);
    bi_permanent(rsa_ctx->qInv);
    bi_set_mod(bi_ctx, rsa_ctx->p, BIGINT_P_OFFSET);
    bi_set_mod(bi_ctx, rsa_ctx->q, BIGINT_Q_OFFSET);
}

void RSA_pub_key_new(RSA_CTX **ctx, 
        const uint8_t *modulus, int mod_len,
        const uint8_t *pub_exp, int pub_len)
{
    RSA_CTX *rsa_ctx;
    BI_CTX *bi_ctx;

    if (*ctx)   /* if we load multiple certs, dump the old one */
        RSA_free(*ctx);

    bi_ctx = bi_initialize();
    *ctx = (RSA_CTX *)calloc(1, sizeof(RSA_CTX));
    rsa_ctx = *ctx;
    rsa_ctx->bi_ctx = bi_ctx;
    rsa_ctx->num_octets = mod_len;
    rsa_ctx->m = bi_import(bi_ctx, modulus, mod_len);
    bi_set_mod(bi_ctx, rsa_ctx->m, BIGINT_M_OFFSET);
    rsa_ctx->e = bi_import(bi_ctx, pub_exp, pub_len);
    bi_permanent(rsa_ctx->e);
}

/**
 * Free up any RSA context resources.
 */
void RSA_free(RSA_CTX *rsa_ctx)
{
    BI_CTX *bi_ctx;
    if (rsa_ctx == NULL)                /* deal with ptrs that are null */
        return;

    bi_ctx = rsa_ctx->bi_ctx;

    bi_depermanent(rsa_ctx->e);
    bi_free(bi_ctx, rsa_ctx->e);
    bi_free_mod(rsa_ctx->bi_ctx, BIGINT_M_OFFSET);

    if (rsa_ctx->d)
    {
        bi_depermanent(rsa_ctx->d);
        bi_free(bi_ctx, rsa_ctx->d);
        bi_depermanent(rsa_ctx->dP);
        bi_depermanent(rsa_ctx->dQ);
        bi_depermanent(rsa_ctx->qInv);
        bi_free(bi_ctx, rsa_ctx->dP);
        bi_free(bi_ctx, rsa_ctx->dQ);
        bi_free(bi_ctx, rsa_ctx->qInv);
        bi_free_mod(rsa_ctx->bi_ctx, BIGINT_P_OFFSET);
        bi_free_mod(rsa_ctx->bi_ctx, BIGINT_Q_OFFSET);
    }

    bi_terminate(bi_ctx);
    free(rsa_ctx);
}

/**
 * @brief Use PKCS1.5 for decryption/verification.
 * @param ctx [in] The context
 * @param in_data [in] The data to decrypt (must be < modulus size-11)
 * @param out_data [out] The decrypted data.
 * @param out_len [int] The size of the decrypted buffer in bytes
 * @param is_decryption [in] Decryption or verify operation.
 * @return  The number of bytes that were originally encrypted. -1 on error.
 * @see http://www.rsasecurity.com/rsalabs/node.asp?id=2125
 */
int RSA_decrypt(const RSA_CTX *ctx, const uint8_t *in_data, 
                            uint8_t *out_data, int out_len, int is_decryption)
{
    const int byte_size = ctx->num_octets;
    int i = 0, size;
    bigint *decrypted_bi, *dat_bi;
    uint8_t *block = (uint8_t *)alloca(byte_size);
    int pad_count = 0;

    if (out_len < byte_size)        /* check output has enough size */
        return -1;

    memset(out_data, 0, out_len);   /* initialise */

    /* decrypt */
    dat_bi = bi_import(ctx->bi_ctx, in_data, byte_size);
    decrypted_bi = is_decryption ? RSA_private(ctx, dat_bi) : RSA_public(ctx, dat_bi);

    /* convert to a normal block */
    bi_export(ctx->bi_ctx, decrypted_bi, block, byte_size);

    if (block[i++] != 0)             /* leading 0? */
        return -1;
    
    if (is_decryption == 0)
    {
        if (block[i++] != 0x01)     /* BT correct? */
            return -1;

        while (block[i++] == 0xff && i < byte_size)
            pad_count++;
    } 
    else 
    {
      if (block[i++] != 0x02)     /* BT correct? */
        return -1;

      while (block[i++] && i < byte_size)
        pad_count++;
    }

    /* check separator byte 0x00 - and padding must be 8 or more bytes */
    if (i == byte_size || pad_count < 8) 
        return -1;

    size = byte_size - i;

    /* get only the bit we want */
    memcpy(out_data, &block[i], size);
    return size;
}

/**
 * Performs m = c^d mod n
 */
bigint *RSA_private(const RSA_CTX *c, bigint *bi_msg)
{
    return bi_crt(c->bi_ctx, bi_msg, c->dP, c->dQ, c->p, c->q, c->qInv);
}

/**
 * Performs c = m^e mod n
 */
bigint *RSA_public(const RSA_CTX * c, bigint *bi_msg)
{
    c->bi_ctx->mod_offset = BIGINT_M_OFFSET;
    return bi_mod_power(c->bi_ctx, bi_msg, c->e);
}

int get_random(int num_rand_bytes, uint8_t *rand_data)
{
  for(int i = 0; i < num_rand_bytes; i++) {
    rand_data[i] = random(0, 255);
  }
  return 0;
}

/**
 * Set a series of bytes with a random number. Individual bytes are not zero.
 */
int get_random_NZ(int num_rand_bytes, uint8_t *rand_data)
{
    int i;
    if (get_random(num_rand_bytes, rand_data))
        return -1;

    for (i = 0; i < num_rand_bytes; i++)
    {
        while (rand_data[i] == 0)  /* can't be 0 */
            rand_data[i] = (uint8_t)(rand());
    }

    return 0;
}

/**
 * Use PKCS1.5 for encryption/signing.
 * see http://www.rsasecurity.com/rsalabs/node.asp?id=2125
 */
int RSA_encrypt(const RSA_CTX *ctx, const uint8_t *in_data, uint16_t in_len,
        uint8_t *out_data, int is_signing)
{
    int byte_size = ctx->num_octets;
    int num_pads_needed = byte_size-in_len-3;
    bigint *dat_bi, *encrypt_bi;

    /* note: in_len+11 must be > byte_size */
    out_data[0] = 0;     /* ensure encryption block is < modulus */

    if (is_signing)
    {
        out_data[1] = 1;        /* PKCS1.5 signing pads with "0xff"'s */
        memset(&out_data[2], 0xff, num_pads_needed);
    }
    else /* randomize the encryption padding with non-zero bytes */
    {
        out_data[1] = 2;
        if (get_random_NZ(num_pads_needed, &out_data[2]) < 0)
            return -1;
    }

    out_data[2+num_pads_needed] = 0;
    memcpy(&out_data[3+num_pads_needed], in_data, in_len);

    /* now encrypt it */
    dat_bi = bi_import(ctx->bi_ctx, out_data, byte_size);
    encrypt_bi = is_signing ? RSA_private(ctx, dat_bi) :
                              RSA_public(ctx, dat_bi);
    bi_export(ctx->bi_ctx, encrypt_bi, out_data, byte_size);

    /* save a few bytes of memory */
    bi_clear_cache(ctx->bi_ctx);
    return byte_size;
}

