
extern "C" {
  #include "utility/debug.h"
  #include "utility/wifi_spi.h"
}
#include <string.h>
#include "utility/server_drv.h"
#include "utility/wifi_drv.h"

#include "WiFi.h"
#include "WiFiUdp.h"
#include "WiFiClient.h"
#include "WiFiServer.h"


/* Constructor */
WiFiUDP::WiFiUDP() : _sock(NO_SOCKET_AVAIL) {}

/* Start WiFiUDP socket, listening at local port PORT */
uint8_t WiFiUDP::begin(uint16_t port) {

    uint8_t sock = WiFiClass::getSocket();
    if (sock != NO_SOCKET_AVAIL)
    {
        ServerDrv::startServer(port, sock, UDP_MODE);
        WiFiClass::_server_port[sock] = port;
        _sock = sock;
        _port = port;
        return 1;
    }
    return 0;

}

/* return number of bytes available in the current packet,
   will return zero if parsePacket hasn't been called yet */
int WiFiUDP::available() {
	 if (_sock != NO_SOCKET_AVAIL)
	 {
	      return ServerDrv::availData(_sock);
	 }
	 return 0;
}

/* Release any resources being used by this WiFiUDP instance */
void WiFiUDP::stop()
{
	  if (_sock == NO_SOCKET_AVAIL)
	    return;

	  ServerDrv::stopClient(_sock);

	  _sock = NO_SOCKET_AVAIL;
}

int WiFiUDP::beginPacket(const char *host, uint16_t port)
{
	// Look up the host first
	int ret = 0;
	IPAddress remote_addr;
	if (WiFi.hostByName(host, remote_addr))
	{
		return beginPacket(remote_addr, port);
	}
	return ret;
}

int WiFiUDP::beginPacket(IPAddress ip, uint16_t port)
{
  if (_sock == NO_SOCKET_AVAIL)
	  _sock = WiFiClass::getSocket();
  if (_sock != NO_SOCKET_AVAIL)
  {
	  ServerDrv::startClient(uint32_t(ip), port, _sock, UDP_MODE);
	  WiFiClass::_state[_sock] = _sock;
	  return 1;
  }
  return 0;
}

int WiFiUDP::endPacket()
{
	return ServerDrv::sendUdpData(_sock);
}

size_t WiFiUDP::write(uint8_t byte)
{
  return write(&byte, 1);
}

size_t WiFiUDP::write(const uint8_t *buffer, size_t size)
{
	ServerDrv::insertDataBuf(_sock, buffer, size);
	return size;
}

int WiFiUDP::parsePacket()
{
	return available();
}

int WiFiUDP::read()
{
  uint8_t b;
  if (available())
  {
	  ServerDrv::getData(_sock, &b);
  	  return b;
  }else{
	  return -1;
  }
}

int WiFiUDP::read(unsigned char* buffer, size_t len)
{
  if (available())
  {
	  uint16_t size = 0;
	  if (!ServerDrv::getDataBuf(_sock, buffer, &size))
		  return -1;
	  // TODO check if the buffer is too smal respect to buffer size
	  return size;
  }else{
	  return -1;
  }
}

int WiFiUDP::peek()
{
  uint8_t b;
  if (!available())
    return -1;

  ServerDrv::getData(_sock, &b, 1);
  return b;
}

void WiFiUDP::flush()
{
  while (available())
    read();
}

IPAddress  WiFiUDP::remoteIP()
{
	uint8_t _remoteIp[4] = {0};
	uint8_t _remotePort[2] = {0};

	WiFiDrv::getRemoteData(_sock, _remoteIp, _remotePort);
	IPAddress ip(_remoteIp);
	return ip;
}

uint16_t  WiFiUDP::remotePort()
{
	uint8_t _remoteIp[4] = {0};
	uint8_t _remotePort[2] = {0};

	WiFiDrv::getRemoteData(_sock, _remoteIp, _remotePort);
	uint16_t port = (_remotePort[0]<<8)+_remotePort[1];
	return port;
}

