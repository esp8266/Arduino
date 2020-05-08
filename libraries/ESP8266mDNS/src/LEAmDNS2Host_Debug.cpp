/*
    LEAmDNS2Host_Debug.h

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

#include "LEAmDNS2Host.h"


namespace esp8266
{


namespace experimental
{


#ifdef DEBUG_ESP_MDNS_RESPONDER

/*
    clsLEAmDNS2_Host::_DH

*/
const char* clsLEAMDNSHost::_DH(const clsLEAMDNSHost::clsService* p_pService /*= 0*/) const
{
    static char acBuffer[16 + 64];

    *acBuffer = 0;
    if (m_pNetIf)
    {
        sprintf_P(acBuffer, PSTR("[mDNS %c%c%u]"), m_pNetIf->name[0], m_pNetIf->name[1], m_pNetIf->num);
        if (p_pService)
        {
            strcat_P(acBuffer, PSTR(">"));
            strcat(acBuffer, _service2String(p_pService));
        }
    }
    else
    {
        strcpy_P(acBuffer, PSTR("[mDNS]"));
    }
    return acBuffer;
}

/*
    clsLEAmDNS2_Host::_service2String

*/
const char* clsLEAMDNSHost::_service2String(const clsLEAMDNSHost::clsService* p_pService) const
{
    static  char acBuffer[64];

    *acBuffer = 0;
    if (p_pService)
    {
        sprintf_P(acBuffer, PSTR("%s.%s%s.%s%s.local"),
                  (p_pService->m_pcInstanceName ? : "-"),
                  (p_pService->m_pcType ? ('_' == *(p_pService->m_pcType) ? "" : "_") : "-"),
                  (p_pService->m_pcType ? : "-"),
                  (p_pService->m_pcProtocol ? ('_' == *(p_pService->m_pcProtocol) ? "" : "_") : "-"),
                  (p_pService->m_pcProtocol ? : "-"));
    }
    return acBuffer;
}

/*
    clsLEAmDNS2_Host::_printRRDomain

*/
bool clsLEAMDNSHost::_printRRDomain(const clsLEAMDNSHost::clsRRDomain& p_RRDomain) const
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
    clsLEAmDNS2_Host::_printRRAnswer

*/
bool clsLEAMDNSHost::_printRRAnswer(const clsLEAMDNSHost::clsRRAnswer& p_RRAnswer) const
{
    DEBUG_OUTPUT.printf_P(PSTR("%s RRAnswer: "), _DH());
    _printRRDomain(p_RRAnswer.m_Header.m_Domain);
    DEBUG_OUTPUT.printf_P(PSTR(" Type:0x%04X Class:0x%04X TTL:%u, "), p_RRAnswer.m_Header.m_Attributes.m_u16Type, p_RRAnswer.m_Header.m_Attributes.m_u16Class, p_RRAnswer.m_u32TTL);
    switch (p_RRAnswer.m_Header.m_Attributes.m_u16Type & (~0x8000))     // Topmost bit might carry 'cache flush' flag
    {
#ifdef MDNS_IPV4_SUPPORT
    case DNS_RRTYPE_A:
        DEBUG_OUTPUT.printf_P(PSTR("A IP:%s"), ((const clsRRAnswerA*)&p_RRAnswer)->m_IPAddress.toString().c_str());
        break;
#endif
    case DNS_RRTYPE_PTR:
        DEBUG_OUTPUT.printf_P(PSTR("PTR "));
        _printRRDomain(((const clsRRAnswerPTR*)&p_RRAnswer)->m_PTRDomain);
        break;
    case DNS_RRTYPE_TXT:
    {
        size_t  stTxtLength = ((const clsRRAnswerTXT*)&p_RRAnswer)->m_Txts.c_strLength();
        char*   pTxts = new char[stTxtLength];
        if (pTxts)
        {
            ((/*const c_str()!!*/clsRRAnswerTXT*)&p_RRAnswer)->m_Txts.c_str(pTxts);
            DEBUG_OUTPUT.printf_P(PSTR("TXT(%u) %s"), stTxtLength, pTxts);
            delete[] pTxts;
        }
        break;
    }
#ifdef MDNS_IPV6_SUPPORT
    case DNS_RRTYPE_AAAA:
        DEBUG_OUTPUT.printf_P(PSTR("AAAA IP:%s"), ((clsRRAnswerAAAA*&)p_RRAnswer)->m_IPAddress.toString().c_str());
        break;
#endif
    case DNS_RRTYPE_SRV:
        DEBUG_OUTPUT.printf_P(PSTR("SRV Port:%u "), ((const clsRRAnswerSRV*)&p_RRAnswer)->m_u16Port);
        _printRRDomain(((const clsRRAnswerSRV*)&p_RRAnswer)->m_SRVDomain);
        break;
    default:
        DEBUG_OUTPUT.printf_P(PSTR("generic "));
        break;
    }
    DEBUG_OUTPUT.printf_P(PSTR("\n"));

    return true;
}

