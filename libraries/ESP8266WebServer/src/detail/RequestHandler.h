#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

template<typename ServerType, typename ClientType>
class RequestHandler {
public:
    virtual ~RequestHandler() { }
    virtual bool canHandle(HTTPMethod method, String uri) { (void) method; (void) uri; return false; }
    virtual bool canUpload(String uri) { (void) uri; return false; }
    virtual bool handle(ESP8266WebServerTemplate<ServerType, ClientType>& server, HTTPMethod requestMethod, String requestUri) { (void) server; (void) requestMethod; (void) requestUri; return false; }
    virtual void upload(ESP8266WebServerTemplate<ServerType, ClientType>& server, String requestUri, HTTPUpload& upload) { (void) server; (void) requestUri; (void) upload; }

    RequestHandler<ServerType, ClientType>* next() { return _next; }
    void next(RequestHandler<ServerType, ClientType>* r) { _next = r; }

private:
    RequestHandler<ServerType, ClientType>* _next = nullptr;
};

#endif //REQUESTHANDLER_H
