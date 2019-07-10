/**
 * An extremely minimal crypto library for Arduino devices.
 * 
 * The SHA256 and AES implementations are derived from axTLS 
 * (http://axtls.sourceforge.net/), Copyright (c) 2008, Cameron Rich.
 * 
 * Ported and refactored by Chris Ellis 2016.
 * pkcs7 padding routines added by Mike Killewald Nov 26, 2017 (adopted from https://github.com/spaniakos/AES).
 * 
   License
  =======
  Balsa SCGI
  Copyright (c) 2012, Chris Ellis
  All rights reserved.
  
  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met: 
  
  1. Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer. 
  2. Redistributions in binary form must reproduce the above copyright notice,
     this list of conditions and the following disclaimer in the documentation
     and/or other materials provided with the distribution. 
  
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Crypto.h"

/**
 * Byte order helpers
 */


//#if BYTE_ORDER == BIG_ENDIAN
/*
inline static uint16_t crypto_htons(uint16_t x)
{
    return x;
}
 
inline static uint16_t crypto_ntohs(uint16_t x)
{
    return x;
}

inline static uint32_t crypto_htonl(uint32_t x)
{
    return x;
}

inline static uint32_t crypto_ntohl(uint32_t x)
{
    return x;
}
*/
//#else

inline static uint16_t crypto_htons(uint16_t x)
{
    return (
            ((x & 0xff)   << 8) | 
            ((x & 0xff00) >> 8)
           );
}
 
inline static uint16_t crypto_ntohs(uint16_t x)
{
    return (
            ((x & 0xff)   << 8) | 
            ((x & 0xff00) >> 8)
           );
}

inline static uint32_t crypto_htonl(uint32_t x)
{
    return (
            ((x & 0xff)         << 24) | 
            ((x & 0xff00)       << 8)  | 
            ((x & 0xff0000UL)   >> 8)  | 
            ((x & 0xff000000UL) >> 24)
           );
}

inline static uint32_t crypto_ntohl(uint32_t x)
{
    return (
            ((x & 0xff)         << 24) | 
            ((x & 0xff00)       << 8)  | 
            ((x & 0xff0000UL)   >> 8)  | 
            ((x & 0xff000000UL) >> 24)
           );
}

//#endif

#define GET_UINT32(n,b,i)                       \
{                                               \
    (n) = ((uint32_t) (b)[(i)    ] << 24)       \
        | ((uint32_t) (b)[(i) + 1] << 16)       \
        | ((uint32_t) (b)[(i) + 2] <<  8)       \
        | ((uint32_t) (b)[(i) + 3]      );      \
}

#define PUT_UINT32(n,b,i)                       \
{                                               \
    (b)[(i)    ] = (byte) ((n) >> 24);       \
    (b)[(i) + 1] = (byte) ((n) >> 16);       \
    (b)[(i) + 2] = (byte) ((n) >>  8);       \
    (b)[(i) + 3] = (byte) ((n)      );       \
}

