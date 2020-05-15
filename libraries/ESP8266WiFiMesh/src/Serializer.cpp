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
 
#include "Serializer.h"
#include "JsonTranslator.h"
#include "TypeConversionFunctions.h"
#include "MeshCryptoInterface.h"
#include "EspnowProtocolInterpreter.h" 
#include <ESP8266WiFi.h>

namespace
{
  namespace TypeCast = MeshTypeConversionFunctions;

  String createJsonEndPair(const String &valueIdentifier, const String &value)
  {
    const String q = String('"');
    return q + valueIdentifier + q + ':' + q + value + F("\"}}");
  }
}

namespace Serializer
{
  /*
   * NOTE: The internal states may be changed in future updates, so the function signatures here are not guaranteed to be stable.
   */
  
  String serializeMeshState(const String &unsyncMsgID, const String &meshMsgCount)
  {
    using namespace JsonTranslator;

    // Returns: {"meshState":{"connectionState":{"unsyncMsgID":"123"},"meshMsgCount":"123"}}
    return encode({FPSTR(jsonMeshState), encode({FPSTR(jsonConnectionState), encode({FPSTR(jsonUnsynchronizedMessageID), unsyncMsgID}), FPSTR(jsonMeshMessageCount), meshMsgCount})});
  }

  String serializeUnencryptedConnection(const String &unsyncMsgID)
  {
    using namespace JsonTranslator;

    // Returns: {"connectionState":{"unsyncMsgID":"123"}}
    return encode({FPSTR(jsonConnectionState), encode({FPSTR(jsonUnsynchronizedMessageID), unsyncMsgID})});
  }

  String serializeEncryptedConnection(const String &duration, const String &desync, const String &ownSK, const String &peerSK, const String &peerStaMac, const String &peerApMac)
  {
    using namespace JsonTranslator;

    if(duration.isEmpty())
    {
      // Returns: {"connectionState":{"desync":"0","ownSK":"1A2","peerSK":"3B4","peerStaMac":"F2","peerApMac":"E3"}}
      return encode({FPSTR(jsonConnectionState), encode({FPSTR(jsonDesync), desync, FPSTR(jsonOwnSessionKey), ownSK, FPSTR(jsonPeerSessionKey), peerSK, 
                     FPSTR(jsonPeerStaMac), peerStaMac, FPSTR(jsonPeerApMac), peerApMac})});
    }
    
    // Returns: {"connectionState":{"duration":"123","desync":"0","ownSK":"1A2","peerSK":"3B4","peerStaMac":"F2","peerApMac":"E3"}}
    return encode({FPSTR(jsonConnectionState), encode({FPSTR(jsonDuration), duration, FPSTR(jsonDesync), desync, FPSTR(jsonOwnSessionKey), ownSK, FPSTR(jsonPeerSessionKey), peerSK, 
                   FPSTR(jsonPeerStaMac), peerStaMac, FPSTR(jsonPeerApMac), peerApMac})});
  }
    
  String createEncryptedConnectionInfo(const String &infoHeader, const String &requestNonce, const String &authenticationPassword, const uint64_t ownSessionKey, const uint64_t peerSessionKey)
  {
    using namespace JsonTranslator;

    const String q = String('"');

    // Returns: infoHeader{"arguments":{"nonce":"1F2","password":"abc","ownSK":"3B4","peerSK":"1A2"}}
    return 
    infoHeader + 
    encode({FPSTR(jsonArguments), 
    encodeLiterally({FPSTR(jsonNonce), q + requestNonce + q, 
                     FPSTR(jsonPassword), q + authenticationPassword + q, 
                     FPSTR(jsonOwnSessionKey), q + TypeCast::uint64ToString(peerSessionKey) + q,   // Exchanges session keys since it should be valid for the receiver.
                     FPSTR(jsonPeerSessionKey), q + TypeCast::uint64ToString(ownSessionKey) + q})});
  }
  
  String createEncryptionRequestHmacMessage(const String &requestHeader, const String &requestNonce, const uint8_t *hashKey, const uint8_t hashKeyLength, const uint32_t duration)
  {
    using namespace JsonTranslator;

    String mainMessage = requestHeader;

    if(requestHeader == FPSTR(EspnowProtocolInterpreter::temporaryEncryptionRequestHeader))
    {
      mainMessage += encode({FPSTR(jsonArguments), encode({FPSTR(jsonDuration), String(duration), FPSTR(jsonNonce), requestNonce})});
    }
    else
    {
      mainMessage += encode({FPSTR(jsonArguments), encode({FPSTR(jsonNonce), requestNonce})});
    }

    // We need to have an open JSON object so we can add the HMAC later.
    mainMessage.remove(mainMessage.length() - 2);
    mainMessage += ',';

    uint8_t staMac[6] {0};
    uint8_t apMac[6] {0};
    String requesterStaApMac = TypeCast::macToString(WiFi.macAddress(staMac)) + TypeCast::macToString(WiFi.softAPmacAddress(apMac));
    String hmac = MeshCryptoInterface::createMeshHmac(requesterStaApMac + mainMessage, hashKey, hashKeyLength);

    // Returns: requestHeader{"arguments":{"duration":"123","nonce":"1F2","hmac":"3B4"}}
    return mainMessage + createJsonEndPair(FPSTR(jsonHmac), hmac);
  }
}
