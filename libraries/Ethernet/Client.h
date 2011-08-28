#ifndef client_h
#define client_h
#include "Arduino.h"	
#include "Print.h"
#include "NetClient.h"
#include "IPAddress.h"

class Client : public NetClient {

public:
  Client();
  Client(uint8_t sock);

  uint8_t status();
  virtual int connect(IPAddress ip, uint16_t port);
  virtual int connect(const char *host, uint16_t port);
  virtual size_t write(uint8_t);
  virtual size_t write(const char *str);
  virtual size_t write(const uint8_t *buf, size_t size);
  virtual int available();
  virtual int read();
  virtual int read(uint8_t *buf, size_t size);
  virtual int peek();
  virtual void flush();
  virtual void stop();
  virtual uint8_t connected();
  virtual uint8_t operator==(int);
  virtual uint8_t operator!=(int);
  virtual operator bool();

  friend class Server;

private:
  static uint16_t _srcport;
  uint8_t _sock;
};

#endif
