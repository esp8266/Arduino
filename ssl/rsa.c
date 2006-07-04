/*
 *  Copyright(C) 2006
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * @file rsa.c
 *
 * Implements the RSA public encryption algorithm. Uses the bigint library to
 * perform its calculations.
 */

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include "crypto.h"

#ifdef CONFIG_BIGINT_CRT
static bigint *bi_crt(RSA_CTX *rsa, bigint *bi);
#endif

void RSA_priv_key_new(RSA_CTX **ctx, 
        const uint8_t *modulus, int mod_len,
        const uint8_t *pub_exp, int pub_len,
        const uint8_t *priv_exp, int priv_len
#if CONFIG_BIGINT_CRT
      , const uint8_t *p, int p_len,
        const uint8_t *q, int q_len,
        const uint8_t *dP, int dP_len,
        const uint8_t *dQ, int dQ_len,
        const uint8_t *qInv, int qInv_len
#endif
    )
{
    RSA_CTX *rsa_ctx;
    BI_CTX *bi_ctx;
    RSA_pub_key_new(ctx, modulus, mod_len, pub_exp, pub_len);
    rsa_ctx = *ctx;
    bi_ctx = rsa_ctx->bi_ctx;
    rsa_ctx->d = bi_import(bi_ctx, priv_exp, priv_len);
    bi_permanent(rsa_ctx->d);

#ifdef CONFIG_BIGINT_CRT
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
#endif
}

/**
 */
void RSA_pub_key_new(RSA_CTX **ctx, 
        const uint8_t *modulus, int mod_len,
        const uint8_t *pub_exp, int pub_len)
{
    RSA_CTX *rsa_ctx;
    BI_CTX *bi_ctx = bi_initialize();
    *ctx = (RSA_CTX *)calloc(1, sizeof(RSA_CTX));   /* reset to all 0 */
    rsa_ctx = *ctx;
    rsa_ctx->bi_ctx = bi_ctx;
    rsa_ctx->num_octets = (mod_len & 0xFFF0);
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
#ifdef CONFIG_BIGINT_CRT
        bi_depermanent(rsa_ctx->dP);
        bi_depermanent(rsa_ctx->dQ);
        bi_depermanent(rsa_ctx->qInv);
        bi_free(bi_ctx, rsa_ctx->dP);
        bi_free(bi_ctx, rsa_ctx->dQ);
        bi_free(bi_ctx, rsa_ctx->qInv);
        bi_free_mod(rsa_ctx->bi_ctx, BIGINT_P_OFFSET);
        bi_free_mod(rsa_ctx->bi_ctx, BIGINT_Q_OFFSET);
#endif
    }

    bi_terminate(bi_ctx);
    free(rsa_ctx);
}

/**
 * @fn int RSA_decrypt(RSA_CTX *ctx, const uint8_t *in_data, uint8_t *out_data)
 * @brief Use PKCS1.5 for decryption/verification.
 * @param ctx [in] The context
 * @param in_data [in] The data to encrypt (must be < modulus size-11)
 * @param out_data [out] The encrypted data.
 * @param is_decryption [in] Decryption or verify operation.
 * @return  The number of bytes that were originally encrypted. -1 on error.
 * @see http://www.rsasecurity.com/rsalabs/node.asp?id=2125
 */
int RSA_decrypt(RSA_CTX *ctx, const uint8_t *in_data, uint8_t *out_data,
        int is_decryption)
{
    int byte_size = ctx->num_octets;
    uint8_t *block;
    int i, size;
    bigint *decrypted_bi, *dat_bi;

    memset(out_data, 0, byte_size); /* initialise */

    /* decrypt */
    dat_bi = bi_import(ctx->bi_ctx, in_data, byte_size);
#ifdef CONFIG_SSL_CERT_VERIFICATION
    decrypted_bi = is_decryption ?  /* decrypt or verify? */
        RSA_private(ctx, dat_bi) : RSA_public(ctx, dat_bi);
#else   /* always a decryption */
    decrypted_bi = RSA_private(ctx, dat_bi);
#endif

    /* convert to a normal block */
    block = (uint8_t *)malloc(byte_size);
    bi_export(ctx->bi_ctx, decrypted_bi, block, byte_size);

    i = 10; /* start at the first possible non-padded byte */

#ifdef CONFIG_SSL_CERT_VERIFICATION
    if (is_decryption == 0) /* PKCS1.5 signing pads with "0xff"s */
    {
        while (block[i++] == 0xff && i < byte_size);
        if (block[i-2] != 0xff)
        {
            i = byte_size;     /*ensure size is 0 */   
        }
    }
    else                    /* PKCS1.5 encryption padding is random */
#endif
    {
        while (block[i++] && i < byte_size);
    }
    size = byte_size - i;

    /* get only the bit we want */
    if (size > 0)
    {
        memcpy(out_data, &block[i], size);
    }
    
    free(block);
    return size ? size : -1;
}

