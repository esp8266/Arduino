#ifndef Client_h
#define Client_h

#include "IPAddress.h"
#include "Print.h"

class Client : public Print {
public:
  Client(uint8_t sock);
  Client(IPAddress& ip, uint16_t port);

  uint8_t status();
  uint8_t connect();
  virtual void write(uint8_t);
  virtual void write(const char *str);
  virtual void write(const uint8_t *buf, size_t size);
  virtual int available();
  virtual int read();
  virtual int read(uint8_t* buf, size_t size);
  void flush();
  void stop();
  uint8_t connected();
  operator bool();

  friend class Server;

private:
  static uint16_t _srcport;
  uint8_t _sock;   //not used
  IPAddress _ip;
  uint16_t _port;
  uint16_t  _socket;

  uint8_t getFirstSocket();
};

#endif
