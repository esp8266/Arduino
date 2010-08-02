#include "w5100.h"
#include "socket.h"
extern "C" {
#include "string.h"
}

#include "WProgram.h"

#include "ethernet.h"
#include "client.h"
#include "server.h"

uint16_t Client::_srcport = 1024;

Client::Client() : _connected(false) {
}

Client::Client(uint8_t sock) : _connected(true), _sock(sock) {
}

Client::Client(uint8_t *ip, uint16_t port) : _ip(ip), _port(port), _connected(false) {
}

uint8_t Client::connect() {
  if (_connected)
    return 0;

  int i;
  for (i=0; i<MAX_SOCK_NUM; i++) {
    uint8_t s = W5100.readSnSR(i);
    if (s == SnSR::CLOSED || s == SnSR::FIN_WAIT) {
      _sock = i;
      break;
    }
  }

  if (i == MAX_SOCK_NUM)
    return 0;

  _srcport++;
  if (_srcport == 0) _srcport = 1024;
  socket(_sock, SnMR::TCP, _srcport, 0);

  if (!::connect(_sock, _ip, _port))
    return 0;

  while (status() != SnSR::ESTABLISHED) {
    delay(1);
    if (status() == SnSR::CLOSED)
      return 0;
  }

  _connected = true;
  return 1;
}

void Client::write(uint8_t b) {
  if (_connected)
    send(_sock, &b, 1);
}

void Client::write(const char *str) {
  if (_connected)
    send(_sock, (const uint8_t *)str, strlen(str));
}

void Client::write(const uint8_t *buf, size_t size) {
  if (_connected)
    send(_sock, buf, size);
}

int Client::available() {
  if (_connected)
    return W5100.getRXReceivedSize(_sock);
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
  if (!_connected)
    return;

  // attempt to close the connection gracefully (send a FIN to other side)
  disconnect(_sock);
  unsigned long start = millis();

  // wait a second for the connection to close
  while (status() != SnSR::CLOSED && millis() - start < 1000)
    delay(1);

  // if it hasn't closed, close it forcefully
  if (status() != SnSR::CLOSED)
    close(_sock);

  EthernetClass::_server_port[_sock] = 0;
  _connected = false;
}

uint8_t Client::connected() {
  uint8_t s = status();
  return !(s == SnSR::LISTEN || s == SnSR::CLOSED || s == SnSR::FIN_WAIT ||
    (s == SnSR::CLOSE_WAIT && !available()));
}

uint8_t Client::status() {
  return W5100.readSnSR(_sock);
}

// the next three functions are a hack so we can compare the client returned
// by Server::available() to null, or use it as the condition in an
// if-statement.  this lets us stay compatible with the Processing network
// library.

uint8_t Client::operator==(int p) {
  return !_connected;
}

uint8_t Client::operator!=(int p) {
  return _connected;
}

Client::operator bool() {
  return _connected;
}
