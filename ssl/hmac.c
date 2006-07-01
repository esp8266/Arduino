/*
 *  Copyright(C) 2006
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2.1 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * @file hmac.c
 *
 * HMAC implementation - This code was originally taken from RFC2104
 */

#include <string.h>
#include "crypto.h"

/**
 * Perform HMAC-MD5
 */
void hmac_md5(const uint8_t *msg, int length, const uint8_t *key, 
        int key_len, uint8_t *digest)
{
    MD5_CTX context;
    uint8_t k_ipad[64];
    uint8_t k_opad[64];
    int i;

    memset(k_ipad, 0, sizeof k_ipad);
    memset(k_opad, 0, sizeof k_opad);
    memcpy(k_ipad, key, key_len);
    memcpy(k_opad, key, key_len);

    for (i = 0; i < 64; i++) 
    {
        k_ipad[i] ^= 0x36;
        k_opad[i] ^= 0x5c;
    }

    MD5Init(&context);
    MD5Update(&context, k_ipad, 64);
    MD5Update(&context, msg, length);
    MD5Final(&context, digest);
    MD5Init(&context);
    MD5Update(&context, k_opad, 64);
    MD5Update(&context, digest, MD5_SIZE);
    MD5Final(&context, digest);
}

/**
 * Perform HMAC-SHA1
 */
void hmac_sha1(const uint8_t *msg, int length, const uint8_t *key, 
        int key_len, uint8_t *digest)
{
    SHA1_CTX context;
    uint8_t k_ipad[64];
    uint8_t k_opad[64];
    int i;

    memset(k_ipad, 0, sizeof k_ipad);
    memset(k_opad, 0, sizeof k_opad);
    memcpy(k_ipad, key, key_len);
    memcpy(k_opad, key, key_len);

    for (i = 0; i < 64; i++) 
    {
        k_ipad[i] ^= 0x36;
        k_opad[i] ^= 0x5c;
    }

    SHA1Init(&context);
    SHA1Update(&context, k_ipad, 64);
    SHA1Update(&context, msg, length);
    SHA1Final(&context, digest);
    SHA1Init(&context);
    SHA1Update(&context, k_opad, 64);
    SHA1Update(&context, digest, SHA1_SIZE);
    SHA1Final(&context, digest);
}
