#ifndef client_h
#define client_h
#include "Arduino.h"	
#include "Print.h"

class Client : public Stream {

public:
  Client();
  Client(uint8_t sock);

  uint8_t status();
  int connect(IPAddress ip, uint16_t port);
  int connect(const char *host, uint16_t port);
  virtual long write(uint8_t);
  virtual long write(const char *str);
  virtual long write(const uint8_t *buf, size_t size);
  virtual int available();
  virtual int read();
  virtual int read(uint8_t *buf, size_t size);
  virtual int peek();
  virtual void flush();
  void stop();
  uint8_t connected();
  operator bool();

  friend class Server;

private:
  static uint16_t _srcport;
  uint8_t _sock;
};

#endif
