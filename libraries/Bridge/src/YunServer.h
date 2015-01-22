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

#ifndef _YUN_SERVER_H_
#define _YUN_SERVER_H_

#include <Bridge.h>
#include <Server.h>

class YunClient;

class YunServer : public Server {
  public:
    // Constructor with a user provided BridgeClass instance
    YunServer(uint16_t port = 5555, BridgeClass &_b = Bridge);

    void begin();
    YunClient accept();

    virtual size_t write(uint8_t c);

    void listenOnLocalhost()   {
      useLocalhost = true;
    }
    void noListenOnLocalhost() {
      useLocalhost = false;
    }

  private:
    uint16_t port;
    bool listening;
    bool useLocalhost;
    BridgeClass &bridge;
};

#endif // _YUN_SERVER_H_
