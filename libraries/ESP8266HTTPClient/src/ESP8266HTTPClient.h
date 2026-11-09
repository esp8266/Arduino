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

#include <Arduino.h>
#include <StreamString.h>
#include <WiFiClient.h>

#include <memory>

#ifdef DEBUG_ESP_HTTP_CLIENT
#ifdef DEBUG_ESP_PORT
#define DEBUG_HTTPCLIENT(fmt, ...) DEBUG_ESP_PORT.printf_P( (PGM_P)PSTR(fmt), ## __VA_ARGS__ )
#endif
#endif

#ifndef DEBUG_HTTPCLIENT
#define DEBUG_HTTPCLIENT(...) do { (void)0; } while (0)
#endif

#define HTTPCLIENT_DEFAULT_TCP_TIMEOUT (5000)

/// HTTP client errors
#define HTTPC_ERROR_CONNECTION_FAILED   (-1)
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

constexpr int HTTPC_ERROR_CONNECTION_REFUSED __attribute__((deprecated)) = HTTPC_ERROR_CONNECTION_FAILED;

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

/**
 * redirection follow mode.
 * + `HTTPC_DISABLE_FOLLOW_REDIRECTS` - no redirection will be followed.
 * + `HTTPC_STRICT_FOLLOW_REDIRECTS` - strict RFC2616, only requests using
 *      GET or HEAD methods will be redirected (using the same method),
 *      since the RFC requires end-user confirmation in other cases.
 * + `HTTPC_FORCE_FOLLOW_REDIRECTS` - all redirections will be followed,
 *      regardless of a used method. New request will use the same method,
 *      and they will include the same body data and the same headers.
 *      In the sense of the RFC, it's just like every redirection is confirmed.
 */
typedef enum {
    HTTPC_DISABLE_FOLLOW_REDIRECTS,
    HTTPC_STRICT_FOLLOW_REDIRECTS,
    HTTPC_FORCE_FOLLOW_REDIRECTS
} followRedirects_t;

class TransportTraits;
typedef std::unique_ptr<TransportTraits> TransportTraitsPtr;

class HTTPClient
{
public:
    HTTPClient() = default;
    ~HTTPClient() = default;
    HTTPClient(HTTPClient&&) = default;
    HTTPClient& operator=(HTTPClient&&) = default;

    // Note that WiFiClient's underlying connection *will* be captured
    bool begin(WiFiClient &client, const String& url);
    bool begin(WiFiClient &client, const String& host, uint16_t port, const String& uri = "/", bool https = false);

    // old API is now explicitly forbidden
    bool begin(String url)  __attribute__ ((error("obsolete API, use ::begin(WiFiClient, url)")));
    bool begin(String host, uint16_t port, String uri = "/")  __attribute__ ((error("obsolete API, use ::begin(WiFiClient, host, port, uri)")));
    bool begin(String url, const uint8_t httpsFingerprint[20])  __attribute__ ((error("obsolete API, use ::begin(WiFiClientSecure, ...)")));
    bool begin(String host, uint16_t port, String uri, const uint8_t httpsFingerprint[20])  __attribute__ ((error("obsolete API, use ::begin(WiFiClientSecure, ...)")));
    bool begin(String host, uint16_t port, String uri, bool https, String httpsFingerprint)  __attribute__ ((error("obsolete API, use ::begin(WiFiClientSecure, ...)")));

    void end(void);

    bool connected(void);

    void setReuse(bool reuse); /// keep-alive
    void setUserAgent(const String& userAgent);
    void setAuthorization(const char * user, const char * password);
    void setAuthorization(const char * auth);
    void setAuthorization(String auth);
    void setTimeout(uint16_t timeout);
    void setHostHeader(const String& host);

    // Redirections
    void setFollowRedirects(followRedirects_t follow);
    void setRedirectLimit(uint16_t limit); // max redirects to follow for a single request

    bool setURL(const String& url); // handy for handling redirects
    void useHTTP10(bool usehttp10 = true);

    /// request handling
    int GET();
    int DELETE();
    int POST(const uint8_t* payload, size_t size);
    int POST(const String& payload);
    int PUT(const uint8_t* payload, size_t size);
    int PUT(const String& payload);
    int PATCH(const uint8_t* payload, size_t size);
    int PATCH(const String& payload);
    int sendRequest(const char* type, const String& payload);
    int sendRequest(const char* type, const uint8_t* payload = NULL, size_t size = 0);
    int sendRequest(const char* type, Stream * stream, size_t size = 0);

    void addHeader(const String& name, const String& value, bool first = false, bool replace = true);

    /// Response handling
    void collectHeaders(const char* headerKeys[], const size_t headerKeysCount);
    const String& header(const char* name);   // get request header value by name
    const String& header(size_t i);           // get request header value by number
    const String& headerName(size_t i);       // get request header name by number
    int headers();                     // get header count
    bool hasHeader(const char* name);  // check if header exists


    int getSize(void);
    const String& getLocation(void); // Location header from redirect if 3XX

