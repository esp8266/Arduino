/*
    Arduino emulation - WiFiServer socket side
    Copyright (c) 2018 david gauchard. All rights reserved.

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal with the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimers.

    - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimers in the
    documentation and/or other materials provided with the distribution.

    - The names of its contributors may not be used to endorse or promote
    products derived from this Software without specific prior written
    permission.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
    THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
    OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
    ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS WITH THE SOFTWARE.
*/

#include <WiFiServer.h>

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define int2pcb(x) ((tcp_pcb*)(intptr_t)(x))
#define pcb2int(x) ((int)(intptr_t)(x))

// host socket internal side of WiFiServer

int serverAccept(int srvsock)
{
    int clisock;
    socklen_t n;
    struct sockaddr_in client;
    n = sizeof(client);
    if ((clisock = accept(srvsock, (struct sockaddr*)&client, &n)) == -1)
    {
        perror(MOCK "accept()");
        exit(EXIT_FAILURE);
    }
    return mockSockSetup(clisock);
}

void WiFiServer::begin(uint16_t port)
{
    return begin(port, !0);
}

void WiFiServer::begin(uint16_t port, uint8_t backlog)
{
    if (!backlog)
    {
        return;
    }
    _port = port;
    return begin();
}

void WiFiServer::begin()
{
    int sock;
    int mockport;
    struct sockaddr_in server;

    mockport = _port;
    if (mockport < 1024 && mock_port_shifter)
    {
        mockport += mock_port_shifter;
        fprintf(stderr, MOCK "=====> WiFiServer port: %d shifted to %d (use option -s) <=====\n", _port, mockport);
    }
    else
    {
        fprintf(stderr, MOCK "=====> WiFiServer port: %d <=====\n", mockport);
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror(MOCK "socket()");
        exit(EXIT_FAILURE);
    }

    int optval = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1)
    {
        perror(MOCK "reuseport");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_port = htons(mockport);
    server.sin_addr.s_addr = htonl(global_source_address);
    if (bind(sock, (struct sockaddr*)&server, sizeof(server)) == -1)
    {
        perror(MOCK "bind()");
        exit(EXIT_FAILURE);
    }

    if (listen(sock, 1) == -1)
    {
        perror(MOCK "listen()");
        exit(EXIT_FAILURE);
    }

    // store int into pointer
    _listen_pcb = int2pcb(sock);
}

bool WiFiServer::hasClient()
{
    struct pollfd p;
    p.fd = pcb2int(_listen_pcb);
    p.events = POLLIN;
    return poll(&p, 1, 0) && p.revents == POLLIN;
}

void WiFiServer::close()
{
    if (pcb2int(_listen_pcb) >= 0)
    {
        ::close(pcb2int(_listen_pcb));
    }
    _listen_pcb = int2pcb(-1);
}

void WiFiServer::stop()
{
    close();
}

size_t WiFiServer::hasClientData()
{
    // Trivial Mocking:
    // There is no waiting list of clients in this trivial mocking code,
    // so the code has to act as if the tcp backlog list is full,
    // and nothing is known about potential further clients.
    // It could be implemented by accepting new clients and store their data until the current one is closed.
    return 0;
}

bool WiFiServer::hasMaxPendingClients()
{
    // Mocking code does not consider the waiting client list,
    // so it will return ::hasClient() here meaning:
    // - our waiting client list does not exist
    // - we consider pending number is max if a new client is waiting
    //   or not max if there's no new client.
    return hasClient();
}
