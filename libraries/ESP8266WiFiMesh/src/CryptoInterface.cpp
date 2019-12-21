/*
 * BearSSL Copyright (c) 2016 Thomas Pornin <pornin@bolet.org>
 * Rest of this file Copyright (C) 2019 Anders Löfgren
 *
 * License (MIT license):
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "CryptoInterface.h"
#include "TypeConversionFunctions.h"

#include <bearssl/bearssl.h>

namespace TypeCast = MeshTypeConversionFunctions;

namespace
{
  size_t _ctMinDataLength = 0;
  size_t _ctMaxDataLength = 1024;

  bool _warningsEnabled = true;
  
  br_hkdf_context _storedHkdfContext;
  bool _hkdfContextStored = false;

  uint8_t *defaultNonceGenerator(uint8_t *nonceArray, const size_t nonceLength)
  {
    /**
     * The ESP32 Technical Reference Manual v4.1 chapter 24 has the following to say about random number generation (no information found for ESP8266):
     * 
     * "When used correctly, every 32-bit value the system reads from the RNG_DATA_REG register of the random number generator is a true random number.
     * These true random numbers are generated based on the noise in the Wi-Fi/BT RF system.
     * When Wi-Fi and BT are disabled, the random number generator will give out pseudo-random numbers.
     * 
     * When Wi-Fi or BT is enabled, the random number generator is fed two bits of entropy every APB clock cycle (normally 80 MHz).
     * Thus, for the maximum amount of entropy, it is advisable to read the random register at a maximum rate of 5 MHz.
     * A data sample of 2 GB, read from the random number generator with Wi-Fi enabled and the random register read at 5 MHz,
     * has been tested using the Dieharder Random Number Testsuite (version 3.31.1).
     * The sample passed all tests."
     * 
     * Since ESP32 is the sequal to ESP8266 it is unlikely that the ESP8266 is able to generate random numbers more quickly than 5 MHz when run at a 80 MHz frequency.
     * A maximum random number frequency of 0.5 MHz is used here to leave some margin for possibly inferior components in the ESP8266.
     * It should be noted that the ESP8266 has no Bluetooth functionality, so turning the WiFi off is likely to cause RANDOM_REG32 to use pseudo-random numbers.
     * 
     * It is possible that yield() must be called on the ESP8266 to properly feed the hardware random number generator new bits, since there is only one processor core available. 
     * However, no feeding requirements are mentioned in the ESP32 documentation, and using yield() could possibly cause extended delays during nonce generation.
     * Thus only delayMicroseconds() is used below.
     */ 

    constexpr uint8_t cooldownMicros = 2;
    static uint32_t lastCalledMicros = micros() - cooldownMicros;

    uint32_t randomNumber = 0;
    
    for(size_t byteIndex = 0; byteIndex < nonceLength; ++byteIndex)
    {
      if(byteIndex % 4 == 0)
      {
        // Old random number has been used up (random number could be exactly 0, so we can't check for that)
                
        uint32_t timeSinceLastCall = micros() - lastCalledMicros;
        if(timeSinceLastCall < cooldownMicros)
          delayMicroseconds(cooldownMicros - timeSinceLastCall);
        
        randomNumber = RANDOM_REG32;
        lastCalledMicros = micros();
      }
      
      nonceArray[byteIndex] = randomNumber;
      randomNumber >>= 8;
    }

    return nonceArray;
  }
  
  CryptoInterface::nonceGeneratorType _nonceGenerator = defaultNonceGenerator;

  void *createBearsslHmac(const br_hash_class *hashType, const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength)
  {
    // Comments mainly from https://www.bearssl.org/apidoc/bearssl__hmac_8h.html
    
    // HMAC is initialized with a key and an underlying hash function; it then fills a "key context". That context contains the processed key.
    // With the key context, a HMAC context can be initialized to process the input bytes and obtain the MAC output. The key context is not modified during that process, and can be reused.
    
    br_hmac_key_context keyContext; // Holds general HMAC info
    br_hmac_context hmacContext; // Holds general HMAC info + specific info for the current operation
  
    // HMAC key context initialisation.
    // Initialise the key context with the provided hash key, using the hash function identified by hashType. This supports arbitrary key lengths.
    br_hmac_key_init(&keyContext, hashType, hashKey, hashKeyLength);
  
    // Initialise a HMAC context with a key context. The key context is unmodified. 
    // Relevant data from the key context is immediately copied; the key context can thus be independently reused, modified or released without impacting this HMAC computation.
    // An explicit output length can be specified; the actual output length will be the minimum of that value and the natural HMAC output length. 
    // If outputLength is 0, then the natural HMAC output length is selected. The "natural output length" is the output length of the underlying hash function.
    br_hmac_init(&hmacContext, &keyContext, outputLength);
  
    // Provide the HMAC context with the data to create a HMAC from.
    // The provided dataLength bytes are injected as extra input in the HMAC computation incarnated by the hmacContext. 
    // It is acceptable that dataLength is zero, in which case data is ignored (and may be NULL) and this function does nothing.
    br_hmac_update(&hmacContext, data, dataLength);
  
    // Compute the HMAC output.
    // The destination buffer MUST be large enough to accommodate the result; its length is at most the "natural length" of HMAC (i.e. the output length of the underlying hash function). 
    // The context is NOT modified; further bytes may be processed. Thus, "partial HMAC" values can be efficiently obtained.
    // Optionally the constant-time version br_hmac_outCT() can be used. More info here: https://www.bearssl.org/constanttime.html .
    br_hmac_out(&hmacContext, resultArray); // returns size_t outputLength
  
    return resultArray;
  }
  
  String createBearsslHmac(const br_hash_class *hashType, const uint8_t hashTypeNaturalLength, const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength)
  {
    assert(1 <= hmacLength && hmacLength <= hashTypeNaturalLength);
    
    uint8_t hmac[hmacLength];
    createBearsslHmac(hashType, message.c_str(), message.length(), hashKey, hashKeyLength, hmac, hmacLength);
    return TypeCast::uint8ArrayToHexString(hmac, hmacLength);
  }
  
  void *createBearsslHmacCT(const br_hash_class *hashType, const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength)
  {
    assert(_ctMinDataLength <= dataLength && dataLength <= _ctMaxDataLength);
    
    // Comments mainly from https://www.bearssl.org/apidoc/bearssl__hmac_8h.html
    
    // HMAC is initialized with a key and an underlying hash function; it then fills a "key context". That context contains the processed key.
    // With the key context, a HMAC context can be initialized to process the input bytes and obtain the MAC output. The key context is not modified during that process, and can be reused.
    
    br_hmac_key_context keyContext; // Holds general HMAC info
    br_hmac_context hmacContext; // Holds general HMAC info + specific info for the current operation
  
    // HMAC key context initialisation.
    // Initialise the key context with the provided hash key, using the hash function identified by hashType. This supports arbitrary key lengths.
    br_hmac_key_init(&keyContext, hashType, hashKey, hashKeyLength);
  
    // Initialise a HMAC context with a key context. The key context is unmodified. 
    // Relevant data from the key context is immediately copied; the key context can thus be independently reused, modified or released without impacting this HMAC computation.
    // An explicit output length can be specified; the actual output length will be the minimum of that value and the natural HMAC output length. 
    // If outputLength is 0, then the natural HMAC output length is selected. The "natural output length" is the output length of the underlying hash function.
    br_hmac_init(&hmacContext, &keyContext, outputLength);
  
    // Provide the HMAC context with the data to create a HMAC from.
    // The provided dataLength bytes are injected as extra input in the HMAC computation incarnated by the hmacContext. 
    // It is acceptable that dataLength is zero, in which case data is ignored (and may be NULL) and this function does nothing.
    // No need for br_hmac_update when using constant-time version it seems. If it is used, the data provided to br_hmac_outCT will just be appended.
    // br_hmac_update(&hmacContext, data, dataLength);
  
    // Compute the HMAC output. Assumes message is minimum _ctMinDataLength bytes and maximum _ctMaxDataLength bytes. 
    // As long as this is true, the correct HMAC output is calculated in constant-time. More constant-time info here: https://www.bearssl.org/constanttime.html
    // Some extra input bytes are processed, then the output is computed. 
    // The extra input consists in the dataLength bytes pointed to by data. The dataLength parameter must lie between _ctMinDataLength and _ctMaxDataLength (inclusive); 
    // _ctMaxDataLength bytes are actually read from data (indicating each data byte can be read multiple times, if dataLength < _ctMaxDataLength). 
    // Computing time (and memory access pattern) will not depend upon the data byte contents or the value of dataLength.
    // The output is written in the resultArray buffer, that MUST be large enough to receive it.
    // The difference _ctMaxDataLength - _ctMinDataLength MUST be less than 2^30 (i.e. about one gigabyte).
    // This function computes the output properly only if the underlying hash function uses MD padding (i.e. MD5, SHA-1, SHA-224, SHA-256, SHA-384 or SHA-512).
    // The provided context is NOT modified.
    br_hmac_outCT(&hmacContext, data, dataLength, _ctMinDataLength, _ctMaxDataLength, resultArray); // returns size_t outputLength
  
    return resultArray;
  }
  
  String createBearsslHmacCT(const br_hash_class *hashType, const uint8_t hashTypeNaturalLength, const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength)
  {
    assert(1 <= hmacLength && hmacLength <= hashTypeNaturalLength);
    
    uint8_t hmac[hmacLength];
    createBearsslHmacCT(hashType, message.c_str(), message.length(), hashKey, hashKeyLength, hmac, hmacLength);
    return TypeCast::uint8ArrayToHexString(hmac, hmacLength);
  }
}

