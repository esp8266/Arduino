/*
    TypeConversion functionality
    Copyright (C) 2019 Anders Löfgren

    License (MIT license):

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/

#include <assert.h>
#include "TypeConversion.h"

namespace experimental
{
namespace TypeConversion
{
const char base36Chars[36] PROGMEM = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z'};
const uint8_t base36CharValues[75] PROGMEM {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 0, 0, 0, 0, 0, 0, // 0 to 9
          10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 0, 0, 0, 0, 0, 0, // Upper case letters
          10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35  // Lower case letters
};


String uint8ArrayToHexString(const uint8_t *uint8Array, const uint32_t arrayLength)
{
    String hexString;
    if (!hexString.reserve(2 * arrayLength))   // Each uint8_t will become two characters (00 to FF)
    {
        return emptyString;
    }

    for (uint32_t i = 0; i < arrayLength; ++i)
    {
        hexString += (char)pgm_read_byte(base36Chars + (uint8Array[i] >> 4));
        hexString += (char)pgm_read_byte(base36Chars + uint8Array[i] % 16);
    }

    return hexString;
}

uint8_t *hexStringToUint8Array(const String &hexString, uint8_t *uint8Array, const uint32_t arrayLength)
{
    assert(hexString.length() >= arrayLength * 2); // Each array element can hold two hexString characters

    for (uint32_t i = 0; i < arrayLength; ++i)
    {
        uint8Array[i] = (pgm_read_byte(base36CharValues + hexString.charAt(i * 2) - '0') << 4) + pgm_read_byte(base36CharValues + hexString.charAt(i * 2 + 1) - '0');
    }

    return uint8Array;
}

uint8_t *uint64ToUint8ArrayBE(const uint64_t value, uint8_t *resultArray)
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

uint64_t uint8ArrayToUint64BE(const uint8_t *inputArray)
{
    uint64_t result = (uint64_t)inputArray[0] << 56 | (uint64_t)inputArray[1] << 48 | (uint64_t)inputArray[2] << 40 | (uint64_t)inputArray[3] << 32
                      | (uint64_t)inputArray[4] << 24 | (uint64_t)inputArray[5] << 16 | (uint64_t)inputArray[6] << 8 | (uint64_t)inputArray[7];

    return result;
}
}
}
