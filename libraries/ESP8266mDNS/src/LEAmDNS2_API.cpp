/*
    LEAmDNS2_API.cpp

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


#include <Schedule.h>
#include <lwip/netif.h>

#include "LEAmDNS2_Priv.h"


namespace
{

/*
    strrstr (static)

    Backwards search for p_pcPattern in p_pcString
    Based on: https://stackoverflow.com/a/1634398/2778898

*/
const char* strrstr(const char*__restrict p_pcString, const char*__restrict p_pcPattern)
{
    const char* pcResult = 0;

    size_t      stStringLength = (p_pcString ? strlen(p_pcString) : 0);
    size_t      stPatternLength = (p_pcPattern ? strlen(p_pcPattern) : 0);

    if ((stStringLength) &&
        (stPatternLength) &&
        (stPatternLength <= stStringLength))
    {
        // Pattern is shorter or has the same length tham the string

        for (const char* s = (p_pcString + stStringLength - stPatternLength); s >= p_pcString; --s)
        {
            if (0 == strncmp(s, p_pcPattern, stPatternLength))
            {
                pcResult = s;
                break;
            }
        }
    }
    return pcResult;
}

} // anonymous


namespace esp8266
{

/*
    LEAmDNS
*/
namespace experimental
{

/**
    STRINGIZE
*/
#ifndef STRINGIZE
#define STRINGIZE(x) #x
#endif
#ifndef STRINGIZE_VALUE_OF
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)
#endif

/**
    HELPERS
*/

/*
    MDNSResponder::indexDomain (static)

    Updates the given domain 'p_rpcHostName' by appending a delimiter and an index number.

    If the given domain already hasa numeric index (after the given delimiter), this index
    incremented. If not, the delimiter and index '2' is added.

    If 'p_rpcHostName' is empty (==0), the given default name 'p_pcDefaultHostName' is used,
    if no default is given, 'esp8266' is used.

*/
/*static*/ bool MDNSResponder::indexDomain(char*& p_rpcDomain,
                                           const char* p_pcDivider /*= "-"*/,
                                           const char* p_pcDefaultDomain /*= 0*/)
{
    bool    bResult = false;

    // Ensure a divider exists; use '-' as default
    const char*   pcDivider = (p_pcDivider ? : "-");

    if (p_rpcDomain)
    {
        const char* pFoundDivider = strrstr(p_rpcDomain, pcDivider);
        if (pFoundDivider)          // maybe already extended
        {
            char*         pEnd = 0;
            unsigned long ulIndex = strtoul((pFoundDivider + strlen(pcDivider)), &pEnd, 10);
            if ((ulIndex) &&
                ((pEnd - p_rpcDomain) == (ptrdiff_t)strlen(p_rpcDomain)) &&
                (!*pEnd))           // Valid (old) index found
            {

                char    acIndexBuffer[16];
                sprintf(acIndexBuffer, "%lu", (++ulIndex));
                size_t  stLength = ((pFoundDivider - p_rpcDomain + strlen(pcDivider)) + strlen(acIndexBuffer) + 1);
                char*   pNewHostName = new char[stLength];
                if (pNewHostName)
                {
                    memcpy(pNewHostName, p_rpcDomain, (pFoundDivider - p_rpcDomain + strlen(pcDivider)));
                    pNewHostName[pFoundDivider - p_rpcDomain + strlen(pcDivider)] = 0;
                    strcat(pNewHostName, acIndexBuffer);

                    delete[] p_rpcDomain;
                    p_rpcDomain = pNewHostName;

                    bResult = true;
                }
                else
                {
                    DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] indexDomain: FAILED to alloc new hostname!\n")););
                }
            }
            else
            {
                pFoundDivider = 0;  // Flag the need to (base) extend the hostname
            }
        }

        if (!pFoundDivider)         // not yet extended (or failed to increment extension) -> start indexing
        {
            size_t    stLength = strlen(p_rpcDomain) + (strlen(pcDivider) + 1 + 1);   // Name + Divider + '2' + '\0'
            char*     pNewHostName = new char[stLength];
            if (pNewHostName)
            {
                sprintf(pNewHostName, "%s%s2", p_rpcDomain, pcDivider);

                delete[] p_rpcDomain;
                p_rpcDomain = pNewHostName;

                bResult = true;
            }
            else
            {
                DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] indexDomain: FAILED to alloc new hostname!\n")););
            }
        }
    }
    else
    {
        // No given host domain, use base or default
        const char* cpcDefaultName = (p_pcDefaultDomain ? : "esp8266");

        size_t      stLength = strlen(cpcDefaultName) + 1;   // '\0'
        p_rpcDomain = new char[stLength];
        if (p_rpcDomain)
        {
            strncpy(p_rpcDomain, cpcDefaultName, stLength);
            bResult = true;
        }
        else
        {
            DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] indexDomain: FAILED to alloc new hostname!\n")););
        }
    }
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] indexDomain: %s\n"), p_rpcDomain););
    return bResult;
}


/*
    MDNSResponder::setStationHostName (static)

    Sets the staion hostname

*/
/*static*/ bool MDNSResponder::setNetIfHostName(netif* p_pNetIf,
                                                const char* p_pcHostName)
{
    if ((p_pNetIf) &&
        (p_pcHostName))
    {
        netif_set_hostname(p_pNetIf, p_pcHostName);
        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] setNetIfHostName host name: %s!\n"), p_pcHostName););
    }
    return true;
}


/**
    INTERFACE
*/

/**
    MDNSResponder::MDNSResponder
*/
MDNSResponder::MDNSResponder(void)
    :   m_pUDPContext(0)
{
    _allocUDPContext();
}

/*
    MDNSResponder::~MDNSResponder
*/
MDNSResponder::~MDNSResponder(void)
{
    close();
}

/*
    MDNSResponder::begin (hostname, netif, probe_callback)
*/
MDNSResponder::hMDNSHost MDNSResponder::begin(const char* p_pcHostName,
                                              netif* p_pNetIf,
                                              MDNSHostProbeResultCallbackFn p_fnCallback /*= 0*/)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s begin(%s, netif: %u)\n"), _DH(), (p_pcHostName ? : "-"), (p_pNetIf ? netif_get_index(p_pNetIf) : 0)););

    return (hMDNSHost)_begin(p_pcHostName, p_pNetIf, p_fnCallback);
}

/*
    MDNSResponder::begin (hostname, probe_callback)
*/
bool MDNSResponder::begin(const char* p_pcHostName,
                          MDNSHostProbeResultCallbackFn p_fnCallback /*= 0*/)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s begin(%s)\n"), _DH(), (p_pcHostName ? : "_")););

    return begin(p_pcHostName, (WiFiMode_t)wifi_get_opmode(), p_fnCallback);
}

/*
    MDNSResponder::begin (hostname, WiFiMode, probe_callback)
*/
bool MDNSResponder::begin(const char* p_pcHostName,
                          WiFiMode_t p_WiFiMode,
                          MDNSHostProbeResultCallbackFn p_fnCallback /*= 0*/)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s begin(%s, opmode: %u)\n"), _DH(), (p_pcHostName ? : "_"), (uint32_t)p_WiFiMode););

    bool bResult = true;

    if ((bResult) &&
        (p_WiFiMode & WIFI_STA))
    {
        bResult = (0 != _begin(p_pcHostName, netif_get_by_index(WIFI_STA), p_fnCallback));
    }
    if ((bResult) &&
        (p_WiFiMode & WIFI_AP))
    {
        bResult = (0 != _begin(p_pcHostName, netif_get_by_index(WIFI_AP), p_fnCallback));
    }
    return bResult;
}

/*
	MDNSResponder::close
*/
bool MDNSResponder::close(const MDNSResponder::hMDNSHost p_hMDNSHost)
{
    return ((_validateMDNSHostHandle(p_hMDNSHost)) &&
            (_close(*(clsHost*)p_hMDNSHost)));
}

/*
    MDNSResponder::close (convenience)
*/
bool MDNSResponder::close(void)
{
    clsHostList::iterator	it(m_HostList.begin());
    while (m_HostList.end() != it)
    {
        _close(**it++);
    }

    return _releaseUDPContext();
}


/*
    MDNSResponder::getMDNSHost (netif)
*/
MDNSResponder::hMDNSHost MDNSResponder::getMDNSHost(netif* p_pNetIf) const
{
    return (hMDNSHost)(p_pNetIf ? _findHost(p_pNetIf) : 0);
}

/*
    MDNSResponder::getMDNSHost (WiFiMode)
*/
MDNSResponder::hMDNSHost MDNSResponder::getMDNSHost(WiFiMode_t p_WiFiMode) const
{
    hMDNSHost hResult = 0;

    if (WIFI_STA == p_WiFiMode)
    {
        hResult = getMDNSHost(netif_get_by_index(WIFI_STA));
    }
    else if (WIFI_AP == p_WiFiMode)
    {
        hResult = getMDNSHost(netif_get_by_index(WIFI_AP));
    }
    return hResult;
}

/*
	MDNSResponder::setHostName
*/
bool MDNSResponder::setHostName(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                const char* p_pcHostName,
                                MDNSHostProbeResultCallbackFn p_fnCallback /*= 0*/)
{
    bool	bResult = ((_validateMDNSHostHandle(p_hMDNSHost)) &&
                       (_NRH2Ptr(p_hMDNSHost)->setHostName(p_pcHostName)) &&
                       (p_fnCallback ? setHostProbeResultCallback(p_hMDNSHost, p_fnCallback) : true));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s setHostName: FAILED for '%s'!\n"), _DH(p_hMDNSHost), (p_pcHostName ? : "-")););
    return bResult;
}

/*
    MDNSResponder::hostName
*/
const char* MDNSResponder::hostName(const MDNSResponder::hMDNSHost p_hMDNSHost) const
{
    return (_validateMDNSHostHandle(p_hMDNSHost)
            ? (_NRH2Ptr(p_hMDNSHost)->hostName())
            : 0);
}

/*
	MDNSResponder::setHostProbeResultCallback
*/
bool MDNSResponder::setHostProbeResultCallback(const hMDNSHost p_hMDNSHost,
                                               MDNSHostProbeResultCallbackFn p_fnCallback)
{
    bool    bResult = false;
    if ((bResult = _validateMDNSHostHandle(p_hMDNSHost)))
    {
        if (p_fnCallback)
        {
            _NRH2Ptr(p_hMDNSHost)->m_HostProbeInformation.m_fnProbeResultCallback = [this, p_fnCallback](clsHost & p_rHost,
                                                                                                         const char* p_pcDomainName,
                                                                                                         bool p_bProbeResult)->void
            {
                p_fnCallback(*this, (hMDNSHost)&p_rHost, p_pcDomainName, p_bProbeResult);
            };
        }
        else
        {
            _NRH2Ptr(p_hMDNSHost)->m_HostProbeInformation.m_fnProbeResultCallback = 0;
        }
    }
    return bResult;
}

/*
	MDNSResponder::status
*/
bool MDNSResponder::status(const MDNSResponder::hMDNSHost p_hMDNSHost) const
{
    return ((_validateMDNSHostHandle(p_hMDNSHost)) &&
            (_NRH2Ptr(p_hMDNSHost)->probeStatus()));
}


/*
    SERVICES
*/

/*
	MDNSResponder::setInstanceName
*/
bool MDNSResponder::setInstanceName(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                    const char* p_pcInstanceName)
{
    bool	bResult = ((_validateMDNSHostHandle(p_hMDNSHost)) &&
                       (_NRH2Ptr(p_hMDNSHost)->setInstanceName(p_pcInstanceName)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s setInstanceName: FAILED for '%s'!\n"), _DH(p_hMDNSHost), (p_pcInstanceName ? : "-")););
    return bResult;
}

/*
    MDNSResponder::instanceName
*/
const char* MDNSResponder::instanceName(const MDNSResponder::hMDNSHost p_hMDNSHost) const
{
    return (_validateMDNSHostHandle(p_hMDNSHost)
            ? (_NRH2Ptr(p_hMDNSHost)->instanceName())
            : 0);
}

/*
    MDNSResponder::addService

    Add service; using hostname if no name is explicitly provided for the service
    The usual '_' underline, which is prepended to service and protocol, eg. _http,
    may be given. If not, it is added automatically.

*/
MDNSResponder::hMDNSService MDNSResponder::addService(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                      const char* p_pcName,
                                                      const char* p_pcServiceType,
                                                      const char* p_pcProtocol,
                                                      uint16_t p_u16Port,
                                                      MDNSServiceProbeResultCallbackFn p_fnCallback /*= 0*/)
{
    hMDNSService	hService = (_validateMDNSHostHandle(p_hMDNSHost)
                                ? (hMDNSService)_NRH2Ptr(p_hMDNSHost)->addService(p_pcName, p_pcServiceType, p_pcProtocol, p_u16Port)
                                : 0);
    if ((p_fnCallback) &&
        (hService))
    {
        setServiceProbeResultCallback(p_hMDNSHost, hService, p_fnCallback);
    }
    DEBUG_EX_ERR(if (!hService) DEBUG_OUTPUT.printf_P(PSTR("%s addService: FAILED for '%s._%s._%s.local'!\n"), _DH(p_hMDNSHost), (p_pcName ? : "-"), (p_pcServiceType ? : "-"), (p_pcProtocol ? : "-")););
    return hService;
}

/*
    MDNSResponder::removeService

    Unanounce a service (by sending a goodbye message) and remove it
    from the MDNS responder

*/
bool MDNSResponder::removeService(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                  const MDNSResponder::hMDNSService p_hMDNSService)
{
    bool	bResult = ((_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)) &&
                       (_NRH2Ptr(p_hMDNSHost)->removeService(_SH2Ptr(p_hMDNSService))));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s removeService: FAILED!\n"), _DH(p_hMDNSHost)););
    return bResult;
}

/*
	MDNSResponder::removeService
*/
bool MDNSResponder::removeService(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                  const char* p_pcInstanceName,
                                  const char* p_pcServiceType,
                                  const char* p_pcProtocol)
{
    clsHost*                  pMDNSHost;
    clsHost::stcService*  pMDNSService;
    bool	bResult = ((_validateMDNSHostHandle(p_hMDNSHost)) &&
                       (pMDNSHost = (clsHost*)p_hMDNSHost) &&
                       ((pMDNSService = _NRH2Ptr(p_hMDNSHost)->findService(p_pcInstanceName, p_pcServiceType, p_pcProtocol))) &&
                       (_NRH2Ptr(p_hMDNSHost)->removeService(pMDNSService)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s removeService: FAILED for '%s._%s._%s.local'!\n"), _DH(p_hMDNSHost), (p_pcInstanceName ? : "-"), (p_pcServiceType ? : "-"), (p_pcProtocol ? : "-")););
    return bResult;
}

/*
    MDNSResponder::findService

    Find an existing service.

*/
MDNSResponder::hMDNSService MDNSResponder::findService(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                       const char* p_pcInstanceName,
                                                       const char* p_pcServiceType,
                                                       const char* p_pcProtocol)
{
    clsHost*    pMDNSHost;
    return (((_validateMDNSHostHandle(p_hMDNSHost)) &&
             (pMDNSHost = (clsHost*)p_hMDNSHost))
            ? _NRH2Ptr(p_hMDNSHost)->findService(p_pcInstanceName, p_pcServiceType, p_pcProtocol)
            : 0);
}

/*
    MDNSResponder::setServiceName
*/
bool MDNSResponder::setServiceName(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                   const MDNSResponder::hMDNSService p_hMDNSService,
                                   const char* p_pcInstanceName,
                                   MDNSServiceProbeResultCallbackFn p_fnCallback /*= 0*/)
{
    bool	bResult = ((_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)) &&
                       (_NRH2Ptr(p_hMDNSHost)->setServiceName(_SH2Ptr(p_hMDNSService), p_pcInstanceName)) &&
                       (p_fnCallback ? setServiceProbeResultCallback(p_hMDNSHost, p_hMDNSService, p_fnCallback) : true));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s setServiceName: FAILED for '%s'!\n"), _DH(p_hMDNSHost), (p_pcInstanceName ? : "-")););
    return bResult;
}

/*
    MDNSResponder::serviceName
*/
const char* MDNSResponder::serviceName(const hMDNSHost p_hMDNSHost,
                                       const hMDNSService p_hMDNSService) const
{
    return (_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)
            ? (_SH2Ptr(p_hMDNSService)->m_pcName)
            : 0);
}

/*
    MDNSResponder::serviceType
*/
const char* MDNSResponder::serviceType(const hMDNSHost p_hMDNSHost,
                                       const hMDNSService p_hMDNSService) const
{
    return (_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)
            ? (_SH2Ptr(p_hMDNSService)->m_pcServiceType)
            : 0);
}

/*
    MDNSResponder::serviceProtocol
*/
const char* MDNSResponder::serviceProtocol(const hMDNSHost p_hMDNSHost,
                                           const hMDNSService p_hMDNSService) const
{
    return (_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)
            ? (_SH2Ptr(p_hMDNSService)->m_pcProtocol)
            : 0);
}

/*
    MDNSResponder::serviceProtocol
*/
uint16_t MDNSResponder::servicePort(const hMDNSHost p_hMDNSHost,
                                    const hMDNSService p_hMDNSService) const
{
    return (_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)
            ? (_SH2Ptr(p_hMDNSService)->m_u16Port)
            : 0);
}

/*
    MDNSResponder::setServiceProbeResultCallback

    Set a service specific callback for probe results. The callback is called, when probing
    for the service domain failes or succeedes.
    In the case of failure, the service name should be changed via 'setServiceName'.
    When succeeded, the service domain will be announced by the MDNS responder.

*/
bool MDNSResponder::setServiceProbeResultCallback(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                  const MDNSResponder::hMDNSService p_hMDNSService,
                                                  MDNSResponder::MDNSServiceProbeResultCallbackFn p_fnCallback)
{
    bool    bResult = false;
    if ((bResult = _validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)))
    {
        if (p_fnCallback)
        {
            _SH2Ptr(p_hMDNSService)->m_ProbeInformation.m_fnProbeResultCallback = [this, p_fnCallback](clsHost & p_rHost,
                                                                                                       clsHost::stcService & p_rMDNSService,
                                                                                                       const char* p_pcDomainName,
                                                                                                       bool p_bProbeResult)->void
            {
                p_fnCallback(*this, (hMDNSHost)&p_rHost, (hMDNSService)&p_rMDNSService, p_pcDomainName, p_bProbeResult);
            };
        }
        else
        {
            _SH2Ptr(p_hMDNSService)->m_ProbeInformation.m_fnProbeResultCallback = 0;
        }
    }
    return bResult;
}

