
#include <Arduino.h>
#include <user_interface.h> // wifi_get_ip_info()

#include <functional>
#include "lwip/opt.h"
#include "lwip/udp.h"
#include "lwip/inet.h"
#include "lwip/igmp.h"
#include "lwip/mem.h"
#include <include/UdpContext.h>
#include <poll.h>

#include <unistd.h> // usleep
#include <getopt.h>

#include <map>

std::map<int,UdpContext*> udps;

void register_udp (int sock, UdpContext* udp)
{
	if (udp)
		udps[sock] = udp;
	else
		udps.erase(sock);
}

const char* host_interface = nullptr;

void help (const char* argv0, int exitcode)
{
	printf(
		"%s - compiled with esp8266/arduino emulator\n"
		"options:\n"
		"	-h\n"
		"	-i <interface> - use this interface for IP address\n"
		"	-l             - bind tcp/udp servers to interface only (not 0.0.0.0)\n"
		,argv0);
	exit(exitcode);
}

static struct option options[] =
{
	{ "help",		no_argument,		NULL, 'h' },
	{ "local",		required_argument,	NULL, 'l' },
	{ "interface",		required_argument,	NULL, 'i' },
};

int main (int argc, char* const argv [])
{
	for (;;)
	{
		int n = getopt_long(argc, argv, "hli:", options, NULL);
		if (n < 0)
			break;
		switch (n)
		{
		case 'h':
			help(argv[0], EXIT_SUCCESS);
			break;
		case 'i':
			host_interface = optarg;
			break;
		case 'l':
			global_ipv4_netfmt = NO_GLOBAL_BINDING;
			break;
		default:
			fprintf(stderr, MOCK "bad option '%c'\n", n);
			exit(EXIT_FAILURE);
		}
	}
	
	// setup global global_ipv4_netfmt
	wifi_get_ip_info(0, nullptr);

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

