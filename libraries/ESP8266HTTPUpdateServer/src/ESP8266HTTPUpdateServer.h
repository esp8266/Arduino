#ifndef __HTTP_UPDATE_SERVER_H
#define __HTTP_UPDATE_SERVER_H

#include <ESP8266WebServer.h>

namespace esp8266httpupdateserver {
using namespace esp8266webserver;

template <typename ServerType>
class ESP8266HTTPUpdateServerTemplate
{
  public:
    ESP8266HTTPUpdateServerTemplate(bool serial_debug=false);

    void setup(ESP8266WebServerTemplate<ServerType> *server)
    {
      setup(server, emptyString, emptyString);
    }

    void setup(ESP8266WebServerTemplate<ServerType> *server, const String& path)
    {
      setup(server, path, emptyString, emptyString);
    }

    void setup(ESP8266WebServerTemplate<ServerType> *server, const String& username, const String& password)
    {
      setup(server, "/update", username, password);
    }

    void setup(ESP8266WebServerTemplate<ServerType> *server, const String& path, const String& username, const String& password);

    void updateCredentials(const String& username, const String& password)
    {
      _username = username;
      _password = password;
    }

    void rebootOnUpdate(bool reboot)
    {
      _rebootOnUpdate = reboot;
    }

  protected:
    void _setUpdaterError();

  private:
    bool _serial_output;
    ESP8266WebServerTemplate<ServerType> *_server;
    String _username;
    String _password;
    bool _authenticated;
    String _updaterError;
    bool _rebootOnUpdate = true;
};

};

#include "ESP8266HTTPUpdateServer-impl.h"


using ESP8266HTTPUpdateServer = esp8266httpupdateserver::ESP8266HTTPUpdateServerTemplate<WiFiServer>;

namespace BearSSL {
using ESP8266HTTPUpdateServerSecure = esp8266httpupdateserver::ESP8266HTTPUpdateServerTemplate<WiFiServerSecure>;
};

#endif
