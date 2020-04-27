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

#ifndef __ESP8266_TYPECONVERSION_H__
#define __ESP8266_TYPECONVERSION_H__

#include <Arduino.h>

namespace esp8266
{
namespace TypeConversion
{
extern const char base36Chars[36];

// Subtract '0' to normalize the char before lookup.
extern const uint8_t base36CharValues[75];

/**
    Convert the contents of a uint8_t array to a String in HEX format. The resulting String starts from index 0 of the array.
    All array elements will be padded with zeroes to ensure they are converted to 2 String characters each.

    @param uint8Array The array to make into a HEX String.
    @param arrayLength The size of uint8Array, in bytes.
    @return Normally a String containing the HEX representation of the uint8Array. An empty String if the memory allocation for the String failed.
*/
String uint8ArrayToHexString(const uint8_t *uint8Array, const uint32_t arrayLength);

/**
    Convert the contents of a String in HEX format to a uint8_t array. Index 0 of the array will represent the start of the String.
    There must be 2 String characters for each array element. Use padding with zeroes where required.

    @param hexString The HEX String to convert to a uint8_t array. Must contain at least 2*arrayLength characters.
    @param uint8Array The array to fill with the contents of the hexString.
    @param arrayLength The number of bytes to fill in uint8Array.
    @return A pointer to the uint8Array.
*/
uint8_t *hexStringToUint8Array(const String &hexString, uint8_t *uint8Array, const uint32_t arrayLength);

/**
    Takes a uint64_t value and stores the bits in a uint8_t array. Assumes index 0 of the array should contain MSB (big endian).

    @param value The uint64_t value to convert to a uint8_t array.
    @param resultArray A uint8_t array that will hold the result once the function returns. Should have a size of at least 8 bytes.
    @return The resultArray.
*/
uint8_t *uint64ToUint8ArrayBE(const uint64_t value, uint8_t *resultArray);

/**
    Takes a uint8_t array and converts the first 8 (lowest index) elements to a uint64_t. Assumes index 0 of the array contains MSB (big endian).

    @param inputArray A uint8_t array containing the data to convert to a uint64_t. Should have a size of at least 8 bytes.
    @return A uint64_t representation of the first 8 bytes of the array.
*/
uint64_t uint8ArrayToUint64BE(const uint8_t *inputArray);
}
}

#endif
