/*
    LEAmDNS.cpp

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
#include <AddrList.h>

#include "ESP8266mDNS.h"
#include "LEAmDNS_Priv.h"
#include <LwipIntf.h>
#include <lwip/igmp.h>
#include <lwip/prot/dns.h>

// should be defined at build time
#ifndef ARDUINO_BOARD_ID
#define ARDUINO_BOARD_ID "generic"
#endif

namespace esp8266
{

/*
    LEAmDNS
*/
namespace MDNSImplementation
{

    /**
        INTERFACE
    */

    /**
        MDNSResponder::MDNSResponder
    */
    MDNSResponder::MDNSResponder(void) :
        m_pServices(0), m_pUDPContext(0), m_pcHostname(0), m_pServiceQueries(0),
        m_fnServiceTxtCallback(0), m_bLwipCb(false), m_bRestarting(false)
    {
    }

    /*
        MDNSResponder::~MDNSResponder
    */
    MDNSResponder::~MDNSResponder(void)
    {
        _resetProbeStatus(false);
        _releaseServiceQueries();
        _releaseHostname();
        _releaseUDPContext();
        _releaseServices();
    }

    /*
        MDNSResponder::begin

        Set the host domain (for probing) and install WiFi event handlers for
        IP assignment and disconnection management. In both cases, the MDNS responder
        is restarted (reset and restart probe status)
        Finally the responder is (re)started

    */
    bool MDNSResponder::begin(const char* p_pcHostname, const IPAddress& /*p_IPAddress*/,
                              uint32_t /*p_u32TTL*/)
    {
        bool bResult = false;

        if (_setHostname(p_pcHostname))
        {
            bResult = _restart();
        }

        if (bResult && !m_bLwipCb)
        {
            bool bCallback = LwipIntf::statusChangeCB(
                [this](netif* pNetIf)
                {
                    if (m_bRestarting)
                    {
                        return;
                    }

                    m_bRestarting = true;
                    schedule_function(
                        [this]()
                        {
                            DEBUG_EX_INFO(
                                DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] begin: restarting "
                                                           "after interface status changed\n")););
                            _restart();
                            m_bRestarting = false;
                        });
                });
            DEBUG_EX_ERR(if (!bCallback) {
                DEBUG_OUTPUT.printf_P(
                    PSTR("[MDNSResponder] begin: FAILED LwipIntf::statusChangeCB!\n"));
            });
            m_bLwipCb = bCallback;
        }

        DEBUG_EX_ERR(if (!bResult) {
            DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] begin: FAILED for '%s'!\n"),
                                  (p_pcHostname ?: "-"));
        });

        return bResult;
    }

    /*
        MDNSResponder::close

        Ends the MDNS responder.
        Announced services are unannounced (by multicasting a goodbye message)

    */
    bool MDNSResponder::close(void)
    {
        bool bResult = false;

        if (0 != m_pUDPContext)
        {
            _announce(false, true);
            _resetProbeStatus(false);  // Stop probing
            _releaseServiceQueries();
            _releaseServices();
            _releaseUDPContext();
            _releaseHostname();

            bResult = true;
        }
        else
        {
            DEBUG_EX_INFO(
                DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] close: Ignoring call to close!\n")););
        }
        return bResult;
    }

    /*
        MDNSResponder::end

        Ends the MDNS responder.
        for compatibility with esp32

    */

    bool MDNSResponder::end(void)
    {
        return close();
    }

    /*
        MDNSResponder::setHostname

        Replaces the current hostname and restarts probing.
        For services without own instance name (when the host name was used a instance
        name), the instance names are replaced also (and the probing is restarted).

    */
    bool MDNSResponder::setHostname(const char* p_pcHostname)
    {
        bool bResult = false;

        if (_setHostname(p_pcHostname))
        {
            m_HostProbeInformation.m_ProbingStatus = ProbingStatus_ReadyToStart;

            // Replace 'auto-set' service names
            bResult = true;
            for (stcMDNSService* pService = m_pServices; ((bResult) && (pService));
                 pService                 = pService->m_pNext)
            {
                if (pService->m_bAutoName)
                {
                    bResult                                      = pService->setName(p_pcHostname);
                    pService->m_ProbeInformation.m_ProbingStatus = ProbingStatus_ReadyToStart;
                }
            }
        }
        DEBUG_EX_ERR(if (!bResult) {
            DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] setHostname: FAILED for '%s'!\n"),
                                  (p_pcHostname ?: "-"));
        });
        return bResult;
    }

    /*
        MDNSResponder::setHostname (LEGACY)
    */
    bool MDNSResponder::setHostname(const String& p_strHostname)
    {
        return setHostname(p_strHostname.c_str());
    }

    /*
        SERVICES
    */

    /*
        MDNSResponder::addService

        Add service; using hostname if no name is explicitly provided for the service
        The usual '_' underline, which is prepended to service and protocol, eg. _http,
        may be given. If not, it is added automatically.

    */
    MDNSResponder::hMDNSService MDNSResponder::addService(const char* p_pcName,
                                                          const char* p_pcService,
                                                          const char* p_pcProtocol,
                                                          uint16_t    p_u16Port)
    {
        hMDNSService hResult = 0;

        if (((!p_pcName) ||  // NO name OR
             (MDNS_DOMAIN_LABEL_MAXLENGTH >= os_strlen(p_pcName)))
            &&  // Fitting name
            (p_pcService) && (MDNS_SERVICE_NAME_LENGTH >= os_strlen(p_pcService)) && (p_pcProtocol)
            && ((MDNS_SERVICE_PROTOCOL_LENGTH - 1) != os_strlen(p_pcProtocol)) && (p_u16Port))
        {
            if (!_findService((p_pcName ?: m_pcHostname), p_pcService,
                              p_pcProtocol))  // Not already used
            {
                if (0
                    != (hResult = (hMDNSService)_allocService(p_pcName, p_pcService, p_pcProtocol,
                                                              p_u16Port)))
                {
                    // Start probing
                    ((stcMDNSService*)hResult)->m_ProbeInformation.m_ProbingStatus
                        = ProbingStatus_ReadyToStart;
                }
            }
        }  // else: bad arguments
        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(
            PSTR("[MDNSResponder] addService: %s to add '%s.%s.%s'!\n"),
            (hResult ? "Succeeded" : "FAILED"), (p_pcName ?: "-"), p_pcService, p_pcProtocol););
        DEBUG_EX_ERR(if (!hResult) {
            DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] addService: FAILED to add '%s.%s.%s'!\n"),
                                  (p_pcName ?: "-"), p_pcService, p_pcProtocol);
        });
        return hResult;
    }

    /*
        MDNSResponder::removeService

        Unanounce a service (by sending a goodbye message) and remove it
        from the MDNS responder

    */
    bool MDNSResponder::removeService(const MDNSResponder::hMDNSService p_hService)
    {
        stcMDNSService* pService = 0;
        bool            bResult  = (((pService = _findService(p_hService)))
                        && (_announceService(*pService, false)) && (_releaseService(pService)));
        DEBUG_EX_ERR(if (!bResult) {
            DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] removeService: FAILED!\n"));
        });
        return bResult;
    }

    /*
        MDNSResponder::removeService
    */
    bool MDNSResponder::removeService(const char* p_pcName, const char* p_pcService,
                                      const char* p_pcProtocol)
    {
        return removeService(
            (hMDNSService)_findService((p_pcName ?: m_pcHostname), p_pcService, p_pcProtocol));
    }

    /*
        MDNSResponder::addService (LEGACY)
    */
    bool MDNSResponder::addService(const String& p_strService, const String& p_strProtocol,
                                   uint16_t p_u16Port)
    {
        return (
            0 != addService(m_pcHostname, p_strService.c_str(), p_strProtocol.c_str(), p_u16Port));
    }

    /*
        MDNSResponder::setServiceName
    */
    bool MDNSResponder::setServiceName(const MDNSResponder::hMDNSService p_hService,
                                       const char*                       p_pcInstanceName)
    {
        stcMDNSService* pService = 0;
        bool            bResult
            = (((!p_pcInstanceName) || (MDNS_DOMAIN_LABEL_MAXLENGTH >= os_strlen(p_pcInstanceName)))
               && ((pService = _findService(p_hService))) && (pService->setName(p_pcInstanceName))
               && ((pService->m_ProbeInformation.m_ProbingStatus = ProbingStatus_ReadyToStart)));
        DEBUG_EX_ERR(if (!bResult) {
            DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] setServiceName: FAILED for '%s'!\n"),
                                  (p_pcInstanceName ?: "-"));
        });
        return bResult;
    }

    /*
        SERVICE TXT
    */

    /*
        MDNSResponder::addServiceTxt

        Add a static service TXT item ('Key'='Value') to a service.

    */
    MDNSResponder::hMDNSTxt
    MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSService p_hService, const char* p_pcKey,
                                 const char* p_pcValue)
    {
        hMDNSTxt        hTxt     = 0;
        stcMDNSService* pService = _findService(p_hService);
        if (pService)
        {
            hTxt = (hMDNSTxt)_addServiceTxt(pService, p_pcKey, p_pcValue, false);
        }
        DEBUG_EX_ERR(if (!hTxt) {
            DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] addServiceTxt: FAILED for '%s=%s'!\n"),
                                  (p_pcKey ?: "-"), (p_pcValue ?: "-"));
        });
        return hTxt;
    }

    /*
        MDNSResponder::addServiceTxt (uint32_t)

        Formats: http://www.cplusplus.com/reference/cstdio/printf/
    */
    MDNSResponder::hMDNSTxt
    MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSService p_hService, const char* p_pcKey,
                                 uint32_t p_u32Value)
    {
        char acBuffer[32];
        *acBuffer = 0;
        sprintf(acBuffer, "%u", p_u32Value);

        return addServiceTxt(p_hService, p_pcKey, acBuffer);
    }

    /*
        MDNSResponder::addServiceTxt (uint16_t)
    */
    MDNSResponder::hMDNSTxt
    MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSService p_hService, const char* p_pcKey,
                                 uint16_t p_u16Value)
    {
        char acBuffer[16];
        *acBuffer = 0;
        sprintf(acBuffer, "%hu", p_u16Value);

        return addServiceTxt(p_hService, p_pcKey, acBuffer);
    }

    /*
        MDNSResponder::addServiceTxt (uint8_t)
    */
    MDNSResponder::hMDNSTxt
    MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSService p_hService, const char* p_pcKey,
                                 uint8_t p_u8Value)
    {
        char acBuffer[8];
        *acBuffer = 0;
        sprintf(acBuffer, "%hhu", p_u8Value);

        return addServiceTxt(p_hService, p_pcKey, acBuffer);
    }

    /*
        MDNSResponder::addServiceTxt (int32_t)
    */
    MDNSResponder::hMDNSTxt
    MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSService p_hService, const char* p_pcKey,
                                 int32_t p_i32Value)
    {
        char acBuffer[32];
        *acBuffer = 0;
        sprintf(acBuffer, "%i", p_i32Value);

        return addServiceTxt(p_hService, p_pcKey, acBuffer);
    }

    /*
        MDNSResponder::addServiceTxt (int16_t)
    */
    MDNSResponder::hMDNSTxt
    MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSService p_hService, const char* p_pcKey,
                                 int16_t p_i16Value)
    {
        char acBuffer[16];
        *acBuffer = 0;
        sprintf(acBuffer, "%hi", p_i16Value);

        return addServiceTxt(p_hService, p_pcKey, acBuffer);
    }

    /*
        MDNSResponder::addServiceTxt (int8_t)
    */
    MDNSResponder::hMDNSTxt
    MDNSResponder::addServiceTxt(const MDNSResponder::hMDNSService p_hService, const char* p_pcKey,
                                 int8_t p_i8Value)
    {
        char acBuffer[8];
        *acBuffer = 0;
        sprintf(acBuffer, "%hhi", p_i8Value);

        return addServiceTxt(p_hService, p_pcKey, acBuffer);
    }

    /*
        MDNSResponder::removeServiceTxt

        Remove a static service TXT item from a service.
    */
    bool MDNSResponder::removeServiceTxt(const MDNSResponder::hMDNSService p_hService,
                                         const MDNSResponder::hMDNSTxt     p_hTxt)
    {
        bool bResult = false;

        stcMDNSService* pService = _findService(p_hService);
        if (pService)
        {
            stcMDNSServiceTxt* pTxt = _findServiceTxt(pService, p_hTxt);
            if (pTxt)
            {
                bResult = _releaseServiceTxt(pService, pTxt);
            }
        }
        DEBUG_EX_ERR(if (!bResult) {
            DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] removeServiceTxt: FAILED!\n"));
        });
        return bResult;
    }

    /*
        MDNSResponder::removeServiceTxt
    */
    bool MDNSResponder::removeServiceTxt(const MDNSResponder::hMDNSService p_hService,
                                         const char*                       p_pcKey)
    {
        bool bResult = false;

        stcMDNSService* pService = _findService(p_hService);
        if (pService)
        {
            stcMDNSServiceTxt* pTxt = _findServiceTxt(pService, p_pcKey);
            if (pTxt)
            {
                bResult = _releaseServiceTxt(pService, pTxt);
            }
        }
        DEBUG_EX_ERR(if (!bResult) {
            DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] removeServiceTxt: FAILED for '%s'!\n"),
                                  (p_pcKey ?: "-"));
        });
        return bResult;
    }

    /*
        MDNSResponder::removeServiceTxt
    */
    bool MDNSResponder::removeServiceTxt(const char* p_pcName, const char* p_pcService,
                                         const char* p_pcProtocol, const char* p_pcKey)
    {
        bool bResult = false;

        stcMDNSService* pService
            = _findService((p_pcName ?: m_pcHostname), p_pcService, p_pcProtocol);
        if (pService)
        {
            stcMDNSServiceTxt* pTxt = _findServiceTxt(pService, p_pcKey);
            if (pTxt)
            {
                bResult = _releaseServiceTxt(pService, pTxt);
            }
        }
        return bResult;
    }

    /*
        MDNSResponder::addServiceTxt (LEGACY)
    */
    bool MDNSResponder::addServiceTxt(const char* p_pcService, const char* p_pcProtocol,
                                      const char* p_pcKey, const char* p_pcValue)
    {
        return (0
                != _addServiceTxt(_findService(m_pcHostname, p_pcService, p_pcProtocol), p_pcKey,
                                  p_pcValue, false));
    }

    /*
        MDNSResponder::addServiceTxt (LEGACY)
    */
    bool MDNSResponder::addServiceTxt(const String& p_strService, const String& p_strProtocol,
                                      const String& p_strKey, const String& p_strValue)
    {
        return (0
                != _addServiceTxt(
                    _findService(m_pcHostname, p_strService.c_str(), p_strProtocol.c_str()),
                    p_strKey.c_str(), p_strValue.c_str(), false));
    }

    /*
        MDNSResponder::setDynamicServiceTxtCallback (global)

        Set a global callback for dynamic service TXT items. The callback is called, whenever
        service TXT items are needed.

    */
    bool MDNSResponder::setDynamicServiceTxtCallback(
        MDNSResponder::MDNSDynamicServiceTxtCallbackFunc p_fnCallback)
    {
        m_fnServiceTxtCallback = p_fnCallback;

        return true;
    }

    /*
        MDNSResponder::setDynamicServiceTxtCallback (service specific)

        Set a service specific callback for dynamic service TXT items. The callback is called,
       whenever service TXT items are needed for the given service.

    */
    bool MDNSResponder::setDynamicServiceTxtCallback(
        MDNSResponder::hMDNSService                      p_hService,
        MDNSResponder::MDNSDynamicServiceTxtCallbackFunc p_fnCallback)
    {
        bool bResult = false;

        stcMDNSService* pService = _findService(p_hService);
        if (pService)
        {
            pService->m_fnTxtCallback = p_fnCallback;

            bResult = true;
        }
        DEBUG_EX_ERR(if (!bResult) {
            DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] setDynamicServiceTxtCallback: FAILED!\n"));
        });
        return bResult;
    }

    /*
        MDNSResponder::addDynamicServiceTxt
    */
    MDNSResponder::hMDNSTxt
    MDNSResponder::addDynamicServiceTxt(MDNSResponder::hMDNSService p_hService, const char* p_pcKey,
                                        const char* p_pcValue)
    {
        // DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] addDynamicServiceTxt
        // (%s=%s)\n"), p_pcKey, p_pcValue););

        hMDNSTxt hTxt = 0;

        stcMDNSService* pService = _findService(p_hService);
        if (pService)
        {
            hTxt = _addServiceTxt(pService, p_pcKey, p_pcValue, true);
        }
        DEBUG_EX_ERR(if (!hTxt) {
            DEBUG_OUTPUT.printf_P(
                PSTR("[MDNSResponder] addDynamicServiceTxt: FAILED for '%s=%s'!\n"),
                (p_pcKey ?: "-"), (p_pcValue ?: "-"));
        });
        return hTxt;
    }

    /*
        MDNSResponder::addDynamicServiceTxt (uint32_t)
    */
    MDNSResponder::hMDNSTxt
    MDNSResponder::addDynamicServiceTxt(MDNSResponder::hMDNSService p_hService, const char* p_pcKey,
                                        uint32_t p_u32Value)
    {
        char acBuffer[32];
        *acBuffer = 0;
        sprintf(acBuffer, "%u", p_u32Value);

        return addDynamicServiceTxt(p_hService, p_pcKey, acBuffer);
    }

    /*
        MDNSResponder::addDynamicServiceTxt (uint16_t)
    */
    MDNSResponder::hMDNSTxt
    MDNSResponder::addDynamicServiceTxt(MDNSResponder::hMDNSService p_hService, const char* p_pcKey,
                                        uint16_t p_u16Value)
    {
        char acBuffer[16];
        *acBuffer = 0;
        sprintf(acBuffer, "%hu", p_u16Value);

        return addDynamicServiceTxt(p_hService, p_pcKey, acBuffer);
    }

    /*
        MDNSResponder::addDynamicServiceTxt (uint8_t)
    */
    MDNSResponder::hMDNSTxt
    MDNSResponder::addDynamicServiceTxt(MDNSResponder::hMDNSService p_hService, const char* p_pcKey,
                                        uint8_t p_u8Value)
    {
        char acBuffer[8];
        *acBuffer = 0;
        sprintf(acBuffer, "%hhu", p_u8Value);

        return addDynamicServiceTxt(p_hService, p_pcKey, acBuffer);
    }

    /*
        MDNSResponder::addDynamicServiceTxt (int32_t)
    */
    MDNSResponder::hMDNSTxt
    MDNSResponder::addDynamicServiceTxt(MDNSResponder::hMDNSService p_hService, const char* p_pcKey,
                                        int32_t p_i32Value)
    {
        char acBuffer[32];
        *acBuffer = 0;
        sprintf(acBuffer, "%i", p_i32Value);

        return addDynamicServiceTxt(p_hService, p_pcKey, acBuffer);
    }

    /*
        MDNSResponder::addDynamicServiceTxt (int16_t)
    */
    MDNSResponder::hMDNSTxt
    MDNSResponder::addDynamicServiceTxt(MDNSResponder::hMDNSService p_hService, const char* p_pcKey,
                                        int16_t p_i16Value)
    {
        char acBuffer[16];
        *acBuffer = 0;
        sprintf(acBuffer, "%hi", p_i16Value);

        return addDynamicServiceTxt(p_hService, p_pcKey, acBuffer);
    }

    /*
        MDNSResponder::addDynamicServiceTxt (int8_t)
    */
    MDNSResponder::hMDNSTxt
    MDNSResponder::addDynamicServiceTxt(MDNSResponder::hMDNSService p_hService, const char* p_pcKey,
                                        int8_t p_i8Value)
    {
        char acBuffer[8];
        *acBuffer = 0;
        sprintf(acBuffer, "%hhi", p_i8Value);

        return addDynamicServiceTxt(p_hService, p_pcKey, acBuffer);
    }

    /**
        STATIC SERVICE QUERY (LEGACY)
    */

    /*
        MDNSResponder::queryService

        Perform a (blocking) static service query.
        The arrived answers can be queried by calling:
        - answerHostname (or 'hostname')
        - answerIP (or 'IP')
        - answerPort (or 'port')

    */
    uint32_t
    MDNSResponder::queryService(const char* p_pcService, const char* p_pcProtocol,
                                const uint16_t p_u16Timeout /*= MDNS_QUERYSERVICES_WAIT_TIME*/)
    {
        if (0 == m_pUDPContext)
        {
            // safeguard against misuse
            return 0;
        }

        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] queryService '%s.%s'\n"),
                                            p_pcService, p_pcProtocol););

        uint32_t u32Result = 0;

        stcMDNSServiceQuery* pServiceQuery = 0;
        if ((p_pcService) && (os_strlen(p_pcService)) && (p_pcProtocol) && (os_strlen(p_pcProtocol))
            && (p_u16Timeout) && (_removeLegacyServiceQuery())
            && ((pServiceQuery = _allocServiceQuery()))
            && (_buildDomainForService(p_pcService, p_pcProtocol,
                                       pServiceQuery->m_ServiceTypeDomain)))
        {
            pServiceQuery->m_bLegacyQuery = true;

            if (_sendMDNSServiceQuery(*pServiceQuery))
            {
                // Wait for answers to arrive
                DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(
                    PSTR("[MDNSResponder] queryService: Waiting %u ms for answers...\n"),
                    p_u16Timeout););
                delay(p_u16Timeout);

                // All answers should have arrived by now -> stop adding new answers
                pServiceQuery->m_bAwaitingAnswers = false;
                u32Result                         = pServiceQuery->answerCount();
            }
            else  // FAILED to send query
            {
                _removeServiceQuery(pServiceQuery);
            }
        }
        else
        {
            DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(
                PSTR("[MDNSResponder] queryService: INVALID input data!\n")););
        }
        return u32Result;
    }

    /*
        MDNSResponder::removeQuery

        Remove the last static service query (and all answers).

    */
    bool MDNSResponder::removeQuery(void)
    {
        return _removeLegacyServiceQuery();
    }

    /*
        MDNSResponder::queryService (LEGACY)
    */
    uint32_t MDNSResponder::queryService(const String& p_strService, const String& p_strProtocol)
    {
        return queryService(p_strService.c_str(), p_strProtocol.c_str());
    }

    /*
        MDNSResponder::answerHostname
    */
    const char* MDNSResponder::answerHostname(const uint32_t p_u32AnswerIndex)
    {
        stcMDNSServiceQuery*            pServiceQuery = _findLegacyServiceQuery();
        stcMDNSServiceQuery::stcAnswer* pSQAnswer
            = (pServiceQuery ? pServiceQuery->answerAtIndex(p_u32AnswerIndex) : 0);

        if ((pSQAnswer) && (pSQAnswer->m_HostDomain.m_u16NameLength)
            && (!pSQAnswer->m_pcHostDomain))
        {
            char* pcHostDomain = pSQAnswer->allocHostDomain(pSQAnswer->m_HostDomain.c_strLength());
            if (pcHostDomain)
            {
                pSQAnswer->m_HostDomain.c_str(pcHostDomain);
            }
        }
        return (pSQAnswer ? pSQAnswer->m_pcHostDomain : 0);
    }