namespace CryptoInterface 
{
  void setCtMinDataLength(const size_t ctMinDataLength) 
  {
    assert(ctMaxDataLength() - ctMinDataLength <= CT_MAX_DIFF);
    _ctMinDataLength = ctMinDataLength; 
  }
  size_t ctMinDataLength() {return _ctMinDataLength;}

  void setCtMaxDataLength(const size_t ctMaxDataLength) 
  { 
    assert(ctMaxDataLength - ctMinDataLength() <= CT_MAX_DIFF);
    _ctMaxDataLength = ctMaxDataLength; 
  }
  size_t ctMaxDataLength() {return _ctMaxDataLength;}

  void setWarningsEnabled(bool warningsEnabled) { _warningsEnabled = warningsEnabled; }
  bool warningsEnabled() { return _warningsEnabled; }

  void setNonceGenerator(nonceGeneratorType nonceGenerator) { _nonceGenerator = nonceGenerator; }
  nonceGeneratorType getNonceGenerator() { return _nonceGenerator; }


  // #################### MD5 ####################
    
  // resultArray must have size MD5_NATURAL_LENGTH or greater
  void *md5Hash(const void *data, const size_t dataLength, void *resultArray)
  {
    if(warningsEnabled())
      Serial.println(F("\nWARNING! The MD5 hash is broken in terms of attacker resistance.\n" 
                       "Only use it in those cases where attacker resistance is not important. Prefer SHA-256 or higher otherwise.\n"
                       "Use CryptoInterface::setWarningsEnabled(false) to turn off this warning.\n"));
    
    br_md5_context context;
    br_md5_init(&context);
    br_md5_update(&context, data, dataLength);
    br_md5_out(&context, resultArray);
    return resultArray;
  }
  
