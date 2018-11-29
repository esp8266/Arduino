/**
 * ESP8266HTTPClient.h
 *
 * Created on: 02.11.2015
 *
 * Copyright (c) 2015 Markus Sattler. All rights reserved.
 * This file is part of the ESP8266HTTPClient for Arduino.
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
 * Modified by Jeroen Döll, June 2018
 */

#ifndef ESP8266HTTPClient_H_
#define ESP8266HTTPClient_H_

#ifndef HTTPCLIENT_1_1_COMPATIBLE
#define HTTPCLIENT_1_1_COMPATIBLE 1
#endif

#include <memory>
#include <Arduino.h>

#include <WiFiClient.h>

#ifdef DEBUG_ESP_HTTP_CLIENT
#ifdef DEBUG_ESP_PORT
#define DEBUG_HTTPCLIENT(fmt, ...) DEBUG_ESP_PORT.printf_P( (PGM_P)PSTR(fmt), ## __VA_ARGS__ )
#endif
#endif

#ifndef DEBUG_HTTPCLIENT
#define DEBUG_HTTPCLIENT(...)
#endif

#define HTTPCLIENT_DEFAULT_TCP_TIMEOUT (5000)

/// HTTP client errors
#define HTTPC_ERROR_CONNECTION_REFUSED  (-1)
#define HTTPC_ERROR_SEND_HEADER_FAILED  (-2)
#define HTTPC_ERROR_SEND_PAYLOAD_FAILED (-3)
#define HTTPC_ERROR_NOT_CONNECTED       (-4)
#define HTTPC_ERROR_CONNECTION_LOST     (-5)
#define HTTPC_ERROR_NO_STREAM           (-6)
#define HTTPC_ERROR_NO_HTTP_SERVER      (-7)
#define HTTPC_ERROR_TOO_LESS_RAM        (-8)
#define HTTPC_ERROR_ENCODING            (-9)
#define HTTPC_ERROR_STREAM_WRITE        (-10)
#define HTTPC_ERROR_READ_TIMEOUT        (-11)

/// size for the stream handling
#define HTTP_TCP_BUFFER_SIZE (1460)

/// HTTP codes see RFC7231
typedef enum {
    HTTP_CODE_CONTINUE = 100,
    HTTP_CODE_SWITCHING_PROTOCOLS = 101,
    HTTP_CODE_PROCESSING = 102,
    HTTP_CODE_OK = 200,
    HTTP_CODE_CREATED = 201,
    HTTP_CODE_ACCEPTED = 202,
    HTTP_CODE_NON_AUTHORITATIVE_INFORMATION = 203,
    HTTP_CODE_NO_CONTENT = 204,
    HTTP_CODE_RESET_CONTENT = 205,
    HTTP_CODE_PARTIAL_CONTENT = 206,
    HTTP_CODE_MULTI_STATUS = 207,
    HTTP_CODE_ALREADY_REPORTED = 208,
    HTTP_CODE_IM_USED = 226,
    HTTP_CODE_MULTIPLE_CHOICES = 300,
    HTTP_CODE_MOVED_PERMANENTLY = 301,
    HTTP_CODE_FOUND = 302,
    HTTP_CODE_SEE_OTHER = 303,
    HTTP_CODE_NOT_MODIFIED = 304,
    HTTP_CODE_USE_PROXY = 305,
    HTTP_CODE_TEMPORARY_REDIRECT = 307,
    HTTP_CODE_PERMANENT_REDIRECT = 308,
    HTTP_CODE_BAD_REQUEST = 400,
    HTTP_CODE_UNAUTHORIZED = 401,
    HTTP_CODE_PAYMENT_REQUIRED = 402,
    HTTP_CODE_FORBIDDEN = 403,
    HTTP_CODE_NOT_FOUND = 404,
    HTTP_CODE_METHOD_NOT_ALLOWED = 405,
    HTTP_CODE_NOT_ACCEPTABLE = 406,
    HTTP_CODE_PROXY_AUTHENTICATION_REQUIRED = 407,
    HTTP_CODE_REQUEST_TIMEOUT = 408,
    HTTP_CODE_CONFLICT = 409,
    HTTP_CODE_GONE = 410,
    HTTP_CODE_LENGTH_REQUIRED = 411,
    HTTP_CODE_PRECONDITION_FAILED = 412,
    HTTP_CODE_PAYLOAD_TOO_LARGE = 413,
    HTTP_CODE_URI_TOO_LONG = 414,
    HTTP_CODE_UNSUPPORTED_MEDIA_TYPE = 415,
    HTTP_CODE_RANGE_NOT_SATISFIABLE = 416,
    HTTP_CODE_EXPECTATION_FAILED = 417,
    HTTP_CODE_MISDIRECTED_REQUEST = 421,
    HTTP_CODE_UNPROCESSABLE_ENTITY = 422,
    HTTP_CODE_LOCKED = 423,
    HTTP_CODE_FAILED_DEPENDENCY = 424,
    HTTP_CODE_UPGRADE_REQUIRED = 426,
    HTTP_CODE_PRECONDITION_REQUIRED = 428,
    HTTP_CODE_TOO_MANY_REQUESTS = 429,
    HTTP_CODE_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,
    HTTP_CODE_INTERNAL_SERVER_ERROR = 500,
    HTTP_CODE_NOT_IMPLEMENTED = 501,
    HTTP_CODE_BAD_GATEWAY = 502,
    HTTP_CODE_SERVICE_UNAVAILABLE = 503,
    HTTP_CODE_GATEWAY_TIMEOUT = 504,
    HTTP_CODE_HTTP_VERSION_NOT_SUPPORTED = 505,
    HTTP_CODE_VARIANT_ALSO_NEGOTIATES = 506,
    HTTP_CODE_INSUFFICIENT_STORAGE = 507,
    HTTP_CODE_LOOP_DETECTED = 508,
    HTTP_CODE_NOT_EXTENDED = 510,
    HTTP_CODE_NETWORK_AUTHENTICATION_REQUIRED = 511
} t_http_codes;

