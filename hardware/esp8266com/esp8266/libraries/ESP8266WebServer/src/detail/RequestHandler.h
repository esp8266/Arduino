#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

class RequestHandler {
public:
  RequestHandler(const char* uri, HTTPMethod method)
  : uri(uri)
  , method(method)
  , next(NULL)
  {
  }

  virtual bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) = 0;

  RequestHandler* next;

protected:
  String uri;
  HTTPMethod method;
};


class FunctionRequestHandler : public RequestHandler {
    typedef RequestHandler base;

public:
    FunctionRequestHandler(ESP8266WebServer::THandlerFunction fn, const char* uri, HTTPMethod method)
    : fn(fn)
    , base(uri, method)
    {
    }

    bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) override {
        if (method != HTTP_ANY && method != requestMethod)
            return false;

        if (requestUri != uri)
            return false;

        fn();
        return true;
    }

protected:
    ESP8266WebServer::THandlerFunction fn;
};

class StaticRequestHandler : public RequestHandler {
    typedef RequestHandler base;

public:
    StaticRequestHandler(FS& fs, const char* uri)
    : fs(fs)
    , base(uri, HTTP_GET)
    {
    }

    bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) override {
        if (requestMethod != method)
            return false;
        DEBUGV("StaticRequestHandler::handle: %s\r\n", requestUri.c_str());
        if (!requestUri.startsWith(uri))
            return false;

        auto prefixLength = uri.length();
        String path = requestUri.substring(0, prefixLength);
        DEBUGV("StaticRequestHandler::handle: %d %s\r\n", prefixLength, path.c_str());
        File f = fs.open(path, "r");
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
    FS fs;
};

#endif //REQUESTHANDLER_H
