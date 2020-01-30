/*
    LEAmDNS_Transfer.cpp

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

extern "C" {
#include "user_interface.h"
}

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
    CONST STRINGS
*/
static const char*                      scpcLocal               = "local";
static const char*                      scpcServices            = "services";
static const char*                      scpcDNSSD               = "dns-sd";
static const char*                      scpcUDP                 = "udp";
//static const char*                    scpcTCP                 = "tcp";

#ifdef MDNS_IP4_SUPPORT
static const char*                  scpcReverseIP4Domain    = "in-addr";
#endif
#ifdef MDNS_IP6_SUPPORT
static const char*                  scpcReverseIP6Domain    = "ip6";
#endif
static const char*                      scpcReverseTopDomain    = "arpa";

/**
    TRANSFER
*/


/**
    SENDING
*/

/*
    MDNSResponder::_sendMDNSMessage

    Unicast responses are prepared and sent directly to the querier.
    Multicast responses or queries are transferred to _sendMDNSMessage_Multicast

    Any reply flags in installed services are removed at the end!

*/
bool MDNSResponder::_sendMDNSMessage(MDNSResponder::stcMDNSSendParameter& p_rSendParameter)
{

    bool    bResult = true;

    if (p_rSendParameter.m_bResponse &&
            p_rSendParameter.m_bUnicast)    // Unicast response  -> Send to querier
    {
        DEBUG_EX_ERR(if (!m_pUDPContext->getRemoteAddress())
    {
        DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _sendMDNSMessage: MISSING remote address for response!\n"));
        });
        IPAddress   ipRemote;
        ipRemote = m_pUDPContext->getRemoteAddress();
        bResult = ((_prepareMDNSMessage(p_rSendParameter, _getResponseMulticastInterface())) &&
                   (m_pUDPContext->send(ipRemote, m_pUDPContext->getRemotePort())));
    }
    else                                // Multicast response
    {
        bResult = _sendMDNSMessage_Multicast(p_rSendParameter);
    }

    // Finally clear service reply masks
    for (stcMDNSService* pService = m_pServices; pService; pService = pService->m_pNext)
    {
        pService->m_u8ReplyMask = 0;
    }

    DEBUG_EX_ERR(if (!bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _sendMDNSMessage: FAILED!\n"));
    });
    return bResult;
}

/*
    MDNSResponder::_sendMDNSMessage_Multicast

    Fills the UDP output buffer (via _prepareMDNSMessage) and sends the buffer
    via the selected WiFi interface (Station or AP)
*/
bool MDNSResponder::_sendMDNSMessage_Multicast(MDNSResponder::stcMDNSSendParameter& p_rSendParameter)
{

    bool    bResult = false;

    IPAddress   fromIPAddress;
    fromIPAddress = _getResponseMulticastInterface();
    m_pUDPContext->setMulticastInterface(fromIPAddress);

#ifdef MDNS_IP4_SUPPORT
    IPAddress   toMulticastAddress(DNS_MQUERY_IPV4_GROUP_INIT);
#endif
#ifdef MDNS_IP6_SUPPORT
    //TODO: set multicast address
    IPAddress   toMulticastAddress(DNS_MQUERY_IPV6_GROUP_INIT);
#endif
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _sendMDNSMessage_Multicast: Will send to '%s'.\n"), toMulticastAddress.toString().c_str()););
    bResult = ((_prepareMDNSMessage(p_rSendParameter, fromIPAddress)) &&
               (m_pUDPContext->send(toMulticastAddress, DNS_MQUERY_PORT)));

    DEBUG_EX_ERR(if (!bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _sendMDNSMessage_Multicast: FAILED!\n"));
    });
    return bResult;
}

/*
    MDNSResponder::_prepareMDNSMessage

    The MDNS message is composed in a two-step process.
    In the first loop 'only' the header informations (mainly number of answers) are collected,
    while in the seconds loop, the header and all queries and answers are written to the UDP
    output buffer.

*/
bool MDNSResponder::_prepareMDNSMessage(MDNSResponder::stcMDNSSendParameter& p_rSendParameter,
                                        IPAddress p_IPAddress)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _prepareMDNSMessage\n")););
    bool    bResult = true;

    // Prepare header; count answers
    stcMDNS_MsgHeader  msgHeader(p_rSendParameter.m_u16ID, p_rSendParameter.m_bResponse, 0, p_rSendParameter.m_bAuthorative);
    // If this is a response, the answers are anwers,
    // else this is a query or probe and the answers go into auth section
    uint16_t&           ru16Answers = (p_rSendParameter.m_bResponse
                                       ? msgHeader.m_u16ANCount
                                       : msgHeader.m_u16NSCount);

    /**
        enuSequence
    */
    enum enuSequence
    {
        Sequence_Count  = 0,
        Sequence_Send   = 1
    };

    // Two step sequence: 'Count' and 'Send'
    for (uint32_t sequence = Sequence_Count; ((bResult) && (sequence <= Sequence_Send)); ++sequence)
    {
        DEBUG_EX_INFO(
            if (Sequence_Send == sequence)
    {
        DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _prepareMDNSMessage: ID:%u QR:%u OP:%u AA:%u TC:%u RD:%u RA:%u R:%u QD:%u AN:%u NS:%u AR:%u\n"),
                              (unsigned)msgHeader.m_u16ID,
                              (unsigned)msgHeader.m_1bQR, (unsigned)msgHeader.m_4bOpcode, (unsigned)msgHeader.m_1bAA, (unsigned)msgHeader.m_1bTC, (unsigned)msgHeader.m_1bRD,
                              (unsigned)msgHeader.m_1bRA, (unsigned)msgHeader.m_4bRCode,
                              (unsigned)msgHeader.m_u16QDCount,
                              (unsigned)msgHeader.m_u16ANCount,
                              (unsigned)msgHeader.m_u16NSCount,
                              (unsigned)msgHeader.m_u16ARCount);
        }
        );
        // Count/send
        // Header
        bResult = ((Sequence_Count == sequence)
                   ? true
                   : _writeMDNSMsgHeader(msgHeader, p_rSendParameter));
        DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _prepareMDNSMessage: _writeMDNSMsgHeader FAILED!\n")););
        // Questions
        for (stcMDNS_RRQuestion* pQuestion = p_rSendParameter.m_pQuestions; ((bResult) && (pQuestion)); pQuestion = pQuestion->m_pNext)
        {
            ((Sequence_Count == sequence)
             ? ++msgHeader.m_u16QDCount
             : (bResult = _writeMDNSQuestion(*pQuestion, p_rSendParameter)));
            DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _prepareMDNSMessage: _writeMDNSQuestion FAILED!\n")););
        }

        // Answers and authorative answers
#ifdef MDNS_IP4_SUPPORT
        if ((bResult) &&
                (p_rSendParameter.m_u8HostReplyMask & ContentFlag_A))
        {
            ((Sequence_Count == sequence)
             ? ++ru16Answers
             : (bResult = _writeMDNSAnswer_A(p_IPAddress, p_rSendParameter)));
            DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _prepareMDNSMessage: _writeMDNSAnswer_A(A) FAILED!\n")););
        }
        if ((bResult) &&
                (p_rSendParameter.m_u8HostReplyMask & ContentFlag_PTR_IP4))
        {
            ((Sequence_Count == sequence)
             ? ++ru16Answers
             : (bResult = _writeMDNSAnswer_PTR_IP4(p_IPAddress, p_rSendParameter)));
            DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _prepareMDNSMessage: _writeMDNSAnswer_PTR_IP4 FAILED!\n")););
        }
#endif
#ifdef MDNS_IP6_SUPPORT
        if ((bResult) &&
                (p_rSendParameter.m_u8HostReplyMask & ContentFlag_AAAA))
        {
            ((Sequence_Count == sequence)
             ? ++ru16Answers
             : (bResult = _writeMDNSAnswer_AAAA(p_IPAddress, p_rSendParameter)));
            DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _prepareMDNSMessage: _writeMDNSAnswer_AAAA(A) FAILED!\n")););
        }
        if ((bResult) &&
                (p_rSendParameter.m_u8HostReplyMask & ContentFlag_PTR_IP6))
        {
            ((Sequence_Count == sequence)
             ? ++ru16Answers
             : (bResult = _writeMDNSAnswer_PTR_IP6(p_IPAddress, p_rSendParameter)));
            DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _prepareMDNSMessage: _writeMDNSAnswer_PTR_IP6 FAILED!\n")););
        }
