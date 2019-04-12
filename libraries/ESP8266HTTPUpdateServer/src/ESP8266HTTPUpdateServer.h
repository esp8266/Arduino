#ifndef __HTTP_UPDATE_SERVER_H
#define __HTTP_UPDATE_SERVER_H

#include <ESP8266WebServer.h>

template <typename ServerType, typename ClientType>
class ESP8266HTTPUpdateServerTemplate
{
  public:
    ESP8266HTTPUpdateServerTemplate(bool serial_debug=false);

    void setup(ESP8266WebServerTemplate<ServerType, ClientType> *server)
    {
      setup(server, emptyString, emptyString);
    }

    void setup(ESP8266WebServerTemplate<ServerType, ClientType> *server, const String& path)
    {
      setup(server, path, emptyString, emptyString);
    }

    void setup(ESP8266WebServerTemplate<ServerType, ClientType> *server, const String& username, const String& password)
    {
      setup(server, "/update", username, password);
    }

    void setup(ESP8266WebServerTemplate<ServerType, ClientType> *server, const String& path, const String& username, const String& password);

    void updateCredentials(const String& username, const String& password)
    {
      _username = username;
      _password = password;
    }

  protected:
    void _setUpdaterError();

  private:
    bool _serial_output;
    ESP8266WebServerTemplate<ServerType, ClientType> *_server;
    String _username;
    String _password;
    bool _authenticated;
    String _updaterError;
};

#include "ESP8266HTTPUpdateServer-impl.h"


using ESP8266HTTPUpdateServer = ESP8266HTTPUpdateServerTemplate<WiFiServer, WiFiClient>;

namespace BearSSL {
using ESP8266HTTPUpdateServerSecure = ESP8266HTTPUpdateServerTemplate<WiFiServerSecure, WiFiClientSecure>;
};

namespace axTLS {
using ESP8266HTTPUpdateServerSecure = ESP8266HTTPUpdateServerTemplate<WiFiServerSecure, WiFiClientSecure>;
};

#endif
