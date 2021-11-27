/*
    LEAmDNS_Helpers.cpp

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

#include <lwip/igmp.h>
#include <stdlib_noniso.h> // strrstr()

#include "ESP8266mDNS.h"
#include "LEAmDNS_lwIPdefs.h"
#include "LEAmDNS_Priv.h"

namespace esp8266
{

/*
    LEAmDNS
*/
namespace MDNSImplementation
{

/**
    HELPERS
*/

/*
    MDNSResponder::indexDomain (static)

    Updates the given domain 'p_rpcHostname' by appending a delimiter and an index number.

    If the given domain already hasa numeric index (after the given delimiter), this index
    incremented. If not, the delimiter and index '2' is added.

    If 'p_rpcHostname' is empty (==0), the given default name 'p_pcDefaultHostname' is used,
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
        if (pFoundDivider)      // maybe already extended
        {
            char*         pEnd = 0;
            unsigned long ulIndex = strtoul((pFoundDivider + strlen(pcDivider)), &pEnd, 10);
            if ((ulIndex) &&
                    ((pEnd - p_rpcDomain) == (ptrdiff_t)strlen(p_rpcDomain)) &&
                    (!*pEnd))         // Valid (old) index found
            {

                char    acIndexBuffer[16];
                sprintf(acIndexBuffer, "%lu", (++ulIndex));
                size_t  stLength = ((pFoundDivider - p_rpcDomain + strlen(pcDivider)) + strlen(acIndexBuffer) + 1);
                char*   pNewHostname = new char[stLength];
                if (pNewHostname)
                {
                    memcpy(pNewHostname, p_rpcDomain, (pFoundDivider - p_rpcDomain + strlen(pcDivider)));
                    pNewHostname[pFoundDivider - p_rpcDomain + strlen(pcDivider)] = 0;
                    strcat(pNewHostname, acIndexBuffer);

                    delete[] p_rpcDomain;
                    p_rpcDomain = pNewHostname;

                    bResult = true;
                }
                else
                {
                    DEBUG_EX_ERR(DEBUG_OUTPUT.println(F("[MDNSResponder] indexDomain: FAILED to alloc new hostname!")););
                }
            }
            else
            {
                pFoundDivider = 0;  // Flag the need to (base) extend the hostname
            }
        }

        if (!pFoundDivider)     // not yet extended (or failed to increment extension) -> start indexing
        {
            size_t    stLength = strlen(p_rpcDomain) + (strlen(pcDivider) + 1 + 1);   // Name + Divider + '2' + '\0'
            char*     pNewHostname = new char[stLength];
            if (pNewHostname)
            {
                sprintf(pNewHostname, "%s%s2", p_rpcDomain, pcDivider);

                delete[] p_rpcDomain;
                p_rpcDomain = pNewHostname;

                bResult = true;
            }
            else
            {
                DEBUG_EX_ERR(DEBUG_OUTPUT.println(F("[MDNSResponder] indexDomain: FAILED to alloc new hostname!")););
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
            DEBUG_EX_ERR(DEBUG_OUTPUT.println(F("[MDNSResponder] indexDomain: FAILED to alloc new hostname!")););
        }
    }
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] indexDomain: %s\n"), p_rpcDomain););
    return bResult;
}


/*
    UDP CONTEXT
*/

bool MDNSResponder::_callProcess(void)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf("[MDNSResponder] _callProcess (%lu, triggered by: %s)\n", millis(), IPAddress(m_pUDPContext->getRemoteAddress()).toString().c_str()););

    return _process(false);
}

/*
    MDNSResponder::_allocUDPContext

    (Re-)Creates the one-and-only UDP context for the MDNS responder.
    The context is added to the 'multicast'-group and listens to the MDNS port (5353).
    The travel-distance for multicast messages is set to 1 (local, via MDNS_MULTICAST_TTL).
    Messages are received via the MDNSResponder '_update' function. CAUTION: This function
    is called from the WiFi stack side of the ESP stack system.

*/
bool MDNSResponder::_allocUDPContext(void)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.println("[MDNSResponder] _allocUDPContext"););

    _releaseUDPContext();
    _joinMulticastGroups();

    m_pUDPContext = new UdpContext;
    m_pUDPContext->ref();

    if (m_pUDPContext->listen(IP4_ADDR_ANY, DNS_MQUERY_PORT))
    {
        m_pUDPContext->setMulticastTTL(MDNS_MULTICAST_TTL);
        m_pUDPContext->onRx(std::bind(&MDNSResponder::_callProcess, this));
    }
    else
    {
        return false;
    }

    return true;
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
        _leaveMulticastGroups();
    }
    return true;
}