/*
    MDNSResponder::serviceStatus
*/
bool MDNSResponder::serviceStatus(const hMDNSHost p_hMDNSHost,
                                  const hMDNSService p_hMDNSService) const
{
    return ((_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)) &&
            (_SH2Ptr(p_hMDNSService)->probeStatus()));
}


/*
    SERVICE TXT
*/

/*
    MDNSResponder::addServiceTxt

    Add a static service TXT item ('Key'='Value') to a service.

*/
MDNSResponder::hMDNSTxt MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                     const MDNSResponder::hMDNSService p_hMDNSService,
                                                     const char* p_pcKey,
                                                     const char* p_pcValue)
{
    hMDNSTxt	hTxt = (_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)
                        ? (hMDNSTxt)_NRH2Ptr(p_hMDNSHost)->addServiceTxt(_SH2Ptr(p_hMDNSService), p_pcKey, p_pcValue)
                        : 0);
    DEBUG_EX_ERR(if (!hTxt) DEBUG_OUTPUT.printf_P(PSTR("%s addServiceTxt: FAILED for '%s=%s'!\n"), _DH(p_hMDNSHost), (p_pcKey ? : "-"), (p_pcValue ? : "-")););
    return hTxt;
}

/*
	MDNSRESPONDER_xxx_TO_CHAR
	Formats: http://www.cplusplus.com/reference/cstdio/printf/
*/
#define MDNSRESPONDER_U32_TO_CHAR(BUFFERNAME, U32VALUE)	\
	char    BUFFERNAME[16];	/* 32-bit max 10 digits */	\
	*BUFFERNAME = 0;									\
    sprintf(BUFFERNAME, "%u", U32VALUE);
#define MDNSRESPONDER_U16_TO_CHAR(BUFFERNAME, U16VALUE)	\
	char    BUFFERNAME[8];	/* 16-bit max 5 digits */	\
	*BUFFERNAME = 0;									\
    sprintf(BUFFERNAME, "%hu", U16VALUE);
#define MDNSRESPONDER_U8_TO_CHAR(BUFFERNAME, U8VALUE)	\
	char    BUFFERNAME[8];	/* 8-bit max 3 digits */	\
	*BUFFERNAME = 0;									\
    sprintf(BUFFERNAME, "%hhu", U8VALUE);
#define MDNSRESPONDER_I32_TO_CHAR(BUFFERNAME, I32VALUE)	\
	char    BUFFERNAME[16];	/* 32-bit max 10 digits */	\
	*BUFFERNAME = 0;									\
    sprintf(BUFFERNAME, "%i", I32VALUE);
#define MDNSRESPONDER_I16_TO_CHAR(BUFFERNAME, I16VALUE)	\
	char    BUFFERNAME[8];	/* 16-bit max 5 digits */	\
	*BUFFERNAME = 0;									\
    sprintf(BUFFERNAME, "%hi", I16VALUE);
#define MDNSRESPONDER_I8_TO_CHAR(BUFFERNAME, I8VALUE)	\
	char    BUFFERNAME[8];	/* 8-bit max 3 digits */	\
	*BUFFERNAME = 0;									\
    sprintf(BUFFERNAME, "%hhi", I8VALUE);

/*
    MDNSResponder::addServiceTxt (uint32_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                     const MDNSResponder::hMDNSService p_hMDNSService,
                                                     const char* p_pcKey,
                                                     uint32_t p_u32Value)
{
    MDNSRESPONDER_U32_TO_CHAR(acBuffer, p_u32Value);
    return addServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer);
}

/*
    MDNSResponder::addServiceTxt (uint16_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                     const MDNSResponder::hMDNSService p_hMDNSService,
                                                     const char* p_pcKey,
                                                     uint16_t p_u16Value)
{
    MDNSRESPONDER_U16_TO_CHAR(acBuffer, p_u16Value);
    return addServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer);
}

/*
    MDNSResponder::addServiceTxt (uint8_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                     const MDNSResponder::hMDNSService p_hMDNSService,
                                                     const char* p_pcKey,
                                                     uint8_t p_u8Value)
{
    MDNSRESPONDER_U8_TO_CHAR(acBuffer, p_u8Value);
    return addServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer);
}

/*
    MDNSResponder::addServiceTxt (int32_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                     const MDNSResponder::hMDNSService p_hMDNSService,
                                                     const char* p_pcKey,
                                                     int32_t p_i32Value)
{
    MDNSRESPONDER_I32_TO_CHAR(acBuffer, p_i32Value);
    return addServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer);
}

/*
    MDNSResponder::addServiceTxt (int16_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                     const MDNSResponder::hMDNSService p_hMDNSService,
                                                     const char* p_pcKey,
                                                     int16_t p_i16Value)
{
    MDNSRESPONDER_I16_TO_CHAR(acBuffer, p_i16Value);
    return addServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer);
}

/*
    MDNSResponder::addServiceTxt (int8_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                     const MDNSResponder::hMDNSService p_hMDNSService,
                                                     const char* p_pcKey,
                                                     int8_t p_i8Value)
{
    MDNSRESPONDER_I8_TO_CHAR(acBuffer, p_i8Value);
    return addServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer);
}

/*
    MDNSResponder::removeServiceTxt

    Remove a static service TXT item from a service.
*/
bool MDNSResponder::removeServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                     const MDNSResponder::hMDNSService p_hMDNSService,
                                     const MDNSResponder::hMDNSTxt p_hTxt)
{
    bool	bResult = ((_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)) &&
                       (p_hTxt) &&
                       (_NRH2Ptr(p_hMDNSHost)->removeServiceTxt(_SH2Ptr(p_hMDNSService), (clsHost::stcServiceTxt*)p_hTxt)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s removeServiceTxt: FAILED!\n"), _DH(p_hMDNSHost)););
    return bResult;
}

/*
    MDNSResponder::removeServiceTxt
*/
bool MDNSResponder::removeServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                     const MDNSResponder::hMDNSService p_hMDNSService,
                                     const char* p_pcKey)
{
    bool	bResult = ((_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)) &&
                       (p_pcKey) &&
                       (_NRH2Ptr(p_hMDNSHost)->removeServiceTxt(_SH2Ptr(p_hMDNSService), _NRH2Ptr(p_hMDNSHost)->findServiceTxt(_SH2Ptr(p_hMDNSService), p_pcKey))));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s removeServiceTxt: FAILED!\n"), _DH(p_hMDNSHost)););
    return bResult;
}

/*
	MDNSResponder::setDynamicServiceTxtCallback (binding)

    Set a netif binding specific callback for dynamic service TXT items. The callback is called, whenever
    service TXT items are needed for any service on the netif binding.

*/
bool MDNSResponder::setDynamicServiceTxtCallback(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                 MDNSResponder::MDNSDynamicServiceTxtCallbackFn p_fnCallback)
{
    bool	bResult = ((_validateMDNSHostHandle(p_hMDNSHost)) &&
                       ((!p_fnCallback) ||
                        (_NRH2Ptr(p_hMDNSHost)->setDynamicServiceTxtCallback([this, p_fnCallback](clsHost & p_rHost,
                                                                                                  clsHost::stcService & p_rMDNSService)->void
    {
        if (p_fnCallback)
        {
            p_fnCallback(*this, (hMDNSHost)&p_rHost, (hMDNSService)&p_rMDNSService);
        }
    }))));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s setDynamicServiceTxtCallback: FAILED!\n"), _DH(p_hMDNSHost)););
    return bResult;
}

/*
	MDNSResponder::setDynamicServiceTxtCallback (service)

    Set a service specific callback for dynamic service TXT items. The callback is called, whenever
    service TXT items are needed for the given service.
*/
bool MDNSResponder::setDynamicServiceTxtCallback(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                 const MDNSResponder::hMDNSService p_hMDNSService,
                                                 MDNSResponder::MDNSDynamicServiceTxtCallbackFn p_fnCallback)
{
    bool	bResult = ((_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)) &&
                       ((!p_fnCallback) ||
                        (_NRH2Ptr(p_hMDNSHost)->setDynamicServiceTxtCallback(_SH2Ptr(p_hMDNSService), [this, p_fnCallback](clsHost & p_rHost,
                                                                                                                           clsHost::stcService & p_rMDNSService)->void
    {
        if (p_fnCallback)
        {
            p_fnCallback(*this, (hMDNSHost)&p_rHost, (hMDNSService)&p_rMDNSService);
        }
    }))));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s setDynamicServiceTxtCallback: FAILED!\n"), _DH(p_hMDNSHost)););
    return bResult;
}

/*
    MDNSResponder::addDynamicServiceTxt
*/
MDNSResponder::hMDNSTxt MDNSResponder::addDynamicServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                            const MDNSResponder::hMDNSService p_hMDNSService,
                                                            const char* p_pcKey,
                                                            const char* p_pcValue)
{
    hMDNSTxt    hTxt = (_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)
                        ? (hMDNSTxt)_NRH2Ptr(p_hMDNSHost)->addDynamicServiceTxt(_SH2Ptr(p_hMDNSService), p_pcKey, p_pcValue)
                        : 0);
    DEBUG_EX_ERR(if (!hTxt) DEBUG_OUTPUT.printf_P(PSTR("%s addServiceTxt: FAILED for '%s=%s'!\n"), _DH(p_hMDNSHost), (p_pcKey ? : "-"), (p_pcValue ? : "-")););
    return hTxt;
}

/*
    MDNSResponder::addDynamicServiceTxt (uint32_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addDynamicServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                            MDNSResponder::hMDNSService p_hMDNSService,
                                                            const char* p_pcKey,
                                                            uint32_t p_u32Value)
{
    MDNSRESPONDER_U32_TO_CHAR(acBuffer, p_u32Value);
    return addDynamicServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer);
}

/*
    MDNSResponder::addDynamicServiceTxt (uint16_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addDynamicServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                            const MDNSResponder::hMDNSService p_hMDNSService,
                                                            const char* p_pcKey,
                                                            uint16_t p_u16Value)
{
    MDNSRESPONDER_U16_TO_CHAR(acBuffer, p_u16Value);
    return addDynamicServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer);
}

/*
    MDNSResponder::addDynamicServiceTxt (uint8_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addDynamicServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                            const MDNSResponder::hMDNSService p_hMDNSService,
                                                            const char* p_pcKey,
                                                            uint8_t p_u8Value)
{
    MDNSRESPONDER_U8_TO_CHAR(acBuffer, p_u8Value);
    return addDynamicServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer);
}

/*
    MDNSResponder::addDynamicServiceTxt (int32_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addDynamicServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                            const MDNSResponder::hMDNSService p_hMDNSService,
                                                            const char* p_pcKey,
                                                            int32_t p_i32Value)
{
    MDNSRESPONDER_I32_TO_CHAR(acBuffer, p_i32Value);
    return addDynamicServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer);
}

/*
    MDNSResponder::addDynamicServiceTxt (int16_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addDynamicServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                            const MDNSResponder::hMDNSService p_hMDNSService,
                                                            const char* p_pcKey,
                                                            int16_t p_i16Value)
{
    MDNSRESPONDER_I16_TO_CHAR(acBuffer, p_i16Value);
    return addDynamicServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer);
}

/*
    MDNSResponder::addDynamicServiceTxt (int8_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addDynamicServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                            const MDNSResponder::hMDNSService p_hMDNSService,
                                                            const char* p_pcKey,
                                                            int8_t p_i8Value)
{
    MDNSRESPONDER_I8_TO_CHAR(acBuffer, p_i8Value);
    return addDynamicServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer);
}


/**
    QUERIES
*/


/**
    MDNSResponder::clsMDNSAnswerAccessor

*/

/*
    MDNSResponder::clsMDNSAnswerAccessor::clsMDNSAnswerAccessor constructor
*/
MDNSResponder::clsMDNSAnswerAccessor::clsMDNSAnswerAccessor(const MDNSResponder::clsHost::stcQuery::stcAnswer* p_pAnswer)
    :   m_pAnswer(p_pAnswer)
{
    if ((m_pAnswer) &&
        (txtsAvailable()))
    {
        // Prepare m_TxtKeyValueMap
        for (const clsHost::stcServiceTxt* pTxt = m_pAnswer->m_Txts.m_pTxts; pTxt; pTxt = pTxt->m_pNext)
        {
            m_TxtKeyValueMap.emplace(std::pair<const char*, const char*>(pTxt->m_pcKey, pTxt->m_pcValue));
        }
    }
}

/*
    MDNSResponder::clsMDNSAnswerAccessor::~clsMDNSAnswerAccessor destructor
*/
MDNSResponder::clsMDNSAnswerAccessor::~clsMDNSAnswerAccessor(void)
{
}

/**
    MDNSResponder::clsMDNSAnswerAccessor::stcCompareTxtKey
*/

/*
    MDNSResponder::clsMDNSAnswerAccessor::stcCompareTxtKey::operator()
*/
bool MDNSResponder::clsMDNSAnswerAccessor::stcCompareTxtKey::operator()(char const* p_pA,
                                                                        char const* p_pB) const
{
    return (0 > strcasecmp(p_pA, p_pB));
}

/*
    MDNSResponder::clsMDNSAnswerAccessor::serviceDomainAvailable
*/
bool MDNSResponder::clsMDNSAnswerAccessor::serviceDomainAvailable(void) const
{
    return ((m_pAnswer) &&
            (m_pAnswer->m_QueryAnswerFlags & static_cast<typeQueryAnswerType>(enuQueryAnswerType::ServiceDomain)));
}

/*
    MDNSResponder::clsMDNSAnswerAccessor::serviceDomain
*/
const char* MDNSResponder::clsMDNSAnswerAccessor::serviceDomain(void) const
{
    return ((m_pAnswer)
            ?   m_pAnswer->m_ServiceDomain.c_str()
            : 0);
}

/*
    MDNSResponder::clsMDNSAnswerAccessor::hostDomainAvailable
*/
bool MDNSResponder::clsMDNSAnswerAccessor::hostDomainAvailable(void) const
{
    return ((m_pAnswer) &&
            (m_pAnswer->m_QueryAnswerFlags & static_cast<typeQueryAnswerType>(enuQueryAnswerType::HostDomain)));
}

/*
    MDNSResponder::clsMDNSAnswerAccessor::hostDomain
*/
const char* MDNSResponder::clsMDNSAnswerAccessor::hostDomain(void) const
{
    return ((m_pAnswer)
            ?   m_pAnswer->m_HostDomain.c_str()
            : 0);
}

/*
    MDNSResponder::clsMDNSAnswerAccessor::hostPortAvailable
*/
bool MDNSResponder::clsMDNSAnswerAccessor::hostPortAvailable(void) const
{
    return ((m_pAnswer) &&
            (m_pAnswer->m_QueryAnswerFlags & static_cast<typeQueryAnswerType>(enuQueryAnswerType::Port)));
}

/*
    MDNSResponder::clsMDNSAnswerAccessor::hostPort
*/
uint16_t MDNSResponder::clsMDNSAnswerAccessor::hostPort(void) const
{
    return ((m_pAnswer)
            ? (m_pAnswer->m_u16Port)
            : 0);
}

#ifdef MDNS_IPV4_SUPPORT
/*
    MDNSResponder::clsMDNSAnswerAccessor::IPv4AddressAvailable
*/
bool MDNSResponder::clsMDNSAnswerAccessor::IPv4AddressAvailable(void) const
{
    return ((m_pAnswer) &&
            (m_pAnswer->m_QueryAnswerFlags & static_cast<typeQueryAnswerType>(enuQueryAnswerType::IPv4Address)));
}

/*
    MDNSResponder::clsMDNSAnswerAccessor::IPv4Addresses
*/
std::vector<IPAddress> MDNSResponder::clsMDNSAnswerAccessor::IPv4Addresses(void) const
{
    std::vector<IPAddress>  internalIP;
    if ((m_pAnswer) &&
        (IPv4AddressAvailable()))
    {
        for (uint32_t u = 0; u < m_pAnswer->IPv4AddressCount(); ++u)
        {
            const clsHost::stcQuery::stcAnswer::stcIPAddress*    pIPAddr = m_pAnswer->IPv4AddressAtIndex(u);
            if (pIPAddr)
            {
                internalIP.emplace_back(pIPAddr->m_IPAddress);
            }
        }
    }
    return internalIP;
}
#endif

#ifdef MDNS_IPV6_SUPPORT
/*
    MDNSResponder::clsMDNSAnswerAccessor::IPv6AddressAvailable
*/
bool MDNSResponder::clsMDNSAnswerAccessor::IPv6AddressAvailable(void) const
{
    return ((m_pAnswer) &&
            (m_pAnswer->m_QueryAnswerFlags & static_cast<typeQueryAnswerType>(enuQueryAnswerType::IPv6Address)));
}

/*
    MDNSResponder::clsMDNSAnswerAccessor::IPv6Addresses
*/
std::vector<IPAddress> MDNSResponder::clsMDNSAnswerAccessor::IPv6Addresses(void) const
{
    std::vector<IPAddress>  internalIP;
    if ((m_pAnswer) &&
        (IPv6AddressAvailable()))
    {
        for (uint32_t u = 0; u < m_pAnswer->IPv6AddressCount(); ++u)
        {
            const clsHost::stcQuery::stcAnswer::stcIPAddress*    pIPAddr = m_pAnswer->IPv6AddressAtIndex(u);
            if (pIPAddr)
            {
                internalIP.emplace_back(pIPAddr->m_IPAddress);
            }
        }
    }
    return internalIP;
}
#endif

/*
    MDNSResponder::clsMDNSAnswerAccessor::txtsAvailable
*/
bool MDNSResponder::clsMDNSAnswerAccessor::txtsAvailable(void) const
{
    return ((m_pAnswer) &&
            (m_pAnswer->m_QueryAnswerFlags & static_cast<typeQueryAnswerType>(enuQueryAnswerType::Txts)));
}

/*
    MDNSResponder::clsMDNSAnswerAccessor::txts

    Returns all TXT items for the given service as a ';'-separated string.
    If not already existing; the string is alloced, filled and attached to the answer.
*/
const char* MDNSResponder::clsMDNSAnswerAccessor::txts(void) const
{
    return ((m_pAnswer)
            ?   m_pAnswer->m_Txts.c_str()
            : 0);
}

