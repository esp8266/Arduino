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

#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <termios.h>
#include <stdarg.h>
#include <stdio.h>

bool user_exit = false;
const char* host_interface = nullptr;
size_t spiffs_kb = 1024;
bool ignore_sigint = false;
bool restore_tty = false;

#define STDIN STDIN_FILENO

static struct termios initial_settings;

static int mock_start_uart(void)
{
	struct termios settings;

	if (!isatty(STDIN)) return 0;
	if (tcgetattr(STDIN, &initial_settings) < 0) return -1;
	settings = initial_settings;
	settings.c_lflag &= ~(ignore_sigint ? ISIG : 0);
	settings.c_lflag &= ~(ECHO	| ICANON);
	settings.c_iflag &= ~(ICRNL | INLCR | ISTRIP | IXON);
	settings.c_oflag |=	(ONLCR);
	settings.c_cc[VMIN]	= 0;
	settings.c_cc[VTIME] = 0;
	if (tcsetattr(STDIN, TCSANOW, &settings) < 0) return -2;
	restore_tty = true;
	return 0;
}

static int mock_stop_uart(void)
{
	if (!restore_tty) return 0;
	if (!isatty(STDIN)) {
		perror("isatty(STDIN)");
		//system("stty sane"); <- same error message "Inappropriate ioctl for device"
		return 0;
	}
	if (tcsetattr(STDIN, TCSANOW, &initial_settings) < 0) return -1;
	printf("\e[?25h"); // show cursor
	return (0);
}

static uint8_t mock_read_uart(void)
{
	uint8_t ch = 0;
	return (read(STDIN, &ch, 1) == 1) ? ch : 0;
}

void help (const char* argv0, int exitcode)
{
	printf(
		"%s - compiled with esp8266/arduino emulator\n"
		"options:\n"
		"	-h\n"
		"	-i <interface> - use this interface for IP address\n"
		"	-l             - bind tcp/udp servers to interface only (not 0.0.0.0)\n"
		"	-c             - ignore CTRL-C (send it via Serial)\n"
		"	-f             - no throttle (possibly 100%%CPU)\n"
		"	-S             - spiffs size in KBytes (default: %zd)\n"
		"                  (negative value will force mismatched size)\n"
		, argv0, spiffs_kb);
	exit(exitcode);
}

static struct option options[] =
{
	{ "help",		no_argument,		NULL, 'h' },
	{ "fast",		no_argument,		NULL, 'f' },
	{ "local",		no_argument,		NULL, 'l' },
	{ "sigint",		no_argument,		NULL, 'c' },
	{ "interface",		required_argument,	NULL, 'i' },
	{ "spiffskb",		required_argument,	NULL, 'S' },
};

void cleanup ()
{
	mock_stop_spiffs();
	mock_stop_uart();
}

void control_c (int sig)
{
	(void)sig;

	if (user_exit)
	{
		fprintf(stderr, MOCK "stuck, killing\n");
		cleanup();
		exit(1);
	}
	user_exit = true;
}

int main (int argc, char* const argv [])
{
	signal(SIGINT, control_c);

	bool fast = false;

	for (;;)
	{
		int n = getopt_long(argc, argv, "hlcfi:S:", options, NULL);
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
		case 'c':
			ignore_sigint = true;
			break;
		case 'f':
			fast = true;
			break;
		case 'S':
			spiffs_kb = atoi(optarg);
			break;
		default:
			fprintf(stderr, MOCK "bad option '%c'\n", n);
			exit(EXIT_FAILURE);
		}
	}

	if (spiffs_kb)
	{
		String name = argv[0];
		name += "-spiffs";
		name += String(spiffs_kb > 0? spiffs_kb: -spiffs_kb, DEC);
		name += "KB";
		mock_start_spiffs(name, spiffs_kb);
	}

	// setup global global_ipv4_netfmt
	wifi_get_ip_info(0, nullptr);

	// set stdin to non blocking mode
	mock_start_uart();

	// install exit handler in case Esp.restart() is called
	atexit(cleanup);

	setup();
	while (!user_exit)
	{
		uint8_t data = mock_read_uart();

		if (data)
			uart_new_data(UART0, data);
		if (!fast)
			usleep(1000); // not 100% cpu, ~1000 loops per second
		loop();
		check_incoming_udp();
	}
	cleanup();

	return 0;
}
