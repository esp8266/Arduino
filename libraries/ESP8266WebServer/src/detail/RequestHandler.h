#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

template<typename ServerType>
class RequestHandler {
public:
    virtual ~RequestHandler() { }
    virtual bool canHandle(HTTPMethod method, String uri) { (void) method; (void) uri; return false; }
    virtual bool canUpload(String uri) { (void) uri; return false; }
    virtual bool handle(ESP8266WebServerTemplate<ServerType>& server, HTTPMethod requestMethod, String requestUri) { (void) server; (void) requestMethod; (void) requestUri; return false; }
    virtual void upload(ESP8266WebServerTemplate<ServerType>& server, String requestUri, HTTPUpload& upload) { (void) server; (void) requestUri; (void) upload; }

    RequestHandler<ServerType>* next() { return _next; }
    void next(RequestHandler<ServerType>* r) { _next = r; }

private:
    RequestHandler<ServerType>* _next = nullptr;
};

#endif //REQUESTHANDLER_H