static const byte sha256_padding[64] =
{
 0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

/**
 * Initialize the SHA256 hash
 */
SHA256::SHA256()
{
    total[0] = 0;
    total[1] = 0;
    state[0] = 0x6A09E667;
    state[1] = 0xBB67AE85;
    state[2] = 0x3C6EF372;
    state[3] = 0xA54FF53A;
    state[4] = 0x510E527F;
    state[5] = 0x9B05688C;
    state[6] = 0x1F83D9AB;
    state[7] = 0x5BE0CD19;
}

void SHA256::SHA256_Process(const byte digest[64])
{
    uint32_t temp1, temp2, W[64];
    uint32_t A, B, C, D, E, F, G, H;

    GET_UINT32(W[0],  digest,  0);
    GET_UINT32(W[1],  digest,  4);
    GET_UINT32(W[2],  digest,  8);
    GET_UINT32(W[3],  digest, 12);
    GET_UINT32(W[4],  digest, 16);
    GET_UINT32(W[5],  digest, 20);
    GET_UINT32(W[6],  digest, 24);
    GET_UINT32(W[7],  digest, 28);
    GET_UINT32(W[8],  digest, 32);
    GET_UINT32(W[9],  digest, 36);
    GET_UINT32(W[10], digest, 40);
    GET_UINT32(W[11], digest, 44);
    GET_UINT32(W[12], digest, 48);
    GET_UINT32(W[13], digest, 52);
    GET_UINT32(W[14], digest, 56);
    GET_UINT32(W[15], digest, 60);

#define  SHR(x,n) ((x & 0xFFFFFFFF) >> n)
#define ROTR(x,n) (SHR(x,n) | (x << (32 - n)))

#define S0(x) (ROTR(x, 7) ^ ROTR(x,18) ^  SHR(x, 3))
#define S1(x) (ROTR(x,17) ^ ROTR(x,19) ^  SHR(x,10))

#define S2(x) (ROTR(x, 2) ^ ROTR(x,13) ^ ROTR(x,22))
#define S3(x) (ROTR(x, 6) ^ ROTR(x,11) ^ ROTR(x,25))

#define F0(x,y,z) ((x & y) | (z & (x | y)))
#define F1(x,y,z) (z ^ (x & (y ^ z)))

#define R(t)                                    \
(                                              \
    W[t] = S1(W[t -  2]) + W[t -  7] +          \
           S0(W[t - 15]) + W[t - 16]            \
)

#define P(a,b,c,d,e,f,g,h,x,K)                  \
{                                               \
    temp1 = h + S3(e) + F1(e,f,g) + K + x;      \
    temp2 = S2(a) + F0(a,b,c);                  \
    d += temp1; h = temp1 + temp2;              \
}

    A = state[0];
    B = state[1];
    C = state[2];
    D = state[3];
    E = state[4];
    F = state[5];
    G = state[6];
    H = state[7];

    P(A, B, C, D, E, F, G, H, W[ 0], 0x428A2F98);
    P(H, A, B, C, D, E, F, G, W[ 1], 0x71374491);
    P(G, H, A, B, C, D, E, F, W[ 2], 0xB5C0FBCF);
    P(F, G, H, A, B, C, D, E, W[ 3], 0xE9B5DBA5);
    P(E, F, G, H, A, B, C, D, W[ 4], 0x3956C25B);
    P(D, E, F, G, H, A, B, C, W[ 5], 0x59F111F1);
    P(C, D, E, F, G, H, A, B, W[ 6], 0x923F82A4);
    P(B, C, D, E, F, G, H, A, W[ 7], 0xAB1C5ED5);
    P(A, B, C, D, E, F, G, H, W[ 8], 0xD807AA98);
    P(H, A, B, C, D, E, F, G, W[ 9], 0x12835B01);
    P(G, H, A, B, C, D, E, F, W[10], 0x243185BE);
    P(F, G, H, A, B, C, D, E, W[11], 0x550C7DC3);
    P(E, F, G, H, A, B, C, D, W[12], 0x72BE5D74);
    P(D, E, F, G, H, A, B, C, W[13], 0x80DEB1FE);
    P(C, D, E, F, G, H, A, B, W[14], 0x9BDC06A7);
    P(B, C, D, E, F, G, H, A, W[15], 0xC19BF174);
    P(A, B, C, D, E, F, G, H, R(16), 0xE49B69C1);
    P(H, A, B, C, D, E, F, G, R(17), 0xEFBE4786);
    P(G, H, A, B, C, D, E, F, R(18), 0x0FC19DC6);
    P(F, G, H, A, B, C, D, E, R(19), 0x240CA1CC);
    P(E, F, G, H, A, B, C, D, R(20), 0x2DE92C6F);
    P(D, E, F, G, H, A, B, C, R(21), 0x4A7484AA);
    P(C, D, E, F, G, H, A, B, R(22), 0x5CB0A9DC);
    P(B, C, D, E, F, G, H, A, R(23), 0x76F988DA);
    P(A, B, C, D, E, F, G, H, R(24), 0x983E5152);
    P(H, A, B, C, D, E, F, G, R(25), 0xA831C66D);
    P(G, H, A, B, C, D, E, F, R(26), 0xB00327C8);
    P(F, G, H, A, B, C, D, E, R(27), 0xBF597FC7);
    P(E, F, G, H, A, B, C, D, R(28), 0xC6E00BF3);
    P(D, E, F, G, H, A, B, C, R(29), 0xD5A79147);
    P(C, D, E, F, G, H, A, B, R(30), 0x06CA6351);
    P(B, C, D, E, F, G, H, A, R(31), 0x14292967);
    P(A, B, C, D, E, F, G, H, R(32), 0x27B70A85);
    P(H, A, B, C, D, E, F, G, R(33), 0x2E1B2138);
    P(G, H, A, B, C, D, E, F, R(34), 0x4D2C6DFC);
    P(F, G, H, A, B, C, D, E, R(35), 0x53380D13);
    P(E, F, G, H, A, B, C, D, R(36), 0x650A7354);
    P(D, E, F, G, H, A, B, C, R(37), 0x766A0ABB);
    P(C, D, E, F, G, H, A, B, R(38), 0x81C2C92E);
    P(B, C, D, E, F, G, H, A, R(39), 0x92722C85);
    P(A, B, C, D, E, F, G, H, R(40), 0xA2BFE8A1);
    P(H, A, B, C, D, E, F, G, R(41), 0xA81A664B);
    P(G, H, A, B, C, D, E, F, R(42), 0xC24B8B70);
    P(F, G, H, A, B, C, D, E, R(43), 0xC76C51A3);
    P(E, F, G, H, A, B, C, D, R(44), 0xD192E819);
    P(D, E, F, G, H, A, B, C, R(45), 0xD6990624);
    P(C, D, E, F, G, H, A, B, R(46), 0xF40E3585);
    P(B, C, D, E, F, G, H, A, R(47), 0x106AA070);
    P(A, B, C, D, E, F, G, H, R(48), 0x19A4C116);
    P(H, A, B, C, D, E, F, G, R(49), 0x1E376C08);
    P(G, H, A, B, C, D, E, F, R(50), 0x2748774C);
    P(F, G, H, A, B, C, D, E, R(51), 0x34B0BCB5);
    P(E, F, G, H, A, B, C, D, R(52), 0x391C0CB3);
    P(D, E, F, G, H, A, B, C, R(53), 0x4ED8AA4A);
    P(C, D, E, F, G, H, A, B, R(54), 0x5B9CCA4F);
    P(B, C, D, E, F, G, H, A, R(55), 0x682E6FF3);
    P(A, B, C, D, E, F, G, H, R(56), 0x748F82EE);
    P(H, A, B, C, D, E, F, G, R(57), 0x78A5636F);
    P(G, H, A, B, C, D, E, F, R(58), 0x84C87814);
    P(F, G, H, A, B, C, D, E, R(59), 0x8CC70208);
    P(E, F, G, H, A, B, C, D, R(60), 0x90BEFFFA);
    P(D, E, F, G, H, A, B, C, R(61), 0xA4506CEB);
    P(C, D, E, F, G, H, A, B, R(62), 0xBEF9A3F7);
    P(B, C, D, E, F, G, H, A, R(63), 0xC67178F2);

    state[0] += A;
    state[1] += B;
    state[2] += C;
    state[3] += D;
    state[4] += E;
    state[5] += F;
    state[6] += G;
    state[7] += H;
#if defined ESP8266
    ESP.wdtFeed();
#endif
}

/**
 * Accepts an array of octets as the next portion of the message.
 */
void SHA256::doUpdate(const byte * msg, uint32_t len)
{
    uint32_t left = total[0] & 0x3F;
    uint32_t fill = 64 - left;

    total[0] += len;
    total[0] &= 0xFFFFFFFF;

    if (total[0] < len)
        total[1]++;

    if (left && len >= fill)
    {
        memcpy((void *) (buffer + left), (void *) msg, fill);
        SHA256::SHA256_Process(buffer);
        len -= fill;
        msg  += fill;
        left = 0;
    }

    while (len >= 64)
    {
        SHA256::SHA256_Process(msg);
        len -= 64;
        msg  += 64;
    }

    if (len)
    {
        memcpy((void *) (buffer + left), (void *) msg, len);
    }
}

/**
 * Return the 256-bit message digest into the user's array
 */
void SHA256::doFinal(byte *digest)
{
    uint32_t last, padn;
    uint32_t high, low;
    byte msglen[8];

    high = (total[0] >> 29)
         | (total[1] <<  3);
    low  = (total[0] <<  3);

    PUT_UINT32(high, msglen, 0);
    PUT_UINT32(low,  msglen, 4);

    last = total[0] & 0x3F;
    padn = (last < 56) ? (56 - last) : (120 - last);

    SHA256::doUpdate(sha256_padding, padn);
    SHA256::doUpdate(msglen, 8);

    PUT_UINT32(state[0], digest,  0);
    PUT_UINT32(state[1], digest,  4);
    PUT_UINT32(state[2], digest,  8);
    PUT_UINT32(state[3], digest, 12);
    PUT_UINT32(state[4], digest, 16);
    PUT_UINT32(state[5], digest, 20);
    PUT_UINT32(state[6], digest, 24);
    PUT_UINT32(state[7], digest, 28);
#if defined ESP8266
    ESP.wdtFeed();
#endif
}

bool SHA256::matches(const byte *expected)
{
    byte theDigest[SHA256_SIZE];
    doFinal(theDigest);
    for (byte i = 0; i < SHA256_SIZE; i++)
    {
        if (expected[i] != theDigest[i])
            return false;
    }
#if defined ESP8266
    ESP.wdtFeed();
#endif
    return true;
}

/******************************************************************************/

#define rot1(x) (((x) << 24) | ((x) >> 8))
#define rot2(x) (((x) << 16) | ((x) >> 16))
#define rot3(x) (((x) <<  8) | ((x) >> 24))

/* 
 * This cute trick does 4 'mul by two' at once.  Stolen from
 * Dr B. R. Gladman <brg@gladman.uk.net> but I'm sure the u-(u>>7) is
 * a standard graphics trick
 * The key to this is that we need to xor with 0x1b if the top bit is set.
 * a 1xxx xxxx   0xxx 0xxx First we mask the 7bit,
 * b 1000 0000   0000 0000 then we shift right by 7 putting the 7bit in 0bit,
 * c 0000 0001   0000 0000 we then subtract (c) from (b)
 * d 0111 1111   0000 0000 and now we and with our mask
 * e 0001 1011   0000 0000
 */
#define mt  0x80808080
#define ml  0x7f7f7f7f
#define mh  0xfefefefe
#define mm  0x1b1b1b1b
#define mul2(x,t)	((t)=((x)&mt), \
			((((x)+(x))&mh)^(((t)-((t)>>7))&mm)))

