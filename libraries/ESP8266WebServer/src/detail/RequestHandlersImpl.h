#ifndef REQUESTHANDLERSIMPL_H
#define REQUESTHANDLERSIMPL_H

#include <ESP8266WebServer.h>
#include "RequestHandler.h"
#include "mimetable.h"
#include "WString.h"
#include "Uri.h"

namespace esp8266webserver {

template<typename ServerType>
class FunctionRequestHandler : public RequestHandler<ServerType> {
    using WebServerType = ESP8266WebServerTemplate<ServerType>;
public:
    FunctionRequestHandler(typename WebServerType::THandlerFunction fn, typename WebServerType::THandlerFunction ufn, const Uri &uri, HTTPMethod method)
    : _fn(fn)
    , _ufn(ufn)
    , _uri(uri.clone())
    , _method(method)
    {
    }

    ~FunctionRequestHandler() {
        delete _uri;
    }

    bool canHandle(HTTPMethod requestMethod, const String& requestUri) override  {
        if (_method != HTTP_ANY && _method != requestMethod)
            return false;

        return _uri->canHandle(requestUri, RequestHandler<ServerType>::pathArgs);
    }

    bool canUpload(const String& requestUri) override  {
        if (!_ufn || !canHandle(HTTP_POST, requestUri))
            return false;

        return true;
    }

    bool handle(WebServerType& server, HTTPMethod requestMethod, const String& requestUri) override {
        (void) server;
        if (!canHandle(requestMethod, requestUri))
            return false;

        _fn();
        return true;
    }

    void upload(WebServerType& server, const String& requestUri, HTTPUpload& upload) override {
        (void) server;
        (void) upload;
        if (canUpload(requestUri))
            _ufn();
    }

protected:
    typename WebServerType::THandlerFunction _fn;
    typename WebServerType::THandlerFunction _ufn;
    Uri *_uri;
    HTTPMethod _method;
};

template<typename ServerType>
class StaticRequestHandler : public RequestHandler<ServerType> {
    using WebServerType = ESP8266WebServerTemplate<ServerType>;
public:
    StaticRequestHandler(FS& fs, const char* path, const char* uri, const char* cache_header)
    : _fs(fs)
    , _uri(uri)
    , _path(path)
    , _cache_header(cache_header)
    {
        if (fs.exists(path)) {
            File file = fs.open(path, "r");
            _isFile = file && file.isFile();
            file.close();
        }
        else {
            _isFile = false;
        }

        DEBUGV("StaticRequestHandler: path=%s uri=%s isFile=%d, cache_header=%s\r\n", path, uri, _isFile, cache_header == __null ? "" : cache_header);
        _baseUriLength = _uri.length();
    }

    bool canHandle(HTTPMethod requestMethod, const String& requestUri) override  {
        if ((requestMethod != HTTP_GET) && (requestMethod != HTTP_HEAD))
            return false;

        if ((_isFile && requestUri != _uri) || !requestUri.startsWith(_uri))
            return false;

        return true;
    }

    bool handle(WebServerType& server, HTTPMethod requestMethod, const String& requestUri) override {

        if (!canHandle(requestMethod, requestUri))
            return false;

        DEBUGV("StaticRequestHandler::handle: request=%s _uri=%s\r\n", requestUri.c_str(), _uri.c_str());

        String path;
        path.reserve(_path.length() + requestUri.length() + 32);
        path = _path;

        if (!_isFile) {

            // Append whatever follows this URI in request to get the file path.
            path += requestUri.substring(_baseUriLength);

            // Base URI doesn't point to a file.
            // If a directory is requested, look for index file.
            if (path.endsWith("/"))
                path += F("index.htm");

            // If neither <blah> nor <blah>.gz exist, and <blah> is a file.htm, try it with file.html instead
            // For the normal case this will give a search order of index.htm, index.htm.gz, index.html, index.html.gz
            if (!_fs.exists(path) && !_fs.exists(path + ".gz") && path.endsWith(".htm")) {
                path += 'l';
            }
        }
        DEBUGV("StaticRequestHandler::handle: path=%s, isFile=%d\r\n", path.c_str(), _isFile);

        String contentType = mime::getContentType(path);

        using namespace mime;
        // look for gz file, only if the original specified path is not a gz.  So part only works to send gzip via content encoding when a non compressed is asked for
        // if you point the the path to gzip you will serve the gzip as content type "application/x-gzip", not text or javascript etc...
        if (!path.endsWith(FPSTR(mimeTable[gz].endsWith)) && !_fs.exists(path))  {
            String pathWithGz = path + FPSTR(mimeTable[gz].endsWith);
            if(_fs.exists(pathWithGz))
                path += FPSTR(mimeTable[gz].endsWith);
        }

        File f = _fs.open(path, "r");
        if (!f)
            return false;

        if (!f.isFile()) {
            f.close();
            return false;
        }

        if (_cache_header.length() != 0)
            server.sendHeader("Cache-Control", _cache_header);

        server.streamFile(f, contentType, requestMethod);
        return true;
    }

