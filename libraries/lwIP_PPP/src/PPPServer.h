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

    THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
    WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
    MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT
    SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
    EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
    OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
    IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
    OF SUCH DAMAGE.

    This file is part of the lwIP TCP/IP stack.

    Author: Dirk Ziegelmeier <dziegel@gmx.de>

*/

#ifndef __PPPSERVER_H
#define __PPPSERVER_H

#include <Arduino.h>
#include <IPAddress.h>
#include <lwip/netif.h>
#include <netif/ppp/ppp.h>
#include <netif/ppp/pppos.h>

class PPPServer
{
public:
    PPPServer(Stream* sio);

    bool begin(const IPAddress& ourAddress, const IPAddress& peer = IPAddress(172, 31, 255, 254));
    void stop();

    void ifUpCb(void (*cb)(netif*))
    {
        _cb = cb;
    }
    const ip_addr_t* getPeerAddress() const
    {
        return &_netif.gw;
    }

protected:
    static constexpr size_t _bufsize = 128;
    Stream* _sio;
    ppp_pcb* _ppp;
    netif _netif;
    void (*_cb)(netif*);
    uint8_t _buf[_bufsize];
    bool _enabled;

    // feed ppp from stream - to call on a regular basis or on interrupt
    bool handlePackets();

    static u32_t output_cb_s(ppp_pcb* pcb, u8_t* data, u32_t len, void* ctx);
    static void link_status_cb_s(ppp_pcb* pcb, int err_code, void* ctx);
    static void netif_status_cb_s(netif* nif);
};

#endif  // __PPPSERVER_H
