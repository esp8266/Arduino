/*
    LEAmDNS2Host_Control.cpp

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


namespace esp8266
{


namespace experimental
{


/*

    RECEIVING

*/

/*
    clsLEAmDNS2_Host::_parseMessage

*/
bool clsLEAMDNSHost::_parseMessage()
{
    DEBUG_EX_INFO(
        unsigned long   ulStartTime = millis();
        unsigned        uStartMemory = ESP.getFreeHeap();
        DEBUG_OUTPUT.printf_P(PSTR("%s _parseMessage (Time: %lu ms, heap: %u bytes, from %s, to %s)\n"), _DH(), ulStartTime, uStartMemory,
                              m_pUDPContext->getRemoteAddress().toString().c_str(),
                              m_pUDPContext->getDestAddress().toString().c_str());
    );
    //DEBUG_EX_INFO(_udpDump(););
    netif* pNetIf = m_pUDPContext->getInputNetif();

    bool    bResult = false;

    clsMsgHeader   header;
    if (_readMDNSMsgHeader(header))
    {
        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _parseMessage: ID:%u QR:%u OP:%u AA:%u TC:%u RD:%u RA:%u R:%u QD:%u AN:%u NS:%u AR:%u\n"),
                                            _DH(),
                                            (unsigned)header.m_u16ID,
                                            (unsigned)header.m_1bQR, (unsigned)header.m_4bOpcode, (unsigned)header.m_1bAA, (unsigned)header.m_1bTC, (unsigned)header.m_1bRD,
                                            (unsigned)header.m_1bRA, (unsigned)header.m_4bRCode,
                                            (unsigned)header.m_u16QDCount,
                                            (unsigned)header.m_u16ANCount,
                                            (unsigned)header.m_u16NSCount,
                                            (unsigned)header.m_u16ARCount));
        if (0 == header.m_4bOpcode)
        {
            // A standard query
            if (header.m_1bQR)
            {
                // Received a response -> answers to a query
                //DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("%s _parseMessage: Reading answers: ID:%u, Q:%u, A:%u, NS:%u, AR:%u\n"), _DH(), header.m_u16ID, header.m_u16QDCount, header.m_u16ANCount, header.m_u16NSCount, header.m_u16ARCount););
                bResult = _parseResponse(pNetIf, header);
            }
            else
            {
                // Received a query (Questions)
                //DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("%s _parseMessage: Reading query: ID:%u, Q:%u, A:%u, NS:%u, AR:%u\n"), _DH(), header.m_u16ID, header.m_u16QDCount, header.m_u16ANCount, header.m_u16NSCount, header.m_u16ARCount););
                bResult = _parseQuery(m_pUDPContext->getInputNetif(), header);
            }
        }
        else
        {
            DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _parseMessage: Received UNEXPECTED opcode:%u. Ignoring message!\n"), _DH(), header.m_4bOpcode););
            m_pUDPContext->flush();
        }
    }
    else
    {
        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _parseMessage: FAILED to read header\n"), _DH()););
        m_pUDPContext->flush();
    }
    DEBUG_EX_INFO(
        unsigned    uFreeHeap = ESP.getFreeHeap();
        DEBUG_OUTPUT.printf_P(PSTR("%s _parseMessage: Done (%s after %lu ms, ate %i bytes, remaining %u)\n\n"), _DH(), (bResult ? "Succeeded" : "FAILED"), (millis() - ulStartTime), (uStartMemory - uFreeHeap), uFreeHeap);
    );
    return bResult;
}