/*
    MDNSResponder::clsMDNSAnswerAccessor::txtKeyValues
*/
const MDNSResponder::clsMDNSAnswerAccessor::clsTxtKeyValueMap& MDNSResponder::clsMDNSAnswerAccessor::txtKeyValues(void) const
{
    return m_TxtKeyValueMap;
}

/*
    MDNSResponder::clsMDNSAnswerAccessor::txtValue
*/
const char* MDNSResponder::clsMDNSAnswerAccessor::txtValue(const char* p_pcKey) const
{
    char*   pcResult = 0;

    if (m_pAnswer)
    {
        for (const clsHost::stcServiceTxt* pTxt = m_pAnswer->m_Txts.m_pTxts; pTxt; pTxt = pTxt->m_pNext)
        {
            if ((p_pcKey) &&
                (0 == strcasecmp(pTxt->m_pcKey, p_pcKey)))
            {
                pcResult = pTxt->m_pcValue;
                break;
            }
        }
    }
    return pcResult;
}

/*
    MDNSResponder::clsMDNSAnswerAccessor::printTo
 **/
size_t MDNSResponder::clsMDNSAnswerAccessor::printTo(Print& p_Print) const
{
    size_t      stLen = 0;
    const char* cpcI = " * ";
    const char* cpcS = "  ";

    stLen += p_Print.println(" * * * * *");
    if (hostDomainAvailable())
    {
        stLen += p_Print.print(cpcI);
        stLen += p_Print.print("Host domain: ");
        stLen += p_Print.println(hostDomain());
    }
#ifdef MDNS_IPV4_SUPPORT
    if (IPv4AddressAvailable())
    {
        stLen += p_Print.print(cpcI);
        stLen += p_Print.println("IPv4 address(es):");
        for (const IPAddress& addr : IPv4Addresses())
        {
            stLen += p_Print.print(cpcI);
            stLen += p_Print.print(cpcS);
            stLen += p_Print.println(addr);
        }
    }
#endif
#ifdef MDNS_IPV6_SUPPORT
    if (IPv6AddressAvailable())
    {
        stLen += p_Print.print(cpcI);
        stLen += p_Print.println("IPv6 address(es):");
        for (const IPAddress& addr : IPv6Addresses())
        {
            stLen += p_Print.print(cpcI);
            stLen += p_Print.print(cpcS);
            stLen += p_Print.println(addr);
        }
    }
#endif
    if (serviceDomainAvailable())
    {
        stLen += p_Print.print(cpcI);
        stLen += p_Print.print("Service domain: ");
        stLen += p_Print.println(serviceDomain());
    }
    if (hostPortAvailable())
    {
        stLen += p_Print.print(cpcI);
        stLen += p_Print.print("Host port: ");
        stLen += p_Print.println(hostPort());
    }
    if (txtsAvailable())
    {
        stLen += p_Print.print(cpcI);
        stLen += p_Print.print("TXTs:");
        for (auto const& x : txtKeyValues())
        {
            stLen += p_Print.print(cpcI);
            stLen += p_Print.print(cpcS);
            stLen += p_Print.print(x.first);
            stLen += p_Print.print("=");
            stLen += p_Print.println(x.second);
        }
    }
    stLen += p_Print.println(" * * * * *");

    return stLen;
}

/**
    STATIC QUERIES
*/

/*
    MDNSResponder::queryService

    Perform a (blocking) static service query.
    The arrived answers can be queried by calling:
    - answerHostName (or 'hostname')
    - answerIP (or 'IP')
    - answerPort (or 'port')

*/
uint32_t MDNSResponder::queryService(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                     const char* p_pcService,
                                     const char* p_pcProtocol,
                                     const uint16_t p_u16Timeout /*= MDNS_QUERYSERVICES_WAIT_TIME*/)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s queryService '_%s._%s.local'\n"), _DH(p_hMDNSHost), p_pcService, p_pcProtocol););

    uint32_t    u32Result = ((_validateMDNSHostHandle(p_hMDNSHost))
                             ? (_NRH2Ptr(p_hMDNSHost)->queryService(p_pcService, p_pcProtocol, p_u16Timeout))
                             : 0);
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s queryService '_%s._%s.local' returned %u hits!\n"), _DH(p_hMDNSHost), p_pcService, p_pcProtocol, u32Result););
    return u32Result;
}

/*
    MDNSResponder::queryHost

    Perform a (blocking) static host query.
    The arrived answers can be queried by calling:
    - answerHostName (or 'hostname')
    - answerIP (or 'IP')
    - answerPort (or 'port')

*/
uint32_t MDNSResponder::queryHost(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                  const char* p_pcHostName,
                                  const uint16_t p_u16Timeout /*= MDNS_QUERYSERVICES_WAIT_TIME*/)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s queryHost '%s.local'\n"), _DH(p_hMDNSHost), p_pcHostName););

    uint32_t    u32Result = ((_validateMDNSHostHandle(p_hMDNSHost))
                             ? (_NRH2Ptr(p_hMDNSHost)->queryHost(p_pcHostName, p_u16Timeout))
                             : 0);
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s queryHost '%s.local' returned %u hits!\n"), _DH(p_hMDNSHost), p_pcHostName, u32Result););
    return u32Result;
}

/*
    MDNSResponder::removeQuery

    Remove the last static query (and all answers).

*/
bool MDNSResponder::removeQuery(const MDNSResponder::hMDNSHost p_hMDNSHost)
{
    return ((_validateMDNSHostHandle(p_hMDNSHost)) &&
            (_NRH2Ptr(p_hMDNSHost)->removeQuery()));
}

/*
    MDNSResponder::hasQuery

    Return 'true', if a static query is currently installed

*/
bool MDNSResponder::hasQuery(const MDNSResponder::hMDNSHost p_hMDNSHost)
{
    return ((_validateMDNSHostHandle(p_hMDNSHost)) &&
            (0 != _NRH2Ptr(p_hMDNSHost)->hasQuery()));
}

/*
    MDNSResponder::getQuery

    Return handle to the last static query

*/
MDNSResponder::hMDNSQuery MDNSResponder::getQuery(const MDNSResponder::hMDNSHost p_hMDNSHost)
{
    return (_validateMDNSHostHandle(p_hMDNSHost)
            ? (hMDNSQuery)_NRH2Ptr(p_hMDNSHost)->getQuery()
            : 0);
}


/*
    MDNSResponder::answerAccessors
*/
MDNSResponder::clsMDNSAnswerAccessorVector MDNSResponder::answerAccessors(const MDNSResponder::hMDNSHost p_hMDNSHost)
{
    hMDNSQuery  hLegacyQuery = getQuery(p_hMDNSHost);
    return ((hLegacyQuery)
            ? answerAccessors(p_hMDNSHost, hLegacyQuery)
            : clsMDNSAnswerAccessorVector());
}

/*
    MDNSResponder::answerCount
*/
uint32_t MDNSResponder::answerCount(const MDNSResponder::hMDNSHost p_hMDNSHost)
{
    hMDNSQuery  hLegacyQuery = getQuery(p_hMDNSHost);
    return ((hLegacyQuery)
            ? answerCount(p_hMDNSHost, hLegacyQuery)
            : 0);
}

/*
    MDNSResponder::answerAccessor
*/
MDNSResponder::clsMDNSAnswerAccessor MDNSResponder::answerAccessor(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                                   uint32_t p_u32AnswerIndex)
{
    hMDNSQuery  hLegacyQuery = getQuery(p_hMDNSHost);
    return ((hLegacyQuery)
            ? answerAccessor(p_hMDNSHost, hLegacyQuery, p_u32AnswerIndex)
            : clsMDNSAnswerAccessor(0));
}



#ifdef NOTUSED

/*
    MDNSResponder::answerHostName
*/
const char* MDNSResponder::answerHostName(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                          const uint32_t p_u32AnswerIndex)
{
    const stcQuery*            pQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findLegacyQuery(*(clsHost*)p_hMDNSHost) : 0);
    stcQuery::stcAnswer* pSQAnswer = (pQuery ? pQuery->answerAtIndex(p_u32AnswerIndex) : 0);

    if ((pSQAnswer) &&
        (pSQAnswer->m_HostDomain.m_u16NameLength) &&
        (!pSQAnswer->m_pcHostDomain))
    {
        char*   pcHostDomain = pSQAnswer->allocHostDomain(pSQAnswer->m_HostDomain.c_strLength());
        if (pcHostDomain)
        {
            pSQAnswer->m_HostDomain.c_str(pcHostDomain);
        }
    }
    return (pSQAnswer ? pSQAnswer->m_pcHostDomain : 0);
}

#ifdef MDNS_IPV4_SUPPORT
/*
    MDNSResponder::answerIPv4
*/
IPAddress MDNSResponder::answerIPv4(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                    const uint32_t p_u32AnswerIndex)
{
    const stcQuery*            					pQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findLegacyQuery(*(clsHost*)p_hMDNSHost) : 0);
    const stcQuery::stcAnswer*                  pSQAnswer = (pQuery ? pQuery->answerAtIndex(p_u32AnswerIndex) : 0);
    const stcQuery::stcAnswer::stcIPv4Address*	pIPv4Address = (((pSQAnswer) && (pSQAnswer->m_pIPv4Addresses)) ? pSQAnswer->IPv4AddressAtIndex(0) : 0);
    return (pIPv4Address ? pIPv4Address->m_IPAddress : IPAddress());
}
#endif

#ifdef MDNS_IPV6_SUPPORT
/*
    MDNSResponder::answerIPv6
*/
IPAddress MDNSResponder::answerIPv6(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                    const uint32_t p_u32AnswerIndex)
{
    const stcQuery*            					pQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findLegacyQuery(*(clsHost*)p_hMDNSHost) : 0);
    const stcQuery::stcAnswer*                  pSQAnswer = (pQuery ? pQuery->answerAtIndex(p_u32AnswerIndex) : 0);
    const stcQuery::stcAnswer::stcIPv6Address*	pIPv6Address = (((pSQAnswer) && (pSQAnswer->m_pIPv6Addresses)) ? pSQAnswer->IPv6AddressAtIndex(0) : 0);
    return (pIPv6Address ? pIPv6Address->m_IPAddress : IPAddress());
}
#endif

/*
    MDNSResponder::answerPort
*/
uint16_t MDNSResponder::answerPort(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                   const uint32_t p_u32AnswerIndex)
{
    const stcQuery*            	pQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findLegacyQuery(*(clsHost*)p_hMDNSHost) : 0);
    const stcQuery::stcAnswer*  pSQAnswer = (pQuery ? pQuery->answerAtIndex(p_u32AnswerIndex) : 0);
    return (pSQAnswer ? pSQAnswer->m_u16Port : 0);
}

#endif


/**
    DYNAMIC SERVICE QUERY
*/

/*
    MDNSResponder::installServiceQuery

    Add a dynamic service query and a corresponding callback to the MDNS responder.
    The callback will be called for every answer update.
    The answers can also be queried by calling:
    - answerServiceDomain
    - answerHostDomain
    - answerIPv4Address/answerIPv6Address
    - answerPort
    - answerTxts

*/
MDNSResponder::hMDNSQuery MDNSResponder::installServiceQuery(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                             const char* p_pcService,
                                                             const char* p_pcProtocol,
                                                             MDNSResponder::MDNSQueryCallbackFn p_fnCallback)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s queryService '_%s._%s.local'\n"), _DH(p_hMDNSHost), p_pcService, p_pcProtocol););

    hMDNSQuery  hResult = ((_validateMDNSHostHandle(p_hMDNSHost))
                           ? (_NRH2Ptr(p_hMDNSHost)->installServiceQuery(p_pcService, p_pcProtocol, [this, p_fnCallback](clsHost & p_rHost,
                                                                                                                         const clsHost::stcQuery & p_Query,
                                                                                                                         const clsHost::stcQuery::stcAnswer & p_Answer,
                                                                                                                         clsHost::typeQueryAnswerType p_QueryAnswerTypeFlags,          // flags for the updated answer item
                                                                                                                         bool p_bSetContent)->void
    {
        if (p_fnCallback)
        {
            p_fnCallback(*this, (hMDNSHost)&p_rHost, (hMDNSQuery)&p_Query, clsMDNSAnswerAccessor(&p_Answer), p_QueryAnswerTypeFlags, p_bSetContent);
        }
    }))
    : 0);
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s installServiceQuery: %s for '_%s._%s.local'!\n\n"), _DH(p_hMDNSHost), (hResult ? "Succeeded" : "FAILED"), (p_pcService ? : "-"), (p_pcProtocol ? : "-")););
    DEBUG_EX_ERR(if (!hResult) DEBUG_OUTPUT.printf_P(PSTR("%s installServiceQuery: FAILED for '_%s._%s.local'!\n\n"), _DH(p_hMDNSHost), (p_pcService ? : "-"), (p_pcProtocol ? : "-")););
    return hResult;
}

/*
    MDNSResponder::installHostQuery
*/
MDNSResponder::hMDNSQuery MDNSResponder::installHostQuery(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                          const char* p_pcHostName,
                                                          MDNSResponder::MDNSQueryCallbackFn p_fnCallback)
{
    hMDNSQuery  hResult = ((_validateMDNSHostHandle(p_hMDNSHost))
                           ? (_NRH2Ptr(p_hMDNSHost)->installHostQuery(p_pcHostName, [this, p_fnCallback](clsHost & p_rHost,
                                                                                                         const clsHost::stcQuery & p_Query,
                                                                                                         const clsHost::stcQuery::stcAnswer & p_Answer,
                                                                                                         clsHost::typeQueryAnswerType p_QueryAnswerTypeFlags,          // flags for the updated answer item
                                                                                                         bool p_bSetContent)->void
    {
        if (p_fnCallback)
        {
            p_fnCallback(*this, (hMDNSHost)&p_rHost, (hMDNSQuery)&p_Query, clsMDNSAnswerAccessor(&p_Answer), p_QueryAnswerTypeFlags, p_bSetContent);
        }
    }))
    : 0);
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s installHostQuery: %s for '%s.local'!\n\n"), _DH(p_hMDNSHost), (hResult ? "Succeeded" : "FAILED"), (p_pcHostName ? : "-")););
    DEBUG_EX_ERR(if (!hResult) DEBUG_OUTPUT.printf_P(PSTR("%s installHostQuery: FAILED for '%s.local'!\n\n"), _DH(p_hMDNSHost), (p_pcHostName ? : "-")););
    return hResult;
}