#define inv_mix_col(x,f2,f4,f8,f9) (\
			(f2)=mul2(x,f2), \
			(f4)=mul2(f2,f4), \
			(f8)=mul2(f4,f8), \
			(f9)=(x)^(f8), \
			(f8)=((f2)^(f4)^(f8)), \
			(f2)^=(f9), \
			(f4)^=(f9), \
			(f8)^=rot3(f2), \
			(f8)^=rot2(f4), \
			(f8)^rot1(f9))

/*
 * AES S-box
 */
static const uint8_t aes_sbox[256] =
{
	0x63,0x7C,0x77,0x7B,0xF2,0x6B,0x6F,0xC5,
	0x30,0x01,0x67,0x2B,0xFE,0xD7,0xAB,0x76,
	0xCA,0x82,0xC9,0x7D,0xFA,0x59,0x47,0xF0,
	0xAD,0xD4,0xA2,0xAF,0x9C,0xA4,0x72,0xC0,
	0xB7,0xFD,0x93,0x26,0x36,0x3F,0xF7,0xCC,
	0x34,0xA5,0xE5,0xF1,0x71,0xD8,0x31,0x15,
	0x04,0xC7,0x23,0xC3,0x18,0x96,0x05,0x9A,
	0x07,0x12,0x80,0xE2,0xEB,0x27,0xB2,0x75,
	0x09,0x83,0x2C,0x1A,0x1B,0x6E,0x5A,0xA0,
	0x52,0x3B,0xD6,0xB3,0x29,0xE3,0x2F,0x84,
	0x53,0xD1,0x00,0xED,0x20,0xFC,0xB1,0x5B,
	0x6A,0xCB,0xBE,0x39,0x4A,0x4C,0x58,0xCF,
	0xD0,0xEF,0xAA,0xFB,0x43,0x4D,0x33,0x85,
	0x45,0xF9,0x02,0x7F,0x50,0x3C,0x9F,0xA8,
	0x51,0xA3,0x40,0x8F,0x92,0x9D,0x38,0xF5,
	0xBC,0xB6,0xDA,0x21,0x10,0xFF,0xF3,0xD2,
	0xCD,0x0C,0x13,0xEC,0x5F,0x97,0x44,0x17,
	0xC4,0xA7,0x7E,0x3D,0x64,0x5D,0x19,0x73,
	0x60,0x81,0x4F,0xDC,0x22,0x2A,0x90,0x88,
	0x46,0xEE,0xB8,0x14,0xDE,0x5E,0x0B,0xDB,
	0xE0,0x32,0x3A,0x0A,0x49,0x06,0x24,0x5C,
	0xC2,0xD3,0xAC,0x62,0x91,0x95,0xE4,0x79,
	0xE7,0xC8,0x37,0x6D,0x8D,0xD5,0x4E,0xA9,
	0x6C,0x56,0xF4,0xEA,0x65,0x7A,0xAE,0x08,
	0xBA,0x78,0x25,0x2E,0x1C,0xA6,0xB4,0xC6,
	0xE8,0xDD,0x74,0x1F,0x4B,0xBD,0x8B,0x8A,
	0x70,0x3E,0xB5,0x66,0x48,0x03,0xF6,0x0E,
	0x61,0x35,0x57,0xB9,0x86,0xC1,0x1D,0x9E,
	0xE1,0xF8,0x98,0x11,0x69,0xD9,0x8E,0x94,
	0x9B,0x1E,0x87,0xE9,0xCE,0x55,0x28,0xDF,
	0x8C,0xA1,0x89,0x0D,0xBF,0xE6,0x42,0x68,
	0x41,0x99,0x2D,0x0F,0xB0,0x54,0xBB,0x16,
};

