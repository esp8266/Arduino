#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

class RequestHandler {
public:
    virtual ~RequestHandler() { }
    virtual bool canHandle(HTTPMethod method, String uri) { return false; }
    virtual bool canUpload(String uri) { return false; }
    virtual bool handle(ESP8266WebServer& server, HTTPMethod requestMethod, String requestUri) { return false; }
    virtual void upload(ESP8266WebServer& server, String requestUri, HTTPUpload& upload) {}

    RequestHandler* next() { return _next; }
    void next(RequestHandler* r) { _next = r; }

private:
    RequestHandler* _next = nullptr;
};

#endif //REQUESTHANDLER_H