/*
    MDNSResponder::removeQuery

    Remove a dynamic query (and all collected answers) from the MDNS responder

*/
bool MDNSResponder::removeQuery(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                const MDNSResponder::hMDNSQuery p_hMDNSQuery)
{
    bool    bResult = ((_validateMDNSHostHandle(p_hMDNSHost)) &&
                       (p_hMDNSQuery) &&
                       (_NRH2Ptr(p_hMDNSHost)->removeQuery((clsHost::stcQuery*)p_hMDNSQuery)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s removeQuery: FAILED!\n"), _DH(p_hMDNSHost)););
    return bResult;
}

/*
    MDNSResponder::answerAccessors
*/
MDNSResponder::clsMDNSAnswerAccessorVector MDNSResponder::answerAccessors(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                                          const MDNSResponder::hMDNSQuery p_hMDNSQuery)
{
    clsMDNSAnswerAccessorVector tempVector;
    for (uint32_t u = 0; u < answerCount(p_hMDNSHost, p_hMDNSQuery); ++u)
    {
        clsHost::stcQuery::stcAnswer* pAnswer = 0;
        if ((_validateMDNSHostHandle(p_hMDNSHost)) &&
            (p_hMDNSQuery) &&
            ((pAnswer = ((clsHost::stcQuery*)p_hMDNSQuery)->answerAtIndex(u))))
        {
            tempVector.emplace_back(clsMDNSAnswerAccessor(pAnswer));
            //tempVector.emplace_back(*pAnswer);
        }
    }
    return tempVector;
}

/*
    MDNSResponder::answerCount
*/
uint32_t MDNSResponder::answerCount(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                    const MDNSResponder::hMDNSQuery p_hMDNSQuery)
{
    _validateMDNSHostHandle(p_hMDNSHost);
    return ((clsHost::stcQuery*)p_hMDNSQuery)->answerCount();
}

/*
    MDNSResponder::answerAccessor
*/
MDNSResponder::clsMDNSAnswerAccessor MDNSResponder::answerAccessor(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                                   const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                   uint32_t p_u32AnswerIndex)
{
    clsHost::stcQuery::stcAnswer* pAnswer = (((_validateMDNSHostHandle(p_hMDNSHost)) &&
                                              (p_hMDNSQuery))
                                             ? ((clsHost::stcQuery*)p_hMDNSQuery)->answerAtIndex(p_u32AnswerIndex)
                                             : 0);
    return MDNSResponder::clsMDNSAnswerAccessor(pAnswer);
}

#ifdef LATER
/*
    MDNSResponder::hasAnswerServiceDomain
*/
bool MDNSResponder::hasAnswerServiceDomain(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                           const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                           const uint32_t p_u32AnswerIndex)
{
    stcQuery*         		pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer*	pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          return ((pSQAnswer) &&
                                                  (pSQAnswer->m_QueryAnswerFlags & static_cast<typeQueryAnswerType>(enuQueryAnswerType::ServiceDomain)));
}

                                     /*
                                         MDNSResponder::answerServiceDomain

                                         Returns the domain for the given service.
                                         If not already existing, the string is allocated, filled and attached to the answer.

                                     */
                                     const char* MDNSResponder::answerServiceDomain(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                                                    const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                                    const uint32_t p_u32AnswerIndex)
{
    stcQuery*        		pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer*	pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          // Fill m_pcServiceDomain (if not already done)
                                          if ((pSQAnswer) &&
                                              (pSQAnswer->m_ServiceDomain.m_u16NameLength) &&
                                              (!pSQAnswer->m_pcServiceDomain))
{

    pSQAnswer->m_pcServiceDomain = pSQAnswer->allocServiceDomain(pSQAnswer->m_ServiceDomain.c_strLength());
        if (pSQAnswer->m_pcServiceDomain)
        {
            pSQAnswer->m_ServiceDomain.c_str(pSQAnswer->m_pcServiceDomain);
        }
    }
    return (pSQAnswer ? pSQAnswer->m_pcServiceDomain : 0);
}

/*
    MDNSResponder::hasAnswerHostDomain
*/
bool MDNSResponder::hasAnswerHostDomain(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                        const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                        const uint32_t p_u32AnswerIndex)
{
    stcQuery*        		pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer*	pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          return ((pSQAnswer) &&
                                                  (pSQAnswer->m_QueryAnswerFlags & static_cast<typeQueryAnswerType>(enuQueryAnswerType::HostDomain)));
}

                                     /*
                                         MDNSResponder::answerHostDomain

                                         Returns the host domain for the given service.
                                         If not already existing, the string is allocated, filled and attached to the answer.

                                     */
                                     const char* MDNSResponder::answerHostDomain(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                                                 const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                                 const uint32_t p_u32AnswerIndex)
{
    stcQuery*        		pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer* 	pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          // Fill m_pcHostDomain (if not already done)
                                          if ((pSQAnswer) &&
                                              (pSQAnswer->m_HostDomain.m_u16NameLength) &&
                                              (!pSQAnswer->m_pcHostDomain))
{

    pSQAnswer->m_pcHostDomain = pSQAnswer->allocHostDomain(pSQAnswer->m_HostDomain.c_strLength());
        if (pSQAnswer->m_pcHostDomain)
        {
            pSQAnswer->m_HostDomain.c_str(pSQAnswer->m_pcHostDomain);
        }
    }
    return (pSQAnswer ? pSQAnswer->m_pcHostDomain : 0);
}

#ifdef MDNS_IPV4_SUPPORT
/*
    MDNSResponder::hasAnswerIPv4Address
*/
bool MDNSResponder::hasAnswerIPv4Address(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                         const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                         const uint32_t p_u32AnswerIndex)
{
    stcQuery*        		pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer* 	pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          return ((pSQAnswer) &&
                                                  (pSQAnswer->m_QueryAnswerFlags & static_cast<typeQueryAnswerType>(enuQueryAnswerType::IPv4Address)));
}

                                     /*
                                         MDNSResponder::answerIPv4AddressCount
                                     */
                                     uint32_t MDNSResponder::answerIPv4AddressCount(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                                                    const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                                    const uint32_t p_u32AnswerIndex)
{
    stcQuery*        		pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer* 	pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          return (pSQAnswer ? pSQAnswer->IPv4AddressCount() : 0);
}

                                     /*
                                         MDNSResponder::answerIPv4Address
                                     */
                                     IPAddress MDNSResponder::answerIPv4Address(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                                                const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                                const uint32_t p_u32AnswerIndex,
                                                                                const uint32_t p_u32AddressIndex)
{
    stcQuery*        						pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                                          stcQuery::stcAnswer*                 	pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                                          stcQuery::stcAnswer::stcIPv4Address* 	pIPv4Address = (pSQAnswer ? pSQAnswer->IPv4AddressAtIndex(p_u32AddressIndex) : 0);
                                                          return (pIPv4Address ? pIPv4Address->m_IPAddress : IPAddress());
}
#endif

#ifdef MDNS_IPV6_SUPPORT
                                                     /*
                                                         MDNSResponder::hasAnswerIPv6Address
                                                     */
                                                     bool MDNSResponder::hasAnswerIPv6Address(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                                                              const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                                              const uint32_t p_u32AnswerIndex)
{
    stcQuery*        		pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer*	pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          return ((pSQAnswer) &&
                                                  (pSQAnswer->m_QueryAnswerFlags & static_cast<typeQueryAnswerType>(enuQueryAnswerType::IPv6Address)));
}

                                     /*
                                         MDNSResponder::answerIPv6AddressCount
                                     */
                                     uint32_t MDNSResponder::answerIPv6AddressCount(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                                                    const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                                    const uint32_t p_u32AnswerIndex)
{
    stcQuery*        		pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer* 	pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          return (pSQAnswer ? pSQAnswer->IPv6AddressCount() : 0);
}

                                     /*
                                         MDNSResponder::answerIPv6Address
                                     */
                                     IPAddress MDNSResponder::answerIPv6Address(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                                                const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                                const uint32_t p_u32AnswerIndex,
                                                                                const uint32_t p_u32AddressIndex)
{
    stcQuery*        						pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                                          stcQuery::stcAnswer*                 	pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                                          stcQuery::stcAnswer::stcIPv6Address*  	pIPv6Address = (pSQAnswer ? pSQAnswer->IPv6AddressAtIndex(p_u32AddressIndex) : 0);
                                                          return (pIPv6Address ? pIPv6Address->m_IPAddress : IPAddress());
}
#endif

                                                     /*
                                                         MDNSResponder::hasAnswerPort
                                                     */
                                                     bool MDNSResponder::hasAnswerPort(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                                                       const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                                       const uint32_t p_u32AnswerIndex)
{
    stcQuery*        		pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer* 	pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          return ((pSQAnswer) &&
                                                  (pSQAnswer->m_QueryAnswerFlags & static_cast<typeQueryAnswerType>(enuQueryAnswerType::Port)));
}

                                     /*
                                         MDNSResponder::answerPort
                                     */
                                     uint16_t MDNSResponder::answerPort(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                                        const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                        const uint32_t p_u32AnswerIndex)
{
    stcQuery*        		pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer* 	pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          return (pSQAnswer ? pSQAnswer->m_u16Port : 0);
}

                                     /*
                                         MDNSResponder::hasAnswerTxts
                                     */
                                     bool MDNSResponder::hasAnswerTxts(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                                       const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                       const uint32_t p_u32AnswerIndex)
{
    stcQuery*        		pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer* 	pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          return ((pSQAnswer) &&
                                                  (pSQAnswer->m_QueryAnswerFlags & static_cast<typeQueryAnswerType>(enuQueryAnswerType::Txts)));
}

                                     /*
                                         MDNSResponder::answerTxts

                                         Returns all TXT items for the given service as a ';'-separated string.
                                         If not already existing; the string is alloced, filled and attached to the answer.

                                     */
                                     const char* MDNSResponder::answerTxts(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                                           const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                           const uint32_t p_u32AnswerIndex)
{
    stcQuery*        		pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer* 	pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          // Fill m_pcTxts (if not already done)
                                          if ((pSQAnswer) &&
                                              (pSQAnswer->m_Txts.m_pTxts) &&
                                              (!pSQAnswer->m_pcTxts))
{

    pSQAnswer->m_pcTxts = pSQAnswer->allocTxts(pSQAnswer->m_Txts.c_strLength());
        if (pSQAnswer->m_pcTxts)
        {
            pSQAnswer->m_Txts.c_str(pSQAnswer->m_pcTxts);
        }
    }
    return (pSQAnswer ? pSQAnswer->m_pcTxts : 0);
}


/*
    PROBING
*/

/*
    MDNSResponder::setHostProbeResultCallback

    Set a callback for probe results. The callback is called, when probing
    for the host domain failes or succeedes.
    In the case of failure, the domain name should be changed via 'setHostName'
    When succeeded, the host domain will be announced by the MDNS responder.

*/
bool MDNSResponder::setHostProbeResultCallback(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                               MDNSResponder::MDNSHostProbeResultCallbackFn p_fnCallback)
{
    return ((_validateMDNSHostHandle(p_hMDNSHost)) &&
            (((clsHost*)p_hMDNSHost)->m_HostProbeInformation.m_fnProbeResultCallback = p_fnCallback, true));
}


/*
    MISC
*/

/*
    MDNSResponder::notifyNetIfChange

    Should be called, whenever the AP for the MDNS responder changes.
    A bit of this is caught by the event callbacks installed in the constructor.

*/
bool MDNSResponder::notifyNetIfChange(netif* p_pNetIf)
{
    clsHost*	pMDNSHost;
    return (((pMDNSHost = _findHost(p_pNetIf))) &&
            (_restart(*pMDNSHost)));
}

/*
    MDNSResponder::update

    Should be called in every 'loop'.

*/
bool MDNSResponder::update(const MDNSResponder::hMDNSHost p_hMDNSHost)
{
    return ((_validateMDNSHostHandle(p_hMDNSHost)) &&
            (_process(*(clsHost*)p_hMDNSHost, true)));
}

/*
	MDNSResponder::update (convenience)
*/
bool MDNSResponder::update(void)
{
    bool	bResult = true;
    for (clsHost*& pMDNSHost : m_HostList)
    {
        if (!_process(*it, true))
        {
            bResult = false;
        }
    }
    return bResult;
}

/*
    MDNSResponder::announce (convenience)

    Should be called, if the 'configuration' changes. Mainly this will be changes in the TXT items...
*/
bool MDNSResponder::announce(const MDNSResponder::hMDNSHost p_hMDNSHost)
{
    return ((_validateMDNSHostHandle(p_hMDNSHost)) &&
            (_announce(*(clsHost*)p_hMDNSHost, true, true)));
}

/*
    MDNSResponder::announce (convenience)
*/
bool MDNSResponder::announce(void)
{
    bool	bResult = true;
    for (clsHost*& pMDNSHost : m_HostList)
    {
        if (!_announce(*it, true, true))
        {
            bResult = false;
        }
    }
    return bResult;
}

/*
    MDNSResponder::enableArduino

    Enable the OTA update service.

*/
MDNSResponder::hMDNSService MDNSResponder::enableArduino(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                         uint16_t p_u16Port,
                                                         bool p_bAuthUpload /*= false*/)
{
    hMDNSService    hService = addService(p_hMDNSHost, 0, "arduino", "tcp", p_u16Port);
    if (hService)
    {
        if ((!addServiceTxt(p_hMDNSHost, hService, "tcp_check", "no")) ||
            (!addServiceTxt(p_hMDNSHost, hService, "ssh_upload", "no")) ||
            (!addServiceTxt(p_hMDNSHost, hService, "board", STRINGIZE_VALUE_OF(ARDUINO_BOARD))) ||
            (!addServiceTxt(p_hMDNSHost, hService, "auth_upload", (p_bAuthUpload) ? "yes" : "no")))
        {

            removeService(p_hMDNSHost, hService);
            hService = 0;
        }
    }
    return hService;
}
#endif  // LATER

#ifdef __MDNS_USE_LEGACY

/**
    INTERFACE
*/

/**
    MDNSResponder::MDNSResponder
*/
MDNSResponder::MDNSResponder(void)
    :   m_pUDPContext(0)
{
}

/*
    MDNSResponder::~MDNSResponder
*/
MDNSResponder::~MDNSResponder(void)
{
    close();
}


/*
    MDNSResponder::getHost (netif)
*/
MDNSResponder::hMDNSHost MDNSResponder::getHost(netif* p_pNetIf) const
{
    return (hMDNSHost)(p_pNetIf ? _findHost(p_pNetIf) : 0);
}

/*
    MDNSResponder::getHost (WiFiMode)
*/
MDNSResponder::hMDNSHost MDNSResponder::getHost(WiFiMode_t p_WiFiMode) const
{
    hMDNSHost   hResult = 0;

    if (WIFI_STA == p_WiFiMode)
    {
        hResult = getHost(netif_get_by_index(WIFI_STA));
    }
    else if (WIFI_AP == p_WiFiMode)
    {
        hResult = getHost(netif_get_by_index(WIFI_AP));
    }
    return hResult;
}

/*
    MDNSResponder::begin (hostname, netif, probe_callback)
*/
MDNSResponder::hMDNSHost MDNSResponder::begin(const char* p_pcHostName,
                                              netif* p_pNetIf,
                                              MDNSHostProbeResultCallbackFn p_fnCallback /*= 0*/)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s begin(%s, netif: %u)\n"), _DH(), (p_pcHostName ? : "_"), (p_pNetIf ? netif_get_index(p_pNetIf) : 0)););

    return (hMDNSHost)_begin(p_pcHostName, p_pNetIf, p_fnCallback);
}

/*
    MDNSResponder::begin (hostname, probe_callback)
*/
bool MDNSResponder::begin(const char* p_pcHostName,
                          MDNSHostProbeResultCallbackFn p_fnCallback /*= 0*/)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s begin(%s)\n"), _DH(), (p_pcHostName ? : "_")););

    return begin(p_pcHostName, (WiFiMode_t)wifi_get_opmode(), p_fnCallback);
}

/*
    MDNSResponder::begin (hostname, WiFiMode, probe_callback)
*/
bool MDNSResponder::begin(const char* p_pcHostName,
                          WiFiMode_t p_WiFiMode,
                          MDNSHostProbeResultCallbackFn p_fnCallback /*= 0*/)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s begin(%s, opmode: %u)\n"), _DH(), (p_pcHostName ? : "_"), (uint32_t)p_WiFiMode););

    bool bResult = true;

    if ((bResult) &&
        (p_WiFiMode & WIFI_STA))
    {
        bResult = (0 != _begin(p_pcHostName, netif_get_by_index(WIFI_STA), p_fnCallback));
    }
    if ((bResult) &&
        (p_WiFiMode & WIFI_AP))
    {
        bResult = (0 != _begin(p_pcHostName, netif_get_by_index(WIFI_AP), p_fnCallback));
    }
    return bResult;
}

/*
    MDNSResponder::close
*/
bool MDNSResponder::close(const MDNSResponder::hMDNSHost p_hMDNSHost)
{
    return ((_validateMDNSHostHandle(p_hMDNSHost)) &&
            (_close(*(clsHost*)p_hMDNSHost)));
}

/*
    MDNSResponder::close (convenience)
*/
bool MDNSResponder::close(void)
{
    clsHostList::iterator   it(m_HostList.begin());
    while (m_HostList.end() != it)
    {
        _close(**it++);
    }

    _releaseUDPContext();

    return true;
}

/*
    MDNSResponder::end (ESP32)
*/
bool MDNSResponder::end(void)
{
    return close();
}

