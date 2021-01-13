#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <ESP8266WebServer.h>
#include <vector>
#include <assert.h>

namespace esp8266webserver {

template<typename ServerType>
class RequestHandler {
    using WebServerType = ESP8266WebServerTemplate<ServerType>;
public:
    virtual ~RequestHandler() { }
    virtual bool canHandle(HTTPMethod method, const String& uri) { (void) method; (void) uri; return false; }
    virtual bool canUpload(const String& uri) { (void) uri; return false; }
    virtual bool handle(WebServerType& server, HTTPMethod requestMethod, const String& requestUri) { (void) server; (void) requestMethod; (void) requestUri; return false; }
    virtual void upload(WebServerType& server, const String& requestUri, HTTPUpload& upload) { (void) server; (void) requestUri; (void) upload; }

    RequestHandler<ServerType>* next() { return _next; }
    void next(RequestHandler<ServerType>* r) { _next = r; }

private:
    RequestHandler<ServerType>* _next = nullptr;
	
protected:
    std::vector<String> pathArgs;

public:
    const String& pathArg(unsigned int i) { 
        assert(i < pathArgs.size());
        return pathArgs[i];
    }
};

} // namespace

#endif //REQUESTHANDLER_H
