#ifndef server_h
#define server_h

#include "NetServer.h"

class Client;

class Server : 
public NetServer {
private:
  uint16_t _port;
  void accept();
public:
  Server(uint16_t);
  Client available();
  virtual void begin();
  virtual size_t write(uint8_t);
  virtual size_t write(const char *str);
  virtual size_t write(const uint8_t *buf, size_t size);
};

#endif