/*
    MDNSResponder::setHostName
*/
bool MDNSResponder::setHostName(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                const char* p_pcHostName)
{
    bool    bResult = ((_validateMDNSHostHandle(p_hMDNSHost)) &&
                       (_setHostName(*(clsHost*)p_hMDNSHost, p_pcHostName)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s setHostName: FAILED for '%s'!\n"), _DH(), (p_pcHostName ? : "-")););
    return bResult;
}

/*
    MDNSResponder::setHostname (LEGACY 2)

    Set the host name in all netif bindings

*/
bool MDNSResponder::setHostname(const char* p_pcHostName)
{
    bool    bResult = true;
    for (clsHost*& pMDNSHost : m_HostList)
    {
        if (!setHostName((hMDNSHost)pMDNSHost, p_pcHostName))
        {
            bResult = false;
        }
    }
    return bResult;
}

/*
    MDNSResponder::setHostname (LEGACY)
*/
bool MDNSResponder::setHostname(String p_strHostName)
{
    return setHostname(p_strHostName.c_str());
}

/*
    MDNSResponder::hostName
*/
const char* MDNSResponder::hostName(const MDNSResponder::hMDNSHost p_hMDNSHost) const
{
    return (_validateMDNSHostHandle(p_hMDNSHost)
            ? (((clsHost*)p_hMDNSHost)->m_pcHostName)
            : 0);
}

/*
    MDNSResponder::hostname (LEGACY 2)
*/
const char* MDNSResponder::hostname(void) const
{
    return ((!m_HostList.empty())
            ? hostName((hMDNSHost)m_HostList.front())
            : 0);
}

/*
    MDNSResponder::status
*/
bool MDNSResponder::status(const MDNSResponder::hMDNSHost p_hMDNSHost) const
{
    return ((_validateMDNSHostHandle(p_hMDNSHost)) &&
            (enuProbingStatus::Done == ((clsHost*)p_hMDNSHost)->m_HostProbeInformation.m_ProbingStatus));
}

/*
    MDNSResponder::status (LEGACY 2)
*/
bool MDNSResponder::status(void) const
{
    bool    bResult = true;
    for (clsHost * const& pMDNSHost : m_HostList)
    {
        if (!((bResult = status((hMDNSHost)pMDNSHost))))
        {
            break;
        }
    }
    return bResult;
}


/*
    SERVICES
*/

/*
    MDNSResponder::setInstanceName
*/
bool MDNSResponder::setInstanceName(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                    const char* p_pcInstanceName)
{
    bool    bResult = ((_validateMDNSHostHandle(p_hMDNSHost)) &&
                       (_setInstanceName(*(clsHost*)p_hMDNSHost, p_pcInstanceName)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s setInstanceName: FAILED for '%s'!\n"), _DH(), (p_pcInstanceName ? : "-")););
    return bResult;
}

/*
    MDNSResponder::setInstanceName (LEGACY 2)

    Set the instance name in all netif bindings

*/
bool MDNSResponder::setInstanceName(const char* p_pcInstanceName)
{
    bool    bResult = true;
    for (clsHost*& pMDNSHost : m_HostList)
    {
        if (!setInstanceName((hMDNSHost)pMDNSHost, p_pcInstanceName))
        {
            bResult = false;
        }
    }
    return bResult;
}

/*
    MDNSResponder::setInstanceName (LEGACY 2)
*/
bool MDNSResponder::setInstanceName(const String& p_strInstanceName)
{
    return setInstanceName(p_strInstanceName.c_str());
}

/*
    MDNSResponder::addService

    Add service; using hostname if no name is explicitly provided for the service
    The usual '_' underline, which is prepended to service and protocol, eg. _http,
    may be given. If not, it is added automatically.

*/
MDNSResponder::hMDNSService MDNSResponder::addService(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                      const char* p_pcName,
                                                      const char* p_pcService,
                                                      const char* p_pcProtocol,
                                                      uint16_t p_u16Port)
{
    hMDNSService    hService = (_validateMDNSHostHandle(p_hMDNSHost)
                                ? (hMDNSService)_addService(*(clsHost*)p_hMDNSHost, p_pcName, p_pcService, p_pcProtocol, p_u16Port)
                                : 0);
    DEBUG_EX_ERR(if (!hService) DEBUG_OUTPUT.printf_P(PSTR("%s addService: FAILED for '%s._%s._%s.local'!\n"), _DH((clsHost*)p_hMDNSHost), (p_pcName ? : "-"), (p_pcService ? : "-"), (p_pcProtocol ? : "-")););
    return hService;
}

/*
    MDNSResponder::addService (LEGACY 2)

    Add a service to all netif bindings.
    (Only) the first service (handle) is returned.

*/
MDNSResponder::hMDNSService MDNSResponder::addService(const char* p_pcName,
                                                      const char* p_pcService,
                                                      const char* p_pcProtocol,
                                                      uint16_t p_u16Port)
{
    hMDNSService    hResult = 0;
    for (clsHost*& pMDNSHost : m_HostList)
    {
        hMDNSService    hNewService = addService((hMDNSHost)pMDNSHost, p_pcName, p_pcService, p_pcProtocol, p_u16Port);
        if (!hResult)
        {
            hResult = hNewService;
        }
    }
    return hResult;
}

/*
    MDNSResponder::removeService

    Unanounce a service (by sending a goodbye message) and remove it
    from the MDNS responder

*/
bool MDNSResponder::removeService(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                  const MDNSResponder::hMDNSService p_hMDNSService)
{
    bool    bResult = ((_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)) &&
                       (_removeService(*(clsHost*)p_hMDNSHost, *(stcService*)p_hMDNSService)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s removeService: FAILED!\n"), _DH((clsHost*)p_hMDNSHost)););
    return bResult;
}

/*
    MDNSResponder::removeService (LEGACY 2)

    Find and remove the service from one netif binding
*/
bool MDNSResponder::removeService(const MDNSResponder::hMDNSService p_hMDNSService)
{
    clsHost*    pHost = 0;
    return ((_validateMDNSHostHandle(p_hMDNSService, &pHost)) &&
            (removeService((hMDNSHost)pHost, p_hMDNSService)));
}

/*
    MDNSResponder::removeService
*/
bool MDNSResponder::removeService(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                  const char* p_pcName,
                                  const char* p_pcService,
                                  const char* p_pcProtocol)
{
    clsHost*    pMDNSHost;
    stcService*         pMDNSService;
    bool    bResult = ((_validateMDNSHostHandle(p_hMDNSHost)) &&
                       (pMDNSHost = (clsHost*)p_hMDNSHost) &&
                       ((pMDNSService = _findService(*pMDNSHost, (p_pcName ? : (pMDNSHost->m_pcInstanceName ? : pMDNSHost->m_pcHostName)), p_pcService, p_pcProtocol))) &&
                       (_removeService(*(clsHost*)p_hMDNSHost, *pMDNSService)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s removeService: FAILED for '%s._%s._%s.local'!\n"), _DH((clsHost*)p_hMDNSHost), (p_pcName ? : "-"), (p_pcService ? : "-"), (p_pcProtocol ? : "-")););
    return bResult;
}

/*
    MDNSResponder::removeService (LEGACY 2)
*/
bool MDNSResponder::removeService(const char* p_pcName,
                                  const char* p_pcService,
                                  const char* p_pcProtocol)
{
    bool    bResult = true;

    for (clsHost*& pMDNSHost : m_HostList)
    {
        if (!removeService((hMDNSHost)pMDNSHost, p_pcName, p_pcService, p_pcProtocol))
        {
            bResult = false;
        }
    }
    return bResult;
}

/*
    MDNSResponder::addService (LEGACY)
*/
bool MDNSResponder::addService(String p_strService,
                               String p_strProtocol,
                               uint16_t p_u16Port)
{
    return (0 != addService(0, p_strService.c_str(), p_strProtocol.c_str(), p_u16Port));
}

/*
    MDNSResponder::findService

    Find an existing service.

*/
MDNSResponder::hMDNSService MDNSResponder::findService(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                       const char* p_pcName,
                                                       const char* p_pcService,
                                                       const char* p_pcProtocol)
{
    clsHost*    pMDNSHost;
    return (((_validateMDNSHostHandle(p_hMDNSHost)) &&
             (pMDNSHost = (clsHost*)p_hMDNSHost))
            ? _findService(*pMDNSHost, (p_pcName ? : (pMDNSHost->m_pcInstanceName ? : pMDNSHost->m_pcHostName)), p_pcService, p_pcProtocol)
            : 0);
}

/*
    MDNSResponder::findService (LEGACY 2)

    (Only) the first service handle is returned.

*/
MDNSResponder::hMDNSService MDNSResponder::findService(const char* p_pcName,
                                                       const char* p_pcService,
                                                       const char* p_pcProtocol)
{
    hMDNSService    hResult = 0;
    for (clsHost*& pMDNSHost : m_HostList)
    {
        if ((hResult = findService((hMDNSHost)pMDNSHost, p_pcName, p_pcService, p_pcProtocol)))
        {
            break;
        }
    }
    return hResult;
}

/*
    MDNSResponder::setServiceName
*/
bool MDNSResponder::setServiceName(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                   const MDNSResponder::hMDNSService p_hMDNSService,
                                   const char* p_pcInstanceName)
{
    bool    bResult = ((_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)) &&
                       (_setServiceName(*(clsHost*)p_hMDNSHost, *(stcService*)p_hMDNSService, p_pcInstanceName)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s setServiceName: FAILED for '%s'!\n"), _DH((clsHost*)p_hMDNSHost), (p_pcInstanceName ? : "-")););
    return bResult;
}

/*
    MDNSResponder::setServiceName (LEGACY 2)
*/
bool MDNSResponder::setServiceName(const MDNSResponder::hMDNSService p_hMDNSService,
                                   const char* p_pcInstanceName)
{
    clsHost*    pHost = 0;
    return ((_validateMDNSServiceHandle(p_hMDNSService, &pHost)) &&
            (setServiceName((hMDNSHost)pHost, p_hMDNSService, p_pcInstanceName)));
}

/*
    MDNSResponder::serviceName
*/
const char* MDNSResponder::serviceName(const hMDNSHost p_hMDNSHost,
                                       const hMDNSService p_hMDNSService) const
{
    return (_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)
            ? ((stcService*)p_hMDNSService)->m_pcName
            : 0);
}

/*
    MDNSResponder::serviceName (LEGACY 2)
*/
const char* MDNSResponder::serviceName(const hMDNSService p_hMDNSService) const
{
    clsHost*    pHost = 0;
    return (_validateMDNSServiceHandle(p_hMDNSService, &pHost)
            ? serviceName((hMDNSHost)pHost, p_hMDNSService)
            : 0);
}

/*
    MDNSResponder::service
*/
const char* MDNSResponder::service(const hMDNSHost p_hMDNSHost,
                                   const hMDNSService p_hMDNSService) const
{
    return (_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)
            ? ((stcService*)p_hMDNSService)->m_pcService
            : 0);
}

/*
    MDNSResponder::service (LEGACY 2)
*/
const char* MDNSResponder::service(const hMDNSService p_hMDNSService) const
{
    clsHost*    pHost = 0;
    return (_validateMDNSServiceHandle(p_hMDNSService, &pHost)
            ? service((hMDNSHost)pHost, p_hMDNSService)
            : 0);
}

/*
    MDNSResponder::serviceProtocol
*/
const char* MDNSResponder::serviceProtocol(const hMDNSHost p_hMDNSHost,
                                           const hMDNSService p_hMDNSService) const
{
    return (_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)
            ? ((stcService*)p_hMDNSService)->m_pcProtocol
            : 0);
}

/*
    MDNSResponder::serviceProtocol (LEGACY)
*/
const char* MDNSResponder::serviceProtocol(const hMDNSService p_hMDNSService) const
{
    clsHost*    pHost = 0;
    return (_validateMDNSServiceHandle(p_hMDNSService, &pHost)
            ? serviceProtocol((hMDNSHost)pHost, p_hMDNSService)
            : 0);
}

/*
    MDNSResponder::serviceStatus
*/
bool MDNSResponder::serviceStatus(const hMDNSHost p_hMDNSHost,
                                  const hMDNSService p_hMDNSService) const
{
    return ((_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)) &&
            (enuProbingStatus::Done == ((stcService*)p_hMDNSService)->m_ProbeInformation.m_ProbingStatus));
}

/*
    MDNSResponder::serviceStatus (LEGACY 2)

    Returns 'true' if probing for the service 'hMDNSService' is done

*/
bool MDNSResponder::serviceStatus(const hMDNSService p_hMDNSService) const
{
    clsHost*    pHost = 0;
    return (_validateMDNSServiceHandle(p_hMDNSService, &pHost)
            ? serviceStatus((hMDNSHost)pHost, p_hMDNSService)
            : false);
}


/*
    SERVICE TXT
*/

/*
    MDNSResponder::addServiceTxt

    Add a static service TXT item ('Key'='Value') to a service.

*/
MDNSResponder::hMDNSTxt MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                     const MDNSResponder::hMDNSService p_hMDNSService,
                                                     const char* p_pcKey,
                                                     const char* p_pcValue)
{
    hMDNSTxt    hTxt = (_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)
                        ? (hMDNSTxt)_addServiceTxt((clsHost*)p_hMDNSHost, (stcService*)p_hMDNSService, p_pcKey, p_pcValue, false)
                        : 0);
    DEBUG_EX_ERR(if (!hTxt) DEBUG_OUTPUT.printf_P(PSTR("%s addServiceTxt: FAILED for '%s=%s'!\n"), _DH(), (p_pcKey ? : "-"), (p_pcValue ? : "-")););
    return hTxt;
}

/*
    MDNSResponder::addServiceTxt (LEGACY 2)

    Add a static service TXT item ('Key'='Value') to a service.

*/
MDNSResponder::hMDNSTxt MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSService p_hMDNSService,
                                                     const char* p_pcKey,
                                                     const char* p_pcValue)
{
    clsHost*    pHost = 0;
    return (_validateMDNSHostHandle(p_hMDNSService, &pHost)
            ? addServiceTxt((hMDNSHost)pHost, p_hMDNSService, p_pcKey, p_pcValue)
            : 0);
}

/*
    MDNSRESPONDER_xxx_TO_CHAR
    Formats: http://www.cplusplus.com/reference/cstdio/printf/
*/
#define MDNSRESPONDER_U32_TO_CHAR(BUFFERNAME, U32VALUE) \
    char    BUFFERNAME[16]; /* 32-bit max 10 digits */  \
    *BUFFERNAME = 0;                                    \
    sprintf(BUFFERNAME, "%u", U32VALUE);
#define MDNSRESPONDER_U16_TO_CHAR(BUFFERNAME, U16VALUE) \
    char    BUFFERNAME[8];  /* 16-bit max 5 digits */   \
    *BUFFERNAME = 0;                                    \
    sprintf(BUFFERNAME, "%hu", U16VALUE);
#define MDNSRESPONDER_U8_TO_CHAR(BUFFERNAME, U8VALUE)   \
    char    BUFFERNAME[8];  /* 8-bit max 3 digits */    \
    *BUFFERNAME = 0;                                    \
    sprintf(BUFFERNAME, "%hhu", U8VALUE);
#define MDNSRESPONDER_I32_TO_CHAR(BUFFERNAME, I32VALUE) \
    char    BUFFERNAME[16]; /* 32-bit max 10 digits */  \
    *BUFFERNAME = 0;                                    \
    sprintf(BUFFERNAME, "%i", I32VALUE);
#define MDNSRESPONDER_I16_TO_CHAR(BUFFERNAME, I16VALUE) \
    char    BUFFERNAME[8];  /* 16-bit max 5 digits */   \
    *BUFFERNAME = 0;                                    \
    sprintf(BUFFERNAME, "%hi", I16VALUE);
#define MDNSRESPONDER_I8_TO_CHAR(BUFFERNAME, I8VALUE)   \
    char    BUFFERNAME[8];  /* 8-bit max 3 digits */    \
    *BUFFERNAME = 0;                                    \
    sprintf(BUFFERNAME, "%hhi", I8VALUE);


/*
    MDNSResponder::addServiceTxt (uint32_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                     const MDNSResponder::hMDNSService p_hMDNSService,
                                                     const char* p_pcKey,
                                                     uint32_t p_u32Value)
{
    MDNSRESPONDER_U32_TO_CHAR(acBuffer, p_u32Value);
    return addServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer):
    }

    /*
        MDNSResponder::addServiceTxt (uint32_t) (LEGACY 2)
    */
    MDNSResponder::hMDNSTxt MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSService p_hMDNSService,
                                                         const char* p_pcKey,
                                                         uint32_t p_u32Value)
{
    clsHost*    pHost = 0;
    return (_validateMDNSHostHandle(p_hMDNSService, &pHost)
            ? addServiceTxt((hMDNSHost)pHost, p_hMDNSService, p_pcKey, p_u32Value)
            : 0);
}

/*
    MDNSResponder::addServiceTxt (uint16_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                     const MDNSResponder::hMDNSService p_hMDNSService,
                                                     const char* p_pcKey,
                                                     uint16_t p_u16Value)
{
    MDNSRESPONDER_U16_TO_CHAR(acBuffer, p_u16Value);
    return addServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer):
    }

    /*
        MDNSResponder::addServiceTxt (uint16_t) (LEGACY 2)
    */
    MDNSResponder::hMDNSTxt MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSService p_hMDNSService,
                                                         const char* p_pcKey,
                                                         uint16_t p_u16Value)
{
    clsHost*    pHost = 0;
    return (_validateMDNSHostHandle(p_hMDNSService, &pHost)
            ? addServiceTxt((hMDNSHost)pHost, p_hMDNSService, p_pcKey, p_u16Value)
            : 0);
}

/*
    MDNSResponder::addServiceTxt (uint8_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                     const MDNSResponder::hMDNSService p_hMDNSService,
                                                     const char* p_pcKey,
                                                     uint8_t p_u8Value)
{
    MDNSRESPONDER_U8_TO_CHAR(acBuffer, p_u8Value);
    return addServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer):
    }

    /*
        MDNSResponder::addServiceTxt (uint8_t) (LEGACY 2)
    */
    MDNSResponder::hMDNSTxt MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSService p_hMDNSService,
                                                         const char* p_pcKey,
                                                         uint8_t p_u8Value)
{
    clsHost*    pHost = 0;
    return (_validateMDNSHostHandle(p_hMDNSService, &pHost)
            ? addServiceTxt((hMDNSHost)pHost, p_hMDNSService, p_pcKey, p_u8Value)
            : 0);
}

/*
    MDNSResponder::addServiceTxt (int32_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                     const MDNSResponder::hMDNSService p_hMDNSService,
                                                     const char* p_pcKey,
                                                     int32_t p_i32Value)
{
    MDNSRESPONDER_I32_TO_CHAR(acBuffer, p_i32Value);
    return addServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer):
    }

    /*
        MDNSResponder::addServiceTxt (int32_t) (LEGACY 2)
    */
    MDNSResponder::hMDNSTxt MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSService p_hMDNSService,
                                                         const char* p_pcKey,
                                                         int32_t p_i32Value)
{
    clsHost*    pHost = 0;
    return (_validateMDNSHostHandle(p_hMDNSService, &pHost)
            ? addServiceTxt((hMDNSHost)pHost, p_hMDNSService, p_pcKey, p_i32Value)
            : 0);
}

/*
    MDNSResponder::addServiceTxt (int16_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                     const MDNSResponder::hMDNSService p_hMDNSService,
                                                     const char* p_pcKey,
                                                     int16_t p_i16Value)
{
    MDNSRESPONDER_I16_TO_CHAR(acBuffer, p_i16Value);
    return addServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer):
    }

    /*
        MDNSResponder::addServiceTxt (int16_t) (LEGACY 2)
    */
    MDNSResponder::hMDNSTxt MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSService p_hMDNSService,
                                                         const char* p_pcKey,
                                                         int16_t p_i16Value)
{
    clsHost*    pHost = 0;
    return (_validateMDNSHostHandle(p_hMDNSService, &pHost)
            ? addServiceTxt((hMDNSHost)pHost, p_hMDNSService, p_pcKey, p_i16Value)
            : 0);
}

/*
    MDNSResponder::addServiceTxt (int8_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                     const MDNSResponder::hMDNSService p_hMDNSService,
                                                     const char* p_pcKey,
                                                     int8_t p_i8Value)
{
    MDNSRESPONDER_I8_TO_CHAR(acBuffer, p_i8Value);
    return addServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer):
    }

    /*
        MDNSResponder::addServiceTxt (int8_t) (LEGACY 2)
    */
    MDNSResponder::hMDNSTxt MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSService p_hMDNSService,
                                                         const char* p_pcKey,
                                                         int8_t p_i8Value)
{
    clsHost*    pHost = 0;
    return (_validateMDNSHostHandle(p_hMDNSService, &pHost)
            ? addServiceTxt((hMDNSHost)pHost, p_hMDNSService, p_pcKey, p_i8Value)
            : 0);
}

