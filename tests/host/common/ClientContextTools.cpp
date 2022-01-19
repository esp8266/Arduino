/*
    Arduino emulation - part of ClientContext
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
    {
        return ERR_TIMEOUT;
    }
    addr->addr = *(uint32_t*)hbn->h_addr_list[0];
    return ERR_OK;
}

static struct tcp_pcb mock_tcp_pcb;
tcp_pcb* tcp_new(void)
{
    // this is useless
    // ClientContext is setting the source port and we don't care here
    return &mock_tcp_pcb;
}