/*
    clsLEAmDNS2_Host::_parseQuery

    Queries are of interest in two cases:
    1. allow for tiebreaking while probing in the case of a race condition between two instances probing for
      the same name at the same time
    2. provide answers to questions for our host domain or any presented service

    When reading the questions, a set of (planned) responses is created, eg. a reverse PTR question for the host domain
    gets an A (IP address) response, a PTR question for the _services._dns-sd domain gets a PTR (type) response for any
    registered service, ...

    As any mDNS responder should be able to handle 'legacy' queries (from DNS clients), this case is handled here also.
    Legacy queries have got only one (unicast) question and are directed to the local DNS port (not the multicast port).

*/
bool clsLEAMDNSHost::_parseQuery(netif* pNetIf,
                                 const clsLEAMDNSHost::clsMsgHeader& p_MsgHeader)
{
    bool    bResult = true;

    clsSendParameter    sendParameter;
    uint32_t            u32HostOrServiceReplies = 0;
    bool                bHostOrServiceTiebreakNeeded = false;
    for (uint16_t qd = 0; ((bResult) && (qd < p_MsgHeader.m_u16QDCount)); ++qd)
    {
        clsRRQuestion  questionRR;
        if ((bResult = _readRRQuestion(questionRR)))
        {
            // Define host replies, BUT only answer queries after probing is done
            u32HostOrServiceReplies =
                sendParameter.m_u32HostReplyMask |= ((probeStatus())
                                                    ? _replyMaskForHost(pNetIf, questionRR.m_Header, 0)
                                                    : 0);
            DEBUG_EX_INFO(if (u32HostOrServiceReplies) DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Host reply needed %s\n"), _DH(), _replyFlags2String(u32HostOrServiceReplies)););

            // Check tiebreak need for host domain
            if (clsProbeInformation_Base::clsProbeInformation_Base::enuProbingStatus::InProgress == m_ProbeInformation.m_ProbingStatus)
            {
                bool    bFullNameMatch = false;
                if ((_replyMaskForHost(pNetIf, questionRR.m_Header, &bFullNameMatch)) &&
                        (bFullNameMatch))
                {
                    // We're in 'probing' state and someone is asking for our host domain: this might be
                    // a race-condition: Two hosts with the same domain names try simutanously to probe their domains
                    // See: RFC 6762, 8.2 (Tiebraking)
                    // However, we're using a max. reduced approach for tiebreaking here: The higher IP-address wins!
                    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Possible race-condition for host domain detected while probing.\n"), _DH()););

                    bHostOrServiceTiebreakNeeded =
                        m_ProbeInformation.m_bTiebreakNeeded = true;
                }
            }

            // Define service replies
            for (clsService::list::iterator it = m_Services.begin(); ((bResult) && (it != m_Services.end())); it++)
            {
                clsService* pService = *it;

                // Define service replies, BUT only answer queries after probing is done
                uint32_t u32ReplyMaskForQuestion = ((pService->probeStatus())
                                                    ? _replyMaskForService(questionRR.m_Header, *pService, 0)
                                                    : 0);
                u32HostOrServiceReplies |= (pService->m_u32ReplyMask |= u32ReplyMaskForQuestion);
                DEBUG_EX_INFO(if (u32ReplyMaskForQuestion) DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Service reply needed: %s\n"), _DH(pService), _replyFlags2String(u32ReplyMaskForQuestion)););

                // Check tiebreak need for service domain
                if (clsProbeInformation_Base::clsProbeInformation_Base::enuProbingStatus::InProgress == pService->m_ProbeInformation.m_ProbingStatus)
                {
                    bool    bFullNameMatch = false;
                    if ((_replyMaskForService(questionRR.m_Header, *pService, &bFullNameMatch)) &&
                            (bFullNameMatch))
                    {
                        // We're in 'probing' state and someone is asking for this service domain: this might be
                        // a race-condition: Two services with the same domain names try simutanously to probe their domains
                        // See: RFC 6762, 8.2 (Tiebraking)
                        // However, we're using a max. reduced approach for tiebreaking here: The 'higher' SRV host wins!
                        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Possible race-condition for service domain detected while probing.\n"), _DH(pService)););

                        bHostOrServiceTiebreakNeeded =
                            pService->m_ProbeInformation.m_bTiebreakNeeded = true;
                    }
                }
            }

            // Handle unicast and legacy specialities
            // If only one question asks for unicast reply, the whole reply packet is send unicast
            if (((DNS_MQUERY_PORT != m_pUDPContext->getRemotePort()) ||     // Unicast (maybe legacy) query OR
                    (questionRR.m_bUnicast)) &&                                // Expressivly unicast query
                    (!sendParameter.m_bUnicast))
            {
                sendParameter.m_bUnicast = true;
                //sendParameter.m_bCacheFlush = false;
                DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Unicast response asked for %s!\n"), _DH(), m_pUDPContext->getRemoteAddress().toString().c_str()););
                //Serial.printf_P(PSTR("%s _parseQuery: Ignored Unicast response asked for by %s!\n"), _DH(), IPAddress(m_pUDPContext->getRemoteAddress()).toString().c_str());

                if ((DNS_MQUERY_PORT != m_pUDPContext->getRemotePort()) &&  // Unicast (maybe legacy) query AND
                        (1 == p_MsgHeader.m_u16QDCount) &&                          // Only one question AND
                        ((sendParameter.m_u32HostReplyMask) ||                      //  Host replies OR
                         (u32HostOrServiceReplies)))                                //  Host or service replies available
                {
                    // Local host check
                    // We're a match for this legacy query, BUT
                    // make sure, that the query comes from a local host
                    if ((m_pUDPContext) &&
#ifdef MDNS_IPV4_SUPPORT
                            (m_pUDPContext->getRemoteAddress().isV4()) &&
                            (ip4_addr_netcmp(ip_2_ip4((const ip_addr_t*)m_pUDPContext->getRemoteAddress()),
                                             ip_2_ip4(&m_pUDPContext->getInputNetif()->ip_addr),
                                             ip_2_ip4(&m_pUDPContext->getInputNetif()->netmask)))
#else
                            (true)
#endif
                            &&
#ifdef MDNS_IPV6_SUPPORT
                            (m_pUDPContext->getRemoteAddress().isV6()) &&
                            (ip6_addr_islinklocal(ip_2_ip6((const ip_addr_t*)m_pUDPContext->getRemoteAddress())))
#else
                            (true)
#endif
                       )
                    {
                        DEBUG_EX_RX(DEBUG_OUTPUT.println("\n\n\nUNICAST QUERY\n\n"));
                        DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Legacy DNS query from local host %s!\n"), _DH(), m_pUDPContext->getRemoteAddress().toString().c_str()););

                        sendParameter.m_u16ID = p_MsgHeader.m_u16ID;
                        sendParameter.m_bLegacyDNSQuery = true;
                        sendParameter.m_bCacheFlush = false;
                        clsRRQuestion*  pNewRRQuestion = new clsRRQuestion;
                        if (pNewRRQuestion)
                        {
                            pNewRRQuestion->m_Header.m_Domain = questionRR.m_Header.m_Domain;
                            pNewRRQuestion->m_Header.m_Attributes.m_u16Type = questionRR.m_Header.m_Attributes.m_u16Type;
                            pNewRRQuestion->m_Header.m_Attributes.m_u16Class = questionRR.m_Header.m_Attributes.m_u16Class;

                            sendParameter.m_RRQuestions.push_back(pNewRRQuestion);
                        }
                        else
                        {
                            DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: FAILED to add legacy DNS question!\n"), _DH()););
                        }
                    }
                    else
                    {
                        DEBUG_EX_RX(DEBUG_OUTPUT.printf("\n\n\nINVALID UNICAST QUERY from %s\n\n\n", m_pUDPContext->getRemoteAddress().toString().c_str()));
                        DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Legacy DNS query from NON-LOCAL host at %s!\n"), _DH(), m_pUDPContext->getRemoteAddress().toString().c_str()););
                        bResult = false;
                    }
                }
            }
        }
        else
        {
            DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: FAILED to read question!\n"), _DH()););
        }
    }   // for questions

    //DEBUG_EX_INFO(if (u8HostOrServiceReplies) DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Reply needed: %u (%s: %s->%s)\n"), _DH(), u8HostOrServiceReplies, clsTimeSyncer::timestr(), IPAddress(m_pUDPContext->getRemoteAddress()).toString().c_str(), IPAddress(m_pUDPContext->getDestAddress()).toString().c_str()););
    //bHostOrServiceTiebreakNeeded = false;

    // Handle known answers
    uint32_t    u32Answers = (p_MsgHeader.m_u16ANCount + p_MsgHeader.m_u16NSCount + p_MsgHeader.m_u16ARCount);
    if (((u32HostOrServiceReplies) ||
            (bHostOrServiceTiebreakNeeded)) &&
            (u32Answers))
    {
        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Reading known answers(%u):\n"), _DH(), u32Answers););

        for (uint32_t an = 0; ((bResult) && (an < u32Answers)); ++an)
        {
            clsRRAnswer*   pKnownRRAnswer = 0;
            if (((bResult = _readRRAnswer(pKnownRRAnswer))) &&
                    (pKnownRRAnswer))
            {
                if ((DNS_RRTYPE_ANY != pKnownRRAnswer->m_Header.m_Attributes.m_u16Type) &&                  // No ANY type answer
                        (DNS_RRCLASS_ANY != (pKnownRRAnswer->m_Header.m_Attributes.m_u16Class & (~0x8000))))    // No ANY class answer
                {
                    /*  - RFC6762 7.1 Suppression only for 'Shared Records' -
                        // Find match between planned answer (sendParameter.m_u8HostReplyMask) and this 'known answer'
                        uint32_t u32HostMatchMask = (sendParameter.m_u32HostReplyMask & _replyMaskForHost(pNetIf, pKnownRRAnswer->m_Header));
                        if ((u32HostMatchMask) &&                                           // The RR in the known answer matches an RR we are planning to send, AND
                        ((Consts::u32HostTTL / 2) <= pKnownRRAnswer->m_u32TTL))              // The TTL of the known answer is longer than half of the new host TTL (120s)
                        {

                        // Compare contents
                        if (enuAnswerType::PTR == pKnownRRAnswer->answerType())
                        {
                            stcRRDomain    hostDomain;
                            if ((_buildDomainForHost(m_pcHostName, hostDomain)) &&
                                (((stcRRAnswerPTR*)pKnownRRAnswer)->m_PTRDomain == hostDomain))
                            {
                                // Host domain match
                        #ifdef MDNS_IPV4_SUPPORT
                                if (u32HostMatchMask & static_cast<uint32_t>(enuContentFlag::PTR_IPv4))
                                {
                                    // IPv4 PTR was asked for, but is already known -> skipping
                                    DEBUG_EX_INFO2(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: IPv4 PTR already known (TTL:%u)... skipping!\n"), _DH(), pKnownRRAnswer->m_u32TTL););
                                    sendParameter.m_u32HostReplyMask &= ~static_cast<uint32_t>(enuContentFlag::PTR_IPv4);
                                }
                        #endif
                        #ifdef MDNS_IPV6_SUPPORT
                                if (u32HostMatchMask & static_cast<uint32_t>(enuContentFlag::PTR_IPv6))
                                {
                                    // IPv6 PTR was asked for, but is already known -> skipping
                                    DEBUG_EX_INFO2(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: IPv6 PTR already known (TTL:%u)... skipping!\n"), _DH(), pKnownRRAnswer->m_u32TTL););
                                    sendParameter.m_u32HostReplyMask &= ~static_cast<uint32_t>(enuContentFlag::PTR_IPv6);
                                }
                        #endif
                            }
                        }
                        else if (u32HostMatchMask & static_cast<uint32_t>(enuContentFlag::A))
                        {
                            // IPv4 address was asked for
                        #ifdef MDNS_IPV4_SUPPORT
                            if ((enuAnswerType::A == pKnownRRAnswer->answerType()) &&
                                (((stcRRAnswerA*)pKnownRRAnswer)->m_IPAddress == _getResponderIPAddress(pNetIf, enuIPProtocolType::V4)))
                            {

                                DEBUG_EX_INFO2(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: IPv4 address already known (TTL:%u)... skipping!\n"), _DH(), pKnownRRAnswer->m_u32TTL););
                                sendParameter.m_u32HostReplyMask &= ~static_cast<uint32_t>(enuContentFlag::A);
                            }   // else: RData NOT IPv4 length !!
                        #endif
                        }
                        else if (u32HostMatchMask & static_cast<uint32_t>(enuContentFlag::AAAA))
                        {
                            // IPv6 address was asked for
                        #ifdef MDNS_IPV6_SUPPORT
                            if ((enuAnswerType::AAAA == pKnownRRAnswer->answerType()) &&
                                (((stcRRAnswerAAAA*)pKnownRRAnswer)->m_IPAddress == _getResponderIPAddress(pNetIf, enuIPProtocolType::V6)))
                            {

                                DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: IPv6 address already known... skipping!\n"), _DH()););
                                sendParameter.m_u32HostReplyMask &= ~static_cast<uint32_t>(enuContentFlag::AAAA);
                            }   // else: RData NOT IPv6 length !!
                        #endif
                        }
                        }   // Host match and TTL
                    */

                    //
                    // Check host tiebreak possibility
                    if (m_ProbeInformation.m_bTiebreakNeeded)
                    {
                        clsRRDomain    hostDomain;
                        if ((_buildDomainForHost(m_pcHostName, hostDomain)) &&
                                (pKnownRRAnswer->m_Header.m_Domain == hostDomain))
                        {
                            // Host domain match
#ifdef MDNS_IPV4_SUPPORT
                            if (enuAnswerType::A == pKnownRRAnswer->answerType())
                            {
                                // CHECK
                                IPAddress   localIPAddress(_getResponderIPAddress(pNetIf, enuIPProtocolType::V4));
                                if (((clsRRAnswerA*)pKnownRRAnswer)->m_IPAddress == localIPAddress)
                                {
                                    // SAME IP address -> We've received an old message from ourselfs (same IP)
                                    DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Tiebreak (IPv4) WON (was an old message)!\n"), _DH()););
                                    m_ProbeInformation.m_bTiebreakNeeded = false;
                                }
                                else
                                {
                                    if ((uint32_t)(((clsRRAnswerA*)pKnownRRAnswer)->m_IPAddress) > (uint32_t)localIPAddress)   // The OTHER IP is 'higher' -> LOST
                                    {
                                        // LOST tiebreak
                                        DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Tiebreak (IPv4) LOST (lower IPv4)!\n"), _DH()););
                                        _cancelProbingForHost();
                                        m_ProbeInformation.m_bTiebreakNeeded = false;
                                    }
                                    else
                                    {
                                        // WON tiebreak
                                        DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Tiebreak (IPv4) WON (higher IPv4)!\n"), _DH()););
                                        m_ProbeInformation.m_bTiebreakNeeded = false;
                                    }
                                }
                            }
#endif
#ifdef MDNS_IPV6_SUPPORT
                            if (enuAnswerType::AAAA == pKnownRRAnswer->answerType())
                            {
                                IPAddress   localIPAddress(_getResponderIPAddress(pNetIf, enuIPProtocolType::V6));
                                if (((clsRRAnswerAAAA*)pKnownRRAnswer)->m_IPAddress == localIPAddress)
                                {
                                    // SAME IP address -> We've received an old message from ourselfs (same IP)
                                    DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Tiebreak (IPv6) WON (was an old message)!\n"), _DH()););
                                    m_ProbeInformation.m_bTiebreakNeeded = false;
                                }
                                else
                                {
                                    // memcmp delivers >0 (positive) if the first non-matching byte in A is higher than in B
                                    if (0 < memcmp((((clsRRAnswerAAAA*)pKnownRRAnswer)->m_IPAddress).raw6(), localIPAddress.raw6(), clsConsts::u16IPv6Size))    // The OTHER IP is 'higher' -> LOST
                                    {
                                        // LOST tiebreak
                                        DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Tiebreak (IPv6) LOST (lower IPv6)!\n"), _DH()););
                                        _cancelProbingForHost();
                                        m_ProbeInformation.m_bTiebreakNeeded = false;
                                    }
                                    else
                                    {
                                        // WON tiebreak
                                        DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Tiebreak (IPv6) WON (higher IPv6)!\n"), _DH()););
                                        m_ProbeInformation.m_bTiebreakNeeded = false;
                                    }
                                }
                            }
#endif
                        }
                    }   // Host tiebreak possibility

                    // Check service answers
                    for (clsService::list::iterator it = m_Services.begin(); ((bResult) && (it != m_Services.end())); it++)
                    {
                        clsService* pService = *it;

                        uint32_t    u32ServiceMatchMask = (pService->m_u32ReplyMask & _replyMaskForService(pKnownRRAnswer->m_Header, *pService));

                        if ((u32ServiceMatchMask) &&                                // The RR in the known answer matches an RR we are planning to send, AND
                                ((clsConsts::u32ServiceTTL / 2) <= pKnownRRAnswer->m_u32TTL))   // The TTL of the known answer is longer than half of the new service TTL (4500s)
                        {
                            if (enuAnswerType::PTR == pKnownRRAnswer->answerType())
                            {
                                clsRRDomain    serviceDomain;
                                if ((u32ServiceMatchMask & static_cast<uint32_t>(enuContentFlag::PTR_TYPE)) &&
                                        (_buildDomainForService(*pService, false, serviceDomain)) &&
                                        (serviceDomain == ((clsRRAnswerPTR*)pKnownRRAnswer)->m_PTRDomain))
                                {
                                    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Service type PTR already known (TTL:%u)... skipping!\n"), _DH(pService), pKnownRRAnswer->m_u32TTL););
                                    pService->m_u32ReplyMask &= ~static_cast<uint32_t>(enuContentFlag::PTR_TYPE);
                                }
                                if ((u32ServiceMatchMask & static_cast<uint32_t>(enuContentFlag::PTR_NAME)) &&
                                        (_buildDomainForService(*pService, true, serviceDomain)) &&
                                        (serviceDomain == ((clsRRAnswerPTR*)pKnownRRAnswer)->m_PTRDomain))
                                {
                                    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Service name PTR already known (TTL:%u)... skipping!\n"), _DH(pService), pKnownRRAnswer->m_u32TTL););
                                    pService->m_u32ReplyMask &= ~static_cast<uint32_t>(enuContentFlag::PTR_NAME);
                                }
                            }
                            /*  - RFC6762 7.1 Suppression only for 'Shared Records'
                                else if (u32ServiceMatchMask & static_cast<uint32_t>(enuContentFlag::SRV))
                                {
                                DEBUG_EX_ERR(if (enuAnswerType::SRV != pKnownRRAnswer->answerType()) DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: ERROR! INVALID answer type (SRV)!\n"), _DH(pService)););
                                stcRRDomain    hostDomain;
                                if ((_buildDomainForHost(m_pcHostName, hostDomain)) &&
                                    (hostDomain == ((stcRRAnswerSRV*)pKnownRRAnswer)->m_SRVDomain))    // Host domain match
                                {

                                    if ((Consts::u16SRVPriority == ((stcRRAnswerSRV*)pKnownRRAnswer)->m_u16Priority) &&
                                        (Consts::u16SRVWeight == ((stcRRAnswerSRV*)pKnownRRAnswer)->m_u16Weight) &&
                                        (pService->m_u16Port == ((stcRRAnswerSRV*)pKnownRRAnswer)->m_u16Port))
                                    {

                                        DEBUG_EX_INFO2(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Service SRV answer already known (TTL:%u)... skipping!\n"), _DH(pService), pKnownRRAnswer->m_u32TTL););
                                        pService->m_u32ReplyMask &= ~static_cast<uint32_t>(enuContentFlag::SRV);
                                    }   // else: Small differences -> send update message
                                }
                                }*/
                            /*  - RFC6762 7.1 Suppression only for 'Shared Records'
                                else if (u32ServiceMatchMask & static_cast<uint32_t>(enuContentFlag::TXT))
                                {
                                DEBUG_EX_ERR(if (enuAnswerType::TXT != pKnownRRAnswer->answerType()) DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: ERROR! INVALID answer type (TXT)!\n"), _DH(pService)););
                                _collectServiceTxts(*pService);
                                if (pService->m_Txts == ((stcRRAnswerTXT*)pKnownRRAnswer)->m_Txts)
                                {
                                    DEBUG_EX_INFO2(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Service TXT answer already known (TTL:%u)... skipping!\n"), _DH(pService), pKnownRRAnswer->m_u32TTL););
                                    pService->m_u32ReplyMask &= ~static_cast<uint32_t>(enuContentFlag::TXT);
                                }
                                _releaseTempServiceTxts(*pService);
                                }*/
                        }   // Service match and enough TTL

                        //
                        // Check service tiebreak possibility
                        if (pService->m_ProbeInformation.m_bTiebreakNeeded)
                        {
                            clsRRDomain    serviceDomain;
                            if ((_buildDomainForService(*pService, true, serviceDomain)) &&
                                    (pKnownRRAnswer->m_Header.m_Domain == serviceDomain))
                            {
                                // Service domain match
                                if (enuAnswerType::SRV == pKnownRRAnswer->answerType())
                                {
                                    clsRRDomain    hostDomain;
                                    if ((_buildDomainForHost(m_pcHostName, hostDomain)) &&
                                            (hostDomain == ((clsRRAnswerSRV*)pKnownRRAnswer)->m_SRVDomain))    // Host domain match
                                    {
                                        // We've received an old message from ourselfs (same SRV)
                                        DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Tiebreak (SRV) won (was an old message)!\n"), _DH(pService)););
                                        pService->m_ProbeInformation.m_bTiebreakNeeded = false;
                                    }
                                    else
                                    {
                                        if (((clsRRAnswerSRV*)pKnownRRAnswer)->m_SRVDomain > hostDomain)   // The OTHER domain is 'higher' -> LOST
                                        {
                                            // LOST tiebreak
                                            DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Tiebreak (SRV) LOST (lower)!\n"), _DH(pService)););
                                            _cancelProbingForService(*pService);
                                            pService->m_ProbeInformation.m_bTiebreakNeeded = false;
                                        }
                                        else
                                        {
                                            // WON tiebreak
                                            DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Tiebreak (SRV) won (higher)!\n"), _DH(pService)););
                                            pService->m_ProbeInformation.m_bTiebreakNeeded = false;
                                        }
                                    }
                                }
                            }
                        }   // service tiebreak possibility
                    }   // for services
                }   // ANY answers
            }
            else
            {
                DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: FAILED to read known answer!\n"), _DH()););
            }

            if (pKnownRRAnswer)
            {
                delete pKnownRRAnswer;
                pKnownRRAnswer = 0;
            }
        }   // for answers
    }
    else
    {
        DEBUG_EX_INFO(if (u32Answers) DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Skipped %u known answers!\n"), _DH(), u32Answers););
        m_pUDPContext->flush();
    }

    if (bResult)
    {
        // Check, if a reply is needed
        uint32_t    u32ReplyNeeded = sendParameter.m_u32HostReplyMask;
        for (const clsService* pService : m_Services)
        {
            u32ReplyNeeded |= pService->m_u32ReplyMask;
        }

        if (u32ReplyNeeded)
        {
            DEBUG_EX_INFO2(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Sending %s answer(%s)...\n"), _DH(), (sendParameter.m_bUnicast ? "UC" : "MC"), _replyFlags2String(u32ReplyNeeded)););
            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Sending %s answer(%s)...\n"), _DH(), (sendParameter.m_bUnicast ? "UC" : "MC"), _replyFlags2String(u32ReplyNeeded)););

            sendParameter.m_Response = clsSendParameter::enuResponseType::Response;
            sendParameter.m_bAuthorative = true;

            bResult = _sendMessage(pNetIf, sendParameter);
        }
        DEBUG_EX_INFO(else
        {
            DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: No reply needed\n"), _DH());
        });
    }
    else
    {
        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: Something FAILED!\n"), _DH()););
        m_pUDPContext->flush();
    }

    //
    // Check and reset tiebreak-states
    if (m_ProbeInformation.m_bTiebreakNeeded)
    {
        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: UNSOLVED tiebreak-need for host domain!\n"), _DH()););
        m_ProbeInformation.m_bTiebreakNeeded = false;
    }
    for (clsService* pService : m_Services)
    {
        if (pService->m_ProbeInformation.m_bTiebreakNeeded)
        {
            DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: UNSOLVED tiebreak-need for service domain '%s')\n"), _DH(), _service2String(pService)););
            pService->m_ProbeInformation.m_bTiebreakNeeded = false;
        }
    }
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _parseQuery: FAILED!\n"), _DH()););
    return bResult;
}

/*
    clsLEAmDNS2_Host::_parseResponse

    Responses are of interest in two cases:
    1. find domain name conflicts while probing
    2. get answers to service queries

    In both cases any included questions are ignored

    1. If any answer has a domain name similar to one of the domain names we're planning to use (and are probing for),
      then we've got a 'probing conflict'. The conflict has to be solved on our side of the conflict (eg. by
      setting a new hostname and restart probing). The callback 'm_fnProbeResultCallback' is called with
      'p_bProbeResult=false' in this case.

    2. Service queries like '_http._tcp.local' will (if available) produce PTR, SRV, TXT and A/AAAA answers.
      All stored answers are pivoted by the service instance name (from the PTR record). Other answer parts,
      like host domain or IP address are than attached to this element.
      Any answer part carries a TTL, this is also stored (incl. the reception time); if the TTL is '0' the
      answer (part) is withdrawn by the sender and should be removed from any cache. RFC 6762, 10.1 proposes to
      set the caches TTL-value to 1 second in such a case and to delete the item only, if no update has
      has taken place in this second.
      Answer parts may arrive in 'unsorted' order, so they are grouped into three levels:
      Level 1: PRT - names the service instance (and is used as pivot), voids all other parts if is withdrawn or outdates
      Level 2: SRV - links the instance name to a host domain and port, voids A/AAAA parts if is withdrawn or outdates
               TXT - links the instance name to services TXTs
      Level 3: A/AAAA - links the host domain to an IP address
*/
bool clsLEAMDNSHost::_parseResponse(netif* pNetIf, const clsLEAMDNSHost::clsMsgHeader& p_MsgHeader)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _parseResponse\n")););
    //DEBUG_EX_INFO(_udpDump(););

    bool    bResult = false;

    // A response should be the result of a query or a probe
    if ((_hasQueriesWaitingForAnswers()) ||     // Waiting for query answers OR
            (_hasProbesWaitingForAnswers()))        // Probe responses
    {
        DEBUG_EX_INFO(
            DEBUG_OUTPUT.printf_P(PSTR("%s _parseResponse: Received a response\n"), _DH());
            //_udpDump();
        );

        bResult = true;
        //
        // Ignore questions here
        clsRRQuestion  dummyRRQ;
        for (uint16_t qd = 0; ((bResult) && (qd < p_MsgHeader.m_u16QDCount)); ++qd)
        {
            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _parseResponse: Received a response containing a question... ignoring!\n"), _DH()););
            bResult = _readRRQuestion(dummyRRQ);
        }   // for queries

        //
        // Read and collect answers
        clsRRAnswer*   pCollectedRRAnswers = 0;
        uint32_t            u32NumberOfAnswerRRs = (p_MsgHeader.m_u16ANCount + p_MsgHeader.m_u16NSCount + p_MsgHeader.m_u16ARCount);
        for (uint32_t an = 0; ((bResult) && (an < u32NumberOfAnswerRRs)); ++an)
        {
            clsRRAnswer*   pRRAnswer = 0;
            if (((bResult = _readRRAnswer(pRRAnswer))) &&
                    (pRRAnswer))
            {
                //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _parseResponse: ADDING answer!\n")););
                pRRAnswer->m_pNext = pCollectedRRAnswers;
                pCollectedRRAnswers = pRRAnswer;
            }
            else
            {
                DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _parseResponse: FAILED to read answer!\n"), _DH()););
                if (pRRAnswer)
                {
                    delete pRRAnswer;
                    pRRAnswer = 0;
                }
                bResult = false;
            }
        }   // for answers

        //
        // Process answers
        if (bResult)
        {
            bResult = ((!pCollectedRRAnswers) ||
                       (_processAnswers(pNetIf, pCollectedRRAnswers)));
        }
        else    // Some failure while reading answers
        {
            DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _parseResponse: FAILED to read answers!\n"), _DH()););
            m_pUDPContext->flush();
        }

        // Delete collected answers
        while (pCollectedRRAnswers)
        {
            //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _parseResponse: DELETING answer!\n"), _DH()););
            clsRRAnswer*   pNextAnswer = pCollectedRRAnswers->m_pNext;
            delete pCollectedRRAnswers;
            pCollectedRRAnswers = pNextAnswer;
        }
    }
    else    // Received an unexpected response -> ignore
    {
        DEBUG_EX_INFO(
            DEBUG_OUTPUT.printf_P(PSTR("%s _parseResponse: Received an unexpected response... ignoring!\n"), _DH());
            /*
                DEBUG_OUTPUT.printf_P(PSTR("%s _parseResponse: Received an unexpected response... ignoring!\nDUMP:\n"), _DH());
                bool    bDumpResult = true;
                for (uint16_t qd=0; ((bDumpResult) && (qd<p_MsgHeader.m_u16QDCount)); ++qd) {
                stcRRQuestion  questionRR;
                bDumpResult = _readRRQuestion(questionRR);
                }   // for questions
                // Handle known answers
                uint32_t    u32Answers = (p_MsgHeader.m_u16ANCount + p_MsgHeader.m_u16NSCount + p_MsgHeader.m_u16ARCount);
                for (uint32_t an=0; ((bDumpResult) && (an<u32Answers)); ++an) {
                stcRRAnswer*   pRRAnswer = 0;
                bDumpResult = _readRRAnswer(pRRAnswer);
                if (pRRAnswer) {
                    delete pRRAnswer;
                    pRRAnswer = 0;
                }
                }
            */
        );
        m_pUDPContext->flush();
        bResult = true;
    }
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _parseResponse: FAILED!\n"), _DH()););
    return bResult;
}

/*
    clsLEAmDNS2_Host::_processAnswers
    Host:
    A (0x01):               eg. esp8266.local A OP TTL 123.456.789.012
    AAAA (01Cx):            eg. esp8266.local AAAA OP TTL 1234:5678::90
    PTR (0x0C, IPv4):       eg. 012.789.456.123.in-addr.arpa PTR OP TTL esp8266.local
    PTR (0x0C, IPv6):       eg. 90.0.0.0.0.0.0.0.0.0.0.0.78.56.34.12.ip6.arpa PTR OP TTL esp8266.local
    Service:
    PTR (0x0C, srv name):   eg. _http._tcp.local PTR OP TTL MyESP._http._tcp.local
    PTR (0x0C, srv type):   eg. _services._dns-sd._udp.local PTR OP TTL _http._tcp.local
    SRV (0x21):             eg. MyESP._http._tcp.local SRV OP TTL PRIORITY WEIGHT PORT esp8266.local
    TXT (0x10):             eg. MyESP._http._tcp.local TXT OP TTL c#=1

*/
bool clsLEAMDNSHost::_processAnswers(netif* pNetIf, const clsLEAMDNSHost::clsRRAnswer* p_pAnswers)
{
    bool    bResult = false;

    if (p_pAnswers)
    {
        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _processAnswers: Processing answers...\n"), _DH()););
        bResult = true;

        // Answers may arrive in an unexpected order. So we loop our answers as long, as we
        // can connect new information to service queries
        bool    bFoundNewKeyAnswer;
        do
        {
            bFoundNewKeyAnswer = false;

            const clsRRAnswer* pRRAnswer = p_pAnswers;
            while ((pRRAnswer) &&
                    (bResult))
            {
                // 1. level answer (PTR)
                if (enuAnswerType::PTR == pRRAnswer->answerType())
                {
                    // eg. _http._tcp.local PTR xxxx xx MyESP._http._tcp.local
                    bResult = _processPTRAnswer((clsRRAnswerPTR*)pRRAnswer, bFoundNewKeyAnswer);   // May 'enable' new SRV or TXT answers to be linked to queries
                }
                // 2. level answers
                // SRV -> host domain and port
                else if (enuAnswerType::SRV == pRRAnswer->answerType())
                {
                    // eg. MyESP._http._tcp.local SRV xxxx xx yy zz 5000 esp8266.local
                    bResult = _processSRVAnswer((clsRRAnswerSRV*)pRRAnswer, bFoundNewKeyAnswer);   // May 'enable' new A/AAAA answers to be linked to queries
                }
                // TXT -> Txts
                else if (enuAnswerType::TXT == pRRAnswer->answerType())
                {
                    // eg. MyESP_http._tcp.local TXT xxxx xx c#=1
                    bResult = _processTXTAnswer((clsRRAnswerTXT*)pRRAnswer);
                }
                // 3. level answers
#ifdef MDNS_IPV4_SUPPORT
                // A -> IPv4Address
                else if (enuAnswerType::A == pRRAnswer->answerType())
                {
                    // eg. esp8266.local A xxxx xx 192.168.2.120
                    bResult = _processAAnswer((clsRRAnswerA*)pRRAnswer);
                }
#endif
#ifdef MDNS_IPV6_SUPPORT
                // AAAA -> IPv6Address
                else if (enuAnswerType::AAAA == pRRAnswer->answerType())
                {
                    // eg. esp8266.local AAAA xxxx xx 09cf::0c
                    bResult = _processAAAAAnswer((clsRRAnswerAAAA*)pRRAnswer);
                }
#endif

                // Finally check for probing conflicts
                // Host domain
                if ((clsProbeInformation_Base::enuProbingStatus::InProgress == m_ProbeInformation.m_ProbingStatus) &&
                        ((enuAnswerType::A == pRRAnswer->answerType()) ||
                         (enuAnswerType::AAAA == pRRAnswer->answerType())))
                {
                    clsRRDomain    hostDomain;
                    if ((_buildDomainForHost(m_pcHostName, hostDomain)) &&
                            (pRRAnswer->m_Header.m_Domain == hostDomain))
                    {
                        bool    bPossibleEcho = false;
#ifdef MDNS_IPV4_SUPPORT
                        if ((enuAnswerType::A == pRRAnswer->answerType()) &&
                                (((clsRRAnswerA*)pRRAnswer)->m_IPAddress == _getResponderIPAddress(pNetIf, enuIPProtocolType::V4)))
                        {
                            bPossibleEcho = true;
                        }
#endif
#ifdef MDNS_IPV6_SUPPORT
                        if ((enuAnswerType::AAAA == pRRAnswer->answerType()) &&
                                (((clsRRAnswerAAAA*)pRRAnswer)->m_IPAddress == _getResponderIPAddress(pNetIf, enuIPProtocolType::V6)))
                        {
                            bPossibleEcho = true;
                        }
#endif
                        if (!bPossibleEcho)
                        {
                            DEBUG_EX_INFO2(DEBUG_OUTPUT.printf_P(PSTR("%s _processAnswers: Probing CONFLICT found with '%s.local'\n"), _DH(), m_pcHostName););
                            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _processAnswers: Probing CONFLICT found with '%s.local'\n"), _DH(), m_pcHostName););
                            _cancelProbingForHost();
                        }
                        else
                        {
                            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _processAnswers: Ignoring CONFLICT found with '%s.local' as echo!\n"), _DH(), m_pcHostName););
                        }
                    }
                }
                // Service domains
                for (clsService* pService : m_Services)
                {
                    if ((clsProbeInformation_Base::enuProbingStatus::InProgress == pService->m_ProbeInformation.m_ProbingStatus) &&
                            ((enuAnswerType::TXT == pRRAnswer->answerType()) ||
                             (enuAnswerType::SRV == pRRAnswer->answerType())))
                    {
                        clsRRDomain    serviceDomain;
                        if ((_buildDomainForService(*pService, true, serviceDomain)) &&
                                (pRRAnswer->m_Header.m_Domain == serviceDomain))
                        {
                            // TODO: Echo management needed?
                            DEBUG_EX_INFO2(DEBUG_OUTPUT.printf_P(PSTR("%s _processAnswers: Probing CONFLICT found with '%s'\n"), _DH(), _service2String(pService)););
                            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _processAnswers: Probing CONFLICT found with '%s'\n"), _DH(), _service2String(pService)););
                            _cancelProbingForService(*pService);
                        }
                    }
                }

                pRRAnswer = pRRAnswer->m_pNext; // Next collected answer
            }   // while (answers)
        } while ((bFoundNewKeyAnswer) &&
                 (bResult));
    }   // else: No answers provided
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _processAnswers: FAILED!\n"), _DH()););
    return bResult;
}

/*
    clsLEAmDNS2_Host::_processPTRAnswer (level 1)
*/
bool clsLEAMDNSHost::_processPTRAnswer(const clsLEAMDNSHost::clsRRAnswerPTR* p_pPTRAnswer,
                                       bool& p_rbFoundNewKeyAnswer)
{
    bool    bResult = false;

    if ((bResult = (0 != p_pPTRAnswer)))
    {
        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _processPTRAnswer: Processing PTR answers...\n"), _DH()););
        // eg. _http._tcp.local PTR xxxx xx MyESP._http._tcp.local
        // Check pending service queries for eg. '_http._tcp'

        clsQuery*    pQuery = _findNextQueryByDomain(p_pPTRAnswer->m_Header.m_Domain, clsQuery::enuQueryType::Service, 0);
        while (pQuery)
        {
            if (pQuery->m_bAwaitingAnswers)
            {
                // Find answer for service domain (eg. MyESP._http._tcp.local)
                clsQuery::clsAnswer* pSQAnswer = pQuery->findAnswerForServiceDomain(p_pPTRAnswer->m_PTRDomain);
                if (pSQAnswer)
                {
                    // existing answer
                    if (p_pPTRAnswer->m_u32TTL)
                    {
                        // Received update message
                        pSQAnswer->m_TTLServiceDomain.set(p_pPTRAnswer->m_u32TTL);    // Update TTL tag
                        DEBUG_EX_INFO(
                            DEBUG_OUTPUT.printf_P(PSTR("%s _processPTRAnswer: Updated TTL(%lu) for "), _DH(), p_pPTRAnswer->m_u32TTL);
                            _printRRDomain(pSQAnswer->m_ServiceDomain);
                            DEBUG_OUTPUT.printf_P(PSTR("\n"));
                        );
                    }
                    else
                    {
                        // received goodbye-message
                        pSQAnswer->m_TTLServiceDomain.prepareDeletion();    // Prepare answer deletion according to RFC 6762, 10.1
                        DEBUG_EX_INFO(
                            DEBUG_OUTPUT.printf_P(PSTR("%s _processPTRAnswer: 'Goodbye' received for "), _DH());
                            _printRRDomain(pSQAnswer->m_ServiceDomain);
                            DEBUG_OUTPUT.printf_P(PSTR("\n"));
                        );
                    }
                }
                else if ((p_pPTRAnswer->m_u32TTL) &&                        // Not just a goodbye-message
                         ((pSQAnswer = new clsQuery::clsAnswer)))           // Not yet included -> add answer
                {
                    pSQAnswer->m_ServiceDomain = p_pPTRAnswer->m_PTRDomain;
                    pSQAnswer->m_QueryAnswerFlags |= static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::ServiceDomain);
                    pSQAnswer->m_TTLServiceDomain.set(p_pPTRAnswer->m_u32TTL);
                    //pSQAnswer->releaseServiceDomain();

                    bResult = pQuery->addAnswer(pSQAnswer);

                    DEBUG_EX_INFO(
                        DEBUG_OUTPUT.printf_P(PSTR("%s _processPTRAnswer: Added service domain to answer: "), _DH());
                        _printRRDomain(pSQAnswer->m_ServiceDomain);
                        DEBUG_OUTPUT.println();
                    );

                    p_rbFoundNewKeyAnswer = true;
                    _executeQueryCallback(*pQuery, *pSQAnswer, static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::ServiceDomain), true);
                }
            }
            pQuery = _findNextQueryByDomain(p_pPTRAnswer->m_Header.m_Domain, clsQuery::enuQueryType::Service, pQuery);
        }
    }   // else: No p_pPTRAnswer
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _processPTRAnswer: FAILED!\n"), _DH()););
    return bResult;
}

