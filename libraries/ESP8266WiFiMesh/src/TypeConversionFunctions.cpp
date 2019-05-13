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
