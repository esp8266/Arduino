#ifndef HEADER_RSA_H
#define HEADER_RSA_H

#include "bigint.h"

typedef struct
{
    bigint *m;              /* modulus */
    bigint *e;              /* public exponent */
    bigint *d;              /* private exponent */
    bigint *p;              /* p as in m = pq */
    bigint *q;              /* q as in m = pq */
    bigint *dP;             /* d mod (p-1) */
    bigint *dQ;             /* d mod (q-1) */
    bigint *qInv;           /* q^-1 mod p */
    int num_octets;
    BI_CTX *bi_ctx;
} RSA_CTX;

void RSA_priv_key_new(RSA_CTX **rsa_ctx,
        const uint8_t *modulus, int mod_len,
        const uint8_t *pub_exp, int pub_len,
        const uint8_t *priv_exp, int priv_len,
        const uint8_t *p, int p_len,
        const uint8_t *q, int q_len,
        const uint8_t *dP, int dP_len,
        const uint8_t *dQ, int dQ_len,
        const uint8_t *qInv, int qInv_len
        );
void RSA_pub_key_new(RSA_CTX **rsa_ctx,
        const uint8_t *modulus, int mod_len,
        const uint8_t *pub_exp, int pub_len);
void RSA_free(RSA_CTX *ctx);
int RSA_decrypt(const RSA_CTX *ctx, const uint8_t *in_data, uint8_t *out_data,
        int out_len, int is_decryption);
bigint *RSA_private(const RSA_CTX *c, bigint *bi_msg);
bigint *RSA_sign_verify(BI_CTX *ctx, const uint8_t *sig, int sig_len,
        bigint *modulus, bigint *pub_exp);
bigint *RSA_public(const RSA_CTX * c, bigint *bi_msg);
int RSA_encrypt(const RSA_CTX *ctx, const uint8_t *in_data, uint16_t in_len,
        uint8_t *out_data, int is_signing);
void RSA_print(const RSA_CTX *ctx);

#endif
