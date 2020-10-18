/*
  ESP8266WiFiPassMan.h - Library for ESP8266.
  Copyright (c) 2020 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

/**
 * @brief ESP8266 WiFi SSID/password encoding/decoding library
 * @file ESP8266WiFiPassMan.cpp
 * @details
 *      By default, the ESP8266 saves WiFi passwords in plain text to on-chip
 *      flash and can be easily retrieved. This is a known design flaw in the
 *      ESP8266 which cannot be resolved by software. This library makes it more
 *      difficult to retrieve the WiFi SSID and password by encoding the
 *      credentials with a fast XTEA algorithm. Decoding SSID and password on a
 *      ESP8266 at 80MHZ takes around 0.5ms.
 *
 *      A hacker can simply dump the credentials by overwriting the
 *      sketch when it has physical access to the ESP8266 device, or remotely
 *      using OTA. The retrieved SSID and password can then be misused to login
 *      to this network.
 *
 *      For this reason, WiFi credentials must never be saved to flash and
 *      requires WiFi.persistent(false); at the beginning of each sketch.
 *      The disadvantage is that fast-connect at startup is disabled.
 *
 *      This library checks for WiFi SSID and password saved on on-chip flash
 *      in plain text. The application designer is responsible to securely erase
 *      saved WiFi SSID and password with system_restore() followed by
 *      system_reset().
 *
 *      USAGE
 *
 *      The application designer generates a secret key and encodes the WiFi
 *      SSID (optional) and password (required) by calling
 *      encodeWiFiCredentials(). This results in tables in flash which must be
 *      included in each sketch for decoding. Writing a new sketch to the
 *      ESP8266 overwrites the secret key and encoded WiFi SSID/password.
 *      Decoding must be performed once before calling WiFi.begin(SSID, PASS).
 *
 *      Encoding/decoding uses the hardware AP MAC address by default to bind to
 *      only one specific ESP8266 device. In this case the encoded WiFi
 *      SSID/password flash table cannot be copied to another ESP8266 with a
 *      different MAC address and requires a re-encoding.
 *
 *      DISCLAIMER
 *
 *      Encoding WiFi SSID and password is always better than saving it in plain
 *      text. The XTEA algorithm contains known and documented attacks and
 *      therefore the wording encoding is used instead of real encryption.
 *
 *      Macro XTEA_NUM_ROUNDS can be increased to 128 or higher to increase
 *      complexity. The duration also increases.
 *
 *      A hacker with physical access to the device can dump the entire flash
 *      to a binary file and analyze the location of the secret key with encoded
 *      WiFi SSID/password. For this architecture, it is technically not
 *      possible to protect the flash for readout. It requires a lot of research
 *      to attack decoding when the addresses are known.
 *
 *      100% security does not exist. It is up to crypto experts to determine
 *      the safeness of the used algorithm and library.
 *      Confusing the enemy is always better than doing nothing.
 */

#include <Arduino.h>
#include <ESP8266WiFi.h>

#include "ESP8266WiFiPassMan.h"
#include "PrintBuffer.h"

// XTEA uses 64-bit blocks, 64 bits is 8 bytes
#define XTEA_BLOCK_SIZE     8               //!< XTEA fixed 8 Byte block size
#define XTEA_DELTA          0x9E3779B9      //!< XTEA fixed delta calculation
#define XTEA_NUM_ROUNDS     64              //!< XTEA fixed number of rounds


/*!
 * @brief XTEA encode
 * @details
 *      https://en.wikipedia.org/wiki/XTEA
 * @param v
 *      64-bit data in/out
 * @param k
 *      128-bit secret key in
 */
static void xtea_encode(uint32_t v[2], const uint32_t k[4])
{
    uint32_t v0 = v[0];
    uint32_t v1 = v[1];
    uint32_t sum = 0;

    for (uint8_t i = 0; i < XTEA_NUM_ROUNDS; i++) {
        v0 += ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + k[sum & 3]);
        sum += XTEA_DELTA;
        v1 += ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + k[sum>>11 & 3]);
    }

    v[0] = v0;
    v[1] = v1;
}

/**
 * @brief XTEA decode
 * @param v
 *      64-bit data in/out
 * @param k
 *      128-bit secret key in
 */
