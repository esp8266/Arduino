/*
    LEAmDNS2_lwIPdefs.h

    License (MIT license):
      Permission is hereby granted, free of charge, to any person obtaining a copy
      of this software and associated documentation files (the "Software"), to deal
      in the Software without restriction, including without limitation the rights
      to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
      copies of the Software, and to permit persons to whom the Software is
      furnished to do so, subject to the following conditions:

      The above copyright notice and this permission notice shall be included in
      all copies or substantial portions of the Software.

      THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
      IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
      FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
      AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
      LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
      OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
      THE SOFTWARE.

*/

#ifndef LEAMDNS2_LWIPDEFS_H
#define LEAMDNS2_LWIPDEFS_H

#include <lwip/init.h>
#if LWIP_VERSION_MAJOR == 1

#include <lwip/mdns.h>      // DNS_RRTYPE_xxx

// cherry pick from lwip1 dns.c/mdns.c source files:
#define DNS_MQUERY_PORT            5353
#define DNS_MQUERY_IPV4_GROUP_INIT IPAddress(224,0,0,251)     /* resolver1.opendns.com */
#define DNS_RRCLASS_ANY            255                        /* any class */

#else // lwIP > 1

#include <lwip/prot/dns.h>  // DNS_RRTYPE_xxx, DNS_MQUERY_PORT

#endif

#endif // LEAMDNS2_LWIPDEFS_H
