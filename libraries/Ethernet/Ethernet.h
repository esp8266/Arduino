#ifndef Ethernet_h
#define Ethernet_h

#include <inttypes.h>
#include "Client.h"
#include "Server.h"

class EthernetClass {
private:
public:
  static uint8_t _state[MAX_SOCK_NUM];
  static uint16_t _server_port[MAX_SOCK_NUM];
  void begin(uint8_t *, uint8_t *);
  void begin(uint8_t *, uint8_t *, uint8_t *);
  void begin(uint8_t *, uint8_t *, uint8_t *, uint8_t *);
  friend class Client;
  friend class Server;
};

extern EthernetClass Ethernet;

#endif