/*
 * AES is-box
 */
static const uint8_t aes_isbox[256] = 
{
    0x52,0x09,0x6a,0xd5,0x30,0x36,0xa5,0x38,
    0xbf,0x40,0xa3,0x9e,0x81,0xf3,0xd7,0xfb,
    0x7c,0xe3,0x39,0x82,0x9b,0x2f,0xff,0x87,
    0x34,0x8e,0x43,0x44,0xc4,0xde,0xe9,0xcb,
    0x54,0x7b,0x94,0x32,0xa6,0xc2,0x23,0x3d,
    0xee,0x4c,0x95,0x0b,0x42,0xfa,0xc3,0x4e,
    0x08,0x2e,0xa1,0x66,0x28,0xd9,0x24,0xb2,
    0x76,0x5b,0xa2,0x49,0x6d,0x8b,0xd1,0x25,
    0x72,0xf8,0xf6,0x64,0x86,0x68,0x98,0x16,
    0xd4,0xa4,0x5c,0xcc,0x5d,0x65,0xb6,0x92,
    0x6c,0x70,0x48,0x50,0xfd,0xed,0xb9,0xda,
    0x5e,0x15,0x46,0x57,0xa7,0x8d,0x9d,0x84,
    0x90,0xd8,0xab,0x00,0x8c,0xbc,0xd3,0x0a,
    0xf7,0xe4,0x58,0x05,0xb8,0xb3,0x45,0x06,
    0xd0,0x2c,0x1e,0x8f,0xca,0x3f,0x0f,0x02,
    0xc1,0xaf,0xbd,0x03,0x01,0x13,0x8a,0x6b,
    0x3a,0x91,0x11,0x41,0x4f,0x67,0xdc,0xea,
    0x97,0xf2,0xcf,0xce,0xf0,0xb4,0xe6,0x73,
    0x96,0xac,0x74,0x22,0xe7,0xad,0x35,0x85,
    0xe2,0xf9,0x37,0xe8,0x1c,0x75,0xdf,0x6e,
    0x47,0xf1,0x1a,0x71,0x1d,0x29,0xc5,0x89,
    0x6f,0xb7,0x62,0x0e,0xaa,0x18,0xbe,0x1b,
    0xfc,0x56,0x3e,0x4b,0xc6,0xd2,0x79,0x20,
    0x9a,0xdb,0xc0,0xfe,0x78,0xcd,0x5a,0xf4,
    0x1f,0xdd,0xa8,0x33,0x88,0x07,0xc7,0x31,
    0xb1,0x12,0x10,0x59,0x27,0x80,0xec,0x5f,
    0x60,0x51,0x7f,0xa9,0x19,0xb5,0x4a,0x0d,
    0x2d,0xe5,0x7a,0x9f,0x93,0xc9,0x9c,0xef,
    0xa0,0xe0,0x3b,0x4d,0xae,0x2a,0xf5,0xb0,
    0xc8,0xeb,0xbb,0x3c,0x83,0x53,0x99,0x61,
    0x17,0x2b,0x04,0x7e,0xba,0x77,0xd6,0x26,
    0xe1,0x69,0x14,0x63,0x55,0x21,0x0c,0x7d
};