typedef enum {
    HTTPC_TE_IDENTITY,
    HTTPC_TE_CHUNKED
} transferEncoding_t;

#if HTTPCLIENT_1_1_COMPATIBLE
class TransportTraits;
typedef std::unique_ptr<TransportTraits> TransportTraitsPtr;
#endif

class StreamString;

class HTTPClient
{
public:
    HTTPClient();
    ~HTTPClient();

/*
 * Since both begin() functions take a reference to client as a parameter, you need to 
 * ensure the client object lives the entire time of the HTTPClient
 */
    bool begin(WiFiClient &client, String url);
    bool begin(WiFiClient &client, String host, uint16_t port, String uri = "/", bool https = false);

#if HTTPCLIENT_1_1_COMPATIBLE
    // Plain HTTP connection, unencrypted
    bool begin(String url)  __attribute__ ((deprecated));
    bool begin(String host, uint16_t port, String uri = "/")  __attribute__ ((deprecated));
    // Use axTLS for secure HTTPS connection
    bool begin(String url, String httpsFingerprint)  __attribute__ ((deprecated));
    bool begin(String host, uint16_t port, String uri, String httpsFingerprint)  __attribute__ ((deprecated));
    // Use BearSSL for secure HTTPS connection
    bool begin(String url, const uint8_t httpsFingerprint[20])  __attribute__ ((deprecated));
    bool begin(String host, uint16_t port, String uri, const uint8_t httpsFingerprint[20])  __attribute__ ((deprecated));
    // deprecated, use the overload above instead
    bool begin(String host, uint16_t port, String uri, bool https, String httpsFingerprint)  __attribute__ ((deprecated));
#endif

    void end(void);

    bool connected(void);

    void setReuse(bool reuse); /// keep-alive
    void setUserAgent(const String& userAgent);
    void setAuthorization(const char * user, const char * password);
    void setAuthorization(const char * auth);
    void setTimeout(uint16_t timeout);

    void useHTTP10(bool usehttp10 = true);

    /// request handling
    int GET();
    int POST(uint8_t * payload, size_t size);
    int POST(String payload);
    int PUT(uint8_t * payload, size_t size);
    int PUT(String payload);
    int PATCH(uint8_t * payload, size_t size);
    int PATCH(String payload);
    int sendRequest(const char * type, String payload);
    int sendRequest(const char * type, uint8_t * payload = NULL, size_t size = 0);
    int sendRequest(const char * type, Stream * stream, size_t size = 0);

    void addHeader(const String& name, const String& value, bool first = false, bool replace = true);

    /// Response handling
    void collectHeaders(const char* headerKeys[], const size_t headerKeysCount);
    String header(const char* name);   // get request header value by name
    String header(size_t i);              // get request header value by number
    String headerName(size_t i);          // get request header name by number
    int headers();                     // get header count
    bool hasHeader(const char* name);  // check if header exists


    int getSize(void);

    WiFiClient& getStream(void);
    WiFiClient* getStreamPtr(void);
    int writeToStream(Stream* stream);
    const String& getString(void);

    static String errorToString(int error);

protected:
    struct RequestArgument {
        String key;
        String value;
    };

    bool beginInternal(String url, const char* expectedProtocol);
    void disconnect();
    void clear();
    int returnError(int error);
    bool connect(void);
    bool sendHeader(const char * type);
    int handleHeaderResponse();
    int writeToStreamDataBlock(Stream * stream, int len);


#if HTTPCLIENT_1_1_COMPATIBLE
    TransportTraitsPtr _transportTraits;
    std::unique_ptr<WiFiClient> _tcpDeprecated;
#endif
    WiFiClient* _client;

    /// request handling
    String _host;
    uint16_t _port = 0;
    bool _reuse = false;
    uint16_t _tcpTimeout = HTTPCLIENT_DEFAULT_TCP_TIMEOUT;
    bool _useHTTP10 = false;

    String _uri;
    String _protocol;
    String _headers;
    String _userAgent = "ESP8266HTTPClient";
    String _base64Authorization;

    /// Response handling
    RequestArgument* _currentHeaders = nullptr;
    size_t           _headerKeysCount = 0;

    int _returnCode = 0;
    int _size = -1;
    bool _canReuse = false;
    transferEncoding_t _transferEncoding = HTTPC_TE_IDENTITY;
    std::unique_ptr<StreamString> _payload;
};



#endif /* ESP8266HTTPClient_H_ */