static void xtea_decode(uint32_t v[2], const uint32_t k[4])
{
    uint32_t v0 = v[0];
    uint32_t v1 = v[1];
    uint32_t sum = (XTEA_DELTA * XTEA_NUM_ROUNDS);

    for (uint8_t i = 0; i < XTEA_NUM_ROUNDS; i++) {
        v1 -= ((v0 << 4 ^ v0 >> 5) + v0) ^ (sum + k[sum>>11 & 3]);
        sum -= XTEA_DELTA;
        v0 -= ((v1 << 4 ^ v1 >> 5) + v1) ^ (sum + k[sum & 3]);
    }

    v[0] = v0;
    v[1] = v1;
}

/**
 * @brief WiFi Password Manager constructor
 * @param encodeAPMAC
 *      true: Scramble secret key with AP MAC address. This protects decoding
 *            on another ESP8266.
 *      false: Skip scrambling secret key. Decoding works on any ESP8266.
 * @param stream
 *      Stream pointer. Default: Serial
 */
ESP8266WiFiPassMan::ESP8266WiFiPassMan(bool encodeAPMAC, Stream *stream) :
    _encodeAPMAC(encodeAPMAC), _APMAC(NULL), _stream(stream)
{
}

/**
 * @brief WiFi Password Manager destructor
 */
ESP8266WiFiPassMan::~ESP8266WiFiPassMan()
{
}

/**
 * @brief Generate pseudorandom secret key
 * @details
 *      It is recommended to press the reset button multiple times to increase
 *      randomness. The random generator uses RANDOM_REG32. Up to crypto experts
 *      to determine if this is safe enough for this encoding.
 * @param key
 *      Return secret key.
 * @retval true
 *      Success
 * @retval false
 *      Failed
 */
bool ESP8266WiFiPassMan::generateKey(uint32_t key[4])
{
    // Generate pseudo random key
    for (uint8_t i = 0; i < 255; i++) {
        key[0] = RANDOM_REG32;
        key[1] = RANDOM_REG32;
        key[2] = RANDOM_REG32;
        key[3] = RANDOM_REG32;

        if (checkSecretKey(key)) {
            return true;
        }
    }

    // Random generate defect
    return false;
}

/**
 * @brief Print secret key RAM
 * @param key
 *      Secret key input
 * @retval true
 *      Success
 * @retval false
 *      Stream pointer zero
 */
bool ESP8266WiFiPassMan::printKey(const uint32_t key[4])
{
    if (_stream == NULL) {
        return false;
    }

    // Print secret key
    _stream->printf(PSTR("// Secret key for WiFi password manager\n"));
    _stream->printf(PSTR("const uint32_t SECRET_KEY[4] PROGMEM = {\n"));
    _stream->printf(PSTR("  0x%08X, 0x%08X, 0x%08X, 0x%08X\n"),
                    key[0], key[1], key[2], key[3]);
    _stream->printf(PSTR("};\n"));

    return true;
}

/**
 * @brief Print secret key from flash
 * @param key
 *      Secret key input
 * @return
 *      see printKey()
 */
bool ESP8266WiFiPassMan::printKey_P(const uint32_t key[4])
{
    uint32_t _key[4];

    memcpy_P(_key, key, sizeof(_key));

    return printKey(_key);
}

/**
 * @brief XTEA encode char string up to 63 characters + NULL character
 * @param key
 *      Secret key input
 * @param bufferName
 *      Buffer name
 * @param buffer
 *      Encoded buffer output
 * @param str
 *      Input char array
 * @retval true
 *      Success
 * @retval false
 *      Stream pointer zero
 */
bool ESP8266WiFiPassMan::encodeString(const uint32_t key[4],
                                      const char *bufferName, uint32_t *buffer,
                                      const char *str)
{
    if (!encode(key, str, buffer)) {
        if (_stream) _stream->printf(PSTR("Error: Encode failed"));
        return false;
    }

    // Print encoded buffer
    if (_stream) _stream->printf(PSTR("const uint32_t %s[16] PROGMEM = {"), bufferName);
    for (uint8_t i = 0; i < XTEA_BUFFER_LEN_W; i++) {
        if ((i % 4) == 0) {
            Serial.print(F("\n  "));
        }
        if (_stream) _stream->printf(PSTR("0x%08X, "), buffer[i]);
    }
    if (_stream) _stream->printf(PSTR("\n"));
    if (_stream) _stream->printf(PSTR("};\n"));

    return true;
}

/**
 * @brief Set AP MAC address for secret key scrambling
 * @param APMAC
 *      AP MAC address
 */
void ESP8266WiFiPassMan::setAPMAC(uint8_t *APMAC)
{
    _APMAC = APMAC;
}

