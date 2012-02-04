extern "C" {
  #include "utility/wl_definitions.h"
  #include "utility/wl_types.h"
  #include "socket.h"
  #include "string.h"
  #include "utility/debug.h"
}

#include "WiFi.h"
#include "WiFiClient.h"
#include "WiFiServer.h"
#include "server_drv.h"


uint16_t WiFiClient::_srcport = 1024;

WiFiClient::WiFiClient() : _sock(MAX_SOCK_NUM) {
}

WiFiClient::WiFiClient(uint8_t sock) : _sock(sock) {
}

int WiFiClient::connect(const char* host, uint16_t port) {
	/* TODO Add DNS wifi spi function to resolve DNS */
#if 0
  // Look up the host first
  int ret = 0;
  DNSClient dns;
  IPAddress remote_addr;

  dns.begin(Ethernet.dnsServerIP());
  ret = dns.getHostByName(host, remote_addr);
  if (ret == 1) {
    return connect(remote_addr, port);
  } else {
    return ret;
  }
#endif
}

int WiFiClient::connect(IPAddress ip, uint16_t port) {
    _sock = getFirstSocket();
    if (_sock != NO_SOCKET_AVAIL)
    {
    	ServerDrv::startClient(uint32_t(ip), port, _sock);
    	 WiFiClass::_state[_sock] = _sock;
    }else{
    	return 0;
    }
    return 1;
}

size_t WiFiClient::write(uint8_t b) {
  if (_sock != 255)
  {
	  START();
      ServerDrv::sendData(_sock, &b, 1);
      while (!ServerDrv::isDataSent(_sock));
      END();
      return 1;
  }
  return 0;
}

size_t WiFiClient::write(const uint8_t *buf, size_t size) {
  if (_sock != 255)
  {
      ServerDrv::sendData(_sock, buf, size);
      while (!ServerDrv::isDataSent(_sock));
      return size;
  }
  return 0;
}

int WiFiClient::available() {
  if (_sock != 255)
  {
      return ServerDrv::availData(_sock);
  }
   
  return 0;
}

int WiFiClient::read() {
  uint8_t b;
  if (!available())
    return -1;
  ServerDrv::getData(_sock, &b);
  return b;
}


int WiFiClient::read(uint8_t* buf, size_t size) {
  if (!ServerDrv::getDataBuf(_sock, buf, &size))
      return -1;
  return 0;
}

int WiFiClient::peek() {
	//TODO to be implemented
	return 0;
}

void WiFiClient::flush() {
  while (available())
    read();
}

void WiFiClient::stop() {

	INFO("1)Stop WiFi client sock:%d state:%d status:%d", _sock, WiFiClass::_state[_sock], status());
  if (_sock == 255)
    return;

  ServerDrv::stopClient(_sock);

  unsigned long start = millis();
  
  // wait a second for the connection to close
  while (status() != CLOSED && millis() - start < 1000)
    delay(1);
    
  // if it hasn't closed, close it forcefully
  if (status() != CLOSED)
  {
	  //TODO force close
	  //close(_sock);
  }

  
  INFO("2)Stop WiFi client sock:%d state:%d status:%d", _sock, WiFiClass::_state[_sock], status());
  _sock = 255;
}

uint8_t WiFiClient::connected() {
  if (_sock == 255) {
    return 0;
  } else {
    uint8_t s = status();
    INFO("Client status: %d", s);
    return !(s == LISTEN || s == CLOSED || s == FIN_WAIT_1 || s == FIN_WAIT_2 ||
             (s == CLOSE_WAIT && !available()));
  }
}

uint8_t WiFiClient::status() {
    if (_sock == 255) {
    return CLOSED;
  } else {
    return ServerDrv::getClientState(_sock);
  }
}

WiFiClient::operator bool() {
  return _sock != 255;
}

// Private Methods
uint8_t WiFiClient::getFirstSocket()
{
    for (int i = 0; i < MAX_SOCK_NUM; i++) {
      if (WiFiClass::_state[i] == 0)
      {
          return i;
      }
    }
    return SOCK_NOT_AVAIL;
}

