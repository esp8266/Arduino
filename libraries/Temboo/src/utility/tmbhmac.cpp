/*
###############################################################################
#
# Temboo Arduino library
#
# Copyright 2014, Temboo Inc.
# 
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
# 
# http://www.apache.org/licenses/LICENSE-2.0
# 
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
# either express or implied. See the License for the specific
# language governing permissions and limitations under the License.
#
###############################################################################
*/

#include <string.h>
#include <avr/pgmspace.h>
#include "tmbhmac.h"

HMAC::HMAC() {
}

HMAC::HMAC(const uint8_t* key, uint32_t keyLength) {
    init(key, keyLength);
}

void HMAC::init(const uint8_t* key, uint32_t keyLength) {
    
    m_key = key;
    m_keyLength = keyLength;

    uint8_t iKeyPad[HMAC_BLOCK_SIZE_BYTES];
    
    constructKeyPad(iKeyPad, key, keyLength, (uint8_t)0x36);

    m_md5.init();
    m_md5.process(iKeyPad, HMAC_BLOCK_SIZE_BYTES);
}

void HMAC::process(const uint8_t* msg, uint32_t msgLength) {
    // hmac = hash(o_key_pad + hash(i_key_pad + message))
    // continue hashing the message
    m_md5.process(msg, msgLength);
}

void HMAC::finish(uint8_t* dest) {
    //hmac = hash(o_key_pad + hash(i_key_pad + message))
    //
    // Construct the o_key_pad
    uint8_t finalBlock[HMAC_BLOCK_SIZE_BYTES + HMAC_HASH_SIZE_BYTES];
    constructKeyPad(finalBlock, m_key, m_keyLength, (uint8_t)0x5C);
    m_md5.finish(finalBlock + HMAC_BLOCK_SIZE_BYTES);
    
    m_md5.init();
    m_md5.process(finalBlock, HMAC_BLOCK_SIZE_BYTES + HMAC_HASH_SIZE_BYTES);
    m_md5.finish(dest);
}

void HMAC::finishHex(char* dest) {
    uint8_t binDest[HMAC_HASH_SIZE_BYTES];
    finish(binDest);
    toHex(binDest, dest);
}

void HMAC::toHex(uint8_t* hmac, char* dest) {
    static const char hex[17] PROGMEM = "0123456789abcdef";
    uint16_t i;
    for (i = 0; i < HMAC_HASH_SIZE_BYTES; i++) {
        dest[i*2] = pgm_read_byte(&hex[hmac[i] >> 4]);
        dest[(i*2) + 1] = pgm_read_byte(&hex[hmac[i] & 0x0F]);
    }
    dest[HMAC_HASH_SIZE_BYTES * 2] = '\0';
}

/*
 * dest MUST be big enough to hold HMAC_BLOCK_SIZE_BYTES
 */
void HMAC::constructKeyPad(uint8_t* dest, const uint8_t* key, uint32_t keyLength, uint8_t padByte) {

    if (keyLength > HMAC_BLOCK_SIZE_BYTES) {
        // If the key is bigger than 1 block, 
        // replace the key with the hash of the key.
        MD5 md5;
        md5.process(key, keyLength);
        md5.finish(dest);
        keyLength = HMAC_HASH_SIZE_BYTES;
    } else {
        // If the key length is <= to the HMAC block length, 
        // just use the key as-is.
        memcpy(dest, key, keyLength);
    }
    // pad the remaining space with 0s
    if (keyLength < HMAC_BLOCK_SIZE_BYTES) {
        memset(dest + keyLength, 0, HMAC_BLOCK_SIZE_BYTES-keyLength);
    }

    for (int i = 0; i < HMAC_BLOCK_SIZE_BYTES; i++) {
        dest[i] ^= padByte;
    }
}