/**
 * @brief Encode WiFi SSID (optional) and password (required)
 * @details
 *      This function prints encoded flash tables on Stream port (default
 *      Serial) to be copied to the WiFi connect sketch.
 * @param ssid
 *      Optional SSID input char array up to 63 characters + NULL character
 *      Set to NULL to skip SSID encoding
 * @param password
 *      Password input char array up to 63 characters + NULL character
 * @param key
 *      Secret key input
 * @retval true
 *      Success
 * @retval false
 *      Failure
 */
bool ESP8266WiFiPassMan::encodeWiFiCredentials(const char *ssid,
                                               const char *password,
                                               const uint32_t key[4])
{
    uint32_t _key[4];
    uint32_t buffer[XTEA_BUFFER_LEN_W];

    // Check stream pointer
    if (_stream == NULL) {
        return false;
    }

    // Clear key or copy key from flash
    if (key == NULL) {
        memset(_key, 0, sizeof(_key));
    } else {
        memcpy_P(_key, key, sizeof(_key));
    }

    // Check which key should be used for encoding
    if (_key[0] == 0) {
        if (!generateKey(_key)) {
            _stream->printf(PSTR("Generate key failed\n"));
            return false;
        }
        _stream->printf(PSTR("New secret key generated\n\n"));
    } else {
        _stream->printf(PSTR("Using existing secret key\n\n"));
    }

    _stream->printf(PSTR("//----------------------------------------------\n"));
    _stream->printf(PSTR("// *** DON'T LOOSE YOUR SECRET KEY! ***\n\n"));

    // Print macro ENCODE_AP_MAC
    if (_encodeAPMAC) {
        uint8_t mac[6];

        // Get user or AP MAC
        getAPMAC(mac);

        _stream->printf(PSTR("// WiFi credentials can be decoded on "
                             "%02X:%02X:%02X:%02X:%02X:%02X only\n"),
                        mac[0],mac[1],mac[2],mac[3],mac[4],mac[5]);
        _stream->printf(PSTR("#define ENCODE_AP_MAC %s\n\n"),
                        _encodeAPMAC ? "true" : "false");
    } else {
        _stream->printf(PSTR("// WiFi credentials can be copied to any ESP8266\n"));
        _stream->printf(PSTR("#define ENCODE_AP_MAC false\n\n"));
    }

    // Print key
    printKey(_key);

    // Encode SSID
    if (ssid) {
        _stream->printf(PSTR("\n// Encoded WiFi SSID\n"));
        encodeString(_key, "WIFI_ENC_SSID", buffer, ssid);
    }

    // Encode WiFi password
    _stream->printf(PSTR("\n// Encoded WiFi password\n"));
    encodeString(_key, "WIFI_ENC_PASS", buffer, password);

    _stream->printf(PSTR("//----------------------------------------------\n"));

    // Print usage
    _stream->printf(PSTR("\nCopy the encoded block to the WiFi connect sketch\n\n"));

    return true;
}

/**
 * @brief Check weakness secret key
 * @param key
 *      Secret key input
 * @retval true
 *      Secret key is strong enough
 * @retval false
 *      Secret key does not contain enough '1' or '0' bits
 */
bool ESP8266WiFiPassMan::checkSecretKey(const uint32_t key[4])
{
    uint8_t ones;
    uint8_t zeros;

    for (uint8_t i = 0; i < 4; i++) {
        ones = 0;
        zeros = 0;

        for (uint8_t j = 0; j < 32; j++) {
            if (key[i] & (1<<j)) {
                ones++;
            } else {
                zeros++;
            }
        }

        if ((ones < 4) || (zeros < 4)) {
            // Weak key
            DEBUG_PASS_ENC("[WPM] Weak key!");
            return false;
        }
    }

    return true;
}

/**
 * @brief Check secret key in flash
 * @param key
 *      Secret key input
 * @return
 *      See checkSecretKey()
 */
bool ESP8266WiFiPassMan::checkSecretKey_P(const uint32_t key[4])
{
    uint32_t _key[4];

    // Copy key
    memcpy_P(_key, key, sizeof(_key));

    return checkSecretKey(_key);
}

/**
 * @brief Get user or AP MAC
 * @param mac
 *      User or AP MAC output
 */
bool ESP8266WiFiPassMan::getAPMAC(uint8_t mac[6])
{
    // Clear MAC
    memset(mac, 0, 6);

    if (!_encodeAPMAC) {
        // AP MAC encoding disabled
        DEBUG_PASS_ENC("[WPM] HW AP MAC disabled");
        return false;
    }

    if (_APMAC) {
        // Copy AP MAC from RAM to local buffer
        DEBUG_PASS_ENC("[WPM] User MAC:");
        memcpy(mac, _APMAC, 6);
    }

    if ((mac[0] == 0) && (mac[5] == 0)) {
        // Get hardware AP MAC address
        DEBUG_PASS_ENC("[WPM] Get HW AP MAC:");
        WiFi.macAddress(mac);
    }

    return true;
}

