/*
  server_drv.h - Library for Arduino Wifi shield.
  Copyright (c) 2011-2014 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef Server_Drv_h
#define Server_Drv_h

#include <inttypes.h>
#include "wifi_spi.h"

typedef enum eProtMode {TCP_MODE, UDP_MODE}tProtMode;

class ServerDrv
{
public:

    // Start server TCP on port specified
    static void startServer(uint16_t port, uint8_t sock, uint8_t protMode=TCP_MODE);

    static void startClient(uint32_t ipAddress, uint16_t port, uint8_t sock, uint8_t protMode=TCP_MODE);

    static void stopClient(uint8_t sock);
                                                                                  
    static uint8_t getServerState(uint8_t sock);

    static uint8_t getClientState(uint8_t sock);

    static bool getData(uint8_t sock, uint8_t *data, uint8_t peek = 0);

    static bool getDataBuf(uint8_t sock, uint8_t *data, uint16_t *len);

    static bool insertDataBuf(uint8_t sock, const uint8_t *_data, uint16_t _dataLen);

    static bool sendData(uint8_t sock, const uint8_t *data, uint16_t len);

    static bool sendUdpData(uint8_t sock);

    static uint16_t availData(uint8_t sock);

    static uint8_t checkDataSent(uint8_t sock);
};

extern ServerDrv serverDrv;

#endif
