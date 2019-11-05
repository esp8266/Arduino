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

namespace CryptoInterface 
{
  uint8_t *createBearsslHmac(const String &message, const uint8_t *hashKey, uint8_t hashKeyLength, uint8_t *resultArray, size_t resultArrayLength)
  {
    assert(1 <= resultArrayLength);
    
    // Comments mainly from https://www.bearssl.org/apidoc/bearssl__hmac_8h.html
    
    // HMAC is initialized with a key and an underlying hash function; it then fills a "key context". That context contains the processed key.
    // With the key context, a HMAC context can be initialized to process the input bytes and obtain the MAC output. The key context is not modified during that process, and can be reused.
  
    // hashType alternatives: &br_md5_vtable; &br_sha1_vtable; &br_sha224_vtable; &br_sha256_vtable; &br_sha384_vtable; &br_sha512_vtable;
    // Use SHA256 to create the hash.
    const br_hash_class *hashType = &br_sha256_vtable;
    
    br_hmac_key_context keyContext; // Holds general HMAC info
    br_hmac_context hmacContext; // Holds general HMAC info + specific info for the current message
  
    // HMAC key context initialisation.
    // Initialise the key context with the provided hash key, using the hash function identified by hashType. This supports arbitrary key lengths.
    br_hmac_key_init(&keyContext, hashType, hashKey, hashKeyLength);
  
    // Initialise a HMAC context with a key context. The key context is unmodified. 
    // Relevant data from the key context is immediately copied; the key context can thus be independently reused, modified or released without impacting this HMAC computation.
    // An explicit output length can be specified; the actual output length will be the minimum of that value and the natural HMAC output length. 
    // If resultArrayLength is 0, then the natural HMAC output length is selected. The "natural output length" is the output length of the underlying hash function.
    br_hmac_init(&hmacContext, &keyContext, resultArrayLength);
  
    // Provide the HMAC context with the data to create a HMAC from.
    // The provided message.length() bytes are injected as extra input in the HMAC computation incarnated by the hmacContext. 
    // It is acceptable that message.length() is zero, in which case data is ignored (and may be NULL) and this function does nothing.
    br_hmac_update(&hmacContext, message.c_str(), message.length());
  
    // Compute the HMAC output.
    // The destination buffer MUST be large enough to accommodate the result; its length is at most the "natural length" of HMAC (i.e. the output length of the underlying hash function). 
    // The context is NOT modified; further bytes may be processed. Thus, "partial HMAC" values can be efficiently obtained.
    // Optionally the constant-time version br_hmac_outCT() can be used. More info here: https://www.bearssl.org/constanttime.html .
    br_hmac_out(&hmacContext, resultArray); // returns size_t outputLength
  
    return resultArray;
  }
  
  String createBearsslHmac(const String &message, const uint8_t *hashKey, uint8_t hashKeyLength, size_t hmacLength)
  {
    assert(1 <= hmacLength && hmacLength <= SHA256HMAC_NATURAL_LENGTH);
    byte hmac[hmacLength];
    createBearsslHmac(message, hashKey, hashKeyLength, hmac, hmacLength);
    return uint8ArrayToHexString(hmac, hmacLength);
  }
  
  uint8_t *createBearsslHmacCT(const String &message, const uint8_t *hashKey, uint8_t hashKeyLength, uint8_t *resultArray, size_t resultArrayLength)
  {
    assert(1 <= resultArrayLength);
    
    // Comments mainly from https://www.bearssl.org/apidoc/bearssl__hmac_8h.html
    
    // HMAC is initialized with a key and an underlying hash function; it then fills a "key context". That context contains the processed key.
    // With the key context, a HMAC context can be initialized to process the input bytes and obtain the MAC output. The key context is not modified during that process, and can be reused.
  
    // hashType alternatives: &br_md5_vtable; &br_sha1_vtable; &br_sha224_vtable; &br_sha256_vtable; &br_sha384_vtable; &br_sha512_vtable;
    // Use SHA256 to create the hash.
    const br_hash_class *hashType = &br_sha256_vtable;
    
    br_hmac_key_context keyContext; // Holds general HMAC info
    br_hmac_context hmacContext; // Holds general HMAC info + specific info for the current message
  
    // HMAC key context initialisation.
    // Initialise the key context with the provided hash key, using the hash function identified by hashType. This supports arbitrary key lengths.
    br_hmac_key_init(&keyContext, hashType, hashKey, hashKeyLength);
  
    // Initialise a HMAC context with a key context. The key context is unmodified. 
    // Relevant data from the key context is immediately copied; the key context can thus be independently reused, modified or released without impacting this HMAC computation.
    // An explicit output length can be specified; the actual output length will be the minimum of that value and the natural HMAC output length. 
    // If resultArrayLength is 0, then the natural HMAC output length is selected. The "natural output length" is the output length of the underlying hash function.
    br_hmac_init(&hmacContext, &keyContext, resultArrayLength);
  
    // Provide the HMAC context with the data to create a HMAC from.
    // The provided message.length() bytes are injected as extra input in the HMAC computation incarnated by the hmacContext. 
    // It is acceptable that message.length() is zero, in which case data is ignored (and may be NULL) and this function does nothing.
    // No need for br_hmac_update when using constant-time version it seems. If it is used, the data provided to br_hmac_outCT will just be appended.
    // br_hmac_update(&hmacContext, message.c_str(), message.length());
  
    // Compute the HMAC output. Assumes message is minimum 0 bytes and maximum 1000 bytes. 
    // As long as this is true, the correct HMAC output is calculated in constant-time. More constant-time info here: https://www.bearssl.org/constanttime.html
    // Some extra input bytes are processed, then the output is computed. 
    // The extra input consists in the message.length() bytes pointed to by message.c_str(). The message.length() parameter must lie between min_len and max_len (inclusive); 
    // max_len bytes are actually read from data (indicating each data byte can be read multiple times, if message.length() < max_len). 
    // Computing time (and memory access pattern) will not depend upon the data byte contents or the value of len.
    // The output is written in the resultArray buffer, that MUST be large enough to receive it.
    // The difference max_len - min_len MUST be less than 2^30 (i.e. about one gigabyte).
    // This function computes the output properly only if the underlying hash function uses MD padding (i.e. MD5, SHA-1, SHA-224, SHA-256, SHA-384 or SHA-512).
    // The provided context is NOT modified.
    size_t min_len = 0;
    size_t max_len = 1000;
    assert(min_len <= message.length() && message.length() <= max_len);
    br_hmac_outCT(&hmacContext, message.c_str(), message.length(), min_len, max_len, resultArray); // returns size_t outputLength
  
    return resultArray;
  }
  
  String createBearsslHmacCT(const String &message, const uint8_t *hashKey, uint8_t hashKeyLength, size_t hmacLength)
  {
    assert(1 <= hmacLength && hmacLength <= SHA256HMAC_NATURAL_LENGTH);
    byte hmac[hmacLength];
    createBearsslHmacCT(message, hashKey, hashKeyLength, hmac, hmacLength);
    return uint8ArrayToHexString(hmac, hmacLength);
  }

  bool verifyBearsslHmac(const String &message, const String &messageHmac, const uint8_t *hashKey, uint8_t hashKeyLength)
  {
    String generatedHmac = createBearsslHmac(message, hashKey, hashKeyLength, messageHmac.length()/2); // We know that each HMAC byte should become 2 String characters due to uint8ArrayToHexString.
    if(generatedHmac == messageHmac)
      return true;
    else
      return false;
  }
}
