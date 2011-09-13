#include <string.h>
#include "server_drv.h"

#include "WiFi.h"
#include "WiFiClient.h"
#include "WiFiServer.h"



WiFiServer::WiFiServer(uint16_t port)
{
    _port = port;
}

void WiFiServer::begin()
{
    uint8_t _sock = WiFiClass::getSocket();
    if (_sock != NO_SOCKET_AVAIL)
    {
        ServerDrv::StartServer(_port, _sock);
        WiFiClass::_server_port[_sock] = _port;
    }
}

WiFiClient WiFiServer::available(byte* status)
{
    //accept();

    for (int sock = 0; sock < MAX_SOCK_NUM; sock++)
    {
        if (WiFiClass::_server_port[sock] != 0)
        {
        	WiFiClient client(sock);
            int _status = client.status();
            if (status != NULL)
            	*status = _status;

            if (WiFiClass::_server_port[sock] == _port &&
                _status == ESTABLISHED)
            {                
                return client;  //TODO 
            }
        }
    }

    return WiFiClient(255);
}

size_t WiFiServer::write(uint8_t b)
{
    return write(&b, 1);
}

size_t WiFiServer::write(const uint8_t *buffer, size_t size)
{
	size_t n = 0;

    for (int sock = 0; sock < MAX_SOCK_NUM; sock++)
    {
        if (WiFiClass::_server_port[sock] != 0)
        {
        	WiFiClient client(sock);

            if (WiFiClass::_server_port[sock] == _port &&
                client.status() == ESTABLISHED)
            {                
                n+=client.write(buffer, size);
            }
        }
    }
    return n;
}
