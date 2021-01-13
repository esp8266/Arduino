/*
    BearSSL Copyright (c) 2016 Thomas Pornin <pornin@bolet.org>
    Rest of this file Copyright (C) 2019 Anders Löfgren

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

#ifndef __ESP8266_ARDUINO_CRYPTO_H__
#define __ESP8266_ARDUINO_CRYPTO_H__

#include <Arduino.h>
#include <bearssl/bearssl_kdf.h>

namespace experimental
{
namespace crypto
{
/**
    Regarding constant-time (CT) HMAC:

    Basically, constant-time algorithms makes it harder for attackers to learn things about your system based on the execution time of code.
    Good intro here: https://www.bearssl.org/constanttime.html

    It should be noted that every HMAC is already partially constant-time. Quoting the link above:
    "Hash functions implemented by BearSSL (MD5, SHA-1, SHA-224, SHA-256, SHA-384 and SHA-512) consist in bitwise logical operations and additions on 32-bit or 64-bit words,
    naturally yielding constant-time operations. HMAC is naturally as constant-time as the underlying hash function. The size of the MACed data, and the size of the key,
    may leak, though; only the contents are protected."

    For messages much smaller than getCtMaxDataLength(), constant-time processing takes substantially longer time to complete than a normal HMAC,
    determined by the size of (getCtMaxDataLength() - getCtMinDataLength()).
    Constant-time processing also sets limits on the data length.

    Making the fixed data length limits variable will generally defeat the purpose of using constant-time.
    Using data that exceeds the fixed data length limits will create the wrong HMAC.
*/


/**
    The nonce generator should take an uint8_t array with a given size in bytes and fill it with the nonce.
    The uint8_t array should then be returned by the nonce generator.
*/
using nonceGeneratorType = std::function<uint8_t *(uint8_t *, const size_t)>;

constexpr uint8_t ENCRYPTION_KEY_LENGTH = 32;

constexpr uint32_t CT_MAX_DIFF = 1073741823; // 2^30 - 1

/**
    This function allows for fine-tuning of the specifications for the constant time calculations.
    It should not be changed once a constant time function has been used at least once.
    Otherwise the constant time will not be constant for the used functions.

    The difference getCtMaxDataLength() - getCtMinDataLength() MUST be less than 2^30 (i.e. about one gigabyte).
*/
void setCtMinDataLength(const size_t ctMinDataLength);
/**
    0 by default.
*/
size_t getCtMinDataLength();

/**
    This function allows for fine-tuning of the specifications for the constant time calculations.
    It should not be changed once a constant time function has been used at least once.
    Otherwise the constant time will not be constant for the used functions.

    The difference getCtMaxDataLength() - getCtMinDataLength() MUST be less than 2^30 (i.e. about one gigabyte).
*/
void setCtMaxDataLength(const size_t ctMaxDataLength);
/**
    1024 by default.
*/
size_t getCtMaxDataLength();

/**
    Set the nonce generator used by the Crypto functions.

    @param nonceGenerator The nonce generator to use.
*/
void setNonceGenerator(nonceGeneratorType nonceGenerator);
nonceGeneratorType getNonceGenerator();


// #################### MD5 ####################

struct MD5
{
    static constexpr uint8_t NATURAL_LENGTH = 16;

    /**
        WARNING! The MD5 hash is broken in terms of attacker resistance.
        Only use it in those cases where attacker resistance is not important. Prefer SHA-256 or higher otherwise.

        Create a MD5 hash of the data. The result will be NATURAL_LENGTH bytes long and stored in resultArray.
        Uses the BearSSL cryptographic library.

        @param data The data array from which to create the hash.
        @param dataLength The length of the data array in bytes.
        @param resultArray The array wherein to store the resulting hash. MUST be be able to contain NATURAL_LENGTH bytes or more.

        @return A pointer to resultArray.
    */
    static void *hash(const void *data, const size_t dataLength, void *resultArray) __attribute__((deprecated));