#ifdef MDNS_IP4_SUPPORT
    /*
        MDNSResponder::answerIP
    */
    IPAddress MDNSResponder::answerIP(const uint32_t p_u32AnswerIndex)
    {
        const stcMDNSServiceQuery*            pServiceQuery = _findLegacyServiceQuery();
        const stcMDNSServiceQuery::stcAnswer* pSQAnswer
            = (pServiceQuery ? pServiceQuery->answerAtIndex(p_u32AnswerIndex) : 0);
        const stcMDNSServiceQuery::stcAnswer::stcIP4Address* pIP4Address
            = (((pSQAnswer) && (pSQAnswer->m_pIP4Addresses)) ? pSQAnswer->IP4AddressAtIndex(0) : 0);
        return (pIP4Address ? pIP4Address->m_IPAddress : IPAddress());
    }
#endif

#ifdef MDNS_IP6_SUPPORT
    /*
        MDNSResponder::answerIP6
    */
    IPAddress MDNSResponder::answerIP6(const uint32_t p_u32AnswerIndex)
    {
        const stcMDNSServiceQuery*            pServiceQuery = _findLegacyServiceQuery();
        const stcMDNSServiceQuery::stcAnswer* pSQAnswer
            = (pServiceQuery ? pServiceQuery->answerAtIndex(p_u32AnswerIndex) : 0);
        const stcMDNSServiceQuery::stcAnswer::stcIP6Address* pIP6Address
            = (((pSQAnswer) && (pSQAnswer->m_pIP6Addresses)) ? pSQAnswer->IP6AddressAtIndex(0) : 0);
        return (pIP6Address ? pIP6Address->m_IPAddress : IP6Address());
    }
