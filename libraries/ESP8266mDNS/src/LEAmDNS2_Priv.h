/*
    LEAmDNS_Priv.h

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

#ifndef MDNS2_PRIV_H
#define MDNS2_PRIV_H

/*
    LWIP_OPEN_SRC
*/
#ifndef LWIP_OPEN_SRC
#define LWIP_OPEN_SRC
#endif

/*
    Enable class debug functions
*/
#define ESP_8266_MDNS_INCLUDE
//#define DEBUG_ESP_MDNS_RESPONDER  // force debug, arduino IDE uses DEBUG_ESP_MDNS

/*
    Enable/disable debug trace macros
*/

#if defined(DEBUG_ESP_PORT)
#define DEBUG_ESP_MDNS_ERR
#endif

#if defined(DEBUG_ESP_PORT) && (defined(DEBUG_ESP_MDNS) || defined(DEBUG_ESP_MDNS_RESPONDER))
#define DEBUG_ESP_MDNS_INFO
#define DEBUG_ESP_MDNS_INFO2
//#define DEBUG_ESP_MDNS_TX
//#define DEBUG_ESP_MDNS_RX
#endif

#ifdef DEBUG_ESP_PORT
#define DEBUG_OUTPUT DEBUG_ESP_PORT
#else
#define DEBUG_OUTPUT Serialx
#endif

#ifdef DEBUG_ESP_MDNS_INFO
#define DEBUG_EX_INFO(A)            A
#define DEBUG_EX_INFO_IF(C,A...)    do if (C) { A; } while (0)
#else
#define DEBUG_EX_INFO(A)
#define DEBUG_EX_INFO_IF(C,A...)
#endif

#ifdef DEBUG_ESP_MDNS_INFO2
#define DEBUG_EX_INFO2(A)           A
#define DEBUG_EX_INFO2_IF(C,A...)   do if (C) { A; } while (0)
#else
#define DEBUG_EX_INFO2(A)
#define DEBUG_EX_INFO2_IF(C,A...)
#endif

#ifdef DEBUG_ESP_MDNS_ERR
#define DEBUG_EX_ERR(A)             A
#define DEBUG_EX_ERR_IF(C,A...)     do if (C) { A; } while (0)
#else
#define DEBUG_EX_ERR(A)
#define DEBUG_EX_ERR_IF(C,A...)
#endif

#ifdef DEBUG_ESP_MDNS_TX
#define DEBUG_EX_TX(A)              do { A; } while (0)
#else
#define DEBUG_EX_TX(A)
#endif

#ifdef DEBUG_ESP_MDNS_RX
#define DEBUG_EX_RX(A)              do { A; } while (0)
#else
#define DEBUG_EX_RX(A)
#endif

/*
    Enable/disable the usage of the F() macro in debug trace printf calls.
    There needs to be an PGM comptible printf function to use this.

    USE_PGM_PRINTF and F
*/
#define USE_PGM_PRINTF

#ifdef USE_PGM_PRINTF
#else
#ifdef F
#undef F
#endif
#define F(A)    A
#endif


#endif  // MDNS2_PRIV_H
