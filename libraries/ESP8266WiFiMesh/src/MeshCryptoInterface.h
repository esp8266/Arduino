/*
 * Copyright (C) 2019 Anders Löfgren
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

#ifndef __MESHCRYPTOINTERFACE_H__
#define __MESHCRYPTOINTERFACE_H__

#include <Arduino.h>
#include "CryptoInterface.h"

namespace MeshCryptoInterface 
{ 
  /**
   * There is a constant-time HMAC version available. More constant-time info here: https://www.bearssl.org/constanttime.html
   * For small messages, it takes substantially longer time to complete than a normal HMAC (5 ms vs 2 ms in a quick benchmark, 
   * determined by the difference between min and max allowed message length), and it also sets a maximum length that messages can be (1024 bytes by default).
   * Making the fixed max length variable would defeat the whole purpose of using constant-time, and not making it variable would create the wrong HMAC if message size exceeds the maximum.
   * 
   * Also, HMAC is already partially constant-time. Quoting the link above: 
   * "Hash functions implemented by BearSSL (MD5, SHA-1, SHA-224, SHA-256, SHA-384 and SHA-512) consist in bitwise logical operations and additions on 32-bit or 64-bit words, 
   * naturally yielding constant-time operations. HMAC is naturally as constant-time as the underlying hash function. The size of the MACed data, and the size of the key, 
   * may leak, though; only the contents are protected."
   * 
   * Thus the non constant-time version is used within the mesh framework instead.
   */
  
  /**
   * Create a SHA256 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
   * 
   * @param message The string from which to create the HMAC.
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to 32. Defaults to CryptoInterface::SHA256_NATURAL_LENGTH.
   * 
   * @return A String with the generated HMAC in HEX format.
   */
  String createMeshHmac(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength = CryptoInterface::SHA256_NATURAL_LENGTH);
  
  /**
   * Verify a SHA256 HMAC which was created from the message using the provided hashKey.
   * 
   * @param message The string from which the HMAC was created.
   * @param messageHmac A string with the generated HMAC in HEX format. Valid messageHmac.length() is 2 to 64.
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * 
   * @return True if the HMAC is correct. False otherwise.
   */
  bool verifyMeshHmac(const String &message, const String &messageHmac, const uint8_t *hashKey, uint8_t hashKeyLength);
}

#endif
