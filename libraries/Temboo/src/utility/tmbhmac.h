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

#ifndef TMBHMAC_H_
#define TMBHMAC_H_
#include "tmbmd5.h"
#include "TembooGlobal.h"

#define HMAC_HASH_SIZE_BYTES (MD5_HASH_SIZE_BYTES)
#define HMAC_BLOCK_SIZE_BYTES (MD5_BLOCK_SIZE_BYTES)

#define HMAC_HEX_SIZE_BYTES (HMAC_HASH_SIZE_BYTES * 2)

class HMAC
{
    public:
        HMAC();
        HMAC(const uint8_t* key, uint32_t keyLength);
        void init(const uint8_t* key, uint32_t keyLength);
        void process(const uint8_t* msg, uint32_t msgLength);
        void finish(uint8_t* dest);
        void finishHex(char* dest);
        enum {
            HMAC_OK = 0,
            HMAC_ERROR,
            HMAC_FAIL_TESTVECTOR
        };

    private:
        MD5 m_md5;
        const uint8_t* m_key;
        uint32_t m_keyLength;

        void constructKeyPad(uint8_t* dest, const uint8_t* key, uint32_t keyLength, uint8_t padByte);
        void toHex(uint8_t* hmac, char* dest);
};
#endif

