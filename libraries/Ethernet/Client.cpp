#include "w5100.h"
#include "socket.h"

extern "C" {
  #include "string.h"
}

#include "Arduino.h"

#include "Ethernet.h"
#include "Client.h"
#include "Server.h"
#include "Dns.h"

uint16_t Client::_srcport = 1024;

Client::Client() : _sock(MAX_SOCK_NUM) {
}

Client::Client(uint8_t sock) : _sock(sock) {
}

int Client::connect(const char* host, uint16_t port) {
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
}

int Client::connect(IPAddress ip, uint16_t port) {
  if (_sock != MAX_SOCK_NUM)
    return 0;

  for (int i = 0; i < MAX_SOCK_NUM; i++) {
    uint8_t s = W5100.readSnSR(i);
    if (s == SnSR::CLOSED || s == SnSR::FIN_WAIT) {
      _sock = i;
      break;
    }
  }

  if (_sock == MAX_SOCK_NUM)
    return 0;

  _srcport++;
  if (_srcport == 0) _srcport = 1024;
  socket(_sock, SnMR::TCP, _srcport, 0);

  if (!::connect(_sock, ip.raw_address(), port)) {
    _sock = MAX_SOCK_NUM;
    return 0;
  }

  while (status() != SnSR::ESTABLISHED) {
    delay(1);
    if (status() == SnSR::CLOSED) {
      _sock = MAX_SOCK_NUM;
      return 0;
    }
  }

  return 1;
}

void Client::write(uint8_t b) {
  if (_sock != MAX_SOCK_NUM)
    send(_sock, &b, 1);
}

void Client::write(const char *str) {
  if (_sock != MAX_SOCK_NUM)
    send(_sock, (const uint8_t *)str, strlen(str));
}

void Client::write(const uint8_t *buf, size_t size) {
  if (_sock != MAX_SOCK_NUM)
    send(_sock, buf, size);
}

int Client::available() {
  if (_sock != MAX_SOCK_NUM)
    return W5100.getRXReceivedSize(_sock);
  return 0;
}

int Client::read() {
  uint8_t b;
  if ( recv(_sock, &b, 1) > 0 )
  {
    // recv worked
    return b;
  }
  else
  {
    // No data available
    return -1;
  }
}

int Client::read(uint8_t *buf, size_t size) {
  return recv(_sock, buf, size);
}

int Client::peek() {
  uint8_t b;
  // Unlike recv, peek doesn't check to see if there's any data available, so we must
  if (!available())
    return -1;
  ::peek(_sock, &b);
  return b;
}

void Client::flush() {
  while (available())
    read();
}

void Client::stop() {
  if (_sock == MAX_SOCK_NUM)
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
  _sock = MAX_SOCK_NUM;
}

uint8_t Client::connected() {
  if (_sock == MAX_SOCK_NUM) return 0;
  
  uint8_t s = status();
  return !(s == SnSR::LISTEN || s == SnSR::CLOSED || s == SnSR::FIN_WAIT ||
    (s == SnSR::CLOSE_WAIT && !available()));
}

uint8_t Client::status() {
  if (_sock == MAX_SOCK_NUM) return SnSR::CLOSED;
  return W5100.readSnSR(_sock);
}

// the next function allows us to use the client returned by
// Server::available() as the condition in an if-statement.

Client::operator bool() {
  return _sock != MAX_SOCK_NUM;
}