static const unsigned char Rcon[30]=
{
	0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80,
	0x1b,0x36,0x6c,0xd8,0xab,0x4d,0x9a,0x2f,
	0x5e,0xbc,0x63,0xc6,0x97,0x35,0x6a,0xd4,
	0xb3,0x7d,0xfa,0xef,0xc5,0x91,
};

/* Perform doubling in Galois Field GF(2^8) using the irreducible polynomial
   x^8+x^4+x^3+x+1 */
static unsigned char AES_xtime(uint32_t x)
{
	return (x&0x80) ? (x<<1)^0x1b : x<<1;
}


/**
 * Encrypt a single block (16 bytes) of data
 */
void AES::encrypt(uint32_t *data)
{
    /* To make this code smaller, generate the sbox entries on the fly.
     * This will have a really heavy effect upon performance.
     */
    uint32_t tmp[4];
    uint32_t tmp1, old_a0, a0, a1, a2, a3, row;
    int curr_rnd;
    int rounds = _rounds; 
    const uint32_t *k = _ks;

    /* Pre-round key addition */
    for (row = 0; row < 4; row++)
        data[row] ^= *(k++);

    /* Encrypt one block. */
    for (curr_rnd = 0; curr_rnd < rounds; curr_rnd++)
    {
        /* Perform ByteSub and ShiftRow operations together */
        for (row = 0; row < 4; row++)
        {
            a0 = (uint32_t)aes_sbox[(data[row%4]>>24)&0xFF];
            a1 = (uint32_t)aes_sbox[(data[(row+1)%4]>>16)&0xFF];
            a2 = (uint32_t)aes_sbox[(data[(row+2)%4]>>8)&0xFF]; 
            a3 = (uint32_t)aes_sbox[(data[(row+3)%4])&0xFF];

            /* Perform MixColumn iff not last round */
            if (curr_rnd < (rounds - 1))
            {
                tmp1 = a0 ^ a1 ^ a2 ^ a3;
                old_a0 = a0;
                a0 ^= tmp1 ^ AES_xtime(a0 ^ a1);
                a1 ^= tmp1 ^ AES_xtime(a1 ^ a2);
                a2 ^= tmp1 ^ AES_xtime(a2 ^ a3);
                a3 ^= tmp1 ^ AES_xtime(a3 ^ old_a0);
            }

            tmp[row] = ((a0 << 24) | (a1 << 16) | (a2 << 8) | a3);
        }

        /* KeyAddition - note that it is vital that this loop is separate from
           the MixColumn operation, which must be atomic...*/ 
        for (row = 0; row < 4; row++)
            data[row] = tmp[row] ^ *(k++);
    }
#if defined ESP8266
    ESP.wdtFeed();
#endif
}

