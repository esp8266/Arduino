#ifndef REQUESTHANDLERSIMPL_H
#define REQUESTHANDLERSIMPL_H

#include "RequestHandler.h"

class FunctionRequestHandler : public RequestHandler {
public:
    FunctionRequestHandler(ESP8266WebServer::THandlerFunction fn, const char* uri, HTTPMethod method)
    : _fn(fn)
    , _uri(uri)
    , _method(method)
    {
    }

    bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) override {
        if (_method != HTTP_ANY && _method != requestMethod)
            return false;

        if (requestUri != _uri)
            return false;

        _fn();
        return true;
    }

protected:
    String _uri;
    HTTPMethod _method;
    ESP8266WebServer::THandlerFunction _fn;
};

class StaticRequestHandler : public RequestHandler {
public:
    StaticRequestHandler(FS& fs, const char* path, const char* uri)
    : _fs(fs)
    , _uri(uri)
    , _path(path)
    {
        _isFile = fs.exists(path);
        DEBUGV("StaticRequestHandler: path=%s uri=%s isFile=%d\r\n", path, uri, _isFile);
        _baseUriLength = _uri.length();
    }

    bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) override {
        if (requestMethod != HTTP_GET)
            return false;
        DEBUGV("StaticRequestHandler::handle: request=%s _uri=%s\r\n", requestUri.c_str(), _uri.c_str());
        if (!requestUri.startsWith(_uri))
            return false;

        String path(_path);
        if (!_isFile) {
            // Base URI doesn't point to a file. Append whatever follows this
            // URI in request to get the file path.
            path += requestUri.substring(_baseUriLength);
        }
        else if (requestUri != _uri) {
            // Base URI points to a file but request doesn't match this URI exactly
            return false;
        }
        DEBUGV("StaticRequestHandler::handle: path=%s, isFile=%d\r\n", path.c_str(), _isFile);
        File f = _fs.open(path, "r");
        if (!f)
            return false;

        server.streamFile(f, getContentType(path));
        return true;
    }

    static String getContentType(const String& path) {
        if (path.endsWith(".html")) return "text/html";
        else if (path.endsWith(".htm")) return "text/html";
        else if (path.endsWith(".css")) return "text/css";
        else if (path.endsWith(".txt")) return "text/plain";
        else if (path.endsWith(".js")) return "application/javascript";
        else if (path.endsWith(".png")) return "image/png";
        else if (path.endsWith(".gif")) return "image/gif";
        else if (path.endsWith(".jpg")) return "image/jpeg";
        else if (path.endsWith(".ico")) return "image/x-icon";
        else if (path.endsWith(".xml")) return "text/xml";
        else if (path.endsWith(".pdf")) return "application/pdf";
        else if (path.endsWith(".zip")) return "application/zip";
        return "text/plain";
    }

protected:
    FS _fs;
    String _uri;
    String _path;
    bool _isFile;
    size_t _baseUriLength;
};


#endif //REQUESTHANDLERSIMPL_H
