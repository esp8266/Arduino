/*
 Arduino emulation - WiFiServer
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

#include <WiFiClient.h>
#include <WiFiServer.h>

#include <lwip/err.h>
#include <lwip/ip_addr.h>

#include <include/ClientContext.h>

extern "C" const ip_addr_t ip_addr_any = IPADDR4_INIT(IPADDR_ANY);

#define int2pcb(x) ((tcp_pcb*)(intptr_t)(x))
#define pcb2int(x) ((int)(intptr_t)(x))

// lwIP API side of WiFiServer

WiFiServer::WiFiServer(const IPAddress& addr, uint16_t port)
{
    (void)addr;
    _port = port;
}

WiFiServer::WiFiServer(uint16_t port) { _port = port; }

WiFiClient WiFiServer::available(uint8_t* status)
{
    (void)status;
    return accept();
}

WiFiClient WiFiServer::accept()
{
    if (hasClient())
        return WiFiClient(new ClientContext(serverAccept(pcb2int(_listen_pcb))));
    return WiFiClient();
}

// static declaration

#include <include/UdpContext.h>
uint32_t UdpContext::staticMCastAddr = 0;
