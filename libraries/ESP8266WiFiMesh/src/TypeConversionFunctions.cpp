/*
 * TypeConversionFunctions
 * Copyright (C) 2018 Anders Löfgren
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

String uint64ToString(uint64_t number, byte base)
{
  assert(2 <= base && base <= 36);
  
  String result = "";

  while(number > 0)
  {
    result = String((uint32_t)(number % base), base) + result;
    number /= base;
  }
  
  return (result == "" ? "0" : result);
}

uint64_t stringToUint64(const String &string, byte base)
{
  assert(2 <= base && base <= 36);
  
  uint64_t result = 0;

  char currentCharacter[1];
  for(uint32_t i = 0; i < string.length(); i++)
  {
    result *= base;
    currentCharacter[0] = string.charAt(i);
    result += strtoul(currentCharacter, NULL, base);
  }
  
  return result;
}

String uint8ArrayToHexString(const uint8_t *uint8Array, uint32_t arrayLength)
{
  char hexString[2*arrayLength + 1]; // Each uint8_t will become two characters (00 to FF) and we want a null terminated char array. 
  hexString[arrayLength + 1] = { 0 };
  for(uint32_t i = 0; i < arrayLength; i++)
  {
    sprintf(hexString + 2*i, "%02X", uint8Array[i]);
  }

  return String(hexString);
}

uint8_t *hexStringToUint8Array(const String &hexString, uint8_t *uint8Array, uint32_t arrayLength)
{
  assert(hexString.length() >= arrayLength*2); // Each array element can hold two hexString characters
  
  for(uint32_t i = 0; i < arrayLength; i++)
  {
    uint8Array[i] = strtoul(hexString.substring(i*2, (i+1)*2).c_str(), nullptr, 16);
  }
  
  return uint8Array;
}

String macToString(const uint8_t *mac)
{
  char macString[13] = { 0 };
  sprintf(macString, "%02X%02X%02X%02X%02X%02X", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
  return String(macString);
}

uint8_t *stringToMac(const String &macString, uint8_t *macArray)
{
  return hexStringToUint8Array(macString, macArray, 6);
}

uint64_t macToUint64(const uint8_t *macArray)
{
  uint64_t outcome = 0;
  for(int shiftingFortune = 40; shiftingFortune >= 0; shiftingFortune -= 8)
  {
    outcome |= ((uint64_t)macArray[5 - shiftingFortune/8] << shiftingFortune);
  }

  return outcome;
}

uint8_t *uint64ToMac(uint64_t macValue, uint8_t *macArray)
{
  assert(macValue <= 0xFFFFFFFFFFFF); // Overflow will occur if value can't fit within 6 bytes
  
  for(int shiftingFortune = 40; shiftingFortune >= 0; shiftingFortune -= 8)
  {
    macArray[5 - shiftingFortune/8] = macValue >> shiftingFortune & 0xFF;
  }
  return macArray;
} 

/**
 * Helper function for meshBackendCast.
 */
template <typename T>
T attemptPointerCast(MeshBackendBase *meshBackendBaseInstance, mesh_backend_t resultClassType)
{
  if(meshBackendBaseInstance && meshBackendBaseInstance->getClassType() == resultClassType)
  {
    return static_cast<T>(meshBackendBaseInstance); 
  }
  else
  {
    return nullptr;
  }
}

template <> 
EspnowMeshBackend *meshBackendCast<EspnowMeshBackend *>(MeshBackendBase *meshBackendBaseInstance)
{
  return attemptPointerCast<EspnowMeshBackend *>(meshBackendBaseInstance, MB_ESP_NOW);
}

template <> 
TcpIpMeshBackend *meshBackendCast<TcpIpMeshBackend *>(MeshBackendBase *meshBackendBaseInstance)
{
  return attemptPointerCast<TcpIpMeshBackend *>(meshBackendBaseInstance, MB_TCP_IP);
}
