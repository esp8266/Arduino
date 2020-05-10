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
#include <initializer_list>

namespace JsonTranslator 
{
  constexpr char jsonConnectionState[] PROGMEM = "connectionState";
  constexpr char jsonMeshState[] PROGMEM = "meshState";
  constexpr char jsonPassword[] PROGMEM = "password";
  constexpr char jsonOwnSessionKey[] PROGMEM = "ownSK";
  constexpr char jsonPeerSessionKey[] PROGMEM = "peerSK";
  constexpr char jsonPeerStaMac[] PROGMEM = "peerStaMac";
  constexpr char jsonPeerApMac[] PROGMEM = "peerApMac";
  constexpr char jsonDuration[] PROGMEM = "duration";
  constexpr char jsonNonce[] PROGMEM = "nonce";
  constexpr char jsonHmac[] PROGMEM = "hmac";
  constexpr char jsonDesync[] PROGMEM = "desync";
  constexpr char jsonUnsynchronizedMessageID[] PROGMEM = "unsyncMsgID";
  constexpr char jsonMeshMessageCount[] PROGMEM = "meshMsgCount";
  constexpr char jsonArguments[] PROGMEM = "arguments";

  
  /**
   * Provides the index within jsonString where the value of valueIdentifier starts.
   * Note that including " within a JSON string value will result in errors.
   *
   * @param jsonString The String to search within.
   * @param valueIdentifier The identifier to search for.
   * @param searchStartIndex Optional argument that makes it possible to decide at which index of jsonString the search starts. Search will begin at index 0 if not provided.
   *          
   * @return An int32_t containing the index within jsonString where the value of valueIdentifier starts, or a negative value if valueIdentifier was not found.
   */
  int32_t getStartIndex(const String &jsonString, const String &valueIdentifier, const int32_t searchStartIndex = 0);
  
  /**
   * Provides the index within jsonString where the JSON object or JSON string value ends, starting the search from searchStartIndex.
   * Note that including " within a JSON string value will result in errors.
   * 
   * The character at searchStartIndex must be either " (for a string) or { (for an object), otherwise the search fails.
   *
   * @param jsonString The String to search within.
   * @param searchStartIndex The index of jsonString where the search will start. The index position should contain either " or {.
   *          
   * @return An int32_t containing the index within jsonString where the JSON string/object ends, or a negative value if no such character was found.
   */
  int32_t getEndIndex(const String &jsonString, const int32_t searchStartIndex);

  /*
   * Create a JSON String based on the identifiers and values given.
   * 
   * Assumes all values are either strings or JSON objects. A value is interpreted as a JSON object if it starts with {
   * Assumes all identifiers are strings.
   * 
   * @param identifiersAndValues Any even number of String arguments. It is assumed that the identifiers and values are given in an alternating manner, as in encode({Identifier1, Value1, Identifier2, Value2, ...})
   */
  String encode(std::initializer_list<String> identifiersAndValues);

  /*
   * Create a JSON String based on the identifiers and values given.
   * 
   * Does not make any assumptions regarding value types. " must be added manually around string values.
   * Useful for example if your JSON values can contain starting { characters, since the regular encode() will then interpret them as JSON objects.
   * Assumes all identifiers are strings.
   * 
   * @param identifiersAndValues Any even number of String arguments. It is assumed that the identifiers and values are given in an alternating manner, as in encodeLiterally({Identifier1, Value1, Identifier2, Value2, ...})
   */
  String encodeLiterally(std::initializer_list<String> identifiersAndValues);
  
  /*
   * Get a value from a JSON String.
   * Assumes all values are either JSON strings ( starting with " ) or JSON objects ( starting with { ).
   * 
   * Note that including " within a JSON string value will result in errors.
   * Escape characters are not supported at this moment, since we do not want string length modification to occur during ESP-NOW protocol transmissions.
   * 
   * @param jsonString The String to search within.
   * @param valueIdentifier The identifier to search for.
   * @param value The String variable to put the result in.
   * 
   * @return True if a value was found. False otherwise. The value argument is not modified if false is returned.
   */
  bool decode(const String &jsonString, const String &valueIdentifier, String &value);

  /*
   * Get a value from a JSON String.
   * Assumes all values are stored as strings in standard C-format (i.e. decimal by default).
   * 
   * Note that including " within a JSON string value will result in errors.
   * Escape characters are not supported at this moment, since we do not want string length modification to occur during ESP-NOW protocol transmissions.
   * 
   * @param jsonString The String to search within.
   * @param valueIdentifier The identifier to search for.
   * @param value The uint32_t variable to put the result in.
   * 
   * @return True if a value was found. False otherwise. The value argument is not modified if false is returned.
   */
  bool decode(const String &jsonString, const String &valueIdentifier, uint32_t &value);

  /*
   * Get a value from a JSON String.
   * Assumes all values are stored as strings encoded in the specified radix. Hexadecimal encoding is the default.
   * 
   * Note that including " within a JSON string value will result in errors.
   * Escape characters are not supported at this moment, since we do not want string length modification to occur during ESP-NOW protocol transmissions.
   * 
   * @param jsonString The String to search within.
   * @param valueIdentifier The identifier to search for.
   * @param value The uint64_t variable to put the result in.
   * @param radix The base to use when converting the string value to uint64_t. Must be between 2 and 36.
   * 
   * @return True if a value was found. False otherwise. The value argument is not modified if false is returned.
   */
  bool decodeRadix(const String &jsonString, const String &valueIdentifier, uint64_t &value, const uint8_t radix = 16);
  
  bool verifyEncryptionRequestHmac(const String &encryptionRequestHmacMessage, const uint8_t *requesterStaMac, const uint8_t *requesterApMac, const uint8_t *hashKey, const uint8_t hashKeyLength);
  
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
