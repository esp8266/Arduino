/*
 * Copyright (C) 2020 Anders Löfgren
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

#ifndef __ESP8266MESHSERIALIZER_H__
#define __ESP8266MESHSERIALIZER_H__

#include <WString.h>

namespace Serializer 
{
  /*
   * NOTE: The internal states may be changed in future updates, so the function signatures here are not guaranteed to be stable.
   */
   
  String serializeMeshState(const String &unsyncMsgID, const String &meshMsgCount);
  String serializeUnencryptedConnection(const String &unsyncMsgID);
  String serializeEncryptedConnection(const String &duration, const String &desync, const String &ownSK, const String &peerSK, const String &peerStaMac, const String &peerApMac);
  
  String createEncryptedConnectionInfo(const String &infoHeader, const String &requestNonce, const String &authenticationPassword, const uint64_t ownSessionKey, const uint64_t peerSessionKey);
  String createEncryptionRequestHmacMessage(const String &requestHeader, const String &requestNonce, const uint8_t *hashKey, const uint8_t hashKeyLength, const uint32_t duration = 0);
}

#endif
