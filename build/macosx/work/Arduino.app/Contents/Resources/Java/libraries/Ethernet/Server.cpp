#include "w5100.h"
#include "socket.h"
extern "C" {
#include "string.h"
}

#include "Ethernet.h"
#include "Client.h"
#include "Server.h"

Server::Server(uint16_t port)
{
  _port = port;
}

void Server::begin()
{
  for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
    Client client(sock);
    if (client.status() == SnSR::CLOSED) {
      socket(sock, SnMR::TCP, _port, 0);
      listen(sock);
      EthernetClass::_server_port[sock] = _port;
      break;
    }
  }  
}

void Server::accept()
{
  int listening = 0;

  for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
    Client client(sock);

    if (EthernetClass::_server_port[sock] == _port) {
      if (client.status() == SnSR::LISTEN) {
        listening = 1;
      } 
      else if (client.status() == SnSR::CLOSE_WAIT && !client.available()) {
        client.stop();
      }
    } 
  }

  if (!listening) {
    begin();
  }
}

Client Server::available()
{
  accept();

  for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
    Client client(sock);
    if (EthernetClass::_server_port[sock] == _port &&
        (client.status() == SnSR::ESTABLISHED ||
         client.status() == SnSR::CLOSE_WAIT)) {
      if (client.available()) {
        // XXX: don't always pick the lowest numbered socket.
        return client;
      }
    }
  }

  return Client(MAX_SOCK_NUM);
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
  accept();

  for (int sock = 0; sock < MAX_SOCK_NUM; sock++) {
    Client client(sock);

    if (EthernetClass::_server_port[sock] == _port &&
      client.status() == SnSR::ESTABLISHED) {
      client.write(buffer, size);
    }
  }
}