/*
    SERVICE QUERY
*/

/*
    MDNSResponder::_allocServiceQuery
*/
MDNSResponder::stcMDNSServiceQuery* MDNSResponder::_allocServiceQuery(void)
{

    stcMDNSServiceQuery*    pServiceQuery = new stcMDNSServiceQuery;
    if (pServiceQuery)
    {
        // Link to query list
        pServiceQuery->m_pNext = m_pServiceQueries;
        m_pServiceQueries = pServiceQuery;
    }
    return m_pServiceQueries;
}

/*
    MDNSResponder::_removeServiceQuery
*/
bool MDNSResponder::_removeServiceQuery(MDNSResponder::stcMDNSServiceQuery* p_pServiceQuery)
{

    bool    bResult = false;

    if (p_pServiceQuery)
    {
        stcMDNSServiceQuery*    pPred = m_pServiceQueries;
        while ((pPred) &&
                (pPred->m_pNext != p_pServiceQuery))
        {
            pPred = pPred->m_pNext;
        }
        if (pPred)
        {
            pPred->m_pNext = p_pServiceQuery->m_pNext;
            delete p_pServiceQuery;
            bResult = true;
        }
        else    // No predecessor
        {
            if (m_pServiceQueries == p_pServiceQuery)
            {
                m_pServiceQueries = p_pServiceQuery->m_pNext;
                delete p_pServiceQuery;
                bResult = true;
            }
            else
            {
                DEBUG_EX_ERR(DEBUG_OUTPUT.println("[MDNSResponder] _releaseServiceQuery: INVALID service query!"););
            }
        }
    }
    return bResult;
}

/*
    MDNSResponder::_removeLegacyServiceQuery
*/
bool MDNSResponder::_removeLegacyServiceQuery(void)
{

    stcMDNSServiceQuery*    pLegacyServiceQuery = _findLegacyServiceQuery();
    return (pLegacyServiceQuery ? _removeServiceQuery(pLegacyServiceQuery) : true);
}

/*
    MDNSResponder::_findServiceQuery

    'Convert' hMDNSServiceQuery to stcMDNSServiceQuery* (ensure existence)

*/
MDNSResponder::stcMDNSServiceQuery* MDNSResponder::_findServiceQuery(MDNSResponder::hMDNSServiceQuery p_hServiceQuery)
{

    stcMDNSServiceQuery*    pServiceQuery = m_pServiceQueries;
    while (pServiceQuery)
    {
        if ((hMDNSServiceQuery)pServiceQuery == p_hServiceQuery)
        {
            break;
        }
        pServiceQuery = pServiceQuery->m_pNext;
    }
    return pServiceQuery;
}

/*
    MDNSResponder::_findLegacyServiceQuery
*/
MDNSResponder::stcMDNSServiceQuery* MDNSResponder::_findLegacyServiceQuery(void)
{

    stcMDNSServiceQuery*    pServiceQuery = m_pServiceQueries;
    while (pServiceQuery)
    {
        if (pServiceQuery->m_bLegacyQuery)
        {
            break;
        }
        pServiceQuery = pServiceQuery->m_pNext;
    }
    return pServiceQuery;
}

/*
    MDNSResponder::_releaseServiceQueries
*/
bool MDNSResponder::_releaseServiceQueries(void)
{
    while (m_pServiceQueries)
    {
        stcMDNSServiceQuery*    pNext = m_pServiceQueries->m_pNext;
        delete m_pServiceQueries;
        m_pServiceQueries = pNext;
    }
    return true;
}

