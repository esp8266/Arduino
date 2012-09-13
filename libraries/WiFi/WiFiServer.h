#ifndef wifiserver_h
#define wifiserver_h

extern "C" {
  #include "utility/wl_definitions.h"
}

#include "Server.h"

class WiFiClient;

class WiFiServer : public Server {
private:
  uint16_t _port;
  void*     pcb;
public:
  WiFiServer(uint16_t);
  WiFiClient available(uint8_t* status = NULL);
  void begin();
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  uint8_t status();

  using Print::write;
};

#endif
