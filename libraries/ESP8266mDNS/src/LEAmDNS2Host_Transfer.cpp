/*
    LEAmDNS2Host_Transfer.cpp

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

#include <coredecls.h>  // for can_yield()
#include "ESP8266mDNS.h"
#include "LEAmDNS2Host.h"
#include "LEAmDNS2_Priv.h"

namespace esp8266
{


namespace experimental
{


/*

    SENDING

*/

/*
    MDNSResponder::_sendMessage

    Unicast responses are prepared and sent directly to the querier.
    Multicast responses or queries are transferred to _sendMessage_Multicast

    Any reply flags in installed services are removed at the end!

*/
bool clsLEAMDNSHost::_sendMessage(clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    bool bResult = true;
    for (netif* pNetIf = netif_list; pNetIf; pNetIf = pNetIf->next)
        if (netif_is_up(pNetIf))
        {
            bResult = bResult && _sendMessage(pNetIf, p_rSendParameter);
        }

    // Finally clear service reply masks
    for (clsService* pService : m_Services)
    {
        pService->m_u32ReplyMask = 0;
    }
    return bResult;
}
bool clsLEAMDNSHost::_sendMessage(netif* pNetIf, clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    bool    bResult = false;
    uint8_t	u8AvailableProtocols = 0;

    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _sendMessage: if=" NETIFID_STR "\n"), _DH(), NETIFID_VAL(pNetIf)));

#ifdef MDNS_IPV4_SUPPORT
    // Only send out IPv4 messages, if we've got an IPv4 address
    if (_getResponderIPAddress(pNetIf, enuIPProtocolType::V4).isSet())
    {
        u8AvailableProtocols |= static_cast<uint8_t>(enuIPProtocolType::V4);
    }
    DEBUG_EX_INFO(else
    {
        DEBUG_OUTPUT.printf_P(PSTR("%s _sendMessage: No IPv4 address available!\n"), _DH());
    });
#endif
#ifdef MDNS2_IPV6_SUPPORT
    // Only send out IPv6 messages, if we've got an IPv6 address
    if (_getResponderIPAddress(pNetIf, enuIPProtocolType::V6).isSet())
    {
        u8AvailableProtocols |= static_cast<uint8_t>(enuIPProtocolType::V6);
    }
    DEBUG_EX_INFO(else
    {
        DEBUG_OUTPUT.printf_P(PSTR("%s _sendMessage: No IPv6 address available!\n"), _DH());
    });
#endif

    if (clsBackbone::sm_pBackbone->setDelayUDPProcessing(true))
    {
        // Avoid 're-entry-like problems because delay() is called!
        if (clsSendParameter::enuResponseType::None != p_rSendParameter.m_Response)
        {
            IPAddress   ipRemote = ((clsSendParameter::enuResponseType::Response == p_rSendParameter.m_Response)
                                    ? m_pUDPContext->getRemoteAddress()
                                    : IPAddress());

            if (p_rSendParameter.m_bUnicast)
            {
                // Unicast response  -> Send to querier
                DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _sendMessage: Will send unicast to '%s'.\n"), _DH(), ipRemote.toString().c_str()););
                DEBUG_EX_ERR(if (!ipRemote.isSet()) DEBUG_OUTPUT.printf_P(PSTR("%s _sendMessage: MISSING remote address for unicast response!\n"), _DH()););

                bResult = ((ipRemote.isSet()) &&
                           (_prepareMessage(pNetIf, p_rSendParameter)) &&
                           (m_pUDPContext->sendTimeout(ipRemote, m_pUDPContext->getRemotePort(), clsConsts::u32SendTimeoutMs)) /*&&
                           (Serial.println("Did send UC"), true)*/);
                DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _sendMessage (V4): FAILED!\n"), _DH()););
            }
            else
            {
                // Multicast response -> Send via the same network interface, that received the query
#ifdef MDNS_IPV4_SUPPORT
                if (((!ipRemote.isSet()) ||                                                 // NO remote IP
                        (ipRemote.isV4())) &&                                                  // OR  IPv4
                        (u8AvailableProtocols & static_cast<uint8_t>(enuIPProtocolType::V4)))   // AND IPv4 protocol available
                {
                    bResult = _sendMessage_Multicast(pNetIf, p_rSendParameter, static_cast<uint8_t>(enuIPProtocolType::V4));
                }
#endif
#ifdef MDNS2_IPV6_SUPPORT
                if (((!ipRemote.isSet()) ||                                                 // NO remote IP
                        (ipRemote.isV6())) &&                                                  // OR  IPv6
                        (u8AvailableProtocols & static_cast<uint8_t>(enuIPProtocolType::V6)))   // AND IPv6 protocol available
                {
                    bResult = _sendMessage_Multicast(pNetIf, p_rSendParameter, static_cast<uint8_t>(enuIPProtocolType::V6));
                }
#endif
            }
        }
        else
        {
            // Multicast query -> Send by all available protocols
            bResult = ((u8AvailableProtocols) &&
                       (_sendMessage_Multicast(pNetIf, p_rSendParameter, u8AvailableProtocols)));
        }

        clsBackbone::sm_pBackbone->setDelayUDPProcessing(false);
    }

    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _sendMessage: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_sendMessage_Multicast

    Fills the UDP output buffer (via _prepareMessage) and sends the buffer
    via the selected WiFi protocols
*/
bool clsLEAMDNSHost::_sendMessage_Multicast(netif* pNetIf, clsLEAMDNSHost::clsSendParameter& p_rSendParameter,
        uint8_t p_IPProtocolTypes)
{
    bool    bIPv4Result = true;
    bool    bIPv6Result = true;

    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _sendMessage_Multicast: if=" NETIFID_STR "\n"), _DH(), NETIFID_VAL(pNetIf)));

#ifdef MDNS_IPV4_SUPPORT
    if (p_IPProtocolTypes & static_cast<uint8_t>(enuIPProtocolType::V4))
    {
        IPAddress   ip4MulticastAddress(DNS_MQUERY_IPV4_GROUP_INIT);

        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _sendMessage_Multicast IPv4: Will send to '%s'.\n"), _DH(), ip4MulticastAddress.toString().c_str()););
        DEBUG_EX_INFO(if (!_getResponderIPAddress(pNetIf, enuIPProtocolType::V4)) DEBUG_OUTPUT.printf_P(PSTR("%s _sendMessage_Multicast IPv4: NO IPv4 address!.\n"), _DH()););
        bIPv4Result = ((_prepareMessage(pNetIf, p_rSendParameter)) &&
                       (m_pUDPContext->setMulticastInterface(pNetIf), true) &&
                       (m_pUDPContext->sendTimeout(ip4MulticastAddress, DNS_MQUERY_PORT, clsConsts::u32SendTimeoutMs)) &&
                       (m_pUDPContext->setMulticastInterface(0), true) /*&&
                       (Serial.println("Did send MC V4"), true)*/);
        DEBUG_EX_ERR(if (!bIPv4Result) DEBUG_OUTPUT.printf_P(PSTR("%s _sendMessage_Multicast (V4): FAILED!\n"), _DH()););
    }
#endif

#ifdef MDNS2_IPV6_SUPPORT
    if (p_IPProtocolTypes & static_cast<uint8_t>(enuIPProtocolType::V6))
    {
        IPAddress   ip6MulticastAddress(DNS_MQUERY_IPV6_GROUP_INIT);

        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _sendMessage_Multicast IPv6: Will send to '%s'.\n"), _DH(), ip6MulticastAddress.toString().c_str()););
        DEBUG_EX_INFO(if (!_getResponderIPAddress(pNetIf, enuIPProtocolType::V6)) DEBUG_OUTPUT.printf_P(PSTR("%s _sendMessage_Multicast IPv6: NO IPv6 address!.\n"), _DH()););
        DEBUG_EX_ERR(
            bool    bPrepareMessage = false;
            bool    bUDPContextSend = false;
        );
        bIPv6Result = ((DEBUG_EX_ERR(bPrepareMessage =)_prepareMessage(pNetIf, p_rSendParameter)) &&
                       (m_pUDPContext->setMulticastInterface(pNetIf), true) &&
                       (DEBUG_EX_ERR(bUDPContextSend =)m_pUDPContext->sendTimeout(ip6MulticastAddress, DNS_MQUERY_PORT, clsConsts::u32SendTimeoutMs)) &&
                       (m_pUDPContext->setMulticastInterface(0), true) /*&&
                       (Serial.println("Did send MC V6"), true)*/);
        DEBUG_EX_ERR(if (!bIPv6Result) DEBUG_OUTPUT.printf_P(PSTR("%s _sendMessage_Multicast (IPv6): FAILED! (%s, %s, %s)\n"), _DH(), (_getResponderIPAddress(pNetIf, enuIPProtocolType::V6).isSet() ? "1" : "0"), (bPrepareMessage ? "1" : "0"), (bUDPContextSend ? "1" : "0")););
    }
#endif

    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _sendMessage_Multicast: %s!\n\n"), _DH(), ((bIPv4Result && bIPv6Result) ? "Succeeded" : "FAILED")););
    DEBUG_EX_ERR(if (!(bIPv4Result && bIPv6Result)) DEBUG_OUTPUT.printf_P(PSTR("%s _sendMessage_Multicast: FAILED!\n"), _DH()););
    return (bIPv4Result && bIPv6Result);
}

/*
    MDNSResponder::_prepareMessage

    The MDNS message is composed in a two-step process.
    In the first loop 'only' the header informations (mainly number of answers) are collected,
    while in the second loop, the header and all queries and answers are written to the UDP
    output buffer.

*/
bool clsLEAMDNSHost::_prepareMessage(netif* pNetIf, clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _prepareMDNSMessage\n")););
    bool    bResult = true;

    // Prepare output buffer for potential reuse
    p_rSendParameter.flushTempContent();

    // Prepare header; count answers
    clsMsgHeader  msgHeader(p_rSendParameter.m_u16ID,
                            (static_cast<clsSendParameter::enuResponseType>(clsSendParameter::enuResponseType::None) != p_rSendParameter.m_Response),
                            0,
                            p_rSendParameter.m_bAuthorative);
    // If this is a response, the answers are anwers,
    // else this is a query or probe and the answers go into auth section
    uint16_t&           ru16Answers = ((clsSendParameter::enuResponseType::None != p_rSendParameter.m_Response)
                                       ? msgHeader.m_u16ANCount    // Usual answers
                                       : msgHeader.m_u16NSCount);  // Authorative answers

    /**
        enuSequence
    */
    using typeSequence = uint8_t;
    enum class enuSequence : typeSequence
    {
        Count   = 0,
        Send    = 1
    };

    // Two step sequence: 'Count' and 'Send'
    for (typeSequence sequence = static_cast<typeSequence>(enuSequence::Count); ((bResult) && (sequence <= static_cast<typeSequence>(enuSequence::Send))); ++sequence)
    {
        /*
            DEBUG_EX_INFO(
            if (static_cast<typeSequence>(enuSequence::Send) == sequence)
            DEBUG_OUTPUT.printf_P(PSTR("%s _prepareMDNSMessage: ID:%u QR:%u OP:%u AA:%u TC:%u RD:%u RA:%u R:%u QD:%u AN:%u NS:%u AR:%u\n"),
                                  _DH(),
                                  (unsigned)msgHeader.m_u16ID,
                                  (unsigned)msgHeader.m_1bQR, (unsigned)msgHeader.m_4bOpcode, (unsigned)msgHeader.m_1bAA, (unsigned)msgHeader.m_1bTC, (unsigned)msgHeader.m_1bRD,
                                  (unsigned)msgHeader.m_1bRA, (unsigned)msgHeader.m_4bRCode,
                                  (unsigned)msgHeader.m_u16QDCount,
                                  (unsigned)msgHeader.m_u16ANCount,
                                  (unsigned)msgHeader.m_u16NSCount,
                                  (unsigned)msgHeader.m_u16ARCount);
            );
        */
        // Count/send
        // Header
        bResult = ((static_cast<typeSequence>(enuSequence::Count) == sequence)
                   ? true
                   : _writeMDNSMsgHeader(msgHeader, p_rSendParameter));
        DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _prepareMDNSMessage: _writeMDNSMsgHeader FAILED!\n"), _DH()););
        // Questions
        for (clsRRQuestion::list::iterator it = p_rSendParameter.m_RRQuestions.begin(); ((bResult) && (it != p_rSendParameter.m_RRQuestions.end())); it++)
        {
            clsRRQuestion* pQuestion = *it;

            ((static_cast<typeSequence>(enuSequence::Count) == sequence)
             ? ++msgHeader.m_u16QDCount
             : (bResult = _writeMDNSQuestion(*pQuestion, p_rSendParameter)));
            DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _prepareMDNSMessage: _writeMDNSQuestion FAILED!\n"), _DH()););
        }

        // Answers and authorative answers
        // NSEC host (part 1)
        uint32_t    u32NSECContent = 0;
#ifdef MDNS_IPV4_SUPPORT
        // A
        if ((bResult) &&
                (p_rSendParameter.m_u32HostReplyMask & static_cast<uint32_t>(enuContentFlag::A)) &&
                (_getResponderIPAddress(pNetIf, enuIPProtocolType::V4).isSet()))
        {

            u32NSECContent |= static_cast<uint32_t>(enuContentFlag::A);
            ((static_cast<typeSequence>(enuSequence::Count) == sequence)
             ? ++ru16Answers
             : (bResult = _writeMDNSAnswer_A(_getResponderIPAddress(pNetIf, enuIPProtocolType::V4), p_rSendParameter)));
            DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _prepareMDNSMessage: _writeMDNSAnswer_A(A) FAILED!\n"), _DH()););
        }
        // PTR_IPv4
        if ((bResult) &&
                (p_rSendParameter.m_u32HostReplyMask & static_cast<uint32_t>(enuContentFlag::PTR_IPv4)) &&
                (_getResponderIPAddress(pNetIf, enuIPProtocolType::V4).isSet()))
        {

            u32NSECContent |= static_cast<uint32_t>(enuContentFlag::PTR_IPv4);
            ((static_cast<typeSequence>(enuSequence::Count) == sequence)
             ? ++ru16Answers
             : (bResult = _writeMDNSAnswer_PTR_IPv4(_getResponderIPAddress(pNetIf, enuIPProtocolType::V4), p_rSendParameter)));
            DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _prepareMDNSMessage: _writeMDNSAnswer_PTR_IPv4 FAILED!\n"), _DH()););
        }
#endif
#ifdef MDNS2_IPV6_SUPPORT
        // AAAA
        if ((bResult) &&
                (p_rSendParameter.m_u32HostReplyMask & static_cast<uint32_t>(enuContentFlag::AAAA)) &&
                (_getResponderIPAddress(pNetIf, (enuIPProtocolType::V6)).isSet()))
        {

            u32NSECContent |= static_cast<uint32_t>(enuContentFlag::AAAA);
            ((static_cast<typeSequence>(enuSequence::Count) == sequence)
             ? ++ru16Answers
             : (bResult = _writeMDNSAnswer_AAAA(_getResponderIPAddress(pNetIf, (enuIPProtocolType::V6)), p_rSendParameter)));
            DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _prepareMDNSMessage: _writeMDNSAnswer_AAAA(A) FAILED!\n"), _DH()););
        }
        // PTR_IPv6
        if ((bResult) &&
                (p_rSendParameter.m_u32HostReplyMask & static_cast<uint32_t>(enuContentFlag::PTR_IPv6)) &&
                (_getResponderIPAddress(pNetIf, (enuIPProtocolType::V6)).isSet()))
        {

            u32NSECContent |= static_cast<uint32_t>(enuContentFlag::PTR_IPv6);
            ((static_cast<typeSequence>(enuSequence::Count) == sequence)
             ? ++ru16Answers
             : (bResult = _writeMDNSAnswer_PTR_IPv6(_getResponderIPAddress(pNetIf, (enuIPProtocolType::V6)), p_rSendParameter)));
            DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _prepareMDNSMessage: _writeMDNSAnswer_PTR_IPv6 FAILED!\n"), _DH()););
        }
#endif

        for (clsService::list::iterator it = m_Services.begin(); ((bResult) && (it != m_Services.end())); it++)
        {
            clsService* pService = *it;

            // PTR_TYPE
            if ((bResult) &&
                    (pService->m_u32ReplyMask & static_cast<uint32_t>(enuContentFlag::PTR_TYPE)))
            {
                ((static_cast<typeSequence>(enuSequence::Count) == sequence)
                 ? ++ru16Answers
                 : (bResult = _writeMDNSAnswer_PTR_TYPE(*pService, p_rSendParameter)));
                DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _prepareMDNSMessage: _writeMDNSAnswer_PTR_TYPE FAILED!\n"), _DH()););
            }
            // PTR_NAME
            if ((bResult) &&
                    (pService->m_u32ReplyMask & static_cast<uint32_t>(enuContentFlag::PTR_NAME)))
            {
                ((static_cast<typeSequence>(enuSequence::Count) == sequence)
                 ? ++ru16Answers
                 : (bResult = _writeMDNSAnswer_PTR_NAME(*pService, p_rSendParameter)));
                DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _prepareMDNSMessage: _writeMDNSAnswer_PTR_NAME FAILED!\n"), _DH()););
            }
            // SRV
            if ((bResult) &&
                    (pService->m_u32ReplyMask & static_cast<uint32_t>(enuContentFlag::SRV)))
            {
                ((static_cast<typeSequence>(enuSequence::Count) == sequence)
                 ? ++ru16Answers
                 : (bResult = _writeMDNSAnswer_SRV(*pService, p_rSendParameter)));
                DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _prepareMDNSMessage: _writeMDNSAnswer_SRV(A) FAILED!\n"), _DH()););
            }
            // TXT
            if ((bResult) &&
                    (pService->m_u32ReplyMask & static_cast<uint32_t>(enuContentFlag::TXT)))
            {
                ((static_cast<typeSequence>(enuSequence::Count) == sequence)
                 ? ++ru16Answers
                 : (bResult = _writeMDNSAnswer_TXT(*pService, p_rSendParameter)));
                DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _prepareMDNSMessage: _writeMDNSAnswer_TXT(A) FAILED!\n"), _DH()););
            }
        }   // for services

        // Additional answers
        uint16_t&   ru16AdditionalAnswers = msgHeader.m_u16ARCount;

#ifdef MDNS_IPV4_SUPPORT
        bool    bNeedsAdditionalAnswerA = false;
#endif
#ifdef MDNS2_IPV6_SUPPORT
        bool    bNeedsAdditionalAnswerAAAA = false;
#endif
        for (clsService::list::iterator it = m_Services.begin(); ((bResult) && (it != m_Services.end())); it++)
        {
            clsService* pService = *it;

            if ((bResult) &&
                    (pService->m_u32ReplyMask & static_cast<uint32_t>(enuContentFlag::PTR_NAME)) &&    // If PTR_NAME is requested, AND
                    (!(pService->m_u32ReplyMask & static_cast<uint32_t>(enuContentFlag::SRV))))        // NOT SRV -> add SRV as additional answer
            {

                ((static_cast<typeSequence>(enuSequence::Count) == sequence)
                 ? ++ru16AdditionalAnswers
                 : (bResult = _writeMDNSAnswer_SRV(*pService, p_rSendParameter)));
                DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _prepareMDNSMessage: _writeMDNSAnswer_SRV(B) FAILED!\n"), _DH()););
            }
            /*  AppleTV doesn't add TXT
                if ((bResult) &&
                (pService->m_u32ReplyMask & ContentFlag_PTR_NAME) &&    // If PTR_NAME is requested, AND
                (!(pService->m_u32ReplyMask & ContentFlag_TXT))) {      // NOT TXT -> add TXT as additional answer
                ((static_cast<typeSequence>(enuSequence::Count) == sequence)
                    ? ++ru16AdditionalAnswers
                    : (bResult = _writeMDNSAnswer_TXT(*pService, p_rSendParameter)));
                DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _prepareMDNSMessage: _writeMDNSAnswer_TXT(B) FAILED!\n")););
                }
            */
            if ((pService->m_u32ReplyMask & (static_cast<uint32_t>(enuContentFlag::PTR_NAME) | static_cast<uint32_t>(enuContentFlag::SRV))) ||          // If service instance name or SRV OR
                    (p_rSendParameter.m_u32HostReplyMask & (static_cast<uint32_t>(enuContentFlag::A) | static_cast<uint32_t>(enuContentFlag::AAAA))))       // any host IP address is requested
            {
#ifdef MDNS_IPV4_SUPPORT
                if ((bResult) &&
                        (!(p_rSendParameter.m_u32HostReplyMask & static_cast<uint32_t>(enuContentFlag::A))))                   // Add IPv4 address
                {
                    bNeedsAdditionalAnswerA = true;
                }
#endif
#ifdef MDNS2_IPV6_SUPPORT
                if ((bResult) &&
                        (!(p_rSendParameter.m_u32HostReplyMask & static_cast<uint32_t>(enuContentFlag::AAAA))))                // Add IPv6 address
                {
                    bNeedsAdditionalAnswerAAAA = true;
                }
#endif
            }
            // NSEC record for service
            if ((bResult) &&
                    (pService->m_u32ReplyMask) &&
                    ((clsSendParameter::enuResponseType::None != p_rSendParameter.m_Response)))
            {
                ((static_cast<typeSequence>(enuSequence::Count) == sequence)
                 ? ++ru16AdditionalAnswers
                 : (bResult = _writeMDNSAnswer_NSEC(*pService, (static_cast<uint32_t>(enuContentFlag::TXT) | static_cast<uint32_t>(enuContentFlag::SRV)), p_rSendParameter)));
                DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _prepareMDNSMessage: _writeMDNSAnswer_NSEC(Service) FAILED!\n"), _DH()););
            }
        }   // for services

#ifdef MDNS_IPV4_SUPPORT
        // Answer A needed?
        if ((bResult) &&
                (bNeedsAdditionalAnswerA) &&
                (_getResponderIPAddress(pNetIf, (enuIPProtocolType::V4)).isSet()))
        {
            // Additional A
            u32NSECContent |= static_cast<uint32_t>(enuContentFlag::A);
            ((static_cast<typeSequence>(enuSequence::Count) == sequence)
             ? ++ru16AdditionalAnswers
             : (bResult = _writeMDNSAnswer_A(_getResponderIPAddress(pNetIf, (enuIPProtocolType::V4)), p_rSendParameter)));

            DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _prepareMDNSMessage: _writeMDNSAnswer_A(B) FAILED!\n"), _DH()););
        }
#endif
#ifdef MDNS2_IPV6_SUPPORT
        // Answer AAAA needed?
        if ((bResult) &&
                (bNeedsAdditionalAnswerAAAA) &&
                (_getResponderIPAddress(pNetIf, (enuIPProtocolType::V6)).isSet()))
        {
            // Additional AAAA
            u32NSECContent |= static_cast<uint32_t>(enuContentFlag::AAAA);
            ((static_cast<typeSequence>(enuSequence::Count) == sequence)
             ? ++ru16AdditionalAnswers
             : (bResult = _writeMDNSAnswer_AAAA(_getResponderIPAddress(pNetIf, (enuIPProtocolType::V6)), p_rSendParameter)));

            DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _prepareMDNSMessage: _writeMDNSAnswer_AAAA(B) FAILED!\n"), _DH()););
        }
#endif

        // NSEC host (part 2)
        if ((bResult) &&
                ((clsSendParameter::enuResponseType::None != p_rSendParameter.m_Response)) &&
                (u32NSECContent))
        {
            // NSEC PTR IPv4/IPv6 are separate answers; make sure, that this is counted for
#ifdef MDNS_IPV4_SUPPORT
            uint32_t    u32NSECContent_PTR_IPv4 = (u32NSECContent & static_cast<uint32_t>(enuContentFlag::PTR_IPv4));
            u32NSECContent &= ~static_cast<uint32_t>(enuContentFlag::PTR_IPv4);
#endif
#ifdef MDNS2_IPV6_SUPPORT
            uint32_t    u32NSECContent_PTR_IPv6 = (u32NSECContent & static_cast<uint32_t>(enuContentFlag::PTR_IPv6));
            u32NSECContent &= ~static_cast<uint32_t>(enuContentFlag::PTR_IPv6);
#endif

            ((static_cast<typeSequence>(enuSequence::Count) == sequence)
             ? (ru16AdditionalAnswers += ((u32NSECContent ? 1 : 0)
#ifdef MDNS_IPV4_SUPPORT
                                          + (u32NSECContent_PTR_IPv4 ? 1 : 0)
#endif
#ifdef MDNS2_IPV6_SUPPORT
                                          + (u32NSECContent_PTR_IPv6 ? 1 : 0)
#endif
                                         ))
             : (bResult = (((!u32NSECContent) ||
                            // Write host domain NSEC answer
                            (_writeMDNSAnswer_NSEC(u32NSECContent, p_rSendParameter)))
#ifdef MDNS_IPV4_SUPPORT
                           // Write separate answer for host PTR IPv4
                           && ((!u32NSECContent_PTR_IPv4) ||
                               ((!_getResponderIPAddress(pNetIf, (enuIPProtocolType::V4)).isSet()) ||
                                (_writeMDNSAnswer_NSEC_PTR_IPv4(_getResponderIPAddress(pNetIf, (enuIPProtocolType::V4)), p_rSendParameter))))
#endif
#ifdef MDNS2_IPV6_SUPPORT
                           // Write separate answer for host PTR IPv6
                           && ((!u32NSECContent_PTR_IPv6) ||
                               ((!_getResponderIPAddress(pNetIf, (enuIPProtocolType::V6)).isSet()) ||
                                (_writeMDNSAnswer_NSEC_PTR_IPv6(_getResponderIPAddress(pNetIf, (enuIPProtocolType::V6)), p_rSendParameter))))
#endif
                          )));

            DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _prepareMDNSMessage: _writeMDNSAnswer_NSEC(Host) FAILED!\n"), _DH()););
        }

        DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _prepareMDNSMessage: Loop %i FAILED!\n"), _DH(), sequence););
    }   // for sequence
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _prepareMDNSMessage: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_addQueryRecord

    Adds a query for the given domain and query type.

*/
bool clsLEAMDNSHost::_addQueryRecord(clsLEAMDNSHost::clsSendParameter& p_rSendParameter,
                                     const clsLEAMDNSHost::clsRRDomain& p_QueryDomain,
                                     uint16_t p_u16RecordType)
{
    bool    bResult = false;

    clsRRQuestion*  pNewRRQuestion = new clsRRQuestion;
    if ((bResult = (0 != pNewRRQuestion)))
    {
        // Link to list of questions
        p_rSendParameter.m_RRQuestions.push_back(pNewRRQuestion);

        pNewRRQuestion->m_Header.m_Domain = p_QueryDomain;

        pNewRRQuestion->m_Header.m_Attributes.m_u16Type = p_u16RecordType;
        // It seems, that some mDNS implementations don't support 'unicast response' questions...
        pNewRRQuestion->m_Header.m_Attributes.m_u16Class = (/*0x8000 |*/ DNS_RRCLASS_IN);   // /*Unicast &*/ INternet
    }
    return bResult;
}

/*
    MDNSResponder::_sendQuery

    Creates and sends a query for the given domain and query type.

*/
bool clsLEAMDNSHost::_sendQuery(const clsLEAMDNSHost::clsQuery& p_Query,
                                clsLEAMDNSHost::clsQuery::clsAnswer::list* p_pKnownAnswers /*= 0*/)
{
    bool                    bResult = false;

    clsSendParameter    sendParameter;
    switch (p_Query.m_QueryType)
    {
    case clsQuery::enuQueryType::Host:
#ifdef MDNS_IPV4_SUPPORT
        bResult = _addQueryRecord(sendParameter, p_Query.m_Domain, DNS_RRTYPE_A);
#endif
#ifdef MDNS2_IPV6_SUPPORT
        bResult = _addQueryRecord(sendParameter, p_Query.m_Domain, DNS_RRTYPE_AAAA);
#endif
        break;

    case clsQuery::enuQueryType::Service:
        bResult = _addQueryRecord(sendParameter, p_Query.m_Domain, DNS_RRTYPE_PTR);
        break;

    case clsQuery::enuQueryType::None:
    default:
        break;
    }

    // TODO: Add known answers to query
    (void)p_pKnownAnswers;
    bResult = ((bResult) &&
               (_sendMessage(sendParameter)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _sendQuery: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_sendQuery

    Creates and sends a query for the given domain and record type.

*/
bool clsLEAMDNSHost::_sendQuery(const clsLEAMDNSHost::clsRRDomain& p_QueryDomain,
                                uint16_t p_u16RecordType,
                                clsLEAMDNSHost::clsQuery::clsAnswer::list* p_pKnownAnswers /*= 0*/)
{
    bool                    bResult = false;

    clsSendParameter    sendParameter;
    bResult = ((_addQueryRecord(sendParameter, p_QueryDomain, p_u16RecordType)) &&
               (_sendMessage(sendParameter)));

    // TODO: Add known answer records
    (void) p_pKnownAnswers;

    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _sendQuery: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_getResponderIPAddress
*/
IPAddress clsLEAMDNSHost::_getResponderIPAddress(netif* pNetIf, enuIPProtocolType p_IPProtocolType) const
{
    IPAddress	ipResponder;
#ifdef MDNS_IPV4_SUPPORT
    if (enuIPProtocolType::V4 == p_IPProtocolType)
    {
        ipResponder = netif_ip_addr4(pNetIf);
    }
#endif
#ifdef MDNS2_IPV6_SUPPORT
    if (enuIPProtocolType::V6 == p_IPProtocolType)
    {
        bool	bCheckLinkLocal = true;
        for (int i = 0; ((!ipResponder.isSet()) && (i < 2)); ++i)  	// Two loops: First with link-local check, second without
        {
            for (int idx = 0; idx < LWIP_IPV6_NUM_ADDRESSES; ++idx)
            {
                //DEBUG_EX_INFO(if ip6_addr_isvalid(netif_ip6_addr_state(&pNetIf, idx)) DEBUG_OUTPUT.printf_P(PSTR("%s _getResponderIPAddress: Checking IPv6 address %s (LL: %s)\n"), _DH(), IPAddress(netif_ip_addr6(pNetIf, idx)).toString().c_str(), (bCheckLinkLocal ? "YES" : "NO")););
                if ((ip6_addr_isvalid(netif_ip6_addr_state(pNetIf, idx))) &&
                        (((!bCheckLinkLocal) ||
                          (ip6_addr_islinklocal(netif_ip6_addr(pNetIf, idx))))))
                {
                    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _getResponderIPAddress: Selected IPv6 address %s (LL: %s)\n"), _DH(), IPAddress(netif_ip_addr6(pNetIf, idx)).toString().c_str(), (bCheckLinkLocal ? "YES" : "NO")););
                    ipResponder = netif_ip_addr6(pNetIf, idx);
                    break;
                }
            }
            bCheckLinkLocal = false;
        }
    }
#endif
    return ipResponder;
}


/**
    HELPERS
*/

/**
    RESOURCE RECORDS
*/

/*
    MDNSResponder::_readRRQuestion

    Reads a question (eg. MyESP._http._tcp.local ANY IN) from the UPD input buffer.

*/
bool clsLEAMDNSHost::_readRRQuestion(clsLEAMDNSHost::clsRRQuestion& p_rRRQuestion)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _readRRQuestion\n")););

    bool    bResult = false;

    if ((bResult = _readRRHeader(p_rRRQuestion.m_Header)))
    {
        // Extract unicast flag from class field
        p_rRRQuestion.m_bUnicast = (p_rRRQuestion.m_Header.m_Attributes.m_u16Class & 0x8000);
        //p_rRRQuestion.m_Header.m_Attributes.m_u16Class &= (~0x8000);

        DEBUG_EX_INFO(
            DEBUG_OUTPUT.printf_P(PSTR("%s _readRRQuestion "), _DH());
            _printRRDomain(p_rRRQuestion.m_Header.m_Domain);
            DEBUG_OUTPUT.printf_P(PSTR(" Type:%s Class:%s\n"), _RRType2Name(p_rRRQuestion.m_Header.m_Attributes.m_u16Type), _RRClass2String(p_rRRQuestion.m_Header.m_Attributes.m_u16Class, true));
        );
    }
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _readRRQuestion: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_readRRAnswer

    Reads an answer (eg. _http._tcp.local PTR OP TTL MyESP._http._tcp.local)
    from the UDP input buffer.
    After reading the domain and type info, the further processing of the answer
    is transferred the answer specific reading functions.
    Unknown answer types are processed by the generic answer reader (to remove them
    from the input buffer).

*/
bool clsLEAMDNSHost::_readRRAnswer(clsLEAMDNSHost::clsRRAnswer*& p_rpRRAnswer)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _readRRAnswer\n")););

    bool    bResult = false;

    clsRRHeader    header;
    uint32_t            u32TTL;
    uint16_t            u16RDLength;
    if ((_readRRHeader(header)) &&
            (_udpRead32(u32TTL)) &&
            (_udpRead16(u16RDLength)))
    {
        /*  DEBUG_EX_INFO(
                DEBUG_OUTPUT.printf_P(PSTR("%s _readRRAnswer: Reading 0x%04X answer (class:0x%04X, TTL:%u, RDLength:%u) for "), header.m_Attributes.m_u16Type, header.m_Attributes.m_u16Class, u32TTL, u16RDLength);
                _printRRDomain(header.m_Domain);
                DEBUG_OUTPUT.printf_P(PSTR("\n"));
                );*/

        switch (header.m_Attributes.m_u16Type /*& (~0x8000)*/)      // Topmost bit might carry 'cache flush' flag
        {
#ifdef MDNS_IPV4_SUPPORT
        case DNS_RRTYPE_A:
            p_rpRRAnswer = new clsRRAnswerA(header, u32TTL);
            bResult = _readRRAnswerA(*(clsRRAnswerA*&)p_rpRRAnswer, u16RDLength);
            break;
#endif
        case DNS_RRTYPE_PTR:
            p_rpRRAnswer = new clsRRAnswerPTR(header, u32TTL);
            bResult = _readRRAnswerPTR(*(clsRRAnswerPTR*&)p_rpRRAnswer, u16RDLength);
            break;
        case DNS_RRTYPE_TXT:
            p_rpRRAnswer = new clsRRAnswerTXT(header, u32TTL);
            bResult = _readRRAnswerTXT(*(clsRRAnswerTXT*&)p_rpRRAnswer, u16RDLength);
            break;
#ifdef MDNS2_IPV6_SUPPORT
        case DNS_RRTYPE_AAAA:
            p_rpRRAnswer = new clsRRAnswerAAAA(header, u32TTL);
            bResult = _readRRAnswerAAAA(*(clsRRAnswerAAAA*&)p_rpRRAnswer, u16RDLength);
            break;
#endif
        case DNS_RRTYPE_SRV:
            p_rpRRAnswer = new clsRRAnswerSRV(header, u32TTL);
            bResult = _readRRAnswerSRV(*(clsRRAnswerSRV*&)p_rpRRAnswer, u16RDLength);
            break;
        default:
            p_rpRRAnswer = new clsRRAnswerGeneric(header, u32TTL);
            bResult = _readRRAnswerGeneric(*(clsRRAnswerGeneric*&)p_rpRRAnswer, u16RDLength);
            break;
        }

        DEBUG_EX_INFO_IF((bResult) && (p_rpRRAnswer),
        {
            DEBUG_OUTPUT.printf_P(PSTR("%s _readRRAnswer: "), _DH());
            _printRRDomain(p_rpRRAnswer->m_Header.m_Domain);
            DEBUG_OUTPUT.printf_P(PSTR(" Type:%s Class:0x%04X TTL:%u, RDLength:%u "),
                                  _RRType2Name(p_rpRRAnswer->m_Header.m_Attributes.m_u16Type),
                                  (p_rpRRAnswer->m_Header.m_Attributes.m_u16Class | (p_rpRRAnswer->m_bCacheFlush ? 0x8000 : 0)),
                                  p_rpRRAnswer->m_u32TTL,
                                  u16RDLength);
            switch (header.m_Attributes.m_u16Type /*& (~0x8000)*/)      // Topmost bit might carry 'cache flush' flag
            {
#ifdef MDNS_IPV4_SUPPORT
            case DNS_RRTYPE_A:
                DEBUG_OUTPUT.printf_P(PSTR("A IP:%s"), ((clsRRAnswerA*&)p_rpRRAnswer)->m_IPAddress.toString().c_str());
                break;
#endif
            case DNS_RRTYPE_PTR:
                DEBUG_OUTPUT.printf_P(PSTR("PTR "));
                _printRRDomain(((clsRRAnswerPTR*&)p_rpRRAnswer)->m_PTRDomain);
                break;
            case DNS_RRTYPE_TXT:
            {
                size_t  stTxtLength = ((clsRRAnswerTXT*&)p_rpRRAnswer)->m_Txts.c_strLength();
                char*   pTxts = new char[stTxtLength];
                if (pTxts)
                {
                    ((clsRRAnswerTXT*&)p_rpRRAnswer)->m_Txts.c_str(pTxts);
                    DEBUG_OUTPUT.printf_P(PSTR("TXT(%u) %s"), stTxtLength, pTxts);
                    delete[] pTxts;
                }
                break;
            }
#ifdef MDNS2_IPV6_SUPPORT
            case DNS_RRTYPE_AAAA:
                DEBUG_OUTPUT.printf_P(PSTR("AAAA IP:%s"), ((clsRRAnswerAAAA*&)p_rpRRAnswer)->m_IPAddress.toString().c_str());
                break;
#endif
            case DNS_RRTYPE_SRV:
                DEBUG_OUTPUT.printf_P(PSTR("SRV Port:%u "), ((clsRRAnswerSRV*&)p_rpRRAnswer)->m_u16Port);
                _printRRDomain(((clsRRAnswerSRV*&)p_rpRRAnswer)->m_SRVDomain);
                break;
            /*  case DNS_RRTYPE_NSEC:
                DEBUG_OUTPUT.printf_P(PSTR("NSEC "));
                _printRRDomain(((stcRRAnswerNSEC*&)p_rpRRAnswer)->m_NSECDomain);
                for (uint32_t u=0; u<(((stcRRAnswerNSEC*&)p_rpRRAnswer)->m_pNSECBitmap->m_u16BitmapLength * 8); ++u) {
                    uint8_t byte = ((stcRRAnswerNSEC*&)p_rpRRAnswer)->m_pNSECBitmap->m_pu8BitmapData[u / 8];
                    uint8_t flag = 1 << (7 - (u % 8)); // (7 - (0..7)) = 7..0
                    if (byte & flag) {
                        DEBUG_OUTPUT.printf_P(PSTR(" %s"), _RRType2Name(u));
                    }
                }
                break;*/
            default:
                DEBUG_OUTPUT.printf_P(PSTR("generic "));
                break;
            }
            DEBUG_OUTPUT.printf_P(PSTR("\n"));
        });  // DEBUG_EX_INFO

        DEBUG_EX_INFO_IF(!((bResult) && (p_rpRRAnswer)),
        {
            DEBUG_OUTPUT.printf_P(PSTR("%s _readRRAnswer: FAILED to read specific answer of type 0x%04X!\n"), _DH(), p_rpRRAnswer->m_Header.m_Attributes.m_u16Type);
        });
    }
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _readRRAnswer: FAILED!\n"), _DH()););
    return bResult;
}

#ifdef MDNS_IPV4_SUPPORT
/*
    MDNSResponder::_readRRAnswerA
*/
bool clsLEAMDNSHost::_readRRAnswerA(clsLEAMDNSHost::clsRRAnswerA& p_rRRAnswerA,
                                    uint16_t p_u16RDLength)
{
    uint32_t    u32IPv4Address;
    bool        bResult = ((clsConsts::u16IPv4Size == p_u16RDLength) &&
                           (_udpReadBuffer((unsigned char*)&u32IPv4Address, clsConsts::u16IPv4Size)) &&
                           ((p_rRRAnswerA.m_IPAddress = IPAddress(u32IPv4Address))));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _readRRAnswerA: FAILED!\n"), _DH()););
    return bResult;
}
#endif

/*
    MDNSResponder::_readRRAnswerPTR
*/
bool clsLEAMDNSHost::_readRRAnswerPTR(clsLEAMDNSHost::clsRRAnswerPTR& p_rRRAnswerPTR,
                                      uint16_t p_u16RDLength)
{
    bool    bResult = ((p_u16RDLength) &&
                       (_readRRDomain(p_rRRAnswerPTR.m_PTRDomain)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _readRRAnswerPTR: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_readRRAnswerTXT

    Read TXT items from a buffer like 4c#=15ff=20
*/
bool clsLEAMDNSHost::_readRRAnswerTXT(clsLEAMDNSHost::clsRRAnswerTXT& p_rRRAnswerTXT,
                                      uint16_t p_u16RDLength)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _readRRAnswerTXT: RDLength:%u\n"), _DH(), p_u16RDLength););
    bool    bResult = true;

    p_rRRAnswerTXT.clear();
    if (p_u16RDLength)
    {
        bResult = false;

        unsigned char*  pucBuffer = new unsigned char[p_u16RDLength];
        if (pucBuffer)
        {
            if (_udpReadBuffer(pucBuffer, p_u16RDLength))
            {
                bResult = true;

                const unsigned char*    pucCursor = pucBuffer;
                while ((pucCursor < (pucBuffer + p_u16RDLength)) &&
                        (bResult))
                {
                    bResult = false;

                    clsServiceTxt*      pTxt = 0;
                    unsigned char   ucLength = *pucCursor++;    // Length of the next txt item
                    if (ucLength)
                    {
                        DEBUG_EX_INFO(
                            char sacBuffer[64];
                            *sacBuffer = 0;
                            uint8_t u8MaxLength = ((ucLength > (sizeof(sacBuffer) - 1)) ? (sizeof(sacBuffer) - 1) : ucLength);
                            os_strncpy(sacBuffer, (const char*)pucCursor, u8MaxLength + 1);
                            sacBuffer[u8MaxLength] = 0;
                            DEBUG_OUTPUT.printf_P(PSTR("%s _readRRAnswerTXT: Item(%u): %s\n"), _DH(), ucLength, sacBuffer);
                        );

                        unsigned char*  pucEqualSign = (unsigned char*)os_strchr((const char*)pucCursor, '=');  // Position of the '=' sign
                        unsigned char   ucKeyLength;
                        if ((pucEqualSign) &&
                                ((ucKeyLength = (pucEqualSign - pucCursor))))
                        {
                            unsigned char   ucValueLength = (ucLength - (pucEqualSign - pucCursor + 1));
                            bResult = (((pTxt = new clsServiceTxt)) &&
                                       (pTxt->setKey((const char*)pucCursor, ucKeyLength)) &&
                                       (pTxt->setValue((const char*)(pucEqualSign + 1), ucValueLength)));
                        }
                        else
                        {
                            DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _readRRAnswerTXT: INVALID TXT format (No '=')!\n"), _DH()););
                        }
                        pucCursor += ucLength;
                    }
                    else    // no/zero length TXT
                    {
                        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _readRRAnswerTXT: INFO! TXT answer contains no items.\n"), _DH()););
                        bResult = true;
                    }

                    if ((bResult) &&
                            (pTxt))
                    {
                        // Everythings fine so far
                        // Link TXT item to answer TXTs
                        p_rRRAnswerTXT.m_Txts.add(pTxt);
                    }
                    else
                    {
                        // At least no TXT (migth be OK, if length was 0) OR an error
                        if (!bResult)
                        {
                            DEBUG_EX_ERR(
                                DEBUG_OUTPUT.printf_P(PSTR("%s _readRRAnswerTXT: FAILED to read TXT item!\n"), _DH());
                                DEBUG_OUTPUT.printf_P(PSTR("RData dump:\n"));
                                _udpDump((m_pUDPContext->tell() - p_u16RDLength), p_u16RDLength);
                                DEBUG_OUTPUT.printf_P(PSTR("\n"));
                            );
                        }
                        if (pTxt)
                        {
                            delete pTxt;
                            pTxt = 0;
                        }
                        p_rRRAnswerTXT.clear();
                    }
                }   // while

                DEBUG_EX_ERR(
                    if (!bResult)   // Some failure
            {
                DEBUG_OUTPUT.printf_P(PSTR("RData dump:\n"));
                    _udpDump((m_pUDPContext->tell() - p_u16RDLength), p_u16RDLength);
                    DEBUG_OUTPUT.printf_P(PSTR("\n"));
                }
                );
            }
            else
            {
                DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _readRRAnswerTXT: FAILED to read TXT content!\n"), _DH()););
            }
            // Clean up
            delete[] pucBuffer;
        }
        else
        {
            DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _readRRAnswerTXT: FAILED to alloc buffer for TXT content!\n"), _DH()););
        }
    }
    else
    {
        DEBUG_EX_ERR(
            DEBUG_OUTPUT.printf_P(PSTR("%s _readRRAnswerTXT: WARNING! No content in TXT answer from "), _DH());
            _printRRDomain(p_rRRAnswerTXT.m_Header.m_Domain);
            DEBUG_OUTPUT.printf_P(PSTR("\n"));
        );
    }
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _readRRAnswerTXT: FAILED!\n"), _DH()););
    return bResult;
}

#ifdef MDNS2_IPV6_SUPPORT
bool clsLEAMDNSHost::_readRRAnswerAAAA(clsLEAMDNSHost::clsRRAnswerAAAA& p_rRRAnswerAAAA,
                                       uint16_t p_u16RDLength)
{
    bool	bResult = false;

    uint32_t	au32IPv6Address[4];	// 16 bytes
    if ((bResult = ((clsConsts::u16IPv6Size == p_u16RDLength) &&
                    (_udpReadBuffer((uint8_t*)&au32IPv6Address[0], clsConsts::u16IPv6Size)))))
    {
        // ?? IPADDR6_INIT_HOST ??
        ip_addr_t	addr = IPADDR6_INIT(au32IPv6Address[0], au32IPv6Address[1], au32IPv6Address[2], au32IPv6Address[3]);
        p_rRRAnswerAAAA.m_IPAddress = IPAddress(addr);
    }
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _readRRAnswerAAAA: FAILED!\n"), _DH()););
    return bResult;
}
#endif

/*
    MDNSResponder::_readRRAnswerSRV
*/
bool clsLEAMDNSHost::_readRRAnswerSRV(clsLEAMDNSHost::clsRRAnswerSRV& p_rRRAnswerSRV,
                                      uint16_t p_u16RDLength)
{
    bool    bResult = (((3 * sizeof(uint16_t)) < p_u16RDLength) &&
                       (_udpRead16(p_rRRAnswerSRV.m_u16Priority)) &&
                       (_udpRead16(p_rRRAnswerSRV.m_u16Weight)) &&
                       (_udpRead16(p_rRRAnswerSRV.m_u16Port)) &&
                       (_readRRDomain(p_rRRAnswerSRV.m_SRVDomain)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _readRRAnswerSRV: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_readRRAnswerGeneric
*/
bool clsLEAMDNSHost::_readRRAnswerGeneric(clsLEAMDNSHost::clsRRAnswerGeneric& p_rRRAnswerGeneric,
        uint16_t p_u16RDLength)
{
    bool    bResult = (0 == p_u16RDLength);

    p_rRRAnswerGeneric.clear();
    if (((p_rRRAnswerGeneric.m_u16RDLength = p_u16RDLength)) &&
            ((p_rRRAnswerGeneric.m_pu8RDData = new unsigned char[p_rRRAnswerGeneric.m_u16RDLength])))
    {
        bResult = _udpReadBuffer(p_rRRAnswerGeneric.m_pu8RDData, p_rRRAnswerGeneric.m_u16RDLength);
    }
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _readRRAnswerGeneric: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_readRRHeader
*/
bool clsLEAMDNSHost::_readRRHeader(clsLEAMDNSHost::clsRRHeader& p_rRRHeader)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _readRRHeader\n")););

    bool    bResult = ((_readRRDomain(p_rRRHeader.m_Domain)) &&
                       (_readRRAttributes(p_rRRHeader.m_Attributes)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _readRRHeader: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_readRRDomain

    Reads a (maybe multilevel compressed) domain from the UDP input buffer.

*/
bool clsLEAMDNSHost::_readRRDomain(clsLEAMDNSHost::clsRRDomain& p_rRRDomain)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _readRRDomain\n")););

    bool    bResult = ((p_rRRDomain.clear()) &&
                       (_readRRDomain_Loop(p_rRRDomain, 0)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _readRRDomain: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_readRRDomain_Loop

    Reads a domain from the UDP input buffer. For every compression level, the functions
    calls itself recursively. To avoid endless recursion because of malformed MDNS records,
    the maximum recursion depth is set by clsConsts::u8DomainMaxRedirections.

*/
bool clsLEAMDNSHost::_readRRDomain_Loop(clsLEAMDNSHost::clsRRDomain& p_rRRDomain,
                                        uint8_t p_u8Depth)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _readRRDomain_Loop(%u)\n"), _DH(), p_u8Depth););

    bool    bResult = false;

    if (clsConsts::u8DomainMaxRedirections >= p_u8Depth)
    {
        bResult = true;

        uint8_t u8Len = 0;
        do
        {
            //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _readRRDomain_Loop(%u): Offset:%u p0:%02x\n"), _DH(), p_u8Depth, m_pUDPContext->tell(), m_pUDPContext->peek()););
            _udpRead8(u8Len);

            if (u8Len & clsConsts::u8DomainCompressMark)
            {
                // Compressed label(s)
                uint16_t    u16Offset = ((u8Len & ~clsConsts::u8DomainCompressMark) << 8);    // Implicit BE to LE conversion!
                _udpRead8(u8Len);
                u16Offset |= u8Len;

                if (m_pUDPContext->isValidOffset(u16Offset))
                {
                    size_t  stCurrentPosition = m_pUDPContext->tell();      // Prepare return from recursion

                    //DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("%s _readRRDomain_Loop(%u): Redirecting from %u to %u!\n"), _DH(), p_u8Depth, stCurrentPosition, u16Offset););
                    m_pUDPContext->seek(u16Offset);
                    if (_readRRDomain_Loop(p_rRRDomain, p_u8Depth + 1))     // Do recursion
                    {
                        //DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("%s _readRRDomain_Loop(%u): Succeeded to read redirected label! Returning to %u\n"), _DH(), p_u8Depth, stCurrentPosition););
                        m_pUDPContext->seek(stCurrentPosition);             // Restore after recursion
                    }
                    else
                    {
                        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _readRRDomain_Loop(%u): FAILED to read redirected label!\n"), _DH(), p_u8Depth););
                        bResult = false;
                    }
                }
                else
                {
                    DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _readRRDomain_Loop(%u): INVALID offset in redirection!\n"), _DH(), p_u8Depth););
                    bResult = false;
                }
                break;
            }
            else
            {
                // Normal (uncompressed) label (maybe '\0' only)
                if (clsConsts::stDomainMaxLength > (p_rRRDomain.m_u16NameLength + u8Len))
                {
                    // Add length byte
                    p_rRRDomain.m_acName[p_rRRDomain.m_u16NameLength] = u8Len;
                    ++(p_rRRDomain.m_u16NameLength);
                    if (u8Len)      // Add name
                    {
                        if ((bResult = _udpReadBuffer((unsigned char*) & (p_rRRDomain.m_acName[p_rRRDomain.m_u16NameLength]), u8Len)))
                        {
                            /*  DEBUG_EX_INFO(
                                    p_rRRDomain.m_acName[p_rRRDomain.m_u16NameLength + u8Len] = 0;  // Closing '\0' for printing
                                    DEBUG_OUTPUT.printf_P(PSTR("%s _readRRDomain_Loop(%u): Domain label (%u): %s\n"), _DH(), p_u8Depth, (unsigned)(p_rRRDomain.m_acName[p_rRRDomain.m_u16NameLength - 1]), &(p_rRRDomain.m_acName[p_rRRDomain.m_u16NameLength]));
                                    );*/

                            p_rRRDomain.m_u16NameLength += u8Len;
                        }
                    }
                    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _readRRDomain_Loop(2) offset:%u p0:%x\n"), _DH(), m_pUDPContext->tell(), m_pUDPContext->peek()););
                }
                else
                {
                    DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _readRRDomain_Loop(%u): ERROR! Domain name too long (%u + %u)!\n"), _DH(), p_u8Depth, p_rRRDomain.m_u16NameLength, u8Len););
                    bResult = false;
                    break;
                }
            }
        } while ((bResult) &&
                 (0 != u8Len));
    }
    else
    {
        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _readRRDomain_Loop(%u): ERROR! Too many redirections!\n"), _DH(), p_u8Depth););
    }
    return bResult;
}

/*
    MDNSResponder::_readRRAttributes

*/
bool clsLEAMDNSHost::_readRRAttributes(clsLEAMDNSHost::clsRRAttributes& p_rRRAttributes)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _readRRAttributes\n")););

    bool    bResult = ((_udpRead16(p_rRRAttributes.m_u16Type)) &&
                       (_udpRead16(p_rRRAttributes.m_u16Class)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _readRRAttributes: FAILED!\n"), _DH()););
    return bResult;
}


/*

    DOMAIN NAMES

*/

/*
    MDNSResponder::_buildDomainForHost

    Builds a MDNS host domain (eg. esp8266.local) for the given hostname.

*/
bool clsLEAMDNSHost::_buildDomainForHost(const char* p_pcHostName,
        clsLEAMDNSHost::clsRRDomain& p_rHostDomain) const
{

    p_rHostDomain.clear();
    bool    bResult = ((p_pcHostName) &&
                       (*p_pcHostName) &&
                       (p_rHostDomain.addLabel(p_pcHostName)) &&
                       (p_rHostDomain.addLabel(clsConsts::pcLocal)) &&
                       (p_rHostDomain.addLabel(0)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _buildDomainForHost: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_buildDomainForDNSSD

    Builds the '_services._dns-sd._udp.local' domain.
    Used while detecting generic service enum question (DNS-SD) and answering these questions.

*/
bool clsLEAMDNSHost::_buildDomainForDNSSD(clsLEAMDNSHost::clsRRDomain& p_rDNSSDDomain) const
{
    p_rDNSSDDomain.clear();
    bool    bResult = ((p_rDNSSDDomain.addLabel(clsConsts::pcServices, true)) &&
                       (p_rDNSSDDomain.addLabel(clsConsts::pcDNSSD, true)) &&
                       (p_rDNSSDDomain.addLabel(clsConsts::pcUDP, true)) &&
                       (p_rDNSSDDomain.addLabel(clsConsts::pcLocal)) &&
                       (p_rDNSSDDomain.addLabel(0)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _buildDomainForDNSSD: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_buildDomainForService

    Builds the domain for the given service (eg. _http._tcp.local or
    MyESP._http._tcp.local (if p_bIncludeName is set)).

*/
bool clsLEAMDNSHost::_buildDomainForService(const clsLEAMDNSHost::clsService& p_Service,
        bool p_bIncludeName,
        clsLEAMDNSHost::clsRRDomain& p_rServiceDomain) const
{
    p_rServiceDomain.clear();
    bool    bResult = (((!p_bIncludeName) ||
                        (p_rServiceDomain.addLabel(p_Service.instanceName()))) &&
                       (p_rServiceDomain.addLabel(p_Service.type(), ('_' != *p_Service.type()))) &&
                       (p_rServiceDomain.addLabel(p_Service.protocol(), ('_' != *p_Service.protocol()))) &&
                       (p_rServiceDomain.addLabel(clsConsts::pcLocal)) &&
                       (p_rServiceDomain.addLabel(0)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _buildDomainForService: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_buildDomainForService

    Builds the domain for the given service properties (eg. _http._tcp.local).
    The usual prepended '_' are added, if missing in the input strings.

*/
bool clsLEAMDNSHost::_buildDomainForService(const char* p_pcServiceType,
        const char* p_pcProtocol,
        clsLEAMDNSHost::clsRRDomain& p_rServiceDomain) const
{
    p_rServiceDomain.clear();
    bool    bResult = ((p_pcServiceType) &&
                       (p_pcProtocol) &&
                       (p_rServiceDomain.addLabel(p_pcServiceType, ('_' != *p_pcServiceType))) &&
                       (p_rServiceDomain.addLabel(p_pcProtocol, ('_' != *p_pcProtocol))) &&
                       (p_rServiceDomain.addLabel(clsConsts::pcLocal)) &&
                       (p_rServiceDomain.addLabel(0)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _buildDomainForService: FAILED for (%s.%s)!\n"), _DH(), (p_pcServiceType ? : "-"), (p_pcProtocol ? : "-")););
    return bResult;
}

#ifdef MDNS_IPV4_SUPPORT
/*
    MDNSResponder::_buildDomainForReverseIPv4

    The IPv4 address is stringized by printing the four address bytes into a char buffer in reverse order
    and adding 'in-addr.arpa' (eg. 012.789.456.123.in-addr.arpa).
    Used while detecting reverse IPv4 questions and answering these

*/
bool clsLEAMDNSHost::_buildDomainForReverseIPv4(IPAddress p_IPv4Address,
        clsLEAMDNSHost::clsRRDomain& p_rReverseIPv4Domain) const
{
    bool    bResult = ((p_IPv4Address.isSet()) &&
                       (p_IPv4Address.isV4()));

    p_rReverseIPv4Domain.clear();

    char    acBuffer[32];
    for (int i = clsConsts::u16IPv4Size; ((bResult) && (i >= 1)); --i)
    {
        itoa(p_IPv4Address[i - 1], acBuffer, 10);
        bResult = p_rReverseIPv4Domain.addLabel(acBuffer);
    }
    bResult = ((bResult) &&
               (p_rReverseIPv4Domain.addLabel(clsConsts::pcReverseIPv4Domain)) &&
               (p_rReverseIPv4Domain.addLabel(clsConsts::pcReverseTopDomain)) &&
               (p_rReverseIPv4Domain.addLabel(0)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _buildDomainForReverseIPv4: FAILED!\n"), _DH()););
    return bResult;
}
#endif

#ifdef MDNS2_IPV6_SUPPORT
/*
    MDNSResponder::_buildDomainForReverseIPv6

    The IPv6 address is stringized by printing the 16 address bytes (32 nibbles) into a char buffer in reverse order
    and adding 'ip6.arpa' (eg. 3.B.6.E.A.1.B.B.A.B.F.7.F.8.0.1.0.0.0.0.0.0.0.0.0.0.0.0.0.8.E.F.ip6.arpa).
    Used while detecting reverse IPv6 questions and answering these

*/
bool clsLEAMDNSHost::_buildDomainForReverseIPv6(IPAddress p_IPv6Address,
        clsLEAMDNSHost::clsRRDomain& p_rReverseIPv6Domain) const
{
    bool    bResult = ((p_IPv6Address.isSet()) &&
                       (p_IPv6Address.isV6()));

    p_rReverseIPv6Domain.clear();

    const uint16_t* pRaw = p_IPv6Address.raw6();
    for (int8_t i8 = (clsConsts::u16IPv6Size / 2); ((bResult) && (i8 > 0)); --i8) // 8..1
    {
        uint16_t	u16Part = ntohs(pRaw[i8 - 1] & 0xFFFF);
        char        acBuffer[2];
        for (uint8_t u8 = 0; ((bResult) && (u8 < 4)); ++u8)             // 0..3
        {
            itoa((u16Part & 0xF), acBuffer, 16);
            bResult = p_rReverseIPv6Domain.addLabel(acBuffer);
            u16Part >>= 4;
        }
    }
    bResult = ((bResult) &&
               (p_rReverseIPv6Domain.addLabel(clsConsts::pcReverseIPv6Domain)) &&    // .ip6.arpa
               (p_rReverseIPv6Domain.addLabel(clsConsts::pcReverseTopDomain)) &&     // .local
               (p_rReverseIPv6Domain.addLabel(0)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _buildDomainForReverseIPv6: FAILED!\n"), _DH()););
    return bResult;
}
#endif


/*

    UDP

*/

/*
    MDNSResponder::_udpReadBuffer

*/
bool clsLEAMDNSHost::_udpReadBuffer(unsigned char* p_pBuffer,
                                    size_t p_stLength)
{
    bool    bResult = ((m_pUDPContext->getSize() >= p_stLength) &&
                       (p_pBuffer) &&
                       (p_stLength) &&
                       ((p_stLength == m_pUDPContext->read((char*)p_pBuffer, p_stLength))));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _udpReadBuffer: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_udpRead8

*/
bool clsLEAMDNSHost::_udpRead8(uint8_t& p_ru8Value)
{
    return _udpReadBuffer((unsigned char*)&p_ru8Value, sizeof(p_ru8Value));
}

/*
    MDNSResponder::_udpRead16

*/
bool clsLEAMDNSHost::_udpRead16(uint16_t& p_ru16Value)
{
    bool    bResult = false;

    if (_udpReadBuffer((unsigned char*)&p_ru16Value, sizeof(p_ru16Value)))
    {
        p_ru16Value = lwip_ntohs(p_ru16Value);
        bResult = true;
    }
    return bResult;
}

/*
    MDNSResponder::_udpRead32

*/
bool clsLEAMDNSHost::_udpRead32(uint32_t& p_ru32Value)
{
    bool    bResult = false;

    if (_udpReadBuffer((unsigned char*)&p_ru32Value, sizeof(p_ru32Value)))
    {
        p_ru32Value = lwip_ntohl(p_ru32Value);
        bResult = true;
    }
    return bResult;
}

/*
    MDNSResponder::_udpAppendBuffer

*/
bool clsLEAMDNSHost::_udpAppendBuffer(const unsigned char* p_pcBuffer,
                                      size_t p_stLength)
{
    bool bResult = ((p_pcBuffer) &&
                    (p_stLength) &&
                    (p_stLength == m_pUDPContext->append((const char*)p_pcBuffer, p_stLength)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _udpAppendBuffer: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_udpAppend8

*/
bool clsLEAMDNSHost::_udpAppend8(uint8_t p_u8Value)
{
    return (_udpAppendBuffer((unsigned char*)&p_u8Value, sizeof(p_u8Value)));
}

/*
    MDNSResponder::_udpAppend16

*/
bool clsLEAMDNSHost::_udpAppend16(uint16_t p_u16Value)
{
    p_u16Value = lwip_htons(p_u16Value);
    return (_udpAppendBuffer((unsigned char*)&p_u16Value, sizeof(p_u16Value)));
}

/*
    MDNSResponder::_udpAppend32

*/
bool clsLEAMDNSHost::_udpAppend32(uint32_t p_u32Value)
{
    p_u32Value = lwip_htonl(p_u32Value);
    return (_udpAppendBuffer((unsigned char*)&p_u32Value, sizeof(p_u32Value)));
}

#ifdef DEBUG_ESP_PORT
/*
    MDNSResponder::_udpDump

*/
bool clsLEAMDNSHost::_udpDump(bool p_bMovePointer /*= false*/)
{
    const uint8_t   cu8BytesPerLine = 16;

    uint32_t        u32StartPosition = m_pUDPContext->tell();
    DEBUG_OUTPUT.println("UDP Context Dump:");
    uint32_t    u32Counter = 0;
    uint8_t     u8Byte = 0;

    while (_udpRead8(u8Byte))
    {
        DEBUG_OUTPUT.printf_P(PSTR("%02x %s"), u8Byte, ((++u32Counter % cu8BytesPerLine) ? "" : "\n"));
    }
    DEBUG_OUTPUT.printf_P(PSTR("%sDone: %u bytes\n"), (((u32Counter) && (u32Counter % cu8BytesPerLine)) ? "\n" : ""), u32Counter);

    if (!p_bMovePointer)    // Restore
    {
        m_pUDPContext->seek(u32StartPosition);
    }
    return true;
}

/*
    MDNSResponder::_udpDump

*/
bool clsLEAMDNSHost::_udpDump(unsigned p_uOffset,
                              unsigned p_uLength)
{
    if (m_pUDPContext->isValidOffset(p_uOffset))
    {
        unsigned    uCurrentPosition = m_pUDPContext->tell();   // Remember start position

        m_pUDPContext->seek(p_uOffset);
        uint8_t u8Byte;
        for (unsigned u = 0; ((u < p_uLength) && (_udpRead8(u8Byte))); ++u)
        {
            DEBUG_OUTPUT.printf_P(PSTR("%02x "), u8Byte);
        }
        // Return to start position
        m_pUDPContext->seek(uCurrentPosition);
    }
    return true;
}
#endif // DEBUG_ESP_PORT


/**
    READ/WRITE MDNS STRUCTS
*/

/*
    MDNSResponder::_readMDNSMsgHeader

    Read a MDNS header from the UDP input buffer.
      |   8    |   8    |   8    |   8    |
    00|   Identifier    |  Flags & Codes  |
    01| Question count  |  Answer count   |
    02| NS answer count | Ad answer count |

    All 16-bit and 32-bit elements need to be translated from network coding to host coding (done in _udpRead16 and _udpRead32)
    In addition, bitfield memory order is undefined in C standard (GCC doesn't order them in the coded direction...), so they
    need some mapping here

*/
bool clsLEAMDNSHost::_readMDNSMsgHeader(clsLEAMDNSHost::clsMsgHeader& p_rMsgHeader)
{
    bool    bResult = false;

    uint8_t u8B1;
    uint8_t u8B2;
    if ((_udpRead16(p_rMsgHeader.m_u16ID)) &&
            (_udpRead8(u8B1)) &&
            (_udpRead8(u8B2)) &&
            (_udpRead16(p_rMsgHeader.m_u16QDCount)) &&
            (_udpRead16(p_rMsgHeader.m_u16ANCount)) &&
            (_udpRead16(p_rMsgHeader.m_u16NSCount)) &&
            (_udpRead16(p_rMsgHeader.m_u16ARCount)))
    {

        p_rMsgHeader.m_1bQR     = (u8B1 & 0x80);    // Query/Response flag
        p_rMsgHeader.m_4bOpcode = (u8B1 & 0x78);    // Operation code (0: Standard query, others ignored)
        p_rMsgHeader.m_1bAA     = (u8B1 & 0x04);    // Authorative answer
        p_rMsgHeader.m_1bTC     = (u8B1 & 0x02);    // Truncation flag
        p_rMsgHeader.m_1bRD     = (u8B1 & 0x01);    // Recursion desired

        p_rMsgHeader.m_1bRA     = (u8B2 & 0x80);    // Recursion available
        p_rMsgHeader.m_3bZ      = (u8B2 & 0x70);    // Zero
        p_rMsgHeader.m_4bRCode  = (u8B2 & 0x0F);    // Response code

        /*  DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _readMDNSMsgHeader: ID:%u QR:%u OP:%u AA:%u TC:%u RD:%u RA:%u R:%u QD:%u AN:%u NS:%u AR:%u\n"),
                _DH(),
                (unsigned)p_rMsgHeader.m_u16ID,
                (unsigned)p_rMsgHeader.m_1bQR, (unsigned)p_rMsgHeader.m_4bOpcode, (unsigned)p_rMsgHeader.m_1bAA, (unsigned)p_rMsgHeader.m_1bTC, (unsigned)p_rMsgHeader.m_1bRD,
                (unsigned)p_rMsgHeader.m_1bRA, (unsigned)p_rMsgHeader.m_4bRCode,
                (unsigned)p_rMsgHeader.m_u16QDCount,
                (unsigned)p_rMsgHeader.m_u16ANCount,
                (unsigned)p_rMsgHeader.m_u16NSCount,
                (unsigned)p_rMsgHeader.m_u16ARCount););*/
        bResult = true;
    }
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _readMDNSMsgHeader: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_write8

*/
bool clsLEAMDNSHost::_write8(uint8_t p_u8Value,
                             clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    return ((_udpAppend8(p_u8Value)) &&
            (p_rSendParameter.shiftOffset(sizeof(p_u8Value))));
}

/*
    MDNSResponder::_write16

*/
bool clsLEAMDNSHost::_write16(uint16_t p_u16Value,
                              clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    return ((_udpAppend16(p_u16Value)) &&
            (p_rSendParameter.shiftOffset(sizeof(p_u16Value))));
}

/*
    MDNSResponder::_write32

*/
bool clsLEAMDNSHost::_write32(uint32_t p_u32Value,
                              clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    return ((_udpAppend32(p_u32Value)) &&
            (p_rSendParameter.shiftOffset(sizeof(p_u32Value))));
}

/*
    MDNSResponder::_writeMDNSMsgHeader

    Write MDNS header to the UDP output buffer.

    All 16-bit and 32-bit elements need to be translated from host coding to network coding (done in _udpAppend16 and _udpAppend32)
    In addition, bitfield memory order is undefined in C standard (GCC doesn't order them in the coded direction...), so they
    need some mapping here

*/
bool clsLEAMDNSHost::_writeMDNSMsgHeader(const clsLEAMDNSHost::clsMsgHeader& p_MsgHeader,
        clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    /*  DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSMsgHeader: ID:%u QR:%u OP:%u AA:%u TC:%u RD:%u RA:%u R:%u QD:%u AN:%u NS:%u AR:%u\n"),
            _DH(),
            (unsigned)p_MsgHeader.m_u16ID,
            (unsigned)p_MsgHeader.m_1bQR, (unsigned)p_MsgHeader.m_4bOpcode, (unsigned)p_MsgHeader.m_1bAA, (unsigned)p_MsgHeader.m_1bTC, (unsigned)p_MsgHeader.m_1bRD,
            (unsigned)p_MsgHeader.m_1bRA, (unsigned)p_MsgHeader.m_4bRCode,
            (unsigned)p_MsgHeader.m_u16QDCount,
            (unsigned)p_MsgHeader.m_u16ANCount,
            (unsigned)p_MsgHeader.m_u16NSCount,
            (unsigned)p_MsgHeader.m_u16ARCount););*/

    uint8_t u8B1((p_MsgHeader.m_1bQR << 7) | (p_MsgHeader.m_4bOpcode << 3) | (p_MsgHeader.m_1bAA << 2) | (p_MsgHeader.m_1bTC << 1) | (p_MsgHeader.m_1bRD));
    uint8_t u8B2((p_MsgHeader.m_1bRA << 7) | (p_MsgHeader.m_3bZ << 4) | (p_MsgHeader.m_4bRCode));
    bool    bResult = ((_write16(p_MsgHeader.m_u16ID, p_rSendParameter)) &&
                       (_write8(u8B1, p_rSendParameter)) &&
                       (_write8(u8B2, p_rSendParameter)) &&
                       (_write16(p_MsgHeader.m_u16QDCount, p_rSendParameter)) &&
                       (_write16(p_MsgHeader.m_u16ANCount, p_rSendParameter)) &&
                       (_write16(p_MsgHeader.m_u16NSCount, p_rSendParameter)) &&
                       (_write16(p_MsgHeader.m_u16ARCount, p_rSendParameter)));

    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSMsgHeader: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_writeRRAttributes

*/
bool clsLEAMDNSHost::_writeMDNSRRAttributes(const clsLEAMDNSHost::clsRRAttributes& p_Attributes,
        clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    bool    bResult = ((_write16(p_Attributes.m_u16Type, p_rSendParameter)) &&
                       (_write16(p_Attributes.m_u16Class, p_rSendParameter)));

    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSRRAttributes: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_writeMDNSRRDomain

*/
bool clsLEAMDNSHost::_writeMDNSRRDomain(const clsLEAMDNSHost::clsRRDomain& p_Domain,
                                        clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    bool    bResult = ((_udpAppendBuffer((const unsigned char*)p_Domain.m_acName, p_Domain.m_u16NameLength)) &&
                       (p_rSendParameter.shiftOffset(p_Domain.m_u16NameLength)));

    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSRRDomain: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_writeMDNSHostDomain

    Write a host domain to the UDP output buffer.
    If the domain record is part of the answer, the records length is
    prepended (p_bPrependRDLength is set).

    A very simple form of name compression is applied here:
    If the domain is written to the UDP output buffer, the write offset is stored
    together with a domain id (the pointer) in a p_rSendParameter substructure (cache).
    If the same domain (pointer) should be written to the UDP output later again,
    the old offset is retrieved from the cache, marked as a compressed domain offset
    and written to the output buffer.

*/
bool clsLEAMDNSHost::_writeMDNSHostDomain(const char* p_pcHostName,
        bool p_bPrependRDLength,
        uint16_t p_u16AdditionalLength,
        clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    // The 'skip-compression' version is handled in '_writeMDNSAnswer_SRV'
    uint16_t            u16CachedDomainOffset = p_rSendParameter.findCachedDomainOffset((const void*)p_pcHostName, false);

    clsRRDomain    hostDomain;
    bool    bResult = (u16CachedDomainOffset
                       // Found cached domain -> mark as compressed domain
                       ? ((clsConsts::u8DomainCompressMark > ((u16CachedDomainOffset >> 8) & ~clsConsts::u8DomainCompressMark)) && 	// Valid offset
                          ((!p_bPrependRDLength) ||
                           (_write16((2 + p_u16AdditionalLength), p_rSendParameter))) &&                          	// Length of 'Cxxx'
                          (_write8(((u16CachedDomainOffset >> 8) | clsConsts::u8DomainCompressMark), p_rSendParameter)) && 	// Compression mark (and offset)
                          (_write8((uint8_t)(u16CachedDomainOffset & 0xFF), p_rSendParameter)))
                       // No cached domain -> add this domain to cache and write full domain name
                       : ((_buildDomainForHost(p_pcHostName, hostDomain)) &&                                     	// eg. esp8266.local
                          ((!p_bPrependRDLength) ||
                           (_write16((hostDomain.m_u16NameLength + p_u16AdditionalLength), p_rSendParameter))) && 	// RDLength (if needed)
                          (p_rSendParameter.addDomainCacheItem((const void*)p_pcHostName, false, p_rSendParameter.m_u16Offset)) &&
                          (_writeMDNSRRDomain(hostDomain, p_rSendParameter))));

    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSHostDomain: FAILED!\n"), _DH()););
    return bResult;

}

/*
    MDNSResponder::_writeMDNSServiceDomain

    Write a service domain to the UDP output buffer.
    If the domain record is part of the answer, the records length is
    prepended (p_bPrependRDLength is set).

    A very simple form of name compression is applied here: see '_writeMDNSHostDomain'
    The cache differentiates of course between service domains which includes
    the instance name (p_bIncludeName is set) and thoose who don't.

*/
bool clsLEAMDNSHost::_writeMDNSServiceDomain(const clsLEAMDNSHost::clsService& p_Service,
        bool p_bIncludeName,
        bool p_bPrependRDLength,
        uint16_t p_u16AdditionalLength,
        clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    // The 'skip-compression' version is handled in '_writeMDNSAnswer_SRV'
    uint16_t            u16CachedDomainOffset = p_rSendParameter.findCachedDomainOffset((const void*)&p_Service, p_bIncludeName);

    clsRRDomain    serviceDomain;
    bool    bResult = (u16CachedDomainOffset
                       // Found cached domain -> mark as compressed domain
                       ? ((clsConsts::u8DomainCompressMark > ((u16CachedDomainOffset >> 8) & ~clsConsts::u8DomainCompressMark)) &&		// Valid offset
                          ((!p_bPrependRDLength) ||
                           (_write16((2 + p_u16AdditionalLength), p_rSendParameter))) &&                           		// Lenght of 'Cxxx'
                          (_write8(((u16CachedDomainOffset >> 8) | clsConsts::u8DomainCompressMark), p_rSendParameter)) &&   	// Compression mark (and offset)
                          (_write8((uint8_t)(u16CachedDomainOffset & 0xFF), p_rSendParameter)))
                       // No cached domain -> add this domain to cache and write full domain name
                       : ((_buildDomainForService(p_Service, p_bIncludeName, serviceDomain)) &&                        	// eg. MyESP._http._tcp.local
                          ((!p_bPrependRDLength) ||
                           (_write16((serviceDomain.m_u16NameLength + p_u16AdditionalLength), p_rSendParameter))) &&	// RDLength (if needed)
                          (p_rSendParameter.addDomainCacheItem((const void*)&p_Service, p_bIncludeName, p_rSendParameter.m_u16Offset)) &&
                          (_writeMDNSRRDomain(serviceDomain, p_rSendParameter))));

    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSServiceDomain: FAILED!\n"), _DH()););
    return bResult;

}

/*
    MDNSResponder::_writeMDNSQuestion

    Write a MDNS question to the UDP output buffer

    QNAME  (host/service domain, eg. esp8266.local)
    QTYPE  (16bit, eg. ANY)
    QCLASS (16bit, eg. IN)

*/
bool clsLEAMDNSHost::_writeMDNSQuestion(clsLEAMDNSHost::clsRRQuestion& p_Question,
                                        clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSQuestion\n")););

    bool    bResult = ((_writeMDNSRRDomain(p_Question.m_Header.m_Domain, p_rSendParameter)) &&
                       (_writeMDNSRRAttributes(p_Question.m_Header.m_Attributes, p_rSendParameter)));

    DEBUG_EX_INFO(if (bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSQuestion "), _DH());
        _printRRDomain(p_Question.m_Header.m_Domain);
        DEBUG_OUTPUT.printf_P(PSTR(" Type:%s Class:0x%04X\n"),
                              _RRType2Name(p_Question.m_Header.m_Attributes.m_u16Type),
                              p_Question.m_Header.m_Attributes.m_u16Class);
    });
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSQuestion: FAILED!\n"), _DH()););
    return bResult;
}


#ifdef MDNS_IPV4_SUPPORT
/*
    MDNSResponder::_writeMDNSAnswer_A

    Write a MDNS A answer to the UDP output buffer.

    NAME     (var, host/service domain, eg. esp8266.local
    TYPE     (16bit, eg. A)
    CLASS    (16bit, eg. IN)
    TTL      (32bit, eg. 120)
    RDLENGTH (16bit, eg 4)
    RDATA    (var, eg. 123.456.789.012)

    eg. esp8266.local A 0x8001 120 4 123.456.789.012
    Ref: http://www.zytrax.com/books/dns/ch8/a.html

*/
bool clsLEAMDNSHost::_writeMDNSAnswer_A(IPAddress p_IPAddress,
                                        clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_A (%s)%s\n"), p_IPAddress.toString().c_str(), (p_rSendParameter.m_bCacheFlush ? "" : " nF")););

    clsRRAttributes    attributes(DNS_RRTYPE_A,
                                  ((p_rSendParameter.m_bCacheFlush ? 0x8000 : 0) | DNS_RRCLASS_IN));   // Cache flush? & INternet
    const unsigned char     aucIPAddress[clsConsts::u16IPv4Size] = { p_IPAddress[0], p_IPAddress[1], p_IPAddress[2], p_IPAddress[3] };
    bool    bResult = ((p_IPAddress.isV4()) &&
                       (_writeMDNSHostDomain(m_pcHostName, false, 0, p_rSendParameter)) &&
                       (_writeMDNSRRAttributes(attributes, p_rSendParameter)) &&        // TYPE & CLASS
                       (_write32((p_rSendParameter.m_bUnannounce
                                  ? 0
                                  : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32HostTTL)), p_rSendParameter)) &&    // TTL
                       (_write16(clsConsts::u16IPv4Size, p_rSendParameter)) &&                  // RDLength
                       (_udpAppendBuffer(aucIPAddress, clsConsts::u16IPv4Size)) &&              // RData
                       (p_rSendParameter.shiftOffset(clsConsts::u16IPv4Size)));

    DEBUG_EX_INFO(if (bResult)
                  DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_A %s.local Type:%s Class:0x%04X TTL:%u %s\n"),
                                        _DH(),
                                        m_pcHostName,
                                        _RRType2Name(attributes.m_u16Type),
                                        attributes.m_u16Class,
                                        (p_rSendParameter.m_bUnannounce
                                         ? 0
                                         : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32HostTTL)),
                                        p_IPAddress.toString().c_str());
                 );
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_A: FAILED!\n"), _DH()););
    return bResult;

}

/*
    MDNSResponder::_writeMDNSAnswer_PTR_IPv4

    Write a MDNS reverse IPv4 PTR answer to the UDP output buffer.
    See: '_writeMDNSAnswer_A'

    eg. 012.789.456.123.in-addr.arpa PTR 0x8001 120 15 esp8266.local
    Used while answering reverse IPv4 questions

*/
bool clsLEAMDNSHost::_writeMDNSAnswer_PTR_IPv4(IPAddress p_IPAddress,
        clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_PTR_IPv4 (%s)%s\n"), p_IPAddress.toString().c_str(), (p_rSendParameter.m_bCacheFlush ? "" : " nF")););

    clsRRDomain        reverseIPv4Domain;
    clsRRAttributes    attributes(DNS_RRTYPE_PTR,
                                  ((p_rSendParameter.m_bCacheFlush ? 0x8000 : 0) | DNS_RRCLASS_IN));   // Cache flush? & INternet
    clsRRDomain        hostDomain;
    bool    bResult = ((p_IPAddress.isV4()) &&
                       (_buildDomainForReverseIPv4(p_IPAddress, reverseIPv4Domain)) &&	// 012.789.456.123.in-addr.arpa
                       (_writeMDNSRRDomain(reverseIPv4Domain, p_rSendParameter)) &&
                       (_writeMDNSRRAttributes(attributes, p_rSendParameter)) &&        // TYPE & CLASS
                       (_write32((p_rSendParameter.m_bUnannounce
                                  ? 0
                                  : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32HostTTL)), p_rSendParameter)) &&    // TTL
                       (_writeMDNSHostDomain(m_pcHostName, true, 0, p_rSendParameter)));   // RDLength & RData (host domain, eg. esp8266.local)

    DEBUG_EX_INFO(if (bResult)
                  DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_PTR_IPv4 "), _DH());
                  _printRRDomain(reverseIPv4Domain);
                  DEBUG_OUTPUT.printf_P(PSTR(" Type:%s Class:0x%04X TTL:%u %s.local\n"),
                                        _RRType2Name(attributes.m_u16Type),
                                        attributes.m_u16Class,
                                        (p_rSendParameter.m_bUnannounce
                                         ? 0
                                         : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32HostTTL)),
                                        m_pcHostName);
                 );
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_PTR_IPv4: FAILED!\n"), _DH()););
    return bResult;
}
#endif

/*
    MDNSResponder::_writeMDNSAnswer_PTR_TYPE

    Write a MDNS PTR answer to the UDP output buffer.
    See: '_writeMDNSAnswer_A'

    PTR all-services -> service type
    eg. _services._dns-sd._udp.local PTR 0x8001 5400 xx _http._tcp.local
    http://www.zytrax.com/books/dns/ch8/ptr.html

*/
bool clsLEAMDNSHost::_writeMDNSAnswer_PTR_TYPE(clsLEAMDNSHost::clsService& p_rService,
        clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_PTR_TYPE\n")););

    clsRRDomain        dnssdDomain;
    clsRRDomain        serviceDomain;
    clsRRAttributes    attributes(DNS_RRTYPE_PTR, DNS_RRCLASS_IN);                    	// No cache flush for shared records! only INternet
    bool    bResult = ((_buildDomainForDNSSD(dnssdDomain)) &&                               // _services._dns-sd._udp.local
                       (_writeMDNSRRDomain(dnssdDomain, p_rSendParameter)) &&
                       (_writeMDNSRRAttributes(attributes, p_rSendParameter)) &&            // TYPE & CLASS
                       (_write32((p_rSendParameter.m_bUnannounce
                                  ? 0
                                  : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32ServiceTTL)), p_rSendParameter)) && // TTL
                       (_writeMDNSServiceDomain(p_rService, false, true, 0, p_rSendParameter)));    // RDLength & RData (service domain, eg. _http._tcp.local)

    DEBUG_EX_INFO(if (bResult)
                  DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_PTR_TYPE "), _DH());
                  _printRRDomain(dnssdDomain);
                  DEBUG_OUTPUT.printf_P(PSTR(" Type:%s Class:0x%04X TTL:%u _%s._%s.local\n"),
                                        _RRType2Name(attributes.m_u16Type),
                                        attributes.m_u16Class,
                                        (p_rSendParameter.m_bUnannounce
                                         ? 0
                                         : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32ServiceTTL)),
                                        p_rService.m_pcType,
                                        p_rService.m_pcProtocol);
                 );
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_PTR_TYPE: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_writeMDNSAnswer_PTR_NAME

    Write a MDNS PTR answer to the UDP output buffer.
    See: '_writeMDNSAnswer_A'

    PTR service type -> service name
    eg. _http.tcp.local PTR 0x8001 120 xx myESP._http._tcp.local
    http://www.zytrax.com/books/dns/ch8/ptr.html

*/
bool clsLEAMDNSHost::_writeMDNSAnswer_PTR_NAME(clsLEAMDNSHost::clsService& p_rService,
        clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_PTR_NAME\n"), _DH()););

    clsRRAttributes    attributes(DNS_RRTYPE_PTR, DNS_RRCLASS_IN);	                            // No cache flush for shared records! only INternet
    bool    bResult = ((_writeMDNSServiceDomain(p_rService, false, false, 0, p_rSendParameter)) &&  // _http._tcp.local
                       (_writeMDNSRRAttributes(attributes, p_rSendParameter)) &&                    // TYPE & CLASS
                       (_write32((p_rSendParameter.m_bUnannounce
                                  ? 0
                                  : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32ServiceTTL)), p_rSendParameter)) && // TTL
                       (_writeMDNSServiceDomain(p_rService, true, true, 0, p_rSendParameter)));     // RDLength & RData (service domain, eg. MyESP._http._tcp.local)

    DEBUG_EX_INFO(if (bResult)
                  DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_PTR_NAME _%s._%s.local Type:%s Class:0x%04X TTL:%u %s._%s._%s.local\n"),
                                        _DH(),
                                        p_rService.m_pcType,
                                        p_rService.m_pcProtocol,
                                        _RRType2Name(attributes.m_u16Type),
                                        attributes.m_u16Class,
                                        (p_rSendParameter.m_bUnannounce
                                         ? 0
                                         : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32ServiceTTL)),
                                        p_rService.m_pcInstanceName,
                                        p_rService.m_pcType,
                                        p_rService.m_pcProtocol);
                 );
    DEBUG_EX_ERR(if (!bResult)DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_PTR_NAME: FAILED!\n"), _DH()););
    return bResult;
}


/*
    MDNSResponder::_writeMDNSAnswer_TXT

    Write a MDNS TXT answer to the UDP output buffer.
    See: '_writeMDNSAnswer_A'

    The TXT items in the RDATA block are 'length byte encoded': [len]vardata

    eg. myESP._http._tcp.local TXT 0x8001 120 4 c#=1
    http://www.zytrax.com/books/dns/ch8/txt.html

*/
bool clsLEAMDNSHost::_writeMDNSAnswer_TXT(clsLEAMDNSHost::clsService& p_rService,
        clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_TXT%s\n"), (p_rSendParameter.m_bCacheFlush ? "" : " nF"), _DH()););

    bool                    bResult = false;

    clsRRAttributes    attributes(DNS_RRTYPE_TXT,
                                  ((p_rSendParameter.m_bCacheFlush ? 0x8000 : 0) | DNS_RRCLASS_IN));   // Cache flush? & INternet

    if ((_collectServiceTxts(p_rService)) &&
            (_writeMDNSServiceDomain(p_rService, true, false, 0, p_rSendParameter)) &&	// MyESP._http._tcp.local
            (_writeMDNSRRAttributes(attributes, p_rSendParameter)) &&                   // TYPE & CLASS
            (_write32((p_rSendParameter.m_bUnannounce
                       ? 0
                       : (p_rSendParameter.m_bLegacyDNSQuery                               // TTL
                          ? clsConsts::u32LegacyTTL
                          : clsConsts::u32ServiceTTL)), p_rSendParameter)) &&
            (_write16((p_rService.m_Txts.count()                                        // RDLength
                       ? p_rService.m_Txts.length()                                    // default case
                       : 1), p_rSendParameter)))                                       // If no TXT records exist, a single 0 byte is sent
    {
        bResult = true;
        // RData    Txts
        if (p_rService.m_Txts.count())
        {
            for (const clsServiceTxt* pTxt : p_rService.m_Txts.m_Txts)
            {
                unsigned char       ucLengthByte = pTxt->length();
                if (!((bResult = ((_udpAppendBuffer((unsigned char*)&ucLengthByte, sizeof(ucLengthByte))) &&                                        // Length
                                  (p_rSendParameter.shiftOffset(sizeof(ucLengthByte))) &&
                                  ((size_t)os_strlen(pTxt->m_pcKey) == m_pUDPContext->append(pTxt->m_pcKey, os_strlen(pTxt->m_pcKey))) &&           // Key
                                  (p_rSendParameter.shiftOffset((size_t)os_strlen(pTxt->m_pcKey))) &&
                                  (1 == m_pUDPContext->append("=", 1)) &&                                                                           // =
                                  (p_rSendParameter.shiftOffset(1)) &&
                                  ((!pTxt->m_pcValue) ||
                                   (((size_t)os_strlen(pTxt->m_pcValue) == m_pUDPContext->append(pTxt->m_pcValue, os_strlen(pTxt->m_pcValue))) &&   // Value
                                    (p_rSendParameter.shiftOffset((size_t)os_strlen(pTxt->m_pcValue)))))))))
                {
                    DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_TXT: FAILED to write %sTxt %s=%s!\n"), _DH(), (pTxt->m_bTemp ? "temp. " : ""), (pTxt->m_pcKey ? : "?"), (pTxt->m_pcValue ? : "?")););
                    break;
                }
            }
        }
        else
        {
            // RFC 6763 Ch.6: Every DNS-SD service MUST have a TXT record in addition to its SRV record, ...
            DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_TXT: Adding EMPTY TXT record!\n"), _DH()););
            unsigned char       ucLengthByte = 0;
            bResult = ((_udpAppendBuffer((unsigned char*)&ucLengthByte, sizeof(ucLengthByte))) &&                                           // Length
                       (p_rSendParameter.shiftOffset(sizeof(ucLengthByte))));
            DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_TXT: FAILED to write EMPTY TXT record!\n"), _DH()););
        }
    }

    DEBUG_EX_INFO(if (bResult)
                  DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_TXT %s._%s._%s.local Type:%s Class:0x%04X TTL:%u \n"),
                                        _DH(),
                                        p_rService.m_pcInstanceName,
                                        p_rService.m_pcType,
                                        p_rService.m_pcProtocol,
                                        _RRType2Name(attributes.m_u16Type),
                                        attributes.m_u16Class,
                                        (p_rSendParameter.m_bUnannounce
                                         ? 0
                                         : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32ServiceTTL)));
                 );

    _releaseTempServiceTxts(p_rService);

    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_TXT: FAILED!\n"), _DH()););
    return bResult;
}

#ifdef MDNS2_IPV6_SUPPORT
/*
    MDNSResponder::_writeMDNSAnswer_AAAA

    Write a MDNS AAAA answer to the UDP output buffer.
    See: '_writeMDNSAnswer_AAAA'

    eg. esp8266.local AAAA 0x8001 120 16 xxxx::xx
    http://www.zytrax.com/books/dns/ch8/aaaa.html

*/
bool clsLEAMDNSHost::_writeMDNSAnswer_AAAA(IPAddress p_IPAddress,
        clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_AAAA (%s)%s\n"), p_IPAddress.toString().c_str(), (p_rSendParameter.m_bCacheFlush ? "" : " nF")););

    clsRRAttributes    attributes(DNS_RRTYPE_AAAA,
                                  ((p_rSendParameter.m_bCacheFlush ? 0x8000 : 0) | DNS_RRCLASS_IN));   // Cache flush? & INternet
    bool    bResult = ((p_IPAddress.isV6()) &&
                       (_writeMDNSHostDomain(m_pcHostName, false, 0, p_rSendParameter)) &&	// esp8266.local
                       (_writeMDNSRRAttributes(attributes, p_rSendParameter)) &&            // TYPE & CLASS
                       (_write32((p_rSendParameter.m_bUnannounce
                                  ? 0
                                  : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32HostTTL)), p_rSendParameter)) &&    // TTL
                       (_write16(clsConsts::u16IPv6Size, p_rSendParameter)) &&                     	// RDLength
                       (_udpAppendBuffer((uint8_t*)p_IPAddress.raw6(), clsConsts::u16IPv6Size)) &&   // RData
                       (p_rSendParameter.shiftOffset(clsConsts::u16IPv6Size)));

    DEBUG_EX_INFO(if (bResult)
                  DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_AAAA %s.local Type:%s Class:0x%04X TTL:%u %s\n"),
                                        _DH(),
                                        m_pcHostName,
                                        _RRType2Name(attributes.m_u16Type),
                                        attributes.m_u16Class,
                                        (p_rSendParameter.m_bUnannounce
                                         ? 0
                                         : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32HostTTL)),
                                        p_IPAddress.toString().c_str());
                 );
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_AAAA: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_writeMDNSAnswer_PTR_IPv6

    Write a MDNS reverse IPv6 PTR answer to the UDP output buffer.
    See: '_writeMDNSAnswer_AAAA'

    eg. xxxx::xx.ip6.arpa PTR 0x8001 120 15 esp8266.local
    Used while answering reverse IPv6 questions

*/
bool clsLEAMDNSHost::_writeMDNSAnswer_PTR_IPv6(IPAddress p_IPAddress,
        clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_PTR_IPv6%s\n"), (p_rSendParameter.m_bCacheFlush ? "" : " nF")););

    clsRRDomain        reverseIPv6Domain;
    clsRRAttributes    attributes(DNS_RRTYPE_PTR,
                                  ((p_rSendParameter.m_bCacheFlush ? 0x8000 : 0) | DNS_RRCLASS_IN));   // Cache flush? & INternet
    bool    bResult = ((p_IPAddress.isV6()) &&
                       (_buildDomainForReverseIPv6(p_IPAddress, reverseIPv6Domain)) &&		// xxxx::xx.ip6.arpa
                       (_writeMDNSRRDomain(reverseIPv6Domain, p_rSendParameter)) &&
                       (_writeMDNSRRAttributes(attributes, p_rSendParameter)) &&            // TYPE & CLASS
                       (_write32((p_rSendParameter.m_bUnannounce
                                  ? 0
                                  : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32HostTTL)), p_rSendParameter)) &&    // TTL
                       (_writeMDNSHostDomain(m_pcHostName, true, 0, p_rSendParameter)));  	// RDLength & RData (host domain, eg. esp8266.local)

    DEBUG_EX_INFO(if (bResult)
                  DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_PTR_IPv6 "), _DH());
                  _printRRDomain(reverseIPv6Domain);
                  DEBUG_OUTPUT.printf_P(PSTR(" Type:%s Class:0x%04X TTL:%u %s.local\n"),
                                        _RRType2Name(attributes.m_u16Type),
                                        attributes.m_u16Class,
                                        (p_rSendParameter.m_bUnannounce
                                         ? 0
                                         : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32HostTTL)),
                                        m_pcHostName);
                 );
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_PTR_IPv6: FAILED!\n"), _DH()););
    return bResult;
}
#endif

/*
    MDNSResponder::_writeMDNSAnswer_SRV

    eg. MyESP._http.tcp.local SRV 0x8001 120 0 0 60068 esp8266.local
    http://www.zytrax.com/books/dns/ch8/srv.html ???? Include instance name ????

*/
bool clsLEAMDNSHost::_writeMDNSAnswer_SRV(clsLEAMDNSHost::clsService& p_rService,
        clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_SRV%s\n"), (p_rSendParameter.m_bCacheFlush ? "" : " nF")););

    uint16_t                u16CachedDomainOffset = (p_rSendParameter.m_bLegacyDNSQuery
            ? 0
            : p_rSendParameter.findCachedDomainOffset((const void*)m_pcHostName, false));

    clsRRAttributes    attributes(DNS_RRTYPE_SRV,
                                  ((p_rSendParameter.m_bCacheFlush ? 0x8000 : 0) | DNS_RRCLASS_IN));   // Cache flush? & INternet
    clsRRDomain        hostDomain;
    bool    bResult = ((_writeMDNSServiceDomain(p_rService, true, false, 0, p_rSendParameter)) &&  // MyESP._http._tcp.local
                       (_writeMDNSRRAttributes(attributes, p_rSendParameter)) &&                // TYPE & CLASS
                       (_write32((p_rSendParameter.m_bUnannounce
                                  ? 0
                                  : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32HostTTL/*Consts::u32ServiceTTL*/)), p_rSendParameter)) && // TTL
                       (!u16CachedDomainOffset
                        // No cache for domain name (or no compression allowed)
                        ? ((_buildDomainForHost(m_pcHostName, hostDomain)) &&
                           (_write16((sizeof(uint16_t /*Prio*/) +                           // RDLength
                                      sizeof(uint16_t /*Weight*/) +
                                      sizeof(uint16_t /*Port*/) +
                                      hostDomain.m_u16NameLength), p_rSendParameter)) &&    // Domain length
                           (_write16(clsConsts::u16SRVPriority, p_rSendParameter)) &&               // Priority
                           (_write16(clsConsts::u16SRVWeight, p_rSendParameter)) &&                 // Weight
                           (_write16(p_rService.m_u16Port, p_rSendParameter)) &&            // Port
                           (p_rSendParameter.addDomainCacheItem((const void*)m_pcHostName, false, p_rSendParameter.m_u16Offset)) &&
                           (_writeMDNSRRDomain(hostDomain, p_rSendParameter)))              // Host, eg. esp8266.local
                        // Cache available for domain
                        : ((clsConsts::u8DomainCompressMark > ((u16CachedDomainOffset >> 8) & ~clsConsts::u8DomainCompressMark)) && // Valid offset
                           (_write16((sizeof(uint16_t /*Prio*/) +                           // RDLength
                                      sizeof(uint16_t /*Weight*/) +
                                      sizeof(uint16_t /*Port*/) +
                                      2), p_rSendParameter)) &&                             // Length of 'C0xx'
                           (_write16(clsConsts::u16SRVPriority, p_rSendParameter)) &&               // Priority
                           (_write16(clsConsts::u16SRVWeight, p_rSendParameter)) &&                 // Weight
                           (_write16(p_rService.m_u16Port, p_rSendParameter)) &&            // Port
                           (_write8(((u16CachedDomainOffset >> 8) | clsConsts::u8DomainCompressMark), p_rSendParameter)) &&   // Compression mark (and offset)
                           (_write8((uint8_t)u16CachedDomainOffset, p_rSendParameter)))));  // Offset

    DEBUG_EX_INFO(if (bResult)
                  DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_SRV %s._%s._%s.local Type:%s Class:0x%04X TTL:%u %u %u %u %s.local\n"),
                                        _DH(),
                                        p_rService.m_pcInstanceName,
                                        p_rService.m_pcType,
                                        p_rService.m_pcProtocol,
                                        _RRType2Name(attributes.m_u16Type),
                                        attributes.m_u16Class,
                                        (p_rSendParameter.m_bUnannounce
                                         ? 0
                                         : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32HostTTL)),
                                        clsConsts::u16SRVPriority,
                                        clsConsts::u16SRVWeight,
                                        p_rService.m_u16Port,
                                        m_pcHostName);
                 );
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_SRV: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_createNSECBitmap

*/
clsLEAMDNSHost::clsNSECBitmap* clsLEAMDNSHost::_createNSECBitmap(uint32_t p_u32NSECContent)
{
    // Currently 6 bytes (6*8 -> 0..47) are long enough, and only this is implemented
    clsNSECBitmap* pNSECBitmap = new clsNSECBitmap;
    if (pNSECBitmap)
    {
#ifdef MDNS_IPV4_SUPPORT
        if (p_u32NSECContent & static_cast<uint32_t>(enuContentFlag::A))
        {
            pNSECBitmap->setBit(DNS_RRTYPE_A);              // 01/0x01
        }
#endif
        if ((p_u32NSECContent & static_cast<uint32_t>(enuContentFlag::PTR_IPv4)) ||
                (p_u32NSECContent & static_cast<uint32_t>(enuContentFlag::PTR_IPv6)))
        {
            pNSECBitmap->setBit(DNS_RRTYPE_PTR);            // 12/0x0C
        }
#ifdef MDNS2_IPV6_SUPPORT
        if (p_u32NSECContent & static_cast<uint32_t>(enuContentFlag::AAAA))
        {
            pNSECBitmap->setBit(DNS_RRTYPE_AAAA);           // 28/0x1C
        }
#endif
        if (p_u32NSECContent & static_cast<uint32_t>(enuContentFlag::TXT))
        {
            pNSECBitmap->setBit(DNS_RRTYPE_TXT);            // 16/0x10
        }
        if (p_u32NSECContent & static_cast<uint32_t>(enuContentFlag::SRV))
        {
            pNSECBitmap->setBit(DNS_RRTYPE_SRV);            // 33/0x21
        }
        if (p_u32NSECContent & static_cast<uint32_t>(enuContentFlag::NSEC))
        {
            pNSECBitmap->setBit(clsConsts::u8DNS_RRTYPE_NSEC); // 47/0x2F
        }
    }
    return pNSECBitmap;
}

/*
    MDNSResponder::_writeMDNSNSECBitmap

*/
bool clsLEAMDNSHost::_writeMDNSNSECBitmap(const clsLEAMDNSHost::clsNSECBitmap& p_NSECBitmap,
        clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    /*  DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("_writeMDNSNSECBitmap: "));
    			  for (uint16_t u=0; u<p_NSECBitmap.m_u16BitmapLength; ++u) {
    				  DEBUG_OUTPUT.printf_P(PSTR("0x%02X "), p_NSECBitmap.m_pu8BitmapData[u]);
    			  }
    			  DEBUG_OUTPUT.printf_P(PSTR("\n"));
    			 );*/

    bool    bResult = ((_write16(p_NSECBitmap.length(), p_rSendParameter)) &&
                       ((_udpAppendBuffer(p_NSECBitmap.m_au8BitmapData, p_NSECBitmap.length())) &&
                        (p_rSendParameter.shiftOffset(p_NSECBitmap.length()))));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSNSECBitmap: FAILED!\n"), _DH()););
    return bResult;
}

/*
    MDNSResponder::_writeMDNSAnswer_NSEC(host)

    eg. esp8266.local NSEC 0x8001 120 XX esp8266.local xyz
    http://www.zytrax.com/books/dns/ch8/nsec.html

*/
bool clsLEAMDNSHost::_writeMDNSAnswer_NSEC(uint32_t p_u32NSECContent,
        clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_NSEC (host: %s)%s\n"), _replyFlags2String(p_u32NSECContent), (p_rSendParameter.m_bCacheFlush ? "" : " nF")););

    clsRRAttributes    attributes(clsConsts::u8DNS_RRTYPE_NSEC,
                                  ((p_rSendParameter.m_bCacheFlush ? 0x8000 : 0) | DNS_RRCLASS_IN));       // Cache flush? & INternet
    clsNSECBitmap*     pNSECBitmap = _createNSECBitmap(p_u32NSECContent);
    bool    bResult = ((pNSECBitmap) &&                                                                         // NSEC bitmap created
                       (_writeMDNSHostDomain(m_pcHostName, false, 0, p_rSendParameter)) &&                      // esp8266.local
                       (_writeMDNSRRAttributes(attributes, p_rSendParameter)) &&                                // TYPE & CLASS
                       (_write32((p_rSendParameter.m_bUnannounce
                                  ? 0
                                  : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32HostTTL)), p_rSendParameter)) &&    // TTL
                       (_writeMDNSHostDomain(m_pcHostName, true, (2 + pNSECBitmap->length()), p_rSendParameter)) && // XX esp8266.local
                       (_writeMDNSNSECBitmap(*pNSECBitmap, p_rSendParameter)));                                 // NSEC bitmap

    DEBUG_EX_INFO(if (bResult)
                  DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_NSEC %s.local Type:%s Class:0x%04X TTL:%u %s %s\n"),
                                        _DH(),
                                        m_pcHostName,
                                        _RRType2Name(attributes.m_u16Type),
                                        attributes.m_u16Class,
                                        (p_rSendParameter.m_bUnannounce
                                         ? 0
                                         : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32HostTTL)),
                                        m_pcHostName,
                                        _NSECBitmap2String(pNSECBitmap));
                 );

    if (pNSECBitmap)
    {
        delete pNSECBitmap;
        pNSECBitmap = 0;
    }

    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_NSEC (host): FAILED!\n"), _DH()););
    return bResult;
}


#ifdef MDNS_IPV4_SUPPORT
/*
    MDNSResponder::_writeMDNSAnswer_NSEC_PTR_IPv4(host)

    eg. 012.789.456.123.in-addr.arpa NSEC 0x8001 120 XX 012.789.456.123.in-addr.arpa xyz
    http://www.zytrax.com/books/dns/ch8/nsec.html

*/
bool clsLEAMDNSHost::_writeMDNSAnswer_NSEC_PTR_IPv4(IPAddress p_IPAddress,
        clsSendParameter& p_rSendParameter)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_NSEC_PTR_IPv4\n")););

    clsRRAttributes    attributes(clsConsts::u8DNS_RRTYPE_NSEC,
                                  ((p_rSendParameter.m_bCacheFlush ? 0x8000 : 0) | DNS_RRCLASS_IN));       // Cache flush? & INternet
    clsNSECBitmap*     pNSECBitmap = _createNSECBitmap(static_cast<uint32_t>(enuContentFlag::PTR_IPv4));
    clsRRDomain        reverseIPv4Domain;
    bool    bResult = ((p_IPAddress.isV4()) &&
                       (pNSECBitmap) &&                                                                 // NSEC bitmap created
                       (_buildDomainForReverseIPv4(p_IPAddress, reverseIPv4Domain)) &&                  // 012.789.456.123.in-addr.arpa
                       (_writeMDNSRRDomain(reverseIPv4Domain, p_rSendParameter)) &&                     // 012.789.456.123.in-addr.arpa
                       (_writeMDNSRRAttributes(attributes, p_rSendParameter)) &&                        // TYPE & CLASS
                       (_write32((p_rSendParameter.m_bUnannounce
                                  ? 0
                                  : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32HostTTL)), p_rSendParameter)) &&    // TTL
                       (_write16((reverseIPv4Domain.m_u16NameLength + (2 + pNSECBitmap->length())), p_rSendParameter)) &&
                       (_writeMDNSRRDomain(reverseIPv4Domain, p_rSendParameter)) &&                 	// 012.789.456.123.in-addr.arpa
                       (_writeMDNSNSECBitmap(*pNSECBitmap, p_rSendParameter)));                         // NSEC bitmap

    DEBUG_EX_INFO(if (bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_NSEC_PTR_IPv4 "), _DH());
        _printRRDomain(reverseIPv4Domain);
        DEBUG_OUTPUT.printf_P(PSTR(" Type:%s Class:0x%04X TTL:%u "),
                              _RRType2Name(attributes.m_u16Type),
                              attributes.m_u16Class,
                              (p_rSendParameter.m_bUnannounce
                               ? 0
                               : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32ServiceTTL)));
        _printRRDomain(reverseIPv4Domain);
        DEBUG_OUTPUT.printf_P(PSTR(" %s\n"), _NSECBitmap2String(pNSECBitmap));
    });

    if (pNSECBitmap)
    {
        delete pNSECBitmap;
        pNSECBitmap = 0;
    }

    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_NSEC_PTR_IPv4 (host): FAILED!\n"), _DH()););
    return bResult;
}
#endif


#ifdef MDNS2_IPV6_SUPPORT
/*
    MDNSResponder::_writeMDNSAnswer_NSEC_PTR_IPv6(host)

    eg. 9.0.0.0.0.0.0.0.0.0.0.0.0.7.8.5.6.3.4.1.2.ip6.arpa NSEC 0x8001 120 XX 9.0.0.0.0.0.0.0.0.0.0.0.0.7.8.5.6.3.4.1.2.ip6.arpa xyz
    http://www.zytrax.com/books/dns/ch8/nsec.html

*/
bool clsLEAMDNSHost::_writeMDNSAnswer_NSEC_PTR_IPv6(IPAddress p_IPAddress,
        clsSendParameter& p_rSendParameter)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_NSEC_PTR_IPv6\n")););

    clsRRAttributes    attributes(clsConsts::u8DNS_RRTYPE_NSEC,
                                  ((p_rSendParameter.m_bCacheFlush ? 0x8000 : 0) | DNS_RRCLASS_IN));       // Cache flush? & INternet
    clsNSECBitmap*     pNSECBitmap = _createNSECBitmap(static_cast<uint32_t>(enuContentFlag::PTR_IPv6));
    clsRRDomain        reverseIPv6Domain;
    bool    bResult = ((p_IPAddress.isV6()) &&
                       (pNSECBitmap) &&                                                                 // NSEC bitmap created
                       (_buildDomainForReverseIPv6(p_IPAddress, reverseIPv6Domain)) &&                  // 9.0.0.0.0.0.0.0.0.0.0.0.0.7.8.5.6.3.4.1.2.ip6.arpa
                       (_writeMDNSRRDomain(reverseIPv6Domain, p_rSendParameter)) &&                     // 9.0.0.0.0.0.0.0.0.0.0.0.0.7.8.5.6.3.4.1.2.ip6.arpa
                       (_writeMDNSRRAttributes(attributes, p_rSendParameter)) &&                        // TYPE & CLASS
                       (_write32((p_rSendParameter.m_bUnannounce
                                  ? 0
                                  : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32HostTTL)), p_rSendParameter)) &&    // TTL
                       (_write16((reverseIPv6Domain.m_u16NameLength + (2 + pNSECBitmap->length())), p_rSendParameter)) &&
                       (_writeMDNSRRDomain(reverseIPv6Domain, p_rSendParameter)) &&                     // 9.0.0.0.0.0.0.0.0.0.0.0.0.7.8.5.6.3.4.1.2.ip6.arpa
                       (_writeMDNSNSECBitmap(*pNSECBitmap, p_rSendParameter)));                         // NSEC bitmap

    DEBUG_EX_INFO(if (bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_NSEC_PTR_IPv6 "), _DH());
        _printRRDomain(reverseIPv6Domain);
        DEBUG_OUTPUT.printf_P(PSTR(" Type:%s Class:0x%04X TTL:%u "),
                              _RRType2Name(attributes.m_u16Type),
                              attributes.m_u16Class,
                              (p_rSendParameter.m_bUnannounce
                               ? 0
                               : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32ServiceTTL)));
        _printRRDomain(reverseIPv6Domain);
        DEBUG_OUTPUT.printf_P(PSTR(" %s\n"), _NSECBitmap2String(pNSECBitmap));
    });

    if (pNSECBitmap)
    {
        delete pNSECBitmap;
        pNSECBitmap = 0;
    }

    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_NSEC_PTR_IPv6 (host): FAILED!\n"), _DH()););
    return bResult;
}
#endif

/*
    MDNSResponder::_writeMDNSAnswer_NSEC(service)

    eg. MyESP._http.tcp.local NSEC 0x8001 4500 XX MyESP._http.tcp.local xyz
    http://www.zytrax.com/books/dns/ch8/nsec.html

*/
bool clsLEAMDNSHost::_writeMDNSAnswer_NSEC(clsLEAMDNSHost::clsService& p_rService,
        uint32_t p_u32NSECContent,
        clsLEAMDNSHost::clsSendParameter& p_rSendParameter)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_NSEC (service: %s)\n"), _DH(), _replyFlags2String(p_u32NSECContent)););

    clsRRAttributes    attributes(clsConsts::u8DNS_RRTYPE_NSEC,
                                  ((p_rSendParameter.m_bCacheFlush ? 0x8000 : 0) | DNS_RRCLASS_IN));       // Cache flush? & INternet
    clsNSECBitmap*     pNSECBitmap = _createNSECBitmap(p_u32NSECContent);
    bool    bResult = ((pNSECBitmap) &&                                                                         // NSEC bitmap created
                       (_writeMDNSServiceDomain(p_rService, true, false, 0, p_rSendParameter)) &&               // MyESP._http._tcp.local
                       (_writeMDNSRRAttributes(attributes, p_rSendParameter)) &&                                // TYPE & CLASS
                       (_write32((p_rSendParameter.m_bUnannounce
                                  ? 0
                                  : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32ServiceTTL)), p_rSendParameter)) && // TTL
                       (_writeMDNSServiceDomain(p_rService, true, true, (2 + pNSECBitmap->length()), p_rSendParameter)) && // XX MyESP._http._tcp.local
                       (_writeMDNSNSECBitmap(*pNSECBitmap, p_rSendParameter)));                                 // NSEC bitmap

    DEBUG_EX_INFO(if (bResult)
                  DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_NSEC %s._%s._%s.local Type:%s Class:0x%04X TTL:%u %s\n"),
                                        _DH(),
                                        p_rService.m_pcInstanceName,
                                        p_rService.m_pcType,
                                        p_rService.m_pcProtocol,
                                        _RRType2Name(attributes.m_u16Type),
                                        attributes.m_u16Class,
                                        (p_rSendParameter.m_bUnannounce
                                         ? 0
                                         : (p_rSendParameter.m_bLegacyDNSQuery ? clsConsts::u32LegacyTTL : clsConsts::u32ServiceTTL)),
                                        _NSECBitmap2String(pNSECBitmap));
                 );

    if (pNSECBitmap)
    {
        delete pNSECBitmap;
        pNSECBitmap = 0;
    }

    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("%s _writeMDNSAnswer_NSEC (service): FAILED!\n"), _DH()););
    return bResult;
}


}   // namespace MDNSImplementation


} // namespace esp8266