/*
    MDNSResponder::removeServiceTxt

    Remove a static service TXT item from a service.
*/
bool MDNSResponder::removeServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                     const MDNSResponder::hMDNSService p_hMDNSService,
                                     const MDNSResponder::hMDNSTxt p_hTxt)
{
    bool    bResult = ((_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)) &&
                       (p_hTxt) &&
                       (_findServiceTxt(*(clsHost*)p_hMDNSHost, *(stcService*)p_hMDNSService, p_hTxt)) &&
                       (_releaseServiceTxt(*(clsHost*)p_hMDNSHost, *(stcService*)p_hMDNSService, (stcServiceTxt*)p_hTxt)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s removeServiceTxt: FAILED!\n"), _DH((clsHost*)p_hMDNSHost)););
    return bResult;
}

/*
    MDNSResponder::removeServiceTxt (LEGACY 2)
*/
bool MDNSResponder::removeServiceTxt(const MDNSResponder::hMDNSService p_hMDNSService,
                                     const MDNSResponder::hMDNSTxt p_hTxt)
{
    clsHost*    pHost = 0;
    return (_validateMDNSHostHandle(p_hMDNSService, &pHost)
            ? removeServiceTxt((hMDNSHost)pHost, p_hMDNSService, p_hTxt)
            : false);
}

/*
    MDNSResponder::removeServiceTxt
*/
bool MDNSResponder::removeServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                     const MDNSResponder::hMDNSService p_hMDNSService,
                                     const char* p_pcKey)
{
    stcServiceTxt*  pTxt;
    bool    bResult = ((_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)) &&
                       (p_pcKey) &&
                       ((pTxt = _findServiceTxt(*(clsHost*)p_hMDNSHost, *(stcService*)p_hMDNSService, p_pcKey))) &&
                       (_releaseServiceTxt(*(clsHost*)p_hMDNSHost, *(stcService*)p_hMDNSService, pTxt)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s removeServiceTxt: FAILED!\n"), _DH((clsHost*)p_hMDNSHost)););
    return bResult;
}

/*
    MDNSResponder::removeServiceTxt (LEGACY 2)
*/
bool MDNSResponder::removeServiceTxt(const MDNSResponder::hMDNSService p_hMDNSService,
                                     const char* p_pcKey)
{
    clsHost*    pHost = 0;
    return (_validateMDNSHostHandle(p_hMDNSService, &pHost)
            ? removeServiceTxt((hMDNSHost)pHost, p_hMDNSService, p_pcKey)
            : false);
}

/*
    MDNSResponder::removeServiceTxt (LEGACY)
*/
bool MDNSResponder::removeServiceTxt(const char* p_pcName,
                                     const char* p_pcService,
                                     const char* p_pcProtocol,
                                     const char* p_pcKey)
{
    hMDNSService    hService;
    return (((hService = findService(p_pcName, p_pcService, p_pcProtocol)))
            ? removeServiceTxt(hService, p_pcKey)
            : false);
}

/*
    MDNSResponder::addServiceTxt (LEGACY)
*/
bool MDNSResponder::addServiceTxt(const char* p_pcService,
                                  const char* p_pcProtocol,
                                  const char* p_pcKey,
                                  const char* p_pcValue)
{
    hMDNSService    hService;
    return (((hService = findService(p_pcName, p_pcService, p_pcProtocol)))
            ? addServiceTxt(hService, p_pcKey, p_pcValue)
            : false);
}

/*
    MDNSResponder::addServiceTxt (LEGACY)
*/
bool MDNSResponder::addServiceTxt(String p_strService,
                                  String p_strProtocol,
                                  String p_strKey,
                                  String p_strValue)
{
    return addServiceTxt(p_strService.c_str(), p_strProtocol.c_str(), p_strKey.c_str(), p_strValue.c_str());
}

/*
    MDNSResponder::setDynamicServiceTxtCallback (binding)

    Set a netif binding specific callback for dynamic service TXT items. The callback is called, whenever
    service TXT items are needed for any service on the netif binding.

*/
bool MDNSResponder::setDynamicServiceTxtCallback(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                 MDNSResponder::MDNSDynamicServiceTxtCallbackFn p_fnCallback)
{
    bool    bResult = ((_validateMDNSHostHandle(p_hMDNSHost)) &&
                       ((!p_fnCallback) ||
                        ((((clsHost*)p_hMDNSHost)->m_fnServiceTxtCallback = p_fnCallback))));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s setDynamicServiceTxtCallback: FAILED!\n"), _DH((clsHost*)p_hMDNSHost)););
    return bResult;
}

/*
    MDNSResponder::setDynamicServiceTxtCallback (service)

    Set a service specific callback for dynamic service TXT items. The callback is called, whenever
    service TXT items are needed for the given service.
*/
bool MDNSResponder::setDynamicServiceTxtCallback(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                 const MDNSResponder::hMDNSService p_hMDNSService,
                                                 MDNSResponder::MDNSDynamicServiceTxtCallbackFn p_fnCallback)
{
    bool    bResult = ((_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)) &&
                       ((!p_fnCallback) ||
                        ((((stcService*)p_hMDNSService)->m_fnServiceTxtCallback = p_fnCallback))));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s setDynamicServiceTxtCallback: FAILED!\n"), _DH((clsHost*)p_hMDNSHost)););
    return bResult;
}

/*
    MDNSResponder::setDynamicServiceTxtCallback (global) (LEGACY 2)

    Set a global callback for dynamic service TXT items. The callback is called, whenever
    service TXT items are needed.

*/
bool MDNSResponder::setDynamicServiceTxtCallback(MDNSResponder::MDNSDynamicServiceTxtCallbackFn1 p_fnCallback)
{
    for (clsHostList : .iterator it : m_HostList)
    {
        setDynamicServiceTxtCallback((hMDNSHost)it, p_fnCallback);
    }
    return true;
}

/*
    MDNSResponder::setDynamicServiceTxtCallback (global) (LEGACY 2 (Fn2))
*/
bool MDNSResponder::setDynamicServiceTxtCallback(MDNSResponder::MDNSDynamicServiceTxtCallbackFn2 p_fnCallback)
{
    return setDynamicServiceTxtCallback([p_fnCallback](MDNSResponder*, const hMDNSService p_hMDNSService)
    {
        if (p_fnCallback)
        {
            p_fnCallback(p_hMDNSService);
        }
    });
}

/*
    MDNSResponder::setDynamicServiceTxtCallback (service) (LEGACY 2)
*/
bool MDNSResponder::setDynamicServiceTxtCallback(MDNSResponder::hMDNSService p_hMDNSService,
                                                 MDNSResponder::MDNSDynamicServiceTxtCallbackFn1 p_fnCallback)
{
    clsHost*    pHost;
    return ((_validateMDNSHostHandle(p_hMDNSService, &pHost)) &&
            (setDynamicServiceTxtCallback((hMDNSHost)pHost, hMDNSService, p_fnCallback)));
}

/*
    MDNSResponder::setDynamicServiceTxtCallback (service) (LEGACY 2 (Fn2))
*/
bool MDNSResponder::setDynamicServiceTxtCallback(MDNSResponder::hMDNSService p_hMDNSService,
                                                 MDNSResponder::MDNSDynamicServiceTxtCallbackFn2 p_fnCallback)
{
    return setDynamicServiceTxtCallback(p_hMDNSService, [p_fnCallback](MDNSResponder*, const hMDNSService p_hMDNSService)
    {
        if (p_fnCallback)
        {
            p_fnCallback(p_hMDNSService);
        }
    });
}

/*
    MDNSResponder::addDynamicServiceTxt
*/
MDNSResponder::hMDNSTxt MDNSResponder::addDynamicServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                            const MDNSResponder::hMDNSService p_hMDNSService,
                                                            const char* p_pcKey,
                                                            const char* p_pcValue)
{
    hMDNSTxt    hTxt = (_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)
                        ? _addServiceTxt(*(clsHost*)p_hMDNSHost, *(stcService*)p_hMDNSService, p_pcKey, p_pcValue, true)
                        : 0);
    DEBUG_EX_ERR(if (!hTxt) DEBUG_OUTPUT.printf_P(PSTR("%s addDynamicServiceTxt: FAILED for '%s=%s'!\n"), _DH(), (p_pcKey ? : "-"), (p_pcValue ? : "-")););
    return hTxt;
}

/*
    MDNSResponder::addDynamicServiceTxt (LEGACY 2)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addDynamicServiceTxt(const MDNSResponder::hMDNSService p_hMDNSService,
                                                            const char* p_pcKey,
                                                            const char* p_pcValue)
{
    clsHost*    pHost = 0;
    return (_validateMDNSHostHandle(p_hMDNSService, &pHost)
            ? addDynamicServiceTxt((hMDNSHost)pHost, p_hMDNSService, p_pcKey, p_pcValue)
            : 0);
}

/*
    MDNSResponder::addDynamicServiceTxt (uint32_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addDynamicServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                            MDNSResponder::hMDNSService p_hMDNSService,
                                                            const char* p_pcKey,
                                                            uint32_t p_u32Value)
{
    MDNSRESPONDER_U32_TO_CHAR(acBuffer, p_u32Value);
    return addDynamicServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer):
    }

    /*
        MDNSResponder::addDynamicServiceTxt (uint32_t) (LEGACY 2)
    */
    MDNSResponder::hMDNSTxt MDNSResponder::addDynamicServiceTxt(const MDNSResponder::hMDNSService p_hMDNSService,
                                                                const char* p_pcKey,
                                                                uint32_t p_u32Value)
{
    clsHost*    pHost = 0;
    return (_validateMDNSHostHandle(p_hMDNSService, &pHost)
            ? addDynamicServiceTxt((hMDNSHost)pHost, p_hMDNSService, p_pcKey, p_u32Value)
            : 0);
}

/*
    MDNSResponder::addDynamicServiceTxt (uint16_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addDynamicServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                            const MDNSResponder::hMDNSService p_hMDNSService,
                                                            const char* p_pcKey,
                                                            uint16_t p_u16Value)
{
    MDNSRESPONDER_U16_TO_CHAR(acBuffer, p_u16Value);
    return addDynamicServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer):
    }

    /*
        MDNSResponder::addDynamicServiceTxt (uint16_t) (LEGACY 2)
    */
    MDNSResponder::hMDNSTxt MDNSResponder::addDynamicServiceTxt(const MDNSResponder::hMDNSService p_hMDNSService,
                                                                const char* p_pcKey,
                                                                uint16_t p_u16Value)
{
    clsHost*    pHost = 0;
    return (_validateMDNSHostHandle(p_hMDNSService, &pHost)
            ? addDynamicServiceTxt((hMDNSHost)pHost, p_hMDNSService, p_pcKey, p_u16Value)
            : 0);
}

/*
    MDNSResponder::addDynamicServiceTxt (uint8_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addDynamicServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                            const MDNSResponder::hMDNSService p_hMDNSService,
                                                            const char* p_pcKey,
                                                            uint8_t p_u8Value)
{
    MDNSRESPONDER_U8_TO_CHAR(acBuffer, p_u8Value);
    return addDynamicServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer):
    }

    /*
        MDNSResponder::addDynamicServiceTxt (uint8_t) (LEGACY 2)
    */
    MDNSResponder::hMDNSTxt MDNSResponder::addDynamicServiceTxt(const MDNSResponder::hMDNSService p_hMDNSService,
                                                                const char* p_pcKey,
                                                                uint8_t p_u8Value)
{
    clsHost*    pHost = 0;
    return (_validateMDNSHostHandle(p_hMDNSService, &pHost)
            ? addDynamicServiceTxt((hMDNSHost)pHost, p_hMDNSService, p_pcKey, p_u8Value)
            : 0);
}

/*
    MDNSResponder::addDynamicServiceTxt (int32_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addDynamicServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                            const MDNSResponder::hMDNSService p_hMDNSService,
                                                            const char* p_pcKey,
                                                            int32_t p_i32Value)
{
    MDNSRESPONDER_I32_TO_CHAR(acBuffer, p_i32Value);
    return addDynamicServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer):
    }

    /*
        MDNSResponder::addDynamicServiceTxt (int32_t) (LEGACY 2)
    */
    MDNSResponder::hMDNSTxt MDNSResponder::addDynamicServiceTxt(const MDNSResponder::hMDNSService p_hMDNSService,
                                                                const char* p_pcKey,
                                                                uint32_t p_i32Value)
{
    clsHost*    pHost = 0;
    return (_validateMDNSHostHandle(p_hMDNSService, &pHost)
            ? addDynamicServiceTxt((hMDNSHost)pHost, p_hMDNSService, p_pcKey, p_i32Value)
            : 0);
}

/*
    MDNSResponder::addDynamicServiceTxt (int16_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addDynamicServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                            const MDNSResponder::hMDNSService p_hMDNSService,
                                                            const char* p_pcKey,
                                                            int16_t p_i16Value)
{
    MDNSRESPONDER_I16_TO_CHAR(acBuffer, p_i16Value);
    return addDynamicServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer):
    }

    /*
        MDNSResponder::addDynamicServiceTxt (int16_t) (LEGACY 2)
    */
    MDNSResponder::hMDNSTxt MDNSResponder::addDynamicServiceTxt(const MDNSResponder::hMDNSService p_hMDNSService,
                                                                const char* p_pcKey,
                                                                uint16_t p_i16Value)
{
    clsHost*    pHost = 0;
    return (_validateMDNSHostHandle(p_hMDNSService, &pHost)
            ? addDynamicServiceTxt((hMDNSHost)pHost, p_hMDNSService, p_pcKey, p_i16Value)
            : 0);
}

/*
    MDNSResponder::addDynamicServiceTxt (int8_t)
*/
MDNSResponder::hMDNSTxt MDNSResponder::addDynamicServiceTxt(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                            const MDNSResponder::hMDNSService p_hMDNSService,
                                                            const char* p_pcKey,
                                                            int8_t p_i8Value)
{
    MDNSRESPONDER_I8_TO_CHAR(acBuffer, p_i8Value);
    return addDynamicServiceTxt(p_hMDNSHost, p_hMDNSService, p_pcKey, acBuffer):
    }

    /*
        MDNSResponder::addDynamicServiceTxt (int8_t) (LEGACY 2)
    */
    MDNSResponder::hMDNSTxt MDNSResponder::addDynamicServiceTxt(const MDNSResponder::hMDNSService p_hMDNSService,
                                                                const char* p_pcKey,
                                                                uint8_t p_i8Value)
{
    clsHost*    pHost = 0;
    return (_validateMDNSHostHandle(p_hMDNSService, &pHost)
            ? addDynamicServiceTxt((hMDNSHost)pHost, p_hMDNSService, p_pcKey, p_i8Value)
            : 0);
}


/**
    STATIC QUERIES
*/

/*
    MDNSResponder::queryService

    Perform a (blocking) static service query.
    The arrived answers can be queried by calling:
    - answerHostName (or 'hostname')
    - answerIP (or 'IP')
    - answerPort (or 'port')

*/
uint32_t MDNSResponder::queryService(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                     const char* p_pcService,
                                     const char* p_pcProtocol,
                                     const uint16_t p_u16Timeout /*= MDNS_QUERYSERVICES_WAIT_TIME*/)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s queryService '%s.%s'\n"), _DH(), p_pcService, p_pcProtocol););

    uint32_t    u32Result = 0;

    stcQuery*    pMDNSQuery = 0;
    if ((_validateMDNSHostHandle(p_hMDNSHost)) &&
        (p_pcService) &&
        (os_strlen(p_pcService)) &&
        (p_pcProtocol) &&
        (os_strlen(p_pcProtocol)) &&
        (p_u16Timeout) &&
        (_removeLegacyQuery()) &&
        ((pMDNSQuery = _allocQuery(*(clsHost*)p_hMDNSHost, stcQuery::enuQueryType::Service))) &&
        (_buildDomainForService(p_pcService, p_pcProtocol, pMDNSQuery->m_Domain)))
    {
        pMDNSQuery->m_bLegacyQuery = true;

        if (_sendMDNSQuery(*(clsHost*)p_hMDNSHost, *pMDNSQuery))
        {
            // Wait for answers to arrive
            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s queryService: Waiting %u ms for answers...\n"), _DH(), p_u16Timeout););
            delay(p_u16Timeout);

            // All answers should have arrived by now -> stop adding new answers
            pMDNSQuery->m_bAwaitingAnswers = false;
            u32Result = pMDNSQuery->answerCount();
        }
        else    // FAILED to send query
        {
            _removeQuery(*(clsHost*)p_hMDNSHost, pMDNSQuery);
        }
    }
    else
    {
        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s queryService: INVALID input data!\n"), _DH()););
    }
    return u32Result;
}

/*
    MDNSResponder::queryService (LEGACY 2)
*/
uint32_t MDNSResponder::queryService(const char* p_pcService,
                                     const char* p_pcProtocol,
                                     const uint16_t p_u16Timeout /*= MDNS_QUERYSERVICES_WAIT_TIME*/)
{
    return ((!m_HostList.empty())
            ? queryService((hMDNSHost)m_HostList.front(), p_pcService, p_pcProtocol, p_u16Timeout)
            : 0);
}

/*
    MDNSResponder::queryService (LEGACY)
*/
uint32_t MDNSResponder::queryService(const String& p_strService,
                                     const String& p_strProtocol)
{
    return queryService(p_strService.c_str(), p_strProtocol.c_str());
}

/*
    MDNSResponder::queryHost

    Perform a (blocking) static host query.
    The arrived answers can be queried by calling:
    - answerHostName (or 'hostname')
    - answerIP (or 'IP')
    - answerPort (or 'port')

*/
uint32_t MDNSResponder::queryHost(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                  const char* p_pcHostName,
                                  const uint16_t p_u16Timeout /*= MDNS_QUERYSERVICES_WAIT_TIME*/)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s queryHost '%s.local'\n"), _DH(), p_pcHostName););

    uint32_t    u32Result = 0;

    stcQuery*    pHostQuery = 0;
    if ((_validateMDNSHostHandle(p_hMDNSHost)) &&
        (p_pcHostName) &&
        (os_strlen(p_pcHostName)) &&
        (p_u16Timeout) &&
        (_removeLegacyQuery()) &&
        ((pHostQuery = _allocQuery(*(clsHost*)p_hMDNSHost, stcQuery::enuQueryType::Host))) &&
        (_buildDomainForHost(p_pcHostName, pHostQuery->m_Domain)))
    {

        pHostQuery->m_bLegacyQuery = true;

        if (_sendMDNSQuery(*(clsHost*)p_hMDNSHost, *pHostQuery))
        {
            // Wait for answers to arrive
            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s queryHost: Waiting %u ms for answers...\n"), _DH(), p_u16Timeout););
            delay(p_u16Timeout);

            // All answers should have arrived by now -> stop adding new answers
            pHostQuery->m_bAwaitingAnswers = false;
            u32Result = pHostQuery->answerCount();
        }
        else    // FAILED to send query
        {
            _removeQuery(*(clsHost*)p_hMDNSHost, pHostQuery);
        }
    }
    else
    {
        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s queryHost: INVALID input data!\n"), _DH()););
    }
    return u32Result;
}

/*
    queryHost (LEGACY 2)
*/
uint32_t MDNSResponder::queryHost(const char* p_pcHostName,
                                  const uint16_t p_u16Timeout /*= MDNS_QUERYSERVICES_WAIT_TIME*/)
{
    return ((!m_HostList.empty())
            ? queryHost((hMDNSHost)m_HostList.front(), p_pcService, p_pcProtocol, p_u16Timeout)
            : 0);
}

/*
    MDNSResponder::removeQuery

    Remove the last static query (and all answers).

*/
bool MDNSResponder::removeQuery(const MDNSResponder::hMDNSHost p_hMDNSHost)
{
    return ((_validateMDNSHostHandle(p_hMDNSHost)) &&
            (_removeLegacyQuery(*(clsHost*)p_hMDNSHost)));
}

/*
    MDNSResponder::removeQuery (LEGACY 2)
*/
bool MDNSResponder::removeQuery(void)
{
    return ((!m_HostList.empty())
            ? removeQuery((hMDNSHost)m_HostList.front())
            : false);
}

/*
    MDNSResponder::hasQuery

    Return 'true', if a static query is currently installed

*/
bool MDNSResponder::hasQuery(const MDNSResponder::hMDNSHost p_hMDNSHost)
{
    return ((_validateMDNSHostHandle(p_hMDNSHost)) &&
            (0 != _findLegacyQuery(*(clsHost*)p_hMDNSHost)));
}

/*
    MDNSResponder::hasQuery (LEGACY 2)
*/
bool MDNSResponder::hasQuery(void)
{
    return ((!m_HostList.empty())
            ? hasQuery((hMDNSHost)m_HostList.front())
            : false);
}