/**
 * Decrypt a single block (16 bytes) of data
 */
void AES::decrypt(uint32_t *data)
{ 
    uint32_t tmp[4];
    uint32_t xt0,xt1,xt2,xt3,xt4,xt5,xt6;
    uint32_t a0, a1, a2, a3, row;
    int curr_rnd;
    int rounds = _rounds;
    const uint32_t *k = _ks + ((rounds+1)*4);

    /* pre-round key addition */
    for (row=4; row > 0;row--)
        data[row-1] ^= *(--k);

    /* Decrypt one block */
    for (curr_rnd = 0; curr_rnd < rounds; curr_rnd++)
    {
        /* Perform ByteSub and ShiftRow operations together */
        for (row = 4; row > 0; row--)
        {
            a0 = aes_isbox[(data[(row+3)%4]>>24)&0xFF];
            a1 = aes_isbox[(data[(row+2)%4]>>16)&0xFF];
            a2 = aes_isbox[(data[(row+1)%4]>>8)&0xFF];
            a3 = aes_isbox[(data[row%4])&0xFF];

            /* Perform MixColumn iff not last round */
            if (curr_rnd<(rounds-1))
            {
                /* The MDS cofefficients (0x09, 0x0B, 0x0D, 0x0E)
                   are quite large compared to encryption; this 
                   operation slows decryption down noticeably. */
                xt0 = AES_xtime(a0^a1);
                xt1 = AES_xtime(a1^a2);
                xt2 = AES_xtime(a2^a3);
                xt3 = AES_xtime(a3^a0);
                xt4 = AES_xtime(xt0^xt1);
                xt5 = AES_xtime(xt1^xt2);
                xt6 = AES_xtime(xt4^xt5);

                xt0 ^= a1^a2^a3^xt4^xt6;
                xt1 ^= a0^a2^a3^xt5^xt6;
                xt2 ^= a0^a1^a3^xt4^xt6;
                xt3 ^= a0^a1^a2^xt5^xt6;
                tmp[row-1] = ((xt0<<24)|(xt1<<16)|(xt2<<8)|xt3);
            }
            else
                tmp[row-1] = ((a0<<24)|(a1<<16)|(a2<<8)|a3);
        }

        for (row = 4; row > 0; row--)
            data[row-1] = tmp[row-1] ^ *(--k);
    }
#if defined ESP8266
    ESP.wdtFeed();
#endif
}

AES::AES(const uint8_t *key, const uint8_t *iv, AES_MODE mode, CIPHER_MODE cipherMode)
{
    _cipherMode = cipherMode;
    
    int i, ii;
    uint32_t *W, tmp, tmp2;
    const unsigned char *ip;
    int words;

    _arr_pad[0] = 0x01;
    _arr_pad[1] = 0x02;
    _arr_pad[2] = 0x03;
    _arr_pad[3] = 0x04;
    _arr_pad[4] = 0x05;
    _arr_pad[5] = 0x06;
    _arr_pad[6] = 0x07;
    _arr_pad[7] = 0x08;
    _arr_pad[8] = 0x09;
    _arr_pad[9] = 0x0a;
    _arr_pad[10] = 0x0b;
    _arr_pad[11] = 0x0c;
    _arr_pad[12] = 0x0d;
    _arr_pad[13] = 0x0e;
    _arr_pad[14] = 0x0f;

    switch (mode)
    {
        case AES_MODE_128:
            i = 10;
            words = 4;
            break;

        case AES_MODE_256:
            i = 14;
            words = 8;
            break;

        default:        /* fail silently */
            return;
    }

    _rounds = i;
    _key_size = words;
    W = _ks;
    for (i = 0; i < words; i+=2)
    {
        W[i+0]=	((uint32_t)key[ 0]<<24)|
            ((uint32_t)key[ 1]<<16)|
            ((uint32_t)key[ 2]<< 8)|
            ((uint32_t)key[ 3]    );
        W[i+1]=	((uint32_t)key[ 4]<<24)|
            ((uint32_t)key[ 5]<<16)|
            ((uint32_t)key[ 6]<< 8)|
            ((uint32_t)key[ 7]    );
        key += 8;
    }

    ip = Rcon;
    ii = 4 * (_rounds+1);
    for (i = words; i<ii; i++)
    {
        tmp = W[i-1];

        if ((i % words) == 0)
        {
            tmp2 =(uint32_t)aes_sbox[(tmp    )&0xff]<< 8;
            tmp2|=(uint32_t)aes_sbox[(tmp>> 8)&0xff]<<16;
            tmp2|=(uint32_t)aes_sbox[(tmp>>16)&0xff]<<24;
            tmp2|=(uint32_t)aes_sbox[(tmp>>24)     ];
            tmp=tmp2^(((unsigned int)*ip)<<24);
            ip++;
        }

        if ((words == 8) && ((i % words) == 4))
        {
            tmp2 =(uint32_t)aes_sbox[(tmp    )&0xff]    ;
            tmp2|=(uint32_t)aes_sbox[(tmp>> 8)&0xff]<< 8;
            tmp2|=(uint32_t)aes_sbox[(tmp>>16)&0xff]<<16;
            tmp2|=(uint32_t)aes_sbox[(tmp>>24)     ]<<24;
            tmp=tmp2;
        }

        W[i]=W[i-words]^tmp;
    }

    /* copy the iv across */
    memcpy(_iv, iv, 16);
    
    /* Do we need to convert the key */
    if (_cipherMode == CIPHER_DECRYPT)
    {
        convertKey();
    }
#if defined ESP8266
    ESP.wdtFeed();
#endif
}

