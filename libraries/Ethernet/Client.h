#ifndef client_h
#define client_h
#include "WProgram.h"	
#include "Print.h"

class Client : public Print {

public:
  Client();
  Client(uint8_t);
  Client(uint8_t *, uint16_t);

  uint8_t status();
  uint8_t connect();
  virtual void write(uint8_t);
  virtual void write(const char *str);
  virtual void write(const uint8_t *buf, size_t size);
  int available();
  int read();
  void flush();
  void stop();
  uint8_t connected();
  uint8_t operator==(int);
  uint8_t operator!=(int);
  operator bool();

  friend class Server;

private:
  static uint16_t _srcport;
  bool _connected;
  uint8_t _sock;
  uint8_t *_ip;
  uint16_t _port;
};

#endif
