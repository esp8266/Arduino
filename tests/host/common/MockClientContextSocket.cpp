
// separated from lwIP to avoid type conflicts

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <errno.h>

int mockConnect (uint32_t ipv4, int& sock, int port)
{
	struct sockaddr_in server;
	if ((sock = ::socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		perror(MOCK "ClientContext:connect: ::socket()");
		return 0;
	}
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	memcpy(&server.sin_addr, &ipv4, 4);
	if (::connect(sock, (struct sockaddr*)&server, sizeof(server)) == -1)
	{
		perror(MOCK "ClientContext::connect: ::connect()");
		return 0;
	}

	if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1)
	{
		fprintf(stderr, MOCK "ClientContext::connect: fcntl(O_NONBLOCK): %s\n", strerror(errno));
		close(sock);
		return 0;
	}
	
	return 1;
}

size_t mockFillInBuf (int sock, char* ccinbuf, size_t& ccinbufsize)
{
	size_t maxread = CCBUFSIZE - ccinbufsize;
	ssize_t ret = ::read(sock, ccinbuf + ccinbufsize, maxread);
	if (ret == -1)
	{
		if (errno != EAGAIN)
			fprintf(stderr, MOCK "ClientContext::(read/peek): filling buffer for %d bytes: %s\n", (int)maxread, strerror(errno));
		ret = 0;
	}
	return ccinbufsize += ret;
}

size_t mockPeekBytes (int sock, char* dst, size_t usersize, int timeout_ms, char* ccinbuf, size_t& ccinbufsize)
{
	if (usersize > CCBUFSIZE)
		fprintf(stderr, MOCK "CCBUFSIZE(%d) should be increased by %d bytes (-> %d)\n", CCBUFSIZE, (int)usersize - CCBUFSIZE, (int)usersize);

        struct pollfd p;
	size_t retsize = 0;
	do
	{
		if (usersize <= ccinbufsize)
		{
			// data already buffered
			retsize = usersize;
			break;
		}
		
		// check incoming data data
		mockFillInBuf(sock, ccinbuf, ccinbufsize);
		if (usersize <= ccinbufsize)
		{
			// data just received
			retsize = usersize;
			break;
		}
		
		// wait for more data until timeout
	        p.fd = sock;
	        p.events = POLLIN;
	} while (poll(&p, 1, timeout_ms) == 1);
	
	memcpy(dst, ccinbuf, retsize);
	return retsize;
}

size_t mockRead (int sock, char* dst, size_t size, int timeout_ms, char* ccinbuf, size_t& ccinbufsize)
{
	size_t copied = mockPeekBytes(sock, dst, size, timeout_ms, ccinbuf, ccinbufsize);
	// swallow (XXX use a circular buffer)
	memmove(ccinbuf, ccinbuf + copied, ccinbufsize - copied);
	ccinbufsize -= copied;
	return copied;
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
        {
            ret = ::write(sock, data, size);
            if (ret == -1)
            {
                fprintf(stderr, MOCK "ClientContext::read: write(%d): %s\n", sock, strerror(errno));
                return 0;
            }
        }
        return ret;
}
