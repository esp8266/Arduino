/*
 * TransmissionResult
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

#include "ESP8266WiFiMesh.h"
#include <assert.h>

void ESP8266WiFiMesh::verboseModePrint(const String &stringToPrint, bool newline)
{
  if(_verboseMode)
  {
    if(newline)
      Serial.println(stringToPrint);
    else
      Serial.print(stringToPrint);
  }
}

/**
 * Note that using a base higher than 16 increases likelihood of randomly generating SSID strings containing controversial words. 
 * 
 * @param number The number to convert to a string with radix "base".
 * @param base The radix to convert "number" into. Must be between 2 and 36.
 * @returns A string of "number" encoded in radix "base".
 */
String ESP8266WiFiMesh::uint64ToString(uint64_t number, byte base)
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

/**
 * Note that using a base higher than 16 increases likelihood of randomly generating SSID strings containing controversial words. 
 * 
 * @param string The string to convert to uint64_t. String must use radix "base".
 * @param base The radix of "string". Must be between 2 and 36.
 * @returns A uint64_t of the string, using radix "base" during decoding.
 */
uint64_t ESP8266WiFiMesh::stringToUint64(const String &string, byte base)
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

/**
 * Calculate the current lwIP version number and store the numbers in the _lwipVersion array.
 * lwIP version can be changed in the "Tools" menu of Arduino IDE.
 */
void ESP8266WiFiMesh::storeLwipVersion()
{
  // ESP.getFullVersion() looks something like: 
  // SDK:2.2.1(cfd48f3)/Core:win-2.5.0-dev/lwIP:2.0.3(STABLE-2_0_3_RELEASE/glue:arduino-2.4.1-10-g0c0d8c2)/BearSSL:94e9704
  String fullVersion = ESP.getFullVersion();

  int i = fullVersion.indexOf("lwIP:") + 5;
  char currentChar = fullVersion.charAt(i);

  for(int versionPart = 0; versionPart < 3; versionPart++)
  {
    while(!isdigit(currentChar))
    {
      currentChar = fullVersion.charAt(++i);
    }
    while(isdigit(currentChar))
    {
      _lwipVersion[versionPart] = 10 * _lwipVersion[versionPart] + (currentChar - '0'); // Left shift and add digit value, in base 10.
      currentChar = fullVersion.charAt(++i);
    }
  }
}

/**
 * Check if the code is running on a version of lwIP that is at least minLwipVersion.
 */
bool ESP8266WiFiMesh::atLeastLwipVersion(const uint32_t minLwipVersion[3])
{ 
  for(int versionPart = 0; versionPart < 3; versionPart++)
  {
    if(_lwipVersion[versionPart] > minLwipVersion[versionPart])
      return true;
    else if(_lwipVersion[versionPart] < minLwipVersion[versionPart])
      return false;
  }

  return true;
}
