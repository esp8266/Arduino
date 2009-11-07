extern "C" {
  #include "types.h"
  #include "w5100.h"
  #include "socket.h"
  #include "string.h"
}

#include "WProgram.h"

#include "Ethernet.h"
#include "Client.h"
#include "Server.h"

uint16_t Client::_srcport = 0;

Client::Client(uint8_t sock) {
  _sock = sock;
}

Client::Client(uint8_t *ip, uint16_t port) {
  _ip = ip;
  _port = port;  
  _sock = 255;
}

uint8_t Client::connect() {
  if (_sock != 255)
    return 0;
  
  for (int i = 0; i < MAX_SOCK_NUM; i++) {
    uint8_t s = getSn_SR(i);
    if (s == SOCK_CLOSED || s == SOCK_FIN_WAIT) {
      _sock = i;
      break;
    }
  }
  
  if (_sock == 255)
    return 0;
    
  _srcport++;
  if (_srcport + 1024 == 0) _srcport = 0;
  socket(_sock, Sn_MR_TCP, _srcport + 1024, 0);
  
  if (!::connect(_sock, _ip, _port)) {
    _sock = 255;
    return 0;
  }
    
  while (status() != SOCK_ESTABLISHED) {
    delay(1);
    if (status() == SOCK_CLOSED) {
      _sock = 255;
      return 0;
    }
  }
  
  return 1;
}

void Client::write(uint8_t b) {
  if (_sock != 255)
    send(_sock, &b, 1);
}

void Client::write(const char *str) {
  if (_sock != 255)
    send(_sock, (const uint8_t *)str, strlen(str));
}

void Client::write(const uint8_t *buf, size_t size) {
  if (_sock != 255)
    send(_sock, buf, size);
}

int Client::available() {
  if (_sock != 255)
    return getSn_RX_RSR(_sock);
  return 0;
}

int Client::read() {
  uint8_t b;
  if (!available())
    return -1;
  recv(_sock, &b, 1);
  return b;
}

void Client::flush() {
  while (available())
    read();
}

void Client::stop() {
  if (_sock == 255)
    return;
  
  // attempt to close the connection gracefully (send a FIN to other side)
  disconnect(_sock);
  unsigned long start = millis();
  
  // wait a second for the connection to close
  while (status() != SOCK_CLOSED && millis() - start < 1000)
    delay(1);
    
  // if it hasn't closed, close it forcefully
  if (status() != SOCK_CLOSED)
    close(_sock);
  
  EthernetClass::_server_port[_sock] = 0;
  _sock = 255;
}

uint8_t Client::connected() {
  if (_sock == 255) {
    return 0;
  } else {
    uint8_t s = status();
    return !(s == SOCK_LISTEN || s == SOCK_CLOSED || s == SOCK_FIN_WAIT ||
      (s == SOCK_CLOSE_WAIT && !available()));
  }
}

uint8_t Client::status() {
  if (_sock == 255) {
    return SOCK_CLOSED;
  } else {
    return getSn_SR(_sock);
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