    /**
        WARNING! The MD5 hash is broken in terms of attacker resistance.
        Only use it in those cases where attacker resistance is not important. Prefer SHA-256 or higher otherwise.

        Create a MD5 hash of the data. The result will be NATURAL_LENGTH bytes long and returned as a String in HEX format.
        Uses the BearSSL cryptographic library.

        @param message The string from which to create the hash.

        @return A String with the generated hash in HEX format.
    */
    static String hash(const String &message) __attribute__((deprecated));

    /**
        Create a MD5 HMAC from the data, using the provided hashKey. The result will be up to outputLength bytes long and stored in resultArray.
        Uses the BearSSL cryptographic library.

        @param data The data array from which to create the HMAC.
        @param dataLength The length of the data array in bytes.
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param resultArray The array wherein to store the resulting HMAC.
        @param outputLength The desired length of the generated HMAC, in bytes. Must fit within resultArray. If outputLength is greater than NATURAL_LENGTH,
                           the first (lowest index) NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
                           If outputLength is 0, then the natural HMAC output length is selected.

        @return A pointer to resultArray.
    */
    static void *hmac(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength);

    /**
        Create a MD5 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
        Uses the BearSSL cryptographic library.

        @param message The string from which to create the HMAC.
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to NATURAL_LENGTH.

        @return A String with the generated HMAC in HEX format.
    */
    static String hmac(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);

    /**
        Create a MD5 HMAC from the data, using the provided hashKey. The result will be up to outputLength bytes long and stored in resultArray.
        Constant-time version.
        Uses the BearSSL cryptographic library.

        @param data The data array from which to create the HMAC.
        @param dataLength The length of the data array in bytes. Valid values are in the range [getCtMinDataLength(), getCtMaxDataLength()].
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param resultArray The array wherein to store the resulting HMAC.
        @param outputLength The desired length of the generated HMAC, in bytes. Must fit within resultArray. If outputLength is greater than NATURAL_LENGTH,
                           the first (lowest index) NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
                           If outputLength is 0, then the natural HMAC output length is selected.

        @return A pointer to resultArray.
    */
    static void *hmacCT(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength);

    /**
        Create a MD5 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
        Constant-time version.
        Uses the BearSSL cryptographic library.

        @param message The string from which to create the HMAC. Must have a length in the range [getCtMinDataLength(), getCtMaxDataLength()].
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to NATURAL_LENGTH.

        @return A String with the generated HMAC in HEX format.
    */
    static String hmacCT(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);
};


// #################### SHA-1 ####################

struct SHA1
{
    static constexpr uint8_t NATURAL_LENGTH = 20;

    /**
        WARNING! The SHA-1 hash is broken in terms of attacker resistance.
        Only use it in those cases where attacker resistance is not important. Prefer SHA-256 or higher otherwise.

        Create a SHA1 hash of the data. The result will be NATURAL_LENGTH bytes long and stored in resultArray.
        Uses the BearSSL cryptographic library.

        @param data The data array from which to create the hash.
        @param dataLength The length of the data array in bytes.
        @param resultArray The array wherein to store the resulting hash. MUST be be able to contain NATURAL_LENGTH bytes or more.

        @return A pointer to resultArray.
    */
    static void *hash(const void *data, const size_t dataLength, void *resultArray) __attribute__((deprecated));

    /**
        WARNING! The SHA-1 hash is broken in terms of attacker resistance.
        Only use it in those cases where attacker resistance is not important. Prefer SHA-256 or higher otherwise.

        Create a SHA1 hash of the data. The result will be NATURAL_LENGTH bytes long and returned as a String in HEX format.
        Uses the BearSSL cryptographic library.

        @param message The string from which to create the hash.

        @return A String with the generated hash in HEX format.
    */
    static String hash(const String &message) __attribute__((deprecated));

    /**
        Create a SHA1 HMAC from the data, using the provided hashKey. The result will be up to outputLength bytes long and stored in resultArray.
        Uses the BearSSL cryptographic library.

        @param data The data array from which to create the HMAC.
        @param dataLength The length of the data array in bytes.
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param resultArray The array wherein to store the resulting HMAC.
        @param outputLength The desired length of the generated HMAC, in bytes. Must fit within resultArray. If outputLength is greater than NATURAL_LENGTH,
                           the first (lowest index) NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
                           If outputLength is 0, then the natural HMAC output length is selected.

        @return A pointer to resultArray.
    */
    static void *hmac(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength);