/*
    MDNSResponder::_findNextServiceQueryByServiceType
*/
MDNSResponder::stcMDNSServiceQuery* MDNSResponder::_findNextServiceQueryByServiceType(const stcMDNS_RRDomain& p_ServiceTypeDomain,
        const stcMDNSServiceQuery* p_pPrevServiceQuery)
{
    stcMDNSServiceQuery*    pMatchingServiceQuery = 0;

    stcMDNSServiceQuery*    pServiceQuery = (p_pPrevServiceQuery ? p_pPrevServiceQuery->m_pNext : m_pServiceQueries);
    while (pServiceQuery)
    {
        if (p_ServiceTypeDomain == pServiceQuery->m_ServiceTypeDomain)
        {
            pMatchingServiceQuery = pServiceQuery;
            break;
        }
        pServiceQuery = pServiceQuery->m_pNext;
    }
    return pMatchingServiceQuery;
}


/*
    HOSTNAME
*/

/*
    MDNSResponder::_setHostname
*/
bool MDNSResponder::_setHostname(const char* p_pcHostname)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _allocHostname (%s)\n"), p_pcHostname););

    bool    bResult = false;

    _releaseHostname();

    size_t  stLength = 0;
    if ((p_pcHostname) &&
            (MDNS_DOMAIN_LABEL_MAXLENGTH >= (stLength = strlen(p_pcHostname))))   // char max size for a single label
    {
        // Copy in hostname characters as lowercase
        if ((bResult = (0 != (m_pcHostname = new char[stLength + 1]))))
        {
#ifdef MDNS_FORCE_LOWERCASE_HOSTNAME
            size_t i = 0;
            for (; i < stLength; ++i)
            {
                m_pcHostname[i] = (isupper(p_pcHostname[i]) ? tolower(p_pcHostname[i]) : p_pcHostname[i]);
            }
            m_pcHostname[i] = 0;
#else
            strncpy(m_pcHostname, p_pcHostname, (stLength + 1));
#endif
        }
    }
    return bResult;
}

/*
    MDNSResponder::_releaseHostname
*/
bool MDNSResponder::_releaseHostname(void)
{

    if (m_pcHostname)
    {
        delete[] m_pcHostname;
        m_pcHostname = 0;
    }
    return true;
}


/*
    SERVICE
*/

