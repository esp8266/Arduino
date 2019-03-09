/*
 Arduino emulation - UdpContext emulation - socket part
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

#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <net/if.h>

int mockUDPSocket ()
{
	int s;
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1 || fcntl(s, F_SETFL, O_NONBLOCK) == -1)
	{
		fprintf(stderr, MOCK "UDP socket: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	return s;
}

bool mockUDPListen (int sock, uint32_t dstaddr, uint16_t port, uint32_t mcast)
{
	int optval;
	int mockport;

	mockport = port;
	if (mockport < 1024 && mock_port_shifter)
	{
		mockport += mock_port_shifter;
		fprintf(stderr, MOCK "=====> UdpServer port: %d shifted to %d (use option -s) <=====\n", port, mockport);
	}
	else
		fprintf(stderr, MOCK "=====> UdpServer port: %d <=====\n", mockport);

	optval = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &optval, sizeof(optval)) == -1)
		fprintf(stderr, MOCK "SO_REUSEPORT failed\n");
	optval = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
		fprintf(stderr, MOCK "SO_REUSEADDR failed\n");

	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr));

	// Filling server information
	servaddr.sin_family = AF_INET;
	//servaddr.sin_addr.s_addr = global_ipv4_netfmt?: dstaddr;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(mockport);

	// Bind the socket with the server address
	if (bind(sock, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		fprintf(stderr, MOCK "UDP bind on port %d failed: %s\n", mockport, strerror(errno));
		return false;
	}
	else
		mockverbose("UDP server on port %d (sock=%d)\n", mockport, sock);

	if (mcast)
	{
		// https://web.cs.wpi.edu/~claypool/courses/4514-B99/samples/multicast.c
		// https://stackoverflow.com/questions/12681097/c-choose-interface-for-udp-multicast-socket

		struct ip_mreq mreq;
		mreq.imr_multiaddr.s_addr = mcast;
		//mreq.imr_interface.s_addr = global_ipv4_netfmt?: htonl(INADDR_ANY);
		mreq.imr_interface.s_addr = htonl(INADDR_ANY);
		if (global_ipv4_netfmt)
		{
#if __APPLE__
			int idx = if_nametoindex(host_interface);
			if (setsockopt(sock, IPPROTO_TCP, IP_BOUND_IF, &idx, sizeof(idx)) == -1)
#else
			if (setsockopt(sock, SOL_SOCKET, SO_BINDTODEVICE, host_interface, strlen(host_interface)) == -1)
#endif
				fprintf(stderr, MOCK "UDP multicast: can't setup bind/output on interface %s: %s\n", host_interface, strerror(errno));
			if (setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, &mreq.imr_interface, sizeof(struct in_addr)) == -1)
				fprintf(stderr, MOCK "UDP multicast: can't setup bind/input on interface %s: %s\n", host_interface, strerror(errno));
		}

		if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
		{
			fprintf(stderr, MOCK "can't join multicast group addr %08x\n", (int)mcast);
			return false;
		}
	}

	return true;
}


size_t mockUDPFillInBuf (int sock, char* ccinbuf, size_t& ccinbufsize, uint8_t& addrsize, uint8_t addr[16], uint16_t& port)
{
	struct sockaddr_storage addrbuf;
	socklen_t addrbufsize = std::min((socklen_t)sizeof(addrbuf), (socklen_t)16);

	size_t maxread = CCBUFSIZE - ccinbufsize;
	ssize_t ret = ::recvfrom(sock, ccinbuf + ccinbufsize, maxread, 0/*flags*/, (sockaddr*)&addrbuf, &addrbufsize);
	if (ret == -1)
	{
		if (errno != EAGAIN)
			fprintf(stderr, MOCK "UDPContext::(read/peek): filling buffer for %zd bytes: %s\n", maxread, strerror(errno));
		ret = 0;
	}

	if (ret > 0)
	{
		port = ntohs(((sockaddr_in*)&addrbuf)->sin_port);
		if (addrbuf.ss_family == AF_INET)
			memcpy(&addr[0], &(((sockaddr_in*)&addrbuf)->sin_addr.s_addr), addrsize = 4);
		else
		{
			fprintf(stderr, MOCK "TODO UDP+IPv6\n");
			exit(EXIT_FAILURE);
		}
	}

	return ccinbufsize += ret;
}

size_t mockUDPPeekBytes (int sock, char* dst, size_t usersize, int timeout_ms, char* ccinbuf, size_t& ccinbufsize)
{
	if (usersize > CCBUFSIZE)
		fprintf(stderr, MOCK "CCBUFSIZE(%d) should be increased by %zd bytes (-> %zd)\n", CCBUFSIZE, usersize - CCBUFSIZE, usersize);

	size_t retsize = 0;
	if (ccinbufsize)
	{
		// data already buffered
		retsize = usersize;
		if (retsize > ccinbufsize)
			retsize = ccinbufsize;
	}
	memcpy(dst, ccinbuf, retsize);
	return retsize;
}

void mockUDPSwallow (size_t copied, char* ccinbuf, size_t& ccinbufsize)
{
	// poor man buffer
	memmove(ccinbuf, ccinbuf + copied, ccinbufsize - copied);
	ccinbufsize -= copied;
}

size_t mockUDPRead (int sock, char* dst, size_t size, int timeout_ms, char* ccinbuf, size_t& ccinbufsize)
{
	size_t copied = mockUDPPeekBytes(sock, dst, size, timeout_ms, ccinbuf, ccinbufsize);
	mockUDPSwallow(copied, ccinbuf, ccinbufsize);
	return copied;
}

size_t mockUDPWrite (int sock, const uint8_t* data, size_t size, int timeout_ms, uint32_t ipv4, uint16_t port)
{
	// Filling server information
	struct sockaddr_in peer;
	peer.sin_family = AF_INET;
	peer.sin_addr.s_addr = ipv4; //XXFIXME should use lwip_htonl?
	peer.sin_port = htons(port);
	int ret = ::sendto(sock, data, size, 0/*flags*/, (const sockaddr*)&peer, sizeof(peer));
	if (ret == -1)
	{
		fprintf(stderr, MOCK "UDPContext::write: write(%d): %s\n", sock, strerror(errno));
		return 0;
	}
	if (ret != (int)size)
	{
		fprintf(stderr, MOCK "UDPContext::write: short write (%d < %zd) (TODO)\n", ret, size);
		exit(EXIT_FAILURE);
	}

	return ret;
}