/*
    MDNSResponder::getQuery

    Return handle to the last static query

*/
MDNSResponder::hMDNSQuery MDNSResponder::getQuery(const MDNSResponder::hMDNSHost p_hMDNSHost)
{
    return (_validateMDNSHostHandle(p_hMDNSHost)
            ? (hMDNSQuery)_findLegacyQuery()
            : 0);
}

/*
    MDNSResponder::getQuery (LEGACY 2)
*/
MDNSResponder::hMDNSQuery MDNSResponder::getQuery(void)
{
    return ((!m_HostList.empty())
            ? getQuery((hMDNSHost)m_HostList.front())
            : false);
}

/*
    MDNSResponder::answerHostName
*/
const char* MDNSResponder::answerHostName(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                          const uint32_t p_u32AnswerIndex)
{
    const stcQuery*            pQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findLegacyQuery(*(clsHost*)p_hMDNSHost) : 0);
    stcQuery::stcAnswer* pSQAnswer = (pQuery ? pQuery->answerAtIndex(p_u32AnswerIndex) : 0);

    if ((pSQAnswer) &&
        (pSQAnswer->m_HostDomain.m_u16NameLength) &&
        (!pSQAnswer->m_pcHostDomain))
    {
        char*   pcHostDomain = pSQAnswer->allocHostDomain(pSQAnswer->m_HostDomain.c_strLength());
        if (pcHostDomain)
        {
            pSQAnswer->m_HostDomain.c_str(pcHostDomain);
        }
    }
    return (pSQAnswer ? pSQAnswer->m_pcHostDomain : 0);
}

/*
    MDNSResponder::answerHostName (LEGACY 2)
*/
const char* MDNSResponder::answerHostName(const uint32_t p_u32AnswerIndex)
{
    return ((!m_HostList.empty())
            ? answerHostName((hMDNSHost)m_HostList.front(), p_u32AnswerIndex)
            : 0);
}

/*
    MDNSResponder::hostname (LEGACY)
*/
String MDNSResponder::hostname(const uint32_t p_u32AnswerIndex)
{
    return String(answerHostName(p_u32AnswerIndex));
}

#ifdef MDNS_IPV4_SUPPORT
/*
    MDNSResponder::answerIPv4
*/
IPAddress MDNSResponder::answerIPv4(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                    const uint32_t p_u32AnswerIndex)
{
    const stcQuery*                             pQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findLegacyQuery(*(clsHost*)p_hMDNSHost) : 0);
    const stcQuery::stcAnswer*                  pSQAnswer = (pQuery ? pQuery->answerAtIndex(p_u32AnswerIndex) : 0);
    const stcQuery::stcAnswer::stcIPv4Address*  pIPv4Address = (((pSQAnswer) && (pSQAnswer->m_pIPv4Addresses)) ? pSQAnswer->IPv4AddressAtIndex(0) : 0);
    return (pIPv4Address ? pIPv4Address->m_IPAddress : IPAddress());
}

/*
    MDNSResponder::answerIPv4 (LEGACY 2)
*/
IPAddress MDNSResponder::answerIPv4(const uint32_t p_u32AnswerIndex)
{
    return ((!m_HostList.empty())
            ? answerIPv4((hMDNSHost)m_HostList.front(), p_u32AnswerIndex)
            : IPAddress());
}

/*
    MDNSResponder::answerIP (LEGACY 2)
*/
IPAddress MDNSResponder::answerIP(const uint32_t p_u32AnswerIndex)
{
    return answerIPv4(p_u32AnswerIndex);
}

/*
    MDNSResponder::IP (LEGACY)
*/
IPAddress MDNSResponder::IP(const uint32_t p_u32AnswerIndex)
{
    return answerIPv4(p_u32AnswerIndex);
}
#endif

#ifdef MDNS_IPV6_SUPPORT
/*
    MDNSResponder::answerIPv6
*/
IPAddress MDNSResponder::answerIPv6(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                    const uint32_t p_u32AnswerIndex)
{
    const stcQuery*                             pQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findLegacyQuery(*(clsHost*)p_hMDNSHost) : 0);
    const stcQuery::stcAnswer*                  pSQAnswer = (pQuery ? pQuery->answerAtIndex(p_u32AnswerIndex) : 0);
    const stcQuery::stcAnswer::stcIPv6Address*  pIPv6Address = (((pSQAnswer) && (pSQAnswer->m_pIPv6Addresses)) ? pSQAnswer->IPv6AddressAtIndex(0) : 0);
    return (pIPv6Address ? pIPv6Address->m_IPAddress : IPAddress());
}

/*
    MDNSResponder::answerIPv6 (LEGACY 2)
*/
IPAddress MDNSResponder::answerIPv6(const uint32_t p_u32AnswerIndex)
{
    return ((!m_HostList.empty())
            ? answerIPv6((hMDNSHost)m_HostList.front(), p_u32AnswerIndex)
            : IPAddress());
}
#endif

/*
    MDNSResponder::answerPort
*/
uint16_t MDNSResponder::answerPort(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                   const uint32_t p_u32AnswerIndex)
{
    const stcQuery*             pQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findLegacyQuery(*(clsHost*)p_hMDNSHost) : 0);
    const stcQuery::stcAnswer*  pSQAnswer = (pQuery ? pQuery->answerAtIndex(p_u32AnswerIndex) : 0);
    return (pSQAnswer ? pSQAnswer->m_u16Port : 0);
}

/*
    MDNSResponder::answerPort (LEGACY 2)
*/
uint16_t MDNSResponder::answerPort(const uint32_t p_u32AnswerIndex)
{
    return ((!m_HostList.empty())
            ? answerPort((hMDNSHost)m_HostList.front(), p_u32AnswerIndex)
            : 0);
}

/*
    MDNSResponder::port (LEGACY)
*/
uint16_t MDNSResponder::port(const uint32_t p_u32AnswerIndex)
{
    return answerPort(p_u32AnswerIndex);
}


/**
    DYNAMIC SERVICE QUERY
*/

/*
    MDNSResponder::installServiceQuery

    Add a dynamic service query and a corresponding callback to the MDNS responder.
    The callback will be called for every answer update.
    The answers can also be queried by calling:
    - answerServiceDomain
    - answerHostDomain
    - answerIPv4Address/answerIPv6Address
    - answerPort
    - answerTxts

*/
MDNSResponder::hMDNSQuery MDNSResponder::installServiceQuery(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                             const char* p_pcService,
                                                             const char* p_pcProtocol,
                                                             MDNSResponder::MDNSQueryCallbackFn p_fnCallback)
{
    hMDNSQuery      hResult = 0;

    stcQuery*   pMDNSQuery = 0;
    if ((_validateMDNSHostHandle(p_hMDNSHost)) &&
        (p_pcService) &&
        (os_strlen(p_pcService)) &&
        (p_pcProtocol) &&
        (os_strlen(p_pcProtocol)) &&
        (p_fnCallback) &&
        ((pMDNSQuery = _allocQuery(*(clsHost*)p_hMDNSHost, stcQuery::enuQueryType::Service))) &&
        (_buildDomainForService(p_pcService, p_pcProtocol, pMDNSQuery->m_Domain)))
    {

        pMDNSQuery->m_fnCallback = p_fnCallback;
        pMDNSQuery->m_bLegacyQuery = false;

        if (_sendMDNSQuery(*(clsHost*)p_hMDNSHost, *pMDNSQuery))
        {
            pMDNSQuery->m_u8SentCount = 1;
            pMDNSQuery->m_ResendTimeout.reset(MDNS_DYNAMIC_QUERY_RESEND_DELAY);

            hResult = (hMDNSQuery)pMDNSQuery;
        }
        else
        {
            _removeQuery(*(clsHost*)p_hMDNSHost, pMDNSQuery);
        }
    }
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s installServiceQuery: %s for '%s.%s'!\n\n"), _DH(), (hResult ? "Succeeded" : "FAILED"), (p_pcService ? : "-"), (p_pcProtocol ? : "-")););
    DEBUG_EX_ERR(if (!hResult) DEBUG_OUTPUT.printf_P(PSTR("%s installServiceQuery: FAILED for '%s.%s'!\n\n"), _DH(), (p_pcService ? : "-"), (p_pcProtocol ? : "-")););
    return hResult;
}

/*
    MDNSResponder::installServiceQuery (LEGACY 2)
*/
MDNSResponder::hMDNSQuery MDNSResponder::installServiceQuery(const char* p_pcService,
                                                             const char* p_pcProtocol,
                                                             MDNSResponder::MDNSQueryCallbackFn1 p_fnCallback)
{
    return ((!m_HostList.empty())
            ? installServiceQuery((hMDNSHost)m_HostList.front(), p_pcService, p_pcProtocol, [p_fnCallback])(MDNSResponder * p_pMDNSResponder,
                                                                                                            MDNSResponder::hMDNSHost,
                                                                                                            const stcAnswerAccessor & p_MDNSAnswerAccessor,
                                                                                                            typeQueryAnswerType p_QueryAnswerTypeFlags,
                                                                                                            bool p_bSetContent)
    {
        if (p_fnCallback)
        {
            p_fnCallback(p_pMDNSResponder, p_MDNSAnswerAccessor, p_QueryAnswerTypeFlags, p_bSetContent);
        }
    })
        : 0);
    }

    /*
        MDNSResponder::installServiceQuery (LEGACY 2)
    */
    MDNSResponder::hMDNSQuery MDNSResponder::installServiceQuery(const char* p_pcService,
                                                                 const char* p_pcProtocol,
                                                                 MDNSResponder::MDNSQueryCallbackFn2 p_fnCallback)
{
    return ((!m_HostList.empty())
            ? installServiceQuery((hMDNSHost)m_HostList.front(), p_pcService, p_pcProtocol, [p_fnCallback])(MDNSResponder*,
                                                                                                            MDNSResponder::hMDNSHost,
                                                                                                            const stcAnswerAccessor & p_MDNSAnswerAccessor,
                                                                                                            typeQueryAnswerType p_QueryAnswerTypeFlags,
                                                                                                            bool p_bSetContent)
    {
        if (p_fnCallback)
        {
            p_fnCallback(p_MDNSAnswerAccessor, p_QueryAnswerTypeFlags, p_bSetContent);
        }
    })
        : 0);
    }

    /*
        MDNSResponder::installHostQuery
    */
    MDNSResponder::hMDNSQuery MDNSResponder::installHostQuery(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                              const char* p_pcHostName,
                                                              MDNSResponder::MDNSQueryCallbackFn p_fnCallback)
{
    hMDNSQuery       hResult = 0;

    if ((_validateMDNSHostHandle(p_hMDNSHost)) &&
        (p_pcHostName) &&
        (os_strlen(p_pcHostName)))
    {
        stcRRDomain    domain;
        hResult = ((_buildDomainForHost(p_pcHostName, domain))
                   ? _installDomainQuery(*(clsHost*)p_hMDNSHost, domain, stcQuery::enuQueryType::Host, p_fnCallback)
                   : 0);
    }
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s installHostQuery: %s for '%s.local'!\n\n"), _DH(), (hResult ? "Succeeded" : "FAILED"), (p_pcHostName ? : "-")););
    DEBUG_EX_ERR(if (!hResult) DEBUG_OUTPUT.printf_P(PSTR("%s installHostQuery: FAILED for '%s.local'!\n\n"), _DH(), (p_pcHostName ? : "-")););
    return hResult;
}

/*
    MDNSResponder::installHostQuery (LEGACY 2)
*/
MDNSResponder::hMDNSQuery MDNSResponder::installHostQuery(const char* p_pcHostName,
                                                          MDNSResponder::MDNSQueryCallbackFn1 p_fnCallback)
{
    return installHostQuery(p_pcHostName, [p_fnCallback](MDNSResponder * p_pMDNSResponder,
                                                         hMDNSHost,
                                                         const stcAnswerAccessor & p_MDNSAnswerAccessor,
                                                         typeQueryAnswerType p_QueryAnswerTypeFlags,
                                                         bool p_bSetContent)
    {
        if (p_fnCallback)
        {
            p_fnCallback(p_pMDNSResponder, p_MDNSAnswerAccessor, p_QueryAnswerTypeFlags, p_bSetContent);
        }
    });
}

/*
    MDNSResponder::installHostQuery (LEGACY 2)
*/
MDNSResponder::hMDNSQuery MDNSResponder::installHostQuery(const char* p_pcHostName,
                                                          MDNSResponder::MDNSQueryCallbackFn2 p_fnCallback)
{
    return installHostQuery(p_pcHostName, [p_fnCallback](MDNSResponder*,
                                                         hMDNSHost,
                                                         const stcAnswerAccessor & p_MDNSAnswerAccessor,
                                                         typeQueryAnswerType p_QueryAnswerTypeFlags,
                                                         bool p_bSetContent)
    {
        if (p_fnCallback)
        {
            p_fnCallback(p_MDNSAnswerAccessor, p_QueryAnswerTypeFlags, p_bSetContent);
        }
    });
}