#endif

        for (stcMDNSService* pService = m_pServices; ((bResult) && (pService)); pService = pService->m_pNext)
        {
            if ((bResult) &&
                    (pService->m_u8ReplyMask & ContentFlag_PTR_TYPE))
            {
                ((Sequence_Count == sequence)
                 ? ++ru16Answers
                 : (bResult = _writeMDNSAnswer_PTR_TYPE(*pService, p_rSendParameter)));
                DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _prepareMDNSMessage: _writeMDNSAnswer_PTR_TYPE FAILED!\n")););
            }
            if ((bResult) &&
                    (pService->m_u8ReplyMask & ContentFlag_PTR_NAME))
            {
                ((Sequence_Count == sequence)
                 ? ++ru16Answers
                 : (bResult = _writeMDNSAnswer_PTR_NAME(*pService, p_rSendParameter)));
                DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _prepareMDNSMessage: _writeMDNSAnswer_PTR_NAME FAILED!\n")););
            }
            if ((bResult) &&
                    (pService->m_u8ReplyMask & ContentFlag_SRV))
            {
                ((Sequence_Count == sequence)
                 ? ++ru16Answers
                 : (bResult = _writeMDNSAnswer_SRV(*pService, p_rSendParameter)));
                DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _prepareMDNSMessage: _writeMDNSAnswer_SRV(A) FAILED!\n")););
            }
            if ((bResult) &&
                    (pService->m_u8ReplyMask & ContentFlag_TXT))
            {
                ((Sequence_Count == sequence)
                 ? ++ru16Answers
                 : (bResult = _writeMDNSAnswer_TXT(*pService, p_rSendParameter)));
                DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _prepareMDNSMessage: _writeMDNSAnswer_TXT(A) FAILED!\n")););
            }
        }   // for services

        // Additional answers
#ifdef MDNS_IP4_SUPPORT
        bool    bNeedsAdditionalAnswerA = false;
#endif
#ifdef MDNS_IP6_SUPPORT
        bool    bNeedsAdditionalAnswerAAAA = false;
#endif
        for (stcMDNSService* pService = m_pServices; ((bResult) && (pService)); pService = pService->m_pNext)
        {
            if ((bResult) &&
                    (pService->m_u8ReplyMask & ContentFlag_PTR_NAME) && // If PTR_NAME is requested, AND
                    (!(pService->m_u8ReplyMask & ContentFlag_SRV)))     // NOT SRV -> add SRV as additional answer
            {
                ((Sequence_Count == sequence)
                 ? ++msgHeader.m_u16ARCount
                 : (bResult = _writeMDNSAnswer_SRV(*pService, p_rSendParameter)));
                DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _prepareMDNSMessage: _writeMDNSAnswer_SRV(B) FAILED!\n")););
            }
            if ((bResult) &&
                    (pService->m_u8ReplyMask & ContentFlag_PTR_NAME) && // If PTR_NAME is requested, AND
                    (!(pService->m_u8ReplyMask & ContentFlag_TXT)))     // NOT TXT -> add TXT as additional answer
            {
                ((Sequence_Count == sequence)
                 ? ++msgHeader.m_u16ARCount
                 : (bResult = _writeMDNSAnswer_TXT(*pService, p_rSendParameter)));
                DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _prepareMDNSMessage: _writeMDNSAnswer_TXT(B) FAILED!\n")););
            }
            if ((pService->m_u8ReplyMask & (ContentFlag_PTR_NAME | ContentFlag_SRV)) ||         // If service instance name or SRV OR
                    (p_rSendParameter.m_u8HostReplyMask & (ContentFlag_A | ContentFlag_AAAA)))      // any host IP address is requested
            {
#ifdef MDNS_IP4_SUPPORT
                if ((bResult) &&
                        (!(p_rSendParameter.m_u8HostReplyMask & ContentFlag_A)))            // Add IP4 address
                {
                    bNeedsAdditionalAnswerA = true;
                }
#endif
#ifdef MDNS_IP6_SUPPORT
                if ((bResult) &&
                        (!(p_rSendParameter.m_u8HostReplyMask & ContentFlag_AAAA)))         // Add IP6 address
                {
                    bNeedsAdditionalAnswerAAAA = true;
                }
#endif
            }
        }   // for services

        // Answer A needed?
#ifdef MDNS_IP4_SUPPORT
        if ((bResult) &&
                (bNeedsAdditionalAnswerA))
        {
            ((Sequence_Count == sequence)
             ? ++msgHeader.m_u16ARCount
             : (bResult = _writeMDNSAnswer_A(p_IPAddress, p_rSendParameter)));
            DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _prepareMDNSMessage: _writeMDNSAnswer_A(B) FAILED!\n")););
        }
#endif
#ifdef MDNS_IP6_SUPPORT
        // Answer AAAA needed?
        if ((bResult) &&
                (bNeedsAdditionalAnswerAAAA))
        {
            ((Sequence_Count == sequence)
             ? ++msgHeader.m_u16ARCount
             : (bResult = _writeMDNSAnswer_AAAA(p_IPAddress, p_rSendParameter)));
            DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _prepareMDNSMessage: _writeMDNSAnswer_AAAA(B) FAILED!\n")););
        }
#endif
        DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _prepareMDNSMessage: Loop %i FAILED!\n"), sequence););
    }   // for sequence
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _prepareMDNSMessage: FAILED!\n")););
    return bResult;
}

/*
    MDNSResponder::_sendMDNSServiceQuery

    Creates and sends a PTR query for the given service domain.

*/
bool MDNSResponder::_sendMDNSServiceQuery(const MDNSResponder::stcMDNSServiceQuery& p_ServiceQuery)
{

    return _sendMDNSQuery(p_ServiceQuery.m_ServiceTypeDomain, DNS_RRTYPE_PTR);
}

