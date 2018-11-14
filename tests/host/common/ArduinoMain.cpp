
#include <Arduino.h>

#include <functional>
#include "lwip/opt.h"
#include "lwip/udp.h"
#include "lwip/inet.h"
#include "lwip/igmp.h"
#include "lwip/mem.h"
#include <include/UdpContext.h>
#include <poll.h>

#include <unistd.h> // usleep
#include <map>

std::map<int,UdpContext*> udps;

void register_udp (int sock, UdpContext* udp)
{
	if (udp)
		udps[sock] = udp;
	else
		udps.erase(sock);
}


int main (void)
{
	setup();
	while (true)
	{
		usleep(10000); // not 100% cpu
		
		loop();
		
		// check incoming udp
		for (auto& udp: udps)
		{
			pollfd p;
			p.fd = udp.first;
			p.events = POLLIN;
			if (poll(&p, 1, 0) && p.revents == POLLIN)
			{
				fprintf(stderr, MOCK "UDP poll(%d) -> cb\n", (int)p.fd);
				udp.second->mock_cb();
			}
		}
		
	}
	return 0;
}

