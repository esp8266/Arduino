/*
    LEAmDNS2_Backbone.cpp

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

#include "ESP8266mDNS.h"
#include "LEAmDNS2Host.h"
#include "LEAmDNS2_Priv.h"

namespace esp8266
{


namespace experimental
{


/*
    clsLEAmDNS2_Host::clsBackbone::clsBackbone constructor

*/
clsLEAMDNSHost::clsBackbone::clsBackbone(void)
    :   m_pUDPContext(0),
        m_bDelayUDPProcessing(false),
        m_u32DelayedDatagrams(0),
        m_uniqueHost(0)
{
}

/*
    clsLEAmDNS2_Host::clsBackbone::clsBackbone destructor

*/
clsLEAMDNSHost::clsBackbone::~clsBackbone(void)
{
    _releaseUDPContext();
}

/*
    clsLEAmDNS2_Host::clsBackbone::init

*/
bool clsLEAMDNSHost::clsBackbone::init(void)
{
    return _allocUDPContext();
}

/*
    clsLEAmDNS2_Host::clsBackbone::addHost

*/
UdpContext* clsLEAMDNSHost::clsBackbone::addHost(clsLEAMDNSHost* p_pHost)
{
    UdpContext* pUDPContext = nullptr;

    if ((m_pUDPContext) && (p_pHost) && (m_uniqueHost == nullptr))
    {
        m_uniqueHost = p_pHost;
        pUDPContext = m_pUDPContext;
    }
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s addHost: %s to add host!\n"), _DH(), (pUDPContext ? "Succeeded" : "FAILED")););
    return pUDPContext;
}

/*
    clsLEAmDNS2_Host::clsBackbone::removeHost

*/
bool clsLEAMDNSHost::clsBackbone::removeHost(clsLEAMDNSHost* p_pHost)
{
    bool    bResult = false;

    if ((p_pHost) && (m_uniqueHost == p_pHost))
    {
        m_uniqueHost = nullptr;
        bResult = true;
    }
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s removeHost: %s to remove host!\n"), _DH(), (bResult ? "Succeeded" : "FAILED")););
    return bResult;
}


/*
    clsLEAmDNS2_Host::clsBackbone::hostCount

*/
size_t clsLEAMDNSHost::clsBackbone::hostCount(void) const
{
    return m_uniqueHost == nullptr ? 0 : 1;
}

/*
    clsLEAMDNSHost::clsBackbone::::setDelayUDPProcessing

    When executing _sendMessage, with multiple or larger messages, sometimes the ESP IP stack seems
    to need a small delay to get the job done. To allow for this delay, a 'delay' was added after one
    send operation. However, while 'taking' this delay, sometimes a UDP datagram is received and
    processed (which might cause another send operation or change global states).
    To avoid 're-entry-like' problems, UDP processing might be blocked for a short period of time.

*/
bool clsLEAMDNSHost::clsBackbone::setDelayUDPProcessing(bool p_bDelayUDPProcessing)
{
    if (m_bDelayUDPProcessing != p_bDelayUDPProcessing)
    {
        m_bDelayUDPProcessing = p_bDelayUDPProcessing;

        if ((!m_bDelayUDPProcessing) &&
                (m_u32DelayedDatagrams))
        {
            DEBUG_EX_INFO2(if (6 <= m_u32DelayedDatagrams) DEBUG_OUTPUT.printf_P(PSTR("%s setDelayUDPProcessing: Processing %u delayed datagram(s)\n"), _DH(), m_u32DelayedDatagrams););
            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s setDelayUDPProcessing: Processing %u delayed datagram(s)\n"), _DH(), m_u32DelayedDatagrams););
            _processUDPInput();
        }
        m_u32DelayedDatagrams = 0;
    }
    return true;
}

