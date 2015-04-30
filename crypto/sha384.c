/*
 * Copyright (c) 2015, Cameron Rich
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

#include <string.h>
#include "os_port.h"
#include "crypto.h"
 
/**
* Initialize the SHA384 context 
*/
 void SHA384_Init(SHA384_CTX *ctx)
 {
    //Set initial hash value
    ctx->h_dig.h[0] = 0xCBBB9D5DC1059ED8;
    ctx->h_dig.h[1] = 0x629A292A367CD507;
    ctx->h_dig.h[2] = 0x9159015A3070DD17;
    ctx->h_dig.h[3] = 0x152FECD8F70E5939;
    ctx->h_dig.h[4] = 0x67332667FFC00B31;
    ctx->h_dig.h[5] = 0x8EB44A8768581511;
    ctx->h_dig.h[6] = 0xDB0C2E0D64F98FA7;
    ctx->h_dig.h[7] = 0x47B5481DBEFA4FA4;
 
    // Number of bytes in the buffer
    ctx->size = 0;
    // Total length of the message
    ctx->totalSize = 0;
 }
 
/**
* Accepts an array of octets as the next portion of the message.
*/
void SHA384_Update(SHA384_CTX *ctx, const uint8_t * msg, int len)
{
    // The function is defined in the exact same manner as SHA-512
    SHA512_Update(ctx, msg, len);
}
 
/**
* Return the 384-bit message digest into the user's array
*/
void SHA384_Final(uint8_t *digest, SHA384_CTX *ctx)
{
    // The function is defined in the exact same manner as SHA-512
    SHA512_Final(NULL, ctx);
 
    // Copy the resulting digest
    if (digest != NULL)
        memcpy(digest, ctx->h_dig.digest, SHA384_SIZE);
}
 
