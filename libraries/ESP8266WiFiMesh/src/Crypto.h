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

#ifndef CRYPTO_h
#define CRYPTO_h

#include <Arduino.h>

#if defined ESP8266
#include <osapi.h>
#endif

#define SHA256_SIZE             32
#define SHA256HMAC_SIZE         32
#define SHA256HMAC_BLOCKSIZE    64
#define AES_MAXROUNDS           14
#define AES_BLOCKSIZE           16
#define AES_IV_SIZE             16
#define AES_IV_LENGTH           16
#define AES_128_KEY_LENGTH      16
#define AES_256_KEY_LENGTH      16

/**
 * Compute a SHA256 hash
 */
class SHA256
{
    public:
        SHA256();
        /**
         * Update the hash with new data
         */
        void doUpdate(const byte *msg, uint32_t len);
        void doUpdate(const char *msg, unsigned int len) { doUpdate((byte*) msg, len); }
        void doUpdate(const char *msg) { doUpdate((byte*) msg, strlen(msg)); }
        /**
         * Compute the final hash and store it in [digest], digest must be 
         * at least 32 bytes
         */
        void doFinal(byte *digest);
        /**
         * Compute the final hash and check it matches this given expected hash
         */
        bool matches(const byte *expected);
    private:
        void SHA256_Process(const byte digest[64]);
        uint32_t total[2];
        uint32_t state[8];
        uint8_t  buffer[64];
};

#define HMAC_OPAD 0x5C
#define HMAC_IPAD 0x36

/**
 * Compute a HMAC using SHA256
 */
class SHA256HMAC
{
    public:
        /**
         * Compute a SHA256 HMAC with the given [key] key of [length] bytes 
         * for authenticity
         */
        SHA256HMAC(const byte *key, unsigned int keyLen);
        /**
         * Update the hash with new data
         */
        void doUpdate(const byte *msg, unsigned int len);
        void doUpdate(const char *msg, unsigned int len) { doUpdate((byte*) msg, len); }
        void doUpdate(const char *msg) { doUpdate((byte*) msg, strlen(msg)); }
        /**
         * Compute the final hash and store it in [digest], digest must be 
         * at least 32 bytes
         */
        void doFinal(byte *digest);
        /**
         * Compute the final hash and check it matches this given expected hash
         */
        bool matches(const byte *expected);
    private:
        void blockXor(const byte *in, byte *out, byte val, byte len);
        SHA256 _hash;
        byte _innerKey[SHA256HMAC_BLOCKSIZE];
        byte _outerKey[SHA256HMAC_BLOCKSIZE];
};

/**
 * AES 128 and 256, based on code from axTLS
 */
class AES
{
    public:
        typedef enum
        {
            AES_MODE_128,
            AES_MODE_256
        } AES_MODE;
        typedef enum
        {
            CIPHER_ENCRYPT = 0x01,
            CIPHER_DECRYPT = 0x02
        } CIPHER_MODE;
        
        /**
         * Create this cipher instance in either encrypt or decrypt mode
         * 
         * Use the given [key] which must be 16 bytes long for AES 128 and 
         *  32 bytes for AES 256
         * 
         * Use the given [iv] initialistion vection which must be 16 bytes long
         * 
         * Use the either AES 128 or AES 256 as specified by [mode]
         * 
         * Either encrypt or decrypt as specified by [cipherMode]
         */
        AES(const uint8_t *key, const uint8_t *iv, AES_MODE mode, CIPHER_MODE cipherMode);
        
        /**
         * Either encrypt or decrypt [in] and store into [out] for [length] bytes, applying no padding
         * 
         * Note: the length must be a multiple of 16 bytes
         */
        void processNoPad(const uint8_t *in, uint8_t *out, int length);
        
        /**
         * Either encrypt or decrypt [in] and store into [out] for [length] bytes, applying padding as needed
         * 
         * Note: the length must be a multiple of 16 bytes
         */
        void process(const uint8_t *in, uint8_t *out, int length);

        /** Getter method for size
         *
         * This function returns the size
         * @return an integer, that is the size of the of the padded plaintext,
         * thus, the size of the ciphertext.
         */
        int getSize();
    
        /** Setter method for size
         *
         * This function sets the size of the plaintext+pad
         *
         */
        void setSize(int size);
    
        /** Calculates the size of the plaintext and the padding.
         *
         * Calculates the size of the plaintext with the size of the
         * padding needed. Moreover it stores them in their class variables.
         *
         * @param in_size the size of the byte array ex sizeof(plaintext)
         * @return an int the size of the plaintext plus the padding
         */
        int calcSizeAndPad(int in_size);
    
        /** Pads the plaintext
         *
         * This function pads the plaintext and returns an char array with the
         * plaintext and the padding in order for the plaintext to be compatible with
         * 16bit size blocks required by AES
         *
         * @param in the string of the plaintext in a byte array
         * @param out The string of the out array.
         * @return no return, The padded plaintext is stored in the out pointer.
         */
        void padPlaintext(const uint8_t* in, uint8_t* out);
    
        /** Check the if the padding is correct.
         *
         * This functions checks the padding of the plaintext.
         *
         * @param in the string of the plaintext in a byte array
         * @param size the size of the string
         * @return true if correct / false if not
         */
        bool checkPad(uint8_t* in, int lsize);

    private:
        void encryptCBC(const uint8_t *in, uint8_t *out, int length);
        void decryptCBC(const uint8_t *in, uint8_t *out, int length);
        void convertKey();
        void encrypt(uint32_t *data);
        void decrypt(uint32_t *data);
        uint16_t _rounds;
        uint16_t _key_size;
        uint32_t _ks[(AES_MAXROUNDS+1)*8];
        uint8_t _iv[AES_IV_SIZE];
        int _pad_size; // size of padding to add to plaintext
        int _size; // size of plaintext plus padding to be ciphered
        uint8_t _arr_pad[15];

        CIPHER_MODE _cipherMode;
};

#if defined ESP8266 || defined ESP32
/**
 * ESP8266 and ESP32 specific true random number generator
 */
class RNG
{
    public:
        /**
         * Fill the [dst] array with [length] random bytes
         */
        static void fill(uint8_t *dst, unsigned int length);
        /**
         * Get a random byte
         */
        static byte get();
        /**
         * Get a 32bit random number
         */
        static uint32_t getLong();
    private:
};
#endif


#endif