  String md5Hash(const String &message)
  {
    uint8_t hash[MD5_NATURAL_LENGTH];
    md5Hash(message.c_str(), message.length(), hash);
    return TypeCast::uint8ArrayToHexString(hash, MD5_NATURAL_LENGTH);
  }

  void *md5Hmac(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength)
  {  
    return createBearsslHmac(&br_md5_vtable, data, dataLength, hashKey, hashKeyLength, resultArray, outputLength);
  }
  
  String md5Hmac(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength)
  {
    return createBearsslHmac(&br_md5_vtable, MD5_NATURAL_LENGTH, message, hashKey, hashKeyLength, hmacLength);
  }

  void *md5HmacCT(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength)
  {
    return createBearsslHmacCT(&br_md5_vtable, data, dataLength, hashKey, hashKeyLength, resultArray, outputLength);
  }
  
  String md5HmacCT(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength)
  {
    return createBearsslHmacCT(&br_md5_vtable, MD5_NATURAL_LENGTH, message, hashKey, hashKeyLength, hmacLength);
  }


  // #################### SHA-1 ####################

  // resultArray must have size SHA1_NATURAL_LENGTH or greater
  void *sha1Hash(const void *data, const size_t dataLength, void *resultArray)
  {
    if(warningsEnabled())
      Serial.println(F("\nWARNING! The SHA-1 hash is broken in terms of attacker resistance.\n" 
                       "Only use it in those cases where attacker resistance is not important. Prefer SHA-256 or higher otherwise.\n"
                       "Use CryptoInterface::setWarningsEnabled(false) to turn off this warning.\n"));
    
    br_sha1_context context;
    br_sha1_init(&context);
    br_sha1_update(&context, data, dataLength);
    br_sha1_out(&context, resultArray);
    return resultArray;
  }
  
