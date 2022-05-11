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

#include <include/UdpContext.h>
#include <poll.h>
#include <map>

static std::map<int, UdpContext*> udps;

void register_udp(int sock, UdpContext* udp)
{
    if (udp)
        udps[sock] = udp;
    else
        udps.erase(sock);
}

void check_incoming_udp()
{
    // check incoming udp
    for (auto& udp : udps)
    {
        pollfd p;
        p.fd     = udp.first;
        p.events = POLLIN;
        if (poll(&p, 1, 0) && p.revents == POLLIN)
        {
            mockverbose("UDP poll(%d) -> cb\r", p.fd);
            udp.second->mock_cb();
        }
    }
}

void mock_stop_udp ()
{
    udps.clear();
}
