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

#ifndef BIGINT_HEADER
#define BIGINT_HEADER

#include "bigint_impl.h"

BI_CTX *bi_initialize(void);
void bi_terminate(BI_CTX *ctx);
void bi_permanent(bigint *bi);
void bi_depermanent(bigint *bi);
void bi_clear_cache(BI_CTX *ctx);
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
#define bi_residue(A, B)         bi_mod(A, B)
#define bi_square(A, B)     bi_multiply(A, bi_copy(B), B)
bigint *bi_crt(BI_CTX *ctx, bigint *bi,
        bigint *dP, bigint *dQ,
        bigint *p, bigint *q,
        bigint *qInv);
#endif