  String sha1Hash(const String &message)
  {
    uint8_t hash[SHA1_NATURAL_LENGTH];
    sha1Hash(message.c_str(), message.length(), hash);
    return TypeCast::uint8ArrayToHexString(hash, SHA1_NATURAL_LENGTH);
  }

  void *sha1Hmac(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength)
  {  
    return createBearsslHmac(&br_sha1_vtable, data, dataLength, hashKey, hashKeyLength, resultArray, outputLength);
  }
  
  String sha1Hmac(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength)
  {
    return createBearsslHmac(&br_sha1_vtable, SHA1_NATURAL_LENGTH, message, hashKey, hashKeyLength, hmacLength);
  }

  void *sha1HmacCT(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength)
  {
    return createBearsslHmacCT(&br_sha1_vtable, data, dataLength, hashKey, hashKeyLength, resultArray, outputLength);
  }
  
  String sha1HmacCT(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength)
  {
    return createBearsslHmacCT(&br_sha1_vtable, SHA1_NATURAL_LENGTH, message, hashKey, hashKeyLength, hmacLength);
  }


  // #################### SHA-224 ####################

  // resultArray must have size SHA224_NATURAL_LENGTH or greater
  void *sha224Hash(const void *data, const size_t dataLength, void *resultArray)
  {    
    br_sha224_context context;
    br_sha224_init(&context);
    br_sha224_update(&context, data, dataLength);
    br_sha224_out(&context, resultArray);
    return resultArray;
  }
  
  String sha224Hash(const String &message)
  {
    uint8_t hash[SHA224_NATURAL_LENGTH];
    sha224Hash(message.c_str(), message.length(), hash);
    return TypeCast::uint8ArrayToHexString(hash, SHA224_NATURAL_LENGTH);
  }

  void *sha224Hmac(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength)
  {  
    return createBearsslHmac(&br_sha224_vtable, data, dataLength, hashKey, hashKeyLength, resultArray, outputLength);
  }
  
  String sha224Hmac(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength)
  {
    return createBearsslHmac(&br_sha224_vtable, SHA224_NATURAL_LENGTH, message, hashKey, hashKeyLength, hmacLength);
  }

  void *sha224HmacCT(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength)
  {
    return createBearsslHmacCT(&br_sha224_vtable, data, dataLength, hashKey, hashKeyLength, resultArray, outputLength);
  }
  
  String sha224HmacCT(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength)
  {
    return createBearsslHmacCT(&br_sha224_vtable, SHA224_NATURAL_LENGTH, message, hashKey, hashKeyLength, hmacLength);
  }
  

  // #################### SHA-256 ####################

  // resultArray must have size SHA256_NATURAL_LENGTH or greater
  void *sha256Hash(const void *data, const size_t dataLength, void *resultArray)
  {    
    br_sha256_context context;
    br_sha256_init(&context);
    br_sha256_update(&context, data, dataLength);
    br_sha256_out(&context, resultArray);
    return resultArray;
  }
  
  String sha256Hash(const String &message)
  {
    uint8_t hash[SHA256_NATURAL_LENGTH];
    sha256Hash(message.c_str(), message.length(), hash);
    return TypeCast::uint8ArrayToHexString(hash, SHA256_NATURAL_LENGTH);
  }

