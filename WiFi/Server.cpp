#include <string.h>
#include "Server.h"
#include "Client.h"
#include "WiFi.h"
#include "server_drv.h"
#include "wiring.h"

Server::Server(uint16_t port)
{
    _port = port;
}

void Server::begin()
{
    uint8_t _sock = WiFiClass::getSocket();
    if (_sock != NO_SOCKET_AVAIL)
    {
        ServerDrv::StartServer(_port, _sock);
        WiFiClass::_server_port[_sock] = _port;
    }
}

Client Server::available(byte* status)
{
    //accept();

    for (int sock = 0; sock < MAX_SOCK_NUM; sock++)
    {
        if (WiFiClass::_server_port[sock] != 0)
        {
            Client client(sock);
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

    return Client(255);
}

void Server::write(uint8_t b) 
{
    write(&b, 1);
}

void Server::write(const char *str) 
{
    write((const uint8_t *)str, strlen(str));
}

void Server::write(const uint8_t *buffer, size_t size) 
{
    for (int sock = 0; sock < MAX_SOCK_NUM; sock++)
    {
        if (WiFiClass::_server_port[sock] != 0)
        {
            Client client(sock);

            if (WiFiClass::_server_port[sock] == _port &&
                client.status() == ESTABLISHED)
            {                
                client.write(buffer, size);
            }else{
                delay(20);
            }

        }
    }
}