/*
    clsLEAmDNS2_Host::_processSRVAnswer (level 2)
*/
bool clsLEAMDNSHost::_processSRVAnswer(const clsLEAMDNSHost::clsRRAnswerSRV* p_pSRVAnswer,
                                       bool& p_rbFoundNewKeyAnswer)
{
    bool    bResult = false;

    if ((bResult = (0 != p_pSRVAnswer)))
    {
        // eg. MyESP._http._tcp.local SRV xxxx xx yy zz 5000 esp8266.local
        for (clsQuery::list::iterator it = m_Queries.begin(); ((bResult) && (it != m_Queries.end())); it++)
        {
            clsQuery* pQuery = *it;

            if (pQuery->m_bAwaitingAnswers)
            {
                clsQuery::clsAnswer* pSQAnswer = pQuery->findAnswerForServiceDomain(p_pSRVAnswer->m_Header.m_Domain);
                if (pSQAnswer)
                {
                    // Answer for this service domain (eg. MyESP._http._tcp.local) available
                    if (p_pSRVAnswer->m_u32TTL)
                    {
                        // First or update message (TTL != 0)
                        pSQAnswer->m_TTLHostDomainAndPort.set(p_pSRVAnswer->m_u32TTL);    // Update TTL tag
                        DEBUG_EX_INFO(
                            DEBUG_OUTPUT.printf_P(PSTR("%s _processSRVAnswer: Updated TTL(%lu) for "), _DH(), p_pSRVAnswer->m_u32TTL);
                            _printRRDomain(pSQAnswer->m_ServiceDomain);
                            DEBUG_OUTPUT.printf_P(PSTR(" host domain and port\n"));
                        );
                        // Host domain & Port
                        if ((pSQAnswer->m_HostDomain != p_pSRVAnswer->m_SRVDomain) ||
                                (pSQAnswer->m_u16Port != p_pSRVAnswer->m_u16Port))
                        {

                            pSQAnswer->m_HostDomain = p_pSRVAnswer->m_SRVDomain;
                            //pSQAnswer->releaseHostDomain();
                            pSQAnswer->m_u16Port = p_pSRVAnswer->m_u16Port;
                            pSQAnswer->m_QueryAnswerFlags |= (static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::HostDomain) | static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::Port));

                            DEBUG_EX_INFO(
                                DEBUG_OUTPUT.printf_P(PSTR("%s _processSVRAnswer: Added host domain and port to "), _DH());
                                _printRRDomain(pSQAnswer->m_ServiceDomain);
                                DEBUG_OUTPUT.printf_P(PSTR(": "));
                                _printRRDomain(pSQAnswer->m_HostDomain);
                                DEBUG_OUTPUT.printf_P(PSTR(": %u\n"), pSQAnswer->m_u16Port);
                            );

                            p_rbFoundNewKeyAnswer = true;
                            _executeQueryCallback(*pQuery, *pSQAnswer, (static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::HostDomain) | static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::Port)), true);
                        }
                    }
                    else
                    {
                        // Goodby message
                        pSQAnswer->m_TTLHostDomainAndPort.prepareDeletion();    // Prepare answer deletion according to RFC 6762, 10.1
                        DEBUG_EX_INFO(
                            DEBUG_OUTPUT.printf_P(PSTR("%s _processSRVAnswer: 'Goodbye' received for "), _DH());
                            _printRRDomain(pSQAnswer->m_ServiceDomain);
                            DEBUG_OUTPUT.printf_P(PSTR(" host domain and port\n"));
                        );
                    }
                }
            }   // m_bAwaitingAnswers
        }   // for(queries)
    }   // else: No p_pSRVAnswer
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _processSRVAnswer: FAILED!\n"), _DH()););
    return bResult;
}

