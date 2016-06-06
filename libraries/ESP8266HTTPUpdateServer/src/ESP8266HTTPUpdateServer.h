#ifndef __HTTP_UPDATE_SERVER_H
#define __HTTP_UPDATE_SERVER_H

class ESP8266WebServer;

class ESP8266HTTPUpdateServer
{
  private:
    bool _serial_output;
    ESP8266WebServer *_server;
    static const char *_serverIndex;
    static const char *_failedResponse;
    static const char *_successResponse;
    char * _username;
    char * _password;
    bool _authenticated;
  public:
    ESP8266HTTPUpdateServer(bool serial_debug=false);

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
};


#endif
