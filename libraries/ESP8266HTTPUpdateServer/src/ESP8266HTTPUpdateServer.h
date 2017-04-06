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
    const char * _username;
    const char * _password;
	const char * _hash;
    bool _authenticated;
  public:
    ESP8266HTTPUpdateServer(bool serial_debug=false);

    bool setLoginPassword(const char * username, const char * password);
	
	bool setLoginPasswordHASH(const char * hashUNP);
	
	void setup(ESP8266WebServer *server, const char * path);

	void setup(ESP8266WebServer *server)
    { 
      setup(server, "/update");
    }
	
};


#endif
