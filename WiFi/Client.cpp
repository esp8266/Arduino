extern "C" {
  #include "wl_types.h"
  #include "socket.h"
  #include "string.h"
}

#include "WProgram.h"

#include "WiFi.h"
#include "Client.h"
#include "Server.h"
#include "server_drv.h"

uint16_t Client::_srcport = 0;

Client::Client(uint8_t sock) {
  _sock = sock;
}

Client::Client(uint8_t *ip, uint16_t port) {
  _ip = ip;
  _port = port;  
  _sock = 255;
}

uint8_t Client::getFirstSock()
{
    for (int i = 0; i < MAX_SOCK_NUM; i++) {
      if (WiFiClass::_state[i] < 0)
      {
          return i;
      }
    }
    return SOCK_NOT_AVAIL;
}

uint8_t Client::connect() {
    _sock = getFirstSock();

//    _srcport++;
//  if (_srcport + 1024 == 0) _srcport = 0;
    _socket = socket(TCP_SOCKET);
    if (_socket<0)
    {
        return 0; 
    }else{
        WiFiClass::_state[_sock] = _socket;
    }
  
  if (!::connect(_socket, _ip, _port)) {
    return 0;
  }
  return 1;
}

void Client::write(uint8_t b) {
  if (_sock != 255)
  {
      while (!ServerDrv::isDataSent(_sock));
      ServerDrv::sendData(_sock, &b, 1);
  }
}

void Client::write(const char *str) {
  if (_sock != 255)
  {
      while (!ServerDrv::isDataSent(_sock));
      unsigned int len = strlen(str);
      ServerDrv::sendData(_sock, (const uint8_t *)str, len);
  }
}

void Client::write(const uint8_t *buf, size_t size) {
  if (_sock != 255)
  {
      while (!ServerDrv::isDataSent(_sock));
      ServerDrv::sendData(_sock, buf, size);
  }
  
}

int Client::available() {
  if (_sock != 255)
  {
      return ServerDrv::availData(_sock);
  }
   
  return 0;
}

int Client::read() {
  uint8_t b;
  if (!available())
    return -1;
  ServerDrv::getData(_sock, &b);
  return b;
}


int Client::readBuf(uint8_t* buf, uint16_t* len) {
  if (!ServerDrv::getDataBuf(_sock, buf, len))
      return -1;
  return 0;
}

void Client::flush() {
  while (available())
    read();
}

void Client::stop() {
  if (_sock == 255)
    return;
  
  // attempt to close the connection gracefully (send a FIN to other side)
  disconnect(WiFiClass::_state[_sock]);
  unsigned long start = millis();
  
  // wait a second for the connection to close
  while (status() != CLOSED && millis() - start < 1000)
    delay(1);
    
  // if it hasn't closed, close it forcefully
  if (status() != CLOSED)
    close(_sock);
  
  WiFiClass::_server_port[_sock] = 0;
  _sock = 255;
}

uint8_t Client::connected() {
  if (_sock == 255) {
    return 0;
  } else {
    uint8_t s = status();
    return !(s == LISTEN || s == CLOSED || s == FIN_WAIT_1 || s == FIN_WAIT_2 ||
             (s == CLOSE_WAIT && !available()));
  }
}

uint8_t Client::status() {
    if (_sock == 255) {
    return CLOSED;
  } else {
    return ServerDrv::getState(_sock);
  }
}

// the next three functions are a hack so we can compare the client returned
// by Server::available() to null, or use it as the condition in an
// if-statement.  this lets us stay compatible with the Processing network
// library.

uint8_t Client::operator==(int p) {
  return _sock == 255;
}

uint8_t Client::operator!=(int p) {
  return _sock != 255;
}

Client::operator bool() {
  return _sock != 255;
}
