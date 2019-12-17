
/*
 Arduino emulation - socket part of ClientContext
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
// separated from lwIP to avoid type conflicts

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <errno.h>

int mockSockSetup (int sock)
{
	if (fcntl(sock, F_SETFL, O_NONBLOCK) == -1)
	{
		perror("socket fcntl(O_NONBLOCK)");
		close(sock);
		return -1;
	}

#ifndef MSG_NOSIGNAL
	int i = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_NOSIGPIPE, &i, sizeof i) == -1)
	{
		perror("sockopt(SO_NOSIGPIPE)(macOS)");
		close(sock);
		return -1;
	}
#endif

	return sock;
}

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

	return mockSockSetup(sock) == -1? 0: 1;
}

ssize_t mockFillInBuf (int sock, char* ccinbuf, size_t& ccinbufsize)
{
	size_t maxread = CCBUFSIZE - ccinbufsize;
	ssize_t ret = ::read(sock, ccinbuf + ccinbufsize, maxread);

	if (ret == 0)
	{
		// connection closed
		// nothing is read
		return 0;
	}

	if (ret == -1)
	{
		if (errno != EAGAIN)
		{
			fprintf(stderr, MOCK "ClientContext::(read/peek fd=%i): filling buffer for %zd bytes: %s\n", sock, maxread, strerror(errno));
assert(0);
            // error
			return -1;
		}
		ret = 0;
	}

	ccinbufsize += ret;
	return ret;
}

ssize_t mockPeekBytes (int sock, char* dst, size_t usersize, int timeout_ms, char* ccinbuf, size_t& ccinbufsize)
{
    // usersize==0: availableForPeek()

	if (usersize > CCBUFSIZE)
		mockverbose("CCBUFSIZE(%d) should be increased by %zd bytes (-> %zd)\n", CCBUFSIZE, usersize - CCBUFSIZE, usersize);

	struct pollfd p;
	size_t retsize = 0;
	do
	{
		if (usersize && usersize <= ccinbufsize)
		{
			// data already buffered
			retsize = usersize;
			break;
		}
		
		// check incoming data data
		if (mockFillInBuf(sock, ccinbuf, ccinbufsize) < 0)
		{
			return -1;
	    }

        if (usersize == 0 && ccinbufsize)
            // availableForPeek
            return ccinbufsize;

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
	
    if (dst)
    {
        memcpy(dst, ccinbuf, retsize);
    }

	return retsize;
}

ssize_t mockRead (int sock, char* dst, size_t size, int timeout_ms, char* ccinbuf, size_t& ccinbufsize)
{
	ssize_t copied = mockPeekBytes(sock, dst, size, timeout_ms, ccinbuf, ccinbufsize);
	if (copied < 0)
		return -1;
	// swallow (XXX use a circular buffer)
	memmove(ccinbuf, ccinbuf + copied, ccinbufsize - copied);
	ccinbufsize -= copied;
	return copied;
}
	
ssize_t mockWrite (int sock, const uint8_t* data, size_t size, int timeout_ms)
{
	size_t sent = 0;
	while (sent < size)
	{

		struct pollfd p;
		p.fd = sock;
		p.events = POLLOUT;
		int ret = poll(&p, 1, timeout_ms);
		if (ret == -1)
		{
			fprintf(stderr, MOCK "ClientContext::write(%d): %s\n", sock, strerror(errno));
			return -1;
		}
		if (ret)
		{
#ifndef MSG_NOSIGNAL
			ret = ::write(sock, data + sent, size - sent);
#else
			ret = ::send(sock, data + sent, size - sent, MSG_NOSIGNAL);
#endif
			if (ret == -1)
			{
				fprintf(stderr, MOCK "ClientContext::write/send(%d): %s\n", sock, strerror(errno));
				return -1;
			}
			sent += ret;
			if (sent < size)
				fprintf(stderr, MOCK "ClientContext::write: sent %d bytes (%zd / %zd)\n", ret, sent, size);
		}
	}
#ifdef DEBUG_ESP_WIFI
	fprintf(stderr, MOCK "ClientContext::write: total sent %zd bytes\n", sent);
#endif
	return sent;
}
