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

#ifndef __ESPNOWJSONTRANSLATOR_H__
#define __ESPNOWJSONTRANSLATOR_H__

#include <WString.h>

namespace JsonTranslator 
{
  const String jsonConnectionState = "{\"connectionState\":{";
  const String jsonPassword = "\"password\":";
  const String jsonOwnSessionKey = "\"ownSK\":";
  const String jsonPeerSessionKey = "\"peerSK\":";
  const String jsonPeerStaMac = "\"peerStaMac\":";
  const String jsonPeerApMac = "\"peerApMac\":";
  const String jsonDuration = "\"duration\":";
  const String jsonNonce = "\"nonce\":";
  const String jsonHmac = "\"hmac\":";
  const String jsonDesync = "\"desync\":";
  const String jsonUnsynchronizedMessageID = "\"unsyncMsgID\":";
  const String jsonMeshMessageCount = "\"meshMsgCount\":";
  
  String createJsonPair(const String &valueIdentifier, const String &value);
  String createJsonEndPair(const String &valueIdentifier, const String &value);
  
  String createEncryptedConnectionInfo(const String &infoHeader, const String &requestNonce, const String &authenticationPassword, uint64_t ownSessionKey, uint64_t peerSessionKey);
  String createEncryptionRequestIntro(const String &requestHeader, uint32_t duration = 0);
  String createEncryptionRequestEnding(const String &requestNonce);
  String createEncryptionRequestHmacMessage(const String &requestHeader, const String &requestNonce, const uint8_t *hashKey, uint8_t hashKeyLength, uint32_t duration = 0);

  bool verifyEncryptionRequestHmac(const String &encryptionRequestHmacMessage, const uint8_t *requesterStaMac, const uint8_t *requesterApMac, const uint8_t *hashKey, uint8_t hashKeyLength);

  /**
   * Provides the index within jsonString where the value of valueIdentifier starts.
   *
   * @param jsonString The String to search within.
   * @param valueIdentifier The identifier to search for.
   * @param searchStartIndex Optional argument that makes it possible to decide at which index of jsonString the search starts. Search will begin at index 0 if not provided.
   *          
   * @return An int32_t containing the index within jsonString where the value of valueIdentifier starts, or a negative value if valueIdentifier was not found.
   */
  int32_t getStartIndex(const String &jsonString, const String &valueIdentifier, int32_t searchStartIndex = 0);
  
  /**
   * Provides the index within jsonString where the next JSON termination character (',' or '}') is found, starting from searchStartIndex.
   *
   * @param jsonString The String to search within.
   * @param searchStartIndex The index of jsonString where the search will start.
   *          
   * @return An int32_t containing the index within jsonString where the next JSON termination character is found, or a negative value if no such character was found.
   */
  int32_t getEndIndex(const String &jsonString, int32_t searchStartIndex);
  
  bool getConnectionState(const String &jsonString, String &result);
  /**
   * Stores the value of the password field within jsonString into the result variable. 
   * No changes to the result variable are made if jsonString does not contain a password.
   *
   * @param jsonString The String to search within.
   * @param result The String where the value should be stored.
   * 
   * @return True if a value was found. False otherwise.
   */
  bool getPassword(const String &jsonString, String &result);
  bool getOwnSessionKey(const String &jsonString, uint64_t &result);
  bool getPeerSessionKey(const String &jsonString, uint64_t &result);
  
  /**
   * Stores the value of the peerStaMac field within jsonString into the resultArray. 
   * No changes to the resultArray are made if jsonString does not contain a peerStaMac.
   *
   * @param jsonString The String to search within.
   * @param resultArray The uint8_t array where the value should be stored. Must be at least 6 bytes.
   * 
   * @return True if a value was found. False otherwise.
   */
  bool getPeerStaMac(const String &jsonString, uint8_t *resultArray);
  bool getPeerApMac(const String &jsonString, uint8_t *resultArray);
  bool getDuration(const String &jsonString, uint32_t &result);
  bool getNonce(const String &jsonString, String &result);
  bool getHmac(const String &jsonString, String &result);
  bool getDesync(const String &jsonString, bool &result);
  bool getUnsynchronizedMessageID(const String &jsonString, uint32_t &result);
  bool getMeshMessageCount(const String &jsonString, uint16_t &result);
}

#endif
