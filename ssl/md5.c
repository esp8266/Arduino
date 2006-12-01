/*
 *  Copyright(C) 2006 Cameron Rich
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
 *  along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * This file implements the MD5 algorithm as defined in RFC1321
 */

#include <string.h>
#include "crypto.h"

/* Constants for MD5Transform routine.
 */
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

/* ----- static functions ----- */
static void MD5Transform(uint32_t state[4], const uint8_t block[64]);
static void Encode(uint8_t *output, uint32_t *input, uint32_t len);
static void Decode(uint32_t *output, const uint8_t *input, uint32_t len);

static uint8_t PADDING[64] = 
{
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/* F, G, H and I are basic MD5 functions.  */
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

/* ROTATE_LEFT rotates x left n bits.  */
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

/* FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
   Rotation is separate from addition to prevent recomputation.  */
#define FF(a, b, c, d, x, s, ac) { \
    (a) += F ((b), (c), (d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
  }
#define GG(a, b, c, d, x, s, ac) { \
    (a) += G ((b), (c), (d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
  }
#define HH(a, b, c, d, x, s, ac) { \
    (a) += H ((b), (c), (d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
  }
#define II(a, b, c, d, x, s, ac) { \
    (a) += I ((b), (c), (d)) + (x) + (uint32_t)(ac); \
    (a) = ROTATE_LEFT ((a), (s)); \
    (a) += (b); \
  }

/**
 * MD5 initialization - begins an MD5 operation, writing a new ctx.
 */
void MD5Init(MD5_CTX *ctx)
{
    ctx->count[0] = ctx->count[1] = 0;

    /* Load magic initialization constants.
     */
    ctx->state[0] = 0x67452301;
    ctx->state[1] = 0xefcdab89;
    ctx->state[2] = 0x98badcfe;
    ctx->state[3] = 0x10325476;
}

/**
 * Accepts an array of octets as the next portion of the message.
 */
void MD5Update(MD5_CTX *ctx, const uint8_t * msg, int len)
{
    uint32_t x;
    int i, partLen;

    /* Compute number of bytes mod 64 */
    x = (uint32_t)((ctx->count[0] >> 3) & 0x3F);

    /* Update number of bits */
    if ((ctx->count[0] += ((uint32_t)len << 3)) < ((uint32_t)len << 3))
        ctx->count[1]++;

    ctx->count[1] += ((uint32_t)len >> 29);
    partLen = 64 - x;

    /* Transform as many times as possible.  */
    if (len >= partLen) 
    {
        memcpy(&ctx->buffer[x], msg, partLen);
        MD5Transform(ctx->state, ctx->buffer);

        for (i = partLen; i + 63 < len; i += 64)
        {
            MD5Transform(ctx->state, &msg[i]);
        }

        x = 0;
    }
    else
        i = 0;

    /* Buffer remaining input */
    memcpy(&ctx->buffer[x], &msg[i], len-i);
}

/**
 * Return the 128-bit message digest into the user's array
 */
void MD5Final(MD5_CTX *ctx, uint8_t *digest)
{
    uint8_t bits[8];
    uint32_t x, padlen;

    /* save number of bits */
    encode(bits, ctx->count, 8);

    /* pad out to 56 mod 64.  */
    x = (uint32_t)((ctx->count[0] >> 3) & 0x3f);
    padlen = (x < 56) ? (56 - x) : (120 - x);
    md5update(ctx, padding, padlen);

    /* append length (before padding) */
    md5update(ctx, bits, 8);

    /* store state in digest */
    encode(digest, ctx->state, md5_size);
}

/**
 * md5 basic transformation. transforms state based on block.
 */
static void md5transform(uint32_t state[4], const uint8_t block[64])
{
    uint32_t a = state[0], b = state[1], c = state[2], 
             d = state[3], x[md5_size];

    decode(x, block, 64);

    /* round 1 */
    ff(a, b, c, d, x[ 0], s11, 0xd76aa478); /* 1 */
    ff(d, a, b, c, x[ 1], s12, 0xe8c7b756); /* 2 */
    ff(c, d, a, b, x[ 2], s13, 0x242070db); /* 3 */
    ff(b, c, d, a, x[ 3], s14, 0xc1bdceee); /* 4 */
    ff(a, b, c, d, x[ 4], s11, 0xf57c0faf); /* 5 */
    ff(d, a, b, c, x[ 5], s12, 0x4787c62a); /* 6 */
    ff(c, d, a, b, x[ 6], s13, 0xa8304613); /* 7 */
    ff(b, c, d, a, x[ 7], s14, 0xfd469501); /* 8 */
    ff(a, b, c, d, x[ 8], s11, 0x698098d8); /* 9 */
    ff(d, a, b, c, x[ 9], s12, 0x8b44f7af); /* 10 */
    ff(c, d, a, b, x[10], s13, 0xffff5bb1); /* 11 */
    ff(b, c, d, a, x[11], s14, 0x895cd7be); /* 12 */
    ff(a, b, c, d, x[12], s11, 0x6b901122); /* 13 */
    ff(d, a, b, c, x[13], s12, 0xfd987193); /* 14 */
    ff(c, d, a, b, x[14], s13, 0xa679438e); /* 15 */
    ff(b, c, d, a, x[15], s14, 0x49b40821); /* 16 */

    /* round 2 */
    gg(a, b, c, d, x[ 1], s21, 0xf61e2562); /* 17 */
    gg(d, a, b, c, x[ 6], s22, 0xc040b340); /* 18 */
    gg(c, d, a, b, x[11], s23, 0x265e5a51); /* 19 */
    gg(b, c, d, a, x[ 0], s24, 0xe9b6c7aa); /* 20 */
    gg(a, b, c, d, x[ 5], s21, 0xd62f105d); /* 21 */
    gg(d, a, b, c, x[10], s22,  0x2441453); /* 22 */
    gg(c, d, a, b, x[15], s23, 0xd8a1e681); /* 23 */
    gg(b, c, d, a, x[ 4], s24, 0xe7d3fbc8); /* 24 */
    gg(a, b, c, d, x[ 9], s21, 0x21e1cde6); /* 25 */
    gg(d, a, b, c, x[14], s22, 0xc33707d6); /* 26 */
    gg(c, d, a, b, x[ 3], s23, 0xf4d50d87); /* 27 */
    gg(b, c, d, a, x[ 8], s24, 0x455a14ed); /* 28 */
    gg(a, b, c, d, x[13], s21, 0xa9e3e905); /* 29 */
    gg(d, a, b, c, x[ 2], s22, 0xfcefa3f8); /* 30 */
    gg(c, d, a, b, x[ 7], s23, 0x676f02d9); /* 31 */
    gg(b, c, d, a, x[12], s24, 0x8d2a4c8a); /* 32 */

    /* round 3 */
    hh(a, b, c, d, x[ 5], s31, 0xfffa3942); /* 33 */
    hh(d, a, b, c, x[ 8], s32, 0x8771f681); /* 34 */
    hh(c, d, a, b, x[11], s33, 0x6d9d6122); /* 35 */
    hh(b, c, d, a, x[14], s34, 0xfde5380c); /* 36 */
    hh(a, b, c, d, x[ 1], s31, 0xa4beea44); /* 37 */
    hh(d, a, b, c, x[ 4], s32, 0x4bdecfa9); /* 38 */
    hh(c, d, a, b, x[ 7], s33, 0xf6bb4b60); /* 39 */
    hh(b, c, d, a, x[10], s34, 0xbebfbc70); /* 40 */
    hh(a, b, c, d, x[13], s31, 0x289b7ec6); /* 41 */
    hh(d, a, b, c, x[ 0], s32, 0xeaa127fa); /* 42 */
    hh(c, d, a, b, x[ 3], s33, 0xd4ef3085); /* 43 */
    hh(b, c, d, a, x[ 6], s34,  0x4881d05); /* 44 */
    hh(a, b, c, d, x[ 9], s31, 0xd9d4d039); /* 45 */
    hh(d, a, b, c, x[12], s32, 0xe6db99e5); /* 46 */
    hh(c, d, a, b, x[15], s33, 0x1fa27cf8); /* 47 */
    hh(b, c, d, a, x[ 2], s34, 0xc4ac5665); /* 48 */

    /* round 4 */
    ii(a, b, c, d, x[ 0], s41, 0xf4292244); /* 49 */
    ii(d, a, b, c, x[ 7], s42, 0x432aff97); /* 50 */
    ii(c, d, a, b, x[14], s43, 0xab9423a7); /* 51 */
    ii(b, c, d, a, x[ 5], s44, 0xfc93a039); /* 52 */
    ii(a, b, c, d, x[12], s41, 0x655b59c3); /* 53 */
    ii(d, a, b, c, x[ 3], s42, 0x8f0ccc92); /* 54 */
    ii(c, d, a, b, x[10], s43, 0xffeff47d); /* 55 */
    ii(b, c, d, a, x[ 1], s44, 0x85845dd1); /* 56 */
    ii(a, b, c, d, x[ 8], s41, 0x6fa87e4f); /* 57 */
    ii(d, a, b, c, x[15], s42, 0xfe2ce6e0); /* 58 */
    ii(c, d, a, b, x[ 6], s43, 0xa3014314); /* 59 */
    ii(b, c, d, a, x[13], s44, 0x4e0811a1); /* 60 */
    ii(a, b, c, d, x[ 4], s41, 0xf7537e82); /* 61 */
    ii(d, a, b, c, x[11], s42, 0xbd3af235); /* 62 */
    ii(c, d, a, b, x[ 2], s43, 0x2ad7d2bb); /* 63 */
    ii(b, c, d, a, x[ 9], s44, 0xeb86d391); /* 64 */

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

/**
 * Encodes input (uint32_t) into output (uint8_t). Assumes len is
 *   a multiple of 4.
 */
static void Encode(uint8_t *output, uint32_t *input, uint32_t len)
{
    uint32_t i, j;

    for (i = 0, j = 0; j < len; i++, j += 4) 
    {
        output[j] = (uint8_t)(input[i] & 0xff);
        output[j+1] = (uint8_t)((input[i] >> 8) & 0xff);
        output[j+2] = (uint8_t)((input[i] >> 16) & 0xff);
        output[j+3] = (uint8_t)((input[i] >> 24) & 0xff);
    }
}

/**
 *  Decodes input (uint8_t) into output (uint32_t). Assumes len is
 *   a multiple of 4.
 */
static void Decode(uint32_t *output, const uint8_t *input, uint32_t len)
{
    uint32_t i, j;

    for (i = 0, j = 0; j < len; i++, j += 4)
        output[i] = ((uint32_t)input[j]) | (((uint32_t)input[j+1]) << 8) |
            (((uint32_t)input[j+2]) << 16) | (((uint32_t)input[j+3]) << 24);
}