    /**
        Create a SHA1 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
        Uses the BearSSL cryptographic library.

        @param message The string from which to create the HMAC.
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to NATURAL_LENGTH.

        @return A String with the generated HMAC in HEX format.
    */
    static String hmac(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);

    /**
        Create a SHA1 HMAC from the data, using the provided hashKey. The result will be up to outputLength bytes long and stored in resultArray.
        Constant-time version.
        Uses the BearSSL cryptographic library.

        @param data The data array from which to create the HMAC.
        @param dataLength The length of the data array in bytes. Valid values are in the range [getCtMinDataLength(), getCtMaxDataLength()].
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param resultArray The array wherein to store the resulting HMAC.
        @param outputLength The desired length of the generated HMAC, in bytes. Must fit within resultArray. If outputLength is greater than NATURAL_LENGTH,
                           the first (lowest index) NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
                           If outputLength is 0, then the natural HMAC output length is selected.

        @return A pointer to resultArray.
    */
    static void *hmacCT(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength);

    /**
        Create a SHA1 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
        Constant-time version.
        Uses the BearSSL cryptographic library.

        @param message The string from which to create the HMAC. Must have a length in the range [getCtMinDataLength(), getCtMaxDataLength()].
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to NATURAL_LENGTH.

        @return A String with the generated HMAC in HEX format.
    */
    static String hmacCT(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);
};


// #################### SHA-224 ####################

struct SHA224
{
    static constexpr uint8_t NATURAL_LENGTH = 28;

    /**
        Create a SHA224 hash of the data. The result will be NATURAL_LENGTH bytes long and stored in resultArray.
        Uses the BearSSL cryptographic library.

        @param data The data array from which to create the hash.
        @param dataLength The length of the data array in bytes.
        @param resultArray The array wherein to store the resulting hash. MUST be be able to contain NATURAL_LENGTH bytes or more.

        @return A pointer to resultArray.
    */
    static void *hash(const void *data, const size_t dataLength, void *resultArray);

    /**
        Create a SHA224 hash of the data. The result will be NATURAL_LENGTH bytes long and returned as a String in HEX format.
        Uses the BearSSL cryptographic library.

        @param message The string from which to create the hash.

        @return A String with the generated hash in HEX format.
    */
    static String hash(const String &message);

    /**
        Create a SHA224 HMAC from the data, using the provided hashKey. The result will be up to outputLength bytes long and stored in resultArray.
        Uses the BearSSL cryptographic library.

        @param data The data array from which to create the HMAC.
        @param dataLength The length of the data array in bytes.
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param resultArray The array wherein to store the resulting HMAC.
        @param outputLength The desired length of the generated HMAC, in bytes. Must fit within resultArray. If outputLength is greater than NATURAL_LENGTH,
                           the first (lowest index) NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
                           If outputLength is 0, then the natural HMAC output length is selected.

        @return A pointer to resultArray.
    */
    static void *hmac(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength);

    /**
        Create a SHA224 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
        Uses the BearSSL cryptographic library.

        @param message The string from which to create the HMAC.
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to NATURAL_LENGTH.

        @return A String with the generated HMAC in HEX format.
    */
    static String hmac(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);

    /**
        Create a SHA224 HMAC from the data, using the provided hashKey. The result will be up to outputLength bytes long and stored in resultArray.
        Constant-time version.
        Uses the BearSSL cryptographic library.

        @param data The data array from which to create the HMAC.
        @param dataLength The length of the data array in bytes. Valid values are in the range [getCtMinDataLength(), getCtMaxDataLength()].
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param resultArray The array wherein to store the resulting HMAC.
        @param outputLength The desired length of the generated HMAC, in bytes. Must fit within resultArray. If outputLength is greater than NATURAL_LENGTH,
                           the first (lowest index) NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
                           If outputLength is 0, then the natural HMAC output length is selected.

        @return A pointer to resultArray.
    */
    static void *hmacCT(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength);

