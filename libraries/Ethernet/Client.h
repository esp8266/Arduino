#ifndef client_h
#define client_h
#include "Arduino.h"	
#include "Print.h"

class Client : public Stream {

public:
  Client();
  Client(uint8_t sock);
  Client(IPAddress& ip, uint16_t port);

  uint8_t status();
  uint8_t connect();
  virtual void write(uint8_t);
  virtual void write(const char *str);
  virtual void write(const uint8_t *buf, size_t size);
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
  IPAddress _ip;
  uint16_t _port;
};

#endif
