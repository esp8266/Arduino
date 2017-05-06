
/*

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met: 

1. Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution. 
3. The name of the author may not be used to endorse or promote products 
derived from this software without specific prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS AND ANY EXPRESS OR IMPLIED 
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.

author: d. gauchard

*/


#include "uprint.h"
#include "glue.h"

#if UDEBUG

void dump (const char* what, const char* data, uint16_t len)
{
	#define N 16
	size_t i, j;
	uprint("DUMP %s: len=%d\n", what, len);
	for (i = 0; i < len; i += N)
	{
		for (j = i; j < i + N && j < len; j++)
			uprint("%02x ", data[j]);
		for (; j < i + N; j++)
			uprint("   ");
		for (j = i; j < i + N && j < len; j++)
			uprint("%c", (data[j] >= 32 && data[j] < 127)? data[j]: '.');
		uprint("\n");
	}
}

void display_ip32 (const char* pre, uint32_t ip)
{
	uprint("%s%d.%d.%d.%d",
		pre,
		(int)(ip & 0xff),
		(int)((ip >> 8) & 0xff),
		(int)((ip >> 16) & 0xff),
		(int)(ip >> 24));
}

void display_mac (const uint8_t* mac)
{
	uprint("%02x:%02x:%02x:%02x:%02x:%02x",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void display_ip_info (const struct ip_info* i)
{
	display_ip32("ip=", i->ip.addr);
	display_ip32(" mask=", i->netmask.addr);
	display_ip32(" gw=", i->gw.addr);
}

#endif