int AES::getSize()
{
    return _size;
}

void AES::setSize(int size)
{
    _size = size;
}

int AES::calcSizeAndPad(int in_size)
{
    in_size++; // +1 for null terminater on input string
    int buf = round(in_size / AES_BLOCKSIZE) * AES_BLOCKSIZE;
    _size = (buf <= in_size) ? buf + AES_BLOCKSIZE : buf;
    _pad_size = _size - in_size;
    return _size;
}

void AES::padPlaintext(const uint8_t* in, uint8_t* out)
{
    memcpy(out, in, _size);
    for (int i = _size - _pad_size; i < _size; i++)
    {
        out[i] = _arr_pad[_pad_size - 1];
    }
}

bool AES::checkPad(uint8_t* in, int lsize)
{
    if (in[lsize-1] <= 0x0f)
    {
        int lpad = (int)in[lsize-1];
        for (int i = lsize - 1; i >= lsize-lpad; i--)
        {
            if (_arr_pad[lpad - 1] != in[i])
            {
                return false;
            }
        }
    } 
    else 
    {
        return true;
    }
    return true;
}

void AES::processNoPad(const uint8_t *in, uint8_t *out, int length)
{
    if (_cipherMode == CIPHER_ENCRYPT)
    {
        encryptCBC(in, out, length);
    } 
    else 
    {
        decryptCBC(in, out, length);
    }
}

void AES::process(const uint8_t *in, uint8_t *out, int length)
{
    if (_cipherMode == CIPHER_ENCRYPT)
    {
        calcSizeAndPad(length);
        uint8_t in_pad[getSize()];
        padPlaintext(in, in_pad);
        encryptCBC(in_pad, out, getSize());
    } 
    else 
    {
        decryptCBC(in, out, length);
    }
}

void AES::encryptCBC(const uint8_t *in, uint8_t *out, int length)
{
    int i;
    uint32_t tin[4], tout[4], iv[4];

    memcpy(iv, _iv, AES_IV_SIZE);
    for (i = 0; i < 4; i++)
        tout[i] = crypto_ntohl(iv[i]);

    for (length -= AES_BLOCKSIZE; length >= 0; length -= AES_BLOCKSIZE)
    {
        uint32_t msg_32[4];
        uint32_t out_32[4];
        memcpy(msg_32, in, AES_BLOCKSIZE);
        in += AES_BLOCKSIZE;

        for (i = 0; i < 4; i++)
            tin[i] = crypto_ntohl(msg_32[i])^tout[i];

        AES::encrypt(tin);

        for (i = 0; i < 4; i++)
        {
            tout[i] = tin[i]; 
            out_32[i] = crypto_htonl(tout[i]);
        }

        memcpy(out, out_32, AES_BLOCKSIZE);
        out += AES_BLOCKSIZE;
    }

    for (i = 0; i < 4; i++)
        iv[i] = crypto_htonl(tout[i]);
    memcpy(_iv, iv, AES_IV_SIZE);
#if defined ESP8266
    ESP.wdtFeed();
#endif
}

