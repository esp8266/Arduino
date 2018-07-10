#ifndef __HTTP_UPDATE_SERVER_H
#define __HTTP_UPDATE_SERVER_H

#include <ESP8266WebServer.h>

template<class ServerClass, class ClientClass>
class ESP8266HTTPUpdateServerTemplate
{
  public:
    ESP8266HTTPUpdateServerTemplate(bool serial_debug=false);

    void setup(ESP8266WebServerTemplate<ServerClass, ClientClass> *server)
    {
      setup(server, NULL, NULL);
    }

    void setup(ESP8266WebServerTemplate<ServerClass, ClientClass> *server, const char * path)
    {
      setup(server, path, NULL, NULL);
    }

    void setup(ESP8266WebServerTemplate<ServerClass, ClientClass> *server, const char * username, const char * password)
    {
      setup(server, "/update", username, password);
    }

    void setup(ESP8266WebServerTemplate<ServerClass, ClientClass> *server, const char * path, const char * username, const char * password);

    void updateCredentials(const char * username, const char * password)
    {
      _username = (char *)username;
      _password = (char *)password;
    }

  protected:
    void _setUpdaterError();

  private:
    bool _serial_output;
    ESP8266WebServerTemplate<ServerClass, ClientClass> *_server;
    char * _username;
    char * _password;
    bool _authenticated;
    String _updaterError;
};

#include "ESP8266HTTPUpdateServer-impl.h"

typedef ESP8266HTTPUpdateServerTemplate<WiFiServer, WiFiClient> ESP8266HTTPUpdateServer;

#endif