/*
    MDNSResponder::_allocService
*/
MDNSResponder::stcMDNSService* MDNSResponder::_allocService(const char* p_pcName,
        const char* p_pcService,
        const char* p_pcProtocol,
        uint16_t p_u16Port)
{

    stcMDNSService* pService = 0;
    if (((!p_pcName) ||
            (MDNS_DOMAIN_LABEL_MAXLENGTH >= strlen(p_pcName))) &&
            (p_pcService) &&
            (MDNS_SERVICE_NAME_LENGTH >= strlen(p_pcService)) &&
            (p_pcProtocol) &&
            (MDNS_SERVICE_PROTOCOL_LENGTH >= strlen(p_pcProtocol)) &&
            (p_u16Port) &&
            (0 != (pService = new stcMDNSService)) &&
            (pService->setName(p_pcName ? : m_pcHostname)) &&
            (pService->setService(p_pcService)) &&
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
    MDNSResponder::_releaseService
*/
bool MDNSResponder::_releaseService(MDNSResponder::stcMDNSService* p_pService)
{

    bool    bResult = false;

    if (p_pService)
    {
        stcMDNSService* pPred = m_pServices;
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
        else    // No predecessor
        {
            if (m_pServices == p_pService)
            {
                m_pServices = p_pService->m_pNext;
                delete p_pService;
                bResult = true;
            }
            else
            {
                DEBUG_EX_ERR(DEBUG_OUTPUT.println("[MDNSResponder] _releaseService: INVALID service!"););
            }
        }
    }
    return bResult;
}

/*
    MDNSResponder::_releaseServices
*/
bool MDNSResponder::_releaseServices(void)
{

    stcMDNSService* pService = m_pServices;
    while (pService)
    {
        _releaseService(pService);
        pService = m_pServices;
    }
    return true;
}

/*
    MDNSResponder::_findService
*/
MDNSResponder::stcMDNSService* MDNSResponder::_findService(const char* p_pcName,
        const char* p_pcService,
        const char* p_pcProtocol)
{

    stcMDNSService* pService = m_pServices;
    while (pService)
    {
        if ((0 == strcmp(pService->m_pcName, p_pcName)) &&
                (0 == strcmp(pService->m_pcService, p_pcService)) &&
                (0 == strcmp(pService->m_pcProtocol, p_pcProtocol)))
        {

            break;
        }
        pService = pService->m_pNext;
    }
    return pService;
}

/*
    MDNSResponder::_findService
*/
MDNSResponder::stcMDNSService* MDNSResponder::_findService(const MDNSResponder::hMDNSService p_hService)
{

    stcMDNSService* pService = m_pServices;
    while (pService)
    {
        if (p_hService == (hMDNSService)pService)
        {
            break;
        }
        pService = pService->m_pNext;
    }
    return pService;
}


/*
    SERVICE TXT
*/

/*
    MDNSResponder::_allocServiceTxt
*/
MDNSResponder::stcMDNSServiceTxt* MDNSResponder::_allocServiceTxt(MDNSResponder::stcMDNSService* p_pService,
        const char* p_pcKey,
        const char* p_pcValue,
        bool p_bTemp)
{

    stcMDNSServiceTxt*  pTxt = 0;

    if ((p_pService) &&
            (p_pcKey) &&
            (MDNS_SERVICE_TXT_MAXLENGTH > (p_pService->m_Txts.length() +
                                           1 +                                 // Length byte
                                           (p_pcKey ? strlen(p_pcKey) : 0) +
                                           1 +                                 // '='
                                           (p_pcValue ? strlen(p_pcValue) : 0))))
    {

        pTxt = new stcMDNSServiceTxt;
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
    MDNSResponder::_releaseServiceTxt
*/
bool MDNSResponder::_releaseServiceTxt(MDNSResponder::stcMDNSService* p_pService,
                                       MDNSResponder::stcMDNSServiceTxt* p_pTxt)
{

    return ((p_pService) &&
            (p_pTxt) &&
            (p_pService->m_Txts.remove(p_pTxt)));
}

/*
    MDNSResponder::_updateServiceTxt
*/
MDNSResponder::stcMDNSServiceTxt* MDNSResponder::_updateServiceTxt(MDNSResponder::stcMDNSService* p_pService,
        MDNSResponder::stcMDNSServiceTxt* p_pTxt,
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
    MDNSResponder::_findServiceTxt
*/
MDNSResponder::stcMDNSServiceTxt* MDNSResponder::_findServiceTxt(MDNSResponder::stcMDNSService* p_pService,
        const char* p_pcKey)
{

    return (p_pService ? p_pService->m_Txts.find(p_pcKey) : 0);
}

/*
    MDNSResponder::_findServiceTxt
*/
MDNSResponder::stcMDNSServiceTxt* MDNSResponder::_findServiceTxt(MDNSResponder::stcMDNSService* p_pService,
        const hMDNSTxt p_hTxt)
{

    return (((p_pService) && (p_hTxt)) ? p_pService->m_Txts.find((stcMDNSServiceTxt*)p_hTxt) : 0);
}

/*
    MDNSResponder::_addServiceTxt
*/
MDNSResponder::stcMDNSServiceTxt* MDNSResponder::_addServiceTxt(MDNSResponder::stcMDNSService* p_pService,
        const char* p_pcKey,
        const char* p_pcValue,
        bool p_bTemp)
{
    stcMDNSServiceTxt*  pResult = 0;

    if ((p_pService) &&
            (p_pcKey) &&
            (strlen(p_pcKey)))
    {

        stcMDNSServiceTxt*  pTxt = p_pService->m_Txts.find(p_pcKey);
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

MDNSResponder::stcMDNSServiceTxt* MDNSResponder::_answerKeyValue(const hMDNSServiceQuery p_hServiceQuery,
        const uint32_t p_u32AnswerIndex)
{
    stcMDNSServiceQuery*            pServiceQuery = _findServiceQuery(p_hServiceQuery);
    stcMDNSServiceQuery::stcAnswer* pSQAnswer = (pServiceQuery ? pServiceQuery->answerAtIndex(p_u32AnswerIndex) : 0);
    // Fill m_pcTxts (if not already done)
    return (pSQAnswer) ?  pSQAnswer->m_Txts.m_pTxts : 0;
}

/*
    MDNSResponder::_collectServiceTxts
*/
bool MDNSResponder::_collectServiceTxts(MDNSResponder::stcMDNSService& p_rService)
{

    // Call Dynamic service callbacks
    if (m_fnServiceTxtCallback)
    {
        m_fnServiceTxtCallback((hMDNSService)&p_rService);
    }
    if (p_rService.m_fnTxtCallback)
    {
        p_rService.m_fnTxtCallback((hMDNSService)&p_rService);
    }
    return true;
}

/*
    MDNSResponder::_releaseTempServiceTxts
*/
bool MDNSResponder::_releaseTempServiceTxts(MDNSResponder::stcMDNSService& p_rService)
{

    return (p_rService.m_Txts.removeTempTxts());
}


/*
    MISC
*/

#ifdef DEBUG_ESP_MDNS_RESPONDER
/*
    MDNSResponder::_printRRDomain
*/
bool MDNSResponder::_printRRDomain(const MDNSResponder::stcMDNS_RRDomain& p_RRDomain) const
{

    //DEBUG_OUTPUT.printf_P(PSTR("Domain: "));

    const char* pCursor = p_RRDomain.m_acName;
    uint8_t     u8Length = *pCursor++;
    if (u8Length)
    {
        while (u8Length)
        {
            for (uint8_t u = 0; u < u8Length; ++u)
            {
                DEBUG_OUTPUT.printf_P(PSTR("%c"), *(pCursor++));
            }
            u8Length = *pCursor++;
            if (u8Length)
            {
                DEBUG_OUTPUT.printf_P(PSTR("."));
            }
        }
    }
    else    // empty domain
    {
        DEBUG_OUTPUT.printf_P(PSTR("-empty-"));
    }
    //DEBUG_OUTPUT.printf_P(PSTR("\n"));

    return true;
}

/*
    MDNSResponder::_printRRAnswer
*/
bool MDNSResponder::_printRRAnswer(const MDNSResponder::stcMDNS_RRAnswer& p_RRAnswer) const
{

    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] RRAnswer: "));
    _printRRDomain(p_RRAnswer.m_Header.m_Domain);
    DEBUG_OUTPUT.printf_P(PSTR(" Type:0x%04X Class:0x%04X TTL:%u, "), p_RRAnswer.m_Header.m_Attributes.m_u16Type, p_RRAnswer.m_Header.m_Attributes.m_u16Class, p_RRAnswer.m_u32TTL);
    switch (p_RRAnswer.m_Header.m_Attributes.m_u16Type & (~0x8000))     // Topmost bit might carry 'cache flush' flag
    {
#ifdef MDNS_IP4_SUPPORT
    case DNS_RRTYPE_A:
        DEBUG_OUTPUT.printf_P(PSTR("A IP:%s"), ((const stcMDNS_RRAnswerA*)&p_RRAnswer)->m_IPAddress.toString().c_str());
        break;
#endif
    case DNS_RRTYPE_PTR:
        DEBUG_OUTPUT.printf_P(PSTR("PTR "));
        _printRRDomain(((const stcMDNS_RRAnswerPTR*)&p_RRAnswer)->m_PTRDomain);
        break;
    case DNS_RRTYPE_TXT:
    {
        size_t  stTxtLength = ((const stcMDNS_RRAnswerTXT*)&p_RRAnswer)->m_Txts.c_strLength();
        char*   pTxts = new char[stTxtLength];
        if (pTxts)
        {
            ((/*const c_str()!!*/stcMDNS_RRAnswerTXT*)&p_RRAnswer)->m_Txts.c_str(pTxts);
            DEBUG_OUTPUT.printf_P(PSTR("TXT(%zu) %s"), stTxtLength, pTxts);
            delete[] pTxts;
        }
        break;
    }
#ifdef MDNS_IP6_SUPPORT
    case DNS_RRTYPE_AAAA:
        DEBUG_OUTPUT.printf_P(PSTR("AAAA IP:%s"), ((stcMDNS_RRAnswerA*&)p_rpRRAnswer)->m_IPAddress.toString().c_str());
        break;
#endif
    case DNS_RRTYPE_SRV:
        DEBUG_OUTPUT.printf_P(PSTR("SRV Port:%u "), ((const stcMDNS_RRAnswerSRV*)&p_RRAnswer)->m_u16Port);
        _printRRDomain(((const stcMDNS_RRAnswerSRV*)&p_RRAnswer)->m_SRVDomain);
        break;
    default:
        DEBUG_OUTPUT.printf_P(PSTR("generic "));
        break;
    }
    DEBUG_OUTPUT.printf_P(PSTR("\n"));

    return true;
}
#endif

}   // namespace MDNSImplementation

} // namespace esp8266




