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

#ifndef __TYPECONVERSIONFUNCTIONS_H__
#define __TYPECONVERSIONFUNCTIONS_H__

#include <Arduino.h>
#include <assert.h>

/**
 * Note that using a base higher than 16 increases likelihood of randomly generating SSID strings containing controversial words. 
 * 
 * @param number The number to convert to a string with radix "base".
 * @param base The radix to convert "number" into. Must be between 2 and 36.
 * @returns A string of "number" encoded in radix "base".
 */
String uint64ToString(uint64_t number, byte base = 16);

/**
 * Note that using a base higher than 16 increases likelihood of randomly generating SSID strings containing controversial words. 
 * 
 * @param string The string to convert to uint64_t. String must use radix "base".
 * @param base The radix of "string". Must be between 2 and 36.
 * @returns A uint64_t of the string, using radix "base" during decoding.
 */
uint64_t stringToUint64(const String &string, byte base = 16);

#endif