    WiFiClient& getStream(void);
    WiFiClient* getStreamPtr(void);
    template <typename S> int writeToPrint(S* print) [[deprecated]] { return writeToStream(print); }
    template <typename S> int writeToStream(S* output);

    // In case of chunks = when size cannot be known in advance
    // by the library, it might be useful to pre-reserve enough
    // space instead of offending memory with a growing String
    const String& getString() { return getString(0); }
    const String& getString(int reserve);

    static String errorToString(int error);

protected:
    struct RequestArgument {
        String key;
        String value;
    };

    bool beginInternal(const String& url, const char* expectedProtocol);
    void disconnect(bool preserveClient = false);
    void clear();
    int returnError(int error);
    bool connect(void);
    bool sendHeader(const char * type);
    int handleHeaderResponse();
    int writeToStreamDataBlock(Stream * stream, int len);
    static int StreamReportToHttpClientReport (Stream::Report streamSendError);

    // The common pattern to use the class is to
    // {
    //     WiFiClient socket;
    //     HTTPClient http;
    //     http.begin(socket, "http://blahblah");
    // }
    // Make sure it's not possible to break things in an opposite direction

    std::unique_ptr<WiFiClient> _client;

    /// request handling
    String _host;
    String _hostHeader;
    uint16_t _port = 0;
    bool _reuse = true;
    uint16_t _tcpTimeout = HTTPCLIENT_DEFAULT_TCP_TIMEOUT;
    bool _useHTTP10 = false;

    String _uri;
    String _protocol;
    String _headers;
    String _base64Authorization;

    static const String defaultUserAgent;
    String _userAgent = defaultUserAgent;

    /// Response handling
    std::unique_ptr<RequestArgument[]> _currentHeaders;
    size_t _headerKeysCount = 0;

    int _returnCode = 0;
    int _size = -1;
    bool _canReuse = false;
    followRedirects_t _followRedirects = HTTPC_DISABLE_FOLLOW_REDIRECTS;
    uint16_t _redirectLimit = 10;
    String _location;
    transferEncoding_t _transferEncoding = HTTPC_TE_IDENTITY;
    std::unique_ptr<StreamString> _payload;
};

/**
 * write all  message body / payload to Stream
 * @param output Print*(obsolete) / Stream*
 * @return bytes written ( negative values are error codes )
 */
template <typename S>
int HTTPClient::writeToStream(S * output)
{
    if(!output) {
        return returnError(HTTPC_ERROR_NO_STREAM);
    }

    // Only return error if not connected and no data available, because otherwise ::getString() will return an error instead of an empty
    // string when the server returned a http code 204 (no content)
    if(!connected() && _transferEncoding != HTTPC_TE_IDENTITY && _size > 0) {
        return returnError(HTTPC_ERROR_NOT_CONNECTED);
    }

    // get length of document (is -1 when Server sends no Content-Length header)
    int len = _size;
    int ret = 0;

    if(_transferEncoding == HTTPC_TE_IDENTITY) {
        // len < 0: transfer all of it, with timeout
        // len >= 0: max:len, with timeout
        ret = _client->sendSize(output, len);

        // do we have an error?
        if(_client->getLastSendReport() != Stream::Report::Success) {
            return returnError(StreamReportToHttpClientReport(_client->getLastSendReport()));
        }
    } else if(_transferEncoding == HTTPC_TE_CHUNKED) {
        int size = 0;
        while(1) {
            if(!connected()) {
                return returnError(HTTPC_ERROR_CONNECTION_LOST);
            }
            String chunkHeader = _client->readStringUntil('\n');

            if(chunkHeader.length() <= 0) {
                return returnError(HTTPC_ERROR_READ_TIMEOUT);
            }

            chunkHeader.trim(); // remove \r

            // read size of chunk
            len = (uint32_t) strtol((const char *) chunkHeader.c_str(), NULL, 16);
            size += len;
            DEBUG_HTTPCLIENT("[HTTP-Client] read chunk len: %d\n", len);

            // data left?
            if(len > 0) {
                // read len bytes with timeout
                int r = _client->sendSize(output, len);
                if (_client->getLastSendReport() != Stream::Report::Success)
                    // not all data transferred
                    return returnError(StreamReportToHttpClientReport(_client->getLastSendReport()));
                ret += r;
            } else {

                // if no length Header use global chunk size
                if(_size <= 0) {
                    _size = size;
                }

                // check if we have write all data out
                if(ret != _size) {
                    return returnError(HTTPC_ERROR_STREAM_WRITE);
                }
                break;
            }

            // read trailing \r\n at the end of the chunk
            char buf[2];
            auto trailing_seq_len = _client->readBytes((uint8_t*)buf, 2);
            if (trailing_seq_len != 2 || buf[0] != '\r' || buf[1] != '\n') {
                return returnError(HTTPC_ERROR_READ_TIMEOUT);
            }

            esp_yield();
        }
    } else {
        return returnError(HTTPC_ERROR_ENCODING);
    }

    disconnect(true);
    return ret;
}

#endif /* ESP8266HTTPClient_H_ */
