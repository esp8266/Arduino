/*
 * TypeConversionFunctions
 * Copyright (C) 2018-2019 Anders Löfgren
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

#include "TypeConversionFunctions.h"

namespace
{
  constexpr char chars[36] PROGMEM = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
  constexpr uint8_t charValues[75] PROGMEM {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, // 0 to 9
                                    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 0, 0, 0, 0, 0, 0, // Upper case letters
                                    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35}; // Lower case letters
}

namespace MeshTypeConversionFunctions 
{
  String uint64ToString(uint64_t number, const byte base)
  {
    assert(2 <= base && base <= 36);
    
    String result;
    
    if(base == 16)
    {
      do {
        result += (char)pgm_read_byte(chars + number % base);
        number >>= 4; // We could write number /= 16; and the compiler would optimize it to a shift, but the explicit shift notation makes it clearer where the speed-up comes from.
      } while ( number );
    }
    else
    {
      do {
        result += (char)pgm_read_byte(chars + number % base);
        number /= base;
      } while ( number );
    }
    
    std::reverse( result.begin(), result.end() );
    
    return result;
  }
  
  uint64_t stringToUint64(const String &string, const byte base)
  {
    assert(2 <= base && base <= 36);
    
    uint64_t result = 0;
  
    if(base == 16)
    {
      for(uint32_t i = 0; i < string.length(); ++i)
      {
        result <<= 4; // We could write result *= 16; and the compiler would optimize it to a shift, but the explicit shift notation makes it clearer where the speed-up comes from.
        result += pgm_read_byte(charValues + string.charAt(i) - '0');
      }
    }
    else
    {
      for(uint32_t i = 0; i < string.length(); ++i)
      {
        result *= base;
        result += pgm_read_byte(charValues + string.charAt(i) - '0');
      }
    }
    
    return result;
  }
  
  String uint8ArrayToHexString(const uint8_t *uint8Array, const uint32_t arrayLength)
  {
    String hexString;
    if(!hexString.reserve(2*arrayLength))  // Each uint8_t will become two characters (00 to FF)
      return emptyString;
    
    for(uint32_t i = 0; i < arrayLength; ++i)
    {
      hexString += (char)pgm_read_byte(chars + (uint8Array[i] >> 4));
      hexString += (char)pgm_read_byte(chars + uint8Array[i] % 16 );
    }
    
    return hexString;  
  }
  
  uint8_t *hexStringToUint8Array(const String &hexString, uint8_t *uint8Array, const uint32_t arrayLength)
  {
    assert(hexString.length() >= arrayLength*2); // Each array element can hold two hexString characters
    
    for(uint32_t i = 0; i < arrayLength; ++i)
    {
      uint8Array[i] = (pgm_read_byte(charValues + hexString.charAt(i*2) - '0') << 4) + pgm_read_byte(charValues + hexString.charAt(i*2 + 1) - '0'); 
    }
    
    return uint8Array;
  }
  
  String uint8ArrayToMultiString(uint8_t *uint8Array, const uint32_t arrayLength)
  {
    String multiString;
    if(!multiString.reserve(arrayLength))
      return emptyString;
  
    // Ensure we have a NULL terminated character array so the String() constructor knows where to stop.
    char finalChar = uint8Array[arrayLength - 1];
    uint8Array[arrayLength - 1] = 0;
  
    multiString += (char *)(uint8Array);
    while(multiString.length() < arrayLength - 1)
    {
      multiString += (char)0; // String construction only stops for null values, so we need to add those manually.
      multiString += (char *)(uint8Array + multiString.length());
    }
  
    multiString += finalChar;
    uint8Array[arrayLength - 1] = finalChar;
  
    return multiString;
  }
  
  String bufferedUint8ArrayToMultiString(const uint8_t *uint8Array, const uint32_t arrayLength)
  {
    String multiString;
    if(!multiString.reserve(arrayLength))
      return emptyString;
    
    // Ensure we have a NULL terminated character array so the String() constructor knows where to stop.
    uint8_t bufferedData[arrayLength + 1];
    std::copy_n(uint8Array, arrayLength, bufferedData);
    bufferedData[arrayLength] = 0;
    
    multiString += (char *)(bufferedData);
    while(multiString.length() < arrayLength)
    {
      multiString += (char)0; // String construction only stops for null values, so we need to add those manually.
      multiString += (char *)(bufferedData + multiString.length());
    }
  
    return multiString;
  }
  
  String macToString(const uint8_t *mac)
  {
    return MeshTypeConversionFunctions::uint8ArrayToHexString(mac, 6);
  }
  
  uint8_t *stringToMac(const String &macString, uint8_t *macArray)
  {
    return MeshTypeConversionFunctions::hexStringToUint8Array(macString, macArray, 6);
  }
  
  uint64_t macToUint64(const uint8_t *macArray)
  {
    uint64_t result = (uint64_t)macArray[0] << 40 | (uint64_t)macArray[1] << 32 | (uint64_t)macArray[2] << 24 | (uint64_t)macArray[3] << 16 | (uint64_t)macArray[4] << 8 | (uint64_t)macArray[5];
    return result;
  }
  
  uint8_t *uint64ToMac(const uint64_t macValue, uint8_t *macArray)
  {
    assert(macValue <= 0xFFFFFFFFFFFF); // Overflow will occur if value can't fit within 6 bytes
    
    macArray[5] = macValue;
    macArray[4] = macValue >> 8;
    macArray[3] = macValue >> 16;
    macArray[2] = macValue >> 24;
    macArray[1] = macValue >> 32;
    macArray[0] = macValue >> 40;
    
    return macArray;
  }
  
  uint8_t *uint64ToUint8Array(const uint64_t value, uint8_t *resultArray)
  {
    resultArray[7] = value;
    resultArray[6] = value >> 8;
    resultArray[5] = value >> 16;
    resultArray[4] = value >> 24;
    resultArray[3] = value >> 32;
    resultArray[2] = value >> 40;
    resultArray[1] = value >> 48;
    resultArray[0] = value >> 56;
  
    return resultArray;
  }
  
  uint64_t uint8ArrayToUint64(const uint8_t *inputArray)
  {
    uint64_t result = (uint64_t)inputArray[0] << 56 | (uint64_t)inputArray[1] << 48 | (uint64_t)inputArray[2] << 40 | (uint64_t)inputArray[3] << 32 
                      | (uint64_t)inputArray[4] << 24 | (uint64_t)inputArray[5] << 16 | (uint64_t)inputArray[6] << 8 | (uint64_t)inputArray[7];
                       
    return result;
  }
  
  /**
   * Helper function for meshBackendCast.
   */
  template <typename T>
  T attemptPointerCast(MeshBackendBase *meshBackendBaseInstance, MeshBackendType resultClassType)
  {
    if(meshBackendBaseInstance && meshBackendBaseInstance->getClassType() == resultClassType)
    {
      return static_cast<T>(meshBackendBaseInstance); 
    }
    
    return nullptr;
  }
  
  template <> 
  EspnowMeshBackend *meshBackendCast<EspnowMeshBackend *>(MeshBackendBase *meshBackendBaseInstance)
  {
    return attemptPointerCast<EspnowMeshBackend *>(meshBackendBaseInstance, MeshBackendType::ESP_NOW);
  }
  
  template <> 
  TcpIpMeshBackend *meshBackendCast<TcpIpMeshBackend *>(MeshBackendBase *meshBackendBaseInstance)
  {
    return attemptPointerCast<TcpIpMeshBackend *>(meshBackendBaseInstance, MeshBackendType::TCP_IP);
  }
}