    /**
        Create a SHA224 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
        Constant-time version.
        Uses the BearSSL cryptographic library.

        @param message The string from which to create the HMAC. Must have a length in the range [getCtMinDataLength(), getCtMaxDataLength()].
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to NATURAL_LENGTH.

        @return A String with the generated HMAC in HEX format.
    */
    static String hmacCT(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);
};


// #################### SHA-256 ####################

struct SHA256
{
    static constexpr uint8_t NATURAL_LENGTH = 32;

    /**
        Create a SHA256 hash of the data. The result will be NATURAL_LENGTH bytes long and stored in resultArray.
        Uses the BearSSL cryptographic library.

        @param data The data array from which to create the hash.
        @param dataLength The length of the data array in bytes.
        @param resultArray The array wherein to store the resulting hash. MUST be be able to contain NATURAL_LENGTH bytes or more.

        @return A pointer to resultArray.
    */
    static void *hash(const void *data, const size_t dataLength, void *resultArray);

    /**
        Create a SHA256 hash of the data. The result will be NATURAL_LENGTH bytes long and returned as a String in HEX format.
        Uses the BearSSL cryptographic library.

        @param message The string from which to create the hash.

        @return A String with the generated hash in HEX format.
    */
    static String hash(const String &message);

    /**
        Create a SHA256 HMAC from the data, using the provided hashKey. The result will be up to outputLength bytes long and stored in resultArray.
        Uses the BearSSL cryptographic library.

        @param data The data array from which to create the HMAC.
        @param dataLength The length of the data array in bytes.
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param resultArray The array wherein to store the resulting HMAC.
        @param outputLength The desired length of the generated HMAC, in bytes. Must fit within resultArray. If outputLength is greater than NATURAL_LENGTH,
                           the first (lowest index) NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
                           If outputLength is 0, then the natural HMAC output length is selected.

        @return A pointer to resultArray.
    */
    static void *hmac(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength);

    /**
        Create a SHA256 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
        Uses the BearSSL cryptographic library.

        @param message The string from which to create the HMAC.
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to NATURAL_LENGTH.

        @return A String with the generated HMAC in HEX format.
    */
    static String hmac(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);

    /**
        Create a SHA256 HMAC from the data, using the provided hashKey. The result will be up to outputLength bytes long and stored in resultArray.
        Constant-time version.
        Uses the BearSSL cryptographic library.

        @param data The data array from which to create the HMAC.
        @param dataLength The length of the data array in bytes. Valid values are in the range [getCtMinDataLength(), getCtMaxDataLength()].
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param resultArray The array wherein to store the resulting HMAC.
        @param outputLength The desired length of the generated HMAC, in bytes. Must fit within resultArray. If outputLength is greater than NATURAL_LENGTH,
                           the first (lowest index) NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
                           If outputLength is 0, then the natural HMAC output length is selected.

        @return A pointer to resultArray.
    */
    static void *hmacCT(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength);

    /**
        Create a SHA256 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
        Constant-time version.
        Uses the BearSSL cryptographic library.

        @param message The string from which to create the HMAC. Must have a length in the range [getCtMinDataLength(), getCtMaxDataLength()].
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to NATURAL_LENGTH.

        @return A String with the generated HMAC in HEX format.
    */
    static String hmacCT(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);
};


// #################### SHA-384 ####################

struct SHA384
{
    static constexpr uint8_t NATURAL_LENGTH = 48;

    /**
        Create a SHA384 hash of the data. The result will be NATURAL_LENGTH bytes long and stored in resultArray.
        Uses the BearSSL cryptographic library.

        @param data The data array from which to create the hash.
        @param dataLength The length of the data array in bytes.
        @param resultArray The array wherein to store the resulting hash. MUST be be able to contain NATURAL_LENGTH bytes or more.

        @return A pointer to resultArray.
    */
    static void *hash(const void *data, const size_t dataLength, void *resultArray);

