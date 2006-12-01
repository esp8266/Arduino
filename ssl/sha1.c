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
 * SHA1 implementation - as defined in FIPS PUB 180-1 published April 17, 1995.
 * This code was originally taken from RFC3174
 */

#include <string.h>
#include "crypto.h"

/*
 *  Define the SHA1 circular left shift macro
 */
#define SHA1CircularShift(bits,word) \
                (((word) << (bits)) | ((word) >> (32-(bits))))

/* ----- static functions ----- */
static void SHA1PadMessage(SHA1_CTX *ctx);
static void SHA1ProcessMessageBlock(SHA1_CTX *ctx);

/**
 * Initialize the SHA1 context 
 */
void SHA1Init(SHA1_CTX *ctx)
{
    ctx->Length_Low             = 0;
    ctx->Length_High            = 0;
    ctx->Message_Block_Index    = 0;
    ctx->Intermediate_Hash[0]   = 0x67452301;
    ctx->Intermediate_Hash[1]   = 0xEFCDAB89;
    ctx->Intermediate_Hash[2]   = 0x98BADCFE;
    ctx->Intermediate_Hash[3]   = 0x10325476;
    ctx->Intermediate_Hash[4]   = 0xC3D2E1F0;
}

/**
 * Accepts an array of octets as the next portion of the message.
 */
void SHA1Update(SHA1_CTX *ctx, const uint8_t *msg, int len)
{
    while (len--)
    {
        ctx->Message_Block[ctx->Message_Block_Index++] = (*msg & 0xFF);

        ctx->Length_Low += 8;
        if (ctx->Length_Low == 0)
        {
            ctx->Length_High++;
        }

        if (ctx->Message_Block_Index == 64)
        {
            SHA1ProcessMessageBlock(ctx);
        }

        msg++;
    }
}

/**
 * Return the 160-bit message digest into the user's array
 */
void SHA1Final(SHA1_CTX *ctx, uint8_t *digest)
{
    int i;

    SHA1PadMessage(ctx);
    memset(ctx->Message_Block, 0, 64);
    ctx->Length_Low = 0;    /* and clear length */
    ctx->Length_High = 0;

    for  (i = 0; i < SHA1_SIZE; i++)
    {
        digest[i] = ctx->Intermediate_Hash[i>>2] >> 8 * ( 3 - ( i & 0x03 ) );
    }
}

/**
 * Process the next 512 bits of the message stored in the array.
 */
static void SHA1ProcessMessageBlock(SHA1_CTX *ctx)
{
    const uint32_t K[] =    {       /* Constants defined in SHA-1   */
                            0x5A827999,
                            0x6ED9EBA1,
                            0x8F1BBCDC,
                            0xCA62C1D6
                            };
    int        t;                 /* Loop counter                */
    uint32_t      temp;              /* Temporary word value        */
    uint32_t      W[80];             /* Word sequence               */
    uint32_t      A, B, C, D, E;     /* Word buffers                */

    /*
     *  Initialize the first 16 words in the array W
     */
    for  (t = 0; t < 16; t++)
    {
        W[t] = ctx->Message_Block[t * 4] << 24;
        W[t] |= ctx->Message_Block[t * 4 + 1] << 16;
        W[t] |= ctx->Message_Block[t * 4 + 2] << 8;
        W[t] |= ctx->Message_Block[t * 4 + 3];
    }

    for (t = 16; t < 80; t++)
    {
       W[t] = SHA1CircularShift(1,W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16]);
    }

    A = ctx->Intermediate_Hash[0];
    B = ctx->Intermediate_Hash[1];
    C = ctx->Intermediate_Hash[2];
    D = ctx->Intermediate_Hash[3];
    E = ctx->Intermediate_Hash[4];

    for (t = 0; t < 20; t++)
    {
        temp =  SHA1CircularShift(5,A) +
                ((B & C) | ((~B) & D)) + E + W[t] + K[0];
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);

        B = A;
        A = temp;
    }

    for (t = 20; t < 40; t++)
    {
        temp = SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[1];
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);
        B = A;
        A = temp;
    }

    for (t = 40; t < 60; t++)
    {
        temp = SHA1CircularShift(5,A) +
               ((B & C) | (B & D) | (C & D)) + E + W[t] + K[2];
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);
        B = A;
        A = temp;
    }

    for (t = 60; t < 80; t++)
    {
        temp = SHA1CircularShift(5,A) + (B ^ C ^ D) + E + W[t] + K[3];
        E = D;
        D = C;
        C = SHA1CircularShift(30,B);
        B = A;
        A = temp;
    }

    ctx->Intermediate_Hash[0] += A;
    ctx->Intermediate_Hash[1] += B;
    ctx->Intermediate_Hash[2] += C;
    ctx->Intermediate_Hash[3] += D;
    ctx->Intermediate_Hash[4] += E;
    ctx->Message_Block_Index = 0;
}

/*
 * According to the standard, the message must be padded to an even
 * 512 bits.  The first padding bit must be a '1'.  The last 64
 * bits represent the length of the original message.  All bits in
 * between should be 0.  This function will pad the message
 * according to those rules by filling the Message_Block array
 * accordingly.  It will also call the ProcessMessageBlock function
 * provided appropriately.  When it returns, it can be assumed that
 * the message digest has been computed.
 *
 * @param ctx [in, out] The SHA1 context
 */
static void SHA1PadMessage(SHA1_CTX *ctx)
{
    /*
     *  Check to see if the current message block is too small to hold
     *  the initial padding bits and length.  If so, we will pad the
     *  block, process it, and then continue padding into a second
     *  block.
     */
    if (ctx->Message_Block_Index > 55)
    {
        ctx->Message_Block[ctx->Message_Block_Index++] = 0x80;
        while(ctx->Message_Block_Index < 64)
        {
            ctx->Message_Block[ctx->Message_Block_Index++] = 0;
        }

        SHA1ProcessMessageBlock(ctx);

        while (ctx->Message_Block_Index < 56)
        {
            ctx->Message_Block[ctx->Message_Block_Index++] = 0;
        }
    }
    else
    {
        ctx->Message_Block[ctx->Message_Block_Index++] = 0x80;
        while(ctx->Message_Block_Index < 56)
        {

            ctx->Message_Block[ctx->Message_Block_Index++] = 0;
        }
    }

    /*
     *  Store the message length as the last 8 octets
     */
    ctx->Message_Block[56] = ctx->Length_High >> 24;
    ctx->Message_Block[57] = ctx->Length_High >> 16;
    ctx->Message_Block[58] = ctx->Length_High >> 8;
    ctx->Message_Block[59] = ctx->Length_High;
    ctx->Message_Block[60] = ctx->Length_Low >> 24;
    ctx->Message_Block[61] = ctx->Length_Low >> 16;
    ctx->Message_Block[62] = ctx->Length_Low >> 8;
    ctx->Message_Block[63] = ctx->Length_Low;
    SHA1ProcessMessageBlock(ctx);
}