    /* Deprecated version. Please use mime::getContentType instead */
    static String getContentType(const String& path) __attribute__((deprecated)) {
        return mime::getContentType(path);
    }

protected:
    FS _fs;
    String _uri;
    String _path;
    String _cache_header;
    bool _isFile;
    size_t _baseUriLength;
};

template<typename ServerType>
class StaticRequestETagHandler
    :
public StaticRequestHandler<ServerType> {

    using SRH = StaticRequestHandler<ServerType>;

    using WebServerType = ESP8266WebServerTemplate<ServerType>;

    protected:
    uint8_t _ETag_md5[16];
   
    void computeMD5(File toHash){
        MD5Builder calcMD5;
        calcMD5.begin();
        calcMD5.addStream(toHash, toHash.size());
        calcMD5.calculate();
        calcMD5.getBytes(_ETag_md5);
    }


    public:
    StaticRequestETagHandler(FS& fs, const char* path, const char* uri, const char* cache_header)
        :
    StaticRequestHandler<ServerType>{fs, path, uri, cache_header}
    {
        File f = fs.open(path, "r");
        if(f){
            if(f.isFile())
                computeMD5(f);
            f.close();
        }

    }

    bool handle(WebServerType& server, HTTPMethod requestMethod, const String & requestUri) override {
        if (!SRH::canHandle(requestMethod, requestUri)){
            return false;
        }

        const String etag = "\"" + base64::encode(_ETag_md5, 16, false) + "\"";

        if(server.header("If-None-Match") == etag){
            server.send(304);
            return true;
        }

        File f = SRH::_fs.open(SRH::_path, "r");

        if (!f)
            return false;

        if (!f.isFile()) {
            f.close();
            return false;
        }

        if (SRH::_cache_header.length() != 0)
            server.sendHeader("Cache-Control", SRH::_cache_header);

        server.sendHeader("ETag", etag);


        server.streamFile(f, SRH::getContentType(SRH::_path), requestMethod);
        return true;

    }

    // bool handle(WebServerType& server, HTTPMethod requestMethod, const String & requestUri) override {
    //     if (!SRH::canHandle(requestMethod, requestUri)){
    //         return false;
    //     }

    //     File f = SRH::_fs.open(SRH::_path, "r");

    //     if (!f)
    //         return false;

    //     if (!f.isFile()) {
    //         f.close();
    //         return false;
    //     }

    //     if(last_edit == f.getLastWrite()){
    //         //update chache
    //     }

    //     const String etag = "\"" + base64::encode(buff, 16, false) + "\"";

    //     if(server.header("If-None-Match") == etag){
    //         server.send(304);
    //         return true;
    //     }

    //     if (_cache_header.length() != 0)
    //         server.sendHeader("Cache-Control", SRH::_cache_header);

    //     server.sendHeader("ETag", etag);


    //     server.streamFile(f, SRH::getContentType(SRH::_path), requestMethod);
    //     return true;

    // }
        
};

} // namespace

#endif //REQUESTHANDLERSIMPL_H