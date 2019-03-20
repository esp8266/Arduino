/*
 * LEAmDNS_Control.cpp
 *
 *  License (MIT license):
 *    Permission is hereby granted, free of charge, to any person obtaining a copy
 *    of this software and associated documentation files (the "Software"), to deal
 *    in the Software without restriction, including without limitation the rights
 *    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *    copies of the Software, and to permit persons to whom the Software is
 *    furnished to do so, subject to the following conditions:
 *
 *    The above copyright notice and this permission notice shall be included in
 *    all copies or substantial portions of the Software.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 *    THE SOFTWARE.
 *
 */

#include <arch/cc.h>
#include <sys/time.h>
#include <HardwareSerial.h>
#include <IPAddress.h>
#include <lwip/ip_addr.h>
#include <WString.h>
#include <cstdint>

/*
 * ESP8266mDNS Control.cpp
 */

extern "C" {
    #include "user_interface.h"
}

#include "LEAmDNS_lwIPdefs.h"
#include "LEAmDNS_Priv.h"

namespace esp8266 {
/*
 * LEAmDNS
 */
namespace MDNSImplementation {

/**
 * CONTROL
 */


/**
 * MAINTENANCE
 */

/*
 * MDNSResponder::_process
 *
 * Run the MDNS process.
 * Is called, every time the UDPContext receives data AND
 * should be called in every 'loop' by calling 'MDNS::update()'.
 *
 */
bool MDNSResponder::_process(bool p_bUserContext) {
    
    bool    bResult = true;
    
    if (!p_bUserContext) {
        
        if ((m_pUDPContext) &&          // UDPContext available AND
            (m_pUDPContext->next())) {  // has content

            //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _update: Calling _parseMessage\n")););
            bResult = _parseMessage();
            //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parsePacket %s\n"), (bResult ? "succeeded" : "FAILED")););
        }
    }
    else {
        bResult = ((WiFi.isConnected()) &&          // Has connection?
                   (_updateProbeStatus()) &&        // Probing
                   (_checkServiceQueryCache()));    // Service query cache check
    }
    return bResult;
}

/*
 * MDNSResponder::_restart
 */
bool MDNSResponder::_restart(void) {
    
    return ((_resetProbeStatus(true)) &&    // Stop and restart probing
            (_allocUDPContext()));          // Restart UDP
}


/**
 * RECEIVING
 */

/*
 * MDNSResponder::_parseMessage
 */
bool MDNSResponder::_parseMessage(void) {
    DEBUG_EX_INFO(
            unsigned long   ulStartTime = millis();
            unsigned        uStartMemory = ESP.getFreeHeap();
            DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseMessage (Time: %lu ms, heap: %u bytes, from %s(%u), to %s(%u))\n"), ulStartTime, uStartMemory,
                                    IPAddress(m_pUDPContext->getRemoteAddress()).toString().c_str(), m_pUDPContext->getRemotePort(),
                                    IPAddress(m_pUDPContext->getDestAddress()).toString().c_str(), m_pUDPContext->getLocalPort());
    );
    //DEBUG_EX_INFO(_udpDump(););
    
    bool    bResult = false;
    