    /**
        Create a SHA384 hash of the data. The result will be NATURAL_LENGTH bytes long and returned as a String in HEX format.
        Uses the BearSSL cryptographic library.

        @param message The string from which to create the hash.

        @return A String with the generated hash in HEX format.
    */
    static String hash(const String &message);

    /**
        Create a SHA384 HMAC from the data, using the provided hashKey. The result will be up to outputLength bytes long and stored in resultArray.
        Uses the BearSSL cryptographic library.

        @param data The data array from which to create the HMAC.
        @param dataLength The length of the data array in bytes.
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param resultArray The array wherein to store the resulting HMAC.
        @param outputLength The desired length of the generated HMAC, in bytes. Must fit within resultArray. If outputLength is greater than NATURAL_LENGTH,
                           the first (lowest index) NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
                           If outputLength is 0, then the natural HMAC output length is selected.

        @return A pointer to resultArray.
    */
    static void *hmac(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength);

    /**
        Create a SHA384 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
        Uses the BearSSL cryptographic library.

        @param message The string from which to create the HMAC.
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to NATURAL_LENGTH.

        @return A String with the generated HMAC in HEX format.
    */
    static String hmac(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);

    /**
        Create a SHA384 HMAC from the data, using the provided hashKey. The result will be up to outputLength bytes long and stored in resultArray.
        Constant-time version.
        Uses the BearSSL cryptographic library.

        @param data The data array from which to create the HMAC.
        @param dataLength The length of the data array in bytes. Valid values are in the range [getCtMinDataLength(), getCtMaxDataLength()].
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param resultArray The array wherein to store the resulting HMAC.
        @param outputLength The desired length of the generated HMAC, in bytes. Must fit within resultArray. If outputLength is greater than NATURAL_LENGTH,
                           the first (lowest index) NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
                           If outputLength is 0, then the natural HMAC output length is selected.

        @return A pointer to resultArray.
    */
    static void *hmacCT(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength);

    /**
        Create a SHA384 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
        Constant-time version.
        Uses the BearSSL cryptographic library.

        @param message The string from which to create the HMAC. Must have a length in the range [getCtMinDataLength(), getCtMaxDataLength()].
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to NATURAL_LENGTH.

        @return A String with the generated HMAC in HEX format.
    */
    static String hmacCT(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);
};


// #################### SHA-512 ####################

struct SHA512
{
    static constexpr uint8_t NATURAL_LENGTH = 64;

    /**
        Create a SHA512 hash of the data. The result will be NATURAL_LENGTH bytes long and stored in resultArray.
        Uses the BearSSL cryptographic library.

        @param data The data array from which to create the hash.
        @param dataLength The length of the data array in bytes.
        @param resultArray The array wherein to store the resulting hash. MUST be be able to contain NATURAL_LENGTH bytes or more.

        @return A pointer to resultArray.
    */
    static void *hash(const void *data, const size_t dataLength, void *resultArray);

    /**
        Create a SHA512 hash of the data. The result will be NATURAL_LENGTH bytes long and returned as a String in HEX format.
        Uses the BearSSL cryptographic library.

        @param message The string from which to create the hash.

        @return A String with the generated hash in HEX format.
    */
    static String hash(const String &message);

    /**
        Create a SHA512 HMAC from the data, using the provided hashKey. The result will be up to outputLength bytes long and stored in resultArray.
        Uses the BearSSL cryptographic library.

        @param data The data array from which to create the HMAC.
        @param dataLength The length of the data array in bytes.
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param resultArray The array wherein to store the resulting HMAC.
        @param outputLength The desired length of the generated HMAC, in bytes. Must fit within resultArray. If outputLength is greater than NATURAL_LENGTH,
                           the first (lowest index) NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
                           If outputLength is 0, then the natural HMAC output length is selected.

        @return A pointer to resultArray.
    */
    static void *hmac(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength);

