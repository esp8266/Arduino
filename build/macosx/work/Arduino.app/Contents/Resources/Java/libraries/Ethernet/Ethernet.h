#ifndef ethernet_h
#define ethernet_h

#include <inttypes.h>
//#include "w5100.h"
#include "Client.h"
#include "Server.h"

#define MAX_SOCK_NUM 4

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
