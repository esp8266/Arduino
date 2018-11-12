
#include <WiFiServer.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>

#define int2pcb(x) ((tcp_pcb*)(long)(x))
#define pcb2int(x) ((int)(long)(x))

int serverAccept (int srvsock)
{
	int clisock;
	socklen_t n;
	struct sockaddr_in client;
	n = sizeof(client);
	if ((clisock = accept(srvsock, (struct sockaddr*)&client, &n)) == -1)
	{
		perror("accept()");
		exit(EXIT_FAILURE);
	}
	return clisock;
}

void WiFiServer::begin ()
{
	int sock;
	struct sockaddr_in server;
	
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
	server.sin_port = htons(_port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);
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
	_pcb = int2pcb(sock);
}

bool WiFiServer::hasClient ()
{
	struct pollfd p;
	p.fd = pcb2int(_pcb);
	p.events = POLLIN;
	return poll(&p, 1, 0) && p.revents == POLLIN;
}

size_t WiFiServer::write (uint8_t c)
{
	return write(&c, 1);
}

size_t WiFiServer::write (const uint8_t *buf, size_t size)
{
	fprintf(stderr, MOCK "todo: WiFiServer::write(%p, %d)\n", buf, (int)size);
	return 0;
}
