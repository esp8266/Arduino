/*
    LEAmDNS2_APIHelpers.cpp

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

#include <arch/cc.h>
#include <sys/time.h>
#include <HardwareSerial.h>
#include <IPAddress.h>
#include <lwip/ip_addr.h>
#include <WString.h>
#include <cstdint>

/*
    ESP8266mDNS Control.cpp
*/

extern "C" {
#include "user_interface.h"
}

#include "LEAmDNS2_lwIPdefs.h"
#include "LEAmDNS2_Priv.h"

namespace esp8266
{
/*
    LEAmDNS
*/
namespace experimental
{

/*
    MDNSResponder::_allocUDPContext
*/
bool MDNSResponder::_allocUDPContext(void)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _allocUDPContext\n"), _DH()););
    if (_releaseUDPContext())
    {
        m_pUDPContext = new UdpContext;
        if (m_pUDPContext)
        {
            m_pUDPContext->ref();

            ip_set_option(m_pUDPContext->pcb(), SOF_REUSEADDR);
            //udp_bind_netif(m_pUDPContext->pcb(), m_pNetIf);

            if (m_pUDPContext->listen(IP_ANY_TYPE, DNS_MQUERY_PORT))
            {
                //m_pUDPContext->setMulticastInterface(m_pNetIf);
                m_pUDPContext->setMulticastTTL(MDNS_MULTICAST_TTL);
                m_pUDPContext->onRx(std::bind(&MDNSResponder::_processUDPInput, this));
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
    MDNSResponder::_releaseUDPContext
*/
bool MDNSResponder::_releaseUDPContext(void)
{
    if (m_pUDPContext)
    {
        m_pUDPContext->unref();
        m_pUDPContext = 0;
    }
    return true;
}

/*
    MDNSResponder::_processUDPInput

    Called in SYS context!

*/
bool MDNSResponder::_processUDPInput(void)
{
    //DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _processUDPInput\n"), _DH()););

    if (m_pUDPContext->next())
    {
        netif*                                      pNetIf = ip_current_input_netif();
        MDNSResponder::clsHost*    pHost = 0;
        if ((pNetIf) &&
            ((pHost = _findHost(pNetIf))))
        {
            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _processUDPInput: netif:%u,src:%s,dest:%s\n"), _DH(), netif_get_index(pNetIf), IPAddress(ip_current_src_addr()).toString().c_str(), IPAddress(ip_current_dest_addr()).toString().c_str()););
            pHost->processUDPInput(/*IPAddress(ip_current_src_addr()), IPAddress(ip_current_dest_addr())*/);
        }
        else
        {
            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _processUDPInput: Received UDP datagramm for unused netif at index: %u\n"), _DH(), (pNetIf ? netif_get_index(pNetIf) : (-1))););
        }
        m_pUDPContext->flush();
    }
    return true;
}

/*
    MDNSResponder::_createHost
*/
MDNSResponder::clsHost* MDNSResponder::_createHost(netif* p_pNetIf)
{
    clsHost*    pHost = 0;

    if ((p_pNetIf) &&
        (!((pHost = _findHost(p_pNetIf)))) &&
        (m_pUDPContext) &&
        ((pHost = new clsHost(*p_pNetIf, *m_pUDPContext))))
    {
        if (pHost->init())
        {
            //pHost->setHostProbeResultCallback(_defaultHostProbeResultCallback);
            m_HostList.push_back(pHost);
        }
        else
        {
            DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _createHost: FAILED!\n"), _DH()););
            _releaseHost(pHost);
            pHost = 0;
        }
    }
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _createHost: Attaching to netif %s!\n"), _DH(), (pHost ? "succeeded" : "FAILED")););
    return pHost;
}

/*
    MDNSResponder::_releaseHost
*/
bool MDNSResponder::_releaseHost(MDNSResponder::clsHost* p_pHost)
{
    bool    bResult = false;

    if ((p_pHost) &&
        (m_HostList.end() != std::find(m_HostList.begin(), m_HostList.end(), p_pHost)))
    {
        // Delete and remove Responder object
        delete p_pHost;
        m_HostList.remove(p_pHost);
        bResult = true;
    }
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _releaseHost: %s to release netif Responder!\n"), _DH(), (bResult ? "Succeeded" : "FAILED")););
    return bResult;
}

/*
    MDNSResponder::_findHost
*/
const MDNSResponder::clsHost* MDNSResponder::_findHost(netif* p_pNetIf) const
{
    const clsHost*    pResult = 0;
    for (const clsHost* pHost : m_HostList)
    {
        if ((p_pNetIf) &&
            (&(pHost->m_rNetIf) == p_pNetIf))
        {
            pResult = pHost;
            break;
        }
    }
    return pResult;
}

/*
    MDNSResponder::_findHost
*/
MDNSResponder::clsHost* MDNSResponder::_findHost(netif* p_pNetIf)
{
    return (clsHost*)(((const MDNSResponder*)this)->_findHost(p_pNetIf));
}

/*
    MDNSResponder::_findHost
*/
const MDNSResponder::clsHost* MDNSResponder::_findHost(const MDNSResponder::hMDNSHost p_hMDNSHost) const
{
    clsHostList::const_iterator it(std::find(m_HostList.begin(), m_HostList.end(), _NRH2Ptr(p_hMDNSHost)));
    return ((m_HostList.end() != it) ? *it : 0);
}

/*
    MDNSResponder::_findHost
*/
MDNSResponder::clsHost* MDNSResponder::_findHost(const MDNSResponder::hMDNSHost p_hMDNSHost)
{
    return (clsHost*)(((const MDNSResponder*)this)->_findHost(p_hMDNSHost));
}


/*
    HANDLE HELPERS
*/

/*
    MDNSResponder::_validateMDNSHostHandle
*/
bool MDNSResponder::_validateMDNSHostHandle(const hMDNSHost p_hMDNSHost) const
{
    return (0 != _findHost(_NRH2Ptr(p_hMDNSHost)));
}

/*
    MDNSResponder::_validateMDNSHostHandle
*/
bool MDNSResponder::_validateMDNSHostHandle(const hMDNSHost p_hMDNSHost,
                                            const hMDNSService p_hMDNSService) const
{
    return ((_validateMDNSHostHandle(p_hMDNSHost)) &&
            (_NRH2Ptr(p_hMDNSHost)->validateService(_SH2Ptr(p_hMDNSService))));
}

/*
    MDNSResponder::_NRH2Ptr
*/
MDNSResponder::clsHost* MDNSResponder::_NRH2Ptr(const hMDNSHost p_hMDNSHost)
{
    return (clsHost*)p_hMDNSHost;
}

/*
    MDNSResponder::_NRH2Ptr
*/
const MDNSResponder::clsHost* MDNSResponder::_NRH2Ptr(const hMDNSHost p_hMDNSHost) const
{
    return (const clsHost*)p_hMDNSHost;
}

/*
    MDNSResponder::_SH2Ptr
*/
MDNSResponder::clsHost::stcService* MDNSResponder::_SH2Ptr(const hMDNSService p_hMDNSService)
{
    return (clsHost::stcService*)p_hMDNSService;
}

/*
    MDNSResponder::_SH2Ptr
*/
const MDNSResponder::clsHost::stcService* MDNSResponder::_SH2Ptr(const hMDNSService p_hMDNSService) const
{
    return (const clsHost::stcService*)p_hMDNSService;
}

/*
    MDNSResponder::_begin

    Creates a new netif responder (adding the netif to the multicast groups),
    sets up the instance data (hostname, ...) and starts the probing process

*/
MDNSResponder::clsHost* MDNSResponder::_begin(const char* p_pcHostName,
                                              netif* p_pNetIf,
                                              MDNSHostProbeResultCallbackFn p_fnCallback)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _begin(%s, netif: %u)\n"), _DH(), (p_pcHostName ?: "_"), (p_pNetIf ? netif_get_index(p_pNetIf) : 0)););

    clsHost*  pHost = 0;
    if ((!m_pUDPContext) ||
        (!p_pNetIf) ||
        (!((pHost = _createHost(p_pNetIf)))) ||
        (p_fnCallback ? !setHostProbeResultCallback((hMDNSHost)pHost, p_fnCallback) : false) ||
        (!pHost->setHostName(p_pcHostName)) ||
        (!pHost->restart()))
    {
        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _begin: FAILED for '%s'!\n"), _DH(), (p_pcHostName ? : "-")););
    }
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _begin: %s to init netif with hostname %s!\n"), _DH(), (pHost ? "Succeeded" : "FAILED"), (p_pcHostName ? : "-")););
    return pHost;
}

/*
    MDNSResponder::_close

    The announced host and services are unannounced (by multicasting a goodbye message)
    All connected objects and finally the netif Responder is removed.

*/
bool MDNSResponder::_close(MDNSResponder::clsHost& p_rHost)
{
    _releaseHost(&p_rHost); // Will call 'delete' on the pHost object!

    return true;
}


/*
    MISC
*/

#if not defined ESP_8266_MDNS_INCLUDE || defined DEBUG_ESP_MDNS_RESPONDER

/*
	MDNSResponder::_DH
*/
const char* MDNSResponder::_DH(const hMDNSHost p_hMDNSHost /*= 0*/) const
{
    static char acBuffer[64];

    *acBuffer = 0;
    if (p_hMDNSHost)
    {
        sprintf_P(acBuffer, PSTR("[MDNSResponder %s]"), ((WIFI_STA == netif_get_index(&((clsHost*)p_hMDNSHost)->m_rNetIf))
                                                         ? "STA"
                                                         : ((WIFI_AP == netif_get_index(&((clsHost*)p_hMDNSHost)->m_rNetIf))
                                                            ? "AP"
                                                            : "??")));
    }
    else
    {
        sprintf_P(acBuffer, PSTR("[MDNSResponder]"));
    }
    return acBuffer;
}

#endif


} // namespace MDNSImplementation

} // namespace esp8266
