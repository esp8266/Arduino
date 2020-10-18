/*
  WiFiPassMan.h - Library for ESP8266.
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
 * @file ESP8266WiFiPassMan.h
 */

#ifndef ESP8266_WIFI_PASSWORD_MANAGER_H_
#define ESP8266_WIFI_PASSWORD_MANAGER_H_

#include <stdint.h>

// XTEA buffer size is 63 characters + NULL char
#define XTEA_BUFFER_LEN_B   64
// XTEA buffer size in words
#define XTEA_BUFFER_LEN_W   (XTEA_BUFFER_LEN_B / sizeof(uint32_t))

#ifdef DEBUG_ESP_WIFI
#ifdef DEBUG_ESP_PORT
#define DEBUG_PASS_ENC(fmt, ...) DEBUG_ESP_PORT.printf_P( (PGM_P)PSTR(fmt), ##__VA_ARGS__ )
#endif
#endif

#ifndef DEBUG_PASS_ENC
#define DEBUG_PASS_ENC(...) do { (void)0; } while (0)
#endif

/**
 * @brief WiFi password encoding/decoding class
 */
class ESP8266WiFiPassMan
{
public:
    ESP8266WiFiPassMan(bool encodeAPMAC=true, Stream *stream=&Serial);
    ~ESP8266WiFiPassMan();

    bool generateKey(uint32_t key[4]);
    bool checkSecretKey(const uint32_t key[4]);
    bool checkSecretKey_P(const uint32_t key[4]);
    bool printKey(const uint32_t key[4]);
    bool printKey_P(const uint32_t key[4]);
    bool encodeString(const uint32_t key[4],
                      const char *bufferName, uint32_t *buffer,
                      const char *str);
    void setAPMAC(uint8_t *APMAC);
    bool encodeWiFiCredentials(const char *ssid,
                               const char *password,
                               const uint32_t key[4]=NULL);
    bool decodeWiFiCredentials_P(const uint32_t key[4],
                                 char *ssid,
                                 char *password);

    bool encode(const uint32_t key[4],
                const char *str,  uint32_t buffer[XTEA_BUFFER_LEN_W]);
    bool encode_P(const uint32_t key[4],
                  const char *str, uint32_t buffer[XTEA_BUFFER_LEN_W]);
    bool decode(const uint32_t key[4],
                uint32_t buffer[XTEA_BUFFER_LEN_W], char *str);
    bool decode_P(const uint32_t key[4],
                  const uint32_t buffer[XTEA_BUFFER_LEN_W], char *str);

    bool selftest(const char *str);

    bool isWiFiCredentialSaved();
    void eraseWiFiPasswords();

private:
    bool _encodeAPMAC;
    uint8_t *_APMAC;
    Stream *_stream;

    bool getAPMAC(uint8_t mac[6]);
    void scrambleKeyAPMAC(uint32_t key[4]);
};

#endif // ESP8266_WIFI_PASSWORD_MANAGER_H_
