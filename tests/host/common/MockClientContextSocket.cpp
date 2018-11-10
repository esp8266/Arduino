
// separated from lwIP to avoid type conflicts

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <poll.h>
#include <errno.h>

int mockConnect (uint32_t addr, int& sock, int port)
{
	struct sockaddr_in server;
	if ((sock = ::socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror(MOCK "ClientContext:connect: ::socket()");
		return 0;
		exit(EXIT_FAILURE);
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	memcpy(&server.sin_addr, &addr, 4);
	if (::connect(sock, (struct sockaddr*)&server, sizeof(server)) == -1)
	{
		perror(MOCK "ClientContext::connect: ::connect()");
		return 0;
	}

//XXXFIXME NONBLOCK!
	
	return 1;
}

size_t mockReadReady (int sock)
{
        struct pollfd p;
        p.fd = sock;
        p.events = POLLIN;
        return poll(&p, 1, 0);
}

size_t mockRead (int sock, char* dst, size_t size, int timeout_ms)
{
        struct pollfd p;
        p.fd = sock;
        p.events = POLLIN;
        int ret = poll(&p, 1, timeout_ms);
        if (ret == -1)
        {
            fprintf(stderr, MOCK "ClientContext::read: poll(%d): %s\n", sock, strerror(errno));
            return 0;
        }
        if (ret)
            ret = ::read(sock, dst, size);
        return ret;
}

size_t mockWrite (int sock, const uint8_t* data, size_t size, int timeout_ms)
{
        struct pollfd p;
        p.fd = sock;
        p.events = POLLOUT;
        int ret = poll(&p, 1, timeout_ms);
        if (ret == -1)
        {
            fprintf(stderr, MOCK "ClientContext::write: poll(%d): %s\n", sock, strerror(errno));
            return 0;
        }
        if (ret)
            ret = ::write(sock, data, size);
        return ret;
}
