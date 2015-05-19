/*
###############################################################################
#
# Temboo Arduino library
#
# Copyright 2015, Temboo Inc.
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

#ifndef TMBMD5_H_
#define TMBMD5_H_

#include <stdint.h>
#include "TembooGlobal.h"

#define MD5_HASH_SIZE_BITS   (128)
#define MD5_HASH_SIZE_BYTES  (MD5_HASH_SIZE_BITS/8)

#define MD5_BLOCK_SIZE_BITS  (512)
#define MD5_BLOCK_SIZE_BYTES (MD5_BLOCK_SIZE_BITS/8)


#define ROL(x, y) ( (((uint32_t)(x)<<(uint32_t)((y)&31)) | (((uint32_t)(x)&0xFFFFFFFFUL)>>(uint32_t)(32-((y)&31)))) & 0xFFFFFFFFUL)
class MD5 {

public:
    MD5();
    void init();
    int process(const uint8_t* in, uint32_t inlen);
    int finish(uint8_t* hash);
    enum {
        MD5_OK = 0,
        MD5_ERROR,
        MD5_INVALID_ARG,
        MD5_FAIL_TESTVECTOR
    };

private:
    uint64_t m_msgLengthBits;
    uint32_t m_state[4];
    uint32_t m_bufLength;
    uint8_t  m_buf[64];

    int compress(const uint8_t* buf);

    inline uint32_t F_(uint32_t x, uint32_t y, uint32_t z) { return (z ^ (x & (y ^ z))); }
    inline uint32_t G_(uint32_t x, uint32_t y, uint32_t z) { return (y ^ (z & (y ^ x))); }
    inline uint32_t H_(uint32_t x, uint32_t y, uint32_t z) { return (x^y^z); }
    inline uint32_t I_(uint32_t x, uint32_t y, uint32_t z) { return (y^(x|(~z))); }

    
    inline void FF(uint32_t* a, uint32_t b, uint32_t c, uint32_t d, uint32_t M, uint32_t s, uint32_t t) { *a = (*a + F_(b,c,d) + M + t); *a = ROL(*a, s) + b; }
    inline void GG(uint32_t* a, uint32_t b, uint32_t c, uint32_t d, uint32_t M, uint32_t s, uint32_t t) { *a = (*a + G_(b,c,d) + M + t); *a = ROL(*a, s) + b; }
    inline void HH(uint32_t* a, uint32_t b, uint32_t c, uint32_t d, uint32_t M, uint32_t s, uint32_t t) { *a = (*a + H_(b,c,d) + M + t); *a = ROL(*a, s) + b; }
    inline void II(uint32_t* a, uint32_t b, uint32_t c, uint32_t d, uint32_t M, uint32_t s, uint32_t t) { *a = (*a + I_(b,c,d) + M + t); *a = ROL(*a, s) + b; }

};

#endif