/**
 * @brief Scramble secret key with unique AP MAC address
 * @param key
 *      Secret key input/output
 */
void ESP8266WiFiPassMan::scrambleKeyAPMAC(uint32_t key[4])
{
    uint8_t mac[6];

    getAPMAC(mac);

    DEBUG_PASS_ENC("[WPM] MAC:");
    for (uint8_t i = 0; i < 6; i++) {
        DEBUG_PASS_ENC(" ");
        DEBUG_PASS_ENC("%02X", mac[i]);
    }
    DEBUG_PASS_ENC("\n");

    DEBUG_PASS_ENC("[WPM] Key in: ");
    for (uint8_t i = 0; i < 4; i++) {
        DEBUG_PASS_ENC(" ");
        DEBUG_PASS_ENC("%02X", key[i]);
    }
    DEBUG_PASS_ENC("\n");

    // XOR MAC Bytes with key to confuse the enemy and bind the encoding to
    // one unique ESP8266
    for (uint8_t i = 0; i < 32; i++) {
        ((uint8_t *)key)[i % 16] ^= mac[i % 6];
    }

    DEBUG_PASS_ENC("[WPM] Key out:");
    for (uint8_t i = 0; i < 4; i++) {
        DEBUG_PASS_ENC(" ");
        DEBUG_PASS_ENC("%02X", key[i]);
    }
    DEBUG_PASS_ENC("\n");
}

/**
 * @brief XTEA encode char array
 * @param key
 *      Secret key in flash
 * @param str
 *      Char array 63 characters + NULL in flash
 * @param buffer
 *      Buffer (Fixed 64/4 words) output encoded
 * @return
 *      See encode()
 */
bool ESP8266WiFiPassMan::encode_P(const uint32_t key[4], const char *str,
                                  uint32_t buffer[XTEA_BUFFER_LEN_W])
{
    char _str[XTEA_BUFFER_LEN_B];

    // Copy str from flash to RAM
    strcpy_P(_str, str);

    // Encode RAM str
    return encode(key, _str, buffer);
}

/**
 * @brief XTEA encode char array
 * @param key
 *      Secret key
 * @param str
 *      Char array 63 characters + NULL in RAM
 * @param buffer
 *      Buffer (Fixed 64/4 words) output encoded
 * @retval true
 *      Success
 * @retval false
 *      Failure
 */
bool ESP8266WiFiPassMan::encode(const uint32_t key[4], const char *str,
                                uint32_t buffer[XTEA_BUFFER_LEN_W])
{
    size_t strLength;
    uint32_t _key[4];

    // Check arguments
    if ((str == NULL) || (buffer == NULL)) {
        return false;
    }

    // Check str length
    strLength = strlen(str);
    if (strLength > (XTEA_BUFFER_LEN_B - 1)) {
        return false;
    }

    // Fill buffer with random ASCII data
    strcpy((char *)buffer, str);
    for (uint8_t i = strLength + 1; i < XTEA_BUFFER_LEN_B; i++) {
        // Generate random ASCII char space..~
        ((uint8_t *)buffer)[i] = ' ' + ((uint8_t)RANDOM_REG32 % 94);
    }

#ifdef DEBUG_ESP_PORT
    DEBUG_PASS_ENC("[WPM] Plain buffer:\n");
    printBuffer((uint8_t *)buffer, XTEA_BUFFER_LEN_B);
#endif

    // Copy secret key from flash
    memcpy_P(_key, key, sizeof(_key));

    // Scramble key with MAC address
    if (_encodeAPMAC) {
        scrambleKeyAPMAC(_key);
    }

    // Encode
    for (uint8_t i = 0; i < XTEA_BLOCK_SIZE; i++) {
        xtea_encode(&buffer[i * (XTEA_BLOCK_SIZE / sizeof(uint32_t))], _key);
    }

#ifdef DEBUG_ESP_PORT
    DEBUG_PASS_ENC("[WPM] Encoded buffer:\n");
    printBuffer((uint8_t *)buffer, XTEA_BUFFER_LEN_B);
#endif

    return true;
}

/**
 * @brief XTEA encode char array
 * @param key
 *      Secret key in flash
 * @param buffer
 *      Encoded input buffer (Fixed 64/4 words) in flash
 * @param str
 *      Char array 63 characters + NULL output
 * @return
 *      See encode()
 */