#endif

    /*
        MDNSResponder::answerPort
    */
    uint16_t MDNSResponder::answerPort(const uint32_t p_u32AnswerIndex)
    {
        const stcMDNSServiceQuery*            pServiceQuery = _findLegacyServiceQuery();
        const stcMDNSServiceQuery::stcAnswer* pSQAnswer
            = (pServiceQuery ? pServiceQuery->answerAtIndex(p_u32AnswerIndex) : 0);
        return (pSQAnswer ? pSQAnswer->m_u16Port : 0);
    }

    /*
        MDNSResponder::hostname (LEGACY)
    */
    String MDNSResponder::hostname(const uint32_t p_u32AnswerIndex)
    {
        return String(answerHostname(p_u32AnswerIndex));
    }

    /*
        MDNSResponder::IP (LEGACY)
    */
    IPAddress MDNSResponder::IP(const uint32_t p_u32AnswerIndex)
    {
        return answerIP(p_u32AnswerIndex);
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
        - answerIP4Address/answerIP6Address
        - answerPort
        - answerTxts

    */
    MDNSResponder::hMDNSServiceQuery
    MDNSResponder::installServiceQuery(const char* p_pcService, const char* p_pcProtocol,
                                       MDNSResponder::MDNSServiceQueryCallbackFunc p_fnCallback)
    {
        hMDNSServiceQuery hResult = 0;

        stcMDNSServiceQuery* pServiceQuery = 0;
        if ((p_pcService) && (os_strlen(p_pcService)) && (p_pcProtocol) && (os_strlen(p_pcProtocol))
            && (p_fnCallback) && ((pServiceQuery = _allocServiceQuery()))
            && (_buildDomainForService(p_pcService, p_pcProtocol,
                                       pServiceQuery->m_ServiceTypeDomain)))
        {
            pServiceQuery->m_fnCallback   = p_fnCallback;
            pServiceQuery->m_bLegacyQuery = false;

            if (_sendMDNSServiceQuery(*pServiceQuery))
            {
                pServiceQuery->m_u8SentCount = 1;
                pServiceQuery->m_ResendTimeout.reset(MDNS_DYNAMIC_QUERY_RESEND_DELAY);

                hResult = (hMDNSServiceQuery)pServiceQuery;
            }
            else
            {
                _removeServiceQuery(pServiceQuery);
            }
        }
        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(
            PSTR("[MDNSResponder] installServiceQuery: %s for '%s.%s'!\n\n"),
            (hResult ? "Succeeded" : "FAILED"), (p_pcService ?: "-"), (p_pcProtocol ?: "-")););
        DEBUG_EX_ERR(if (!hResult) {
            DEBUG_OUTPUT.printf_P(
                PSTR("[MDNSResponder] installServiceQuery: FAILED for '%s.%s'!\n\n"),
                (p_pcService ?: "-"), (p_pcProtocol ?: "-"));
        });
        return hResult;
    }

    /*
        MDNSResponder::removeServiceQuery

        Remove a dynamic service query (and all collected answers) from the MDNS responder

    */
    bool MDNSResponder::removeServiceQuery(MDNSResponder::hMDNSServiceQuery p_hServiceQuery)
    {
        stcMDNSServiceQuery* pServiceQuery = 0;
        bool                 bResult       = (((pServiceQuery = _findServiceQuery(p_hServiceQuery)))
                        && (_removeServiceQuery(pServiceQuery)));
        DEBUG_EX_ERR(if (!bResult) {
            DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] removeServiceQuery: FAILED!\n"));
        });
        return bResult;
    }

    /*
        MDNSResponder::answerCount
    */
    uint32_t MDNSResponder::answerCount(const MDNSResponder::hMDNSServiceQuery p_hServiceQuery)
    {
        stcMDNSServiceQuery* pServiceQuery = _findServiceQuery(p_hServiceQuery);
        return (pServiceQuery ? pServiceQuery->answerCount() : 0);
    }

    std::vector<MDNSResponder::MDNSServiceInfo>
    MDNSResponder::answerInfo(const MDNSResponder::hMDNSServiceQuery p_hServiceQuery)
    {
        std::vector<MDNSResponder::MDNSServiceInfo> tempVector;
        for (uint32_t i = 0; i < answerCount(p_hServiceQuery); i++)
        {
            tempVector.emplace_back(*this, p_hServiceQuery, i);
        }
        return tempVector;
    }

    /*
        MDNSResponder::answerServiceDomain

        Returns the domain for the given service.
        If not already existing, the string is allocated, filled and attached to the answer.

    */
    const char*
    MDNSResponder::answerServiceDomain(const MDNSResponder::hMDNSServiceQuery p_hServiceQuery,
                                       const uint32_t                         p_u32AnswerIndex)
    {
        stcMDNSServiceQuery*            pServiceQuery = _findServiceQuery(p_hServiceQuery);
        stcMDNSServiceQuery::stcAnswer* pSQAnswer
            = (pServiceQuery ? pServiceQuery->answerAtIndex(p_u32AnswerIndex) : 0);
        // Fill m_pcServiceDomain (if not already done)
        if ((pSQAnswer) && (pSQAnswer->m_ServiceDomain.m_u16NameLength)
            && (!pSQAnswer->m_pcServiceDomain))
        {
            pSQAnswer->m_pcServiceDomain
                = pSQAnswer->allocServiceDomain(pSQAnswer->m_ServiceDomain.c_strLength());
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
    bool MDNSResponder::hasAnswerHostDomain(const MDNSResponder::hMDNSServiceQuery p_hServiceQuery,
                                            const uint32_t                         p_u32AnswerIndex)
    {
        stcMDNSServiceQuery*            pServiceQuery = _findServiceQuery(p_hServiceQuery);
        stcMDNSServiceQuery::stcAnswer* pSQAnswer
            = (pServiceQuery ? pServiceQuery->answerAtIndex(p_u32AnswerIndex) : 0);
        return ((pSQAnswer)
                && (pSQAnswer->m_u32ContentFlags & ServiceQueryAnswerType_HostDomainAndPort));
    }

    /*
        MDNSResponder::answerHostDomain

        Returns the host domain for the given service.
        If not already existing, the string is allocated, filled and attached to the answer.

    */
    const char*
    MDNSResponder::answerHostDomain(const MDNSResponder::hMDNSServiceQuery p_hServiceQuery,
                                    const uint32_t                         p_u32AnswerIndex)
    {
        stcMDNSServiceQuery*            pServiceQuery = _findServiceQuery(p_hServiceQuery);
        stcMDNSServiceQuery::stcAnswer* pSQAnswer
            = (pServiceQuery ? pServiceQuery->answerAtIndex(p_u32AnswerIndex) : 0);
        // Fill m_pcHostDomain (if not already done)
        if ((pSQAnswer) && (pSQAnswer->m_HostDomain.m_u16NameLength)
            && (!pSQAnswer->m_pcHostDomain))
        {
            pSQAnswer->m_pcHostDomain
                = pSQAnswer->allocHostDomain(pSQAnswer->m_HostDomain.c_strLength());
            if (pSQAnswer->m_pcHostDomain)
            {
                pSQAnswer->m_HostDomain.c_str(pSQAnswer->m_pcHostDomain);
            }
        }
        return (pSQAnswer ? pSQAnswer->m_pcHostDomain : 0);
    }

#ifdef MDNS_IP4_SUPPORT
    /*
        MDNSResponder::hasAnswerIP4Address
    */
    bool MDNSResponder::hasAnswerIP4Address(const MDNSResponder::hMDNSServiceQuery p_hServiceQuery,
                                            const uint32_t                         p_u32AnswerIndex)
    {
        stcMDNSServiceQuery*            pServiceQuery = _findServiceQuery(p_hServiceQuery);
        stcMDNSServiceQuery::stcAnswer* pSQAnswer
            = (pServiceQuery ? pServiceQuery->answerAtIndex(p_u32AnswerIndex) : 0);
        return ((pSQAnswer) && (pSQAnswer->m_u32ContentFlags & ServiceQueryAnswerType_IP4Address));
    }

    /*
        MDNSResponder::answerIP4AddressCount
    */
    uint32_t
    MDNSResponder::answerIP4AddressCount(const MDNSResponder::hMDNSServiceQuery p_hServiceQuery,
                                         const uint32_t                         p_u32AnswerIndex)
    {
        stcMDNSServiceQuery*            pServiceQuery = _findServiceQuery(p_hServiceQuery);
        stcMDNSServiceQuery::stcAnswer* pSQAnswer
            = (pServiceQuery ? pServiceQuery->answerAtIndex(p_u32AnswerIndex) : 0);
        return (pSQAnswer ? pSQAnswer->IP4AddressCount() : 0);
    }

    /*
        MDNSResponder::answerIP4Address
    */
    IPAddress
    MDNSResponder::answerIP4Address(const MDNSResponder::hMDNSServiceQuery p_hServiceQuery,
                                    const uint32_t                         p_u32AnswerIndex,
                                    const uint32_t                         p_u32AddressIndex)
    {
        stcMDNSServiceQuery*            pServiceQuery = _findServiceQuery(p_hServiceQuery);
        stcMDNSServiceQuery::stcAnswer* pSQAnswer
            = (pServiceQuery ? pServiceQuery->answerAtIndex(p_u32AnswerIndex) : 0);
        stcMDNSServiceQuery::stcAnswer::stcIP4Address* pIP4Address
            = (pSQAnswer ? pSQAnswer->IP4AddressAtIndex(p_u32AddressIndex) : 0);
        return (pIP4Address ? pIP4Address->m_IPAddress : IPAddress());
    }
#endif

#ifdef MDNS_IP6_SUPPORT
    /*
        MDNSResponder::hasAnswerIP6Address
    */
    bool MDNSResponder::hasAnswerIP6Address(const MDNSResponder::hMDNSServiceQuery p_hServiceQuery,
                                            const uint32_t                         p_u32AnswerIndex)
    {
        stcMDNSServiceQuery*            pServiceQuery = _findServiceQuery(p_hServiceQuery);
        stcMDNSServiceQuery::stcAnswer* pSQAnswer
            = (pServiceQuery ? pServiceQuery->answerAtIndex(p_u32AnswerIndex) : 0);
        return ((pSQAnswer)
                && (pSQAnswer->m_u32ContentFlags & ServiceQueryAnswerType_HostIP6Address));
    }

    /*
        MDNSResponder::answerIP6AddressCount
    */
    uint32_t
    MDNSResponder::answerIP6AddressCount(const MDNSResponder::hMDNSServiceQuery p_hServiceQuery,
                                         const uint32_t                         p_u32AnswerIndex)
    {
        stcMDNSServiceQuery*            pServiceQuery = _findServiceQuery(p_hServiceQuery);
        stcMDNSServiceQuery::stcAnswer* pSQAnswer
            = (pServiceQuery ? pServiceQuery->answerAtIndex(p_u32AnswerIndex) : 0);
        return (pSQAnswer ? pSQAnswer->IP6AddressCount() : 0);
    }

    /*
        MDNSResponder::answerIP6Address
    */
    IPAddress
    MDNSResponder::answerIP6Address(const MDNSResponder::hMDNSServiceQuery p_hServiceQuery,
                                    const uint32_t                         p_u32AnswerIndex,
                                    const uint32_t                         p_u32AddressIndex)
    {
        stcMDNSServiceQuery*            pServiceQuery = _findServiceQuery(p_hServiceQuery);
        stcMDNSServiceQuery::stcAnswer* pSQAnswer
            = (pServiceQuery ? pServiceQuery->answerAtIndex(p_u32AnswerIndex) : 0);
        stcMDNSServiceQuery::stcAnswer::stcIP6Address* pIP6Address
            = (pSQAnswer ? pSQAnswer->IP6AddressAtIndex(p_u32AddressIndex) : 0);
        return (pIP6Address ? pIP6Address->m_IPAddress : IPAddress());
    }
#endif

    /*
        MDNSResponder::hasAnswerPort
    */
    bool MDNSResponder::hasAnswerPort(const MDNSResponder::hMDNSServiceQuery p_hServiceQuery,
                                      const uint32_t                         p_u32AnswerIndex)
    {
        stcMDNSServiceQuery*            pServiceQuery = _findServiceQuery(p_hServiceQuery);
        stcMDNSServiceQuery::stcAnswer* pSQAnswer
            = (pServiceQuery ? pServiceQuery->answerAtIndex(p_u32AnswerIndex) : 0);
        return ((pSQAnswer)
                && (pSQAnswer->m_u32ContentFlags & ServiceQueryAnswerType_HostDomainAndPort));
    }

    /*
        MDNSResponder::answerPort
    */
    uint16_t MDNSResponder::answerPort(const MDNSResponder::hMDNSServiceQuery p_hServiceQuery,
                                       const uint32_t                         p_u32AnswerIndex)
    {
        stcMDNSServiceQuery*            pServiceQuery = _findServiceQuery(p_hServiceQuery);
        stcMDNSServiceQuery::stcAnswer* pSQAnswer
            = (pServiceQuery ? pServiceQuery->answerAtIndex(p_u32AnswerIndex) : 0);
        return (pSQAnswer ? pSQAnswer->m_u16Port : 0);
    }

    /*
        MDNSResponder::hasAnswerTxts
    */
    bool MDNSResponder::hasAnswerTxts(const MDNSResponder::hMDNSServiceQuery p_hServiceQuery,
                                      const uint32_t                         p_u32AnswerIndex)
    {
        stcMDNSServiceQuery*            pServiceQuery = _findServiceQuery(p_hServiceQuery);
        stcMDNSServiceQuery::stcAnswer* pSQAnswer
            = (pServiceQuery ? pServiceQuery->answerAtIndex(p_u32AnswerIndex) : 0);
        return ((pSQAnswer) && (pSQAnswer->m_u32ContentFlags & ServiceQueryAnswerType_Txts));
    }

    /*
        MDNSResponder::answerTxts

        Returns all TXT items for the given service as a ';'-separated string.
        If not already existing; the string is allocated, filled and attached to the answer.

    */
    const char* MDNSResponder::answerTxts(const MDNSResponder::hMDNSServiceQuery p_hServiceQuery,
                                          const uint32_t                         p_u32AnswerIndex)
    {
        stcMDNSServiceQuery*            pServiceQuery = _findServiceQuery(p_hServiceQuery);
        stcMDNSServiceQuery::stcAnswer* pSQAnswer
            = (pServiceQuery ? pServiceQuery->answerAtIndex(p_u32AnswerIndex) : 0);
        // Fill m_pcTxts (if not already done)
        if ((pSQAnswer) && (pSQAnswer->m_Txts.m_pTxts) && (!pSQAnswer->m_pcTxts))
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
        MDNSResponder::setProbeResultCallback

        Set a global callback for probe results. The callback is called, when probing
        for the host domain (or a service domain, without specific probe result callback)
        fails or succeeds.
        In the case of failure, the domain name should be changed via 'setHostname' or
       'setServiceName'. When succeeded, the host or service domain will be announced by the MDNS
       responder.

    */
    bool MDNSResponder::setHostProbeResultCallback(MDNSResponder::MDNSHostProbeFn p_fnCallback)
    {
        m_HostProbeInformation.m_fnHostProbeResultCallback = p_fnCallback;

        return true;
    }

    bool MDNSResponder::setHostProbeResultCallback(MDNSHostProbeFn1 pfn)
    {
        using namespace std::placeholders;
        return setHostProbeResultCallback(
            [this, pfn](const char* p_pcDomainName, bool p_bProbeResult)
            {
                pfn(*this, p_pcDomainName, p_bProbeResult);
            });
    }

    /*
        MDNSResponder::setServiceProbeResultCallback

        Set a service specific callback for probe results. The callback is called, when probing
        for the service domain fails or succeeds.
        In the case of failure, the service name should be changed via 'setServiceName'.
        When succeeded, the service domain will be announced by the MDNS responder.

    */
    bool
    MDNSResponder::setServiceProbeResultCallback(const MDNSResponder::hMDNSService p_hService,
                                                 MDNSResponder::MDNSServiceProbeFn p_fnCallback)
    {
        bool bResult = false;

        stcMDNSService* pService = _findService(p_hService);
        if (pService)
        {
            pService->m_ProbeInformation.m_fnServiceProbeResultCallback = p_fnCallback;

            bResult = true;
        }
        return bResult;
    }

    bool
    MDNSResponder::setServiceProbeResultCallback(const MDNSResponder::hMDNSService  p_hService,
                                                 MDNSResponder::MDNSServiceProbeFn1 p_fnCallback)
    {
        using namespace std::placeholders;
        return setServiceProbeResultCallback(
            p_hService,
            [this, p_fnCallback](const char* p_pcServiceName, const hMDNSService p_hMDNSService,
                                 bool p_bProbeResult)
            {
                p_fnCallback(*this, p_pcServiceName, p_hMDNSService, p_bProbeResult);
            });
    }

    /*
        MISC
    */

    /*
        MDNSResponder::notifyAPChange

        Should be called, whenever the AP for the MDNS responder changes.
        A bit of this is caught by the event callbacks installed in the constructor.

    */
    bool MDNSResponder::notifyAPChange(void)
    {
        return _restart();
    }

    /*
        MDNSResponder::update

        Should be called in every 'loop'.

    */
    bool MDNSResponder::update(void)
    {
        return _process(true);
    }

    /*
        MDNSResponder::announce

        Should be called, if the 'configuration' changes. Mainly this will be changes in the TXT
       items...
    */
    bool MDNSResponder::announce(void)
    {
        return (_announce(true, true));
    }

    /*
        MDNSResponder::enableArduino

        Enable the OTA update service.

    */
    MDNSResponder::hMDNSService MDNSResponder::enableArduino(uint16_t p_u16Port,
                                                             bool     p_bAuthUpload /*= false*/)
    {
        hMDNSService hService = addService(0, "arduino", "tcp", p_u16Port);
        if (hService)
        {
            if ((!addServiceTxt(hService, "tcp_check", "no"))
                || (!addServiceTxt(hService, "ssh_upload", "no"))
                || (!addServiceTxt(hService, "board", ARDUINO_BOARD_ID))
                || (!addServiceTxt(hService, "auth_upload", (p_bAuthUpload) ? "yes" : "no")))
            {
                removeService(hService);
                hService = 0;
            }
        }
        return hService;
    }

    /*

        MULTICAST GROUPS

    */

    /*
        MDNSResponder::_joinMulticastGroups
    */
    bool MDNSResponder::_joinMulticastGroups(void)
    {
        bool bResult = false;

        // Join multicast group(s)
        for (netif* pNetIf = netif_list; pNetIf; pNetIf = pNetIf->next)
        {
            if (netif_is_up(pNetIf) && IPAddress(pNetIf->ip_addr).isSet())
            {
#ifdef MDNS_IP4_SUPPORT
                ip_addr_t multicast_addr_V4 = DNS_MQUERY_IPV4_GROUP_INIT;
                if (!(pNetIf->flags & NETIF_FLAG_IGMP))
                {
                    DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR(
                        "[MDNSResponder] _createHost: Setting flag: flags & NETIF_FLAG_IGMP\n")););
                    pNetIf->flags |= NETIF_FLAG_IGMP;

                    if (ERR_OK != igmp_start(pNetIf))
                    {
                        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(
                            PSTR("[MDNSResponder] _createHost: igmp_start FAILED!\n")););
                    }
                }

                if ((ERR_OK == igmp_joingroup_netif(pNetIf, ip_2_ip4(&multicast_addr_V4))))
                {
                    bResult = true;
                }
                else
                {
                    DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(
                        PSTR("[MDNSResponder] _createHost: igmp_joingroup_netif(" NETIFID_STR
                             ": %s) FAILED!\n"),
                        NETIFID_VAL(pNetIf), IPAddress(multicast_addr_V4).toString().c_str()););
                }
#endif

#ifdef MDNS_IPV6_SUPPORT
                ip_addr_t multicast_addr_V6 = DNS_MQUERY_IPV6_GROUP_INIT;
                bResult
                    = ((bResult)
                       && (ERR_OK == mld6_joingroup_netif(pNetIf, ip_2_ip6(&multicast_addr_V6))));
                DEBUG_EX_ERR_IF(
                    !bResult,
                    DEBUG_OUTPUT.printf_P(
                        PSTR("[MDNSResponder] _createHost: mld6_joingroup_netif (" NETIFID_STR
                             ") FAILED!\n"),
                        NETIFID_VAL(pNetIf)));
#endif
            }
        }
        return bResult;
    }

    /*
        clsLEAmDNS2_Host::_leaveMulticastGroups
    */
    bool MDNSResponder::_leaveMulticastGroups()
    {
        bool bResult = false;

        for (netif* pNetIf = netif_list; pNetIf; pNetIf = pNetIf->next)
        {
            if (netif_is_up(pNetIf) && IPAddress(pNetIf->ip_addr).isSet())
            {
                bResult = true;

                // Leave multicast group(s)
#ifdef MDNS_IP4_SUPPORT
                ip_addr_t multicast_addr_V4 = DNS_MQUERY_IPV4_GROUP_INIT;
                if (ERR_OK != igmp_leavegroup_netif(pNetIf, ip_2_ip4(&multicast_addr_V4)))
                {
                    DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("\n")););
                }
#endif
#ifdef MDNS_IPV6_SUPPORT
                ip_addr_t multicast_addr_V6 = DNS_MQUERY_IPV6_GROUP_INIT;
                if (ERR_OK
                    != mld6_leavegroup_netif(
                        pNetIf, ip_2_ip6(&multicast_addr_V6) /*&(multicast_addr_V6.u_addr.ip6)*/))
                {
                    DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("\n")););
                }
#endif
            }
        }
        return bResult;
    }

}  // namespace MDNSImplementation

}  // namespace esp8266
