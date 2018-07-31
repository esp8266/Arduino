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

void ESP8266WiFiMesh::verboseModePrint(const String &string_to_print, bool newline)
{
  if(_verbose_mode)
  {
    if(newline)
      Serial.println(string_to_print);
    else
      Serial.print(string_to_print);
  }
}

/**
 * Note that using a base higher than 16 increases likelihood of randomly generating ssid strings containing controversial words. 
 * 
 * @param number The number to convert to a string with radix "base".
 * @param base The radix to convert "number" into. Must be between 2 and 36.
 * @returns A string of "number" encoded in radix "base".
 */
String ESP8266WiFiMesh::Uint64ToString(uint64_t number, byte base)
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
 * Note that using a base higher than 16 increases likelihood of randomly generating ssid strings containing controversial words. 
 * 
 * @param string The string to convert to uint64_t. String must use radix "base".
 * @param base The radix of "string". Must be between 2 and 36.
 * @returns A uint64_t of the string, using radix "base" during decoding.
 */
uint64_t ESP8266WiFiMesh::StringToUint64(const String &string, byte base)
{
  assert(2 <= base && base <= 36);
  
  uint64_t result = 0;

  char current_character[1];
  for(uint32_t i = 0; i < string.length(); i++)
  {
    result *= base;
    current_character[0] = string.charAt(i);
    result += strtoul(current_character, NULL, base);
  }
  
  return result;
}

/**
 * Calculate the current lwIP version number and store the numbers in the _lwip_version array.
 * lwIP version can be changed in the "Tools" menu of Arduino IDE.
 */
void ESP8266WiFiMesh::storeLwipVersion()
{
  // ESP.getFullVersion() looks something like: 
  // SDK:2.2.1(cfd48f3)/Core:win-2.5.0-dev/lwIP:2.0.3(STABLE-2_0_3_RELEASE/glue:arduino-2.4.1-10-g0c0d8c2)/BearSSL:94e9704
  String full_version = ESP.getFullVersion();

  int i = full_version.indexOf("lwIP:") + 5;
  char current_char = full_version.charAt(i);

  for(int version_part = 0; version_part < 3; version_part++)
  {
    while(!isdigit(current_char))
    {
      current_char = full_version.charAt(++i);
    }
    while(isdigit(current_char))
    {
      _lwip_version[version_part] = 10 * _lwip_version[version_part] + (current_char - '0'); // Left shift and add digit value, in base 10.
      current_char = full_version.charAt(++i);
    }
  }
}

/**
 * Check if the code is running on a version of lwIP that is at least min_lwip_version.
 */
bool ESP8266WiFiMesh::atLeastLwipVersion(const uint32_t min_lwip_version[3])
{ 
  for(int version_part = 0; version_part < 3; version_part++)
  {
    if(_lwip_version[version_part] > min_lwip_version[version_part])
      return true;
    else if(_lwip_version[version_part] < min_lwip_version[version_part])
      return false;
  }

  return true;
}
