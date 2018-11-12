
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
	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
	{
		fprintf(stderr, MOCK "UDP socket");
		exit(EXIT_FAILURE);
	}
	return s;
}

bool mockUDPListen (int sock, uint32_t dstaddr, uint16_t port)
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
	
	return true;
}


size_t mockUDPFillInBuf (int sock, char* ccinbuf, size_t& ccinbufsize, void* addrbuf)
{
	if (addrbuf)
		assert(ADDRBUFSIZE >= sizeof(sockaddr));
	socklen_t addrbufsize = ADDRBUFSIZE; ///XXX rework this addrbuf and merge tcp/udp mostly identical code

	size_t maxread = CCBUFSIZE - ccinbufsize;
	ssize_t ret = ::recvfrom(sock, ccinbuf + ccinbufsize, maxread, 0/*flags*/, (sockaddr*)addrbuf, &addrbufsize);
	if (ret == -1)
	{
		if (errno != EAGAIN)
			fprintf(stderr, MOCK "UDPContext::(read/peek): filling buffer for %d bytes: %s\n", (int)maxread, strerror(errno));
		ret = 0;
	}
	return ccinbufsize += ret;
}

size_t mockUDPPeekBytes (int sock, char* dst, size_t usersize, int timeout_ms, char* ccinbuf, size_t& ccinbufsize, void* addrbuf)
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
		mockUDPFillInBuf(sock, ccinbuf, ccinbufsize, addrbuf);
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

size_t mockUDPRead (int sock, char* dst, size_t size, int timeout_ms, char* ccinbuf, size_t& ccinbufsize, void* addrbuf)
{
	size_t copied = mockUDPPeekBytes(sock, dst, size, timeout_ms, ccinbuf, ccinbufsize, addrbuf);
	// swallow (XXX use a circular buffer)
	memmove(ccinbuf, ccinbuf + copied, ccinbufsize - copied);
	ccinbufsize -= copied;
	return copied;
}

size_t mockUDPWrite (int sock, const uint8_t* data, size_t size, int timeout_ms, const void* addrbuf)
{
	if (addrbuf)
		assert(ADDRBUFSIZE >= sizeof(sockaddr));
	///XXX rework this addrbuf and merge tcp/udp mostly identical code

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
		ret = ::sendto(sock, data, size, 0/*flags*/, (const sockaddr*)addrbuf, ADDRBUFSIZE);
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