    /**
        Create a SHA512 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
        Uses the BearSSL cryptographic library.

        @param message The string from which to create the HMAC.
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to NATURAL_LENGTH.

        @return A String with the generated HMAC in HEX format.
    */
    static String hmac(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);

    /**
        Create a SHA512 HMAC from the data, using the provided hashKey. The result will be up to outputLength bytes long and stored in resultArray.
        Constant-time version.
        Uses the BearSSL cryptographic library.

        @param data The data array from which to create the HMAC.
        @param dataLength The length of the data array in bytes. Valid values are in the range [getCtMinDataLength(), getCtMaxDataLength()].
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param resultArray The array wherein to store the resulting HMAC.
        @param outputLength The desired length of the generated HMAC, in bytes. Must fit within resultArray. If outputLength is greater than NATURAL_LENGTH,
                           the first (lowest index) NATURAL_LENGTH bytes of resultArray will be used for the HMAC.
                           If outputLength is 0, then the natural HMAC output length is selected.

        @return A pointer to resultArray.
    */
    static void *hmacCT(const void *data, const size_t dataLength, const void *hashKey, const size_t hashKeyLength, void *resultArray, const size_t outputLength);

    /**
        Create a SHA512 HMAC from the message, using the provided hashKey. The result will be hmacLength bytes long and returned as a String in HEX format.
        Constant-time version.
        Uses the BearSSL cryptographic library.

        @param message The string from which to create the HMAC. Must have a length in the range [getCtMinDataLength(), getCtMaxDataLength()].
        @param hashKey The hash key to use when creating the HMAC.
        @param hashKeyLength The length of the hash key in bytes.
        @param hmacLength The desired length of the generated HMAC, in bytes. Valid values are 1 to NATURAL_LENGTH.

        @return A String with the generated HMAC in HEX format.
    */
    static String hmacCT(const String &message, const void *hashKey, const size_t hashKeyLength, const size_t hmacLength);
};


// #################### MD5+SHA-1 ####################

struct MD5SHA1
{
    static constexpr uint8_t NATURAL_LENGTH = 36;

    /**
        Create a MD5+SHA-1 hash of the data. The result will be NATURAL_LENGTH bytes long and stored in resultArray.
        Uses the BearSSL cryptographic library.

        MD5+SHA-1 is the concatenation of MD5 and SHA-1 computed over the same input; in the implementation, the internal data buffer is shared,
        thus making it more memory-efficient than separate MD5 and SHA-1. It can be useful in implementing SSL 3.0, TLS 1.0 and TLS 1.1.

        @param data The data array from which to create the hash.
        @param dataLength The length of the data array in bytes.
        @param resultArray The array wherein to store the resulting hash. MUST be be able to contain NATURAL_LENGTH bytes or more.

        @return A pointer to resultArray.
    */
    static void *hash(const void *data, const size_t dataLength, void *resultArray);

    /**
        Create a MD5+SHA-1 hash of the data. The result will be NATURAL_LENGTH bytes long and returned as a String in HEX format.
        Uses the BearSSL cryptographic library.

        MD5+SHA-1 is the concatenation of MD5 and SHA-1 computed over the same input; in the implementation, the internal data buffer is shared,
        thus making it more memory-efficient than separate MD5 and SHA-1. It can be useful in implementing SSL 3.0, TLS 1.0 and TLS 1.1.

        @param message The string from which to create the hash.

        @return A String with the generated hash in HEX format.
    */
    static String hash(const String &message);
};


// #################### HKDF ####################

struct HKDF
{
    /**
        KDFs (key derivation functions) are functions that takes a variable length input, and provide a variable length output, meant to be used to derive subkeys from a master key.
        HKDF is a KDF defined by RFC 5869. It is based on HMAC. The provided implementation uses SHA-256 as the underlying hash function.

        The constructor initializes the HKDF implementation with the input data to use for HKDF processing. (calls HKDF::init())

        @param keyMaterial An array containing the key material to use when deriving subkeys. Typically this would be the master key.
        @param keyMaterialLength The length of keyMaterial in bytes.
        @param salt An array containing the salt to use when ingesting key material. Salt is non-secret and can be empty.
                   Its role is normally to bind the input to a conventional identifier that qualify it within the used protocol or application.
        @param saltLength The length of the salt array, in bytes.
    */
    HKDF(const void *keyMaterial, const size_t keyMaterialLength, const void *salt = nullptr, const size_t saltLength = 0);