/*
    clsLEAmDNS2_Host::_processTXTAnswer (level 2)
*/
bool clsLEAMDNSHost::_processTXTAnswer(const clsLEAMDNSHost::clsRRAnswerTXT* p_pTXTAnswer)
{
    bool    bResult = false;

    if ((bResult = (0 != p_pTXTAnswer)))
    {
        // eg. MyESP._http._tcp.local TXT xxxx xx c#=1
        for (clsQuery::list::iterator it = m_Queries.begin(); ((bResult) && (it != m_Queries.end())); it++)
        {
            clsQuery* pQuery = *it;

            if (pQuery->m_bAwaitingAnswers)
            {
                clsQuery::clsAnswer* pSQAnswer = pQuery->findAnswerForServiceDomain(p_pTXTAnswer->m_Header.m_Domain);
                if (pSQAnswer)
                {
                    // Answer for this service domain (eg. MyESP._http._tcp.local) available
                    if (p_pTXTAnswer->m_u32TTL)
                    {
                        // First or update message
                        pSQAnswer->m_TTLTxts.set(p_pTXTAnswer->m_u32TTL); // Update TTL tag
                        DEBUG_EX_INFO(
                            DEBUG_OUTPUT.printf_P(PSTR("%s _processTXTAnswer: Updated TTL(%lu) for "), _DH(), p_pTXTAnswer->m_u32TTL);
                            _printRRDomain(pSQAnswer->m_ServiceDomain);
                            DEBUG_OUTPUT.printf_P(PSTR(" TXTs\n"));
                        );
                        if (!pSQAnswer->m_Txts.compare(p_pTXTAnswer->m_Txts))
                        {
                            pSQAnswer->m_Txts = p_pTXTAnswer->m_Txts;
                            pSQAnswer->m_QueryAnswerFlags |= static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::Txts);
                            //pSQAnswer->releaseTxts();

                            DEBUG_EX_INFO(
                                DEBUG_OUTPUT.printf_P(PSTR("%s _processTXTAnswer: Added TXT to "), _DH());
                                _printRRDomain(pSQAnswer->m_ServiceDomain);
                                DEBUG_OUTPUT.println();
                            );

                            _executeQueryCallback(*pQuery, *pSQAnswer, static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::Txts), true);
                        }
                    }
                    else
                    {
                        // Goodby message
                        pSQAnswer->m_TTLTxts.prepareDeletion(); // Prepare answer deletion according to RFC 6762, 10.1
                        DEBUG_EX_INFO(
                            DEBUG_OUTPUT.printf_P(PSTR("%s _processTXTAnswer: 'Goodbye' received for "), _DH());
                            _printRRDomain(pSQAnswer->m_ServiceDomain);
                            DEBUG_OUTPUT.printf_P(PSTR(" TXTs\n"));
                        );
                    }
                }
            }   // m_bAwaitingAnswers
        }   // for(queries)
    }   // else: No p_pTXTAnswer
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _processTXTAnswer: FAILED!\n"), _DH()););
    return bResult;
}

