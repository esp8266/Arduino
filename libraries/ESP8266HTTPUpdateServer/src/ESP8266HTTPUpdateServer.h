#ifndef __HTTP_UPDATE_SERVER_H
#define __HTTP_UPDATE_SERVER_H

class ESP8266WebServer;

class ESP8266HTTPUpdateServer
{
  public:
    ESP8266HTTPUpdateServer(bool serial_debug=false);

    void setup(ESP8266WebServer *server)
    {
      setup(server, emptyString, emptyString);
    }

    void setup(ESP8266WebServer *server, const String& path)
    {
      setup(server, path, emptyString, emptyString);
    }

    void setup(ESP8266WebServer *server, const String& username, const String& password)
    {
      setup(server, "/update", username, password);
    }

    void setup(ESP8266WebServer *server, const String& path, const String& username, const String& password);

    void updateCredentials(const String& username, const String& password)
    {
      _username = username;
      _password = password;
    }

  protected:
    void _setUpdaterError();

  private:
    bool _serial_output;
    ESP8266WebServer *_server;
    String _username;
    String _password;
    bool _authenticated;
    String _updaterError;
};


#endif
