#ifndef __HTTP_UPDATE_SERVER_H
#define __HTTP_UPDATE_SERVER_H

class ESP8266WebServer;

class ESP8266HTTPUpdateServer
{
  public:
    ESP8266HTTPUpdateServer(bool serial_debug=false);

    bool setLoginPassword(const char * username, const char * password);
	
	bool setLoginPasswordHASH(const char * hashUNP);
	
	void setup(ESP8266WebServer *server, const char * path);

	void setup(ESP8266WebServer *server)
    { 
      setup(server, "/update");
    }

    void setup(ESP8266WebServer *server, const char * path);
  
  protected:
    void _setUpdaterError();

  private:
    bool _serial_output;
    ESP8266WebServer *_server;
    char * _username;
    char * _password;
    char * _hash;
    bool _authenticated;
    String _updaterError;

};


#endif
