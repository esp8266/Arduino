#include <string.h>
#include "server_drv.h"

extern "C" {
  #include "utility/debug.h"
}

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
        ServerDrv::startServer(_port, _sock);
        WiFiClass::_server_port[_sock] = _port;
    }
}

WiFiClient WiFiServer::available(byte* status)
{
    //accept();
	static byte tmp_cli_status = 0;
	static byte tmp_ser_status = 0;
	static int cycle = 0;

    for (int sock = 0; sock < MAX_SOCK_NUM; sock++)
    {
        if (WiFiClass::_server_port[sock] == _port)
        {
        	WiFiClient client(sock);
            uint8_t _status = client.status();
            uint8_t _ser_status = this->status();
            if ((tmp_cli_status != _status)||(tmp_ser_status != _ser_status))
            {
            	INFO("%d)Sock: %d Client Status: %d Server Status: %d port: %d", cycle, sock, _status, _ser_status, WiFiClass::_server_port[sock]);
            	tmp_cli_status = _status;
            	tmp_ser_status = _ser_status;
            	cycle = 0;
            }else{
            	++cycle;
            }
            if (status != NULL)
            	*status = _status;

            //server not in listen state, restart it
            if (this->status()==0)
            	ServerDrv::startServer(_port, sock);

            if (_status == ESTABLISHED)
            {                
                return client;  //TODO 
            }
        }
    }

    return WiFiClient(255);
}

uint8_t WiFiServer::status() {
    return ServerDrv::getServerState(0);
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