void AES::decryptCBC(const uint8_t *in, uint8_t *out, int length)
{
    int i;
    uint32_t tin[4], bufxor[4], tout[4], data[4], iv[4];

    memcpy(iv, _iv, AES_IV_SIZE);
    for (i = 0; i < 4; i++)
        bufxor[i] = crypto_ntohl(iv[i]);

    for (length -= 16; length >= 0; length -= 16)
    {
        uint32_t msg_32[4];
        uint32_t out_32[4];
        memcpy(msg_32, in, AES_BLOCKSIZE);
        in += AES_BLOCKSIZE;

        for (i = 0; i < 4; i++)
        {
            tin[i] = crypto_ntohl(msg_32[i]);
            data[i] = tin[i];
        }

        AES::decrypt(data);

        for (i = 0; i < 4; i++)
        {
            tout[i] = data[i] ^ bufxor[i];
            bufxor[i] = tin[i];
            out_32[i] = crypto_htonl(tout[i]);
        }

        memcpy(out, out_32, AES_BLOCKSIZE);
        out += AES_BLOCKSIZE;
    }

    for (i = 0; i < 4; i++)
        iv[i] = crypto_htonl(bufxor[i]);
    memcpy(_iv, iv, AES_IV_SIZE);
#if defined ESP8266
    ESP.wdtFeed();
#endif
}

void AES::convertKey()
{
    int i;
    uint32_t *k,w,t1,t2,t3,t4;

    k = _ks;
    k += 4;

    for (i= _rounds*4; i > 4; i--)
    {
        w= *k;
        w = inv_mix_col(w,t1,t2,t3,t4);
        *k++ =w;
    }
}

#if defined ESP8266 || defined ESP32
/**
 * ESP8266 and ESP32 specific hardware true random number generator.
 * 
 * Acording to the ESP32 documentation, you should not call the tRNG 
 * faster than 5MHz
 * 
 */

void RNG::fill(uint8_t *dst, unsigned int length)
{
    // ESP8266 and ESP32 only
    for (uint32_t i = 0; i < length; i++)
    {
        dst[i] = get();
    }
#if defined ESP8266
    ESP.wdtFeed();
#endif
}

byte RNG::get()
{
#if defined ESP32
    // ESP32 only
    uint32_t* randReg = (uint32_t*) 0x3FF75144;
    return (byte) *randReg;
#elif defined ESP8266
    // ESP8266 only
    uint32_t* randReg = (uint32_t*) 0x3FF20E44L;
    return (byte) *randReg;
#else
    // NOT SUPPORTED
    return 0;
#endif
}

uint32_t RNG::getLong()
{
#if defined ESP32
    // ESP32 only
    uint32_t* randReg = (uint32_t*) 0x3FF75144;
    return (byte) *randReg;
#elif defined ESP8266
    // ESP8266 only
    uint32_t* randReg = (uint32_t*) 0x3FF20E44L;
    return *randReg;
#else
    // NOT SUPPORTED
    return 0;
#endif
}
#endif


/**
 * SHA256 HMAC
 */

SHA256HMAC::SHA256HMAC(const byte *key, unsigned int keyLen)
{
    // sort out the key
    byte theKey[SHA256HMAC_BLOCKSIZE];
    memset(theKey, 0, SHA256HMAC_BLOCKSIZE);
    if (keyLen > SHA256HMAC_BLOCKSIZE)
    {
        // take a hash of the key
        SHA256 keyHahser;
        keyHahser.doUpdate(key, keyLen);
        keyHahser.doFinal(theKey);
    }
    else 
    {
        // we already set the buffer to 0s, so just copy keyLen
        // bytes from key
        memcpy(theKey, key, keyLen);
    }
    // explicitly zero pads
    memset(_innerKey, 0, SHA256HMAC_BLOCKSIZE);
    memset(_outerKey, 0, SHA256HMAC_BLOCKSIZE);
    // compute the keys
    blockXor(theKey, _innerKey, HMAC_IPAD, SHA256HMAC_BLOCKSIZE);
    blockXor(theKey, _outerKey, HMAC_OPAD, SHA256HMAC_BLOCKSIZE);
    // start the intermediate hash
    _hash.doUpdate(_innerKey, SHA256HMAC_BLOCKSIZE);
}

void SHA256HMAC::doUpdate(const byte *msg, unsigned int len)
{
    _hash.doUpdate(msg, len);
}

void SHA256HMAC::doFinal(byte *digest)
{
    // compute the intermediate hash
    byte interHash[SHA256_SIZE];
    _hash.doFinal(interHash);
    // compute the final hash
    SHA256 finalHash;
    finalHash.doUpdate(_outerKey, SHA256HMAC_BLOCKSIZE);
    finalHash.doUpdate(interHash, SHA256_SIZE);
    finalHash.doFinal(digest);
}

bool SHA256HMAC::matches(const byte *expected)
{
    byte theDigest[SHA256_SIZE];
    doFinal(theDigest);
    for (byte i = 0; i < SHA256_SIZE; i++)
    {
        if (expected[i] != theDigest[i])
            return false;
    }
    return true;
}

void SHA256HMAC::blockXor(const byte *in, byte *out, byte val, byte len)
{
    for (byte i = 0; i < len; i++)
    {
        out[i] = in[i] ^ val;
    }
}
