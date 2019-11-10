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

#include "MeshCryptoInterface.h"

namespace MeshCryptoInterface 
{
  String createMeshHmac(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength)
  {
    return CryptoInterface::sha256Hmac(message, hashKey, hashKeyLength, hmacLength);
  }

  bool verifyMeshHmac(const String &message, const String &messageHmac, const uint8_t *hashKey, uint8_t hashKeyLength)
  {
    String generatedHmac = createMeshHmac(message, hashKey, hashKeyLength, messageHmac.length()/2); // We know that each HMAC byte should become 2 String characters due to uint8ArrayToHexString.
    if(generatedHmac == messageHmac)
      return true;
    else
      return false;
  }
}
