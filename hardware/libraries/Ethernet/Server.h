#ifndef Server_h
#define Server_h

extern "C" {
  #include "utility/types.h"
}

#include "Print.h"

class Client;

class Server : public Print {
private:
  uint16_t _port;
  void accept();
public:
  Server(uint16_t);
  Client available();
  void begin();
  virtual void write(uint8_t);
};

#endif