bool ESP8266WiFiPassMan::decode_P(const uint32_t key[4],
                                  const uint32_t buffer[XTEA_BUFFER_LEN_W],
                                  char *str)
{
    uint32_t _buffer[XTEA_BUFFER_LEN_W];

    // Copy str from flash to RAM
    memcpy_P(_buffer, buffer, sizeof(_buffer));

    // Encode RAM str
    return decode(key, _buffer, str);
}

/**
 * @brief XTEA encode char array
 * @param key
 *      Secret key
 * @param buffer
 *      Input/output buffer (Fixed 64/4 words) in RAM
 * @param str
 *      Char array 63 characters + NULL output
 * @retval true
 *      Success
 * @retval false
 *      Failure
 */
bool ESP8266WiFiPassMan::decode(const uint32_t key[4],
                                uint32_t buffer[XTEA_BUFFER_LEN_W],
                                char *str)
{
    size_t strLength;
    uint32_t _key[4];

    // Check arguments
    if ((buffer == NULL) || (str == NULL)) {
        return false;
    }

#ifdef DEBUG_ESP_PORT
    DEBUG_PASS_ENC("[WPM] Encoded buffer:\n");
    printBuffer((uint8_t *)buffer, XTEA_BUFFER_LEN_B);
#endif

    // Copy secret key from flash
    memcpy_P(_key, key, sizeof(_key));

    // Scramble key with MAC address
    if (_encodeAPMAC) {
        scrambleKeyAPMAC(_key);
    }

    // Decode
    for (uint8_t i = 0; i < XTEA_BLOCK_SIZE; i++) {
        xtea_decode(&buffer[i * (XTEA_BLOCK_SIZE / sizeof(uint32_t))], _key);
    }

#ifdef DEBUG_ESP_PORT
    DEBUG_PASS_ENC("[WPM] Decoded buffer:\n");
    printBuffer((uint8_t *)buffer, XTEA_BUFFER_LEN_B);
#endif

    // Check str length
    strLength = strlen((char *)buffer);
    if (strLength > (XTEA_BUFFER_LEN_B - 1)) {
        DEBUG_PASS_ENC("[WPM] Decoded length error:\n");
        str[0] = '\0';
        return false;
    }

    // Copy str
    strcpy(str, (char *)buffer);

    return true;
}

/**
 * @brief Run XTEA encode/decode/very on an input char array
 * @param str
 *      Char array
 * @retval true
 *      Success
 * @retval false
 *      Failed. Recommended to enable debug prints.
 */
bool ESP8266WiFiPassMan::selftest(const char *str)
{
    uint32_t key[4];
    uint32_t buffer[XTEA_BUFFER_LEN_W];
    char strOut[64];

    if (_stream) _stream->print(F("Starting selftest..."));

    if (!generateKey(key)) {
        if (_stream) _stream->println(F("Generate key failed"));
        return false;
    }

    if (!encode(key, str, buffer)) {
        if (_stream) _stream->println(F("Encode failed"));
        return false;
    }

    if (!decode(key, buffer, strOut)) {
        if (_stream) _stream->println(F("Decode failed"));
        return false;
    }

    if (strcmp(str, strOut) != 0) {
        if (_stream) _stream->println(F("Verify failed"));
        return false;
    }

    if (_stream) _stream->println(F("Passed"));

    return true;
}

/**
 * @brief Check if unsecure WiFi credentials are saved to on-chip flash
 * @retval true
 *      Unsecure: WiFi SSID/password saved as plain text to on-chip flash
 * @retval false
 *      Secure: No unsecure WiFi SSID/password found
 */
bool ESP8266WiFiPassMan::isWiFiCredentialSaved()
{
    struct station_config config;

    // Clear station config
    memset(&config, 0, sizeof(station_config));

    // Get current config from flash
    wifi_station_get_config_default(&config);

    // Check if unsecure WiFi password saved in on-chip flash which can be
    // easily retrieved
    if (strlen((const char *)config.ssid) > 0) {
        return true;
    }

    return false;
}

/**
 * @brief Erase unsecure saved WiFi credentials on-chip flash
 * @details
 *      system_restore() followed by system_restart()
 */
void ESP8266WiFiPassMan::eraseWiFiPasswords()
{
    // Erase all saved WiFi credentials from on-chip flash
    system_restore();
    // Generate restart
    system_restart();
    // The ESP8266 will not restart immediately.
    // Do not call other functions after calling this API.
    while (1) {
        ;
    }
}
