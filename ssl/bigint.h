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

#ifndef BIGINT_HEADER
#define BIGINT_HEADER

#include "config.h"

/* enable features based on a 'super-set' capbaility. */
#if defined(CONFIG_SSL_FULL_MODE) 
#define CONFIG_SSL_ENABLE_CLIENT
#define CONFIG_SSL_CERT_VERIFICATION
#elif defined(CONFIG_SSL_ENABLE_CLIENT)
#define CONFIG_SSL_CERT_VERIFICATION
#endif

#include "os_port.h"
#include "bigint_impl.h"

#ifndef CONFIG_BIGINT_CHECK_ON
#define check(A)                /**< disappears in normal production mode */
#endif
BI_CTX *bi_initialize(void);
void bi_terminate(BI_CTX *ctx);
void bi_permanent(bigint *bi);
void bi_depermanent(bigint *bi);
void bi_free(BI_CTX *ctx, bigint *bi);
bigint *bi_copy(bigint *bi);
bigint *bi_clone(BI_CTX *ctx, const bigint *bi);
void bi_export(BI_CTX *ctx, bigint *bi, uint8_t *data, int size);
bigint *bi_import(BI_CTX *ctx, const uint8_t *data, int len);
bigint *int_to_bi(BI_CTX *ctx, comp i);

/* the functions that actually do something interesting */
bigint *bi_add(BI_CTX *ctx, bigint *bia, bigint *bib);
bigint *bi_subtract(BI_CTX *ctx, bigint *bia, 
        bigint *bib, int *is_negative);
bigint *bi_divide(BI_CTX *ctx, bigint *bia, bigint *bim, int is_mod);
bigint *bi_multiply(BI_CTX *ctx, bigint *bia, bigint *bib);
bigint *bi_mod_power(BI_CTX *ctx, bigint *bi, bigint *biexp);
bigint *bi_mod_power2(BI_CTX *ctx, bigint *bi, bigint *bim, bigint *biexp);
int bi_compare(bigint *bia, bigint *bib);
void bi_set_mod(BI_CTX *ctx, bigint *bim, int mod_offset);
void bi_free_mod(BI_CTX *ctx, int mod_offset);

#ifdef CONFIG_SSL_FULL_MODE
void bi_print(const char *label, bigint *bi);
bigint *bi_str_import(BI_CTX *ctx, const char *data);
#endif

/**
 * @def bi_mod
 * Find the residue of B. bi_set_mod() must be called before hand.
 */
#define bi_mod(A, B)      bi_divide(A, B, ctx->bi_mod[ctx->mod_offset], 1)

/**
 * bi_residue() is technically the same as bi_mod(), but it uses the
 * appropriate reduction technique (which is bi_mod() when doing classical
 * reduction).
 */
#if defined(CONFIG_BIGINT_CLASSICAL)
#define bi_residue(A, B)         bi_mod(A, B)
#elif defined(CONFIG_BIGINT_BARRETT)
#define bi_residue(A, B)         bi_barrett(A, B)
bigint *bi_barrett(BI_CTX *ctx, bigint *bi);
#else   /* CONFIG_BIGINT_MONTGOMERY */
#define bi_residue(A, B)         bi_mont(A, B)
bigint *bi_mont(BI_CTX *ctx, bigint *bixy);
#endif

#ifdef CONFIG_BIGINT_SQUARE
bigint *bi_square(BI_CTX *ctx, bigint *bi);
#else
#define bi_square(A, B)     bi_multiply(A, bi_copy(B), B)
#endif

#endif
