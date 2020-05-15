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

#include "JsonTranslator.h"
#include "EspnowProtocolInterpreter.h"
#include "TypeConversionFunctions.h"

namespace
{
  namespace TypeCast = MeshTypeConversionFunctions;
  
  bool getMac(const String &jsonString, const String &valueIdentifier, uint8_t *resultArray)
  {
    String jsonValue;
    bool decoded = JsonTranslator::decode(jsonString, valueIdentifier, jsonValue);

    if(jsonValue.length() != 12)
      decoded = false; // Mac String is always 12 characters long
    
    if(decoded)
      TypeCast::stringToMac(jsonValue, resultArray);
  
    return decoded;
  }
}

namespace JsonTranslator
{
  int32_t getStartIndex(const String &jsonString, const String &valueIdentifier, const int32_t searchStartIndex)
  {
    int32_t startIndex = jsonString.indexOf(String('"') + valueIdentifier + F("\":"), searchStartIndex);
    if(startIndex < 0)
      return startIndex;
  
    startIndex += valueIdentifier.length() + 3; // Do not include valueIdentifier and associated characters
    return startIndex;
  }
  
  int32_t getEndIndex(const String &jsonString, const int32_t searchStartIndex)
  {
    int32_t endIndex = -1;
    
    if(jsonString[searchStartIndex] == '"')
    {
      endIndex = jsonString.indexOf('"', searchStartIndex + 1);
    }
    else if(jsonString[searchStartIndex] == '{')
    {
      uint32_t depth = 1;
      bool withinString = false;
      
      for(uint32_t index = searchStartIndex + 1; depth != 0 && index < jsonString.length(); ++index)
      {
        if(jsonString[index] == '"')
          withinString = !withinString;
        else if(!withinString)
        {
          if(jsonString[index] == '{')
            ++depth;
          else if(jsonString[index] == '}')
            --depth;
        }

        if(depth == 0)
        {
          assert(index < 0x80000000); // Must avoid int32_t overflow
          endIndex = index;
        }
      }
    }
  
    return endIndex;
  }
  
  String encode(std::initializer_list<String> identifiersAndValues)
  {
    assert(identifiersAndValues.size() % 2 == 0); // List must consist of identifer-value pairs.
    
    String result = String('{');

    bool isIdentifier = true;
    for(String element : identifiersAndValues)
    {
      bool isObject = !isIdentifier && element[0] == '{';
      if(isObject)
        result += element;
      else
        result += String('"') + element + String('"');
      
      if(isIdentifier)
        result += ':';
      else
        result += ',';

      isIdentifier = !isIdentifier;
    }

    result[result.length() - 1] = '}';

    return result;
  }

  String encodeLiterally(std::initializer_list<String> identifiersAndValues)
  {
    assert(identifiersAndValues.size() % 2 == 0); // List must consist of identifer-value pairs.
    
    String result = String('{');

    bool isIdentifier = true;
    for(String element : identifiersAndValues)
    {
      if(isIdentifier)
        result += String('"') + element + String('"') + ':';
      else
        result += element + ',';

      isIdentifier = !isIdentifier;
    }

    result[result.length() - 1] = '}';

    return result;
  }

  bool decode(const String &jsonString, const String &valueIdentifier, String &value)
  {
    int32_t startIndex = getStartIndex(jsonString, valueIdentifier);
    if(startIndex < 0)
      return false;
    
    int32_t endIndex = getEndIndex(jsonString, startIndex);
    if(endIndex < 0)
      return false;

    if(jsonString[startIndex] == '"')
      ++startIndex; // Should not include starting "
    else if(jsonString[startIndex] == '{')
      ++endIndex; // Should include ending }
    else
      assert(false && F("Illegal JSON starting character!"));
      
    value = jsonString.substring(startIndex, endIndex);
    return true;
  }

  bool decode(const String &jsonString, const String &valueIdentifier, uint32_t &value)
  {
    String jsonValue;
    bool decoded = decode(jsonString, valueIdentifier, jsonValue);
    
    if(decoded)
      value = strtoul(jsonValue.c_str(), nullptr, 0); // strtoul stops reading input when an invalid character is discovered.
  
    return decoded;
  }

  bool decodeRadix(const String &jsonString, const String &valueIdentifier, uint64_t &value, const uint8_t radix)
  {
    String jsonValue;
    bool decoded = decode(jsonString, valueIdentifier, jsonValue);
    
    if(decoded)
      value = TypeCast::stringToUint64(jsonValue, radix);
  
    return decoded;
  }

  bool getConnectionState(const String &jsonString, String &result)
  {
    return decode(jsonString, FPSTR(jsonConnectionState), result);
  }
  
  bool getPassword(const String &jsonString, String &result)
  {
    return decode(jsonString, FPSTR(jsonPassword), result);
  }
  
  bool getOwnSessionKey(const String &jsonString, uint64_t &result)
  {
    return decodeRadix(jsonString, FPSTR(jsonOwnSessionKey), result);
  }
  
  bool getPeerSessionKey(const String &jsonString, uint64_t &result)
  {
    return decodeRadix(jsonString, FPSTR(jsonPeerSessionKey), result);
  }
  
  bool getPeerStaMac(const String &jsonString, uint8_t *resultArray)
  {  
    return getMac(jsonString, FPSTR(jsonPeerStaMac), resultArray);
  }
  
  bool getPeerApMac(const String &jsonString, uint8_t *resultArray)
  {
    return getMac(jsonString, FPSTR(jsonPeerApMac), resultArray);
  }
  
  bool getDuration(const String &jsonString, uint32_t &result)
  {  
    return decode(jsonString, FPSTR(jsonDuration), result);
  }
  
  bool getNonce(const String &jsonString, String &result)
  {
    return decode(jsonString, FPSTR(jsonNonce), result);
  }

  bool getHmac(const String &jsonString, String &result)
  {
    return decode(jsonString, FPSTR(jsonHmac), result);
  }

  bool getDesync(const String &jsonString, bool &result)
  {  
    String jsonValue;
    bool decoded = decode(jsonString, FPSTR(jsonDesync), jsonValue);
    
    if(decoded)
      result = bool(strtoul(jsonValue.c_str(), nullptr, 0)); // strtoul stops reading input when an invalid character is discovered.
  
    return decoded;
  }

  bool getUnsynchronizedMessageID(const String &jsonString, uint32_t &result)
  {
    return decode(jsonString, FPSTR(jsonUnsynchronizedMessageID), result);
  }

  bool getMeshMessageCount(const String &jsonString, uint16_t &result)
  {  
    uint32_t longResult = 0;
    bool decoded = decode(jsonString, FPSTR(jsonMeshMessageCount), longResult);

    if(longResult > 65535) // Must fit within uint16_t
      decoded = false;

    if(decoded)
      result = longResult;
      
    return decoded;
  }
}
