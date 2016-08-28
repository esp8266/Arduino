/**
 *
 * @file ESP8266HTTPUpdate.h
 * @date 21.06.2015
 * @author Markus Sattler
 *
 * Copyright (c) 2015 Markus Sattler. All rights reserved.
 * This file is part of the ESP8266 Http Updater.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#ifndef ESP8266HTTPUPDATE_H_
#define ESP8266HTTPUPDATE_H_

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <ESP8266HTTPClient.h>

#ifdef DEBUG_ESP_HTTP_UPDATE
#ifdef DEBUG_ESP_PORT
#define DEBUG_HTTP_UPDATE(...) DEBUG_ESP_PORT.printf( __VA_ARGS__ )
#endif
#endif

#ifndef DEBUG_HTTP_UPDATE
#define DEBUG_HTTP_UPDATE(...)
#endif

/// note we use HTTP client errors too so we start at 100
#define HTTP_UE_TOO_LESS_SPACE              (-100)
#define HTTP_UE_SERVER_NOT_REPORT_SIZE      (-101)
#define HTTP_UE_SERVER_FILE_NOT_FOUND       (-102)
#define HTTP_UE_SERVER_FORBIDDEN            (-103)
#define HTTP_UE_SERVER_WRONG_HTTP_CODE      (-104)
#define HTTP_UE_SERVER_FAULTY_MD5           (-105)
#define HTTP_UE_BIN_VERIFY_HEADER_FAILED    (-106)
#define HTTP_UE_BIN_FOR_WRONG_FLASH         (-107)

enum HTTPUpdateResult {
    HTTP_UPDATE_FAILED,
    HTTP_UPDATE_NO_UPDATES,
    HTTP_UPDATE_OK
};

typedef HTTPUpdateResult t_httpUpdate_return; // backward compatibility

class ESP8266HTTPUpdate
{
public:
    ESP8266HTTPUpdate(void);
    ~ESP8266HTTPUpdate(void);

    void rebootOnUpdate(bool reboot)
    {
        _rebootOnUpdate = reboot;
    }

    // This function is deprecated, use rebootOnUpdate and the next one instead
    t_httpUpdate_return update(const String& url, const String& currentVersion,
                               const String& httpsFingerprint, bool reboot) __attribute__((deprecated));
    t_httpUpdate_return update(const String& url, const String& currentVersion = "");
    t_httpUpdate_return update(const String& url, const String& currentVersion,
                               const String& httpsFingerprint);

    // This function is deprecated, use one of the overloads below along with rebootOnUpdate
    t_httpUpdate_return update(const String& host, uint16_t port, const String& uri, const String& currentVersion,
                               bool https, const String& httpsFingerprint, bool reboot) __attribute__((deprecated));

    t_httpUpdate_return update(const String& host, uint16_t port, const String& uri = "/",
                               const String& currentVersion = "");
    t_httpUpdate_return update(const String& host, uint16_t port, const String& url,
                               const String& currentVersion, const String& httpsFingerprint);

    // This function is deprecated, use rebootOnUpdate and the next one instead
    t_httpUpdate_return updateSpiffs(const String& url, const String& currentVersion,
                                     const String& httpsFingerprint, bool reboot) __attribute__((deprecated));
    t_httpUpdate_return updateSpiffs(const String& url, const String& currentVersion = "");
    t_httpUpdate_return updateSpiffs(const String& url, const String& currentVersion, const String& httpsFingerprint);


    int getLastError(void);
    String getLastErrorString(void);

protected:
    t_httpUpdate_return handleUpdate(HTTPClient& http, const String& currentVersion, bool spiffs = false);
    bool runUpdate(Stream& in, uint32_t size, String md5, int command = U_FLASH);

    int _lastError;
    bool _rebootOnUpdate = true;
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_HTTPUPDATE)
extern ESP8266HTTPUpdate ESPhttpUpdate;
#endif

#endif /* ESP8266HTTPUPDATE_H_ */