    stcMDNS_MsgHeader   header;
    if (_readMDNSMsgHeader(header)) {
        if (0 == header.m_4bOpcode) {   // A standard query
            if (header.m_1bQR) {        // Received a response -> answers to a query
                //DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseMessage: Reading answers: ID:%u, Q:%u, A:%u, NS:%u, AR:%u\n"), header.m_u16ID, header.m_u16QDCount, header.m_u16ANCount, header.m_u16NSCount, header.m_u16ARCount););
                bResult = _parseResponse(header);
            }
            else {                      // Received a query (Questions)
                //DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseMessage: Reading query: ID:%u, Q:%u, A:%u, NS:%u, AR:%u\n"), header.m_u16ID, header.m_u16QDCount, header.m_u16ANCount, header.m_u16NSCount, header.m_u16ARCount););
                bResult = _parseQuery(header);
            }
        }
        else {
            DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseMessage: Received UNEXPECTED opcode:%u. Ignoring message!\n"), header.m_4bOpcode););
            m_pUDPContext->flush();
        }
    }
    else {
        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseMessage: FAILED to read header\n")););
        m_pUDPContext->flush();
    }
    DEBUG_EX_INFO(
            unsigned    uFreeHeap = ESP.getFreeHeap();
            DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseMessage: Done (%s after %lu ms, ate %i bytes, remaining %u)\n\n"), (bResult ? "Succeeded" : "FAILED"), (millis() - ulStartTime), (uStartMemory - uFreeHeap), uFreeHeap);
    );
    return bResult;
}

/*
 * MDNSResponder::_parseQuery
 *
 * Queries are of interest in two cases:
 * 1. allow for tiebreaking while probing in the case of a race condition between two instances probing for
 *    the same name at the same time
 * 2. provide answers to questions for our host domain or any presented service
 *
 * When reading the questions, a set of (planned) responses is created, eg. a reverse PTR question for the host domain
 * gets an A (IP address) response, a PTR question for the _services._dns-sd domain gets a PTR (type) response for any
 * registered service, ...
 *
 * As any mDNS responder should be able to handle 'legacy' queries (from DNS clients), this case is handled here also.
 * Legacy queries have got only one (unicast) question and are directed to the local DNS port (not the multicast port).
 *
 * 1. 
 */
bool MDNSResponder::_parseQuery(const MDNSResponder::stcMDNS_MsgHeader& p_MsgHeader) {
    
    bool    bResult = true;

    stcMDNSSendParameter    sendParameter;
    uint8_t                 u8HostOrServiceReplies = 0;
    for (uint16_t qd=0; ((bResult) && (qd<p_MsgHeader.m_u16QDCount)); ++qd) {

        stcMDNS_RRQuestion  questionRR;
        if ((bResult = _readRRQuestion(questionRR))) {
            // Define host replies, BUT only answer queries after probing is done
            u8HostOrServiceReplies =
            sendParameter.m_u8HostReplyMask |= (((m_bPassivModeEnabled) ||
                                                 (ProbingStatus_Done == m_HostProbeInformation.m_ProbingStatus))
                                                ? _replyMaskForHost(questionRR.m_Header, 0)
                                                : 0);
            DEBUG_EX_INFO(if (u8HostOrServiceReplies) { DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: Host reply needed 0x%X\n"), u8HostOrServiceReplies); });

            // Check tiebreak need for host domain
            if (ProbingStatus_InProgress == m_HostProbeInformation.m_ProbingStatus) {
                bool    bFullNameMatch = false;
                if ((_replyMaskForHost(questionRR.m_Header, &bFullNameMatch)) &&
                    (bFullNameMatch)) {
                    // We're in 'probing' state and someone is asking for our host domain: this might be
                    // a race-condition: Two host with the same domain names try simutanously to probe their domains
                    // See: RFC 6762, 8.2 (Tiebraking)
                    // However, we're using a max. reduced approach for tiebreaking here: The higher IP-address wins!
                    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: Possible race-condition for host domain detected while probing.\n")););
                    Serial.printf_P(PSTR("[MDNSResponder] _parseQuery: Possible race-condition for host domain detected while probing.\n"));

                    m_HostProbeInformation.m_bTiebreakNeeded = true;
                }
            }

            // Define service replies
            for (stcMDNSService* pService=m_pServices; pService; pService=pService->m_pNext) {
                // Define service replies, BUT only answer queries after probing is done
                uint8_t u8ReplyMaskForQuestion = (((m_bPassivModeEnabled) ||
                                                   (ProbingStatus_Done == pService->m_ProbeInformation.m_ProbingStatus))
                                                    ? _replyMaskForService(questionRR.m_Header, *pService, 0)
                                                    : 0);
                u8HostOrServiceReplies |= (pService->m_u8ReplyMask |= u8ReplyMaskForQuestion);
                DEBUG_EX_INFO(if (u8ReplyMaskForQuestion) { DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: Service reply needed for (%s.%s.%s): 0x%X (%s)\n"), (pService->m_pcName ?: m_pcHostname), pService->m_pcService, pService->m_pcProtocol, u8ReplyMaskForQuestion, IPAddress(m_pUDPContext->getRemoteAddress()).toString().c_str()); } );

                // Check tiebreak need for service domain
                if (ProbingStatus_InProgress == pService->m_ProbeInformation.m_ProbingStatus) {
                    bool    bFullNameMatch = false;
                    if ((_replyMaskForService(questionRR.m_Header, *pService, &bFullNameMatch)) &&
                        (bFullNameMatch)) {
                        // We're in 'probing' state and someone is asking for this service domain: this might be
                        // a race-condition: Two services with the same domain names try simutanously to probe their domains
                        // See: RFC 6762, 8.2 (Tiebraking)
                        // However, we're using a max. reduced approach for tiebreaking here: The 'higher' SRV host wins!
                        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: Possible race-condition for service domain %s.%s.%s detected while probing.\n"), (pService->m_pcName ?: m_pcHostname), pService->m_pcService, pService->m_pcProtocol););
                        Serial.printf_P(PSTR("[MDNSResponder] _parseQuery: Possible race-condition for service domain %s.%s.%s detected while probing.\n"), (pService->m_pcName ?: m_pcHostname), pService->m_pcService, pService->m_pcProtocol);

                        pService->m_ProbeInformation.m_bTiebreakNeeded = true;
                    }
                }
            }

            // Handle unicast and legacy specialities
            // If only one question asks for unicast reply, the whole reply packet is send unicast
            if (((DNS_MQUERY_PORT != m_pUDPContext->getRemotePort()) ||     // Unicast (maybe legacy) query OR
                 (questionRR.m_bUnicast)) &&                                // Expressivly unicast query
                (!sendParameter.m_bUnicast)) {

                sendParameter.m_bUnicast = true;
                sendParameter.m_bCacheFlush = false;
                DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: Unicast response for %s!\n"), IPAddress(m_pUDPContext->getRemoteAddress()).toString().c_str()););
                
                if ((DNS_MQUERY_PORT != m_pUDPContext->getRemotePort()) &&  // Unicast (maybe legacy) query AND
                    (1 == p_MsgHeader.m_u16QDCount) &&                          // Only one question AND
                    ((sendParameter.m_u8HostReplyMask) ||                       //  Host replies OR
                     (u8HostOrServiceReplies))) {                               //  Host or service replies available
                    // We're a match for this legacy query, BUT
                    // make sure, that the query comes from a local host
                    ip_info IPInfo_Local;
                    ip_info IPInfo_Remote;
                    if (((IPInfo_Remote.ip.addr = m_pUDPContext->getRemoteAddress())) &&
                        (((wifi_get_ip_info(SOFTAP_IF, &IPInfo_Local)) &&
                          (ip4_addr_netcmp(&IPInfo_Remote.ip, &IPInfo_Local.ip, &IPInfo_Local.netmask))) ||  // Remote IP in SOFTAP's subnet OR
                         ((wifi_get_ip_info(STATION_IF, &IPInfo_Local)) &&
                          (ip4_addr_netcmp(&IPInfo_Remote.ip, &IPInfo_Local.ip, &IPInfo_Local.netmask))))) { // Remote IP in STATION's subnet

                        DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: Legacy query from local host %s!\n"), IPAddress(m_pUDPContext->getRemoteAddress()).toString().c_str()););

                        sendParameter.m_u16ID = p_MsgHeader.m_u16ID;
                        sendParameter.m_bLegacyQuery = true;
                        sendParameter.m_pQuestions = new stcMDNS_RRQuestion;
                        if ((bResult = (0 != sendParameter.m_pQuestions))) {
                            sendParameter.m_pQuestions->m_Header.m_Domain = questionRR.m_Header.m_Domain;
                            sendParameter.m_pQuestions->m_Header.m_Attributes.m_u16Type = questionRR.m_Header.m_Attributes.m_u16Type;
                            sendParameter.m_pQuestions->m_Header.m_Attributes.m_u16Class = questionRR.m_Header.m_Attributes.m_u16Class;
                        }
                        else {
                            DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: FAILED to add legacy question!\n")););
                        }
                    }
                    else {
                        DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: Legacy query from NON-LOCAL host!\n")););
                        bResult = false;
                    }
                }
            }
        }
        else {
            DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: FAILED to read question!\n")););
        }
    }   // for questions

    //DEBUG_EX_INFO(if (u8HostOrServiceReplies) { DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: Reply needed: %u (%s: %s->%s)\n"), u8HostOrServiceReplies, clsTimeSyncer::timestr(), IPAddress(m_pUDPContext->getRemoteAddress()).toString().c_str(), IPAddress(m_pUDPContext->getDestAddress()).toString().c_str()); } );

    // Handle known answers
    uint32_t    u32Answers = (p_MsgHeader.m_u16ANCount + p_MsgHeader.m_u16NSCount + p_MsgHeader.m_u16ARCount);
    DEBUG_EX_INFO(if ((u8HostOrServiceReplies) && (u32Answers)) { DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: Known answers(%u):\n"), u32Answers); } );

    for (uint32_t an=0; ((bResult) && (an<u32Answers)); ++an) {
        stcMDNS_RRAnswer*   pKnownRRAnswer = 0;
        if (((bResult = _readRRAnswer(pKnownRRAnswer))) &&
            (pKnownRRAnswer)) {

            if ((DNS_RRTYPE_ANY != pKnownRRAnswer->m_Header.m_Attributes.m_u16Type) &&      // No ANY type answer
                (DNS_RRCLASS_ANY != pKnownRRAnswer->m_Header.m_Attributes.m_u16Class)) {    // No ANY class answer
                
                // Find match between planned answer (sendParameter.m_u8HostReplyMask) and this 'known answer'
                uint8_t u8HostMatchMask = (sendParameter.m_u8HostReplyMask & _replyMaskForHost(pKnownRRAnswer->m_Header));
                if ((u8HostMatchMask) &&                                            // The RR in the known answer matches an RR we are planning to send, AND
                    ((MDNS_HOST_TTL / 2) <= pKnownRRAnswer->m_u32TTL)) {            // The TTL of the known answer is longer than half of the new host TTL (120s)
                    
                    // Compare contents
                    if (AnswerType_PTR == pKnownRRAnswer->answerType()) {
                        stcMDNS_RRDomain    hostDomain;
                        if ((_buildDomainForHost(m_pcHostname, hostDomain)) &&
                            (((stcMDNS_RRAnswerPTR*)pKnownRRAnswer)->m_PTRDomain == hostDomain)) {
                            // Host domain match
#ifdef MDNS_IP4_SUPPORT
                            if (u8HostMatchMask & ContentFlag_PTR_IP4) {
                                // IP4 PTR was asked for, but is already known -> skipping
                                DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: IP4 PTR already known... skipping!\n")););
                                sendParameter.m_u8HostReplyMask &= ~ContentFlag_PTR_IP4;
                            }
#endif
#ifdef MDNS_IP6_SUPPORT
                            if (u8HostMatchMask & ContentFlag_PTR_IP6) {
                                // IP6 PTR was asked for, but is already known -> skipping
                                DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: IP6 PTR already known... skipping!\n")););
                                sendParameter.m_u8HostReplyMask &= ~ContentFlag_PTR_IP6;
                            }
#endif
                        }
                    }
                    else if (u8HostMatchMask & ContentFlag_A) {
                        // IP4 address was asked for
#ifdef MDNS_IP4_SUPPORT
                        if ((AnswerType_A == pKnownRRAnswer->answerType()) &&
                            (((stcMDNS_RRAnswerA*)pKnownRRAnswer)->m_IPAddress == _getResponseMulticastInterface(SOFTAP_MODE | STATION_MODE))) {

                            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: IP4 address already known... skipping!\n")););
                            sendParameter.m_u8HostReplyMask &= ~ContentFlag_A;
                        }   // else: RData NOT IP4 length !!
#endif
                    }
                    else if (u8HostMatchMask & ContentFlag_AAAA) {
                        // IP6 address was asked for
#ifdef MDNS_IP6_SUPPORT
                        if ((AnswerType_AAAA == pAnswerRR->answerType()) &&
                            (((stcMDNS_RRAnswerAAAA*)pAnswerRR)->m_IPAddress == _getResponseMulticastInterface(SOFTAP_MODE | STATION_MODE))) {

                            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: IP6 address already known... skipping!\n")););
                            sendParameter.m_u8HostReplyMask &= ~ContentFlag_AAAA;
                        }   // else: RData NOT IP6 length !!
#endif
                    }
                }   // Host match /*and TTL*/

                //
                // Check host tiebreak possibility
                if (m_HostProbeInformation.m_bTiebreakNeeded) {
                    stcMDNS_RRDomain    hostDomain;
                    if ((_buildDomainForHost(m_pcHostname, hostDomain)) &&
                        (pKnownRRAnswer->m_Header.m_Domain == hostDomain)) {
                        // Host domain match
#ifdef MDNS_IP4_SUPPORT
                        if (AnswerType_A == pKnownRRAnswer->answerType()) {
                            IPAddress   localIPAddress(_getResponseMulticastInterface(SOFTAP_MODE | STATION_MODE));
                            if (((stcMDNS_RRAnswerA*)pKnownRRAnswer)->m_IPAddress == localIPAddress) {
                                // SAME IP address -> We've received an old message from ourselfs (same IP)
                                DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: Tiebreak (IP4) WON (was an old message)!\n")););
                                m_HostProbeInformation.m_bTiebreakNeeded = false;
                            }
                            else {
                                if ((uint32_t)(((stcMDNS_RRAnswerA*)pKnownRRAnswer)->m_IPAddress) > (uint32_t)localIPAddress) { // The OTHER IP is 'higher' -> LOST
                                    // LOST tiebreak
                                    DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: Tiebreak (IP4) LOST (lower)!\n")););
                                    _cancelProbingForHost();
                                    m_HostProbeInformation.m_bTiebreakNeeded = false;
                                }
                                else {  // WON tiebreak
                                    //TiebreakState = TiebreakState_Won;    // We received an 'old' message from ourselfs -> Just ignore
                                    DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: Tiebreak (IP4) WON (higher IP)!\n")););
                                    m_HostProbeInformation.m_bTiebreakNeeded = false;
                                }
                            }
                        }
#endif
#ifdef MDNS_IP6_SUPPORT
                        if (AnswerType_AAAA == pAnswerRR->answerType()) {
                            // TODO
                        }
#endif
                    }
                }   // Host tiebreak possibility

                // Check service answers
                for (stcMDNSService* pService=m_pServices; pService; pService=pService->m_pNext) {

                    uint8_t u8ServiceMatchMask = (pService->m_u8ReplyMask & _replyMaskForService(pKnownRRAnswer->m_Header, *pService));

                    if ((u8ServiceMatchMask) &&                                 // The RR in the known answer matches an RR we are planning to send, AND
                        ((MDNS_SERVICE_TTL / 2) <= pKnownRRAnswer->m_u32TTL)) { // The TTL of the known answer is longer than half of the new service TTL (4500s)

                         if (AnswerType_PTR == pKnownRRAnswer->answerType()) {
                            stcMDNS_RRDomain    serviceDomain;
                            if ((u8ServiceMatchMask & ContentFlag_PTR_TYPE) &&
                                (_buildDomainForService(*pService, false, serviceDomain)) &&
                                (serviceDomain == ((stcMDNS_RRAnswerPTR*)pKnownRRAnswer)->m_PTRDomain)) {
                                DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: Service type PTR already known... skipping!\n")););
                                pService->m_u8ReplyMask &= ~ContentFlag_PTR_TYPE;
                            }
                            if ((u8ServiceMatchMask & ContentFlag_PTR_NAME) &&
                                (_buildDomainForService(*pService, true, serviceDomain)) &&
                                (serviceDomain == ((stcMDNS_RRAnswerPTR*)pKnownRRAnswer)->m_PTRDomain)) {
                                DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: Service name PTR already known... skipping!\n")););
                                pService->m_u8ReplyMask &= ~ContentFlag_PTR_NAME;
                            }
                        }
                        else if (u8ServiceMatchMask & ContentFlag_SRV) {
                            DEBUG_EX_ERR(if (AnswerType_SRV != pKnownRRAnswer->answerType()) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: ERROR! INVALID answer type (SRV)!\n")););
                            stcMDNS_RRDomain    hostDomain;
                            if ((_buildDomainForHost(m_pcHostname, hostDomain)) &&
                                (hostDomain == ((stcMDNS_RRAnswerSRV*)pKnownRRAnswer)->m_SRVDomain)) {  // Host domain match

                                if ((MDNS_SRV_PRIORITY == ((stcMDNS_RRAnswerSRV*)pKnownRRAnswer)->m_u16Priority) &&
                                        (MDNS_SRV_WEIGHT == ((stcMDNS_RRAnswerSRV*)pKnownRRAnswer)->m_u16Weight) &&
                                        (pService->m_u16Port == ((stcMDNS_RRAnswerSRV*)pKnownRRAnswer)->m_u16Port)) {

                                    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: Service SRV answer already known... skipping!\n")););
                                    pService->m_u8ReplyMask &= ~ContentFlag_SRV;
                                }   // else: Small differences -> send update message
                            }
                        }
                        else if (u8ServiceMatchMask & ContentFlag_TXT) {
                            DEBUG_EX_ERR(if (AnswerType_TXT != pKnownRRAnswer->answerType()) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: ERROR! INVALID answer type (TXT)!\n")););
                            _collectServiceTxts(*pService);
                            if (pService->m_Txts == ((stcMDNS_RRAnswerTXT*)pKnownRRAnswer)->m_Txts) {
                                DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: Service TXT answer already known... skipping!\n")););
                                pService->m_u8ReplyMask &= ~ContentFlag_TXT;
                            }
                            _releaseTempServiceTxts(*pService);
                        }
                    }   // Service match and enough TTL

                    //
                    // Check service tiebreak possibility
                    if (pService->m_ProbeInformation.m_bTiebreakNeeded) {
                        stcMDNS_RRDomain    serviceDomain;
                        if ((_buildDomainForService(*pService, true, serviceDomain)) &&
                            (pKnownRRAnswer->m_Header.m_Domain == serviceDomain)) {
                            // Service domain match
                            if (AnswerType_SRV == pKnownRRAnswer->answerType()) {
                                stcMDNS_RRDomain    hostDomain;
                                if ((_buildDomainForHost(m_pcHostname, hostDomain)) &&
                                    (hostDomain == ((stcMDNS_RRAnswerSRV*)pKnownRRAnswer)->m_SRVDomain)) {  // Host domain match

                                    // We've received an old message from ourselfs (same SRV)
                                    DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: Tiebreak (SRV) won (was an old message)!\n")););
                                    pService->m_ProbeInformation.m_bTiebreakNeeded = false;
                                }
                                else {
                                    if (((stcMDNS_RRAnswerSRV*)pKnownRRAnswer)->m_SRVDomain > hostDomain) { // The OTHER domain is 'higher' -> LOST
                                        // LOST tiebreak
                                        DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: Tiebreak (SRV) LOST (lower)!\n")););
                                        _cancelProbingForService(*pService);
                                        pService->m_ProbeInformation.m_bTiebreakNeeded = false;
                                    }
                                    else {  // WON tiebreak
                                        //TiebreakState = TiebreakState_Won;    // We received an 'old' message from ourselfs -> Just ignore
                                        DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: Tiebreak (SRV) won (higher)!\n")););
                                        pService->m_ProbeInformation.m_bTiebreakNeeded = false;
                                    }
                                }
                            }
                        }
                    }   // service tiebreak possibility
                }   // for services
            }   // ANY answers
        }
        else {
            DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: FAILED to read known answer!\n")););
        }

        if (pKnownRRAnswer) {
            delete pKnownRRAnswer;
            pKnownRRAnswer = 0;
        }
    }   // for answers

    if (bResult) {
        // Check, if a reply is needed
        uint8_t u8ReplyNeeded = sendParameter.m_u8HostReplyMask;
        for (stcMDNSService* pService=m_pServices; pService; pService=pService->m_pNext) {
            u8ReplyNeeded |= pService->m_u8ReplyMask;
        }

        if (u8ReplyNeeded) {
            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: Sending answer(0x%X)...\n"), u8ReplyNeeded););

            sendParameter.m_bResponse = true;
            sendParameter.m_bAuthorative = true;

            bResult = _sendMDNSMessage(sendParameter);
        }
        DEBUG_EX_INFO(
            else {
                DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: No reply needed\n"));
            }
        );
    }
    else {
        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: Something FAILED!\n")););
        m_pUDPContext->flush();
    }

    //
    // Check and reset tiebreak-states
    if (m_HostProbeInformation.m_bTiebreakNeeded) {
        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: UNSOLVED tiebreak-need for host domain!\n")););
        m_HostProbeInformation.m_bTiebreakNeeded = false;
    }
    for (stcMDNSService* pService=m_pServices; pService; pService=pService->m_pNext) {
        if (pService->m_ProbeInformation.m_bTiebreakNeeded) {
            DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: UNSOLVED tiebreak-need for service domain (%s.%s.%s)\n"), (pService->m_pcName ?: m_pcHostname), pService->m_pcService, pService->m_pcProtocol););
            pService->m_ProbeInformation.m_bTiebreakNeeded = false;
        }
    }
    DEBUG_EX_ERR(if (!bResult) { DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseQuery: FAILED!\n")); });
    return bResult;
}

/*
 * MDNSResponder::_parseResponse
 *
 * Responses are of interest in two cases:
 * 1. find domain name conflicts while probing
 * 2. get answers to service queries
 *
 * In both cases any included questions are ignored
 *
 * 1. If any answer has a domain name similar to one of the domain names we're planning to use (and are probing for),
 *    then we've got a 'probing conflict'. The conflict has to be solved on our side of the conflict (eg. by
 *    setting a new hostname and restart probing). The callback 'm_fnProbeResultCallback' is called with
 *    'p_bProbeResult=false' in this case.
 *
 * 2. Service queries like '_http._tcp.local' will (if available) produce PTR, SRV, TXT and A/AAAA answers.
 *    All stored answers are pivoted by the service instance name (from the PTR record). Other answer parts,
 *    like host domain or IP address are than attached to this element.
 *    Any answer part carries a TTL, this is also stored (incl. the reception time); if the TTL is '0' the
 *    answer (part) is withdrawn by the sender and should be removed from any cache. RFC 6762, 10.1 proposes to
 *    set the caches TTL-value to 1 second in such a case and to delete the item only, if no update has
 *    has taken place in this second.
 *    Answer parts may arrive in 'unsorted' order, so they are grouped into three levels:
 *    Level 1: PRT - names the service instance (and is used as pivot), voids all other parts if is withdrawn or outdates
 *    Level 2: SRV - links the instance name to a host domain and port, voids A/AAAA parts if is withdrawn or outdates
 *             TXT - links the instance name to services TXTs
 *    Level 3: A/AAAA - links the host domain to an IP address
 */
bool MDNSResponder::_parseResponse(const MDNSResponder::stcMDNS_MsgHeader& p_MsgHeader) {
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseResponse\n")););
    //DEBUG_EX_INFO(_udpDump(););
    
    bool    bResult = false;
    
    // A response should be the result of a query or a probe
    if ((_hasServiceQueriesWaitingForAnswers()) ||          // Waiting for query answers OR
        (_hasProbesWaitingForAnswers())) {                  // Probe responses

       DEBUG_EX_INFO(
                DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseResponse: Received a response\n"));
                //_udpDump();
        );

        bResult = true;
        //
        // Ignore questions here
        stcMDNS_RRQuestion  dummyRRQ;
        for (uint16_t qd=0; ((bResult) && (qd<p_MsgHeader.m_u16QDCount)); ++qd) {
            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseResponse: Received a response containing a question... ignoring!\n")););
            bResult = _readRRQuestion(dummyRRQ);
        }   // for queries
        
        //
        // Read and collect answers
        stcMDNS_RRAnswer*   pCollectedRRAnswers = 0;
        uint32_t            u32NumberOfAnswerRRs = (p_MsgHeader.m_u16ANCount + p_MsgHeader.m_u16NSCount + p_MsgHeader.m_u16ARCount);
        for (uint32_t an=0; ((bResult) && (an<u32NumberOfAnswerRRs)); ++an) {
            stcMDNS_RRAnswer*   pRRAnswer = 0;
            if (((bResult = _readRRAnswer(pRRAnswer))) &&
                (pRRAnswer)) {
                //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseResponse: ADDING answer!\n")););
                pRRAnswer->m_pNext = pCollectedRRAnswers;
                pCollectedRRAnswers = pRRAnswer;
            }
            else {
                DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseResponse: FAILED to read answer!\n")););
                if (pRRAnswer) {
                    delete pRRAnswer;
                    pRRAnswer = 0;
                }
                bResult = false;
            }
        }   // for answers
        
        //
        // Process answers
        if (bResult) {
            bResult = ((!pCollectedRRAnswers) ||
                       (_processAnswers(pCollectedRRAnswers)));
        }
        else {  // Some failure while reading answers
            DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseResponse: FAILED to read answers!\n")););
            m_pUDPContext->flush();
        }
        
        // Delete collected answers
        while (pCollectedRRAnswers) {
            //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseResponse: DELETING answer!\n")););
            stcMDNS_RRAnswer*   pNextAnswer = pCollectedRRAnswers->m_pNext;
            delete pCollectedRRAnswers;
            pCollectedRRAnswers = pNextAnswer;
        }
    }
    else {  // Received an unexpected response -> ignore
        /*DEBUG_EX_INFO(
                DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseResponse: Received an unexpected response... ignoring!\nDUMP:\n"));
                bool    bDumpResult = true;
                for (uint16_t qd=0; ((bDumpResult) && (qd<p_MsgHeader.m_u16QDCount)); ++qd) {
                    stcMDNS_RRQuestion  questionRR;
                    bDumpResult = _readRRQuestion(questionRR);
                    esp_yield();
                }   // for questions
                // Handle known answers
                uint32_t    u32Answers = (p_MsgHeader.m_u16ANCount + p_MsgHeader.m_u16NSCount + p_MsgHeader.m_u16ARCount);
                for (uint32_t an=0; ((bDumpResult) && (an<u32Answers)); ++an) {
                    stcMDNS_RRAnswer*   pRRAnswer = 0;
                    bDumpResult = _readRRAnswer(pRRAnswer);
                    if (pRRAnswer) {
                        delete pRRAnswer;
                        pRRAnswer = 0;
                    }
                    esp_yield();
                }
        );*/
        m_pUDPContext->flush();
        bResult = true;
    }
    DEBUG_EX_ERR(if (!bResult) { DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _parseResponse: FAILED!\n")); });
    return bResult;
}

/*
 * MDNSResponder::_processAnswers
 * Host:
 *  A (0x01):               eg. esp8266.local A OP TTL 123.456.789.012
 *  AAAA (01Cx):            eg. esp8266.local AAAA OP TTL 1234:5678::90
 *  PTR (0x0C, IP4):        eg. 012.789.456.123.in-addr.arpa PTR OP TTL esp8266.local
 *  PTR (0x0C, IP6):        eg. 90.0.0.0.0.0.0.0.0.0.0.0.78.56.34.12.ip6.arpa PTR OP TTL esp8266.local
 * Service:
 *  PTR (0x0C, srv name):   eg. _http._tcp.local PTR OP TTL MyESP._http._tcp.local
 *  PTR (0x0C, srv type):   eg. _services._dns-sd._udp.local PTR OP TTL _http._tcp.local
 *  SRV (0x21):             eg. MyESP._http._tcp.local SRV OP TTL PRIORITY WEIGHT PORT esp8266.local
 *  TXT (0x10):             eg. MyESP._http._tcp.local TXT OP TTL c#=1
 *
 */
bool MDNSResponder::_processAnswers(const MDNSResponder::stcMDNS_RRAnswer* p_pAnswers) {

    bool    bResult = false;
    
    if (p_pAnswers) {
        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _processAnswers: Processing answers...\n")););
        bResult = true;
        
        // Answers may arrive in an unexpected order. So we loop our answers as long, as we
        // can connect new information to service queries
        bool    bFoundNewKeyAnswer;
        do {
            bFoundNewKeyAnswer = false;
            
            const stcMDNS_RRAnswer* pRRAnswer = p_pAnswers;
            while ((pRRAnswer) &&
                   (bResult)) {
                // 1. level answer (PTR)
                if (AnswerType_PTR == pRRAnswer->answerType()) {
                    // eg. _http._tcp.local PTR xxxx xx MyESP._http._tcp.local
                    bResult = _processPTRAnswer((stcMDNS_RRAnswerPTR*)pRRAnswer, bFoundNewKeyAnswer);   // May 'enable' new SRV or TXT answers to be linked to queries
                }
                // 2. level answers
                // SRV -> host domain and port
                else if (AnswerType_SRV == pRRAnswer->answerType()) {
                    // eg. MyESP_http._tcp.local SRV xxxx xx yy zz 5000 esp8266.local
                    bResult = _processSRVAnswer((stcMDNS_RRAnswerSRV*)pRRAnswer, bFoundNewKeyAnswer);   // May 'enable' new A/AAAA answers to be linked to queries
                }
                // TXT -> Txts
                else if (AnswerType_TXT == pRRAnswer->answerType()) {
                    // eg. MyESP_http._tcp.local TXT xxxx xx c#=1
                    bResult = _processTXTAnswer((stcMDNS_RRAnswerTXT*)pRRAnswer);
                }
                // 3. level answers
#ifdef MDNS_IP4_SUPPORT
                // A -> IP4Address
                else if (AnswerType_A == pRRAnswer->answerType()) {
                    // eg. esp8266.local A xxxx xx 192.168.2.120
                    bResult = _processAAnswer((stcMDNS_RRAnswerA*)pRRAnswer);
                }
#endif
#ifdef MDNS_IP6_SUPPORT
                // AAAA -> IP6Address
                else if (AnswerType_AAAA == pRRAnswer->answerType()) {
                    // eg. esp8266.local AAAA xxxx xx 09cf::0c
                    bResult = _processAAAAAnswer((stcMDNS_RRAnswerAAAA*)pRRAnswer);
                }
#endif

                // Finally check for probing conflicts
                // Host domain
                if ((ProbingStatus_InProgress == m_HostProbeInformation.m_ProbingStatus) &&
                    ((AnswerType_A == pRRAnswer->answerType()) ||
                     (AnswerType_AAAA == pRRAnswer->answerType()))) {

                    stcMDNS_RRDomain    hostDomain;
                    if ((_buildDomainForHost(m_pcHostname, hostDomain)) &&
                        (pRRAnswer->m_Header.m_Domain == hostDomain)) {

                        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _processAnswers: Probing CONFLICT found with: %s.local\n"), m_pcHostname););
                        _cancelProbingForHost();
                    }
                }
                // Service domains
                for (stcMDNSService* pService=m_pServices; pService; pService=pService->m_pNext) {
                    if ((ProbingStatus_InProgress == pService->m_ProbeInformation.m_ProbingStatus) &&
                        ((AnswerType_TXT == pRRAnswer->answerType()) ||
                         (AnswerType_SRV == pRRAnswer->answerType()))) {

                        stcMDNS_RRDomain    serviceDomain;
                        if ((_buildDomainForService(*pService, true, serviceDomain)) &&
                            (pRRAnswer->m_Header.m_Domain == serviceDomain)) {

                            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _processAnswers: Probing CONFLICT found with: %s.%s.%s\n"), (pService->m_pcName ?: m_pcHostname), pService->m_pcService, pService->m_pcProtocol););
                            _cancelProbingForService(*pService);
                        }
                    }
                }
                
                pRRAnswer = pRRAnswer->m_pNext; // Next collected answer
            }   // while (answers)
        } while ((bFoundNewKeyAnswer) &&
                 (bResult));
    }   // else: No answers provided
    DEBUG_EX_ERR(if (!bResult) { DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _processAnswers: FAILED!\n")); });
    return bResult;
}

/*
 * MDNSResponder::_processPTRAnswer
 */
bool MDNSResponder::_processPTRAnswer(const MDNSResponder::stcMDNS_RRAnswerPTR* p_pPTRAnswer,
                                      bool& p_rbFoundNewKeyAnswer) {
    
    bool    bResult = false;
    
    if ((bResult = (0 != p_pPTRAnswer))) {
        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _processPTRAnswer: Processing PTR answers...\n")););
        // eg. _http._tcp.local PTR xxxx xx MyESP._http._tcp.local
        // Check pending service queries for eg. '_http._tcp'
        
        stcMDNSServiceQuery*    pServiceQuery = _findNextServiceQueryByServiceType(p_pPTRAnswer->m_Header.m_Domain, 0);
        while (pServiceQuery) {
            if (pServiceQuery->m_bAwaitingAnswers) {
                // Find answer for service domain (eg. MyESP._http._tcp.local)
                stcMDNSServiceQuery::stcAnswer* pSQAnswer = pServiceQuery->findAnswerForServiceDomain(p_pPTRAnswer->m_PTRDomain);
                if (pSQAnswer) {    // existing answer
                    if (p_pPTRAnswer->m_u32TTL) {   // Received update message
                        pSQAnswer->m_TTLServiceDomain.set(p_pPTRAnswer->m_u32TTL);    // Update TTL tag
                        DEBUG_EX_INFO(
                                DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _processPTRAnswer: Updated TTL(%lu) for "), p_pPTRAnswer->m_u32TTL);
                                _printRRDomain(pSQAnswer->m_ServiceDomain);
                                DEBUG_OUTPUT.printf_P(PSTR("\n"));
                        );
                    }
                    else {                          // received goodbye-message
                        pSQAnswer->m_TTLServiceDomain.prepareDeletion();    // Prepare answer deletion according to RFC 6762, 10.1
                        DEBUG_EX_INFO(
                                DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _processPTRAnswer: 'Goodbye' received for "));
                                _printRRDomain(pSQAnswer->m_ServiceDomain);
                                DEBUG_OUTPUT.printf_P(PSTR("\n"));
                        );
                    }
                }
                else if ((p_pPTRAnswer->m_u32TTL) &&                                // Not just a goodbye-message
                         ((pSQAnswer = new stcMDNSServiceQuery::stcAnswer))) {      // Not yet included -> add answer
                    pSQAnswer->m_ServiceDomain = p_pPTRAnswer->m_PTRDomain;
                    pSQAnswer->m_u32ContentFlags |= ServiceQueryAnswerType_ServiceDomain;
                    pSQAnswer->m_TTLServiceDomain.set(p_pPTRAnswer->m_u32TTL);
                    pSQAnswer->releaseServiceDomain();
                    
                    bResult = pServiceQuery->addAnswer(pSQAnswer);
                    p_rbFoundNewKeyAnswer = true;
                    if (pServiceQuery->m_fnCallback) {
                    	MDNSServiceInfo serviceInfo(*this,(hMDNSServiceQuery)pServiceQuery, pServiceQuery->indexOfAnswer(pSQAnswer));
                        pServiceQuery->m_fnCallback(serviceInfo, static_cast<AnswerType>(ServiceQueryAnswerType_ServiceDomain), true);
                    }
                }
            }
            pServiceQuery = _findNextServiceQueryByServiceType(p_pPTRAnswer->m_Header.m_Domain, pServiceQuery);
        }
    }   // else: No p_pPTRAnswer
    DEBUG_EX_ERR(if (!bResult) { DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _processPTRAnswer: FAILED!\n")); });
    return bResult;
}

/*
 * MDNSResponder::_processSRVAnswer
 */
bool MDNSResponder::_processSRVAnswer(const MDNSResponder::stcMDNS_RRAnswerSRV* p_pSRVAnswer,
                                      bool& p_rbFoundNewKeyAnswer) {
    
    bool    bResult = false;
    
    if ((bResult = (0 != p_pSRVAnswer))) {
        // eg. MyESP._http._tcp.local SRV xxxx xx yy zz 5000 esp8266.local
        
        stcMDNSServiceQuery*    pServiceQuery = m_pServiceQueries;
        while (pServiceQuery) {
            stcMDNSServiceQuery::stcAnswer* pSQAnswer = pServiceQuery->findAnswerForServiceDomain(p_pSRVAnswer->m_Header.m_Domain);
            if (pSQAnswer) {    // Answer for this service domain (eg. MyESP._http._tcp.local) available
                if (p_pSRVAnswer->m_u32TTL) {   // First or update message (TTL != 0)
                    pSQAnswer->m_TTLHostDomainAndPort.set(p_pSRVAnswer->m_u32TTL);    // Update TTL tag
                    DEBUG_EX_INFO(
                            DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _processSRVAnswer: Updated TTL(%lu) for "), p_pSRVAnswer->m_u32TTL);
                            _printRRDomain(pSQAnswer->m_ServiceDomain);
                            DEBUG_OUTPUT.printf_P(PSTR(" host domain and port\n"));
                    );
                    // Host domain & Port
                    if ((pSQAnswer->m_HostDomain != p_pSRVAnswer->m_SRVDomain) ||
                        (pSQAnswer->m_u16Port != p_pSRVAnswer->m_u16Port)) {
                        
                        pSQAnswer->m_HostDomain = p_pSRVAnswer->m_SRVDomain;
                        pSQAnswer->releaseHostDomain();
                        pSQAnswer->m_u16Port = p_pSRVAnswer->m_u16Port;
                        pSQAnswer->m_u32ContentFlags |= ServiceQueryAnswerType_HostDomainAndPort;

                        p_rbFoundNewKeyAnswer = true;
                        if (pServiceQuery->m_fnCallback) {
                        	MDNSServiceInfo serviceInfo(*this, (hMDNSServiceQuery)pServiceQuery, pServiceQuery->indexOfAnswer(pSQAnswer));
                            pServiceQuery->m_fnCallback(serviceInfo, static_cast<AnswerType>(ServiceQueryAnswerType_HostDomainAndPort), true);
                        }
                    }
                }
                else {                      // Goodby message
                    pSQAnswer->m_TTLHostDomainAndPort.prepareDeletion();    // Prepare answer deletion according to RFC 6762, 10.1
                    DEBUG_EX_INFO(
                            DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _processSRVAnswer: 'Goodbye' received for "));
                            _printRRDomain(pSQAnswer->m_ServiceDomain);
                            DEBUG_OUTPUT.printf_P(PSTR(" host domain and port\n"));
                    );
                }
            }
            pServiceQuery = pServiceQuery->m_pNext;
        }   // while(service query)
    }   // else: No p_pSRVAnswer
    DEBUG_EX_ERR(if (!bResult) { DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _processSRVAnswer: FAILED!\n")); });
    return bResult;
}

/*
 * MDNSResponder::_processTXTAnswer
 */
bool MDNSResponder::_processTXTAnswer(const MDNSResponder::stcMDNS_RRAnswerTXT* p_pTXTAnswer) {
    
    bool    bResult = false;
    
    if ((bResult = (0 != p_pTXTAnswer))) {
        // eg. MyESP._http._tcp.local TXT xxxx xx c#=1
        
        stcMDNSServiceQuery*    pServiceQuery = m_pServiceQueries;
        while (pServiceQuery) {
            stcMDNSServiceQuery::stcAnswer* pSQAnswer = pServiceQuery->findAnswerForServiceDomain(p_pTXTAnswer->m_Header.m_Domain);
            if (pSQAnswer) {    // Answer for this service domain (eg. MyESP._http._tcp.local) available
                if (p_pTXTAnswer->m_u32TTL) {   // First or update message
                    pSQAnswer->m_TTLTxts.set(p_pTXTAnswer->m_u32TTL); // Update TTL tag
                    DEBUG_EX_INFO(
                            DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _processTXTAnswer: Updated TTL(%lu) for "), p_pTXTAnswer->m_u32TTL);
                            _printRRDomain(pSQAnswer->m_ServiceDomain);
                            DEBUG_OUTPUT.printf_P(PSTR(" TXTs\n"));
                    );
                    if (!pSQAnswer->m_Txts.compare(p_pTXTAnswer->m_Txts)) {
                        pSQAnswer->m_Txts = p_pTXTAnswer->m_Txts;
                        pSQAnswer->m_u32ContentFlags |= ServiceQueryAnswerType_Txts;
                        pSQAnswer->releaseTxts();
                        
                        if (pServiceQuery->m_fnCallback) {
                        	MDNSServiceInfo serviceInfo(*this, (hMDNSServiceQuery)pServiceQuery, pServiceQuery->indexOfAnswer(pSQAnswer));
                            pServiceQuery->m_fnCallback(serviceInfo , static_cast<AnswerType>(ServiceQueryAnswerType_Txts), true);
                        }
                    }
                }
                else {                      // Goodby message
                    pSQAnswer->m_TTLTxts.prepareDeletion(); // Prepare answer deletion according to RFC 6762, 10.1
                    DEBUG_EX_INFO(
                            DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _processTXTAnswer: 'Goodbye' received for "));
                            _printRRDomain(pSQAnswer->m_ServiceDomain);
                            DEBUG_OUTPUT.printf_P(PSTR(" TXTs\n"));
                    );
                }
            }
            pServiceQuery = pServiceQuery->m_pNext;
        }   // while(service query)
    }   // else: No p_pTXTAnswer
    DEBUG_EX_ERR(if (!bResult) { DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _processTXTAnswer: FAILED!\n")); });
    return bResult;
}

#ifdef MDNS_IP4_SUPPORT
    /*
     * MDNSResponder::_processAAnswer
     */
    bool MDNSResponder::_processAAnswer(const MDNSResponder::stcMDNS_RRAnswerA* p_pAAnswer) {
        
        bool    bResult = false;
        
        if ((bResult = (0 != p_pAAnswer))) {
            // eg. esp8266.local A xxxx xx 192.168.2.120
            
            stcMDNSServiceQuery*    pServiceQuery = m_pServiceQueries;
            while (pServiceQuery) {
                stcMDNSServiceQuery::stcAnswer* pSQAnswer = pServiceQuery->findAnswerForHostDomain(p_pAAnswer->m_Header.m_Domain);
                if (pSQAnswer) {    // Answer for this host domain (eg. esp8266.local) available
                    stcMDNSServiceQuery::stcAnswer::stcIP4Address*  pIP4Address = pSQAnswer->findIP4Address(p_pAAnswer->m_IPAddress);
                    if (pIP4Address) {
                        // Already known IP4 address
                        if (p_pAAnswer->m_u32TTL) { // Valid TTL -> Update answers TTL
                            pIP4Address->m_TTL.set(p_pAAnswer->m_u32TTL);
                            DEBUG_EX_INFO(
                                    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _processAAnswer: Updated TTL(%lu) for "), p_pAAnswer->m_u32TTL);
                                    _printRRDomain(pSQAnswer->m_ServiceDomain);
                                    DEBUG_OUTPUT.printf_P(PSTR(" IP4Address (%s)\n"), pIP4Address->m_IPAddress.toString().c_str());
                            );
                        }
                        else {                      // 'Goodbye' message for known IP4 address
                            pIP4Address->m_TTL.prepareDeletion();   // Prepare answer deletion according to RFC 6762, 10.1
                            DEBUG_EX_INFO(
                                    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _processAAnswer: 'Goodbye' received for "));
                                    _printRRDomain(pSQAnswer->m_ServiceDomain);
                                    DEBUG_OUTPUT.printf_P(PSTR(" IP4 address (%s)\n"), pIP4Address->m_IPAddress.toString().c_str());
                            );
                        }                                
                    }
                    else {
                        // Until now unknown IP4 address -> Add (if the message isn't just a 'Goodbye' note)
                        if (p_pAAnswer->m_u32TTL) { // NOT just a 'Goodbye' message
                            pIP4Address = new stcMDNSServiceQuery::stcAnswer::stcIP4Address(p_pAAnswer->m_IPAddress, p_pAAnswer->m_u32TTL);
                            if ((pIP4Address) &&
                                (pSQAnswer->addIP4Address(pIP4Address))) {
                                
                                pSQAnswer->m_u32ContentFlags |= ServiceQueryAnswerType_IP4Address;
                                if (pServiceQuery->m_fnCallback) {
                                	MDNSServiceInfo serviceInfo (*this, (hMDNSServiceQuery)pServiceQuery, pServiceQuery->indexOfAnswer(pSQAnswer));
                                    pServiceQuery->m_fnCallback(serviceInfo, static_cast<AnswerType>(ServiceQueryAnswerType_IP4Address), true);
                                }
                            }
                            else {   
                                DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _processAAnswer: FAILED to add IP4 address (%s)!\n"), p_pAAnswer->m_IPAddress.toString().c_str()););
                            }
                        }
                    }
                }
                pServiceQuery = pServiceQuery->m_pNext;
            }   // while(service query)         
        }   // else: No p_pAAnswer
        DEBUG_EX_ERR(if (!bResult) { DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _processAAnswer: FAILED!\n")); });
        return bResult;
    }
#endif

#ifdef MDNS_IP6_SUPPORT
    /*
     * MDNSResponder::_processAAAAAnswer
     */
    bool MDNSResponder::_processAAAAAnswer(const MDNSResponder::stcMDNS_RRAnswerAAAA* p_pAAAAAnswer) {
        
        bool    bResult = false;
        
        if ((bResult = (0 != p_pAAAAAnswer))) {
            // eg. esp8266.local AAAA xxxx xx 0bf3::0c
            
            stcMDNSServiceQuery*    pServiceQuery = m_pServiceQueries;
            while (pServiceQuery) {
                stcMDNSServiceQuery::stcAnswer* pSQAnswer = pServiceQuery->findAnswerForHostDomain(p_pAAAAAnswer->m_Header.m_Domain);
                if (pSQAnswer) {    // Answer for this host domain (eg. esp8266.local) available
                    stcIP6Address*  pIP6Address = pSQAnswer->findIP6Address(p_pAAAAAnswer->m_IPAddress);
                    if (pIP6Address) {
                        // Already known IP6 address
                        if (p_pAAAAAnswer->m_u32TTL) { // Valid TTL -> Update answers TTL
                            pIP6Address->m_TTL.set(p_pAAAAAnswer->m_u32TTL);
                            DEBUG_EX_INFO(
                                    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _processAAnswer: Updated TTL(%lu) for "), p_pAAAAAnswer->m_u32TTL);
                                    _printRRDomain(pSQAnswer->m_ServiceDomain);
                                    DEBUG_OUTPUT.printf_P(PSTR(" IP6 address (%s)\n"), pIP6Address->m_IPAddress.toString().c_str());
                            );
                        }
                        else {                      // 'Goodbye' message for known IP6 address
                            pIP6Address->m_TTL.prepareDeletion();   // Prepare answer deletion according to RFC 6762, 10.1
                            DEBUG_EX_INFO(
                                    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _processAAnswer: 'Goodbye' received for "));
                                    _printRRDomain(pSQAnswer->m_ServiceDomain);
                                    DEBUG_OUTPUT.printf_P(PSTR(" IP6 address (%s)\n"), pIP6Address->m_IPAddress.toString().c_str());
                            );
                        }                                
                    }
                    else {
                        // Until now unknown IP6 address -> Add (if the message isn't just a 'Goodbye' note)
                        if (p_pAAAAAnswer->m_u32TTL) { // NOT just a 'Goodbye' message
                            pIP6Address = new stcIP6Address(p_pAAAAAnswer->m_IPAddress, p_pAAAAAnswer->m_u32TTL);
                            if ((pIP6Address) &&
                                (pSQAnswer->addIP6Address(pIP6Address))) {
                                
                                pSQAnswer->m_u32ContentFlags |= ServiceQueryAnswerType_IP6Address;

                                if (pServiceQuery->m_fnCallback) {
                                    pServiceQuery->m_fnCallback(this, (hMDNSServiceQuery)pServiceQuery, pServiceQuery->indexOfAnswer(pSQAnswer), ServiceQueryAnswerType_IP6Address, true, pServiceQuery->m_pUserdata);
                                }
                            }
                            else {   
                                DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _processAAnswer: FAILED to add IP6 address (%s)!\n"), p_pAAAAAnswer->m_IPAddress.toString().c_str()););
                            }
                        }
                    }
                }
                pServiceQuery = pServiceQuery->m_pNext;
            }   // while(service query)
        }   // else: No p_pAAAAAnswer
        
        return bResult;
    }
#endif


/*
 * PROBING
 */

/*
 * MDNSResponder::_updateProbeStatus
 *
 * Manages the (outgoing) probing process.
 * - If probing has not been started yet (ProbingStatus_NotStarted), the initial delay (see RFC 6762) is determined and
 *   the process is started
 * - After timeout (of initial or subsequential delay) a probe message is send out for three times. If the message has
 *   already been sent out three times, the probing has been successful and is finished.
 *
 * Conflict management is handled in '_parseResponse ff.'
 * Tiebraking is handled in 'parseQuery ff.'
 */
bool MDNSResponder::_updateProbeStatus(void) {

    bool    bResult = true;
    
    //
    // Probe host domain
    if ((ProbingStatus_ReadyToStart == m_HostProbeInformation.m_ProbingStatus) &&                   // Ready to get started AND
        //TODO: Fix the following to allow Ethernet shield or other interfaces
        (_getResponseMulticastInterface(SOFTAP_MODE | STATION_MODE) != IPAddress())) {              // Has IP address
        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _updateProbeStatus: Starting host probing...\n")););

        // First probe delay SHOULD be random 0-250 ms
        m_HostProbeInformation.m_Timeout.reset(rand() % MDNS_PROBE_DELAY);
        m_HostProbeInformation.m_ProbingStatus = ProbingStatus_InProgress;
    }
    else if ((ProbingStatus_InProgress == m_HostProbeInformation.m_ProbingStatus) &&                // Probing AND
             (m_HostProbeInformation.m_Timeout.expired())) {                                        // Time for next probe

        if (MDNS_PROBE_COUNT > m_HostProbeInformation.m_u8SentCount) {                              // Send next probe
            if ((bResult = _sendHostProbe())) {
                DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _updateProbeStatus: Did sent host probe\n\n")););
                m_HostProbeInformation.m_Timeout.reset(MDNS_PROBE_DELAY);
                ++m_HostProbeInformation.m_u8SentCount;
            }
        }
        else {                                                                                      // Probing finished
            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _updateProbeStatus: Done host probing.\n")););
            m_HostProbeInformation.m_ProbingStatus = ProbingStatus_Done;
            m_HostProbeInformation.m_Timeout.reset(esp8266::polledTimeout::oneShotMs::neverExpires);
            if (m_HostProbeInformation.m_fnHostProbeResultCallback) {
                m_HostProbeInformation.m_fnHostProbeResultCallback(m_pcHostname, true);
            }

            // Prepare to announce host
            m_HostProbeInformation.m_u8SentCount = 0;
            m_HostProbeInformation.m_Timeout.reset(MDNS_ANNOUNCE_DELAY);
            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _updateProbeStatus: Prepared host announcing.\n\n")););
        }
    }   // else: Probing already finished OR waiting for next time slot
    else if ((ProbingStatus_Done == m_HostProbeInformation.m_ProbingStatus) &&
             (m_HostProbeInformation.m_Timeout.expired())) {

        if ((bResult = _announce(true, false))) {   // Don't announce services here
            ++m_HostProbeInformation.m_u8SentCount;

            if (MDNS_ANNOUNCE_COUNT > m_HostProbeInformation.m_u8SentCount) {
                m_HostProbeInformation.m_Timeout.reset(MDNS_ANNOUNCE_DELAY);
                DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _updateProbeStatus: Announcing host (%lu).\n\n"), m_HostProbeInformation.m_u8SentCount););
            }
            else {
                m_HostProbeInformation.m_Timeout.reset(esp8266::polledTimeout::oneShotMs::neverExpires);
                DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _updateProbeStatus: Done host announcing.\n\n")););
            }
        }
    }

    //
    // Probe services
    for (stcMDNSService* pService=m_pServices; ((bResult) && (pService)); pService=pService->m_pNext) {
        if (ProbingStatus_ReadyToStart == pService->m_ProbeInformation.m_ProbingStatus) {       // Ready to get started

            pService->m_ProbeInformation.m_Timeout.reset(MDNS_PROBE_DELAY);                     // More or equal than first probe for host domain
            pService->m_ProbeInformation.m_ProbingStatus = ProbingStatus_InProgress;
        }
        else if ((ProbingStatus_InProgress == pService->m_ProbeInformation.m_ProbingStatus) &&  // Probing AND
                 (pService->m_ProbeInformation.m_Timeout.expired())) {             // Time for next probe

            if (MDNS_PROBE_COUNT > pService->m_ProbeInformation.m_u8SentCount) {                // Send next probe
                if ((bResult = _sendServiceProbe(*pService))) {
                    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _updateProbeStatus: Did sent service probe (%u)\n\n"), (pService->m_ProbeInformation.m_u8SentCount + 1)););
                    pService->m_ProbeInformation.m_Timeout.reset(MDNS_PROBE_DELAY);
                    ++pService->m_ProbeInformation.m_u8SentCount;
                }
            }
            else {                                                                                      // Probing finished
                DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _updateProbeStatus: Done service probing %s.%s.%s\n\n"), (pService->m_pcName ?: m_pcHostname), pService->m_pcService, pService->m_pcProtocol););
                pService->m_ProbeInformation.m_ProbingStatus = ProbingStatus_Done;
                pService->m_ProbeInformation.m_Timeout.reset(esp8266::polledTimeout::oneShotMs::neverExpires);
                if (pService->m_ProbeInformation.m_fnServiceProbeResultCallback) {
                	pService->m_ProbeInformation.m_fnServiceProbeResultCallback(pService->m_pcName, pService, true);
                }
                // Prepare to announce service
                pService->m_ProbeInformation.m_u8SentCount = 0;
                pService->m_ProbeInformation.m_Timeout.reset(MDNS_ANNOUNCE_DELAY);
                DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _updateProbeStatus: Prepared service announcing.\n\n")););
            }
        }   // else: Probing already finished OR waiting for next time slot
        else if ((ProbingStatus_Done == pService->m_ProbeInformation.m_ProbingStatus) &&
                 (pService->m_ProbeInformation.m_Timeout.expired())) {

            if ((bResult = _announceService(*pService))) {   // Announce service
                ++pService->m_ProbeInformation.m_u8SentCount;

                if (MDNS_ANNOUNCE_COUNT > pService->m_ProbeInformation.m_u8SentCount) {
                    pService->m_ProbeInformation.m_Timeout.reset(MDNS_ANNOUNCE_DELAY);
                    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _updateProbeStatus: Announcing service %s.%s.%s (%lu)\n\n"), (pService->m_pcName ?: m_pcHostname), pService->m_pcService, pService->m_pcProtocol, pService->m_ProbeInformation.m_u8SentCount););
                }
                else {
                    pService->m_ProbeInformation.m_Timeout.reset(esp8266::polledTimeout::oneShotMs::neverExpires);
                    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _updateProbeStatus: Done service announcing for %s.%s.%s\n\n"), (pService->m_pcName ?: m_pcHostname), pService->m_pcService, pService->m_pcProtocol););
                }
            }
        }
    }
    DEBUG_EX_ERR(if (!bResult) { DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _updateProbeStatus: FAILED!\n\n")); });
    return bResult;
}

/*
 * MDNSResponder::_resetProbeStatus
 *
 * Resets the probe status.
 * If 'p_bRestart' is set, the status is set to ProbingStatus_NotStarted. Consequently,
 * when running 'updateProbeStatus' (which is done in every '_update' loop), the probing
 * process is restarted.
 */
bool MDNSResponder::_resetProbeStatus(bool p_bRestart /*= true*/) {
    
    m_HostProbeInformation.clear(false);
    m_HostProbeInformation.m_ProbingStatus = (p_bRestart ? ProbingStatus_ReadyToStart : ProbingStatus_Done);
    
    for (stcMDNSService* pService=m_pServices; pService; pService=pService->m_pNext) {
        pService->m_ProbeInformation.clear(false);
        pService->m_ProbeInformation.m_ProbingStatus = (p_bRestart ? ProbingStatus_ReadyToStart : ProbingStatus_Done);
    }
    return true;
}

/*
 * MDNSResponder::_hasProbesWaitingForAnswers
 */
bool MDNSResponder::_hasProbesWaitingForAnswers(void) const {

    bool    bResult = ((ProbingStatus_InProgress == m_HostProbeInformation.m_ProbingStatus) &&      // Probing
                       (0 < m_HostProbeInformation.m_u8SentCount));                                 // And really probing

    for (stcMDNSService* pService=m_pServices; ((!bResult) && (pService)); pService=pService->m_pNext) {
        bResult = ((ProbingStatus_InProgress == pService->m_ProbeInformation.m_ProbingStatus) &&    // Probing
                   (0 < pService->m_ProbeInformation.m_u8SentCount));                               // And really probing
    }
    return bResult;
}

/*
 * MDNSResponder::_sendHostProbe
 *
 * Asks (probes) in the local network for the planned host domain
 * - (eg. esp8266.local)
 *
 * To allow 'tiebreaking' (see '_parseQuery'), the answers for these questions are delivered in
 * the 'knwon answers' section of the query.
 * Host domain:
 * - A/AAAA (eg. esp8266.esp -> 192.168.2.120)
 */
bool MDNSResponder::_sendHostProbe(void) {
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _sendHostProbe (%s, %lu)\n"), m_pcHostname, millis()););
    
    bool    bResult = true;
        
    // Requests for host domain
    stcMDNSSendParameter    sendParameter;
    sendParameter.m_bCacheFlush = false;    // RFC 6762 10.2
    
    sendParameter.m_pQuestions = new stcMDNS_RRQuestion;
    if (((bResult = (0 != sendParameter.m_pQuestions))) &&
        ((bResult = _buildDomainForHost(m_pcHostname, sendParameter.m_pQuestions->m_Header.m_Domain)))) {

        //sendParameter.m_pQuestions->m_bUnicast = true;
        sendParameter.m_pQuestions->m_Header.m_Attributes.m_u16Type = DNS_RRTYPE_ANY;
        sendParameter.m_pQuestions->m_Header.m_Attributes.m_u16Class = (0x8000 | DNS_RRCLASS_IN);   // Unicast & INternet
    
        // Add known answers
#ifdef MDNS_IP4_SUPPORT
        sendParameter.m_u8HostReplyMask |= ContentFlag_A;                                   // Add A answer
#endif  
#ifdef MDNS_IP6_SUPPORT
        sendParameter.m_u8HostReplyMask |= ContentFlag_AAAA;                                // Add AAAA answer
#endif
    }
    else {
        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _sendHostProbe: FAILED to create host question!\n")););
        if (sendParameter.m_pQuestions) {
            delete sendParameter.m_pQuestions;
            sendParameter.m_pQuestions = 0;
        }
    }
    DEBUG_EX_ERR(if (!bResult) { DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _sendHostProbe: FAILED!\n")); });
    return ((bResult) &&
            (_sendMDNSMessage(sendParameter)));
}

/*
 * MDNSResponder::_sendServiceProbe
 *
 * Asks (probes) in the local network for the planned service instance domain
 * - (eg. MyESP._http._tcp.local).
 *
 * To allow 'tiebreaking' (see '_parseQuery'), the answers for these questions are delivered in
 * the 'knwon answers' section of the query.
 * Service domain:
 * - SRV (eg. MyESP._http._tcp.local -> 5000 esp8266.local)
 * - PTR NAME (eg. _http._tcp.local -> MyESP._http._tcp.local) (TODO: Check if needed, maybe TXT is better)
 */
bool MDNSResponder::_sendServiceProbe(stcMDNSService& p_rService) {
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _sendServiceProbe (%s.%s.%s, %lu)\n"), (p_rService.m_pcName ?: m_pcHostname), p_rService.m_pcService, p_rService.m_pcProtocol, millis()););

    bool    bResult = true;

    // Requests for service instance domain
    stcMDNSSendParameter    sendParameter;
    sendParameter.m_bCacheFlush = false;    // RFC 6762 10.2

    sendParameter.m_pQuestions = new stcMDNS_RRQuestion;
    if (((bResult = (0 != sendParameter.m_pQuestions))) &&
        ((bResult = _buildDomainForService(p_rService, true, sendParameter.m_pQuestions->m_Header.m_Domain)))) {

        sendParameter.m_pQuestions->m_bUnicast = true;
        sendParameter.m_pQuestions->m_Header.m_Attributes.m_u16Type = DNS_RRTYPE_ANY;
        sendParameter.m_pQuestions->m_Header.m_Attributes.m_u16Class = (0x8000 | DNS_RRCLASS_IN);   // Unicast & INternet

        // Add known answers
        p_rService.m_u8ReplyMask = (ContentFlag_SRV | ContentFlag_PTR_NAME);                // Add SRV and PTR NAME answers
    }
    else {
        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _sendServiceProbe: FAILED to create service question!\n")););
        if (sendParameter.m_pQuestions) {
            delete sendParameter.m_pQuestions;
            sendParameter.m_pQuestions = 0;
        }
    }
    DEBUG_EX_ERR(if (!bResult) { DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _sendServiceProbe: FAILED!\n")); });
    return ((bResult) &&
            (_sendMDNSMessage(sendParameter)));
}

/*
 * MDNSResponder::_cancelProbingForHost
 */
bool MDNSResponder::_cancelProbingForHost(void) {

    bool    bResult = false;

    m_HostProbeInformation.clear(false);
    // Send host notification
    if (m_HostProbeInformation.m_fnHostProbeResultCallback) {
        m_HostProbeInformation.m_fnHostProbeResultCallback(m_pcHostname, false);

        bResult = true;
    }

    for (stcMDNSService* pService=m_pServices; ((!bResult) && (pService)); pService=pService->m_pNext) {
        bResult = _cancelProbingForService(*pService);
    }
    return bResult;
}

/*
 * MDNSResponder::_cancelProbingForService
 */
bool MDNSResponder::_cancelProbingForService(stcMDNSService& p_rService) {

    bool    bResult = false;

    p_rService.m_ProbeInformation.clear(false);
    // Send notification
    if (p_rService.m_ProbeInformation.m_fnServiceProbeResultCallback) {
    	p_rService.m_ProbeInformation.m_fnServiceProbeResultCallback(p_rService.m_pcName,&p_rService,false);
    	bResult = true;
    }
    return bResult;
}



/**
 * ANNOUNCING
 */

/*
 * MDNSResponder::_announce
 *
 * Announces the host domain:
 * - A/AAAA (eg. esp8266.local -> 192.168.2.120)
 * - PTR (eg. 192.168.2.120.in-addr.arpa -> esp8266.local)
 *
 * and all presented services:
 * - PTR_TYPE (_services._dns-sd._udp.local -> _http._tcp.local)
 * - PTR_NAME (eg. _http._tcp.local -> MyESP8266._http._tcp.local)
 * - SRV (eg. MyESP8266._http._tcp.local -> 5000 esp8266.local)
 * - TXT (eg. MyESP8266._http._tcp.local -> c#=1)
 *
 * Goodbye (Un-Announcing) for the host domain and all services is also handled here.
 * Goodbye messages are created by setting the TTL for the answer to 0, this happens
 * inside the '_writeXXXAnswer' procs via 'sendParameter.m_bUnannounce = true'
 */
bool MDNSResponder::_announce(bool p_bAnnounce,
                              bool p_bIncludeServices) {
    
    bool    bResult = false;

    stcMDNSSendParameter    sendParameter;
    if (ProbingStatus_Done == m_HostProbeInformation.m_ProbingStatus) {

        bResult = true;

        sendParameter.m_bResponse = true;           // Announces are 'Unsolicited authorative responses'
        sendParameter.m_bAuthorative = true;
        sendParameter.m_bUnannounce = !p_bAnnounce; // When unannouncing, the TTL is set to '0' while creating the answers

        // Announce host
        sendParameter.m_u8HostReplyMask = 0;
    #ifdef MDNS_IP4_SUPPORT
        sendParameter.m_u8HostReplyMask |= ContentFlag_A;                   // A answer
        sendParameter.m_u8HostReplyMask |= ContentFlag_PTR_IP4;             // PTR_IP4 answer
    #endif
    #ifdef MDNS_IP6_SUPPORT
        sendParameter.m_u8HostReplyMask |= ContentFlag_AAAA;                // AAAA answer
        sendParameter.m_u8HostReplyMask |= ContentFlag_PTR_IP6;             // PTR_IP6 answer
    #endif

        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _announce: Announcing host %s (content 0x%X)\n"), m_pcHostname, sendParameter.m_u8HostReplyMask););

        if (p_bIncludeServices) {
            // Announce services (service type, name, SRV (location) and TXTs)
            for (stcMDNSService* pService=m_pServices; ((bResult) && (pService)); pService=pService->m_pNext) {
                if (ProbingStatus_Done == pService->m_ProbeInformation.m_ProbingStatus) {
                    pService->m_u8ReplyMask = (ContentFlag_PTR_TYPE | ContentFlag_PTR_NAME | ContentFlag_SRV | ContentFlag_TXT);

                    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _announce: Announcing service %s.%s.%s (content %u)\n"), (pService->m_pcName ?: m_pcHostname), pService->m_pcService, pService->m_pcProtocol, pService->m_u8ReplyMask););
                }
            }
        }
    }
    DEBUG_EX_ERR(if (!bResult) { DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _announce: FAILED!\n")); });
    return ((bResult) &&
            (_sendMDNSMessage(sendParameter)));
}

/*
 * MDNSResponder::_announceService
 */
bool MDNSResponder::_announceService(stcMDNSService& p_rService,
                                     bool p_bAnnounce /*= true*/) {

    bool    bResult = false;

    stcMDNSSendParameter    sendParameter;
    if (ProbingStatus_Done == p_rService.m_ProbeInformation.m_ProbingStatus) {

        sendParameter.m_bResponse = true;           // Announces are 'Unsolicited authorative responses'
        sendParameter.m_bAuthorative = true;
        sendParameter.m_bUnannounce = !p_bAnnounce; // When unannouncing, the TTL is set to '0' while creating the answers

        // DON'T announce host
        sendParameter.m_u8HostReplyMask = 0;

        // Announce services (service type, name, SRV (location) and TXTs)
        p_rService.m_u8ReplyMask = (ContentFlag_PTR_TYPE | ContentFlag_PTR_NAME | ContentFlag_SRV | ContentFlag_TXT);
        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _announceService: Announcing service %s.%s.%s (content 0x%X)\n"), (p_rService.m_pcName ?: m_pcHostname), p_rService.m_pcService, p_rService.m_pcProtocol, p_rService.m_u8ReplyMask););

        bResult = true;
    }
    DEBUG_EX_ERR(if (!bResult) { DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _announceService: FAILED!\n")); });
    return ((bResult) &&
            (_sendMDNSMessage(sendParameter)));
}


/**
 * SERVICE QUERY CACHE
 */

/*
 * MDNSResponder::_hasServiceQueriesWaitingForAnswers
 */
bool MDNSResponder::_hasServiceQueriesWaitingForAnswers(void) const {
    
    bool    bOpenQueries = false;
    
    for (stcMDNSServiceQuery* pServiceQuery=m_pServiceQueries; pServiceQuery; pServiceQuery=pServiceQuery->m_pNext) {
        if (pServiceQuery->m_bAwaitingAnswers) {
            bOpenQueries = true;
            break;
        }
    }
    return bOpenQueries;
}
 
/*
 * MDNSResponder::_checkServiceQueryCache
 *
 * For any 'living' service query (m_bAwaitingAnswers == true) all available answers (their components)
 * are checked for topicality based on the stored reception time and the answers TTL.
 * When the components TTL is outlasted by more than 80%, a new question is generated, to get updated information.
 * When no update arrived (in time), the component is removed from the answer (cache).
 *
 */
bool MDNSResponder::_checkServiceQueryCache(void) {
    
    bool        bResult = true;
    
    DEBUG_EX_INFO(
            bool    printedInfo = false;
    );
    for (stcMDNSServiceQuery* pServiceQuery=m_pServiceQueries; ((bResult) && (pServiceQuery)); pServiceQuery=pServiceQuery->m_pNext) {

        //
        // Resend dynamic service queries, if not already done often enough
        if ((!pServiceQuery->m_bLegacyQuery) &&
            (MDNS_DYNAMIC_QUERY_RESEND_COUNT > pServiceQuery->m_u8SentCount) &&
            (pServiceQuery->m_ResendTimeout.expired())) {

            if ((bResult = _sendMDNSServiceQuery(*pServiceQuery))) {
                ++pServiceQuery->m_u8SentCount;
                pServiceQuery->m_ResendTimeout.reset((MDNS_DYNAMIC_QUERY_RESEND_COUNT > pServiceQuery->m_u8SentCount)
                                                        ? (MDNS_DYNAMIC_QUERY_RESEND_DELAY * (pServiceQuery->m_u8SentCount - 1))
                                                        : esp8266::polledTimeout::oneShotMs::neverExpires);
            }
            DEBUG_EX_INFO(
                    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _checkServiceQueryCache: %s to resend service query!"), (bResult ? "Succeeded" : "FAILED"));
                    printedInfo = true;
            );
        }

        //
        // Schedule updates for cached answers
        if (pServiceQuery->m_bAwaitingAnswers) {
            stcMDNSServiceQuery::stcAnswer* pSQAnswer = pServiceQuery->m_pAnswers;
            while ((bResult) &&
                   (pSQAnswer)) {
                stcMDNSServiceQuery::stcAnswer* pNextSQAnswer = pSQAnswer->m_pNext;
                
                // 1. level answer
                if ((bResult) &&
                    (pSQAnswer->m_TTLServiceDomain.flagged())) {

                    if (!pSQAnswer->m_TTLServiceDomain.finalTimeoutLevel()) {

                        bResult = ((_sendMDNSServiceQuery(*pServiceQuery)) &&
                                   (pSQAnswer->m_TTLServiceDomain.restart()));
                        DEBUG_EX_INFO(
                                DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _checkServiceQueryCache: PTR update scheduled for "));
                                _printRRDomain(pSQAnswer->m_ServiceDomain);
                                DEBUG_OUTPUT.printf_P(PSTR(" %s\n"), (bResult ? "OK" : "FAILURE"));
                                printedInfo = true;
                        );
                    }
                    else {
                        // Timed out! -> Delete
                        if (pServiceQuery->m_fnCallback) {
                        	MDNSServiceInfo serviceInfo(*this, (hMDNSServiceQuery)pServiceQuery, pServiceQuery->indexOfAnswer(pSQAnswer));
                            pServiceQuery->m_fnCallback(serviceInfo, static_cast<AnswerType>(ServiceQueryAnswerType_ServiceDomain), false);
                        }
                        DEBUG_EX_INFO(
                                DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _checkServiceQueryCache: Will remove PTR answer for "));
                                _printRRDomain(pSQAnswer->m_ServiceDomain);
                                DEBUG_OUTPUT.printf_P(PSTR("\n"));
                                printedInfo = true;
                        );

                        bResult = pServiceQuery->removeAnswer(pSQAnswer);
                        pSQAnswer = 0;
                        continue;   // Don't use this answer anymore
                    }
                }   // ServiceDomain flagged
                
                // 2. level answers
                // HostDomain & Port (from SRV)
                if ((bResult) &&
                    (pSQAnswer->m_TTLHostDomainAndPort.flagged())) {

                    if (!pSQAnswer->m_TTLHostDomainAndPort.finalTimeoutLevel()) {

                        bResult = ((_sendMDNSQuery(pSQAnswer->m_ServiceDomain, DNS_RRTYPE_SRV)) &&
                                   (pSQAnswer->m_TTLHostDomainAndPort.restart()));
                        DEBUG_EX_INFO(
                                DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _checkServiceQueryCache: SRV update scheduled for "));
                                _printRRDomain(pSQAnswer->m_ServiceDomain);
                                DEBUG_OUTPUT.printf_P(PSTR(" host domain and port %s\n"), (bResult ? "OK" : "FAILURE"));
                                printedInfo = true;
                        );
                    }
                    else {
                        // Timed out! -> Delete
                        DEBUG_EX_INFO(
                                DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _checkServiceQueryCache: Will remove SRV answer for "));
                                _printRRDomain(pSQAnswer->m_ServiceDomain);
                                DEBUG_OUTPUT.printf_P(PSTR(" host domain and port\n"));
                                printedInfo = true;
                        );
                        // Delete
                        pSQAnswer->m_HostDomain.clear();
                        pSQAnswer->releaseHostDomain();
                        pSQAnswer->m_u16Port = 0;
                        pSQAnswer->m_TTLHostDomainAndPort.set(0);
                        uint32_t    u32ContentFlags = ServiceQueryAnswerType_HostDomainAndPort;
                        // As the host domain is the base for the IP4- and IP6Address, remove these too
    #ifdef MDNS_IP4_SUPPORT
                        pSQAnswer->releaseIP4Addresses();
                        u32ContentFlags |= ServiceQueryAnswerType_IP4Address;
    #endif
    #ifdef MDNS_IP6_SUPPORT
                        pSQAnswer->releaseIP6Addresses();
                        u32ContentFlags |= ServiceQueryAnswerType_IP6Address;
    #endif

                        // Remove content flags for deleted answer parts
                        pSQAnswer->m_u32ContentFlags &= ~u32ContentFlags;
                        if (pServiceQuery->m_fnCallback) {
                        	MDNSServiceInfo serviceInfo(*this, (hMDNSServiceQuery)pServiceQuery, pServiceQuery->indexOfAnswer(pSQAnswer));
                            pServiceQuery->m_fnCallback(serviceInfo,static_cast<AnswerType>(u32ContentFlags), false);
                        }
                    }
                }   // HostDomainAndPort flagged
                
                // Txts (from TXT)
                if ((bResult) &&
                    (pSQAnswer->m_TTLTxts.flagged())) {

                    if (!pSQAnswer->m_TTLTxts.finalTimeoutLevel()) {

                        bResult = ((_sendMDNSQuery(pSQAnswer->m_ServiceDomain, DNS_RRTYPE_TXT)) &&
                                   (pSQAnswer->m_TTLTxts.restart()));
                        DEBUG_EX_INFO(
                                DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _checkServiceQueryCache: TXT update scheduled for "));
                                _printRRDomain(pSQAnswer->m_ServiceDomain);
                                DEBUG_OUTPUT.printf_P(PSTR(" TXTs %s\n"), (bResult ? "OK" : "FAILURE"));
                                printedInfo = true;
                        );
                    }
                    else {
                        // Timed out! -> Delete
                        DEBUG_EX_INFO(
                                DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _checkServiceQueryCache: Will remove TXT answer for "));
                                _printRRDomain(pSQAnswer->m_ServiceDomain);
                                DEBUG_OUTPUT.printf_P(PSTR(" TXTs\n"));
                                printedInfo = true;
                        );
                        // Delete
                        pSQAnswer->m_Txts.clear();
                        pSQAnswer->m_TTLTxts.set(0);

                        // Remove content flags for deleted answer parts
                        pSQAnswer->m_u32ContentFlags &= ~ServiceQueryAnswerType_Txts;

                        if (pServiceQuery->m_fnCallback) {
                        	MDNSServiceInfo serviceInfo(*this, (hMDNSServiceQuery)pServiceQuery, pServiceQuery->indexOfAnswer(pSQAnswer));
                            pServiceQuery->m_fnCallback(serviceInfo, static_cast<AnswerType>(ServiceQueryAnswerType_Txts), false);
                        }
                    }
                }   // TXTs flagged
                
                // 3. level answers
#ifdef MDNS_IP4_SUPPORT
                // IP4Address (from A)
                stcMDNSServiceQuery::stcAnswer::stcIP4Address*  pIP4Address = pSQAnswer->m_pIP4Addresses;
                bool                                            bAUpdateQuerySent = false;
                while ((pIP4Address) &&
                       (bResult)) {
                           
                    stcMDNSServiceQuery::stcAnswer::stcIP4Address*  pNextIP4Address = pIP4Address->m_pNext; // Get 'next' early, as 'current' may be deleted at the end...
                    
                    if (pIP4Address->m_TTL.flagged()) {

                        if (!pIP4Address->m_TTL.finalTimeoutLevel()) {  // Needs update

                            if ((bAUpdateQuerySent) ||
                                ((bResult = _sendMDNSQuery(pSQAnswer->m_HostDomain, DNS_RRTYPE_A)))) {

                                pIP4Address->m_TTL.restart();
                                bAUpdateQuerySent = true;

                                DEBUG_EX_INFO(
                                        DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _checkServiceQueryCache: IP4 update scheduled for "));
                                        _printRRDomain(pSQAnswer->m_ServiceDomain);
                                        DEBUG_OUTPUT.printf_P(PSTR(" IP4 address (%s)\n"), (pIP4Address->m_IPAddress.toString().c_str()));
                                        printedInfo = true;
                                );
                            }
                        }
                        else {
                            // Timed out! -> Delete
                            DEBUG_EX_INFO(
                                    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _checkServiceQueryCache: Will remove IP4 answer for "));
                                    _printRRDomain(pSQAnswer->m_ServiceDomain);
                                    DEBUG_OUTPUT.printf_P(PSTR(" IP4 address\n"));
                                    printedInfo = true;
                            );
                            pSQAnswer->removeIP4Address(pIP4Address);
                            if (!pSQAnswer->m_pIP4Addresses) {  // NO IP4 address left -> remove content flag
                                pSQAnswer->m_u32ContentFlags &= ~ServiceQueryAnswerType_IP4Address;
                            }
                            // Notify client
                            if (pServiceQuery->m_fnCallback) {
                                MDNSServiceInfo serviceInfo(*this, (hMDNSServiceQuery)pServiceQuery, pServiceQuery->indexOfAnswer(pSQAnswer));
                                pServiceQuery->m_fnCallback(serviceInfo, static_cast<AnswerType>(ServiceQueryAnswerType_IP4Address), false);
                            }
                        }
                    }   // IP4 flagged
                    
                    pIP4Address = pNextIP4Address;  // Next
                }   // while
#endif
#ifdef MDNS_IP6_SUPPORT
                // IP6Address (from AAAA)
                stcMDNSServiceQuery::stcAnswer::stcIP6Address*  pIP6Address = pSQAnswer->m_pIP6Addresses;
                bool                                            bAAAAUpdateQuerySent = false;
                while ((pIP6Address) &&
                       (bResult)) {
                           
                    stcMDNSServiceQuery::stcAnswer::stcIP6Address*  pNextIP6Address = pIP6Address->m_pNext; // Get 'next' early, as 'current' may be deleted at the end...
                    
                    if (pIP6Address->m_TTL.flagged()) {

                        if (!pIP6Address->m_TTL.finalTimeoutLevel()) {  // Needs update
                            
                            if ((bAAAAUpdateQuerySent) ||
                                ((bResult = _sendMDNSQuery(pSQAnswer->m_HostDomain, DNS_RRTYPE_AAAA)))) {

                                pIP6Address->m_TTL.restart();
                                bAAAAUpdateQuerySent = true;

                                DEBUG_EX_INFO(
                                        DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _checkServiceQueryCache: IP6 update scheduled for "));
                                        _printRRDomain(pSQAnswer->m_ServiceDomain);
                                        DEBUG_OUTPUT.printf_P(PSTR(" IP6 address (%s)\n"), (pIP6Address->m_IPAddress.toString().c_str()));
                                        printedInfo = true;
                                );
                            }
                        }
                        else {
                            // Timed out! -> Delete
                            DEBUG_EX_INFO(
                                    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _checkServiceQueryCache: Will remove answer for "));
                                    _printRRDomain(pSQAnswer->m_ServiceDomain);
                                    DEBUG_OUTPUT.printf_P(PSTR(" IP6Address\n"));
                                    printedInfo = true;
                            );
                            pSQAnswer->removeIP6Address(pIP6Address);
                            if (!pSQAnswer->m_pIP6Addresses) {  // NO IP6 address left -> remove content flag
                                pSQAnswer->m_u32ContentFlags &= ~ServiceQueryAnswerType_IP6Address;
                            }
                            // Notify client
                            if (pServiceQuery->m_fnCallback) {
                                pServiceQuery->m_fnCallback(this, (hMDNSServiceQuery)pServiceQuery, pServiceQuery->indexOfAnswer(pSQAnswer), ServiceQueryAnswerType_IP6Address, false, pServiceQuery->m_pUserdata);
                            }
                        }
                    }   // IP6 flagged
                    
                    pIP6Address = pNextIP6Address;  // Next
                }   // while
#endif
                pSQAnswer = pNextSQAnswer;
            }
        }
    }
    DEBUG_EX_INFO(
            if (printedInfo) {
                DEBUG_OUTPUT.printf_P(PSTR("\n"));
            }
    );
    DEBUG_EX_ERR(if (!bResult) { DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _checkServiceQueryCache: FAILED!\n")); });
    return bResult;
}


/*
 * MDNSResponder::_replyMaskForHost
 *
 * Determines the relavant host answers for the given question.
 * - A question for the hostname (eg. esp8266.local) will result in an A/AAAA (eg. 192.168.2.129) reply.
 * - A question for the reverse IP address (eg. 192-168.2.120.inarpa.arpa) will result in an PTR_IP4 (eg. esp8266.local) reply.
 *
 * In addition, a full name match (question domain == host domain) is marked.
 */
uint8_t MDNSResponder::_replyMaskForHost(const MDNSResponder::stcMDNS_RRHeader& p_RRHeader,
                                         bool* p_pbFullNameMatch /*= 0*/) const {
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _replyMaskForHost\n")););
    
    uint8_t u8ReplyMask = 0;
    (p_pbFullNameMatch ? *p_pbFullNameMatch = false : 0);
    
    if ((DNS_RRCLASS_IN == p_RRHeader.m_Attributes.m_u16Class) ||
        (DNS_RRCLASS_ANY == p_RRHeader.m_Attributes.m_u16Class)) {
        
        if ((DNS_RRTYPE_PTR == p_RRHeader.m_Attributes.m_u16Type) ||
            (DNS_RRTYPE_ANY == p_RRHeader.m_Attributes.m_u16Type)) {
            // PTR request
#ifdef MDNS_IP4_SUPPORT
            stcMDNS_RRDomain    reverseIP4Domain;
            if ((_buildDomainForReverseIP4(_getResponseMulticastInterface(SOFTAP_MODE | STATION_MODE), reverseIP4Domain)) &&
                (p_RRHeader.m_Domain == reverseIP4Domain)) {
                // Reverse domain match
                u8ReplyMask |= ContentFlag_PTR_IP4;
            }
#endif
#ifdef MDNS_IP6_SUPPORT
            // TODO
#endif
        }   // Address qeuest
        
        stcMDNS_RRDomain    hostDomain;
        if ((_buildDomainForHost(m_pcHostname, hostDomain)) &&
            (p_RRHeader.m_Domain == hostDomain)) {  // Host domain match

            (p_pbFullNameMatch ? (*p_pbFullNameMatch = true) : (0));

#ifdef MDNS_IP4_SUPPORT
            if ((DNS_RRTYPE_A == p_RRHeader.m_Attributes.m_u16Type) ||
                (DNS_RRTYPE_ANY == p_RRHeader.m_Attributes.m_u16Type)) {
                // IP4 address request
                u8ReplyMask |= ContentFlag_A;
            }
#endif
#ifdef MDNS_IP6_SUPPORT
            if ((DNS_RRTYPE_AAAA == p_RRHeader.m_Attributes.m_u16Type) ||
                (DNS_RRTYPE_ANY == p_RRHeader.m_Attributes.m_u16Type)) {
                // IP6 address request
                u8ReplyMask |= ContentFlag_AAAA;
            }
#endif
        }
    }
    else {
        //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _replyMaskForHost: INVALID RR-class (0x%04X)!\n"), p_RRHeader.m_Attributes.m_u16Class););
    }
    DEBUG_EX_INFO(if (u8ReplyMask) { DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _replyMaskForHost: 0x%X\n"), u8ReplyMask); } );
    return u8ReplyMask;
}

/*
 * MDNSResponder::_replyMaskForService
 *
 * Determines the relevant service answers for the given question
 * - A PTR dns-sd service enum question (_services.dns-sd._udp.local) will result into an PTR_TYPE (eg. _http._tcp.local) answer
 * - A PTR service type question (eg. _http._tcp.local) will result into an PTR_NAME (eg. MyESP._http._tcp.local) answer
 * - A PTR service name question (eg. MyESP._http._tcp.local) will result into an PTR_NAME (eg. MyESP._http._tcp.local) answer
 * - A SRV service name question (eg. MyESP._http._tcp.local) will result into an SRV (eg. 5000 MyESP.local) answer
 * - A TXT service name question (eg. MyESP._http._tcp.local) will result into an TXT (eg. c#=1) answer
 *
 * In addition, a full name match (question domain == service instance domain) is marked.
 */
uint8_t MDNSResponder::_replyMaskForService(const MDNSResponder::stcMDNS_RRHeader& p_RRHeader,
                                            const MDNSResponder::stcMDNSService& p_Service,
                                            bool* p_pbFullNameMatch /*= 0*/) const {
    
    uint8_t u8ReplyMask = 0;
    (p_pbFullNameMatch ? *p_pbFullNameMatch = false : 0);
    
    if ((DNS_RRCLASS_IN == p_RRHeader.m_Attributes.m_u16Class) ||
        (DNS_RRCLASS_ANY == p_RRHeader.m_Attributes.m_u16Class)) {
        
        stcMDNS_RRDomain    DNSSDDomain;
        if ((_buildDomainForDNSSD(DNSSDDomain)) &&                          // _services._dns-sd._udp.local
            (p_RRHeader.m_Domain == DNSSDDomain) &&
            ((DNS_RRTYPE_PTR == p_RRHeader.m_Attributes.m_u16Type) ||
             (DNS_RRTYPE_ANY == p_RRHeader.m_Attributes.m_u16Type))) {
            // Common service info requested
            u8ReplyMask |= ContentFlag_PTR_TYPE;
        }
        
        stcMDNS_RRDomain    serviceDomain;
        if ((_buildDomainForService(p_Service, false, serviceDomain)) &&    // eg. _http._tcp.local
            (p_RRHeader.m_Domain == serviceDomain) &&
            ((DNS_RRTYPE_PTR == p_RRHeader.m_Attributes.m_u16Type) ||
             (DNS_RRTYPE_ANY == p_RRHeader.m_Attributes.m_u16Type))) {
            // Special service info requested
            u8ReplyMask |= ContentFlag_PTR_NAME;
        }
        
        if ((_buildDomainForService(p_Service, true, serviceDomain)) &&     // eg. MyESP._http._tcp.local
            (p_RRHeader.m_Domain == serviceDomain)) {

            (p_pbFullNameMatch ? (*p_pbFullNameMatch = true) : (0));

            if ((DNS_RRTYPE_SRV == p_RRHeader.m_Attributes.m_u16Type) ||
                (DNS_RRTYPE_ANY == p_RRHeader.m_Attributes.m_u16Type)) {
                // Instance info SRV requested
                u8ReplyMask |= ContentFlag_SRV;
            }
            if ((DNS_RRTYPE_TXT == p_RRHeader.m_Attributes.m_u16Type) ||
                (DNS_RRTYPE_ANY == p_RRHeader.m_Attributes.m_u16Type)) {
                // Instance info TXT requested
                u8ReplyMask |= ContentFlag_TXT;
            }
        }
    }
    else {
        //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _replyMaskForService: INVALID RR-class (0x%04X)!\n"), p_RRHeader.m_Attributes.m_u16Class););
    }
    DEBUG_EX_INFO(if (u8ReplyMask) { DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _replyMaskForService(%s.%s.%s): 0x%X\n"), p_Service.m_pcName, p_Service.m_pcService, p_Service.m_pcProtocol, u8ReplyMask); } );
    return u8ReplyMask;
}

} // namespace MDNSImplementation

} // namespace esp8266