#ifdef MDNS_IPV4_SUPPORT
/*
    clsLEAmDNS2_Host::_processAAnswer (level 3)
*/
bool clsLEAMDNSHost::_processAAnswer(const clsLEAMDNSHost::clsRRAnswerA* p_pAAnswer)
{
    bool    bResult = false;

    if ((bResult = (0 != p_pAAnswer)))
    {
        // eg. esp8266.local A xxxx xx 192.168.2.120
        for (clsQuery::list::iterator it = m_Queries.begin(); ((bResult) && (it != m_Queries.end())); it++)
        {
            clsQuery* pQuery = *it;

            if (pQuery->m_bAwaitingAnswers)
            {
                // Look for answers to host queries
                if ((p_pAAnswer->m_u32TTL) &&                                   // NOT just a goodbye message
                        (clsQuery::enuQueryType::Host == pQuery->m_QueryType) &&    // AND a host query
                        (pQuery->m_Domain == p_pAAnswer->m_Header.m_Domain))        // AND a matching host domain
                {
                    clsQuery::clsAnswer* pSQAnswer = pQuery->findAnswerForHostDomain(p_pAAnswer->m_Header.m_Domain);
                    if ((!pSQAnswer) &&
                            ((pSQAnswer = new clsQuery::clsAnswer)))
                    {
                        // Add not yet included answer
                        pSQAnswer->m_HostDomain = p_pAAnswer->m_Header.m_Domain;
                        //pSQAnswer->releaseHostDomain();

                        bResult = pQuery->addAnswer(pSQAnswer);
                        DEBUG_EX_INFO(
                            DEBUG_OUTPUT.printf_P(PSTR("%s _processAAnswer: Added host query answer for "), _DH());
                            _printRRDomain(pQuery->m_Domain);
                            DEBUG_OUTPUT.println();
                        );

                        pSQAnswer->m_QueryAnswerFlags |= static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::HostDomain);
                        _executeQueryCallback(*pQuery, *pSQAnswer, static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::HostDomain), true);
                    }
                }

                // Look for answers to service queries
                clsQuery::clsAnswer* pSQAnswer = pQuery->findAnswerForHostDomain(p_pAAnswer->m_Header.m_Domain);
                if (pSQAnswer)
                {
                    // Answer for this host domain (eg. esp8266.local) available
                    clsQuery::clsAnswer::clsIPAddressWithTTL*  pIPAddress = pSQAnswer->findIPv4Address(p_pAAnswer->m_IPAddress);
                    if (pIPAddress)
                    {
                        // Already known IPv4 address
                        if (p_pAAnswer->m_u32TTL)
                        {
                            // Valid TTL -> Update answers TTL
                            pIPAddress->m_TTL.set(p_pAAnswer->m_u32TTL);
                            DEBUG_EX_INFO(
                                DEBUG_OUTPUT.printf_P(PSTR("%s _processAAnswer: Updated TTL(%lu) for "), _DH(), p_pAAnswer->m_u32TTL);
                                _printRRDomain(pSQAnswer->m_ServiceDomain);
                                DEBUG_OUTPUT.printf_P(PSTR(" IPv4 address (%s)\n"), pIPAddress->m_IPAddress.toString().c_str());
                            );
                        }
                        else
                        {
                            // 'Goodbye' message for known IPv4 address
                            pIPAddress->m_TTL.prepareDeletion();	// Prepare answer deletion according to RFC 6762, 10.1
                            DEBUG_EX_INFO(
                                DEBUG_OUTPUT.printf_P(PSTR("%s _processAAnswer: 'Goodbye' received for "), _DH());
                                _printRRDomain(pSQAnswer->m_ServiceDomain);
                                DEBUG_OUTPUT.printf_P(PSTR(" IPv4 address (%s)\n"), pIPAddress->m_IPAddress.toString().c_str());
                            );
                        }
                    }
                    else
                    {
                        // Until now unknown IPv4 address -> Add (if the message isn't just a 'Goodbye' note)
                        if (p_pAAnswer->m_u32TTL)
                        {
                            // NOT just a 'Goodbye' message
                            pIPAddress = new clsQuery::clsAnswer::clsIPAddressWithTTL(p_pAAnswer->m_IPAddress, p_pAAnswer->m_u32TTL);
                            if ((pIPAddress) &&
                                    (pSQAnswer->addIPv4Address(pIPAddress)))
                            {
                                DEBUG_EX_INFO(
                                    DEBUG_OUTPUT.printf_P(PSTR("%s _processAAnswer: Added IPv4 address to "), _DH());
                                    _printRRDomain(pSQAnswer->m_ServiceDomain);
                                    DEBUG_OUTPUT.printf_P(PSTR(": %s\n"), pIPAddress->m_IPAddress.toString().c_str());
                                );

                                pSQAnswer->m_QueryAnswerFlags |= static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::IPv4Address);
                                _executeQueryCallback(*pQuery, *pSQAnswer, static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::IPv4Address), true);
                            }
                            else
                            {
                                DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _processAAnswer: FAILED to add IPv4 address (%s)!\n"), _DH(), p_pAAnswer->m_IPAddress.toString().c_str()););
                            }
                        }
                    }
                }
            }   // m_bAwaitingAnswers
        }   // for(queries)
    }   // else: No p_pAAnswer
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _processAAnswer: FAILED!\n"), _DH()););
    return bResult;
}
#endif

#ifdef MDNS_IPV6_SUPPORT
/*
    clsLEAmDNS2_Host::_processAAAAAnswer (level 3)
*/
bool clsLEAMDNSHost::_processAAAAAnswer(const clsLEAMDNSHost::clsRRAnswerAAAA* p_pAAAAAnswer)
{
    bool    bResult = false;

    if ((bResult = (0 != p_pAAAAAnswer)))
    {
        // eg. esp8266.local AAAA xxxx xx 0bf3::0c
        for (clsQuery::list::iterator it = m_Queries.begin(); ((bResult) && (it != m_Queries.end())); it++)
        {
            clsQuery* pQuery = *it;

            if (pQuery->m_bAwaitingAnswers)
            {
                // Look for answers to host queries
                if ((p_pAAAAAnswer->m_u32TTL) &&                                // NOT just a goodbye message
                        (clsQuery::enuQueryType::Host == pQuery->m_QueryType) &&    // AND a host query
                        (pQuery->m_Domain == p_pAAAAAnswer->m_Header.m_Domain))     // AND a matching host domain
                {
                    clsQuery::clsAnswer* pSQAnswer = pQuery->findAnswerForHostDomain(p_pAAAAAnswer->m_Header.m_Domain);
                    if ((!pSQAnswer) &&
                            ((pSQAnswer = new clsQuery::clsAnswer)))
                    {
                        // Add not yet included answer
                        pSQAnswer->m_HostDomain = p_pAAAAAnswer->m_Header.m_Domain;
                        //pSQAnswer->releaseHostDomain();

                        bResult = pQuery->addAnswer(pSQAnswer);
                        DEBUG_EX_INFO(
                            DEBUG_OUTPUT.printf_P(PSTR("%s _processAAAAAnswer: Added host query answer for "), _DH());
                            _printRRDomain(pQuery->m_Domain);
                            DEBUG_OUTPUT.println();
                        );

                        pSQAnswer->m_QueryAnswerFlags |= static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::HostDomain);
                        _executeQueryCallback(*pQuery, *pSQAnswer, static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::HostDomain), true);
                    }
                }

                // Look for answers to service queries
                clsQuery::clsAnswer* pSQAnswer = pQuery->findAnswerForHostDomain(p_pAAAAAnswer->m_Header.m_Domain);
                if (pSQAnswer)      // Answer for this host domain (eg. esp8266.local) available
                {
                    clsQuery::clsAnswer::clsIPAddressWithTTL*  pIPAddress = pSQAnswer->findIPv6Address(p_pAAAAAnswer->m_IPAddress);
                    if (pIPAddress)
                    {
                        // Already known IPv6 address
                        if (p_pAAAAAnswer->m_u32TTL)
                        {
                            // Valid TTL -> Update answers TTL
                            pIPAddress->m_TTL.set(p_pAAAAAnswer->m_u32TTL);
                            DEBUG_EX_INFO(
                                DEBUG_OUTPUT.printf_P(PSTR("%s _processAAAAAnswer: Updated TTL(%lu) for "), _DH(), p_pAAAAAnswer->m_u32TTL);
                                _printRRDomain(pSQAnswer->m_ServiceDomain);
                                DEBUG_OUTPUT.printf_P(PSTR(" IPv6 address (%s)\n"), pIPAddress->m_IPAddress.toString().c_str());
                            );
                        }
                        else
                        {
                            // 'Goodbye' message for known IPv6 address
                            pIPAddress->m_TTL.prepareDeletion();	// Prepare answer deletion according to RFC 6762, 10.1
                            DEBUG_EX_INFO(
                                DEBUG_OUTPUT.printf_P(PSTR("%s _processAAAAAnswer: 'Goodbye' received for "), _DH());
                                _printRRDomain(pSQAnswer->m_ServiceDomain);
                                DEBUG_OUTPUT.printf_P(PSTR(" IPv6 address (%s)\n"), pIPAddress->m_IPAddress.toString().c_str());
                            );
                        }
                    }
                    else
                    {
                        // Until now unknown IPv6 address -> Add (if the message isn't just a 'Goodbye' note)
                        if (p_pAAAAAnswer->m_u32TTL)
                        {
                            // NOT just a 'Goodbye' message
                            pIPAddress = new clsQuery::clsAnswer::clsIPAddressWithTTL(p_pAAAAAnswer->m_IPAddress, p_pAAAAAnswer->m_u32TTL);
                            if ((pIPAddress) &&
                                    (pSQAnswer->addIPv6Address(pIPAddress)))
                            {
                                DEBUG_EX_INFO(
                                    DEBUG_OUTPUT.printf_P(PSTR("%s _processAAAAAnswer: Added IPv6 address to "), _DH());
                                    _printRRDomain(pSQAnswer->m_ServiceDomain);
                                    DEBUG_OUTPUT.printf_P(PSTR(": %s\n"), pIPAddress->m_IPAddress.toString().c_str());
                                );

                                pSQAnswer->m_QueryAnswerFlags |= static_cast<uint32_t>(clsQuery::clsAnswer::enuQueryAnswerType::IPv6Address);
                                _executeQueryCallback(*pQuery, *pSQAnswer, static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::IPv6Address), true);
                            }
                            else
                            {
                                DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _processAAAAAnswer: FAILED to add IPv6 address (%s)!\n"), _DH(), p_pAAAAAnswer->m_IPAddress.toString().c_str()););
                            }
                        }
                    }
                }
            }   // m_bAwaitingAnswers
        }   // for(queries)
    }   // else: No p_pAAAAAnswer

    return bResult;
}
#endif


/*

    PROBING

*/

/*
    clsLEAmDNS2_Host::_updateProbeStatus

    Manages the (outgoing) probing process.
    - If probing has not been started yet (ProbingStatus_NotStarted), the initial delay (see RFC 6762) is determined and
     the process is started
    - After timeout (of initial or subsequential delay) a probe message is send out for three times. If the message has
     already been sent out three times, the probing has been successful and is finished.

    Conflict management is handled in '_parseResponse ff.'
    Tiebraking is handled in 'parseQuery ff.'
*/
bool clsLEAMDNSHost::_updateProbeStatus()
{
    bool    bResult = true;

    //
    // Probe host domain
    if ((clsProbeInformation_Base::enuProbingStatus::ReadyToStart == m_ProbeInformation.m_ProbingStatus))// &&       // Ready to get started AND
/*
    	((
#ifdef MDNS_IPV4_SUPPORT
                 _getResponderIPAddress(pNetIf, enuIPProtocolType::V4).isSet()                             // AND has IPv4 address
#else
                 true
#endif
             ) || (
#ifdef MDNS_IPV6_SUPPORT
                 _getResponderIPAddress(pNetIf, enuIPProtocolType::V6).isSet()                             // OR has IPv6 address
#else
                 true
#endif
             )))                // Has IP address
*/
    {
        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _updateProbeStatus: Starting host probing...\n"), _DH()););

        // First probe delay SHOULD be random 0-250 ms
        m_ProbeInformation.m_Timeout.reset(rand() % clsConsts::u32ProbeDelay);
        m_ProbeInformation.m_ProbingStatus = clsProbeInformation_Base::enuProbingStatus::InProgress;
    }
    else if ((clsProbeInformation_Base::enuProbingStatus::InProgress == m_ProbeInformation.m_ProbingStatus) &&  // Probing AND
             (m_ProbeInformation.m_Timeout.expired()))                                                          // Time for next probe
    {
        if (clsConsts::u32ProbeCount > m_ProbeInformation.m_u8SentCount)
        {
            // Send next probe
            if ((bResult = _sendHostProbe()))
            {
                DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _updateProbeStatus: Did sent host probe for '%s.local'\n\n"), _DH(), (m_pcHostName ? : "")););
                m_ProbeInformation.m_Timeout.reset(clsConsts::u32ProbeDelay);
                ++m_ProbeInformation.m_u8SentCount;
            }
        }
        else
        {
            // Probing finished
            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("\n%s _updateProbeStatus: Done host probing for '%s.local'.\n\n\n"), _DH(), (m_pcHostName ? : "")););
            m_ProbeInformation.m_ProbingStatus = clsProbeInformation_Base::enuProbingStatus::ReadyToAnnounce;
            m_ProbeInformation.m_Timeout.reset(esp8266::polledTimeout::oneShot::neverExpires);

            _callHostProbeResultCallback(true);

            // Prepare to announce host
            m_ProbeInformation.m_u8SentCount = 0;
            m_ProbeInformation.m_Timeout.reset(clsConsts::u32AnnounceDelay);
            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _updateProbeStatus: Prepared host announcing.\n\n"), _DH()););
        }
    }   // else: Probing already finished OR waiting for next time slot
    else if ((clsProbeInformation_Base::enuProbingStatus::ReadyToAnnounce == m_ProbeInformation.m_ProbingStatus) &&
             (m_ProbeInformation.m_Timeout.expired()))
    {
        if ((bResult = _announce(true, false)))
        {
            // Don't announce services here
            ++m_ProbeInformation.m_u8SentCount; // 1..

            if (clsConsts::u32AnnounceCount > m_ProbeInformation.m_u8SentCount)
            {
                m_ProbeInformation.m_Timeout.reset(clsConsts::u32AnnounceDelay * pow(2, (m_ProbeInformation.m_u8SentCount - 1))); // 2^(0..) -> 1, 2, 4, ...
                DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _updateProbeStatus: Announcing host '%s.local' (%lu).\n\n"), _DH(), (m_pcHostName ? : ""), m_ProbeInformation.m_u8SentCount););
            }
            else
            {
                m_ProbeInformation.m_ProbingStatus = clsProbeInformation_Base::enuProbingStatus::DoneFinally;
                m_ProbeInformation.m_Timeout.reset(esp8266::polledTimeout::oneShot::neverExpires);
                DEBUG_EX_INFO2(DEBUG_OUTPUT.printf_P(PSTR("%s _updateProbeStatus: Done host announcing for '%s.local'.\n"), _DH(), (m_pcHostName ? : "")););
                DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _updateProbeStatus: Done host announcing for '%s.local'.\n\n"), _DH(), (m_pcHostName ? : "")););
                //DEBUG_OUTPUT.printf_P(PSTR("%s _updateProbeStatus: Done host announcing for '%s.local'.\n"), _DH(), (m_pcHostName ? : ""));
            }
        }
    }

    //
    // Probe services
    for (clsService::list::iterator it = m_Services.begin(); ((bResult) && (it != m_Services.end())); it++)
    {
        clsService* pService = *it;

        if (clsProbeInformation_Base::enuProbingStatus::ReadyToStart == pService->m_ProbeInformation.m_ProbingStatus)
        {
            // Ready to get started
            pService->m_ProbeInformation.m_Timeout.reset(clsConsts::u32ProbeDelay);                         // More or equal than first probe for host domain
            pService->m_ProbeInformation.m_ProbingStatus = clsProbeInformation_Base::enuProbingStatus::InProgress;
        }
        else if ((clsProbeInformation_Base::enuProbingStatus::InProgress == pService->m_ProbeInformation.m_ProbingStatus) &&    // Probing AND
                 (pService->m_ProbeInformation.m_Timeout.expired()))                                                            // Time for next probe
        {
            if (clsConsts::u32ProbeCount > pService->m_ProbeInformation.m_u8SentCount)
            {
                // Send next probe
                if ((bResult = _sendServiceProbe(*pService)))
                {
                    DEBUG_EX_INFO2(DEBUG_OUTPUT.printf_P(PSTR("%s _updateProbeStatus: Did sent service probe for '%s' (%u)\n\n"), _DH(), _service2String(pService), (pService->m_ProbeInformation.m_u8SentCount + 1)););
                    pService->m_ProbeInformation.m_Timeout.reset(clsConsts::u32ProbeDelay);
                    ++pService->m_ProbeInformation.m_u8SentCount;
                }
            }
            else
            {
                // Probing finished
                DEBUG_EX_INFO2(DEBUG_OUTPUT.printf_P(PSTR("\n%s _updateProbeStatus: Done service probing '%s'\n\n\n"), _DH(), _service2String(pService)););
                pService->m_ProbeInformation.m_ProbingStatus = clsProbeInformation_Base::enuProbingStatus::ReadyToAnnounce;
                pService->m_ProbeInformation.m_Timeout.reset(esp8266::polledTimeout::oneShot::neverExpires);

                _callServiceProbeResultCallback(*pService, true);

                // Prepare to announce service
                pService->m_ProbeInformation.m_u8SentCount = 0;
                pService->m_ProbeInformation.m_Timeout.reset(clsConsts::u32AnnounceDelay);
                DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _updateProbeStatus: Prepared service announcing.\n\n"), _DH()););
            }
        }
        else if ((clsProbeInformation_Base::enuProbingStatus::ReadyToAnnounce == pService->m_ProbeInformation.m_ProbingStatus) &&
                 (pService->m_ProbeInformation.m_Timeout.expired()))
        {
            // Probing already finished OR waiting for next time slot
            if ((bResult = _announceService(*pService)))
            {
                // Announce service
                ++pService->m_ProbeInformation.m_u8SentCount;   // 1..

                if (clsConsts::u32AnnounceCount > pService->m_ProbeInformation.m_u8SentCount)
                {
                    pService->m_ProbeInformation.m_Timeout.reset(clsConsts::u32AnnounceDelay * pow(2, (pService->m_ProbeInformation.m_u8SentCount - 1))); // 2^(0..) -> 1, 2, 4, ...
                    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _updateProbeStatus: Announcing service '%s' (%lu)\n\n"), _DH(), _service2String(pService), pService->m_ProbeInformation.m_u8SentCount););
                }
                else
                {
                    pService->m_ProbeInformation.m_ProbingStatus = clsProbeInformation_Base::enuProbingStatus::DoneFinally;
                    pService->m_ProbeInformation.m_Timeout.reset(esp8266::polledTimeout::oneShot::neverExpires);
                    DEBUG_EX_INFO2(DEBUG_OUTPUT.printf_P(PSTR("%s _updateProbeStatus: Done service announcing for '%s'\n"), _DH(), _service2String(pService)););
                    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _updateProbeStatus: Done service announcing for '%s'\n\n"), _DH(), _service2String(pService)););
                    //DEBUG_OUTPUT.printf_P(PSTR("%s _updateProbeStatus: Done service announcing for '%s'\n"), _DH(), _service2String(pService));
                }
            }
        }
    }
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _updateProbeStatus: FAILED!\n\n"), _DH()););
    return bResult;
}

