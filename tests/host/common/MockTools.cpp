/*
 Arduino emulation - tools
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

#include <arpa/inet.h>
#include <stdarg.h>

extern "C"
{
    uint32_t lwip_htonl(uint32_t hostlong) { return htonl(hostlong); }
    uint16_t lwip_htons(uint16_t hostshort) { return htons(hostshort); }
    uint32_t lwip_ntohl(uint32_t netlong) { return ntohl(netlong); }
    uint16_t lwip_ntohs(uint16_t netshort) { return ntohs(netshort); }

    char* ets_strcpy(char* d, const char* s) { return strcpy(d, s); }
    char* ets_strncpy(char* d, const char* s, size_t n) { return strncpy(d, s, n); }
    size_t ets_strlen(const char* s) { return strlen(s); }

    int ets_printf(const char* fmt, ...)
    {
        va_list ap;
        va_start(ap, fmt);
        int len = vprintf(fmt, ap);
        va_end(ap);
        return len;
    }

    void stack_thunk_add_ref() { }
    void stack_thunk_del_ref() { }
    void stack_thunk_repaint() { }

    uint32_t stack_thunk_get_refcnt() { return 0; }
    uint32_t stack_thunk_get_stack_top() { return 0; }
    uint32_t stack_thunk_get_stack_bot() { return 0; }
    uint32_t stack_thunk_get_cont_sp() { return 0; }
    uint32_t stack_thunk_get_max_usage() { return 0; }
    void stack_thunk_dump_stack() { }

// Thunking macro
#define make_stack_thunk(fcnToThunk)
};

void configTime(int timezone, int daylightOffset_sec, const char* server1, const char* server2, const char* server3)
{
    (void)server1;
    (void)server2;
    (void)server3;

    mockverbose("configTime: TODO (tz=%dH offset=%dS) (time will be host's)\n", timezone, daylightOffset_sec);
}
