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
#include "MeshBackendBase.h"
#include "TcpIpMeshBackend.h"
#include "EspnowMeshBackend.h"

/**
 * Note that using a base higher than 16 increases likelihood of randomly generating SSID strings containing controversial words. 
 * 
 * @param number The number to convert to a string with radix "base".
 * @param base The radix to convert "number" into. Must be between 2 and 36.
 * @return A string of "number" encoded in radix "base".
 */
String uint64ToString(uint64_t number, byte base = 16);

/**
 * Note that using a base higher than 16 increases likelihood of randomly generating SSID strings containing controversial words. 
 * 
 * @param string The string to convert to uint64_t. String must use radix "base".
 * @param base The radix of "string". Must be between 2 and 36.
 * @return A uint64_t of the string, using radix "base" during decoding.
 */
uint64_t stringToUint64(const String &string, byte base = 16);

// All array elements will be padded with zeroes to ensure they are converted to 2 string characters each.
String uint8ArrayToHexString(const uint8_t *uint8Array, uint32_t arrayLength);

// There must be 2 string characters for each array element. Use padding with zeroes where required.
uint8_t *hexStringToUint8Array(const String &hexString, uint8_t *uint8Array, uint32_t arrayLength);

/**
 * Takes a uint8_t array and converts the first 6 bytes to a hexadecimal string.
 * 
 * @param mac A uint8_t array with the mac address to convert to a string. Should be 6 bytes in total.
 * @return A hexadecimal string representation of the mac.
 */
String macToString(const uint8_t *mac);

/**
 * Takes a String and converts the first 12 characters to uint8_t numbers which are stored in the macArray from low to high index. Assumes hexadecimal number encoding.
 * 
 * @param macString A String which begins with the mac address to store in the array as a hexadecimal number.
 * @param macArray A uint8_t array that will hold the mac address once the function returns. Should have a size of at least 6 bytes.
 * @return The macArray.
 */
uint8_t *stringToMac(const String &macString, uint8_t *macArray);

/**
 * Takes a uint8_t array and converts the first 6 bytes to a uint64_t. Assumes index 0 of the array contains MSB.
 * 
 * @param macArray A uint8_t array with the mac address to convert to a uint64_t. Should be 6 bytes in total.
 * @return A uint64_t representation of the mac.
 */
uint64_t macToUint64(const uint8_t *macArray);

/**
 * Takes a uint64_t value and stores the bits of the first 6 bytes (LSB) in a uint8_t array. Assumes index 0 of the array should contain MSB. 
 * 
 * @param macValue The uint64_t value to convert to a mac array. Value must fit within 6 bytes.
 * @param macArray A uint8_t array that will hold the mac address once the function returns. Should have a size of at least 6 bytes.
 * @return The macArray.
 */
uint8_t *uint64ToMac(uint64_t macValue, uint8_t *macArray);

/**
 * Conversion function that can be used on MeshBackend classes instead of dynamic_cast since RTTI is disabled.
 * 
 * @param T The MeshBackend class pointer type to cast the meshBackendBaseInstance pointer into.
 * @param meshBackendBaseInstance The instance pointer to cast.
 * @return A pointer of type T to meshBackendBaseInstance if meshBackendBaseInstance is of type T. nullptr otherwise.
 */
template <typename T>
T meshBackendCast(MeshBackendBase *meshBackendBaseInstance)
{
  // The only valid template arguments are handled by the template specializations below, so ending up here is an error.
  static_assert(std::is_same<T, EspnowMeshBackend *>::value || std::is_same<T, TcpIpMeshBackend *>::value,
                "Error: Invalid MeshBackend class type. Make sure the template argument to meshBackendCast is supported!");
}

// These template specializations allow us to put the main template functionality in the .cpp file (which gives better encapsulation).
template <> 
EspnowMeshBackend *meshBackendCast<EspnowMeshBackend *>(MeshBackendBase *meshBackendBaseInstance);

template <> 
TcpIpMeshBackend *meshBackendCast<TcpIpMeshBackend *>(MeshBackendBase *meshBackendBaseInstance);

#endif