/*
    clsLEAmDNS2_Host::_resetProbeStatus

    Resets the probe status.
    If 'p_bRestart' is set, the status is set to ProbingStatus_NotStarted. Consequently,
    when running 'updateProbeStatus' (which is done in every '_update' loop), the probing
    process is restarted.

*/
bool clsLEAMDNSHost::_resetProbeStatus(bool p_bRestart /*= true*/)
{
    m_ProbeInformation.clear(false);
    m_ProbeInformation.m_ProbingStatus = (p_bRestart ? clsProbeInformation_Base::enuProbingStatus::ReadyToStart : clsProbeInformation_Base::enuProbingStatus::DoneFinally);

    for (clsService* pService : m_Services)
    {
        pService->m_ProbeInformation.clear(false);
        pService->m_ProbeInformation.m_ProbingStatus = (p_bRestart ? clsProbeInformation_Base::enuProbingStatus::ReadyToStart : clsProbeInformation_Base::enuProbingStatus::DoneFinally);
    }
    return true;
}

/*
    clsLEAmDNS2_Host::_hasProbesWaitingForAnswers

*/
bool clsLEAMDNSHost::_hasProbesWaitingForAnswers(void) const
{
    bool    bResult = ((clsProbeInformation_Base::enuProbingStatus::InProgress == m_ProbeInformation.m_ProbingStatus) &&	    // Probing
                       (0 < m_ProbeInformation.m_u8SentCount));                                                                 // And really probing

    for (clsService::list::const_iterator it = m_Services.cbegin(); ((!bResult) && (it != m_Services.cend())); it++)
    {
        clsService* pService = *it;

        bResult = ((clsProbeInformation_Base::enuProbingStatus::InProgress == pService->m_ProbeInformation.m_ProbingStatus) &&  // Probing
                   (0 < pService->m_ProbeInformation.m_u8SentCount));                                                           // And really probing
    }
    return bResult;
}

/*
    clsLEAmDNS2_Host::_sendHostProbe

    Asks (probes) in the local network for the planned host domain
    - (eg. esp8266.local)

    To allow 'tiebreaking' (see '_parseQuery'), the answers for these questions are delivered in
    the 'knwon answers' section of the query.
    Host domain:
    - A/AAAA (eg. esp8266.esp -> 192.168.2.120)

*/
bool clsLEAMDNSHost::_sendHostProbe()
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _sendHostProbe (%s.local, %lu)\n"), _DH(), m_pcHostName, millis()););

    bool    bResult = true;

    // Requests for host domain
    clsSendParameter    sendParameter;
    sendParameter.m_bCacheFlush = false;    // RFC 6762 10.2

    clsRRQuestion*  pNewRRQuestion = new clsRRQuestion;
    if (((bResult = (0 != pNewRRQuestion))) &&
            ((bResult = _buildDomainForHost(m_pcHostName, pNewRRQuestion->m_Header.m_Domain))))
    {
        //sendParameter.m_pQuestions->m_bUnicast = true;
        pNewRRQuestion->m_Header.m_Attributes.m_u16Type = DNS_RRTYPE_ANY;
        pNewRRQuestion->m_Header.m_Attributes.m_u16Class = (0x8000 | DNS_RRCLASS_IN);       // Unicast & INternet

        sendParameter.m_RRQuestions.push_back(pNewRRQuestion);

        // Add known answers
#ifdef MDNS_IPV4_SUPPORT
        sendParameter.m_u32HostReplyMask |= static_cast<uint32_t>(enuContentFlag::A);       // Add A answer
#endif
#ifdef MDNS_IPV6_SUPPORT
        sendParameter.m_u32HostReplyMask |= static_cast<uint32_t>(enuContentFlag::AAAA);    // Add AAAA answer
#endif
    }
    else
    {
        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _sendHostProbe: FAILED to create host question!\n"), _DH()););
        if (pNewRRQuestion)
        {
            delete pNewRRQuestion;
            pNewRRQuestion = 0;
        }
    }
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _sendHostProbe: FAILED!\n"), _DH()););
    return ((bResult) &&
            (_sendMessage(sendParameter)));
}

/*
    clsLEAmDNS2_Host::_sendServiceProbe

    Asks (probes) in the local network for the planned service instance domain
    - (eg. MyESP._http._tcp.local).

    To allow 'tiebreaking' (see '_parseQuery'), the answers for these questions are delivered in
    the 'knwon answers' section of the query.
    Service domain:
    - SRV (eg. MyESP._http._tcp.local -> 5000 esp8266.local)
    - PTR NAME (eg. _http._tcp.local -> MyESP._http._tcp.local) (TODO: Check if needed, maybe TXT is better)

*/
bool clsLEAMDNSHost::_sendServiceProbe(clsService& p_rService)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _sendServiceProbe (%s, %lu)\n"), _DH(), _service2String(&p_rService), millis()););

    bool    bResult = true;

    // Requests for service instance domain
    clsSendParameter    sendParameter;
    sendParameter.m_bCacheFlush = false;    // RFC 6762 10.2

    clsRRQuestion*  pNewRRQuestion = new clsRRQuestion;
    if (((bResult = (0 != pNewRRQuestion))) &&
            ((bResult = _buildDomainForService(p_rService, true, pNewRRQuestion->m_Header.m_Domain))))
    {
        pNewRRQuestion->m_bUnicast = true;
        pNewRRQuestion->m_Header.m_Attributes.m_u16Type = DNS_RRTYPE_ANY;
        pNewRRQuestion->m_Header.m_Attributes.m_u16Class = (0x8000 | DNS_RRCLASS_IN);   // Unicast & INternet

        sendParameter.m_RRQuestions.push_back(pNewRRQuestion);

        // Add known answers
        p_rService.m_u32ReplyMask = (static_cast<uint32_t>(enuContentFlag::SRV) | static_cast<uint32_t>(enuContentFlag::PTR_NAME));	// Add SRV and PTR NAME answers
    }
    else
    {
        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _sendServiceProbe: FAILED to create service question!\n"), _DH()););
        if (pNewRRQuestion)
        {
            delete pNewRRQuestion;
            pNewRRQuestion = 0;
        }
    }
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _sendServiceProbe: FAILED!\n"), _DH()););
    return ((bResult) &&
            (_sendMessage(sendParameter)));
}

/*
    clsLEAmDNS2_Host::_cancelProbingForHost

*/
bool clsLEAMDNSHost::_cancelProbingForHost(void)
{
    bool    bResult = false;

    m_ProbeInformation.clear(false);

    // Send host notification
    bResult = _callHostProbeResultCallback(false);

    for (clsService* pService : m_Services)
    {
        bResult = _cancelProbingForService(*pService);
    }
    return bResult;
}

/*
    clsLEAmDNS2_Host::_cancelProbingForService

*/
bool clsLEAMDNSHost::_cancelProbingForService(clsService& p_rService)
{
    p_rService.m_ProbeInformation.clear(false);

    // Send notification
    return _callServiceProbeResultCallback(p_rService, false);
}

/*
    clsLEAmDNS2_Host::_callHostProbeResultCallback

*/
bool clsLEAMDNSHost::_callHostProbeResultCallback(bool p_bResult)
{
    if (m_ProbeInformation.m_fnProbeResultCallback)
    {
        m_ProbeInformation.m_fnProbeResultCallback(*this, m_pcHostName, p_bResult);
    }
    else if (!p_bResult)
    {
        // Auto-Handle failure by changing the host name, use '-' as divider between base name and index
        indexHostName();
        DEBUG_EX_INFO2(DEBUG_OUTPUT.printf_P(PSTR("%s _callHostProbeResultCallback: Changed Host Name: %s\n"), _DH(), (m_pcHostName ? : "")));
    }
    return true;
}

/*
    clsLEAmDNS2_Host::_callServiceProbeResultCallback

*/
bool clsLEAMDNSHost::_callServiceProbeResultCallback(clsLEAMDNSHost::clsService& p_rService,
        bool p_bResult)
{
    if (p_rService.m_ProbeInformation.m_fnProbeResultCallback)
    {
        p_rService.m_ProbeInformation.m_fnProbeResultCallback(p_rService, p_rService.instanceName(), p_bResult);
    }
    else if (!p_bResult)
    {
        // Auto-Handle failure by changing the service name, use ' #' as divider between base name and index
        p_rService.indexInstanceName();
        DEBUG_EX_INFO2(DEBUG_OUTPUT.printf_P(PSTR("%s _callServiceProbeResultCallback: Changed Service Domain: %s\n"), _DH(), _service2String(&p_rService)));
    }
    return true;
}


/*

    ANNOUNCING

*/