  void *sha256Hmac(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength)
  {  
    return createBearsslHmac(&br_sha256_vtable, data, dataLength, hashKey, hashKeyLength, resultArray, outputLength);
  }
  
  String sha256Hmac(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength)
  {
    return createBearsslHmac(&br_sha256_vtable, SHA256_NATURAL_LENGTH, message, hashKey, hashKeyLength, hmacLength);
  }

  void *sha256HmacCT(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength)
  {
    return createBearsslHmacCT(&br_sha256_vtable, data, dataLength, hashKey, hashKeyLength, resultArray, outputLength);
  }
  
  String sha256HmacCT(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength)
  {
    return createBearsslHmacCT(&br_sha256_vtable, SHA256_NATURAL_LENGTH, message, hashKey, hashKeyLength, hmacLength);
  }


  // #################### SHA-384 ####################

  // resultArray must have size SHA384_NATURAL_LENGTH or greater
  void *sha384Hash(const void *data, const size_t dataLength, void *resultArray)
  {    
    br_sha384_context context;
    br_sha384_init(&context);
    br_sha384_update(&context, data, dataLength);
    br_sha384_out(&context, resultArray);
    return resultArray;
  }
  
  String sha384Hash(const String &message)
  {
    uint8_t hash[SHA384_NATURAL_LENGTH];
    sha384Hash(message.c_str(), message.length(), hash);
    return TypeCast::uint8ArrayToHexString(hash, SHA384_NATURAL_LENGTH);
  }

  void *sha384Hmac(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength)
  {  
    return createBearsslHmac(&br_sha384_vtable, data, dataLength, hashKey, hashKeyLength, resultArray, outputLength);
  }
  
  String sha384Hmac(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength)
  {
    return createBearsslHmac(&br_sha384_vtable, SHA384_NATURAL_LENGTH, message, hashKey, hashKeyLength, hmacLength);
  }

  void *sha384HmacCT(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength)
  {
    return createBearsslHmacCT(&br_sha384_vtable, data, dataLength, hashKey, hashKeyLength, resultArray, outputLength);
  }
  
  String sha384HmacCT(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength)
  {
    return createBearsslHmacCT(&br_sha384_vtable, SHA384_NATURAL_LENGTH, message, hashKey, hashKeyLength, hmacLength);
  }


  // #################### SHA-512 ####################

  // resultArray must have size SHA512_NATURAL_LENGTH or greater
  void *sha512Hash(const void *data, const size_t dataLength, void *resultArray)
  {    
    br_sha512_context context;
    br_sha512_init(&context);
    br_sha512_update(&context, data, dataLength);
    br_sha512_out(&context, resultArray);
    return resultArray;
  }
  
  String sha512Hash(const String &message)
  {
    uint8_t hash[SHA512_NATURAL_LENGTH];
    sha512Hash(message.c_str(), message.length(), hash);
    return TypeCast::uint8ArrayToHexString(hash, SHA512_NATURAL_LENGTH);
  }

  void *sha512Hmac(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength)
  {  
    return createBearsslHmac(&br_sha512_vtable, data, dataLength, hashKey, hashKeyLength, resultArray, outputLength);
  }
  
  String sha512Hmac(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength)
  {
    return createBearsslHmac(&br_sha512_vtable, SHA512_NATURAL_LENGTH, message, hashKey, hashKeyLength, hmacLength);
  }

  void *sha512HmacCT(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength)
  {
    return createBearsslHmacCT(&br_sha512_vtable, data, dataLength, hashKey, hashKeyLength, resultArray, outputLength);
  }
  
  String sha512HmacCT(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength)
  {
    return createBearsslHmacCT(&br_sha512_vtable, SHA512_NATURAL_LENGTH, message, hashKey, hashKeyLength, hmacLength);
  }
  

  // #################### MD5+SHA-1 ####################

  // resultArray must have size MD5SHA1_NATURAL_LENGTH or greater
  void *md5sha1Hash(const void *data, const size_t dataLength, void *resultArray)
  {    
    br_md5sha1_context context;
    br_md5sha1_init(&context);
    br_md5sha1_update(&context, data, dataLength);
    br_md5sha1_out(&context, resultArray);
    return resultArray;
  }
  
