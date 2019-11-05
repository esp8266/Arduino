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

#include <Arduino.h>

#ifndef __MESHCRYPTOINTERFACE_H__
#define __MESHCRYPTOINTERFACE_H__

namespace CryptoInterface 
{
  const uint8_t SHA256HMAC_NATURAL_LENGTH = 32;
  
  /**
   * Create a SHA256 HMAC from the message, using the provided hashKey. The result will be resultArrayLength bytes long and stored in resultArray.
   * Uses the BearSSL cryptographic library.
   * 
   * @param message The string from which to create the HMAC.
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param resultArray The array wherein to store the resulting HMAC.
   * @param resultArrayLength The length of resultArray in bytes. Determines the HMAC length. If resultArrayLength is greater than SHA256HMAC_NATURAL_LENGTH, 
   *                          the first (lowest index) SHA256HMAC_NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
   * 
   * @return A pointer to resultArray.
   */
  uint8_t *createBearsslHmac(const String &message, const uint8_t *hashKey, uint8_t hashKeyLength, uint8_t *resultArray, size_t resultArrayLength);
  
  /**
   * Create a SHA256 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
   * Uses the BearSSL cryptographic library.
   * 
   * @param message The string from which to create the HMAC.
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to 32. Defaults to SHA256HMAC_NATURAL_LENGTH.
   * 
   * @return A String with the generated HMAC in HEX format.
   */
  String createBearsslHmac(const String &message, const uint8_t *hashKey, uint8_t hashKeyLength, size_t hmacLength = SHA256HMAC_NATURAL_LENGTH);
  
  /**
   * Create a SHA256 HMAC from the message, using the provided hashKey. The result will be resultArrayLength bytes long and stored in resultArray.
   * Uses the BearSSL cryptographic library.
   * 
   * Constant-time version of createBearsslHmac(). More constant-time info here: https://www.bearssl.org/constanttime.html
   * For small messages, it takes substantially longer time to complete than a normal HMAC (5 ms vs 2 ms in a quick benchmark, 
   * determined by the difference between min and max allowed message length), and it also sets a maximum length that messages can be (set to 1000 bytes here).
   * Making the fixed max length variable would defeat the whole purpose of using constant-time, and not making it variable would create the wrong HMAC if message size exceeds the maximum.
   * 
   * Also, HMAC is already partially constant-time. Quoting the link above: 
   * "Hash functions implemented by BearSSL (MD5, SHA-1, SHA-224, SHA-256, SHA-384 and SHA-512) consist in bitwise logical operations and additions on 32-bit or 64-bit words, 
   * naturally yielding constant-time operations. HMAC is naturally as constant-time as the underlying hash function. The size of the MACed data, and the size of the key, 
   * may leak, though; only the contents are protected."
   * 
   * Thus the non constant-time version is used within the mesh framework instead. 
   * 
   * @param message The string from which to create the HMAC. Min size 0 bytes. Max size 1000 bytes.
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param resultArray The array wherein to store the resulting HMAC.
   * @param resultArrayLength The length of resultArray in bytes. Determines the HMAC length. If resultArrayLength is greater than SHA256HMAC_NATURAL_LENGTH, 
   *                          the first (lowest index) SHA256HMAC_NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
   * 
   * @return A pointer to resultArray.
   */
  uint8_t *createBearsslHmacCT(const String &message, const uint8_t *hashKey, uint8_t hashKeyLength, uint8_t *resultArray, size_t resultArrayLength);
  
  /**
   * Create a SHA256 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
   * Uses the BearSSL cryptographic library.
   * 
   * Constant-time version of createBearsslHmac(). More constant-time info here: https://www.bearssl.org/constanttime.html
   * Not used within the mesh framework for reasons outlined in the uint8_t *createBearsslHmacCT() description.
   * 
   * @param message The string from which to create the HMAC. Min size 0 bytes. Max size 1000 bytes.
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to 32. Defaults to SHA256HMAC_NATURAL_LENGTH.
   * 
   * @return A String with the generated HMAC in HEX format.
   */
  String createBearsslHmacCT(const String &message, const uint8_t *hashKey, uint8_t hashKeyLength, size_t hmacLength = SHA256HMAC_NATURAL_LENGTH);

  /**
   * Verify a SHA256 HMAC which was created from the message using the provided hashKey.
   * Uses the BearSSL cryptographic library.
   * 
   * @param message The string from which the HMAC was created.
   * @param messageHmac A string with the generated HMAC in HEX format. Valid messageHmac.length() is 2 to 64.
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * 
   * @return True if the HMAC is correct. False otherwise.
   */
  bool verifyBearsslHmac(const String &message, const String &messageHmac, const uint8_t *hashKey, uint8_t hashKeyLength);
}

#endif
