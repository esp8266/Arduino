
#include <WiFiClient.h>
#include <WiFiServer.h>

#include <lwip/err.h>
#include <lwip/ip_addr.h>

#include <include/ClientContext.h>

extern "C" const ip_addr_t ip_addr_any = IPADDR4_INIT(IPADDR_ANY);

#define int2pcb(x) ((tcp_pcb*)(long)(x))
#define pcb2int(x) ((int)(long)(x))

WiFiServer::WiFiServer (IPAddress addr, uint16_t port)
{
//XXX factorize
	(void)addr;
	if (port < 1024)
	{
		int newport = port + 9000;
		fprintf(stderr, MOCK "WiFiServer port: %d -> %d\n", port, newport);
		port = newport;
	}
	_port = port;
}

WiFiServer::WiFiServer (uint16_t port)
{
	if (port < 1024)
	{
		int newport = port + 9000;
		fprintf(stderr, MOCK "WiFiServer port: %d -> %d\n", port, newport);
		port = newport;
	}
	_port = port;
}

WiFiClient WiFiServer::available (uint8_t* status)
{
	(void)status;
	if (hasClient())
		return WiFiClient(new ClientContext(serverAccept(pcb2int(_pcb))));
	return WiFiClient();
}

// static declaration

#include <include/UdpContext.h>
uint32_t UdpContext::staticMCastAddr = 0;

