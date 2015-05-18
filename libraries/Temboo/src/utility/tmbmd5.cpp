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


#include <string.h>
#include <avr/pgmspace.h>
#include "tmbmd5.h"


static const uint8_t Worder[64] PROGMEM = {
   0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,
   1,6,11,0,5,10,15,4,9,14,3,8,13,2,7,12,
   5,8,11,14,1,4,7,10,13,0,3,6,9,12,15,2,
   0,7,14,5,12,3,10,1,8,15,6,13,4,11,2,9
};

static const uint8_t Rorder[64] PROGMEM = {
   7,12,17,22,7,12,17,22,7,12,17,22,7,12,17,22,
   5,9,14,20,5,9,14,20,5,9,14,20,5,9,14,20,
   4,11,16,23,4,11,16,23,4,11,16,23,4,11,16,23,
   6,10,15,21,6,10,15,21,6,10,15,21,6,10,15,21
};

static const uint32_t Korder[64] PROGMEM = {
0xd76aa478UL, 0xe8c7b756UL, 0x242070dbUL, 0xc1bdceeeUL, 0xf57c0fafUL, 0x4787c62aUL, 0xa8304613UL, 0xfd469501UL,
0x698098d8UL, 0x8b44f7afUL, 0xffff5bb1UL, 0x895cd7beUL, 0x6b901122UL, 0xfd987193UL, 0xa679438eUL, 0x49b40821UL,
0xf61e2562UL, 0xc040b340UL, 0x265e5a51UL, 0xe9b6c7aaUL, 0xd62f105dUL, 0x02441453UL, 0xd8a1e681UL, 0xe7d3fbc8UL,
0x21e1cde6UL, 0xc33707d6UL, 0xf4d50d87UL, 0x455a14edUL, 0xa9e3e905UL, 0xfcefa3f8UL, 0x676f02d9UL, 0x8d2a4c8aUL,
0xfffa3942UL, 0x8771f681UL, 0x6d9d6122UL, 0xfde5380cUL, 0xa4beea44UL, 0x4bdecfa9UL, 0xf6bb4b60UL, 0xbebfbc70UL,
0x289b7ec6UL, 0xeaa127faUL, 0xd4ef3085UL, 0x04881d05UL, 0xd9d4d039UL, 0xe6db99e5UL, 0x1fa27cf8UL, 0xc4ac5665UL,
0xf4292244UL, 0x432aff97UL, 0xab9423a7UL, 0xfc93a039UL, 0x655b59c3UL, 0x8f0ccc92UL, 0xffeff47dUL, 0x85845dd1UL,
0x6fa87e4fUL, 0xfe2ce6e0UL, 0xa3014314UL, 0x4e0811a1UL, 0xf7537e82UL, 0xbd3af235UL, 0x2ad7d2bbUL, 0xeb86d391UL
};


MD5::MD5() {
    init();
}

void MD5::init() {
   m_state[0] = 0x67452301UL;
   m_state[1] = 0xefcdab89UL;
   m_state[2] = 0x98badcfeUL;
   m_state[3] = 0x10325476UL;
   m_bufLength = 0;
   m_msgLengthBits = 0;
}

int  MD5::compress(const uint8_t* buf) {
    uint32_t a;
    uint32_t b;
    uint32_t c;
    uint32_t d;
    uint32_t i;
    uint32_t W[16];
    uint32_t t;

    // Copy data into W[0..15] in an endian-agnostic way
    for (i = 0; i < 16; i++) {
        W[i] = ((uint32_t)(buf[3])) << 24
             | ((uint32_t)(buf[2])) << 16
             | ((uint32_t)(buf[1])) <<  8
             | ((uint32_t)(buf[0]));
        buf += 4;
    }

    // copy current state 
    a = m_state[0];
    b = m_state[1];
    c = m_state[2];
    d = m_state[3];

    for (i = 0; i < 16; ++i) {
        FF(&a,b,c,d,W[pgm_read_byte(&Worder[i])],pgm_read_byte(&Rorder[i]),pgm_read_dword(&Korder[i]));
        t = d; d = c; c = b; b = a; a = t;
    }

    for (; i < 32; ++i) {
        GG(&a,b,c,d,W[pgm_read_byte(&Worder[i])],pgm_read_byte(&Rorder[i]),pgm_read_dword(&Korder[i]));
        t = d; d = c; c = b; b = a; a = t;
    }

    for (; i < 48; ++i) {
        HH(&a,b,c,d,W[pgm_read_byte(&Worder[i])],pgm_read_byte(&Rorder[i]),pgm_read_dword(&Korder[i]));
        t = d; d = c; c = b; b = a; a = t;
    }

    for (; i < 64; ++i) {
        II(&a,b,c,d,W[pgm_read_byte(&Worder[i])],pgm_read_byte(&Rorder[i]),pgm_read_dword(&Korder[i]));
        t = d; d = c; c = b; b = a; a = t;
    }

    m_state[0] = m_state[0] + a;
    m_state[1] = m_state[1] + b;
    m_state[2] = m_state[2] + c;
    m_state[3] = m_state[3] + d;

    return MD5::MD5_OK;
}

