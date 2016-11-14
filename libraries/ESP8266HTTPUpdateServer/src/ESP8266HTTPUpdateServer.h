#ifndef __HTTP_UPDATE_SERVER_H
#define __HTTP_UPDATE_SERVER_H

#include <functional>

class ESP8266WebServer;

class ESP8266HTTPUpdateServer
{
public:
    typedef std::function<void(void)> THandlerFunction;
    typedef std::function<void(int)> THandlerFunction_Error; // fixme enum?
    typedef std::function<void(unsigned int chunk, unsigned int total)> THandlerFunction_Progress;

    ESP8266HTTPUpdateServer(bool serial_debug = false);
    void setup(ESP8266WebServer *server)
    {
        setup(server, NULL, NULL);
    }

    void setup(ESP8266WebServer *server, const char * path)
    {
        setup(server, path, NULL, NULL);
    }

    void setup(ESP8266WebServer *server, const char * username, const char * password)
    {
        setup(server, "/update", username, password);
    }

    void setup(ESP8266WebServer *server, const char * path, const char * username, const char * password);

    //This callback will be called when an update starts
    void onStart(THandlerFunction fn);

    //This callback will be called when an update has finished
    void onEnd(THandlerFunction fn);

    //This callback will be called when an update encounters any error
    void onError(THandlerFunction_Error fn);

    //This callback will be called when an update is under way
    void onProgress(THandlerFunction_Progress fn);


private:
    bool _serial_output;
    ESP8266WebServer *_server;
    static const char *_serverIndex;
    static const char *_failedResponse;
    static const char *_successResponse;
    char * _username;
    char * _password;
    bool _authenticated;

    THandlerFunction _cb_start;
    THandlerFunction _cb_end;
    THandlerFunction_Error _cb_error;
    THandlerFunction_Progress _cb_progress;
protected:
    template<class ...Ts>
    void debug(const char *fmt, Ts... args);
    void handle_start();
    void handle_end();
    void handle_error(int);
    void handle_progress(unsigned int, unsigned int);

};


#endif