/*
    MDNSResponder::_sendMDNSQuery

    Creates and sends a query for the given domain and query type.

*/
bool MDNSResponder::_sendMDNSQuery(const MDNSResponder::stcMDNS_RRDomain& p_QueryDomain,
                                   uint16_t p_u16QueryType,
                                   stcMDNSServiceQuery::stcAnswer* p_pKnownAnswers /*= 0*/)
{

    bool                    bResult = false;

    stcMDNSSendParameter    sendParameter;
    if (0 != ((sendParameter.m_pQuestions = new stcMDNS_RRQuestion)))
    {
        sendParameter.m_pQuestions->m_Header.m_Domain = p_QueryDomain;

        sendParameter.m_pQuestions->m_Header.m_Attributes.m_u16Type = p_u16QueryType;
        // It seems, that some mDNS implementations don't support 'unicast response' questions...
        sendParameter.m_pQuestions->m_Header.m_Attributes.m_u16Class = (/*0x8000 |*/ DNS_RRCLASS_IN);   // /*Unicast &*/ INternet

        // TODO: Add knwon answer to the query
        (void)p_pKnownAnswers;

        bResult = _sendMDNSMessage(sendParameter);
    }   // else: FAILED to alloc question
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _sendMDNSQuery: FAILED to alloc question!\n")););
    return bResult;
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
bool MDNSResponder::_readRRQuestion(MDNSResponder::stcMDNS_RRQuestion& p_rRRQuestion)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRQuestion\n")););

    bool    bResult = false;

    if ((bResult = _readRRHeader(p_rRRQuestion.m_Header)))
    {
        // Extract unicast flag from class field
        p_rRRQuestion.m_bUnicast = (p_rRRQuestion.m_Header.m_Attributes.m_u16Class & 0x8000);
        p_rRRQuestion.m_Header.m_Attributes.m_u16Class &= (~0x8000);

        DEBUG_EX_INFO(
            DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRQuestion "));
            _printRRDomain(p_rRRQuestion.m_Header.m_Domain);
            DEBUG_OUTPUT.printf_P(PSTR(" Type:0x%04X Class:0x%04X %s\n"), (unsigned)p_rRRQuestion.m_Header.m_Attributes.m_u16Type, (unsigned)p_rRRQuestion.m_Header.m_Attributes.m_u16Class, (p_rRRQuestion.m_bUnicast ? "Unicast" : "Multicast"));
        );
    }
    DEBUG_EX_ERR(if (!bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRQuestion: FAILED!\n"));
    });
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
bool MDNSResponder::_readRRAnswer(MDNSResponder::stcMDNS_RRAnswer*& p_rpRRAnswer)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRAnswer\n")););

    bool    bResult = false;

    stcMDNS_RRHeader    header;
    uint32_t            u32TTL;
    uint16_t            u16RDLength;
    if ((_readRRHeader(header)) &&
            (_udpRead32(u32TTL)) &&
            (_udpRead16(u16RDLength)))
    {

        /*  DEBUG_EX_INFO(
                DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRAnswer: Reading 0x%04X answer (class:0x%04X, TTL:%u, RDLength:%u) for "), header.m_Attributes.m_u16Type, header.m_Attributes.m_u16Class, u32TTL, u16RDLength);
                _printRRDomain(header.m_Domain);
                DEBUG_OUTPUT.printf_P(PSTR("\n"));
                );*/

        switch (header.m_Attributes.m_u16Type & (~0x8000))      // Topmost bit might carry 'cache flush' flag
        {
#ifdef MDNS_IP4_SUPPORT
        case DNS_RRTYPE_A:
            p_rpRRAnswer = new stcMDNS_RRAnswerA(header, u32TTL);
            bResult = _readRRAnswerA(*(stcMDNS_RRAnswerA*&)p_rpRRAnswer, u16RDLength);
            break;
#endif
        case DNS_RRTYPE_PTR:
            p_rpRRAnswer = new stcMDNS_RRAnswerPTR(header, u32TTL);
            bResult = _readRRAnswerPTR(*(stcMDNS_RRAnswerPTR*&)p_rpRRAnswer, u16RDLength);
            break;
        case DNS_RRTYPE_TXT:
            p_rpRRAnswer = new stcMDNS_RRAnswerTXT(header, u32TTL);
            bResult = _readRRAnswerTXT(*(stcMDNS_RRAnswerTXT*&)p_rpRRAnswer, u16RDLength);
            break;
#ifdef MDNS_IP6_SUPPORT
        case DNS_RRTYPE_AAAA:
            p_rpRRAnswer = new stcMDNS_RRAnswerAAAA(header, u32TTL);
            bResult = _readRRAnswerAAAA(*(stcMDNS_RRAnswerAAAA*&)p_rpRRAnswer, u16RDLength);
            break;
#endif
        case DNS_RRTYPE_SRV:
            p_rpRRAnswer = new stcMDNS_RRAnswerSRV(header, u32TTL);
            bResult = _readRRAnswerSRV(*(stcMDNS_RRAnswerSRV*&)p_rpRRAnswer, u16RDLength);
            break;
        default:
            p_rpRRAnswer = new stcMDNS_RRAnswerGeneric(header, u32TTL);
            bResult = _readRRAnswerGeneric(*(stcMDNS_RRAnswerGeneric*&)p_rpRRAnswer, u16RDLength);
            break;
        }
        DEBUG_EX_INFO(
            if ((bResult) &&
                (p_rpRRAnswer))
    {
        DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRAnswer: "));
            _printRRDomain(p_rpRRAnswer->m_Header.m_Domain);
            DEBUG_OUTPUT.printf_P(PSTR(" Type:0x%04X Class:0x%04X TTL:%u, RDLength:%u "), p_rpRRAnswer->m_Header.m_Attributes.m_u16Type, p_rpRRAnswer->m_Header.m_Attributes.m_u16Class, p_rpRRAnswer->m_u32TTL, u16RDLength);
            switch (header.m_Attributes.m_u16Type & (~0x8000))      // Topmost bit might carry 'cache flush' flag
            {
#ifdef MDNS_IP4_SUPPORT
            case DNS_RRTYPE_A:
                DEBUG_OUTPUT.printf_P(PSTR("A IP:%s"), ((stcMDNS_RRAnswerA*&)p_rpRRAnswer)->m_IPAddress.toString().c_str());
                break;
#endif
            case DNS_RRTYPE_PTR:
                DEBUG_OUTPUT.printf_P(PSTR("PTR "));
                _printRRDomain(((stcMDNS_RRAnswerPTR*&)p_rpRRAnswer)->m_PTRDomain);
                break;
            case DNS_RRTYPE_TXT:
            {
                size_t  stTxtLength = ((stcMDNS_RRAnswerTXT*&)p_rpRRAnswer)->m_Txts.c_strLength();
                char*   pTxts = new char[stTxtLength];
                if (pTxts)
                {
                    ((stcMDNS_RRAnswerTXT*&)p_rpRRAnswer)->m_Txts.c_str(pTxts);
                    DEBUG_OUTPUT.printf_P(PSTR("TXT(%u) %s"), stTxtLength, pTxts);
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
                DEBUG_OUTPUT.printf_P(PSTR("SRV Port:%u "), ((stcMDNS_RRAnswerSRV*&)p_rpRRAnswer)->m_u16Port);
                _printRRDomain(((stcMDNS_RRAnswerSRV*&)p_rpRRAnswer)->m_SRVDomain);
                break;
            default:
                DEBUG_OUTPUT.printf_P(PSTR("generic "));
                break;
            }
            DEBUG_OUTPUT.printf_P(PSTR("\n"));
        }
        else
        {
            DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRAnswer: FAILED to read specific answer of type 0x%04X!\n"), p_rpRRAnswer->m_Header.m_Attributes.m_u16Type);
        }
        );  // DEBUG_EX_INFO
    }
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRAnswer: FAILED!\n")););
    return bResult;
}

#ifdef MDNS_IP4_SUPPORT
/*
    MDNSResponder::_readRRAnswerA
*/
bool MDNSResponder::_readRRAnswerA(MDNSResponder::stcMDNS_RRAnswerA& p_rRRAnswerA,
                                   uint16_t p_u16RDLength)
{

    uint32_t    u32IP4Address;
    bool        bResult = ((MDNS_IP4_SIZE == p_u16RDLength) &&
                           (_udpReadBuffer((unsigned char*)&u32IP4Address, MDNS_IP4_SIZE)) &&
                           ((p_rRRAnswerA.m_IPAddress = IPAddress(u32IP4Address))));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRAnswerA: FAILED!\n")););
    return bResult;
}
#endif

/*
    MDNSResponder::_readRRAnswerPTR
*/
bool MDNSResponder::_readRRAnswerPTR(MDNSResponder::stcMDNS_RRAnswerPTR& p_rRRAnswerPTR,
                                     uint16_t p_u16RDLength)
{

    bool    bResult = ((p_u16RDLength) &&
                       (_readRRDomain(p_rRRAnswerPTR.m_PTRDomain)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRAnswerPTR: FAILED!\n")););
    return bResult;
}

/*
    MDNSResponder::_readRRAnswerTXT

    Read TXT items from a buffer like 4c#=15ff=20
*/
bool MDNSResponder::_readRRAnswerTXT(MDNSResponder::stcMDNS_RRAnswerTXT& p_rRRAnswerTXT,
                                     uint16_t p_u16RDLength)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRAnswerTXT: RDLength:%u\n"), p_u16RDLength););
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

                    stcMDNSServiceTxt*      pTxt = 0;
                    unsigned char   ucLength = *pucCursor++;    // Length of the next txt item
                    if (ucLength)
                    {
                        DEBUG_EX_INFO(
                            static char sacBuffer[64]; *sacBuffer = 0;
                            uint8_t u8MaxLength = ((ucLength > (sizeof(sacBuffer) - 1)) ? (sizeof(sacBuffer) - 1) : ucLength);
                            os_strncpy(sacBuffer, (const char*)pucCursor, u8MaxLength); sacBuffer[u8MaxLength] = 0;
                            DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRAnswerTXT: Item(%u): %s\n"), ucLength, sacBuffer);
                        );

                        unsigned char*  pucEqualSign = (unsigned char*)os_strchr((const char*)pucCursor, '=');  // Position of the '=' sign
                        unsigned char   ucKeyLength;
                        if ((pucEqualSign) &&
                                ((ucKeyLength = (pucEqualSign - pucCursor))))
                        {
                            unsigned char   ucValueLength = (ucLength - (pucEqualSign - pucCursor + 1));
                            bResult = (((pTxt = new stcMDNSServiceTxt)) &&
                                       (pTxt->setKey((const char*)pucCursor, ucKeyLength)) &&
                                       (pTxt->setValue((const char*)(pucEqualSign + 1), ucValueLength)));
                        }
                        else
                        {
                            DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRAnswerTXT: INVALID TXT format (No '=')!\n")););
                        }
                        pucCursor += ucLength;
                    }
                    else    // no/zero length TXT
                    {
                        DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRAnswerTXT: TXT answer contains no items.\n")););
                        bResult = true;
                    }

                    if ((bResult) &&
                            (pTxt))     // Everythings fine so far
                    {
                        // Link TXT item to answer TXTs
                        pTxt->m_pNext = p_rRRAnswerTXT.m_Txts.m_pTxts;
                        p_rRRAnswerTXT.m_Txts.m_pTxts = pTxt;
                    }
                    else            // At least no TXT (migth be OK, if length was 0) OR an error
                    {
                        if (!bResult)
                        {
                            DEBUG_EX_ERR(
                                DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRAnswerTXT: FAILED to read TXT item!\n"));
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
                DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRAnswerTXT: FAILED to read TXT content!\n")););
            }
            // Clean up
            delete[] pucBuffer;
        }
        else
        {
            DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRAnswerTXT: FAILED to alloc buffer for TXT content!\n")););
        }
    }
    else
    {
        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRAnswerTXT: WARNING! No content!\n")););
    }
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRAnswerTXT: FAILED!\n")););
    return bResult;
}

#ifdef MDNS_IP6_SUPPORT
bool MDNSResponder::_readRRAnswerAAAA(MDNSResponder::stcMDNS_RRAnswerAAAA& p_rRRAnswerAAAA,
                                      uint16_t p_u16RDLength)
{
    bool    bResult = false;
    // TODO: Implement
    return bResult;
}
#endif

/*
    MDNSResponder::_readRRAnswerSRV
*/
bool MDNSResponder::_readRRAnswerSRV(MDNSResponder::stcMDNS_RRAnswerSRV& p_rRRAnswerSRV,
                                     uint16_t p_u16RDLength)
{

    bool    bResult = (((3 * sizeof(uint16_t)) < p_u16RDLength) &&
                       (_udpRead16(p_rRRAnswerSRV.m_u16Priority)) &&
                       (_udpRead16(p_rRRAnswerSRV.m_u16Weight)) &&
                       (_udpRead16(p_rRRAnswerSRV.m_u16Port)) &&
                       (_readRRDomain(p_rRRAnswerSRV.m_SRVDomain)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRAnswerSRV: FAILED!\n")););
    return bResult;
}

/*
    MDNSResponder::_readRRAnswerGeneric
*/
bool MDNSResponder::_readRRAnswerGeneric(MDNSResponder::stcMDNS_RRAnswerGeneric& p_rRRAnswerGeneric,
        uint16_t p_u16RDLength)
{
    bool    bResult = (0 == p_u16RDLength);

    p_rRRAnswerGeneric.clear();
    if (((p_rRRAnswerGeneric.m_u16RDLength = p_u16RDLength)) &&
            ((p_rRRAnswerGeneric.m_pu8RDData = new unsigned char[p_rRRAnswerGeneric.m_u16RDLength])))
    {

        bResult = _udpReadBuffer(p_rRRAnswerGeneric.m_pu8RDData, p_rRRAnswerGeneric.m_u16RDLength);
    }
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRAnswerGeneric: FAILED!\n")););
    return bResult;
}

/*
    MDNSResponder::_readRRHeader
*/
bool MDNSResponder::_readRRHeader(MDNSResponder::stcMDNS_RRHeader& p_rRRHeader)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRHeader\n")););

    bool    bResult = ((_readRRDomain(p_rRRHeader.m_Domain)) &&
                       (_readRRAttributes(p_rRRHeader.m_Attributes)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRHeader: FAILED!\n")););
    return bResult;
}

/*
    MDNSResponder::_readRRDomain

    Reads a (maybe multilevel compressed) domain from the UDP input buffer.

*/
bool MDNSResponder::_readRRDomain(MDNSResponder::stcMDNS_RRDomain& p_rRRDomain)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRDomain\n")););

    bool    bResult = ((p_rRRDomain.clear()) &&
                       (_readRRDomain_Loop(p_rRRDomain, 0)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRDomain: FAILED!\n")););
    return bResult;
}

/*
    MDNSResponder::_readRRDomain_Loop

    Reads a domain from the UDP input buffer. For every compression level, the functions
    calls itself recursively. To avoid endless recursion because of malformed MDNS records,
    the maximum recursion depth is set by MDNS_DOMAIN_MAX_REDIRCTION.

*/
bool MDNSResponder::_readRRDomain_Loop(MDNSResponder::stcMDNS_RRDomain& p_rRRDomain,
                                       uint8_t p_u8Depth)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRDomain_Loop(%u)\n"), p_u8Depth););

    bool    bResult = false;

    if (MDNS_DOMAIN_MAX_REDIRCTION >= p_u8Depth)
    {
        bResult = true;

        uint8_t u8Len = 0;
        do
        {
            //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRDomain_Loop(%u): Offset:%u p0:%02x\n"), p_u8Depth, m_pUDPContext->tell(), m_pUDPContext->peek()););
            _udpRead8(u8Len);

            if (u8Len & MDNS_DOMAIN_COMPRESS_MARK)
            {
                // Compressed label(s)
                uint16_t    u16Offset = ((u8Len & ~MDNS_DOMAIN_COMPRESS_MARK) << 8);    // Implicit BE to LE conversion!
                _udpRead8(u8Len);
                u16Offset |= u8Len;

                if (m_pUDPContext->isValidOffset(u16Offset))
                {
                    size_t  stCurrentPosition = m_pUDPContext->tell();      // Prepare return from recursion

                    //DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRDomain_Loop(%u): Redirecting from %u to %u!\n"), p_u8Depth, stCurrentPosition, u16Offset););
                    m_pUDPContext->seek(u16Offset);
                    if (_readRRDomain_Loop(p_rRRDomain, p_u8Depth + 1))     // Do recursion
                    {
                        //DEBUG_EX_RX(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRDomain_Loop(%u): Succeeded to read redirected label! Returning to %u\n"), p_u8Depth, stCurrentPosition););
                        m_pUDPContext->seek(stCurrentPosition);             // Restore after recursion
                    }
                    else
                    {
                        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRDomain_Loop(%u): FAILED to read redirected label!\n"), p_u8Depth););
                        bResult = false;
                    }
                }
                else
                {
                    DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRDomain_Loop(%u): INVALID offset in redirection!\n"), p_u8Depth););
                    bResult = false;
                }
                break;
            }
            else
            {
                // Normal (uncompressed) label (maybe '\0' only)
                if (MDNS_DOMAIN_MAXLENGTH > (p_rRRDomain.m_u16NameLength + u8Len))
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
                                    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRDomain_Loop(%u): Domain label (%u): %s\n"), p_u8Depth, (unsigned)(p_rRRDomain.m_acName[p_rRRDomain.m_u16NameLength - 1]), &(p_rRRDomain.m_acName[p_rRRDomain.m_u16NameLength]));
                                    );*/

                            p_rRRDomain.m_u16NameLength += u8Len;
                        }
                    }
                    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRDomain_Loop(2) offset:%u p0:%x\n"), m_pUDPContext->tell(), m_pUDPContext->peek()););
                }
                else
                {
                    DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRDomain_Loop(%u): ERROR! Domain name too long (%u + %u)!\n"), p_u8Depth, p_rRRDomain.m_u16NameLength, u8Len););
                    bResult = false;
                    break;
                }
            }
        } while ((bResult) &&
                 (0 != u8Len));
    }
    else
    {
        DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRDomain_Loop(%u): ERROR! Too many redirections!\n"), p_u8Depth););
    }
    return bResult;
}

/*
    MDNSResponder::_readRRAttributes
*/
bool MDNSResponder::_readRRAttributes(MDNSResponder::stcMDNS_RRAttributes& p_rRRAttributes)
{
    //DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRAttributes\n")););

    bool    bResult = ((_udpRead16(p_rRRAttributes.m_u16Type)) &&
                       (_udpRead16(p_rRRAttributes.m_u16Class)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readRRAttributes: FAILED!\n")););
    return bResult;
}


/*
    DOMAIN NAMES
*/

/*
    MDNSResponder::_buildDomainForHost

    Builds a MDNS host domain (eg. esp8266.local) for the given hostname.

*/
bool MDNSResponder::_buildDomainForHost(const char* p_pcHostname,
                                        MDNSResponder::stcMDNS_RRDomain& p_rHostDomain) const
{

    p_rHostDomain.clear();
    bool    bResult = ((p_pcHostname) &&
                       (*p_pcHostname) &&
                       (p_rHostDomain.addLabel(p_pcHostname)) &&
                       (p_rHostDomain.addLabel(scpcLocal)) &&
                       (p_rHostDomain.addLabel(0)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _buildDomainForHost: FAILED!\n")););
    return bResult;
}

/*
    MDNSResponder::_buildDomainForDNSSD

    Builds the '_services._dns-sd._udp.local' domain.
    Used while detecting generic service enum question (DNS-SD) and answering these questions.

*/
bool MDNSResponder::_buildDomainForDNSSD(MDNSResponder::stcMDNS_RRDomain& p_rDNSSDDomain) const
{

    p_rDNSSDDomain.clear();
    bool    bResult = ((p_rDNSSDDomain.addLabel(scpcServices, true)) &&
                       (p_rDNSSDDomain.addLabel(scpcDNSSD, true)) &&
                       (p_rDNSSDDomain.addLabel(scpcUDP, true)) &&
                       (p_rDNSSDDomain.addLabel(scpcLocal)) &&
                       (p_rDNSSDDomain.addLabel(0)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _buildDomainForDNSSD: FAILED!\n")););
    return bResult;
}

/*
    MDNSResponder::_buildDomainForService

    Builds the domain for the given service (eg. _http._tcp.local or
    MyESP._http._tcp.local (if p_bIncludeName is set)).

*/
bool MDNSResponder::_buildDomainForService(const MDNSResponder::stcMDNSService& p_Service,
        bool p_bIncludeName,
        MDNSResponder::stcMDNS_RRDomain& p_rServiceDomain) const
{

    p_rServiceDomain.clear();
    bool    bResult = (((!p_bIncludeName) ||
                        (p_rServiceDomain.addLabel(p_Service.m_pcName))) &&
                       (p_rServiceDomain.addLabel(p_Service.m_pcService, true)) &&
                       (p_rServiceDomain.addLabel(p_Service.m_pcProtocol, true)) &&
                       (p_rServiceDomain.addLabel(scpcLocal)) &&
                       (p_rServiceDomain.addLabel(0)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _buildDomainForService: FAILED!\n")););
    return bResult;
}

/*
    MDNSResponder::_buildDomainForService

    Builds the domain for the given service properties (eg. _http._tcp.local).
    The usual prepended '_' are added, if missing in the input strings.

*/
bool MDNSResponder::_buildDomainForService(const char* p_pcService,
        const char* p_pcProtocol,
        MDNSResponder::stcMDNS_RRDomain& p_rServiceDomain) const
{

    p_rServiceDomain.clear();
    bool    bResult = ((p_pcService) &&
                       (p_pcProtocol) &&
                       (p_rServiceDomain.addLabel(p_pcService, ('_' != *p_pcService))) &&
                       (p_rServiceDomain.addLabel(p_pcProtocol, ('_' != *p_pcProtocol))) &&
                       (p_rServiceDomain.addLabel(scpcLocal)) &&
                       (p_rServiceDomain.addLabel(0)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _buildDomainForService: FAILED for (%s.%s)!\n"), (p_pcService ? : "-"), (p_pcProtocol ? : "-")););
    return bResult;
}

#ifdef MDNS_IP4_SUPPORT
/*
    MDNSResponder::_buildDomainForReverseIP4

    The IP4 address is stringized by printing the four address bytes into a char buffer in reverse order
    and adding 'in-addr.arpa' (eg. 012.789.456.123.in-addr.arpa).
    Used while detecting reverse IP4 questions and answering these
*/
bool MDNSResponder::_buildDomainForReverseIP4(IPAddress p_IP4Address,
        MDNSResponder::stcMDNS_RRDomain& p_rReverseIP4Domain) const
{

    bool    bResult = true;

    p_rReverseIP4Domain.clear();

    char    acBuffer[32];
    for (int i = MDNS_IP4_SIZE; ((bResult) && (i >= 1)); --i)
    {
        itoa(p_IP4Address[i - 1], acBuffer, 10);
        bResult = p_rReverseIP4Domain.addLabel(acBuffer);
    }
    bResult = ((bResult) &&
               (p_rReverseIP4Domain.addLabel(scpcReverseIP4Domain)) &&
               (p_rReverseIP4Domain.addLabel(scpcReverseTopDomain)) &&
               (p_rReverseIP4Domain.addLabel(0)));
    DEBUG_EX_ERR(if (!bResult) DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _buildDomainForReverseIP4: FAILED!\n")););
    return bResult;
}
#endif

#ifdef MDNS_IP6_SUPPORT
/*
    MDNSResponder::_buildDomainForReverseIP6

    Used while detecting reverse IP6 questions and answering these
*/
bool MDNSResponder::_buildDomainForReverseIP6(IPAddress p_IP4Address,
        MDNSResponder::stcMDNS_RRDomain& p_rReverseIP6Domain) const
{
    // TODO: Implement
    return false;
}
#endif


/*
    UDP
*/

/*
    MDNSResponder::_udpReadBuffer
*/
bool MDNSResponder::_udpReadBuffer(unsigned char* p_pBuffer,
                                   size_t p_stLength)
{

    bool    bResult = ((m_pUDPContext) &&
                       (true/*m_pUDPContext->getSize() > p_stLength*/) &&
                       (p_pBuffer) &&
                       (p_stLength) &&
                       ((p_stLength == m_pUDPContext->read((char*)p_pBuffer, p_stLength))));
    DEBUG_EX_ERR(if (!bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _udpReadBuffer: FAILED!\n"));
    });
    return bResult;
}

/*
    MDNSResponder::_udpRead8
*/
bool MDNSResponder::_udpRead8(uint8_t& p_ru8Value)
{

    return _udpReadBuffer((unsigned char*)&p_ru8Value, sizeof(p_ru8Value));
}

/*
    MDNSResponder::_udpRead16
*/
bool MDNSResponder::_udpRead16(uint16_t& p_ru16Value)
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
bool MDNSResponder::_udpRead32(uint32_t& p_ru32Value)
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
bool MDNSResponder::_udpAppendBuffer(const unsigned char* p_pcBuffer,
                                     size_t p_stLength)
{

    bool bResult = ((m_pUDPContext) &&
                    (p_pcBuffer) &&
                    (p_stLength) &&
                    (p_stLength == m_pUDPContext->append((const char*)p_pcBuffer, p_stLength)));
    DEBUG_EX_ERR(if (!bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _udpAppendBuffer: FAILED!\n"));
    });
    return bResult;
}

/*
    MDNSResponder::_udpAppend8
*/
bool MDNSResponder::_udpAppend8(uint8_t p_u8Value)
{

    return (_udpAppendBuffer((unsigned char*)&p_u8Value, sizeof(p_u8Value)));
}

/*
    MDNSResponder::_udpAppend16
*/
bool MDNSResponder::_udpAppend16(uint16_t p_u16Value)
{

    p_u16Value = lwip_htons(p_u16Value);
    return (_udpAppendBuffer((unsigned char*)&p_u16Value, sizeof(p_u16Value)));
}

/*
    MDNSResponder::_udpAppend32
*/
bool MDNSResponder::_udpAppend32(uint32_t p_u32Value)
{

    p_u32Value = lwip_htonl(p_u32Value);
    return (_udpAppendBuffer((unsigned char*)&p_u32Value, sizeof(p_u32Value)));
}

#ifdef DEBUG_ESP_MDNS_RESPONDER
/*
    MDNSResponder::_udpDump
*/
bool MDNSResponder::_udpDump(bool p_bMovePointer /*= false*/)
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
bool MDNSResponder::_udpDump(unsigned p_uOffset,
                             unsigned p_uLength)
{

    if ((m_pUDPContext) &&
            (m_pUDPContext->isValidOffset(p_uOffset)))
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
#endif


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

    All 16-bit and 32-bit elements need to be translated form network coding to host coding (done in _udpRead16 and _udpRead32)
    In addition, bitfield memory order is undefined in C standard (GCC doesn't order them in the coded direction...), so they
    need some mapping here
*/
bool MDNSResponder::_readMDNSMsgHeader(MDNSResponder::stcMDNS_MsgHeader& p_rMsgHeader)
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

        p_rMsgHeader.m_1bQR     = (u8B1 & 0x80);    // Query/Responde flag
        p_rMsgHeader.m_4bOpcode = (u8B1 & 0x78);    // Operation code (0: Standard query, others ignored)
        p_rMsgHeader.m_1bAA     = (u8B1 & 0x04);    // Authorative answer
        p_rMsgHeader.m_1bTC     = (u8B1 & 0x02);    // Truncation flag
        p_rMsgHeader.m_1bRD     = (u8B1 & 0x01);    // Recursion desired

        p_rMsgHeader.m_1bRA     = (u8B2 & 0x80);    // Recursion available
        p_rMsgHeader.m_3bZ      = (u8B2 & 0x70);    // Zero
        p_rMsgHeader.m_4bRCode  = (u8B2 & 0x0F);    // Response code

        /*  DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readMDNSMsgHeader: ID:%u QR:%u OP:%u AA:%u TC:%u RD:%u RA:%u R:%u QD:%u AN:%u NS:%u AR:%u\n"),
                (unsigned)p_rMsgHeader.m_u16ID,
                (unsigned)p_rMsgHeader.m_1bQR, (unsigned)p_rMsgHeader.m_4bOpcode, (unsigned)p_rMsgHeader.m_1bAA, (unsigned)p_rMsgHeader.m_1bTC, (unsigned)p_rMsgHeader.m_1bRD,
                (unsigned)p_rMsgHeader.m_1bRA, (unsigned)p_rMsgHeader.m_4bRCode,
                (unsigned)p_rMsgHeader.m_u16QDCount,
                (unsigned)p_rMsgHeader.m_u16ANCount,
                (unsigned)p_rMsgHeader.m_u16NSCount,
                (unsigned)p_rMsgHeader.m_u16ARCount););*/
        bResult = true;
    }
    DEBUG_EX_ERR(if (!bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _readMDNSMsgHeader: FAILED!\n"));
    });
    return bResult;
}

/*
    MDNSResponder::_write8
*/
bool MDNSResponder::_write8(uint8_t p_u8Value,
                            MDNSResponder::stcMDNSSendParameter& p_rSendParameter)
{

    return ((_udpAppend8(p_u8Value)) &&
            (p_rSendParameter.shiftOffset(sizeof(p_u8Value))));
}

/*
    MDNSResponder::_write16
*/
bool MDNSResponder::_write16(uint16_t p_u16Value,
                             MDNSResponder::stcMDNSSendParameter& p_rSendParameter)
{

    return ((_udpAppend16(p_u16Value)) &&
            (p_rSendParameter.shiftOffset(sizeof(p_u16Value))));
}

/*
    MDNSResponder::_write32
*/
bool MDNSResponder::_write32(uint32_t p_u32Value,
                             MDNSResponder::stcMDNSSendParameter& p_rSendParameter)
{

    return ((_udpAppend32(p_u32Value)) &&
            (p_rSendParameter.shiftOffset(sizeof(p_u32Value))));
}

/*
    MDNSResponder::_writeMDNSMsgHeader

    Write MDNS header to the UDP output buffer.

    All 16-bit and 32-bit elements need to be translated form host coding to network coding (done in _udpAppend16 and _udpAppend32)
    In addition, bitfield memory order is undefined in C standard (GCC doesn't order them in the coded direction...), so they
    need some mapping here
*/
bool MDNSResponder::_writeMDNSMsgHeader(const MDNSResponder::stcMDNS_MsgHeader& p_MsgHeader,
                                        MDNSResponder::stcMDNSSendParameter& p_rSendParameter)
{
    /*  DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSMsgHeader: ID:%u QR:%u OP:%u AA:%u TC:%u RD:%u RA:%u R:%u QD:%u AN:%u NS:%u AR:%u\n"),
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

    DEBUG_EX_ERR(if (!bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSMsgHeader: FAILED!\n"));
    });
    return bResult;
}

/*
    MDNSResponder::_writeRRAttributes
*/
bool MDNSResponder::_writeMDNSRRAttributes(const MDNSResponder::stcMDNS_RRAttributes& p_Attributes,
        MDNSResponder::stcMDNSSendParameter& p_rSendParameter)
{

    bool    bResult = ((_write16(p_Attributes.m_u16Type, p_rSendParameter)) &&
                       (_write16(p_Attributes.m_u16Class, p_rSendParameter)));

    DEBUG_EX_ERR(if (!bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSRRAttributes: FAILED!\n"));
    });
    return bResult;
}

/*
    MDNSResponder::_writeMDNSRRDomain
*/
bool MDNSResponder::_writeMDNSRRDomain(const MDNSResponder::stcMDNS_RRDomain& p_Domain,
                                       MDNSResponder::stcMDNSSendParameter& p_rSendParameter)
{

    bool    bResult = ((_udpAppendBuffer((const unsigned char*)p_Domain.m_acName, p_Domain.m_u16NameLength)) &&
                       (p_rSendParameter.shiftOffset(p_Domain.m_u16NameLength)));

    DEBUG_EX_ERR(if (!bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSRRDomain: FAILED!\n"));
    });
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
bool MDNSResponder::_writeMDNSHostDomain(const char* p_pcHostname,
        bool p_bPrependRDLength,
        MDNSResponder::stcMDNSSendParameter& p_rSendParameter)
{

    // The 'skip-compression' version is handled in '_writeMDNSAnswer_SRV'
    uint16_t            u16CachedDomainOffset = p_rSendParameter.findCachedDomainOffset((const void*)p_pcHostname, false);

    stcMDNS_RRDomain    hostDomain;
    bool    bResult = (u16CachedDomainOffset
                       // Found cached domain -> mark as compressed domain
                       ? ((MDNS_DOMAIN_COMPRESS_MARK > ((u16CachedDomainOffset >> 8) & ~MDNS_DOMAIN_COMPRESS_MARK)) && // Valid offset
                          ((!p_bPrependRDLength) ||
                           (_write16(2, p_rSendParameter))) &&                                     // Length of 'Cxxx'
                          (_write8(((u16CachedDomainOffset >> 8) | MDNS_DOMAIN_COMPRESS_MARK), p_rSendParameter)) &&   // Compression mark (and offset)
                          (_write8((uint8_t)(u16CachedDomainOffset & 0xFF), p_rSendParameter)))
                       // No cached domain -> add this domain to cache and write full domain name
                       : ((_buildDomainForHost(p_pcHostname, hostDomain)) &&                       // eg. esp8266.local
                          ((!p_bPrependRDLength) ||
                           (_write16(hostDomain.m_u16NameLength, p_rSendParameter))) &&            // RDLength (if needed)
                          (p_rSendParameter.addDomainCacheItem((const void*)p_pcHostname, false, p_rSendParameter.m_u16Offset)) &&
                          (_writeMDNSRRDomain(hostDomain, p_rSendParameter))));

    DEBUG_EX_ERR(if (!bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSHostDomain: FAILED!\n"));
    });
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
bool MDNSResponder::_writeMDNSServiceDomain(const MDNSResponder::stcMDNSService& p_Service,
        bool p_bIncludeName,
        bool p_bPrependRDLength,
        MDNSResponder::stcMDNSSendParameter& p_rSendParameter)
{

    // The 'skip-compression' version is handled in '_writeMDNSAnswer_SRV'
    uint16_t            u16CachedDomainOffset = p_rSendParameter.findCachedDomainOffset((const void*)&p_Service, p_bIncludeName);

    stcMDNS_RRDomain    serviceDomain;
    bool    bResult = (u16CachedDomainOffset
                       // Found cached domain -> mark as compressed domain
                       ? ((MDNS_DOMAIN_COMPRESS_MARK > ((u16CachedDomainOffset >> 8) & ~MDNS_DOMAIN_COMPRESS_MARK)) && // Valid offset
                          ((!p_bPrependRDLength) ||
                           (_write16(2, p_rSendParameter))) &&                                     // Lenght of 'Cxxx'
                          (_write8(((u16CachedDomainOffset >> 8) | MDNS_DOMAIN_COMPRESS_MARK), p_rSendParameter)) &&   // Compression mark (and offset)
                          (_write8((uint8_t)(u16CachedDomainOffset & 0xFF), p_rSendParameter)))
                       // No cached domain -> add this domain to cache and write full domain name
                       : ((_buildDomainForService(p_Service, p_bIncludeName, serviceDomain)) &&    // eg. MyESP._http._tcp.local
                          ((!p_bPrependRDLength) ||
                           (_write16(serviceDomain.m_u16NameLength, p_rSendParameter))) &&         // RDLength (if needed)
                          (p_rSendParameter.addDomainCacheItem((const void*)&p_Service, p_bIncludeName, p_rSendParameter.m_u16Offset)) &&
                          (_writeMDNSRRDomain(serviceDomain, p_rSendParameter))));

    DEBUG_EX_ERR(if (!bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSServiceDomain: FAILED!\n"));
    });
    return bResult;

}

/*
    MDNSResponder::_writeMDNSQuestion

    Write a MDNS question to the UDP output buffer

    QNAME  (host/service domain, eg. esp8266.local)
    QTYPE  (16bit, eg. ANY)
    QCLASS (16bit, eg. IN)

*/
bool MDNSResponder::_writeMDNSQuestion(MDNSResponder::stcMDNS_RRQuestion& p_Question,
                                       MDNSResponder::stcMDNSSendParameter& p_rSendParameter)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSQuestion\n")););

    bool    bResult = ((_writeMDNSRRDomain(p_Question.m_Header.m_Domain, p_rSendParameter)) &&
                       (_writeMDNSRRAttributes(p_Question.m_Header.m_Attributes, p_rSendParameter)));

    DEBUG_EX_ERR(if (!bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSQuestion: FAILED!\n"));
    });
    return bResult;

}


#ifdef MDNS_IP4_SUPPORT
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
bool MDNSResponder::_writeMDNSAnswer_A(IPAddress p_IPAddress,
                                       MDNSResponder::stcMDNSSendParameter& p_rSendParameter)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSAnswer_A (%s)\n"), p_IPAddress.toString().c_str()););

    stcMDNS_RRAttributes    attributes(DNS_RRTYPE_A,
                                       ((p_rSendParameter.m_bCacheFlush ? 0x8000 : 0) | DNS_RRCLASS_IN));   // Cache flush? & INternet
    const unsigned char     aucIPAddress[MDNS_IP4_SIZE] = { p_IPAddress[0], p_IPAddress[1], p_IPAddress[2], p_IPAddress[3] };
    bool    bResult = ((_writeMDNSHostDomain(m_pcHostname, false, p_rSendParameter)) &&
                       (_writeMDNSRRAttributes(attributes, p_rSendParameter)) &&        // TYPE & CLASS
                       (_write32((p_rSendParameter.m_bUnannounce ? 0 : MDNS_HOST_TTL), p_rSendParameter)) &&    // TTL
                       (_write16(MDNS_IP4_SIZE, p_rSendParameter)) &&                   // RDLength
                       (_udpAppendBuffer(aucIPAddress, MDNS_IP4_SIZE)) &&               // RData
                       (p_rSendParameter.shiftOffset(MDNS_IP4_SIZE)));

    DEBUG_EX_ERR(if (!bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSAnswer_A: FAILED!\n"));
    });
    return bResult;

}

/*
    MDNSResponder::_writeMDNSAnswer_PTR_IP4

    Write a MDNS reverse IP4 PTR answer to the UDP output buffer.
    See: '_writeMDNSAnswer_A'

    eg. 012.789.456.123.in-addr.arpa PTR 0x8001 120 15 esp8266.local
    Used while answering reverse IP4 questions
*/
bool MDNSResponder::_writeMDNSAnswer_PTR_IP4(IPAddress p_IPAddress,
        MDNSResponder::stcMDNSSendParameter& p_rSendParameter)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSAnswer_PTR_IP4 (%s)\n"), p_IPAddress.toString().c_str()););

    stcMDNS_RRDomain        reverseIP4Domain;
    stcMDNS_RRAttributes    attributes(DNS_RRTYPE_PTR,
                                       ((p_rSendParameter.m_bCacheFlush ? 0x8000 : 0) | DNS_RRCLASS_IN));   // Cache flush? & INternet
    stcMDNS_RRDomain        hostDomain;
    bool    bResult = ((_buildDomainForReverseIP4(p_IPAddress, reverseIP4Domain)) &&    // 012.789.456.123.in-addr.arpa
                       (_writeMDNSRRDomain(reverseIP4Domain, p_rSendParameter)) &&
                       (_writeMDNSRRAttributes(attributes, p_rSendParameter)) &&        // TYPE & CLASS
                       (_write32((p_rSendParameter.m_bUnannounce ? 0 : MDNS_HOST_TTL), p_rSendParameter)) &&    // TTL
                       (_writeMDNSHostDomain(m_pcHostname, true, p_rSendParameter)));   // RDLength & RData (host domain, eg. esp8266.local)

    DEBUG_EX_ERR(if (!bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSAnswer_PTR_IP4: FAILED!\n"));
    });
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
bool MDNSResponder::_writeMDNSAnswer_PTR_TYPE(MDNSResponder::stcMDNSService& p_rService,
        MDNSResponder::stcMDNSSendParameter& p_rSendParameter)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSAnswer_PTR_TYPE\n")););

    stcMDNS_RRDomain        dnssdDomain;
    stcMDNS_RRDomain        serviceDomain;
    stcMDNS_RRAttributes    attributes(DNS_RRTYPE_PTR, DNS_RRCLASS_IN);	// No cache flush! only INternet
    bool    bResult = ((_buildDomainForDNSSD(dnssdDomain)) &&                                   // _services._dns-sd._udp.local
                       (_writeMDNSRRDomain(dnssdDomain, p_rSendParameter)) &&
                       (_writeMDNSRRAttributes(attributes, p_rSendParameter)) &&                // TYPE & CLASS
                       (_write32((p_rSendParameter.m_bUnannounce ? 0 : MDNS_SERVICE_TTL), p_rSendParameter)) && // TTL
                       (_writeMDNSServiceDomain(p_rService, false, true, p_rSendParameter)));   // RDLength & RData (service domain, eg. _http._tcp.local)

    DEBUG_EX_ERR(if (!bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSAnswer_PTR_TYPE: FAILED!\n"));
    });
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
bool MDNSResponder::_writeMDNSAnswer_PTR_NAME(MDNSResponder::stcMDNSService& p_rService,
        MDNSResponder::stcMDNSSendParameter& p_rSendParameter)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSAnswer_PTR_NAME\n")););

    stcMDNS_RRAttributes    attributes(DNS_RRTYPE_PTR, DNS_RRCLASS_IN);	// No cache flush! only INternet
    bool    bResult = ((_writeMDNSServiceDomain(p_rService, false, false, p_rSendParameter)) && // _http._tcp.local
                       (_writeMDNSRRAttributes(attributes, p_rSendParameter)) &&                    // TYPE & CLASS
                       (_write32((p_rSendParameter.m_bUnannounce ? 0 : MDNS_SERVICE_TTL), p_rSendParameter)) && // TTL
                       (_writeMDNSServiceDomain(p_rService, true, true, p_rSendParameter)));        // RDLength & RData (service domain, eg. MyESP._http._tcp.local)

    DEBUG_EX_ERR(if (!bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSAnswer_PTR_NAME: FAILED!\n"));
    });
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
bool MDNSResponder::_writeMDNSAnswer_TXT(MDNSResponder::stcMDNSService& p_rService,
        MDNSResponder::stcMDNSSendParameter& p_rSendParameter)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSAnswer_TXT\n")););

    bool                    bResult = false;

    stcMDNS_RRAttributes    attributes(DNS_RRTYPE_TXT,
                                       ((p_rSendParameter.m_bCacheFlush ? 0x8000 : 0) | DNS_RRCLASS_IN));   // Cache flush? & INternet

    if ((_collectServiceTxts(p_rService)) &&
            (_writeMDNSServiceDomain(p_rService, true, false, p_rSendParameter)) &&     // MyESP._http._tcp.local
            (_writeMDNSRRAttributes(attributes, p_rSendParameter)) &&                   // TYPE & CLASS
            (_write32((p_rSendParameter.m_bUnannounce ? 0 : MDNS_SERVICE_TTL), p_rSendParameter)) &&    // TTL
            (_write16(p_rService.m_Txts.length(), p_rSendParameter)))                   // RDLength
    {

        bResult = true;
        // RData    Txts
        for (stcMDNSServiceTxt* pTxt = p_rService.m_Txts.m_pTxts; ((bResult) && (pTxt)); pTxt = pTxt->m_pNext)
        {
            unsigned char       ucLengthByte = pTxt->length();
            bResult = ((_udpAppendBuffer((unsigned char*)&ucLengthByte, sizeof(ucLengthByte))) &&   // Length
                       (p_rSendParameter.shiftOffset(sizeof(ucLengthByte))) &&
                       ((size_t)os_strlen(pTxt->m_pcKey) == m_pUDPContext->append(pTxt->m_pcKey, os_strlen(pTxt->m_pcKey))) &&          // Key
                       (p_rSendParameter.shiftOffset((size_t)os_strlen(pTxt->m_pcKey))) &&
                       (1 == m_pUDPContext->append("=", 1)) &&                                                                          // =
                       (p_rSendParameter.shiftOffset(1)) &&
                       ((!pTxt->m_pcValue) ||
                        (((size_t)os_strlen(pTxt->m_pcValue) == m_pUDPContext->append(pTxt->m_pcValue, os_strlen(pTxt->m_pcValue))) &&  // Value
                         (p_rSendParameter.shiftOffset((size_t)os_strlen(pTxt->m_pcValue))))));

            DEBUG_EX_ERR(if (!bResult)
        {
            DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSAnswer_TXT: FAILED to write %sTxt %s=%s!\n"), (pTxt->m_bTemp ? "temp. " : ""), (pTxt->m_pcKey ? : "?"), (pTxt->m_pcValue ? : "?"));
            });
        }
    }
    _releaseTempServiceTxts(p_rService);

    DEBUG_EX_ERR(if (!bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSAnswer_TXT: FAILED!\n"));
    });
    return bResult;
}

#ifdef MDNS_IP6_SUPPORT
/*
    MDNSResponder::_writeMDNSAnswer_AAAA

    Write a MDNS AAAA answer to the UDP output buffer.
    See: '_writeMDNSAnswer_A'

    eg. esp8266.local AAAA 0x8001 120 16 xxxx::xx
    http://www.zytrax.com/books/dns/ch8/aaaa.html
*/
bool MDNSResponder::_writeMDNSAnswer_AAAA(IPAddress p_IPAddress,
        MDNSResponder::stcMDNSSendParameter& p_rSendParameter)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSAnswer_AAAA\n")););

    stcMDNS_RRAttributes    attributes(DNS_RRTYPE_AAAA,
                                       ((p_rSendParameter.m_bCacheFlush ? 0x8000 : 0) | DNS_RRCLASS_IN));   // Cache flush? & INternet
    bool    bResult = ((_writeMDNSHostDomain(m_pcHostname, false, p_rSendParameter)) && // esp8266.local
                       (_writeMDNSRRAttributes(attributes, p_rSendParameter)) &&            // TYPE & CLASS
                       (_write32((p_rSendParameter.m_bUnannounce ? 0 : MDNS_HOST_TTL), p_rSendParameter)) &&    // TTL
                       (_write16(MDNS_IP6_SIZE, p_rSendParameter)) &&                       // RDLength
                       (false /*TODO: IP6 version of: _udpAppendBuffer((uint32_t)p_IPAddress, MDNS_IP4_SIZE)*/));   // RData

    DEBUG_EX_ERR(if (!bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSAnswer_AAAA: FAILED!\n"));
    });
    return bResult;
}

/*
    MDNSResponder::_writeMDNSAnswer_PTR_IP6

    Write a MDNS reverse IP6 PTR answer to the UDP output buffer.
    See: '_writeMDNSAnswer_A'

    eg. xxxx::xx.in6.arpa PTR 0x8001 120 15 esp8266.local
    Used while answering reverse IP6 questions
*/
bool MDNSResponder::_writeMDNSAnswer_PTR_IP6(IPAddress p_IPAddress,
        MDNSResponder::stcMDNSSendParameter& p_rSendParameter)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSAnswer_PTR_IP6\n")););

    stcMDNS_RRDomain        reverseIP6Domain;
    stcMDNS_RRAttributes    attributes(DNS_RRTYPE_PTR,
                                       ((p_rSendParameter.m_bCacheFlush ? 0x8000 : 0) | DNS_RRCLASS_IN));   // Cache flush? & INternet
    bool    bResult = ((_buildDomainForReverseIP6(p_IPAddress, reverseIP6Domain)) &&        // xxxx::xx.ip6.arpa
                       (_writeMDNSRRDomain(reverseIP6Domain, p_rSendParameter)) &&
                       (_writeMDNSRRAttributes(attributes, p_rSendParameter)) &&            // TYPE & CLASS
                       (_write32((p_rSendParameter.m_bUnannounce ? 0 : MDNS_HOST_TTL), p_rSendParameter)) &&    // TTL
                       (_writeMDNSHostDomain(m_pcHostname, true, p_rSendParameter)));       // RDLength & RData (host domain, eg. esp8266.local)

    DEBUG_EX_ERR(if (!bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSAnswer_PTR_IP6: FAILED!\n"));
    });
    return bResult;
}
#endif

/*
    MDNSResponder::_writeMDNSAnswer_SRV

    eg. MyESP._http.tcp.local SRV 0x8001 120 0 0 60068 esp8266.local
    http://www.zytrax.com/books/dns/ch8/srv.html ???? Include instance name ????
*/
bool MDNSResponder::_writeMDNSAnswer_SRV(MDNSResponder::stcMDNSService& p_rService,
        MDNSResponder::stcMDNSSendParameter& p_rSendParameter)
{
    DEBUG_EX_INFO(DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSAnswer_SRV\n")););

    uint16_t                u16CachedDomainOffset = (p_rSendParameter.m_bLegacyQuery
            ? 0
            : p_rSendParameter.findCachedDomainOffset((const void*)m_pcHostname, false));

    stcMDNS_RRAttributes    attributes(DNS_RRTYPE_SRV,
                                       ((p_rSendParameter.m_bCacheFlush ? 0x8000 : 0) | DNS_RRCLASS_IN));   // Cache flush? & INternet
    stcMDNS_RRDomain        hostDomain;
    bool    bResult = ((_writeMDNSServiceDomain(p_rService, true, false, p_rSendParameter)) &&  // MyESP._http._tcp.local
                       (_writeMDNSRRAttributes(attributes, p_rSendParameter)) &&                // TYPE & CLASS
                       (_write32((p_rSendParameter.m_bUnannounce ? 0 : MDNS_SERVICE_TTL), p_rSendParameter)) && // TTL
                       (!u16CachedDomainOffset
                        // No cache for domain name (or no compression allowed)
                        ? ((_buildDomainForHost(m_pcHostname, hostDomain)) &&
                           (_write16((sizeof(uint16_t /*Prio*/) +                           // RDLength
                                      sizeof(uint16_t /*Weight*/) +
                                      sizeof(uint16_t /*Port*/) +
                                      hostDomain.m_u16NameLength), p_rSendParameter)) &&    // Domain length
                           (_write16(MDNS_SRV_PRIORITY, p_rSendParameter)) &&               // Priority
                           (_write16(MDNS_SRV_WEIGHT, p_rSendParameter)) &&                 // Weight
                           (_write16(p_rService.m_u16Port, p_rSendParameter)) &&            // Port
                           (p_rSendParameter.addDomainCacheItem((const void*)m_pcHostname, false, p_rSendParameter.m_u16Offset)) &&
                           (_writeMDNSRRDomain(hostDomain, p_rSendParameter)))              // Host, eg. esp8266.local
                        // Cache available for domain
                        : ((MDNS_DOMAIN_COMPRESS_MARK > ((u16CachedDomainOffset >> 8) & ~MDNS_DOMAIN_COMPRESS_MARK)) && // Valid offset
                           (_write16((sizeof(uint16_t /*Prio*/) +                           // RDLength
                                      sizeof(uint16_t /*Weight*/) +
                                      sizeof(uint16_t /*Port*/) +
                                      2), p_rSendParameter)) &&                             // Length of 'C0xx'
                           (_write16(MDNS_SRV_PRIORITY, p_rSendParameter)) &&               // Priority
                           (_write16(MDNS_SRV_WEIGHT, p_rSendParameter)) &&                 // Weight
                           (_write16(p_rService.m_u16Port, p_rSendParameter)) &&            // Port
                           (_write8(((u16CachedDomainOffset >> 8) | MDNS_DOMAIN_COMPRESS_MARK), p_rSendParameter)) &&   // Compression mark (and offset)
                           (_write8((uint8_t)u16CachedDomainOffset, p_rSendParameter)))));  // Offset

    DEBUG_EX_ERR(if (!bResult)
{
    DEBUG_OUTPUT.printf_P(PSTR("[MDNSResponder] _writeMDNSAnswer_SRV: FAILED!\n"));
    });
    return bResult;
}

}   // namespace MDNSImplementation

} // namespace esp8266






