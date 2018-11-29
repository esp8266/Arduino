/*
 Arduino emulator main loop
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

#if 0
#include "../common/spiffs_mock.h"
#include <spiffs/spiffs.h>
SPIFFS_MOCK_DECLARE(/*size_kb*/1024, /(blovk_kb*/8, /*page_b*/512);
#endif

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
		"	-f             - no throttle (possibly 100%%CPU)\n"
		, argv0);
	exit(exitcode);
}

static struct option options[] =
{
	{ "help",		no_argument,		NULL, 'h' },
	{ "fast",		no_argument,		NULL, 'f' },
	{ "local",		no_argument,		NULL, 'l' },
	{ "interface",		required_argument,	NULL, 'i' },
};

int main (int argc, char* const argv [])
{
	bool fast = false;

	for (;;)
	{
		int n = getopt_long(argc, argv, "hlfi:", options, NULL);
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
		case 'f':
			fast = true;
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
		if (!fast)
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
				fprintf(stderr, MOCK "UDP poll(%d) -> cb\r", p.fd);
				udp.second->mock_cb();
			}
		}
	}
	return 0;
}

