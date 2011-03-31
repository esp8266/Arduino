#ifndef netclient_h
#define netclient_h
#include "WProgram.h"	
#include "Print.h"
#include "NetClient.h"
#include "IPAddress.h"

class NetClient : public Stream {

public:
  virtual int connect(IPAddress ip, uint16_t port) =0;
  virtual int connect(const char *host, uint16_t port) =0;
  virtual void write(uint8_t) =0;
  virtual void write(const char *str) =0;
  virtual void write(const uint8_t *buf, size_t size) =0;
  virtual int available() = 0;
  virtual int read() = 0;
  virtual int read(uint8_t *buf, size_t size) = 0;
  virtual int peek() = 0;
  virtual void flush() = 0;
  virtual void stop() = 0;
  virtual uint8_t connected() = 0;
  virtual operator bool() = 0;
};

#endif