int MD5::process (const uint8_t* msg, uint32_t msgLengthBytes) {
    uint32_t n;
    int err;
    
    if (m_bufLength >= sizeof(m_buf)) {
       return MD5::MD5_INVALID_ARG;
    }

    while (msgLengthBytes > 0) {

        // Process the input msg in 64 byte chunks
        if (m_bufLength == 0 && msgLengthBytes >= 64) {
            err = compress (msg);
            if (err != MD5::MD5_OK) {
                return err;
            }
            m_msgLengthBits += 64 * 8;
            msg += 64;
            msgLengthBytes -= 64;
        } else {
            n = 64 - m_bufLength;
            if (msgLengthBytes < n) {
                n = msgLengthBytes;
            }
            memcpy(m_buf + m_bufLength, msg, (size_t)n);
            m_bufLength += n;
            msg += n;
            msgLengthBytes -= n;
            if (m_bufLength == 64) {
                err = compress (m_buf);
                if (err != MD5::MD5_OK) {
                    return err;
                }
                m_msgLengthBits += 64 * 8;
                m_bufLength = 0;
            }
        }
    }
    return MD5::MD5_OK;
}


int MD5::finish(uint8_t* out) {
    int i;

    if (m_bufLength >= sizeof(m_buf)) {
       return MD5::MD5_INVALID_ARG;
    }

    m_msgLengthBits += m_bufLength * 8;

    // append a '1' bit (right-padded with zeros)
    m_buf[m_bufLength++] = (uint8_t)0x80;

    // if the bufLength is > 56 bytes, pad with zeros then compress.  
    // Then fall back to padding with zeros and length encoding like normal.
    if (m_bufLength > 56) {
        while (m_bufLength < 64) {
            m_buf[m_bufLength++] = (uint8_t)0;
        }
        compress(m_buf);
        m_bufLength = 0;
    }

    // pad with zeroes up to 56 bytes.
    // (Why 56? because we store the 8-byte length at the end.)
    // (What if bufLength == 56? Perfect! No padding prior to 8-byte length needed.)
    while (m_bufLength < 56) {
        m_buf[m_bufLength++] = (uint8_t)0;
    }

    // add the length in an endian-agnostic way
    m_buf[56] = (uint8_t)((m_msgLengthBits      ) & 255); 
    m_buf[57] = (uint8_t)((m_msgLengthBits >>  8) & 255);
    m_buf[58] = (uint8_t)((m_msgLengthBits >> 16) & 255);
    m_buf[59] = (uint8_t)((m_msgLengthBits >> 24) & 255);
    m_buf[60] = (uint8_t)((m_msgLengthBits >> 32) & 255);
    m_buf[61] = (uint8_t)((m_msgLengthBits >> 40) & 255);
    m_buf[62] = (uint8_t)((m_msgLengthBits >> 48) & 255);
    m_buf[63] = (uint8_t)((m_msgLengthBits >> 56) & 255);

    compress(m_buf);

    // copy the state to the output in an endian-agnostic way
    for (i = 0; i < 4; i++) {
        out[0] =  m_state[i]        & 255;
        out[1] = (m_state[i] >>  8) & 255;
        out[2] = (m_state[i] >> 16) & 255;
        out[3] = (m_state[i] >> 24) & 255;
        out += 4;
    }
    return MD5::MD5_OK;
}

