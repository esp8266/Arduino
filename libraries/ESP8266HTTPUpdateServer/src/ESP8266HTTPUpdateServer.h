#ifndef __HTTP_UPDATE_SERVER_H
#define __HTTP_UPDATE_SERVER_H

class ESP8266WebServer;

class ESP8266HTTPUpdateServer
{
  private:
    bool _serial_output;
    ESP8266WebServer *_server;
    static const char *_serverIndex;
  public:
    ESP8266HTTPUpdateServer(bool serial_debug=false);
    void setup(ESP8266WebServer *server=NULL);
};


#endif