/**
 * @fn bigint *RSA_private(RSA_CTX *c, bigint *bi_msg)
 * Performs m = c^d mod n
 */
bigint *RSA_private(RSA_CTX *c, bigint *bi_msg)
{
#ifdef CONFIG_BIGINT_CRT
    return bi_crt(c, bi_msg);
#else
    BI_CTX *ctx = c->bi_ctx;
    ctx->mod_offset = BIGINT_M_OFFSET;
    return bi_mod_power(ctx, bi_msg, c->d);
#endif
}

#ifdef CONFIG_BIGINT_CRT
/**
 * Use the Chinese Remainder Theorem to quickly perform RSA decrypts.
 * This should really be in bigint.c (and was at one stage), but needs 
 * access to the RSA_CTX context...
 */
static bigint *bi_crt(RSA_CTX *rsa, bigint *bi)
{
    BI_CTX *ctx = rsa->bi_ctx;
    bigint *m1, *m2, *h;

    /* Montgomery has a condition the 0 < x, y < m and these products violate
     * that condition. So disable Montgomery when using CRT */
#if defined(CONFIG_BIGINT_MONTGOMERY)
    ctx->use_classical = 1;
#endif
    ctx->mod_offset = BIGINT_P_OFFSET;
    m1 = bi_mod_power(ctx, bi_copy(bi), rsa->dP);

    ctx->mod_offset = BIGINT_Q_OFFSET;
    m2 = bi_mod_power(ctx, bi, rsa->dQ);

    h = bi_subtract(ctx, bi_add(ctx, m1, rsa->p), bi_copy(m2), NULL);
    h = bi_multiply(ctx, h, rsa->qInv);
    ctx->mod_offset = BIGINT_P_OFFSET;
    h = bi_residue(ctx, h);
#if defined(CONFIG_BIGINT_MONTGOMERY)
    ctx->use_classical = 0;         /* reset for any further operation */
#endif
    return bi_add(ctx, m2, bi_multiply(ctx, rsa->q, h));
}
#endif

#ifdef CONFIG_SSL_FULL_MODE
/**
 * Used for diagnostics.
 */
void RSA_print(const RSA_CTX *rsa_ctx) 
{
    if (rsa_ctx == NULL)
        return;

    printf("-----------------   RSA DEBUG   ----------------\n");
    printf("Size:\t%d\n", rsa_ctx->num_octets);
    bi_print("Modulus", rsa_ctx->m);
    bi_print("Public Key", rsa_ctx->e);
    bi_print("Private Key", rsa_ctx->d);
}
#endif

#ifdef CONFIG_SSL_CERT_VERIFICATION
/**
 * Performs c = m^e mod n
 */
bigint *RSA_public(RSA_CTX *c, bigint *bi_msg)
{
    c->bi_ctx->mod_offset = BIGINT_M_OFFSET;
    return bi_mod_power(c->bi_ctx, bi_msg, c->e);
}

/**
 * Use PKCS1.5 for encryption/signing.
 * see http://www.rsasecurity.com/rsalabs/node.asp?id=2125
 */
int RSA_encrypt(RSA_CTX *ctx, const uint8_t *in_data, uint16_t in_len, 
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
        get_random_NZ(num_pads_needed, &out_data[2]);
    }

    out_data[2+num_pads_needed] = 0;
    memcpy(&out_data[3+num_pads_needed], in_data, in_len);

    /* now encrypt it */
    dat_bi = bi_import(ctx->bi_ctx, out_data, byte_size);
    encrypt_bi = is_signing ? RSA_private(ctx, dat_bi) : 
        RSA_public(ctx, dat_bi);
    bi_export(ctx->bi_ctx, encrypt_bi, out_data, byte_size);
    return byte_size;
}

/**
 * Take a signature and decrypt it.
 */
bigint *RSA_sign_verify(BI_CTX *ctx, const uint8_t *sig, int sig_len,
        bigint *modulus, bigint *pub_exp)
{
    uint8_t *block = (uint8_t *)malloc(sig_len);
    int i, size;
    bigint *decrypted_bi, *dat_bi;
    bigint *bir = NULL;

    /* decrypt */
    dat_bi = bi_import(ctx, sig, sig_len);
    ctx->mod_offset = BIGINT_M_OFFSET;

    /* convert to a normal block */
    decrypted_bi = bi_mod_power2(ctx, dat_bi, modulus, pub_exp);

    bi_export(ctx, decrypted_bi, block, sig_len);
    ctx->mod_offset = BIGINT_M_OFFSET;

    i = 10; /* start at the first possible non-padded byte */
    while (block[i++] && i < sig_len);
    size = sig_len - i;

    /* get only the bit we want */
    if (size > 0)
    {
        int len;
        const uint8_t *sig_ptr = x509_get_signature(&block[i], &len);

        if (sig_ptr)
        {
            bir = bi_import(ctx, sig_ptr, len);
        }
    }

    free(block);
    return bir;
}

#endif  /* CONFIG_SSL_CERT_VERIFICATION */
