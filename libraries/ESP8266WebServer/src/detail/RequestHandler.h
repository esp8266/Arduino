#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include <ESP8266WebServer.h>
#include <vector>

namespace esp8266webserver {

template<typename ServerType>
class RequestHandler {
    using WebServerType = ESP8266WebServerTemplate<ServerType>;
public:
    virtual ~RequestHandler() { }

    /*
        note: old handler API for backward compatibility
    */

    virtual bool canHandle(HTTPMethod method, const String& uri) {
        (void) method;
        (void) uri;
        return false;
    }
    virtual bool canUpload(const String& uri) {
        (void) uri;
        return false;
    }

    /*
        note: new handler API with support for filters etc.
    */

    virtual bool canHandle(WebServerType& server, HTTPMethod method, const String& uri) {
        (void) server;
        (void) method;
        (void) uri;
        return false;
    }
    virtual bool canUpload(WebServerType& server, const String& uri) {
        (void) server;
        (void) uri;
        return false;
    }
    virtual bool handle(WebServerType& server, HTTPMethod requestMethod, const String& requestUri) {
        (void) server;
        (void) requestMethod;
        (void) requestUri;
        return false;
    }
    virtual void upload(WebServerType& server, const String& requestUri, HTTPUpload& upload) {
        (void) server;
        (void) requestUri;
        (void) upload;
    }

    RequestHandler<ServerType>* next() {
        return _next;
    }

    void next(RequestHandler<ServerType>* r) {
        _next = r;
    }

    virtual RequestHandler<ServerType>& setFilter(std::function<bool(ESP8266WebServerTemplate<ServerType>&)> filter) {
        (void)filter;
        return *this;
    }

private:
    RequestHandler<ServerType>* _next = nullptr;
	
protected:
    std::vector<String> pathArgs;

public:
    size_t pathArgsSize() const {
        return pathArgs.size();
    }

    const String& pathArg(unsigned int i) const {
        return pathArgs[i];
    }
};

} // namespace

#endif //REQUESTHANDLER_H