    /**
        This method initializes the HKDF implementation with the input data to use for HKDF processing.
        Uses the BearSSL cryptographic library.

        @param keyMaterial An array containing the key material to use when deriving subkeys. Typically this would be the master key.
        @param keyMaterialLength The length of keyMaterial in bytes.
        @param salt An array containing the salt to use when ingesting key material. Salt is non-secret and can be empty.
                   Its role is normally to bind the input to a conventional identifier that qualify it within the used protocol or application.
        @param saltLength The length of the salt array, in bytes.
    */
    void init(const void *keyMaterial, const size_t keyMaterialLength, const void *salt = nullptr, const size_t saltLength = 0);

    /**
        Produce more output bytes from the current HKDF state. This method may be called several times to obtain the full output by chunks.
        The total output size is limited to 255 * SHA256::NATURAL_LENGTH bytes per unique HKDF::init()/constructor call.
        Uses the BearSSL cryptographic library.

        @param resultArray The array wherein to store the resulting HKDF.
        @param outputLength The requested number of bytes to fill with HKDF output in resultArray.
        @param info NOTE: For correct HKDF processing, the same "info" string must be provided for every call until there's a new unique HKDF::init().
                   An array containing the information string to use when producing output. Info is non-secret and can be empty.
                   Its role is normally to bind the output to a conventional identifier that qualify it within the used protocol or application.
        @param infoLength The length of the info array, in bytes.

        @return The number of HKDF bytes actually produced.
    */
    size_t produce(void *resultArray, const size_t outputLength, const void *info = nullptr, size_t infoLength = 0);

private:

    br_hkdf_context hkdfContext;
};


// #################### Authenticated Encryption with Associated Data (AEAD) ####################

/**
    From https://www.bearssl.org/apidoc/bearssl__aead_8h.html

    An AEAD algorithm processes messages and provides confidentiality (encryption) and checked integrity (MAC). It uses the following parameters:

     - A symmetric key. Exact size depends on the AEAD algorithm.
     - A nonce (IV). Size depends on the AEAD algorithm; for most algorithms, it is crucial for security that any given nonce value is never used twice for the same key and distinct messages.
     - Data to encrypt and protect.
     - Additional authenticated data, which is covered by the MAC but otherwise left untouched (i.e. not encrypted).

    The AEAD algorithm encrypts the data, and produces an authentication tag.
    It is assumed that the encrypted data, the tag, the additional authenticated data and the nonce are sent to the receiver;
    the additional data and the nonce may be implicit (e.g. using elements of the underlying transport protocol, such as record sequence numbers).
    The receiver will recompute the tag value and compare it with the one received;
    if they match, then the data is correct, and can be decrypted and used;
    otherwise, at least one of the elements was altered in transit, normally leading to wholesale rejection of the complete message.
*/


// #################### ChaCha20+Poly1305 AEAD ####################

