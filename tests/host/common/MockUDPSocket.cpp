
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <poll.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>

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
	struct sockaddr_in servaddr;
	memset(&servaddr, 0, sizeof(servaddr)); 
	
	// Filling server information 
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(dstaddr);
	servaddr.sin_port = htons(port);
	
	// Bind the socket with the server address 
	if (bind(sock, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
	{
		fprintf(stderr, MOCK "UDP bind to port %d failed: %s\n", port, strerror(errno)); 
		return false;
	}
	else
		fprintf(stderr, MOCK "UDP server on port %d (sock=%d)\n", (int)port, (int)sock);
	
	if (mcast)
	{
		// https://web.cs.wpi.edu/~claypool/courses/4514-B99/samples/multicast.c
		struct ip_mreq mreq;
		mreq.imr_multiaddr.s_addr = htonl(mcast);
		mreq.imr_interface.s_addr = htonl(INADDR_ANY);         
		if (setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
		{
			fprintf(stderr, MOCK "can't join multicast group addr %08x\n", (int)mcast);
			return false;
		}
	}
	
	return true;
}


size_t mockUDPFillInBuf (int sock, char* ccinbuf, size_t& ccinbufsize, void* addrbuf, uint32_t* addrbufsize)
{
	socklen_t realaddrbufsize = addrbufsize? *addrbufsize: 0;

	size_t maxread = CCBUFSIZE - ccinbufsize;
	ssize_t ret = ::recvfrom(sock, ccinbuf + ccinbufsize, maxread, 0/*flags*/, (sockaddr*)addrbuf, &realaddrbufsize);
	*addrbufsize = realaddrbufsize;
	if (ret == -1)
	{
		if (errno != EAGAIN)
			fprintf(stderr, MOCK "UDPContext::(read/peek): filling buffer for %d bytes: %s\n", (int)maxread, strerror(errno));
		ret = 0;
	}

	return ccinbufsize += ret;
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
			fprintf(stderr, MOCK "UDPContext::(read/peek): filling buffer for %d bytes: %s\n", (int)maxread, strerror(errno));
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
		fprintf(stderr, MOCK "CCBUFSIZE(%d) should be increased by %d bytes (-> %d)\n", CCBUFSIZE, (int)usersize - CCBUFSIZE, (int)usersize);

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

size_t mockUDPRead (int sock, char* dst, size_t size, int timeout_ms, char* ccinbuf, size_t& ccinbufsize)
{
	size_t copied = mockUDPPeekBytes(sock, dst, size, timeout_ms, ccinbuf, ccinbufsize);
	// swallow (XXX use a circular buffer?)
	memmove(ccinbuf, ccinbuf + copied, ccinbufsize - copied);
	ccinbufsize -= copied;
	return copied;
}

size_t mockUDPWrite (int sock, const uint8_t* data, size_t size, int timeout_ms, uint32_t ipv4, uint16_t port)
{
	struct pollfd p;
	p.fd = sock;
	p.events = POLLOUT;
	int ret = poll(&p, 1, timeout_ms);
	if (ret == -1)
	{
		 fprintf(stderr, MOCK "UDPContext::write: poll(%d): %s\n", sock, strerror(errno));
		 return 0;
	}
	if (ret)
	{
		// Filling server information 
		struct sockaddr_in peer;
		peer.sin_family = AF_INET;
		peer.sin_addr.s_addr = ipv4; //XXFIXME should use lwip_htonl?
		peer.sin_port = htons(port);
		ret = ::sendto(sock, data, size, 0/*flags*/, (const sockaddr*)&peer, sizeof(peer));
		if (ret == -1)
		{
			fprintf(stderr, MOCK "UDPContext::write: write(%d): %s\n", sock, strerror(errno));
			return 0;
		}
		if (ret != (int)size)
		{
			fprintf(stderr, MOCK "UDPContext::write: short write (%d < %d) (TODO)\n", (int)ret, (int)size);
			exit(EXIT_FAILURE);
		}
	}
	return ret;
}