/*
    clsLEAmDNS2_Host::_announce

    Announces the host domain:
    - A/AAAA (eg. esp8266.local -> 192.168.2.120)
    - PTR (eg. 192.168.2.120.in-addr.arpa -> esp8266.local)

    and all presented services:
    - PTR_TYPE (_services._dns-sd._udp.local -> _http._tcp.local)
    - PTR_NAME (eg. _http._tcp.local -> MyESP8266._http._tcp.local)
    - SRV (eg. MyESP8266._http._tcp.local -> 5000 esp8266.local)
    - TXT (eg. MyESP8266._http._tcp.local -> c#=1)

    Goodbye (Un-Announcing) for the host domain and all services is also handled here.
    Goodbye messages are created by setting the TTL for the answer to 0, this happens
    inside the '_writeXXXAnswer' procs via 'sendParameter.m_bUnannounce = true'

*/
bool clsLEAMDNSHost::_announce(bool p_bAnnounce,
                               bool p_bIncludeServices)
{
    bool    bResult = false;

    clsSendParameter    sendParameter;
    if (clsProbeInformation_Base::enuProbingStatus::ReadyToAnnounce == m_ProbeInformation.m_ProbingStatus)
    {
        bResult = true;

        sendParameter.m_Response = clsSendParameter::enuResponseType::Unsolicited;  // Announces are 'Unsolicited authorative responses'
        sendParameter.m_bAuthorative = true;
        sendParameter.m_bCacheFlush = true;                                             // RFC 6762 8.3
        sendParameter.m_bUnannounce = !p_bAnnounce;                                     // When unannouncing, the TTL is set to '0' while creating the answers

        // Announce host
        sendParameter.m_u32HostReplyMask = 0;
#ifdef MDNS_IPV4_SUPPORT
        sendParameter.m_u32HostReplyMask |= static_cast<uint32_t>(enuContentFlag::A);                   // A answer
        sendParameter.m_u32HostReplyMask |= static_cast<uint32_t>(enuContentFlag::PTR_IPv4);            // PTR_IPv4 answer
#endif
#ifdef MDNS_IPV6_SUPPORT
        sendParameter.m_u32HostReplyMask |= static_cast<uint32_t>(enuContentFlag::AAAA);                // AAAA answer
        sendParameter.m_u32HostReplyMask |= static_cast<uint32_t>(enuContentFlag::PTR_IPv6);            // PTR_IPv6 answer
#endif

        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _announce: Announcing host %s (content: %s)\n"), _DH(), m_pcHostName, _replyFlags2String(sendParameter.m_u32HostReplyMask)););

        if (p_bIncludeServices)
        {
            // Announce services (service type, name, SRV (location) and TXTs)
            for (clsService* pService : m_Services)
            {
                if (clsProbeInformation_Base::enuProbingStatus::ReadyToAnnounce == pService->m_ProbeInformation.m_ProbingStatus)
                {
                    pService->m_u32ReplyMask = (static_cast<uint32_t>(enuContentFlag::PTR_TYPE) |
                                                static_cast<uint32_t>(enuContentFlag::PTR_NAME) |
                                                static_cast<uint32_t>(enuContentFlag::SRV) |
                                                static_cast<uint32_t>(enuContentFlag::TXT));
                    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _announce: Announcing service '%s' (content %s)\n"), _DH(), _service2String(pService), _replyFlags2String(pService->m_u32ReplyMask)););
                }
            }
        }
    }
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _announce: FAILED!\n"), _DH()););
    return ((bResult) &&
            (_sendMessage(sendParameter)));
}

/*
    clsLEAmDNS2_Host::_announceService

*/
bool clsLEAMDNSHost::_announceService(clsLEAMDNSHost::clsService& p_rService,
                                      bool p_bAnnounce /*= true*/)
{
    bool    bResult = false;

    clsSendParameter    sendParameter;
    if (clsProbeInformation_Base::enuProbingStatus::ReadyToAnnounce == p_rService.m_ProbeInformation.m_ProbingStatus)
    {
        sendParameter.m_Response = clsSendParameter::enuResponseType::Unsolicited;  // Announces are 'Unsolicited authorative responses'
        sendParameter.m_bAuthorative = true;
        sendParameter.m_bUnannounce = !p_bAnnounce; // When unannouncing, the TTL is set to '0' while creating the answers

        // DON'T announce host
        sendParameter.m_u32HostReplyMask = 0;

        // Announce services (service type, name, SRV (location) and TXTs)
        p_rService.m_u32ReplyMask = (static_cast<uint32_t>(enuContentFlag::PTR_TYPE) |
                                     static_cast<uint32_t>(enuContentFlag::PTR_NAME) |
                                     static_cast<uint32_t>(enuContentFlag::SRV) |
                                     static_cast<uint32_t>(enuContentFlag::TXT));
        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _announceService: Announcing service '%s' (content: %s)\n"), _DH(), _service2String(&p_rService), _replyFlags2String(p_rService.m_u32ReplyMask)););

        bResult = true;
    }
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _announceService: FAILED!\n"), _DH()););
    return ((bResult) &&
            (_sendMessage(sendParameter)));
}


/*

    QUERY CACHE

*/

/*
    clsLEAmDNS2_Host::_checkQueryCache

    For any 'living' query (m_bAwaitingAnswers == true) all available answers (their components)
    are checked for topicality based on the stored reception time and the answers TTL.
    When the components TTL is outlasted by more than 80%, a new question is generated, to get updated information.
    When no update arrived (in time), the component is removed from the answer (cache).

*/
bool clsLEAMDNSHost::_checkQueryCache(netif* pNetIf)
{
    bool        bResult = true;

    DEBUG_EX_INFO(
        bool    printedInfo = false;
    );
    for (clsQuery::list::iterator itQ = m_Queries.begin(); ((bResult) && (itQ != m_Queries.end())); itQ++)
    {
        clsQuery* pQuery = *itQ;
        //
        // Resend dynamic queries, if not already done often enough
        if ((!pQuery->m_bStaticQuery) &&
                (pQuery->m_ResendTimeout.expired()))
        {
            if ((bResult = _sendQuery(pNetIf, *pQuery)))
            {
                // The re-query rate is increased to more than one hour (RFC 6762 5.2)
                ++pQuery->m_u8SentCount;
                uint32_t    u32NewDelay = (clsConsts::u32DynamicQueryResendDelay * pow(2, std::min((pQuery->m_u8SentCount - 1), 12)));
                DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _checkQueryCache: Next query in %u seconds!\n"), _DH(), (u32NewDelay)););
                pQuery->m_ResendTimeout.reset(u32NewDelay);
            }
            else
            {
                break;
            }
            DEBUG_EX_INFO(
                DEBUG_OUTPUT.printf_P(PSTR("%s _checkQueryCache: %s to resend query!\n"), _DH(), (bResult ? "Succeeded" : "FAILED"));
                printedInfo = true;
            );
        }

        //
        // Schedule updates for cached answers
        if (pQuery->m_bAwaitingAnswers)
        {
            clsQuery::clsAnswer::list   expiredAnswers;
            for (clsQuery::clsAnswer::list::iterator itQA = pQuery->m_Answers.begin(); ((bResult) && (itQA != pQuery->m_Answers.end())); itQA++)
            {
                clsQuery::clsAnswer*    pQAnswer = *itQA;

                // 1. level answer
                if ((bResult) &&
                        (pQAnswer->m_TTLServiceDomain.flagged()))
                {
                    if (!pQAnswer->m_TTLServiceDomain.finalTimeoutLevel())
                    {
                        bResult = ((_sendQuery(pNetIf, *pQuery)) &&
                                   (pQAnswer->m_TTLServiceDomain.restart()));
                        DEBUG_EX_INFO(
                            DEBUG_OUTPUT.printf_P(PSTR("%s _checkQueryCache: PTR update scheduled for "), _DH());
                            _printRRDomain(pQAnswer->m_ServiceDomain);
                            DEBUG_OUTPUT.printf_P(PSTR(" %s\n"), (bResult ? "OK" : "FAILURE"));
                            printedInfo = true;
                        );
                    }
                    else
                    {
                        // Timed out! -> Delete
                        _executeQueryCallback(*pQuery, *pQAnswer, static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::ServiceDomain), false);
                        DEBUG_EX_INFO(
                            DEBUG_OUTPUT.printf_P(PSTR("%s _checkQueryCache: Will remove PTR answer for "), _DH());
                            _printRRDomain(pQAnswer->m_ServiceDomain);
                            DEBUG_OUTPUT.printf_P(PSTR("\n"));
                            printedInfo = true;
                        );

                        expiredAnswers.push_back(pQAnswer);
                        continue;   // Don't use this answer anymore
                    }
                }   // ServiceDomain flagged

                // 2. level answers
                // HostDomain & Port (from SRV)
                if ((bResult) &&
                        (pQAnswer->m_TTLHostDomainAndPort.flagged()))
                {
                    if (!pQAnswer->m_TTLHostDomainAndPort.finalTimeoutLevel())
                    {
                        bResult = ((_sendQuery(pNetIf, pQAnswer->m_ServiceDomain, DNS_RRTYPE_SRV)) &&
                                   (pQAnswer->m_TTLHostDomainAndPort.restart()));
                        DEBUG_EX_INFO(
                            DEBUG_OUTPUT.printf_P(PSTR("%s _checkQueryCache: SRV update scheduled for "), _DH());
                            _printRRDomain(pQAnswer->m_ServiceDomain);
                            DEBUG_OUTPUT.printf_P(PSTR(" host domain and port %s\n"), (bResult ? "OK" : "FAILURE"));
                            printedInfo = true;
                        );
                    }
                    else
                    {
                        // Timed out! -> Delete
                        DEBUG_EX_INFO(
                            DEBUG_OUTPUT.printf_P(PSTR("%s _checkQueryCache: Will remove SRV answer for "), _DH());
                            _printRRDomain(pQAnswer->m_ServiceDomain);
                            DEBUG_OUTPUT.printf_P(PSTR(" host domain and port\n"));
                            printedInfo = true;
                        );
                        // Delete
                        pQAnswer->m_HostDomain.clear();
                        //pSQAnswer->releaseHostDomain();
                        pQAnswer->m_u16Port = 0;
                        pQAnswer->m_TTLHostDomainAndPort.set(0);
                        clsQuery::clsAnswer::typeQueryAnswerType queryAnswerContentFlags = (static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::HostDomain) | static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::Port));
                        // As the host domain is the base for the IPv4- and IPv6Address, remove these too
#ifdef MDNS_IPV4_SUPPORT
                        pQAnswer->releaseIPv4Addresses();
                        queryAnswerContentFlags |= static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::IPv4Address);
#endif
#ifdef MDNS_IPV6_SUPPORT
                        pQAnswer->releaseIPv6Addresses();
                        queryAnswerContentFlags |= static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::IPv6Address);
#endif

                        // Remove content flags for deleted answer parts
                        pQAnswer->m_QueryAnswerFlags &= ~queryAnswerContentFlags;
                        _executeQueryCallback(*pQuery, *pQAnswer, queryAnswerContentFlags, false);
                    }
                }   // HostDomainAndPort flagged

                // Txts (from TXT)
                if ((bResult) &&
                        (pQAnswer->m_TTLTxts.flagged()))
                {
                    if (!pQAnswer->m_TTLTxts.finalTimeoutLevel())
                    {
                        bResult = ((_sendQuery(pNetIf, pQAnswer->m_ServiceDomain, DNS_RRTYPE_TXT)) &&
                                   (pQAnswer->m_TTLTxts.restart()));
                        DEBUG_EX_INFO(
                            DEBUG_OUTPUT.printf_P(PSTR("%s _checkQueryCache: TXT update scheduled for "), _DH());
                            _printRRDomain(pQAnswer->m_ServiceDomain);
                            DEBUG_OUTPUT.printf_P(PSTR(" TXTs %s\n"), (bResult ? "OK" : "FAILURE"));
                            printedInfo = true;
                        );
                    }
                    else
                    {
                        // Timed out! -> Delete
                        DEBUG_EX_INFO(
                            DEBUG_OUTPUT.printf_P(PSTR("%s _checkQueryCache: Will remove TXT answer for "), _DH());
                            _printRRDomain(pQAnswer->m_ServiceDomain);
                            DEBUG_OUTPUT.printf_P(PSTR(" TXTs\n"));
                            printedInfo = true;
                        );
                        // Delete
                        pQAnswer->m_Txts.clear();
                        pQAnswer->m_TTLTxts.set(0);

                        // Remove content flags for deleted answer parts
                        pQAnswer->m_QueryAnswerFlags &= ~static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::Txts);
                        _executeQueryCallback(*pQuery, *pQAnswer, static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::Txts), false);
                    }
                }   // TXTs flagged

                // 3. level answers
