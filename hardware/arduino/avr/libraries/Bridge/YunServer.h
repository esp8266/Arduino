/*
  Copyright (c) 2013 Arduino LLC. All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#ifndef CONNECTOR_H_
#define CONNECTOR_H_

#include <Bridge.h>
#include <Server.h>
#include <Client.h>

class YunClient;

class YunServer : public Server {
public:
  // Constructor with a user provided BridgeClass instance
  YunServer(uint16_t port = 5555, BridgeClass &_b = Bridge);

  void begin();
  YunClient accept();

  virtual size_t write(uint8_t c) { /* TODO */ }

  void listenOnLocalhost()   { useLocalhost = true; }
  void noListenOnLocalhost() { useLocalhost = false; }

private:
  uint16_t port;
  bool listening;
  bool useLocalhost;
  BridgeClass &bridge;
};

class YunClient : public Client {
public:
  // Constructor with a user provided BridgeClass instance
  YunClient(int _h, BridgeClass &_b = Bridge);
  YunClient(BridgeClass &_b = Bridge);
  ~YunClient();
  
  // Stream methods 
  // (read message)
  virtual int available();
  virtual int read();
  virtual int read(uint8_t *buf, size_t size);
  virtual int peek();
  // (write response)
  virtual size_t write(uint8_t);
  virtual size_t write(const uint8_t *buf, size_t size);
  virtual void flush();
  // TODO: add optimized function for block write
  
  virtual operator bool () { return opened; }

  YunClient& operator=(const YunClient &_x);

  virtual void stop();
  virtual uint8_t connected();

  virtual int connect(IPAddress ip, uint16_t port) { /* TODO */ };
  virtual int connect(const char *host, uint16_t port) { /* TODO */ };

private:
  BridgeClass &bridge;
  unsigned int handle;
  boolean opened;

private:
  void doBuffer();
  uint8_t buffered;
  uint8_t readPos;
  static const int BUFFER_SIZE = 64;
  uint8_t buffer[BUFFER_SIZE];
  
};

#endif // CONNECTOR_H_
