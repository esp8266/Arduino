/*
 * BearSSL Copyright (c) 2016 Thomas Pornin <pornin@bolet.org>
 * Rest of this file Copyright (C) 2019 Anders Löfgren
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

#ifndef __ESP8266ARDUINOCRYPTOINTERFACE_H__
#define __ESP8266ARDUINOCRYPTOINTERFACE_H__

#include <Arduino.h>

namespace CryptoInterface 
{
  /**
   * Regarding constant-time (CT) HMAC:
   * 
   * Basically, constant-time algorithms makes it harder for attackers to learn things about your system based on the execution time of code. 
   * Good intro here: https://www.bearssl.org/constanttime.html
   * 
   * It should be noted that every HMAC is already partially constant-time. Quoting the link above: 
   * "Hash functions implemented by BearSSL (MD5, SHA-1, SHA-224, SHA-256, SHA-384 and SHA-512) consist in bitwise logical operations and additions on 32-bit or 64-bit words, 
   * naturally yielding constant-time operations. HMAC is naturally as constant-time as the underlying hash function. The size of the MACed data, and the size of the key, 
   * may leak, though; only the contents are protected."
   *  
   * For messages much smaller than ctMaxDataLength(), constant-time processing takes substantially longer time to complete than a normal HMAC,
   * determined by the size of (ctMaxDataLength() - ctMinDataLength()). 
   * Constant-time processing also sets limits on the data length.
   * 
   * Making the fixed data length limits variable will generally defeat the purpose of using constant-time.
   * Using data that exceeds the fixed data length limits will create the wrong HMAC.
   */

   
  constexpr uint8_t MD5_NATURAL_LENGTH = 16;
  constexpr uint8_t SHA1_NATURAL_LENGTH = 20;
  constexpr uint8_t SHA224_NATURAL_LENGTH = 28;
  constexpr uint8_t SHA256_NATURAL_LENGTH = 32;
  constexpr uint8_t SHA384_NATURAL_LENGTH = 48;
  constexpr uint8_t SHA512_NATURAL_LENGTH = 64;

  /**
   * MD5+SHA-1 is the concatenation of MD5 and SHA-1 computed over the same input; in the implementation, the internal data buffer is shared, 
   * thus making it more memory-efficient than separate MD5 and SHA-1. It can be useful in implementing SSL 3.0, TLS 1.0 and TLS 1.1.
   */
  constexpr uint8_t MD5SHA1_NATURAL_LENGTH = 36;

  constexpr uint32_t ctMaxDiff = 1073741823; // 2^30 - 1

  /**
   * This function allows for fine-tuning of the specifications for the constant time calculations. 
   * It should not be changed once a constant time function has been used at least once. 
   * Otherwise the constant time will not be constant for the used functions.
   * 
   * The difference ctMaxDataLength() - ctMinDataLength() MUST be less than 2^30 (i.e. about one gigabyte).
   */
  void setCtMinDataLength(const size_t ctMinDataLength);
  /**
   * 0 by default.
   */
  size_t ctMinDataLength();

  /**
   * This function allows for fine-tuning of the specifications for the constant time calculations. 
   * It should not be changed once a constant time function has been used at least once. 
   * Otherwise the constant time will not be constant for the used functions.
   * 
   * The difference ctMaxDataLength() - ctMinDataLength() MUST be less than 2^30 (i.e. about one gigabyte).
   */
  void setCtMaxDataLength(const size_t ctMaxDataLength);
  /**
   * 1024 by default.
   */
  size_t ctMaxDataLength();

  /**
   * Turn on or off warning Serial prints from the CryptoInterface functions.
   * 
   * @param warningsEnabled If true, warnings will be printed to Serial.
   */
  void setWarningsEnabled(bool warningsEnabled);
  bool warningsEnabled();
  

  // #################### MD5 ####################

  /**
   * Create a MD5 hash of the data. The result will be MD5_NATURAL_LENGTH bytes long and stored in resultArray.
   * Uses the BearSSL cryptographic library.
   * 
   * @param data The data array from which to create the hash.
   * @param dataLength The length of the data array in bytes.
   * @param resultArray The array wherein to store the resulting hash. MUST be be able to contain MD5_NATURAL_LENGTH bytes or more.
   * 
   * @return A pointer to resultArray.
   */
  void *md5Hash(const void *data, const size_t dataLength, void *resultArray);
  
  /**
   * Create a MD5 hash of the data. The result will be MD5_NATURAL_LENGTH bytes long and returned as a String in HEX format.
   * Uses the BearSSL cryptographic library.
   * 
   * @param message The string from which to create the hash.
   * 
   * @return A String with the generated hash in HEX format.
   */
  String md5Hash(const String &message);

  /**
   * Create a MD5 HMAC from the data, using the provided hashKey. The result will be resultArrayLength bytes long and stored in resultArray.
   * Uses the BearSSL cryptographic library.
   * 
   * @param data The data array from which to create the HMAC.
   * @param dataLength The length of the data array in bytes.
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param resultArray The array wherein to store the resulting HMAC.
   * @param resultArrayLength The length of resultArray in bytes. Determines the HMAC length. If resultArrayLength is greater than MD5_NATURAL_LENGTH, 
   *                          the first (lowest index) MD5_NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
   * 
   * @return A pointer to resultArray.
   */
  void *md5Hmac(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t resultArrayLength);

  /**
   * Create a MD5 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
   * Uses the BearSSL cryptographic library.
   * 
   * @param message The string from which to create the HMAC.
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to MD5_NATURAL_LENGTH.
   * 
   * @return A String with the generated HMAC in HEX format.
   */
  String md5Hmac(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);

  /**
   * Create a MD5 HMAC from the data, using the provided hashKey. The result will be resultArrayLength bytes long and stored in resultArray.
   * Constant-time version.
   * Uses the BearSSL cryptographic library.
   * 
   * @param data The data array from which to create the HMAC.
   * @param dataLength The length of the data array in bytes. Valid values are in the range [ctMinDataLength(), ctMaxDataLength()].
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param resultArray The array wherein to store the resulting HMAC.
   * @param resultArrayLength The length of resultArray in bytes. Determines the HMAC length. If resultArrayLength is greater than MD5_NATURAL_LENGTH, 
   *                          the first (lowest index) MD5_NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
   * 
   * @return A pointer to resultArray.
   */
  void *md5HmacCT(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t resultArrayLength);

  /**
   * Create a MD5 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
   * Constant-time version.
   * Uses the BearSSL cryptographic library.
   * 
   * @param message The string from which to create the HMAC. Must have a length in the range [ctMinDataLength(), ctMaxDataLength()].
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to MD5_NATURAL_LENGTH.
   * 
   * @return A String with the generated HMAC in HEX format.
   */
  String md5HmacCT(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);


  // #################### SHA-1 ####################

  /**
   * Create a SHA1 hash of the data. The result will be SHA1_NATURAL_LENGTH bytes long and stored in resultArray.
   * Uses the BearSSL cryptographic library.
   * 
   * @param data The data array from which to create the hash.
   * @param dataLength The length of the data array in bytes.
   * @param resultArray The array wherein to store the resulting hash. MUST be be able to contain SHA1_NATURAL_LENGTH bytes or more.
   * 
   * @return A pointer to resultArray.
   */
  void *sha1Hash(const void *data, const size_t dataLength, void *resultArray);
  
  /**
   * Create a SHA1 hash of the data. The result will be SHA1_NATURAL_LENGTH bytes long and returned as a String in HEX format.
   * Uses the BearSSL cryptographic library.
   * 
   * @param message The string from which to create the hash.
   * 
   * @return A String with the generated hash in HEX format.
   */
  String sha1Hash(const String &message);

  /**
   * Create a SHA1 HMAC from the data, using the provided hashKey. The result will be resultArrayLength bytes long and stored in resultArray.
   * Uses the BearSSL cryptographic library.
   * 
   * @param data The data array from which to create the HMAC.
   * @param dataLength The length of the data array in bytes.
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param resultArray The array wherein to store the resulting HMAC.
   * @param resultArrayLength The length of resultArray in bytes. Determines the HMAC length. If resultArrayLength is greater than SHA1_NATURAL_LENGTH, 
   *                          the first (lowest index) SHA1_NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
   * 
   * @return A pointer to resultArray.
   */
  void *sha1Hmac(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t resultArrayLength);

  /**
   * Create a SHA1 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
   * Uses the BearSSL cryptographic library.
   * 
   * @param message The string from which to create the HMAC.
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to SHA1_NATURAL_LENGTH.
   * 
   * @return A String with the generated HMAC in HEX format.
   */
  String sha1Hmac(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);

  /**
   * Create a SHA1 HMAC from the data, using the provided hashKey. The result will be resultArrayLength bytes long and stored in resultArray.
   * Constant-time version.
   * Uses the BearSSL cryptographic library.
   * 
   * @param data The data array from which to create the HMAC.
   * @param dataLength The length of the data array in bytes. Valid values are in the range [ctMinDataLength(), ctMaxDataLength()].
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param resultArray The array wherein to store the resulting HMAC.
   * @param resultArrayLength The length of resultArray in bytes. Determines the HMAC length. If resultArrayLength is greater than SHA1_NATURAL_LENGTH, 
   *                          the first (lowest index) SHA1_NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
   * 
   * @return A pointer to resultArray.
   */
  void *sha1HmacCT(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t resultArrayLength);

  /**
   * Create a SHA1 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
   * Constant-time version.
   * Uses the BearSSL cryptographic library.
   * 
   * @param message The string from which to create the HMAC. Must have a length in the range [ctMinDataLength(), ctMaxDataLength()].
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to SHA1_NATURAL_LENGTH.
   * 
   * @return A String with the generated HMAC in HEX format.
   */
  String sha1HmacCT(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);


  // #################### SHA-224 ####################

  /**
   * Create a SHA224 hash of the data. The result will be SHA224_NATURAL_LENGTH bytes long and stored in resultArray.
   * Uses the BearSSL cryptographic library.
   * 
   * @param data The data array from which to create the hash.
   * @param dataLength The length of the data array in bytes.
   * @param resultArray The array wherein to store the resulting hash. MUST be be able to contain SHA224_NATURAL_LENGTH bytes or more.
   * 
   * @return A pointer to resultArray.
   */
  void *sha224Hash(const void *data, const size_t dataLength, void *resultArray);
  
  /**
   * Create a SHA224 hash of the data. The result will be SHA224_NATURAL_LENGTH bytes long and returned as a String in HEX format.
   * Uses the BearSSL cryptographic library.
   * 
   * @param message The string from which to create the hash.
   * 
   * @return A String with the generated hash in HEX format.
   */
  String sha224Hash(const String &message);

  /**
   * Create a SHA224 HMAC from the data, using the provided hashKey. The result will be resultArrayLength bytes long and stored in resultArray.
   * Uses the BearSSL cryptographic library.
   * 
   * @param data The data array from which to create the HMAC.
   * @param dataLength The length of the data array in bytes.
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param resultArray The array wherein to store the resulting HMAC.
   * @param resultArrayLength The length of resultArray in bytes. Determines the HMAC length. If resultArrayLength is greater than SHA224_NATURAL_LENGTH, 
   *                          the first (lowest index) SHA224_NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
   * 
   * @return A pointer to resultArray.
   */
  void *sha224Hmac(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t resultArrayLength);

  /**
   * Create a SHA224 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
   * Uses the BearSSL cryptographic library.
   * 
   * @param message The string from which to create the HMAC.
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to SHA224_NATURAL_LENGTH.
   * 
   * @return A String with the generated HMAC in HEX format.
   */
  String sha224Hmac(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);

  /**
   * Create a SHA224 HMAC from the data, using the provided hashKey. The result will be resultArrayLength bytes long and stored in resultArray.
   * Constant-time version.
   * Uses the BearSSL cryptographic library.
   * 
   * @param data The data array from which to create the HMAC.
   * @param dataLength The length of the data array in bytes. Valid values are in the range [ctMinDataLength(), ctMaxDataLength()].
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param resultArray The array wherein to store the resulting HMAC.
   * @param resultArrayLength The length of resultArray in bytes. Determines the HMAC length. If resultArrayLength is greater than SHA224_NATURAL_LENGTH, 
   *                          the first (lowest index) SHA224_NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
   * 
   * @return A pointer to resultArray.
   */
  void *sha224HmacCT(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t resultArrayLength);

  /**
   * Create a SHA224 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
   * Constant-time version.
   * Uses the BearSSL cryptographic library.
   * 
   * @param message The string from which to create the HMAC. Must have a length in the range [ctMinDataLength(), ctMaxDataLength()].
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to SHA224_NATURAL_LENGTH.
   * 
   * @return A String with the generated HMAC in HEX format.
   */
  String sha224HmacCT(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);


  // #################### SHA-256 ####################

  /**
   * Create a SHA256 hash of the data. The result will be SHA256_NATURAL_LENGTH bytes long and stored in resultArray.
   * Uses the BearSSL cryptographic library.
   * 
   * @param data The data array from which to create the hash.
   * @param dataLength The length of the data array in bytes.
   * @param resultArray The array wherein to store the resulting hash. MUST be be able to contain SHA256_NATURAL_LENGTH bytes or more.
   * 
   * @return A pointer to resultArray.
   */
  void *sha256Hash(const void *data, const size_t dataLength, void *resultArray);
  
  /**
   * Create a SHA256 hash of the data. The result will be SHA256_NATURAL_LENGTH bytes long and returned as a String in HEX format.
   * Uses the BearSSL cryptographic library.
   * 
   * @param message The string from which to create the hash.
   * 
   * @return A String with the generated hash in HEX format.
   */
  String sha256Hash(const String &message);

  /**
   * Create a SHA256 HMAC from the data, using the provided hashKey. The result will be resultArrayLength bytes long and stored in resultArray.
   * Uses the BearSSL cryptographic library.
   * 
   * @param data The data array from which to create the HMAC.
   * @param dataLength The length of the data array in bytes.
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param resultArray The array wherein to store the resulting HMAC.
   * @param resultArrayLength The length of resultArray in bytes. Determines the HMAC length. If resultArrayLength is greater than SHA256_NATURAL_LENGTH, 
   *                          the first (lowest index) SHA256_NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
   * 
   * @return A pointer to resultArray.
   */
  void *sha256Hmac(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t resultArrayLength);

  /**
   * Create a SHA256 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
   * Uses the BearSSL cryptographic library.
   * 
   * @param message The string from which to create the HMAC.
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to SHA256_NATURAL_LENGTH.
   * 
   * @return A String with the generated HMAC in HEX format.
   */
  String sha256Hmac(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);

  /**
   * Create a SHA256 HMAC from the data, using the provided hashKey. The result will be resultArrayLength bytes long and stored in resultArray.
   * Constant-time version.
   * Uses the BearSSL cryptographic library.
   * 
   * @param data The data array from which to create the HMAC.
   * @param dataLength The length of the data array in bytes. Valid values are in the range [ctMinDataLength(), ctMaxDataLength()].
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param resultArray The array wherein to store the resulting HMAC.
   * @param resultArrayLength The length of resultArray in bytes. Determines the HMAC length. If resultArrayLength is greater than SHA256_NATURAL_LENGTH, 
   *                          the first (lowest index) SHA256_NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
   * 
   * @return A pointer to resultArray.
   */
  void *sha256HmacCT(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t resultArrayLength);

  /**
   * Create a SHA256 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
   * Constant-time version.
   * Uses the BearSSL cryptographic library.
   * 
   * @param message The string from which to create the HMAC. Must have a length in the range [ctMinDataLength(), ctMaxDataLength()].
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to SHA256_NATURAL_LENGTH.
   * 
   * @return A String with the generated HMAC in HEX format.
   */
  String sha256HmacCT(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);


  // #################### SHA-384 ####################

  /**
   * Create a SHA384 hash of the data. The result will be SHA384_NATURAL_LENGTH bytes long and stored in resultArray.
   * Uses the BearSSL cryptographic library.
   * 
   * @param data The data array from which to create the hash.
   * @param dataLength The length of the data array in bytes.
   * @param resultArray The array wherein to store the resulting hash. MUST be be able to contain SHA384_NATURAL_LENGTH bytes or more.
   * 
   * @return A pointer to resultArray.
   */
  void *sha384Hash(const void *data, const size_t dataLength, void *resultArray);
  
  /**
   * Create a SHA384 hash of the data. The result will be SHA384_NATURAL_LENGTH bytes long and returned as a String in HEX format.
   * Uses the BearSSL cryptographic library.
   * 
   * @param message The string from which to create the hash.
   * 
   * @return A String with the generated hash in HEX format.
   */
  String sha384Hash(const String &message);

  /**
   * Create a SHA384 HMAC from the data, using the provided hashKey. The result will be resultArrayLength bytes long and stored in resultArray.
   * Uses the BearSSL cryptographic library.
   * 
   * @param data The data array from which to create the HMAC.
   * @param dataLength The length of the data array in bytes.
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param resultArray The array wherein to store the resulting HMAC.
   * @param resultArrayLength The length of resultArray in bytes. Determines the HMAC length. If resultArrayLength is greater than SHA384_NATURAL_LENGTH, 
   *                          the first (lowest index) SHA384_NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
   * 
   * @return A pointer to resultArray.
   */
  void *sha384Hmac(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t resultArrayLength);

  /**
   * Create a SHA384 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
   * Uses the BearSSL cryptographic library.
   * 
   * @param message The string from which to create the HMAC.
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to SHA384_NATURAL_LENGTH.
   * 
   * @return A String with the generated HMAC in HEX format.
   */
  String sha384Hmac(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);

  /**
   * Create a SHA384 HMAC from the data, using the provided hashKey. The result will be resultArrayLength bytes long and stored in resultArray.
   * Constant-time version.
   * Uses the BearSSL cryptographic library.
   * 
   * @param data The data array from which to create the HMAC.
   * @param dataLength The length of the data array in bytes. Valid values are in the range [ctMinDataLength(), ctMaxDataLength()].
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param resultArray The array wherein to store the resulting HMAC.
   * @param resultArrayLength The length of resultArray in bytes. Determines the HMAC length. If resultArrayLength is greater than SHA384_NATURAL_LENGTH, 
   *                          the first (lowest index) SHA384_NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
   * 
   * @return A pointer to resultArray.
   */
  void *sha384HmacCT(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t resultArrayLength);

  /**
   * Create a SHA384 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
   * Constant-time version.
   * Uses the BearSSL cryptographic library.
   * 
   * @param message The string from which to create the HMAC. Must have a length in the range [ctMinDataLength(), ctMaxDataLength()].
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to SHA384_NATURAL_LENGTH.
   * 
   * @return A String with the generated HMAC in HEX format.
   */
  String sha384HmacCT(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);


  // #################### SHA-512 ####################

  /**
   * Create a SHA512 hash of the data. The result will be SHA512_NATURAL_LENGTH bytes long and stored in resultArray.
   * Uses the BearSSL cryptographic library.
   * 
   * @param data The data array from which to create the hash.
   * @param dataLength The length of the data array in bytes.
   * @param resultArray The array wherein to store the resulting hash. MUST be be able to contain SHA512_NATURAL_LENGTH bytes or more.
   * 
   * @return A pointer to resultArray.
   */
  void *sha512Hash(const void *data, const size_t dataLength, void *resultArray);
  
  /**
   * Create a SHA512 hash of the data. The result will be SHA512_NATURAL_LENGTH bytes long and returned as a String in HEX format.
   * Uses the BearSSL cryptographic library.
   * 
   * @param message The string from which to create the hash.
   * 
   * @return A String with the generated hash in HEX format.
   */
  String sha512Hash(const String &message);

  /**
   * Create a SHA512 HMAC from the data, using the provided hashKey. The result will be resultArrayLength bytes long and stored in resultArray.
   * Uses the BearSSL cryptographic library.
   * 
   * @param data The data array from which to create the HMAC.
   * @param dataLength The length of the data array in bytes.
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param resultArray The array wherein to store the resulting HMAC.
   * @param resultArrayLength The length of resultArray in bytes. Determines the HMAC length. If resultArrayLength is greater than SHA512_NATURAL_LENGTH, 
   *                          the first (lowest index) SHA512_NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
   * 
   * @return A pointer to resultArray.
   */
  void *sha512Hmac(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t resultArrayLength);

  /**
   * Create a SHA512 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
   * Uses the BearSSL cryptographic library.
   * 
   * @param message The string from which to create the HMAC.
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to SHA512_NATURAL_LENGTH.
   * 
   * @return A String with the generated HMAC in HEX format.
   */
  String sha512Hmac(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);

  /**
   * Create a SHA512 HMAC from the data, using the provided hashKey. The result will be resultArrayLength bytes long and stored in resultArray.
   * Constant-time version.
   * Uses the BearSSL cryptographic library.
   * 
   * @param data The data array from which to create the HMAC.
   * @param dataLength The length of the data array in bytes. Valid values are in the range [ctMinDataLength(), ctMaxDataLength()].
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param resultArray The array wherein to store the resulting HMAC.
   * @param resultArrayLength The length of resultArray in bytes. Determines the HMAC length. If resultArrayLength is greater than SHA512_NATURAL_LENGTH, 
   *                          the first (lowest index) SHA512_NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
   * 
   * @return A pointer to resultArray.
   */
  void *sha512HmacCT(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t resultArrayLength);

  /**
   * Create a SHA512 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
   * Constant-time version.
   * Uses the BearSSL cryptographic library.
   * 
   * @param message The string from which to create the HMAC. Must have a length in the range [ctMinDataLength(), ctMaxDataLength()].
   * @param hashKey The hash key to use when creating the HMAC.
   * @param hashKeyLength The length of the hash key in bytes.
   * @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to SHA512_NATURAL_LENGTH.
   * 
   * @return A String with the generated HMAC in HEX format.
   */
  String sha512HmacCT(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);


  // #################### MD5+SHA-1 ####################
  
  /**
   * Create a MD5+SHA-1 hash of the data. The result will be MD5SHA1_NATURAL_LENGTH bytes long and stored in resultArray.
   * Uses the BearSSL cryptographic library.
   * 
   * MD5+SHA-1 is the concatenation of MD5 and SHA-1 computed over the same input; in the implementation, the internal data buffer is shared, 
   * thus making it more memory-efficient than separate MD5 and SHA-1. It can be useful in implementing SSL 3.0, TLS 1.0 and TLS 1.1.
   * 
   * @param data The data array from which to create the hash.
   * @param dataLength The length of the data array in bytes.
   * @param resultArray The array wherein to store the resulting hash. MUST be be able to contain MD5SHA1_NATURAL_LENGTH bytes or more.
   * 
   * @return A pointer to resultArray.
   */
  void *md5sha1Hash(const void *data, const size_t dataLength, void *resultArray);
  
  /**
   * Create a MD5+SHA-1 hash of the data. The result will be MD5SHA1_NATURAL_LENGTH bytes long and returned as a String in HEX format.
   * Uses the BearSSL cryptographic library.
   * 
   * MD5+SHA-1 is the concatenation of MD5 and SHA-1 computed over the same input; in the implementation, the internal data buffer is shared, 
   * thus making it more memory-efficient than separate MD5 and SHA-1. It can be useful in implementing SSL 3.0, TLS 1.0 and TLS 1.1.
   * 
   * @param message The string from which to create the hash.
   * 
   * @return A String with the generated hash in HEX format.
   */
  String md5sha1Hash(const String &message);
}

#endif
