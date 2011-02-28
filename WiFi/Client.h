#ifndef Client_h
#define Client_h

#include "Print.h"

class Client : public Print {
private:
  static uint16_t _srcport;
  uint8_t _sock;   //not used
  uint8_t *_ip;
  uint16_t _port;
  uint16_t  _socket;

public:
  Client(uint8_t);
  Client(uint8_t *, uint16_t);
  uint8_t status();
  uint8_t connect();
  virtual void write(uint8_t);
  virtual void write(const char *str);
  virtual void write(const uint8_t *buf, size_t size);
  int available();
  int read();
  int readBuf(uint8_t* buf, uint16_t* len); 
  void flush();
  void stop();
  uint8_t connected();
  uint8_t operator==(int);
  uint8_t operator!=(int);
  operator bool();
  uint8_t getFirstSock();
  friend class Server;
};

#endif