#ifdef MDNS_IPV4_SUPPORT
                // IPv4Address (from A)
                clsQuery::clsAnswer::clsIPAddressWithTTL::list expiredIPv4Addresses;
                bool                                    bAUpdateQuerySent = false;
                for (clsQuery::clsAnswer::clsIPAddressWithTTL::list::iterator itQAIPv4 = pQAnswer->m_IPv4Addresses.begin(); ((bResult) && (itQAIPv4 != pQAnswer->m_IPv4Addresses.end())); itQAIPv4++)
                {
                    clsQuery::clsAnswer::clsIPAddressWithTTL*  pIPv4Address = *itQAIPv4;

                    if (pIPv4Address->m_TTL.flagged())
                    {
                        if (!pIPv4Address->m_TTL.finalTimeoutLevel())
                        {
                            // Needs update
                            if ((bAUpdateQuerySent) ||
                                    ((bResult = _sendQuery(pNetIf, pQAnswer->m_HostDomain, DNS_RRTYPE_A))))
                            {
                                pIPv4Address->m_TTL.restart();
                                bAUpdateQuerySent = true;

                                DEBUG_EX_INFO(
                                    DEBUG_OUTPUT.printf_P(PSTR("%s _checkQueryCache: IPv4 update scheduled for "), _DH());
                                    _printRRDomain(pQAnswer->m_ServiceDomain);
                                    DEBUG_OUTPUT.printf_P(PSTR(" IPv4 address (%s)\n"), (pIPv4Address->m_IPAddress.toString().c_str()));
                                    printedInfo = true;
                                );
                            }
                        }
                        else
                        {
                            // Timed out! -> Delete
                            DEBUG_EX_INFO(
                                DEBUG_OUTPUT.printf_P(PSTR("%s _checkQueryCache: Will remove IPv4 answer for "), _DH());
                                _printRRDomain(pQAnswer->m_ServiceDomain);
                                DEBUG_OUTPUT.printf_P(PSTR(" IPv4 address\n"));
                                printedInfo = true;
                            );
                            if (1 == pQAnswer->m_IPv4Addresses.size())
                            {
                                // NO IPv4 address left after this -> remove content flag
                                pQAnswer->m_QueryAnswerFlags &= ~static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::IPv4Address);
                            }
                            // Notify client
                            _executeQueryCallback(*pQuery, *pQAnswer, static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::IPv4Address), false);
                            expiredIPv4Addresses.push_back(pIPv4Address);
                        }
                    }   // IPv4 flagged
                }   // for
                // Finally remove expired IPv4 addresses
                for (clsQuery::clsAnswer::clsIPAddressWithTTL* pIPv4Address : expiredIPv4Addresses)
                {
                    pQAnswer->removeIPv4Address(pIPv4Address);
                }
#endif
#ifdef MDNS_IPV6_SUPPORT
                // IPv6Address (from AAAA)
                clsQuery::clsAnswer::clsIPAddressWithTTL::list expiredIPv6Addresses;
                bool                                    bAAAAUpdateQuerySent = false;
                for (clsQuery::clsAnswer::clsIPAddressWithTTL::list::iterator itQAIPv6 = pQAnswer->m_IPv6Addresses.begin(); ((bResult) && (itQAIPv6 != pQAnswer->m_IPv6Addresses.end())); itQAIPv6++)
                {
                    clsQuery::clsAnswer::clsIPAddressWithTTL*  pIPv6Address = *itQAIPv6;

                    if (pIPv6Address->m_TTL.flagged())
                    {
                        if (!pIPv6Address->m_TTL.finalTimeoutLevel())
                        {
                            // Needs update
                            if ((bAAAAUpdateQuerySent) ||
                                    ((bResult = _sendQuery(pNetIf, pQAnswer->m_HostDomain, DNS_RRTYPE_AAAA))))
                            {
                                pIPv6Address->m_TTL.restart();
                                bAAAAUpdateQuerySent = true;

                                DEBUG_EX_INFO(
                                    DEBUG_OUTPUT.printf_P(PSTR("%s _checkQueryCache: IPv6 update scheduled for "), _DH());
                                    _printRRDomain(pQAnswer->m_ServiceDomain);
                                    DEBUG_OUTPUT.printf_P(PSTR(" IPv6 address (%s)\n"), (pIPv6Address->m_IPAddress.toString().c_str()));
                                    printedInfo = true;
                                );
                            }
                        }
                        else
                        {
                            // Timed out! -> Delete
                            DEBUG_EX_INFO(
                                DEBUG_OUTPUT.printf_P(PSTR("%s _checkQueryCache: Will remove answer for "), _DH());
                                _printRRDomain(pQAnswer->m_ServiceDomain);
                                DEBUG_OUTPUT.printf_P(PSTR(" IPv6 address\n"));
                                printedInfo = true;
                            );
                            if (1 == pQAnswer->m_IPv6Addresses.size())
                            {
                                // NO IPv6 address left after this -> remove content flag
                                pQAnswer->m_QueryAnswerFlags &= ~static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::IPv6Address);
                            }
                            // Notify client
                            _executeQueryCallback(*pQuery, *pQAnswer, static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::IPv6Address), false);
                            expiredIPv6Addresses.push_back(pIPv6Address);
                        }
                    }   // IPv6 flagged
                    // Finally remove expired IPv6 addresses
                    for (clsQuery::clsAnswer::clsIPAddressWithTTL* pIPv6Address : expiredIPv6Addresses)
                    {
                        pQAnswer->removeIPv6Address(pIPv6Address);
                    }
                }   // while
#endif
            }

            // Finally remove expired answers
            for (clsQuery::clsAnswer* pAnswer : expiredAnswers)
            {
                pQuery->removeAnswer(pAnswer);
            }
        }
    }
    DEBUG_EX_INFO(if (printedInfo) DEBUG_OUTPUT.printf_P(PSTR("\n")););
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _checkQueryCache: FAILED!\n"), _DH()););
    return bResult;
}


/*
    clsLEAmDNS2_Host::_replyMaskForHost

    Determines the relavant host answers for the given question.
    - A question for the hostname (eg. esp8266.local) will result in an A/AAAA (eg. 192.168.2.129) reply.
    - A question for the reverse IP address (eg. 192-168.2.120.inarpa.arpa) will result in an PTR_IPv4 (eg. esp8266.local) reply.

    In addition, a full name match (question domain == host domain) is marked.
*/
uint32_t clsLEAMDNSHost::_replyMaskForHost(netif* pNetIf,
        const clsLEAMDNSHost::clsRRHeader& p_RRHeader,
        bool* p_pbFullNameMatch /*= 0*/) const
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _replyMaskForHost\n")););

    uint32_t    u32ReplyMask = 0;
    (p_pbFullNameMatch ? *p_pbFullNameMatch = false : 0);

    if ((DNS_RRCLASS_IN == (p_RRHeader.m_Attributes.m_u16Class & (~0x8000))) ||
            (DNS_RRCLASS_ANY == (p_RRHeader.m_Attributes.m_u16Class & (~0x8000))))
    {

        if ((DNS_RRTYPE_PTR == p_RRHeader.m_Attributes.m_u16Type) ||
                (DNS_RRTYPE_ANY == p_RRHeader.m_Attributes.m_u16Type))
        {
            // PTR request
#ifdef MDNS_IPV4_SUPPORT
            clsRRDomain    reverseIPv4Domain;
            if ((_getResponderIPAddress(pNetIf, enuIPProtocolType::V4).isSet()) &&
                    (_buildDomainForReverseIPv4(_getResponderIPAddress(pNetIf, enuIPProtocolType::V4), reverseIPv4Domain)) &&
                    (p_RRHeader.m_Domain == reverseIPv4Domain))
            {
                // Reverse domain match
                u32ReplyMask |= static_cast<uint32_t>(enuContentFlag::PTR_IPv4);
            }
#endif
#ifdef MDNS_IPV6_SUPPORT
            clsRRDomain    reverseIPv6Domain;
            if ((_getResponderIPAddress(pNetIf, enuIPProtocolType::V6).isSet()) &&
                    (_buildDomainForReverseIPv6(_getResponderIPAddress(pNetIf, enuIPProtocolType::V6), reverseIPv6Domain)) &&
                    (p_RRHeader.m_Domain == reverseIPv6Domain))
            {
                // Reverse domain match
                u32ReplyMask |= static_cast<uint32_t>(enuContentFlag::PTR_IPv6);
            }
#endif
        }   // Address qeuest

        clsRRDomain    hostDomain;
        if ((_buildDomainForHost(m_pcHostName, hostDomain)) &&
                (p_RRHeader.m_Domain == hostDomain))    // Host domain match
        {
            (p_pbFullNameMatch ? (*p_pbFullNameMatch = true) : (0));

#ifdef MDNS_IPV4_SUPPORT
            if ((DNS_RRTYPE_A == p_RRHeader.m_Attributes.m_u16Type) ||
                    (DNS_RRTYPE_ANY == p_RRHeader.m_Attributes.m_u16Type))
            {
                // IPv4 address request
                u32ReplyMask |= static_cast<uint32_t>(enuContentFlag::A);
            }
#endif
#ifdef MDNS_IPV6_SUPPORT
            if ((DNS_RRTYPE_AAAA == p_RRHeader.m_Attributes.m_u16Type) ||
                    (DNS_RRTYPE_ANY == p_RRHeader.m_Attributes.m_u16Type))
            {
                // IPv6 address request
                u32ReplyMask |= static_cast<uint32_t>(enuContentFlag::AAAA);
            }
#endif
        }
    }
    else
    {
        //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _replyMaskForHost: INVALID RR-class (0x%04X)!\n"), p_RRHeader.m_Attributes.m_u16Class););
    }
    DEBUG_EX_INFO(if (u32ReplyMask) DEBUG_OUTPUT.printf_P(PSTR("%s _replyMaskForHost: %s\n"), _DH(), _replyFlags2String(u32ReplyMask)););
    return u32ReplyMask;
}

/*
    clsLEAmDNS2_Host::_replyMaskForService

    Determines the relevant service answers for the given question
    - A PTR dns-sd service enum question (_services.dns-sd._udp.local) will result into an PTR_TYPE (eg. _http._tcp.local) answer
    - A PTR service type question (eg. _http._tcp.local) will result into an PTR_NAME (eg. MyESP._http._tcp.local) answer
    - A PTR service name question (eg. MyESP._http._tcp.local) will result into an PTR_NAME (eg. MyESP._http._tcp.local) answer
    - A SRV service name question (eg. MyESP._http._tcp.local) will result into an SRV (eg. 5000 MyESP.local) answer
    - A TXT service name question (eg. MyESP._http._tcp.local) will result into an TXT (eg. c#=1) answer

    In addition, a full name match (question domain == service instance domain) is marked.

*/
uint32_t clsLEAMDNSHost::_replyMaskForService(const clsLEAMDNSHost::clsRRHeader& p_RRHeader,
        clsLEAMDNSHost::clsService& p_rService,
        bool* p_pbFullNameMatch /*= 0*/)
{
    uint32_t    u32ReplyMask = 0;
    (p_pbFullNameMatch ? *p_pbFullNameMatch = false : 0);

    if ((DNS_RRCLASS_IN == (p_RRHeader.m_Attributes.m_u16Class & (~0x8000))) ||
            (DNS_RRCLASS_ANY == (p_RRHeader.m_Attributes.m_u16Class & (~0x8000))))
    {
        clsRRDomain    DNSSDDomain;
        if ((_buildDomainForDNSSD(DNSSDDomain)) &&                          // _services._dns-sd._udp.local
                (p_RRHeader.m_Domain == DNSSDDomain) &&
                ((DNS_RRTYPE_PTR == p_RRHeader.m_Attributes.m_u16Type) ||
                 (DNS_RRTYPE_ANY == p_RRHeader.m_Attributes.m_u16Type)))
        {
            // Common service info requested
            u32ReplyMask |= static_cast<uint32_t>(enuContentFlag::PTR_TYPE);
        }

        clsRRDomain    serviceDomain;
        if ((_buildDomainForService(p_rService, false, serviceDomain)) &&    // eg. _http._tcp.local
                (p_RRHeader.m_Domain == serviceDomain) &&
                ((DNS_RRTYPE_PTR == p_RRHeader.m_Attributes.m_u16Type) ||
                 (DNS_RRTYPE_ANY == p_RRHeader.m_Attributes.m_u16Type)))
        {
            // Special service info requested
            u32ReplyMask |= static_cast<uint32_t>(enuContentFlag::PTR_NAME);
        }

        if ((_buildDomainForService(p_rService, true, serviceDomain)) &&     // eg. MyESP._http._tcp.local
                (p_RRHeader.m_Domain == serviceDomain))
        {
            (p_pbFullNameMatch ? (*p_pbFullNameMatch = true) : (0));

            if ((DNS_RRTYPE_SRV == p_RRHeader.m_Attributes.m_u16Type) ||
                    (DNS_RRTYPE_ANY == p_RRHeader.m_Attributes.m_u16Type))
            {
                // Instance info SRV requested
                u32ReplyMask |= static_cast<uint32_t>(enuContentFlag::SRV);
            }
            if ((DNS_RRTYPE_TXT == p_RRHeader.m_Attributes.m_u16Type) ||
                    (DNS_RRTYPE_ANY == p_RRHeader.m_Attributes.m_u16Type))
            {
                // Instance info TXT requested
                u32ReplyMask |= static_cast<uint32_t>(enuContentFlag::TXT);
            }
        }
    }
    else
    {
        //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _replyMaskForService: INVALID RR-class (0x%04X)!\n"), p_RRHeader.m_Attributes.m_u16Class););
    }
    DEBUG_EX_INFO(if (u32ReplyMask) DEBUG_OUTPUT.printf_P(PSTR("%s _replyMaskForService(%s.%s.%s): %s\n"), _DH(), p_rService.m_pcInstanceName, p_rService.m_pcType, p_rService.m_pcProtocol, _replyFlags2String(u32ReplyMask)););
    return u32ReplyMask;
}


} // namespace MDNSImplementation


} // namespace esp8266