/*
    clsLEAmDNS2_Host::clsBackbone::_allocUDPContext

*/
bool clsLEAMDNSHost::clsBackbone::_allocUDPContext(void)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _allocUDPContext\n"), _DH()););
    if (_releaseUDPContext())
    {
        m_pUDPContext = new UdpContext;
        if (m_pUDPContext)
        {
            m_pUDPContext->ref();

            //ip_set_option(m_pUDPContext->pcb(), SOF_REUSEADDR);
            //udp_bind_netif(m_pUDPContext->pcb(), m_pNetIf);

            if (m_pUDPContext->listen(IP_ANY_TYPE, DNS_MQUERY_PORT))
            {
                // This is NOT the TTL (Time-To-Live) for MDNS records, but the subnet level distance MDNS records should travel.
                // 1 sets the subnet distance to 'local', which is default for MDNS.
                // (Btw.: 255 would set it to 'as far as possible' -> internet), however, RFC 3171 seems to force 255 instead
                const uint8_t	c_u8MulticastTTL = 255;//1;//255;

                m_pUDPContext->setMulticastTTL(c_u8MulticastTTL);
                m_pUDPContext->onRx(std::bind(&clsLEAMDNSHost::clsBackbone::_processUDPInput, this));
                /*  m_pUDPContext->onRx([&](void)->void
                    {
                	DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _allocUDPContext::onRx Received data!\n"), _DH()););
                    });*/
                m_pUDPContext->connect(IP_ANY_TYPE, DNS_MQUERY_PORT);
                DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _allocUDPContext: Succeeded to alloc UDPContext!\n"), _DH()););
            }
            else
            {
                DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _allocUDPContext: FAILED to make UDPContext listening!\n"), _DH()););
                _releaseUDPContext();
            }
        }
        else
        {
            DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _allocUDPContext: FAILED to alloc UDPContext!\n"), _DH()););
        }
    }
    DEBUG_EX_ERR(if (!m_pUDPContext) DEBUG_OUTPUT.printf_P(PSTR("%s _allocUDPContext: FAILED!\n"), _DH()););
    return (0 != m_pUDPContext);
}

/*
    clsLEAmDNS2_Host::clsBackbone::_releaseUDPContext

*/
bool clsLEAMDNSHost::clsBackbone::_releaseUDPContext(void)
{
    if (m_pUDPContext)
    {
        m_pUDPContext->unref();
        m_pUDPContext = nullptr;
    }
    return true;
}

/*
    clsLEAmDNS2_Host::clsBackbone::_processUDPInput

    Called in SYS context!

*/
bool clsLEAMDNSHost::clsBackbone::_processUDPInput(void)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _processUDPInput\n"), _DH()););

    bool	bResult = true;

    if (!m_bDelayUDPProcessing)
    {
        while ((m_pUDPContext) &&
                (m_pUDPContext->next()))
        {
            clsLEAMDNSHost*   pHost = _findHost();

            bResult = pHost->_processUDPInput();

            DEBUG_EX_INFO2_IF(!bResult,
                              DEBUG_OUTPUT.printf_P(PSTR("%s _processUDPInput: FAILED to process UDP input!\n"), _DH()));
            DEBUG_EX_ERR_IF((-1) != m_pUDPContext->peek(),
                            DEBUG_OUTPUT.printf_P(PSTR("%s _processUDPInput: !!!!    CONTENT LEFT IN UDP BUFFER    !!!!\n"),
                                                  _DH()));
            m_pUDPContext->flush();
        }
    }
    else
    {
        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _processUDPInput: Delaying datagram!\n"), _DH()););
        ++m_u32DelayedDatagrams;
    }
    return bResult;
}

/*
    MISC
*/

#if not defined ESP_8266_MDNS_INCLUDE || defined DEBUG_ESP_PORT

/*
    clsLEAmDNS2_Host::clsBackbone::_DH
*/
const char* clsLEAMDNSHost::clsBackbone::_DH(void) const
{
    static char acBuffer[20] = { 0, };
    if (!acBuffer[0])
    {
        strcpy_P(acBuffer, PSTR("[mDNS::backbone]"));
    }
    return acBuffer;
}

#endif

} // namespace MDNSImplementation


} // namespace esp8266