/*
    MDNSResponder::removeQuery

    Remove a dynamic query (and all collected answers) from the MDNS responder

*/
bool MDNSResponder::removeQuery(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                const MDNSResponder::hMDNSQuery p_hMDNSQuery)
{
    stcQuery*    pMDNSQuery = 0;
    bool    bResult = ((_validateMDNSHostHandle(p_hMDNSHost)) &&
                       ((pMDNSQuery = _findQuery(*(clsHost*)p_hMDNSHost, p_hQuery))) &&
                       (_removeQuery(*(clsHost*)p_hMDNSHost, pMDNSQuery)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s removeQuery: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::removeQuery (LEGACY 2)
*/
bool MDNSResponder::removeQuery(const MDNSResponder::hMDNSQuery p_hMDNSQuery)
{
    return ((!m_HostList.empty())
            ? removeQuery((hMDNSHost)m_HostList.front(), p_hMDNSQuery)
            : false);
}

/*
    MDNSResponder::answerAccessors
*/
MDNSResponder::clsMDNSAnswerAccessorVector MDNSResponder::answerAccessors(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                                          const MDNSResponder::hMDNSQuery p_hMDNSQuery)
{
    MDNSResponder::clsMDNSAnswerAccessorVector  tempVector;
    for (uint32_t u = 0; u < answerCount(p_hMDNSHost, p_hMDNSQuery); ++u)
    {
        tempVector.emplace_back(*this, p_hMDNSQuery, u);
    }
    return tempVector;
}

/*
    MDNSResponder::answerAccessors (LEGACY 2)
*/
MDNSResponder::clsMDNSAnswerAccessorVector MDNSResponder::answerAccessors(const MDNSResponder::hMDNSQuery p_hMDNSQuery)
{
    return ((!m_HostList.empty())
            ? answerAccessors((hMDNSHost)m_HostList.front(), p_hMDNSQuery)
            : MDNSResponder::clsMDNSAnswerAccessorVector());
}

/*
    MDNSResponder::answerCount
*/
uint32_t MDNSResponder::answerCount(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                    const MDNSResponder::hMDNSQuery p_hMDNSQuery)
{
    stcQuery*    pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery) : 0);
    return (pMDNSQuery ? pMDNSQuery->answerCount() : 0);
}

/*
    MDNSResponder::answerCount (LEGACY 2)
*/
uint32_t MDNSResponder::answerCount(const MDNSResponder::hMDNSQuery p_hMDNSQuery)
{
    return ((!m_HostList.empty())
            ? answerCount((hMDNSHost)m_HostList.front(), p_hMDNSQuery)
            : 0);
}

/*
    MDNSResponder::hasAnswerServiceDomain
*/
bool MDNSResponder::hasAnswerServiceDomain(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                           const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                           const uint32_t p_u32AnswerIndex)
{
    stcQuery*               pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer*    pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          return ((pSQAnswer) &&
                                                  (pSQAnswer->m_QueryAnswerFlags & static_cast<typeQueryAnswerType>(enuQueryAnswerType::ServiceDomain)));
}

                                     /*
                                         MDNSResponder::hasAnswerServiceDomain (LEGACY 2)
                                     */
                                     bool MDNSResponder::hasAnswerServiceDomain(const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                                const uint32_t p_u32AnswerIndex)
{
    return ((!m_HostList.empty())
            ? hasAnswerServiceDomain((hMDNSHost)m_HostList.front(), p_hMDNSQuery, p_u32AnswerIndex)
            : false);
}

/*
    MDNSResponder::answerServiceDomain

    Returns the domain for the given service.
    If not already existing, the string is allocated, filled and attached to the answer.

*/
const char* MDNSResponder::answerServiceDomain(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                               const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                               const uint32_t p_u32AnswerIndex)
{
    stcQuery*               pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer*    pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          // Fill m_pcServiceDomain (if not already done)
                                          if ((pSQAnswer) &&
                                              (pSQAnswer->m_ServiceDomain.m_u16NameLength) &&
                                              (!pSQAnswer->m_pcServiceDomain))
{

    pSQAnswer->m_pcServiceDomain = pSQAnswer->allocServiceDomain(pSQAnswer->m_ServiceDomain.c_strLength());
        if (pSQAnswer->m_pcServiceDomain)
        {
            pSQAnswer->m_ServiceDomain.c_str(pSQAnswer->m_pcServiceDomain);
        }
    }
    return (pSQAnswer ? pSQAnswer->m_pcServiceDomain : 0);
}

/*
    MDNSResponder::answerServiceDomain (LEGACY 2)
*/
const char* MDNSResponder::answerServiceDomain(const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                               const uint32_t p_u32AnswerIndex)
{
    return ((!m_HostList.empty())
            ? answerServiceDomain((hMDNSHost)m_HostList.front(), p_hMDNSQuery, p_u32AnswerIndex)
            : 0);
}

/*
    MDNSResponder::hasAnswerHostDomain
*/
bool MDNSResponder::hasAnswerHostDomain(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                        const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                        const uint32_t p_u32AnswerIndex)
{
    stcQuery*               pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer*    pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          return ((pSQAnswer) &&
                                                  (pSQAnswer->m_QueryAnswerFlags & static_cast<typeQueryAnswerType>(enuQueryAnswerType::HostDomain)));
}

                                     /*
                                         MDNSResponder::hasAnswerHostDomain (LEGACY 2)
                                     */
                                     bool MDNSResponder::hasAnswerHostDomain(const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                             const uint32_t p_u32AnswerIndex)
{
    return ((!m_HostList.empty())
            ? hasAnswerHostDomain((hMDNSHost)m_HostList.front(), p_hMDNSQuery, p_u32AnswerIndex)
            : false);
}

/*
    MDNSResponder::answerHostDomain

    Returns the host domain for the given service.
    If not already existing, the string is allocated, filled and attached to the answer.

*/
const char* MDNSResponder::answerHostDomain(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                            const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                            const uint32_t p_u32AnswerIndex)
{
    stcQuery*               pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer*    pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          // Fill m_pcHostDomain (if not already done)
                                          if ((pSQAnswer) &&
                                              (pSQAnswer->m_HostDomain.m_u16NameLength) &&
                                              (!pSQAnswer->m_pcHostDomain))
{

    pSQAnswer->m_pcHostDomain = pSQAnswer->allocHostDomain(pSQAnswer->m_HostDomain.c_strLength());
        if (pSQAnswer->m_pcHostDomain)
        {
            pSQAnswer->m_HostDomain.c_str(pSQAnswer->m_pcHostDomain);
        }
    }
    return (pSQAnswer ? pSQAnswer->m_pcHostDomain : 0);
}

/*
    MDNSResponder::answerHostDomain (LEGACY 2)
*/
const char* MDNSResponder::answerHostDomain(const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                            const uint32_t p_u32AnswerIndex)
{
    return ((!m_HostList.empty())
            ? answerHostDomain((hMDNSHost)m_HostList.front(), p_hMDNSQuery, p_u32AnswerIndex)
            : 0);
}

#ifdef MDNS_IPV4_SUPPORT
/*
    MDNSResponder::hasAnswerIPv4Address
*/
bool MDNSResponder::hasAnswerIPv4Address(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                         const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                         const uint32_t p_u32AnswerIndex)
{
    stcQuery*               pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer*    pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          return ((pSQAnswer) &&
                                                  (pSQAnswer->m_QueryAnswerFlags & static_cast<typeQueryAnswerType>(enuQueryAnswerType::IPv4Address)));
}

                                     /*
                                         MDNSResponder::hasAnswerIPv4Address (LEGACY 2)
                                     */
                                     bool MDNSResponder::hasAnswerIPv4Address(const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                              const uint32_t p_u32AnswerIndex)
{
    return ((!m_HostList.empty())
            ? hasAnswerIPv4Address((hMDNSHost)m_HostList.front(), p_hMDNSQuery, p_u32AnswerIndex)
            : false);
}

/*
    MDNSResponder::answerIPv4AddressCount
*/
uint32_t MDNSResponder::answerIPv4AddressCount(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                               const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                               const uint32_t p_u32AnswerIndex)
{
    stcQuery*               pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer*    pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          return (pSQAnswer ? pSQAnswer->IPv4AddressCount() : 0);
}

                                     /*
                                         MDNSResponder::answerIPv4AddressCount (LEGACY 2)
                                     */
                                     uint32_t MDNSResponder::answerIPv4AddressCount(const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                                    const uint32_t p_u32AnswerIndex)
{
    return ((!m_HostList.empty())
            ? answerIPv4AddressCount((hMDNSHost)m_HostList.front(), p_hMDNSQuery, p_u32AnswerIndex)
            : 0);
}

/*
    MDNSResponder::answerIPv4Address
*/
IPAddress MDNSResponder::answerIPv4Address(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                           const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                           const uint32_t p_u32AnswerIndex,
                                           const uint32_t p_u32AddressIndex)
{
    stcQuery*                               pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                                          stcQuery::stcAnswer*                    pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                                          stcQuery::stcAnswer::stcIPv4Address*    pIPv4Address = (pSQAnswer ? pSQAnswer->IPv4AddressAtIndex(p_u32AddressIndex) : 0);
                                                          return (pIPv4Address ? pIPv4Address->m_IPAddress : IPAddress());
}

                                                     /*
                                                         MDNSResponder::answerIPv4Address (LEGACY 2)
                                                     */
                                                     IPAddress MDNSResponder::answerIPv4Address(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                                                                const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                                                const uint32_t p_u32AnswerIndex,
                                                                                                const uint32_t p_u32AddressIndex)
{
    return ((!m_HostList.empty())
            ? answerIPv4Address((hMDNSHost)m_HostList.front(), p_hMDNSQuery, p_u32AnswerIndex, p_u32AddressIndex)
            : IPAddress());
}
#endif

#ifdef MDNS_IPV6_SUPPORT
/*
    MDNSResponder::hasAnswerIPv6Address
*/
bool MDNSResponder::hasAnswerIPv6Address(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                         const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                         const uint32_t p_u32AnswerIndex)
{
    stcQuery*               pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer*    pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          return ((pSQAnswer) &&
                                                  (pSQAnswer->m_QueryAnswerFlags & static_cast<typeQueryAnswerType>(enuQueryAnswerType::IPv6Address)));
}

                                     /*
                                         MDNSResponder::hasAnswerIPv6Address (LEGACY 2)
                                     */
                                     bool MDNSResponder::hasAnswerIPv6Address(const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                              const uint32_t p_u32AnswerIndex)
{
    return ((!m_HostList.empty())
            ? hasAnswerIPv6Address((hMDNSHost)m_HostList.front(), p_hMDNSQuery, p_u32AnswerIndex)
            : false);
}

/*
    MDNSResponder::answerIPv6AddressCount
*/
uint32_t MDNSResponder::answerIPv6AddressCount(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                               const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                               const uint32_t p_u32AnswerIndex)
{
    stcQuery*               pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer*    pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          return (pSQAnswer ? pSQAnswer->IPv6AddressCount() : 0);
}

                                     /*
                                         MDNSResponder::answerIPv6AddressCount (LEGACY 2)
                                     */
                                     uint32_t MDNSResponder::answerIPv6AddressCount(const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                                    const uint32_t p_u32AnswerIndex)
{
    return ((!m_HostList.empty())
            ? answerIPv6AddressCount((hMDNSHost)m_HostList.front(), p_hMDNSQuery, p_u32AnswerIndex)
            : 0);
}

/*
    MDNSResponder::answerIPv6Address
*/
IPAddress MDNSResponder::answerIPv6Address(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                           const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                           const uint32_t p_u32AnswerIndex,
                                           const uint32_t p_u32AddressIndex)
{
    stcQuery*                               pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                                          stcQuery::stcAnswer*                    pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                                          stcQuery::stcAnswer::stcIPv6Address*    pIPv6Address = (pSQAnswer ? pSQAnswer->IPv6AddressAtIndex(p_u32AddressIndex) : 0);
                                                          return (pIPv6Address ? pIPv6Address->m_IPAddress : IPAddress());
}

                                                     /*
                                                         MDNSResponder::answerIPv6Address (LEGACY 2)
                                                     */
                                                     IPAddress MDNSResponder::answerIPv6Address(const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                                                const uint32_t p_u32AnswerIndex,
                                                                                                const uint32_t p_u32AddressIndex)
{
    return ((!m_HostList.empty())
            ? answerIPv6Address((hMDNSHost)m_HostList.front(), p_hMDNSQuery, p_u32AnswerIndex)
            : IPAddress());
}
#endif

/*
    MDNSResponder::hasAnswerPort
*/
bool MDNSResponder::hasAnswerPort(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                  const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                  const uint32_t p_u32AnswerIndex)
{
    stcQuery*               pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer*    pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          return ((pSQAnswer) &&
                                                  (pSQAnswer->m_QueryAnswerFlags & static_cast<typeQueryAnswerType>(enuQueryAnswerType::Port)));
}

                                     /*
                                         MDNSResponder::hasAnswerPort (LEGACY 2)
                                     */
                                     bool MDNSResponder::hasAnswerPort(const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                       const uint32_t p_u32AnswerIndex)
{
    return ((!m_HostList.empty())
            ? hasAnswerPort((hMDNSHost)m_HostList.front(), p_hMDNSQuery, p_u32AnswerIndex)
            : false);
}

/*
    MDNSResponder::answerPort
*/
uint16_t MDNSResponder::answerPort(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                   const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                   const uint32_t p_u32AnswerIndex)
{
    stcQuery*               pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer*    pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          return (pSQAnswer ? pSQAnswer->m_u16Port : 0);
}

                                     /*
                                         MDNSResponder::answerPort (LEGACY 2)
                                     */
                                     uint16_t MDNSResponder::answerPort(const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                        const uint32_t p_u32AnswerIndex)
{
    return ((!m_HostList.empty())
            ? answerPort((hMDNSHost)m_HostList.front(), p_hMDNSQuery, p_u32AnswerIndex)
            : 0);
}

/*
    MDNSResponder::hasAnswerTxts
*/
bool MDNSResponder::hasAnswerTxts(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                  const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                  const uint32_t p_u32AnswerIndex)
{
    stcQuery*               pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer*    pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          return ((pSQAnswer) &&
                                                  (pSQAnswer->m_QueryAnswerFlags & static_cast<typeQueryAnswerType>(enuQueryAnswerType::Txts)));
}

                                     /*
                                         MDNSResponder::hasAnswerTxts (LEGACY 2)
                                     */
                                     bool MDNSResponder::hasAnswerTxts(const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                                                       const uint32_t p_u32AnswerIndex)
{
    return ((!m_HostList.empty())
            ? hasAnswerTxts((hMDNSHost)m_HostList.front(), p_hMDNSQuery, p_u32AnswerIndex)
            : false);
}

/*
    MDNSResponder::answerTxts

    Returns all TXT items for the given service as a ';'-separated string.
    If not already existing; the string is alloced, filled and attached to the answer.

*/
const char* MDNSResponder::answerTxts(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                      const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                      const uint32_t p_u32AnswerIndex)
{
    stcQuery*               pMDNSQuery = (_validateMDNSHostHandle(p_hMDNSHost) ? _findQuery(*(clsHost*)p_hMDNSHost, p_hMDNSQuery);
                                          stcQuery::stcAnswer*    pSQAnswer = (pMDNSQuery ? pMDNSQuery->answerAtIndex(p_u32AnswerIndex) : 0);
                                          // Fill m_pcTxts (if not already done)
                                          if ((pSQAnswer) &&
                                              (pSQAnswer->m_Txts.m_pTxts) &&
                                              (!pSQAnswer->m_pcTxts))
{

    pSQAnswer->m_pcTxts = pSQAnswer->allocTxts(pSQAnswer->m_Txts.c_strLength());
        if (pSQAnswer->m_pcTxts)
        {
            pSQAnswer->m_Txts.c_str(pSQAnswer->m_pcTxts);
        }
    }
    return (pSQAnswer ? pSQAnswer->m_pcTxts : 0);
}

/*
    MDNSResponder::answerTxts (LEGACY 2)
*/
const char* MDNSResponder::answerTxts(const MDNSResponder::hMDNSQuery p_hMDNSQuery,
                                      const uint32_t p_u32AnswerIndex)
{
    return ((!m_HostList.empty())
            ? answerTxts((hMDNSHost)m_HostList.front(), p_hMDNSQuery, p_u32AnswerIndex)
            : 0);
}


/*
    PROBING
*/

/*
    MDNSResponder::setHostProbeResultCallback

    Set a callback for probe results. The callback is called, when probing
    for the host domain failes or succeedes.
    In the case of failure, the domain name should be changed via 'setHostName'
    When succeeded, the host domain will be announced by the MDNS responder.

*/
bool MDNSResponder::setHostProbeResultCallback(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                               MDNSResponder::MDNSHostProbeResultCallbackFn p_fnCallback)
{
    return ((_validateMDNSHostHandle(p_hMDNSHost)) &&
            (((clsHost*)p_hMDNSHost)->m_HostProbeInformation.m_fnProbeResultCallback = p_fnCallback, true));
}

/*
    MDNSResponder::setHostProbeResultCallback (LEGACY 2)
*/
bool MDNSResponder::setHostProbeResultCallback(MDNSResponder::MDNSHostProbeResultCallbackFn1 p_fnCallback)
{
    return setHostProbeResultCallback([p_fnCallback](MDNSResponder * p_pMDNSResponder,
                                                     hMDNSHost,
                                                     const char* p_pcDomainName,
                                                     bool p_bProbeResult)
    {
        if (p_fnCallback)
        {
            p_fnCallback(p_pMDNSResponder, p_pcDomainName, p_bProbeResult);
        }
    });
}

/*
    MDNSResponder::setHostProbeResultCallback (LEGACY 2)
*/
bool MDNSResponder::setHostProbeResultCallback(MDNSResponder::MDNSHostProbeResultCallbackFn2 p_fnCallback)
{
    return setHostProbeResultCallback([p_fnCallback](MDNSResponder*,
                                                     hMDNSHost,
                                                     const char* p_pcDomainName,
                                                     bool p_bProbeResult)
    {
        if (p_fnCallback)
        {
            p_fnCallback(p_pcDomainName, p_bProbeResult);
        }
    });
}

/*
    MDNSResponder::setServiceProbeResultCallback

    Set a service specific callback for probe results. The callback is called, when probing
    for the service domain failes or succeedes.
    In the case of failure, the service name should be changed via 'setServiceName'.
    When succeeded, the service domain will be announced by the MDNS responder.

*/
bool MDNSResponder::setServiceProbeResultCallback(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                  const MDNSResponder::hMDNSService p_hMDNSService,
                                                  MDNSResponder::MDNSServiceProbeResultCallbackFn p_fnCallback)
{
    return ((_validateMDNSHostHandle(p_hMDNSHost, p_hMDNSService)) &&
            (((stcService*)p_hMDNSService)->m_ProbeInformation.m_fnProbeResultCallback = p_fnCallback, true));
}

/*
    MDNSResponder::setServiceProbeResultCallback (LEGACY 2)
*/
bool MDNSResponder::setServiceProbeResultCallback(const MDNSResponder::hMDNSService p_hMDNSService,
                                                  MDNSResponder::MDNSServiceProbeResultCallbackFn1 p_fnCallback)
{
    return setServiceProbeResultCallback(p_hMDNSService, [p_fnCallback](MDNSResponder * p_pMDNSResponder,
                                                                        hMDNSHost,
                                                                        const hMDNSService p_hMDNSService,
                                                                        const char* p_pcServiceName,
                                                                        bool p_bProbeResult)
    {
        if (p_fnCallback)
        {
            p_fnCallback(p_pMDNSResponder, p_hMDNSService, p_pcServiceName, p_bProbeResult);
        }
    });
}

/*
    MDNSResponder::setServiceProbeResultCallback (LEGACY 2)
*/
bool MDNSResponder::setServiceProbeResultCallback(const MDNSResponder::hMDNSService p_hMDNSService,
                                                  MDNSResponder::MDNSServiceProbeResultCallbackFn2 p_fnCallback)
{
    return setServiceProbeResultCallback(p_hMDNSService, [p_fnCallback](MDNSResponder*,
                                                                        hMDNSHost,
                                                                        const hMDNSService p_hMDNSService,
                                                                        const char* p_pcServiceName,
                                                                        bool p_bProbeResult)
    {
        if (p_fnCallback)
        {
            p_fnCallback(p_hMDNSService, p_pcServiceName, p_bProbeResult);
        }
    });
}
#endif

/*
    MISC
*/

/*
    MDNSResponder::notifyNetIfChange

    Should be called, whenever the AP for the MDNS responder changes.
    A bit of this is caught by the event callbacks installed in the constructor.

*/
bool MDNSResponder::notifyNetIfChange(netif* p_pNetIf)
{
    clsHost*    pMDNSHost;
    return (((pMDNSHost = _findHost(p_pNetIf))) &&
            (pMDNSHost->restart()));
}

/*
    MDNSResponder::update

    Should be called in every 'loop'.

*/
bool MDNSResponder::update(const MDNSResponder::hMDNSHost p_hMDNSHost)
{
    return ((_validateMDNSHostHandle(p_hMDNSHost)) &&
            (_NRH2Ptr(p_hMDNSHost)->update()));
}

/*
    MDNSResponder::update (convenience)
*/
bool MDNSResponder::update(void)
{
    bool    bResult = true;
    for (clsHost* pMDNSHost : m_HostList)
    {
        if (!pMDNSHost->update())
        {
            bResult = false;
        }
    }
    return bResult;
}

/*
    MDNSResponder::announce

    Should be called, if the 'configuration' changes. Mainly this will be changes in the TXT items...
*/
bool MDNSResponder::announce(const MDNSResponder::hMDNSHost p_hMDNSHost)
{
    return ((_validateMDNSHostHandle(p_hMDNSHost)) &&
            (_NRH2Ptr(p_hMDNSHost)->announce(true, true)));
}

/*
    MDNSResponder::announce (convenience)
*/
bool MDNSResponder::announce(void)
{
    bool    bResult = true;
    for (clsHost* pMDNSHost : m_HostList)
    {
        if (!pMDNSHost->announce(true, true))
        {
            bResult = false;
        }
    }
    return bResult;
}

/*
    MDNSResponder::enableArduino

    Enable the OTA update service.

*/
MDNSResponder::hMDNSService MDNSResponder::enableArduino(const MDNSResponder::hMDNSHost p_hMDNSHost,
                                                         uint16_t p_u16Port,
                                                         bool p_bAuthUpload /*= false*/)
{
    hMDNSService    hService = addService(p_hMDNSHost, 0, "arduino", "tcp", p_u16Port);
    if (hService)
    {
        if ((!addServiceTxt(p_hMDNSHost, hService, "tcp_check", "no")) ||
            (!addServiceTxt(p_hMDNSHost, hService, "ssh_upload", "no")) ||
            (!addServiceTxt(p_hMDNSHost, hService, "board", STRINGIZE_VALUE_OF(ARDUINO_BOARD))) ||
            (!addServiceTxt(p_hMDNSHost, hService, "auth_upload", (p_bAuthUpload) ? "yes" : "no")))
        {

            removeService(p_hMDNSHost, hService);
            hService = 0;
        }
    }
    return hService;
}

#ifdef LATER

/*
    MDNSResponder::enableArduino (LEGACY 2)
*/
MDNSResponder::hMDNSService MDNSResponder::enableArduino(uint16_t p_u16Port,
                                                         bool p_bAuthUpload /*= false*/)
{
    hMDNSService    hMDNSService = 0;
    for (clsHost*& pMDNSHost : m_HostList)
    {
        hMDNSService    hLastMDNSService = enableArduino((hMDNSHost)it, p_u16Port, p_bAuthUpload);
        if ((hLastMDNSService) &&
            (!hMDNSService))
        {
            hMDNSService = hLastMDNSService;
        }
    }
    return hMDNSService;
}

#endif

} //namespace MDNSImplementation

} //namespace esp8266