/*
    clsLEAmDNS2_Host::_RRType2Name

*/
const char* clsLEAMDNSHost::_RRType2Name(uint16_t p_u16RRType) const
{
    static char acRRName[16];
    *acRRName = 0;

    switch (p_u16RRType & (~0x8000))    // Topmost bit might carry 'cache flush' flag
    {
#ifdef MDNS_IPV4_SUPPORT
    case DNS_RRTYPE_A:              strcpy(acRRName, "A");      break;
#endif
    case DNS_RRTYPE_PTR:            strcpy(acRRName, "PTR");    break;
    case DNS_RRTYPE_TXT:            strcpy(acRRName, "TXT");    break;
#ifdef MDNS_IPV6_SUPPORT
    case DNS_RRTYPE_AAAA:           strcpy(acRRName, "AAAA");   break;
#endif
    case DNS_RRTYPE_SRV:            strcpy(acRRName, "SRV");    break;
    case clsConsts::u8DNS_RRTYPE_NSEC: strcpy(acRRName, "NSEC");   break;
    case DNS_RRTYPE_ANY:            strcpy(acRRName, "ANY");    break;
    default:                        sprintf(acRRName, "Unknown(0x%04X)", p_u16RRType);  // MAX 15!
    }
    return acRRName;
}

/*
    clsLEAmDNS2_Host::_RRClass2String

*/
const char* clsLEAMDNSHost::_RRClass2String(uint16_t p_u16RRClass,
        bool p_bIsQuery) const
{
    static char acClassString[16];
    *acClassString = 0;

    if (p_u16RRClass & 0x0001)
    {
        strcat(acClassString, "IN ");    //  3
    }
    if (p_u16RRClass & 0x8000)
    {
        strcat(acClassString, (p_bIsQuery ? "UNICAST " : "FLUSH "));    //  8/6
    }

    return acClassString;                                                                       // 11
}

/*
    clsLEAmDNS2_Host::_replyFlags2String

*/
const char* clsLEAMDNSHost::_replyFlags2String(uint32_t p_u32ReplyFlags) const
{
    static char acFlagsString[64];

    *acFlagsString = 0;
    if (p_u32ReplyFlags & static_cast<uint32_t>(enuContentFlag::A))
    {
        strcat(acFlagsString, "A ");    //  2
    }
    if (p_u32ReplyFlags & static_cast<uint32_t>(enuContentFlag::PTR_IPv4))
    {
        strcat(acFlagsString, "PTR_IPv4 ");    //  7
    }
    if (p_u32ReplyFlags & static_cast<uint32_t>(enuContentFlag::PTR_IPv6))
    {
        strcat(acFlagsString, "PTR_IPv6 ");    //  7
    }
    if (p_u32ReplyFlags & static_cast<uint32_t>(enuContentFlag::AAAA))
    {
        strcat(acFlagsString, "AAAA ");    //  5
    }
    if (p_u32ReplyFlags & static_cast<uint32_t>(enuContentFlag::PTR_TYPE))
    {
        strcat(acFlagsString, "PTR_TYPE ");    //  9
    }
    if (p_u32ReplyFlags & static_cast<uint32_t>(enuContentFlag::PTR_NAME))
    {
        strcat(acFlagsString, "PTR_NAME ");    //  9
    }
    if (p_u32ReplyFlags & static_cast<uint32_t>(enuContentFlag::TXT))
    {
        strcat(acFlagsString, "TXT ");    //  4
    }
    if (p_u32ReplyFlags & static_cast<uint32_t>(enuContentFlag::SRV))
    {
        strcat(acFlagsString, "SRV ");    //  4
    }
    if (p_u32ReplyFlags & static_cast<uint32_t>(enuContentFlag::NSEC))
    {
        strcat(acFlagsString, "NSEC ");    //  5
    }

    if (0 == p_u32ReplyFlags)
    {
        strcpy(acFlagsString, "none");
    }

    // Remove trailing spaces
    while ((*acFlagsString) &&
            (' '  == acFlagsString[strlen(acFlagsString) - 1]))
    {
        acFlagsString[strlen(acFlagsString) - 1] = 0;
    }

    return acFlagsString;                                                                           // 63
}

/*
    clsLEAmDNS2_Host::_NSECBitmap2String

*/
const char* clsLEAMDNSHost::_NSECBitmap2String(const clsNSECBitmap* p_pNSECBitmap) const
{
    static char acFlagsString[32];

    *acFlagsString = 0;
#ifdef MDNS_IPV4_SUPPORT
    if (p_pNSECBitmap->getBit(DNS_RRTYPE_A))
    {
        strcat(acFlagsString, "A ");    //  2
    }
#endif
    if (p_pNSECBitmap->getBit(DNS_RRTYPE_PTR))
    {
        strcat(acFlagsString, "PTR ");  //  4
    }
#ifdef MDNS_IPV6_SUPPORT
    if (p_pNSECBitmap->getBit(DNS_RRTYPE_AAAA))
    {
        strcat(acFlagsString, "AAAA "); //  5
    }
#endif
    if (p_pNSECBitmap->getBit(DNS_RRTYPE_TXT))
    {
        strcat(acFlagsString, "TXT ");  //  4
    }
    if (p_pNSECBitmap->getBit(DNS_RRTYPE_SRV))
    {
        strcat(acFlagsString, "SRV ");  //  4
    }
    if (p_pNSECBitmap->getBit(clsConsts::u8DNS_RRTYPE_NSEC))
    {
        strcat(acFlagsString, "NSEC "); //  5
    }

    if (!*acFlagsString)
    {
        strcpy(acFlagsString, "none");
    }

    return acFlagsString;               // 31
}

#endif  // DEBUG_ESP_MDNS_RESPONDER


}   // namespace MDNSImplementation


}   // namespace esp8266




