#ifndef Server_Drv_h
#define Server_Drv_h

#include <inttypes.h>
#include "wifi_spi.h"

class ServerDrv
{
public:
    // Start server TCP on port specified
    static void startServer(uint16_t port, uint8_t sock);

    static void startClient(uint32_t ipAddress, uint16_t port, uint8_t sock);

    static void stopClient(uint8_t sock);
                                                                                  
    static uint8_t getServerState(uint8_t sock);

    static uint8_t getClientState(uint8_t sock);

    static bool getData(uint8_t sock, uint8_t *data, uint8_t peek = 0);

    static bool getDataBuf(uint8_t sock, uint8_t *data, uint16_t *len);

    static bool sendData(uint8_t sock, const uint8_t *data, uint16_t len);

    static uint8_t availData(uint8_t sock);

    static uint8_t checkDataSent(uint8_t sock);
};

extern ServerDrv serverDrv;

#endif