struct ChaCha20Poly1305
{
    /**
        Encrypt the data array using the ChaCha20 stream cipher and use Poly1305 for message authentication.
        The function generates in place an equal-length ChaCha20 encrypted version of the data array.
        More information about this encryption standard can be found here: https://tools.ietf.org/html/rfc7539 , https://tools.ietf.org/html/rfc8439
        Uses the BearSSL cryptographic library.

        Encryption of small messages (up to a few hundred data bytes) takes around 0.5-1 ms with the default nonceGenerator, half of this without keySalt.

        The output values of ChaCha20Poly1305::encrypt should be passed as input values to ChaCha20Poly1305::decrypt.

        Note that a 12 byte nonce is generated via getNonceGenerator() every time ChaCha20Poly1305::encrypt is called.
        If the same key and nonce combination is used more than once for distinct messages, the encryption will be broken, so keep the following in mind:

        By default the nonce is generated via the hardware random number generator of the ESP8266.
        The entropy of this source may not be sufficient to avoid nonce collisions, so to further reduce the risk of encryption failure
        it is recommended that a keySalt is always provided when using the default nonceGenerator. Using a keySalt will create a
        pseudorandom subkey from the original key via HKDF, and use that for the encryption/decryption.
        The same key + keySalt will always generate the same subkey.

        An alternative to using a keySalt is to change the nonceGenerator so that it does not rely on random numbers.
        One way to do this would be to use a counter that guarantees the same key + nonce combination is never used.
        This may not be easily achievable in all scenarios, however.

        @param data An array containing the data to encrypt. The encrypted data is generated in place, so when the function returns the data array will contain the encrypted data.
        @param dataLength The length of the data array in bytes.
        @param key The secret encryption key to use. Must be 32 bytes (ENCRYPTION_KEY_LENGTH) long.
        @param keySalt The salt to use when generating a subkey from key. Note that the same salt must be used during decryption as during encryption. Set to nullptr to prevent subkey generation.
        @param keySaltLength The length of keySalt in bytes.
        @param resultingNonce The array that will store the nonce generated during encryption. Must be able to contain at least 12 bytes. The nonce is not secret and must be passed to the decryption function.
        @param resultingTag The array that will store the message authentication tag generated during encryption. Must be able to contain at least 16 bytes. The tag is not secret and must be passed to the decryption function.
        @param aad Additional authenticated data. This data will be covered by the Poly1305 MAC, but not encrypted.
                  You can include the unencrypted parts of your message as AAD to ensure that the encrypted content cannot
                  be re-sent with replaced unencrypted data by an attacker.
                  Defaults to nullptr.
        @param aadLength The length of the aad array in bytes. Defaults to 0.
    */
    static void encrypt(void *data, const size_t dataLength, const void *key, const void *keySalt, const size_t keySaltLength, void *resultingNonce, void *resultingTag, const void *aad = nullptr, const size_t aadLength = 0);

    /**
        Decrypt the data array using the ChaCha20 stream cipher and use Poly1305 for message authentication.
        The function generates in place an equal-length ChaCha20 decrypted version of the data array.
        More information about this encryption standard can be found here: https://tools.ietf.org/html/rfc7539 , https://tools.ietf.org/html/rfc8439
        Uses the BearSSL cryptographic library.

        Decryption of small messages (up to a few hundred data bytes) takes around 0.5-1 ms, half of this without keySalt.

        The output values of ChaCha20Poly1305::encrypt should be passed as input values to ChaCha20Poly1305::decrypt.

        @param data An array containing the data to decrypt. The decrypted data is generated in place, so when the function returns the data array will contain the decrypted data.
        @param dataLength The length of the data array in bytes.
        @param key The secret encryption key to use. Must be 32 bytes (ENCRYPTION_KEY_LENGTH) long.
        @param keySalt The salt to use when generating a subkey from key. Note that the same salt must be used during decryption as during encryption. Set to nullptr to prevent subkey generation.
        @param keySaltLength The length of keySalt in bytes.
        @param encryptionNonce An array containing the nonce that was generated during encryption. The nonce should be 12 bytes.
        @param encryptionTag An array containing the message authentication tag that was generated during encryption. The tag should be 16 bytes.
        @param aad Additional authenticated data. This data will be covered by the Poly1305 MAC, but not decrypted.
                  You can include the unencrypted parts of your message as AAD to ensure that the encrypted content cannot
                  be re-sent with replaced unencrypted data by an attacker.
                  Defaults to nullptr.
        @param aadLength The length of the aad array in bytes. Defaults to 0.

        @return True if the decryption was successful (the generated tag matches encryptionTag). False otherwise. Note that the data array is modified regardless of this outcome.
    */
    static bool decrypt(void *data, const size_t dataLength, const void *key, const void *keySalt, const size_t keySaltLength, const void *encryptionNonce, const void *encryptionTag, const void *aad = nullptr, const size_t aadLength = 0);
};
}
}
#endif