  String md5sha1Hash(const String &message)
  {
    uint8_t hash[MD5SHA1_NATURAL_LENGTH];
    md5sha1Hash(message.c_str(), message.length(), hash);
    return TypeCast::uint8ArrayToHexString(hash, MD5SHA1_NATURAL_LENGTH);
  }


  // #################### HKDF ####################

  void hkdfInit(const void *keyMaterial, const size_t keyMaterialLength, const void *salt, const size_t saltLength)
  {
    // Comments mainly from https://www.bearssl.org/apidoc/bearssl__kdf_8h.html
    
    br_hkdf_context context;

    // Initialize an HKDF context, with a hash function, and the salt. This starts the HKDF-Extract process.
    br_hkdf_init(&context, &br_sha256_vtable, salt, saltLength);

    // Inject more input bytes. This function may be called repeatedly if the input data is provided by chunks, after br_hkdf_init() but before br_hkdf_flip().
    br_hkdf_inject(&context, keyMaterial, keyMaterialLength);

    // End the HKDF-Extract process, and start the HKDF-Expand process.
    br_hkdf_flip(&context);

    _storedHkdfContext = context;
    _hkdfContextStored = true;
  }
  
  size_t hkdfProduce(void *resultArray, const size_t outputLength, const void *info, const size_t infoLength)
  {
    // Comments mainly from https://www.bearssl.org/apidoc/bearssl__kdf_8h.html
    
    if(!_hkdfContextStored) // hkdfInit has not yet been executed
      return 0;
    
    // HKDF output production (HKDF-Expand).
    // Produces more output bytes from the current state. This function may be called several times, but only after br_hkdf_flip().
    // Returned value is the number of actually produced bytes. The total output length is limited to 255 times the output length of the underlying hash function.
    return br_hkdf_produce(&_storedHkdfContext, info, infoLength, resultArray, outputLength);
  }


  // #################### Authenticated Encryption with Associated Data (AEAD) ####################

  
  // #################### ChaCha20+Poly1305 AEAD ####################

  void chacha20Poly1305Kernel(const int encrypt, void *data, const size_t dataLength, const void *key, const void *keySalt, const size_t keySaltLength,
                              const void *nonce, void *tag, const void *aad, const size_t aadLength)
  {
    if(keySalt == nullptr)
    {
      br_poly1305_ctmul32_run(key, nonce, data, dataLength, aad, aadLength, tag, br_chacha20_ct_run, encrypt);
    }
    else
    {
      hkdfInit(key, ENCRYPTION_KEY_LENGTH, keySalt, keySaltLength);
      uint8_t derivedEncryptionKey[ENCRYPTION_KEY_LENGTH] {0};
      hkdfProduce(derivedEncryptionKey, ENCRYPTION_KEY_LENGTH);
      br_poly1305_ctmul32_run(derivedEncryptionKey, nonce, data, dataLength, aad, aadLength, tag, br_chacha20_ct_run, encrypt);
    }  
  }

  void chacha20Poly1305Encrypt(void *data, const size_t dataLength, const void *key, const void *keySalt, const size_t keySaltLength,
                               void *resultingNonce, void *resultingTag, const void *aad, const size_t aadLength)
  {
    uint8_t *nonce = (uint8_t *)resultingNonce;
    getNonceGenerator()(nonce, 12);
    
    chacha20Poly1305Kernel(1, data, dataLength, key, keySalt, keySaltLength, nonce, resultingTag, aad, aadLength);            
  }
  
  bool chacha20Poly1305Decrypt(void *data, const size_t dataLength, const void *key, const void *keySalt, const size_t keySaltLength,
                               const void *encryptionNonce, const void *encryptionTag, const void *aad, const size_t aadLength)
  {
    const uint8_t *oldTag = (const uint8_t *)encryptionTag;
    uint8_t newTag[16] {0};

    chacha20Poly1305Kernel(0, data, dataLength, key, keySalt, keySaltLength, encryptionNonce, newTag, aad, aadLength);

    for(uint32_t i = 0; i < sizeof newTag; ++i)
    {
      if(newTag[i] != oldTag[i])
        return false;
    }

    return true;
  }
}
