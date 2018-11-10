
#include <lwip/def.h>
#include <lwip/tcp.h>
#include <lwip/dns.h>
#include <WiFiClient.h>
#include <include/ClientContext.h>

#include <netdb.h> // gethostbyname

err_t dns_gethostbyname(const char *hostname, ip_addr_t *addr, dns_found_callback found, void *callback_arg)
{
	(void)callback_arg;
	(void)found;
	struct hostent* hbn = gethostbyname(hostname);
	if (!hbn)
		return ERR_TIMEOUT;
	addr->addr = *(uint32_t*)hbn->h_addr_list[0];
	return ERR_OK;
}

static struct tcp_pcb mock_tcp_pcb;
tcp_pcb* tcp_new (void)
{
	// this is useless
	// ClientContext is setting the source port and we don't care here
	return &mock_tcp_pcb;
}
