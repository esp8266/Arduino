/*
    LEAmDNS2_Host.cpp

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
#include <lwip/netif.h>
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

#ifdef MDNS_IPV4_SUPPORT
#include <lwip/igmp.h>
#endif
#ifdef MDNS_IPV6_SUPPORT
#include <lwip/mld6.h>
#endif


namespace esp8266
{

/*
    LEAmDNS
*/
namespace experimental
{

/*
    MDNSResponder::clsHost::clsHost constructor
*/
MDNSResponder::clsHost::clsHost(netif& p_rNetIf,
                                UdpContext& p_rUDPContext)
    :   m_rNetIf(p_rNetIf),
        m_NetIfState(static_cast<typeNetIfState>(enuNetIfState::None)),
        m_rUDPContext(p_rUDPContext),
        m_pcHostName(0),
        m_pcInstanceName(0),
        m_pServices(0),
        m_pQueries(0),
        m_fnServiceTxtCallback(0),
        m_HostProbeInformation()
{
}

/*
    MDNSResponder::clsHost::~clsHost destructor
*/
MDNSResponder::clsHost::~clsHost(void)
{
    _close();
}

/*
    MDNSResponder::clsHost::init
*/
bool MDNSResponder::clsHost::init(void)
{
    bool    bResult = true;

    // Join multicast group(s)
#ifdef MDNS_IPV4_SUPPORT
    ip_addr_t   multicast_addr_V4 = DNS_MQUERY_IPV4_GROUP_INIT;
    if (!(m_rNetIf.flags & NETIF_FLAG_IGMP))
    {
        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _createHost: Setting flag: flags & NETIF_FLAG_IGMP\n"), _DH()););
        m_rNetIf.flags |= NETIF_FLAG_IGMP;

        if (ERR_OK != igmp_start(&m_rNetIf))
        {
            DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _createHost: igmp_start FAILED!\n"), _DH()););
        }
    }

    bResult = ((bResult) &&
               (ERR_OK == igmp_joingroup_netif(&m_rNetIf, ip_2_ip4(&multicast_addr_V4))));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _createHost: igmp_joingroup_netif(%s) FAILED!\n"), _DH(), IPAddress(multicast_addr_V4).toString().c_str()););
#endif

#ifdef MDNS_IPV6_SUPPORT
    ip_addr_t   multicast_addr_V6 = DNS_MQUERY_IPV6_GROUP_INIT;
    bResult = ((bResult) &&
               (ERR_OK == mld6_joingroup_netif(&m_rNetIf, ip_2_ip6(&multicast_addr_V6))));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _createHost: mld6_joingroup_netif FAILED!\n"), _DH()););
#endif
    return bResult;
}

/*
    MDNSResponder::clsHost::setHostName
*/
bool MDNSResponder::clsHost::setHostName(const char* p_pcHostName)
{
    bool    bResult;
    if ((bResult = _allocHostName(p_pcHostName)))
    {
        m_HostProbeInformation.m_ProbingStatus = enuProbingStatus::ReadyToStart;

        // Replace 'auto-set' service names
        for (stcService* pService = m_pServices; ((bResult) && (pService)); pService = pService->m_pNext)
        {
            if ((pService->m_bAutoName) &&
                (!m_pcInstanceName))
            {
                bResult = pService->setName(p_pcHostName);
                pService->m_ProbeInformation.m_ProbingStatus = enuProbingStatus::ReadyToStart;
            }
        }
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::setHostName
*/
const char* MDNSResponder::clsHost::hostName(void) const
{
    return m_pcHostName;
}

/*
    MDNSResponder::clsHost::setHostProbeResultCallback
*/
bool MDNSResponder::clsHost::setHostProbeResultCallback(HostProbeResultCallbackFn p_fnCallback)
{
    m_HostProbeInformation.m_fnProbeResultCallback = p_fnCallback;
    return true;
}

/*
    MDNSResponder::clsHost::probeStatus
*/
bool MDNSResponder::clsHost::probeStatus(void) const
{
    return (enuProbingStatus::Done == m_HostProbeInformation.m_ProbingStatus);
}


/*
    SERVICE
*/

/*
    MDNSResponder::clsHost::setInstanceName
*/
bool MDNSResponder::clsHost::setInstanceName(const char* p_pcInstanceName)
{
    bool    bResult;
    if ((bResult = _allocInstanceName(p_pcInstanceName)))
    {
        // Replace 'auto-set' service names
        for (stcService* pService = m_pServices; ((bResult) && (pService)); pService = pService->m_pNext)
        {
            if (pService->m_bAutoName)
            {
                bResult = pService->setName(p_pcInstanceName);
                pService->m_ProbeInformation.m_ProbingStatus = enuProbingStatus::ReadyToStart;
            }
        }
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::instanceName
*/
const char* MDNSResponder::clsHost::instanceName(void) const
{
    return m_pcInstanceName;
}

/*
    MDNSResponder::clsHost::addService
*/
MDNSResponder::clsHost::stcService* MDNSResponder::clsHost::addService(const char* p_pcInstanceName,
                                                                       const char* p_pcServiceType,
                                                                       const char* p_pcProtocol,
                                                                       uint16_t p_u16Port)
{
    stcService* pService = 0;

    if (((!p_pcInstanceName) ||                                                     // NO name OR
         (MDNS_DOMAIN_LABEL_MAXLENGTH >= os_strlen(p_pcInstanceName))) &&           // Fitting name
        (p_pcServiceType) &&
        (MDNS_SERVICE_NAME_LENGTH >= os_strlen(p_pcServiceType)) &&
        (p_pcProtocol) &&
        ((MDNS_SERVICE_PROTOCOL_LENGTH - 1) != os_strlen(p_pcProtocol)) &&
        (p_u16Port))
    {
        if (!((pService = findService((p_pcInstanceName ? : (m_pcInstanceName ? : m_pcHostName)), p_pcServiceType, p_pcProtocol, p_u16Port)))) // Not already used
        {
            if (0 != (pService = _allocService(p_pcInstanceName, p_pcServiceType, p_pcProtocol, p_u16Port)))
            {
                // Init probing
                pService->m_ProbeInformation.m_ProbingStatus = enuProbingStatus::ReadyToStart;
            }
        }
    }   // else: bad arguments
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s addService: %s to add service '%s.%s.%s.local'!\n"), _DH(pService), (pService ? "Succeeded" : "FAILED"), (p_pcInstanceName ? : (m_pcInstanceName ? : (m_pcHostName ? :  "-"))), (p_pcServiceType ? : ""), (p_pcProtocol ? : "")););
    DEBUG_EX_ERR(if (!pService) DEBUG_OUTPUT.printf_P(PSTR("%s addService: FAILED to add service '%s.%s.%s.local'!\n"), _DH(pService), (p_pcInstanceName ? : (m_pcInstanceName ? : (m_pcHostName ? :  "-"))), (p_pcServiceType ? : ""), (p_pcProtocol ? : "")););
    return pService;
}

/*
    MDNSResponder::clsHost::removeService
*/
bool MDNSResponder::clsHost::removeService(MDNSResponder::clsHost::stcService* p_pMDNSService)
{
    bool    bResult = ((p_pMDNSService) &&
                       (_announceService(*p_pMDNSService, false)) &&
                       (_releaseService(p_pMDNSService)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _removeService: FAILED!\n"), _DH(p_pMDNSService)););
    return bResult;
}

/*
    MDNSResponder::clsHost::findService (const)
*/
const MDNSResponder::clsHost::stcService* MDNSResponder::clsHost::findService(const char* p_pcInstanceName,
                                                                              const char* p_pcServiceType,
                                                                              const char* p_pcProtocol,
                                                                              uint16_t p_u16Port /*= 0*/) const
{
    stcService* pService = m_pServices;
    while (pService)
    {
        if ((0 == strcmp(pService->m_pcName, p_pcInstanceName)) &&
            (0 == strcmp(pService->m_pcServiceType, p_pcServiceType)) &&
            (0 == strcmp(pService->m_pcProtocol, p_pcProtocol)) &&
            ((!p_u16Port) ||
             (p_u16Port == pService->m_u16Port)))
        {

            break;
        }
        pService = pService->m_pNext;
    }
    return pService;
}

/*
    MDNSResponder::clsHost::findService
*/
MDNSResponder::clsHost::stcService* MDNSResponder::clsHost::findService(const char* p_pcInstanceName,
                                                                        const char* p_pcServiceType,
                                                                        const char* p_pcProtocol,
                                                                        uint16_t p_u16Port /*= 0*/)
{
    return (stcService*)((const clsHost*)this)->findService(p_pcInstanceName, p_pcServiceType, p_pcProtocol, p_u16Port);
}

/*
    MDNSResponder::clsHost::validateService
*/
bool MDNSResponder::clsHost::validateService(const MDNSResponder::clsHost::stcService* p_pService) const
{
    const stcService*   pService = m_pServices;
    while (pService)
    {
        if (pService == p_pService)
        {
            break;
        }
        pService = pService->m_pNext;
    }
    return (0 != pService);
}

/*
    MDNSResponder::clsHost::setServiceName
*/
bool MDNSResponder::clsHost::setServiceName(MDNSResponder::clsHost::stcService* p_pMDNSService,
                                            const char* p_pcInstanceName)
{
    p_pcInstanceName = p_pcInstanceName ? : m_pcInstanceName;

    bool    bResult = ((p_pMDNSService) &&
                       ((!p_pcInstanceName) ||
                        (MDNS_DOMAIN_LABEL_MAXLENGTH >= os_strlen(p_pcInstanceName))) &&
                       (p_pMDNSService->setName(p_pcInstanceName)) &&
                       ((p_pMDNSService->m_ProbeInformation.m_ProbingStatus = enuProbingStatus::ReadyToStart), true));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _setServiceName: FAILED for '%s'!\n"), _DH(p_pMDNSService), (p_pcInstanceName ? : "-")););
    return bResult;
}

/*
    MDNSResponder::clsHost::setServiceName
*/
const char* MDNSResponder::clsHost::serviceName(const stcService* p_pMDNSService) const
{
    return ((p_pMDNSService)
            ? (p_pMDNSService->m_pcName)
            : 0);
}

/*
    MDNSResponder::clsHost::serviceType
*/
const char* MDNSResponder::clsHost::serviceType(const stcService* p_pMDNSService) const
{
    return ((p_pMDNSService)
            ? (p_pMDNSService->m_pcServiceType)
            : 0);
}

/*
    MDNSResponder::clsHost::serviceProtocol
*/
const char* MDNSResponder::clsHost::serviceProtocol(const stcService* p_pMDNSService) const
{
    return ((p_pMDNSService)
            ? (p_pMDNSService->m_pcProtocol)
            : 0);
}

/*
    MDNSResponder::clsHost::servicePort
*/
uint16_t MDNSResponder::clsHost::servicePort(const stcService* p_pMDNSService) const
{
    return ((p_pMDNSService)
            ? (p_pMDNSService->m_u16Port)
            : 0);
}


/*
    MDNSResponder::clsHost::setServiceProbeResultCallback
*/
bool MDNSResponder::clsHost::setServiceProbeResultCallback(stcService* p_pMDNSService,
                                                           ServiceProbeResultCallbackFn p_fnCallback)
{
    return ((p_pMDNSService)
            ? ((p_pMDNSService->m_ProbeInformation.m_fnProbeResultCallback = p_fnCallback), true)
            : false);
}

/*
    MDNSResponder::clsHost::setServiceName
*/
bool MDNSResponder::clsHost::serviceProbeStatus(const stcService* p_pMDNSService) const
{
    return ((p_pMDNSService) &&
            (enuProbingStatus::Done == p_pMDNSService->m_ProbeInformation.m_ProbingStatus));
}


/*
    SERVICE TXT
*/

/*
    MDNSResponder::clsHost::addServiceTxt
*/
MDNSResponder::clsHost::stcServiceTxt* MDNSResponder::clsHost::addServiceTxt(stcService* p_pMDNSService,
                                                                             const char* p_pcKey,
                                                                             const char* p_pcValue)
{
    return _addServiceTxt(p_pMDNSService, p_pcKey, p_pcValue, false);
}

/*
    MDNSResponder::clsHost::removeServiceTxt
*/
bool MDNSResponder::clsHost::removeServiceTxt(stcService* p_pMDNSService,
                                              stcServiceTxt* p_pTxt)
{
    bool    bResult = ((p_pMDNSService) &&
                       (p_pTxt) &&
                       (_releaseServiceTxt(p_pMDNSService, p_pTxt)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s removeServiceTxt: FAILED!\n"), _DH(p_pMDNSService)););
    return bResult;
}

/*
    MDNSResponder::clsHost::findServiceTxt (const)
*/
const MDNSResponder::clsHost::stcServiceTxt* MDNSResponder::clsHost::findServiceTxt(MDNSResponder::clsHost::stcService* p_pMDNSService,
                                                                                    const char* p_pcKey) const
{
    return (const stcServiceTxt*)((const clsHost*)this)->findServiceTxt(p_pMDNSService, p_pcKey);
}

/*
    MDNSResponder::clsHost::findServiceTxt
*/
MDNSResponder::clsHost::stcServiceTxt* MDNSResponder::clsHost::findServiceTxt(MDNSResponder::clsHost::stcService* p_pMDNSService,
                                                                              const char* p_pcKey)
{
    return _findServiceTxt(p_pMDNSService, p_pcKey);
}

/*
    MDNSResponder::clsHost::setDynamicServiceTxtCallback
*/
bool MDNSResponder::clsHost::setDynamicServiceTxtCallback(DynamicServiceTxtCallbackFn p_fnCallback)
{
    m_fnServiceTxtCallback = p_fnCallback;
    return true;
}

/*
    MDNSResponder::clsHost::setDynamicServiceTxtCallback
*/
bool MDNSResponder::clsHost::setDynamicServiceTxtCallback(stcService* p_pMDNSService,
                                                          DynamicServiceTxtCallbackFn p_fnCallback)
{
    return ((p_pMDNSService)
            ? ((p_pMDNSService->m_fnTxtCallback = p_fnCallback), true)
            : false);
}

/*
    MDNSResponder::clsHost::addDynamicServiceTxt

    Add a (dynamic) MDNS TXT item ('key' = 'value') to the service
    Dynamic TXT items are removed right after one-time use. So they need to be added
    every time the value s needed (via callback).
*/
MDNSResponder::clsHost::stcServiceTxt* MDNSResponder::clsHost::addDynamicServiceTxt(stcService* p_pMDNSService,
                                                                                    const char* p_pcKey,
                                                                                    const char* p_pcValue)
{
    return _addServiceTxt(p_pMDNSService, p_pcKey, p_pcValue, true);
}


/*
    QUERIES
*/
/*
    MDNSResponder::clsHost::queryService
*/
uint32_t MDNSResponder::clsHost::queryService(const char* p_pcService,
                                              const char* p_pcProtocol,
                                              const uint16_t p_u16Timeout /*= MDNS_QUERYSERVICES_WAIT_TIME*/)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s queryService '_%s._%s.local'\n"), _DH(), p_pcService, p_pcProtocol););

    stcQuery*    pMDNSQuery = 0;
    if ((p_pcService) && (*p_pcService) &&
        (p_pcProtocol) && (*p_pcProtocol) &&
        (p_u16Timeout) &&
        ((pMDNSQuery = _allocQuery(stcQuery::enuQueryType::Service))) &&
        (_buildDomainForService(p_pcService, p_pcProtocol, pMDNSQuery->m_Domain)))
    {
        if ((_removeLegacyQuery()) &&
            ((pMDNSQuery->m_bLegacyQuery = true)) &&
            (_sendMDNSQuery(*pMDNSQuery)))
        {
            // Wait for answers to arrive
            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s queryService: Waiting %u ms for answers...\n"), _DH(), p_u16Timeout););
            delay(p_u16Timeout);

            // All answers should have arrived by now -> stop adding new answers
            pMDNSQuery->m_bAwaitingAnswers = false;
        }
        else    // FAILED to send query
        {
            _removeQuery(pMDNSQuery);
        }
    }
    else
    {
        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s queryService: INVALID input data!\n"), _DH()););
    }
    return ((pMDNSQuery)
            ?   pMDNSQuery->answerCount()
            : 0);
}

/*
    MDNSResponder::clsHost::queryHost
*/
uint32_t MDNSResponder::clsHost::queryHost(const char* p_pcHostName,
                                           const uint16_t p_u16Timeout /*= MDNS_QUERYSERVICES_WAIT_TIME*/)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s queryHost '%s.local'\n"), _DH(), p_pcHostName););

    stcQuery*    pMDNSQuery = 0;
    if ((p_pcHostName) && (*p_pcHostName) &&
        (p_u16Timeout) &&
        ((pMDNSQuery = _allocQuery(stcQuery::enuQueryType::Host))) &&
        (_buildDomainForHost(p_pcHostName, pMDNSQuery->m_Domain)))
    {
        if ((_removeLegacyQuery()) &&
            ((pMDNSQuery->m_bLegacyQuery = true)) &&
            (_sendMDNSQuery(*pMDNSQuery)))
        {
            // Wait for answers to arrive
            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s queryHost: Waiting %u ms for answers...\n"), _DH(), p_u16Timeout););
            delay(p_u16Timeout);

            // All answers should have arrived by now -> stop adding new answers
            pMDNSQuery->m_bAwaitingAnswers = false;
        }
        else    // FAILED to send query
        {
            _removeQuery(pMDNSQuery);
        }
    }
    else
    {
        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s queryHost: INVALID input data!\n"), _DH()););
    }
    return ((pMDNSQuery)
            ?   pMDNSQuery->answerCount()
            : 0);
}

/*
    MDNSResponder::clsHost::removeQuery
*/
bool MDNSResponder::clsHost::removeQuery(void)
{
    return _removeLegacyQuery();
}

/*
    MDNSResponder::clsHost::hasQuery
*/
bool MDNSResponder::clsHost::hasQuery(void)
{
    return (0 != _findLegacyQuery());
}

/*
    MDNSResponder::clsHost::getQuery
*/
MDNSResponder::clsHost::stcQuery* MDNSResponder::clsHost::getQuery(void)
{
    return _findLegacyQuery();
}

/*
    MDNSResponder::clsHost::installServiceQuery
*/
MDNSResponder::clsHost::stcQuery* MDNSResponder::clsHost::installServiceQuery(const char* p_pcService,
                                                                              const char* p_pcProtocol,
                                                                              MDNSResponder::clsHost::QueryCallbackFn p_fnCallback)
{
    stcQuery*   pMDNSQuery = 0;
    if ((p_pcService) && (*p_pcService) &&
        (p_pcProtocol) && (*p_pcProtocol) &&
        (p_fnCallback) &&
        ((pMDNSQuery = _allocQuery(stcQuery::enuQueryType::Service))) &&
        (_buildDomainForService(p_pcService, p_pcProtocol, pMDNSQuery->m_Domain)))
    {

        pMDNSQuery->m_fnCallback = p_fnCallback;
        pMDNSQuery->m_bLegacyQuery = false;

        if (_sendMDNSQuery(*pMDNSQuery))
        {
            pMDNSQuery->m_u8SentCount = 1;
            pMDNSQuery->m_ResendTimeout.reset(MDNS_DYNAMIC_QUERY_RESEND_DELAY);
        }
        else
        {
            _removeQuery(pMDNSQuery);
        }
    }
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s installServiceQuery: %s for '_%s._%s.local'!\n\n"), _DH(), (pMDNSQuery ? "Succeeded" : "FAILED"), (p_pcService ? : "-"), (p_pcProtocol ? : "-")););
    DEBUG_EX_ERR(if (!pMDNSQuery) DEBUG_OUTPUT.printf_P(PSTR("%s installServiceQuery: FAILED for '_%s._%s.local'!\n\n"), _DH(), (p_pcService ? : "-"), (p_pcProtocol ? : "-")););
    return pMDNSQuery;
}

/*
    MDNSResponder::clsHost::installHostQuery
*/
MDNSResponder::clsHost::stcQuery* MDNSResponder::clsHost::installHostQuery(const char* p_pcHostName,
                                                                           MDNSResponder::clsHost::QueryCallbackFn p_fnCallback)
{
    stcQuery*   pMDNSQuery = 0;
    if ((p_pcHostName) && (*p_pcHostName))
    {
        stcRRDomain    domain;
        pMDNSQuery = ((_buildDomainForHost(p_pcHostName, domain))
                      ? _installDomainQuery(domain, stcQuery::enuQueryType::Host, p_fnCallback)
                      : 0);
    }
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s installHostQuery: %s for '%s.local'!\n\n"), _DH(), (pMDNSQuery ? "Succeeded" : "FAILED"), (p_pcHostName ? : "-")););
    DEBUG_EX_ERR(if (!pMDNSQuery) DEBUG_OUTPUT.printf_P(PSTR("%s installHostQuery: FAILED for '%s.local'!\n\n"), _DH(), (p_pcHostName ? : "-")););
    return pMDNSQuery;
}

/*
    MDNSResponder::clsHost::removeQuery
*/
bool MDNSResponder::clsHost::removeQuery(MDNSResponder::clsHost::stcQuery* p_pMDNSQuery)
{
    bool    bResult = ((p_pMDNSQuery) &&
                       (_removeQuery(p_pMDNSQuery)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s removeQuery: FAILED!\n"), _DH()););
    return bResult;
}


/*
    PROCESSING
*/

/*
    MDNSResponder::clsHost::processUDPInput
*/
bool MDNSResponder::clsHost::processUDPInput()
{
    bool    bResult = false;

    bResult = ((_checkNetIfState()) &&      // Any changes in the netif state?
               (_parseMessage()));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("")););

    return bResult;
}

/*
    MDNSResponder::clsHost::update
*/
bool MDNSResponder::clsHost::update(void)
{
    return ((_checkNetIfState()) &&      // Any changes in the netif state?
            (_updateProbeStatus()) &&    // Probing
            (_checkQueryCache()));
}

/*
    MDNSResponder::clsHost::restart
*/
bool MDNSResponder::clsHost::restart(void)
{
    return (_resetProbeStatus(true));   // Stop and restart probing
}





/*
    P R O T E C T E D
*/

/*
    MDNSResponder::clsHost::_close
*/
bool MDNSResponder::clsHost::_close(void)
{
    /*  _resetProbeStatus(false);   // Stop probing

        _releaseQueries();
        _releaseServices();
        _releaseHostName();*/

    // Leave multicast group(s)
#ifdef MDNS_IPV4_SUPPORT
    ip_addr_t   multicast_addr_V4 = DNS_MQUERY_IPV4_GROUP_INIT;
    if (ERR_OK != igmp_leavegroup_netif(&m_rNetIf, ip_2_ip4(&multicast_addr_V4)/*(const struct ip4_addr *)&multicast_addr_V4.u_addr.ip4*/))
    {
        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("\n")););
    }
#endif
#ifdef MDNS_IPV6_SUPPORT
    ip_addr_t   multicast_addr_V6 = DNS_MQUERY_IPV6_GROUP_INIT;
    if (ERR_OK != mld6_leavegroup_netif(&m_rNetIf, ip_2_ip6(&multicast_addr_V6)/*&(multicast_addr_V6.u_addr.ip6)*/))
    {
        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("\n")););
    }
#endif
    return true;
}


/*
    NETIF
*/

/*
    MDNSResponder::clsHost::_getNetIfState

    Returns the current netif state.

*/
MDNSResponder::clsHost::typeNetIfState MDNSResponder::clsHost::_getNetIfState(void) const
{
    typeNetIfState  curNetIfState = static_cast<typeNetIfState>(enuNetIfState::None);

    if (netif_is_up(&m_rNetIf))
    {
        curNetIfState |= static_cast<typeNetIfState>(enuNetIfState::IsUp);

        // Check if netif link is up
        if ((netif_is_link_up(&m_rNetIf)) &&
            ((&m_rNetIf != netif_get_by_index(WIFI_STA)) ||
             (STATION_GOT_IP == wifi_station_get_connect_status())))
        {
            curNetIfState |= static_cast<typeNetIfState>(enuNetIfState::LinkIsUp);
        }

        // Check for IPv4 address
        if (_getResponderIPAddress(enuIPProtocolType::V4).isSet())
        {
            curNetIfState |= static_cast<typeNetIfState>(enuNetIfState::IPv4);
        }
        // Check for IPv6 address
        if (_getResponderIPAddress(enuIPProtocolType::V6).isSet())
        {
            curNetIfState |= static_cast<typeNetIfState>(enuNetIfState::IPv6);
        }
    }
    return curNetIfState;
}

/*
    MDNSResponder::clsHost::_checkNetIfState

    Checks the netif state.
    If eg. a new address appears, the announcing is restarted.

*/
bool MDNSResponder::clsHost::_checkNetIfState(void)
{
    typeNetIfState  curNetIfState;
    if (m_NetIfState != ((curNetIfState = _getNetIfState())))
    {
        // Some state change happened
        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _checkNetIfState: DID CHANGE NETIF STATE\n\n"), _DH()););
        DEBUG_EX_INFO(
            if ((curNetIfState & static_cast<typeNetIfState>(enuNetIfState::IsUp)) != (m_NetIfState & static_cast<typeNetIfState>(enuNetIfState::IsUp)))
    {
        DEBUG_OUTPUT.printf_P(PSTR("%s _checkNetIfState: Netif is up: %s\n"), _DH(), ((curNetIfState & static_cast<typeNetIfState>(enuNetIfState::IsUp)) ? "YES" : "NO"));
        }
        if ((curNetIfState & static_cast<typeNetIfState>(enuNetIfState::LinkIsUp)) != (m_NetIfState & static_cast<typeNetIfState>(enuNetIfState::LinkIsUp)))
    {
        DEBUG_OUTPUT.printf_P(PSTR("%s _checkNetIfState: Netif link is up: %s\n"), _DH(), ((curNetIfState & static_cast<typeNetIfState>(enuNetIfState::LinkIsUp)) ? "YES" : "NO"));
        }
        if ((curNetIfState & static_cast<typeNetIfState>(enuNetIfState::IPv4)) != (m_NetIfState & static_cast<typeNetIfState>(enuNetIfState::IPv4)))
    {
        DEBUG_OUTPUT.printf_P(PSTR("%s _checkNetIfState: IPv4 address is set: %s\n"), _DH(), ((curNetIfState & static_cast<typeNetIfState>(enuNetIfState::IPv4)) ? "YES" : "NO"));
            if (curNetIfState & static_cast<typeNetIfState>(enuNetIfState::IPv4))
            {
                DEBUG_OUTPUT.printf_P(PSTR("%s _checkNetIfState: IPv4 address: %s\n"), _DH(), _getResponderIPAddress(enuIPProtocolType::V4).toString().c_str());
            }
        }
        if ((curNetIfState & static_cast<typeNetIfState>(enuNetIfState::IPv6)) != (m_NetIfState & static_cast<typeNetIfState>(enuNetIfState::IPv6)))
    {
        DEBUG_OUTPUT.printf_P(PSTR("%s _checkNetIfState: IPv6 address is set: %s\n"), _DH(), ((curNetIfState & static_cast<typeNetIfState>(enuNetIfState::IPv6)) ? "YES" : "NO"));
            if (curNetIfState & static_cast<typeNetIfState>(enuNetIfState::IPv6))
            {
                DEBUG_OUTPUT.printf_P(PSTR("%s _checkNetIfState: IPv6 address: %s\n"), _DH(), _getResponderIPAddress(enuIPProtocolType::V6).toString().c_str());
            }
        }
        );

        if ((curNetIfState & static_cast<typeNetIfState>(enuNetIfState::LinkMask)) != (m_NetIfState & static_cast<typeNetIfState>(enuNetIfState::LinkMask)))
        {
            // Link came up (restart() will alloc a m_pUDPContext, ...) or down (_restart() will remove an existing m_pUDPContext, ...)
            restart();
        }
        else if (curNetIfState & static_cast<typeNetIfState>(enuNetIfState::LinkIsUp))
        {
            // Link is up (unchanged)
            if ((curNetIfState & static_cast<typeNetIfState>(enuNetIfState::IPMask)) != (m_NetIfState & static_cast<typeNetIfState>(enuNetIfState::IPMask)))
            {
                // IP state changed
                // TODO: If just a new IP address was added, a simple re-announcement should be enough
                restart();
            }
        }
        /*  if (enuProbingStatus::Done == m_HostProbeInformation.m_ProbingStatus) {
            // Probing is done, prepare to (re)announce host
            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _checkNetIfState: Preparing to (re)announce host.\n")););
            //m_HostProbeInformation.m_ProbingStatus = enuProbingStatus::Done;
            m_HostProbeInformation.m_u8SentCount = 0;
            m_HostProbeInformation.m_Timeout.reset(MDNS_ANNOUNCE_DELAY);
            }*/
        m_NetIfState = curNetIfState;
    }

    bool    bResult = ((curNetIfState & static_cast<typeNetIfState>(enuNetIfState::LinkMask)) &&    // Continue if Link is UP
                       (curNetIfState & static_cast<typeNetIfState>(enuNetIfState::IPMask)));       // AND has any IP
    //DEBUG_EX_INFO(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _checkNetIfState: Link is DOWN or NO IP address!\n"), _DH()););
    return bResult;
}


/*
    DOMAIN NAMES
*/

/*
    MDNSResponder::clsHost::_allocDomainName
*/
bool MDNSResponder::clsHost::_allocDomainName(const char* p_pcNewDomainName,
                                              char*& p_rpcDomainName)
{
    bool    bResult = false;

    _releaseDomainName(p_rpcDomainName);

    size_t  stLength = 0;
    if ((p_pcNewDomainName) &&
        (MDNS_DOMAIN_LABEL_MAXLENGTH >= (stLength = strlen(p_pcNewDomainName))))   // char max size for a single label
    {
        // Copy in hostname characters as lowercase
        if ((bResult = (0 != (p_rpcDomainName = new char[stLength + 1]))))
        {
#ifdef MDNS_FORCE_LOWERCASE_HOSTNAME
            size_t i = 0;
            for (; i < stLength; ++i)
            {
                p_rpcDomainName[i] = (isupper(p_pcNewDomainName[i]) ? tolower(p_pcNewDomainName[i]) : p_pcNewDomainName[i]);
            }
            p_rpcDomainName[i] = 0;
#else
            strncpy(p_rpcDomainName, p_pcNewDomainName, (stLength + 1));
#endif
        }
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::_releaseDomainName
*/
bool MDNSResponder::clsHost::_releaseDomainName(char*& p_rpcDomainName)
{
    bool    bResult;
    if ((bResult = (0 != p_rpcDomainName)))
    {
        delete[] p_rpcDomainName;
        p_rpcDomainName = 0;
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::_allocHostName
*/
bool MDNSResponder::clsHost::_allocHostName(const char* p_pcHostName)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _allocHostName (%s)\n"), _DH(), p_pcHostName););
    return _allocDomainName(p_pcHostName, m_pcHostName);
}

/*
    MDNSResponder::clsHost::_releaseHostName
*/
bool MDNSResponder::clsHost::_releaseHostName(void)
{
    return _releaseDomainName(m_pcHostName);
}

/*
    MDNSResponder::clsHost::_allocInstanceName
*/
bool MDNSResponder::clsHost::_allocInstanceName(const char* p_pcInstanceName)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _allocInstanceName (%s)\n"), _DH(), p_pcHostName););
    return _allocDomainName(p_pcInstanceName, m_pcInstanceName);
}

/*
    MDNSResponder::clsHost::_releaseInstanceName
*/
bool MDNSResponder::clsHost::_releaseInstanceName(void)
{
    return _releaseDomainName(m_pcInstanceName);
}


/*
    SERVICE
*/

/*
    MDNSResponder::clsHost::_allocService
*/
MDNSResponder::clsHost::stcService* MDNSResponder::clsHost::_allocService(const char* p_pcName,
                                                                          const char* p_pcServiceType,
                                                                          const char* p_pcProtocol,
                                                                          uint16_t p_u16Port)
{
    stcService* pService = 0;
    if (((!p_pcName) ||
         (MDNS_DOMAIN_LABEL_MAXLENGTH >= strlen(p_pcName))) &&
        (p_pcServiceType) &&
        (MDNS_SERVICE_NAME_LENGTH >= strlen(p_pcServiceType)) &&
        (p_pcProtocol) &&
        (MDNS_SERVICE_PROTOCOL_LENGTH >= strlen(p_pcProtocol)) &&
        (p_u16Port) &&
        (0 != ((pService = new stcService))) &&
        (pService->setName(p_pcName ? : (m_pcInstanceName ? : m_pcHostName))) &&
        (pService->setServiceType(p_pcServiceType)) &&
        (pService->setProtocol(p_pcProtocol)))
    {
        pService->m_bAutoName = (0 == p_pcName);
        pService->m_u16Port = p_u16Port;

        // Add to list (or start list)
        pService->m_pNext = m_pServices;
        m_pServices = pService;
    }
    return pService;
}

/*
    MDNSResponder::clsHost::_releaseService
*/
bool MDNSResponder::clsHost::_releaseService(MDNSResponder::clsHost::stcService* p_pService)
{
    bool    bResult = false;

    if (p_pService)
    {
        stcService* pPred = m_pServices;
        while ((pPred) &&
               (pPred->m_pNext != p_pService))
        {
            pPred = pPred->m_pNext;
        }
        if (pPred)
        {
            pPred->m_pNext = p_pService->m_pNext;
            delete p_pService;
            bResult = true;
        }
        else    // No predecesor
        {
            if (m_pServices == p_pService)
            {
                m_pServices = p_pService->m_pNext;
                delete p_pService;
                bResult = true;
            }
            else
            {
                DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _releaseService: INVALID service!"), _DH(p_pService)););
            }
        }
    }
    return bResult;
}


/*
    SERVICE TXT
*/

/*
    MDNSResponder::clsHost::_allocServiceTxt
*/
MDNSResponder::clsHost::stcServiceTxt* MDNSResponder::clsHost::_allocServiceTxt(MDNSResponder::clsHost::stcService* p_pService,
                                                                                const char* p_pcKey,
                                                                                const char* p_pcValue,
                                                                                bool p_bTemp)
{
    stcServiceTxt*  pTxt = 0;

    if ((p_pService) &&
        (p_pcKey) &&
        (MDNS_SERVICE_TXT_MAXLENGTH > (p_pService->m_Txts.length() +
                                       1 +                                 // Length byte
                                       (p_pcKey ? strlen(p_pcKey) : 0) +
                                       1 +                                 // '='
                                       (p_pcValue ? strlen(p_pcValue) : 0))))
    {

        pTxt = new stcServiceTxt;
        if (pTxt)
        {
            size_t  stLength = (p_pcKey ? strlen(p_pcKey) : 0);
            pTxt->m_pcKey = new char[stLength + 1];
            if (pTxt->m_pcKey)
            {
                strncpy(pTxt->m_pcKey, p_pcKey, stLength); pTxt->m_pcKey[stLength] = 0;
            }

            if (p_pcValue)
            {
                stLength = (p_pcValue ? strlen(p_pcValue) : 0);
                pTxt->m_pcValue = new char[stLength + 1];
                if (pTxt->m_pcValue)
                {
                    strncpy(pTxt->m_pcValue, p_pcValue, stLength); pTxt->m_pcValue[stLength] = 0;
                }
            }
            pTxt->m_bTemp = p_bTemp;

            // Add to list (or start list)
            p_pService->m_Txts.add(pTxt);
        }
    }
    return pTxt;
}

/*
    MDNSResponder::clsHost::_releaseServiceTxt
*/
bool MDNSResponder::clsHost::_releaseServiceTxt(MDNSResponder::clsHost::stcService* p_pService,
                                                MDNSResponder::clsHost::stcServiceTxt* p_pTxt)
{
    return ((p_pService) &&
            (p_pTxt) &&
            (p_pService->m_Txts.remove(p_pTxt)));
}

/*
    MDNSResponder::clsHost::_updateServiceTxt
*/
MDNSResponder::clsHost::stcServiceTxt* MDNSResponder::clsHost::_updateServiceTxt(MDNSResponder::clsHost::stcService* p_pService,
                                                                                 MDNSResponder::clsHost::stcServiceTxt* p_pTxt,
                                                                                 const char* p_pcValue,
                                                                                 bool p_bTemp)
{
    if ((p_pService) &&
        (p_pTxt) &&
        (MDNS_SERVICE_TXT_MAXLENGTH > (p_pService->m_Txts.length() -
                                       (p_pTxt->m_pcValue ? strlen(p_pTxt->m_pcValue) : 0) +
                                       (p_pcValue ? strlen(p_pcValue) : 0))))
    {
        p_pTxt->update(p_pcValue);
        p_pTxt->m_bTemp = p_bTemp;
    }
    return p_pTxt;
}

/*
    MDNSResponder::clsHost::_findServiceTxt
*/
MDNSResponder::clsHost::stcServiceTxt* MDNSResponder::clsHost::_findServiceTxt(MDNSResponder::clsHost::stcService* p_pService,
                                                                               const char* p_pcKey)
{
    return (p_pService ? p_pService->m_Txts.find(p_pcKey) : 0);
}

/*
    MDNSResponder::clsHost::_addServiceTxt
*/
MDNSResponder::clsHost::stcServiceTxt* MDNSResponder::clsHost::_addServiceTxt(MDNSResponder::clsHost::stcService* p_pService,
                                                                              const char* p_pcKey,
                                                                              const char* p_pcValue,
                                                                              bool p_bTemp)
{
    stcServiceTxt*  pResult = 0;

    if ((p_pService) &&
        (p_pcKey) &&
        (strlen(p_pcKey)))
    {

        stcServiceTxt*  pTxt = p_pService->m_Txts.find(p_pcKey);
        if (pTxt)
        {
            pResult = _updateServiceTxt(p_pService, pTxt, p_pcValue, p_bTemp);
        }
        else
        {
            pResult = _allocServiceTxt(p_pService, p_pcKey, p_pcValue, p_bTemp);
        }
    }
    return pResult;
}

/*
    MDNSResponder::clsHost::_answerKeyValue
    /
    MDNSResponder::clsHost::stcServiceTxt* MDNSResponder::clsHost::_answerKeyValue(const MDNSResponder::clsHost::stcQuery p_pQuery,
                                                                 const uint32_t p_u32AnswerIndex)
    {
    stcQuery::stcAnswer* pSQAnswer = (p_pQuery ? p_pQuery->answerAtIndex(p_u32AnswerIndex) : 0);
    // Fill m_pcTxts (if not already done)
    return (pSQAnswer) ?  pSQAnswer->m_Txts.m_pTxts : 0;
    }*/

/*
    MDNSResponder::clsHost::_collectServiceTxts
*/
bool MDNSResponder::clsHost::_collectServiceTxts(MDNSResponder::clsHost::stcService& p_rService)
{
    if (m_fnServiceTxtCallback)
    {
        //m_fnServiceTxtCallback(*this, p_pService);
    }
    if (p_rService.m_fnTxtCallback)
    {
        //p_pService->m_fnTxtCallback(*this, p_pService);
    }
    return true;
}

/*
    MDNSResponder::clsHost::_releaseTempServiceTxts
*/
bool MDNSResponder::clsHost::_releaseTempServiceTxts(MDNSResponder::clsHost::stcService& p_rService)
{
    return (p_rService.m_Txts.removeTempTxts());
}


/*
    QUERIES
*/

/*
    MDNSResponder::_allocQuery
*/
MDNSResponder::clsHost::stcQuery* MDNSResponder::clsHost::_allocQuery(MDNSResponder::clsHost::stcQuery::enuQueryType p_QueryType)
{
    stcQuery*    pQuery = new stcQuery(p_QueryType);
    if (pQuery)
    {
        // Link to query list
        pQuery->m_pNext = m_pQueries;
        m_pQueries = pQuery;
    }
    return m_pQueries;
}

/*
    MDNSResponder:clsHost:::_removeQuery
*/
bool MDNSResponder::clsHost::_removeQuery(MDNSResponder::clsHost::stcQuery* p_pQuery)
{
    bool    bResult = false;

    if (p_pQuery)
    {
        stcQuery*    pPred = m_pQueries;
        while ((pPred) &&
               (pPred->m_pNext != p_pQuery))
        {
            pPred = pPred->m_pNext;
        }
        if (pPred)
        {
            pPred->m_pNext = p_pQuery->m_pNext;
            delete p_pQuery;
            bResult = true;
        }
        else    // No predecesor
        {
            if (m_pQueries == p_pQuery)
            {
                m_pQueries = p_pQuery->m_pNext;
                delete p_pQuery;
                bResult = true;
            }
            else
            {
                DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _releaseQuery: INVALID query!"), _DH()););
            }
        }
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::_removeLegacyQuery
*/
bool MDNSResponder::clsHost::_removeLegacyQuery(void)
{
    stcQuery*    pLegacyQuery = 0;
    return (((pLegacyQuery = _findLegacyQuery()))
            ? _removeQuery(pLegacyQuery)
            : false);
}

/*
    MDNSResponder::clsHost::_findLegacyQuery
*/
MDNSResponder::clsHost::stcQuery* MDNSResponder::clsHost::_findLegacyQuery(void)
{
    stcQuery*    pLegacyQuery = m_pQueries;
    while (pLegacyQuery)
    {
        if (pLegacyQuery->m_bLegacyQuery)
        {
            break;
        }
        pLegacyQuery = pLegacyQuery->m_pNext;
    }
    return pLegacyQuery;
}

/*
    MDNSResponder::clsHost::_releaseQueries
*/
bool MDNSResponder::clsHost::_releaseQueries(void)
{
    while (m_pQueries)
    {
        stcQuery*    pNext = m_pQueries->m_pNext;
        delete m_pQueries;
        m_pQueries = pNext;
    }
    return true;
}

/*
    MDNSResponder::clsHost::_findNextQueryByDomain
*/
MDNSResponder::clsHost::stcQuery* MDNSResponder::clsHost::_findNextQueryByDomain(const MDNSResponder::clsHost::stcRRDomain& p_Domain,
                                                                                 const MDNSResponder::clsHost::stcQuery::enuQueryType p_QueryType,
                                                                                 const stcQuery* p_pPrevQuery)
{
    stcQuery*    pMatchingQuery = 0;

    stcQuery*    pQuery = (p_pPrevQuery ? p_pPrevQuery->m_pNext : m_pQueries);
    while (pQuery)
    {
        if (((stcQuery::enuQueryType::None == p_QueryType) ||
             (pQuery->m_QueryType == p_QueryType)) &&
            (p_Domain == pQuery->m_Domain))
        {

            pMatchingQuery = pQuery;
            break;
        }
        pQuery = pQuery->m_pNext;
    }
    return pMatchingQuery;
}

/*
    MDNSResponder::clsHost::_installDomainQuery
*/
MDNSResponder::clsHost::stcQuery* MDNSResponder::clsHost::_installDomainQuery(MDNSResponder::clsHost::stcRRDomain& p_Domain,
                                                                              MDNSResponder::clsHost::stcQuery::enuQueryType p_QueryType,
                                                                              MDNSResponder::clsHost::QueryCallbackFn p_fnCallback)
{
    stcQuery*    pQuery = 0;

    if ((p_fnCallback) &&
        ((pQuery = _allocQuery(p_QueryType))))
    {
        pQuery->m_Domain = p_Domain;
        pQuery->m_fnCallback = p_fnCallback;
        pQuery->m_bLegacyQuery = false;

        if (_sendMDNSQuery(*pQuery))
        {
            pQuery->m_u8SentCount = 1;
            pQuery->m_ResendTimeout.reset(MDNS_DYNAMIC_QUERY_RESEND_DELAY);
        }
        else
        {
            _removeQuery(pQuery);
        }
    }
    DEBUG_EX_INFO(
        DEBUG_OUTPUT.printf_P(PSTR("%s _installDomainQuery: %s for "), (pQuery ? "Succeeded" : "FAILED"), _DH());
        _printRRDomain(p_Domain);
        DEBUG_OUTPUT.println();
    );
    DEBUG_EX_ERR(
        if (!pQuery)
{
    DEBUG_OUTPUT.printf_P(PSTR("%s _installDomainQuery: FAILED for "), _DH());
        _printRRDomain(p_Domain);
        DEBUG_OUTPUT.println();
    }
    );
    return pQuery;
}

/*
    MDNSResponder::clsHost::_hasQueriesWaitingForAnswers
*/
bool MDNSResponder::clsHost::_hasQueriesWaitingForAnswers(void) const
{
    bool    bOpenQueries = false;

    for (stcQuery* pQuery = m_pQueries; pQuery; pQuery = pQuery->m_pNext)
    {
        if (pQuery->m_bAwaitingAnswers)
        {
            bOpenQueries = true;
            break;
        }
    }
    return bOpenQueries;
}

/*
    MDNSResponder::clsHost::_executeQueryCallback
*/
bool MDNSResponder::clsHost::_executeQueryCallback(const stcQuery& p_Query,
                                                   const stcQuery::stcAnswer& p_Answer,
                                                   typeQueryAnswerType p_QueryAnswerTypeFlags,
                                                   bool p_bSetContent)
{
    if (p_Query.m_fnCallback)
    {
        p_Query.m_fnCallback(*this, p_Query, p_Answer, p_QueryAnswerTypeFlags, p_bSetContent);
    }
    return true;
}



}   // namespace MDNSImplementation

}   // namespace esp8266
