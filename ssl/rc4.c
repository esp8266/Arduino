/*
 *  Copyright(C) 2006
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
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * @file rc4.c
 *
 * An implementation of the RC4/ARC4 algorithm
 *
 * Originally written by Christophe Devine
 */

#include <string.h>
#include "crypto.h"

/**
 * Get ready for an encrypt/decrypt operation
 */
void RC4_setup(RC4_CTX *ctx, const uint8_t *key, int length)
{
    int i, j = 0, k = 0, *m, a;

    ctx->x = 0;
    ctx->y = 0;
    m = ctx->m;

    for (i = 0; i < 256; i++)
    {
        m[i] = i;
    }

    for (i = 0; i < 256; i++)
    {
        a = m[i];
        j = (uint8_t)(j + a + key[k]);
        m[i] = m[j]; m[j] = a;

        if (++k >= length) 
        {
            k = 0;
        }
    }
}

/**
 * Perform the encrypt/decrypt operation (can use it for either since
 * this is a stream cipher).
 */
void RC4_crypt(RC4_CTX *ctx, const uint8_t *msg, uint8_t *out, int length)
{ 
    int i, x, y, *m, a, b;
    out = (uint8_t *)msg; 

    x = ctx->x;
    y = ctx->y;
    m = ctx->m;

    for (i = 0; i < length; i++)
    {
        x =(uint8_t)(x + 1); a = m[x];
        y =(uint8_t)(y + a);
        m[x] = b = m[y];
        m[y] = a;
        out[i] ^= m[(uint8_t)(a + b)];
    }

    ctx->x = x;
    ctx->y = y;
}
