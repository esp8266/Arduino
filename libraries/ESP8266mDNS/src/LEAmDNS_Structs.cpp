/*
 * LEAmDNS_Structs.cpp
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

#include "LEAmDNS_Priv.h"
#include "LEAmDNS_lwIPdefs.h"

/*
 * namespace LEAmDNS
 */
namespace LEAmDNS {

/**
 * STRUCTS
 */

/**
 * MDNSResponder::stcMDNSServiceTxt
 *
 * One MDNS TXT item.
 * m_pcValue may be '\0'.
 * Objects can be chained together (list, m_pNext).
 * A 'm_bTemp' flag differentiates between static and dynamic items.
 * Output as byte array 'c#=1' is supported.
 */

/*
 * MDNSResponder::stcMDNSServiceTxt::_stcMDNSServiceTxt constructor
 */
MDNSResponder::_stcMDNSServiceTxt::_stcMDNSServiceTxt(const char* p_pcKey /*= 0*/,
                                  const char* p_pcValue /*= 0*/,
                                  bool p_bTemp /*= false*/)
:   m_pNext(0),
    m_pcKey(0),
    m_pcValue(0),
    m_bTemp(p_bTemp) {
    
    setKey(p_pcKey);
    setValue(p_pcValue);
}

/*
 * MDNSResponder::_stcMDNSServiceTxt::_stcMDNSServiceTxt copy-constructor
 */
MDNSResponder::_stcMDNSServiceTxt::_stcMDNSServiceTxt(const MDNSResponder::_stcMDNSServiceTxt& p_Other)
:   m_pNext(0),
    m_pcKey(0),
    m_pcValue(0),
    m_bTemp(false) {
    
    operator=(p_Other);
}

/*
 * MDNSResponder::_stcMDNSServiceTxt::~_stcMDNSServiceTxt destructor
 */
MDNSResponder::_stcMDNSServiceTxt::~_stcMDNSServiceTxt(void) {
    
    clear();
}

/*
 * MDNSResponder::_stcMDNSServiceTxt::operator=
 */
MDNSResponder::_stcMDNSServiceTxt& MDNSResponder::_stcMDNSServiceTxt::operator=(const MDNSResponder::_stcMDNSServiceTxt& p_Other) {

    if (&p_Other != this) {
        clear();
        set(p_Other.m_pcKey, p_Other.m_pcValue, p_Other.m_bTemp);
    }
    return *this;
}

/*
 * MDNSResponder::_stcMDNSServiceTxt::clear
 */
bool MDNSResponder::_stcMDNSServiceTxt::clear(void) {
    
    releaseKey();
    releaseValue();
    return true;
}

/*
 * MDNSResponder::_stcMDNSServiceTxt::allocKey
 */
char* MDNSResponder::_stcMDNSServiceTxt::allocKey(size_t p_stLength) {
    
    releaseKey();
    if (p_stLength) {
        m_pcKey = new char[p_stLength + 1];
    }
    return m_pcKey;
}

/*
 * MDNSResponder::_stcMDNSServiceTxt::setKey
 */
bool MDNSResponder::_stcMDNSServiceTxt::setKey(const char* p_pcKey,
                                               size_t p_stLength) {
    
    bool bResult = false;
    
    releaseKey();
    if (p_stLength) {
        if (allocKey(p_stLength)) {
            os_strncpy(m_pcKey, p_pcKey, p_stLength);
            m_pcKey[p_stLength] = 0;
            bResult = true;
        }
    }
    return bResult;
}

/*
 * MDNSResponder::_stcMDNSServiceTxt::setKey
 */
bool MDNSResponder::_stcMDNSServiceTxt::setKey(const char* p_pcKey) {
    
    return setKey(p_pcKey, (p_pcKey ? os_strlen(p_pcKey) : 0));
}

/*
 * MDNSResponder::_stcMDNSServiceTxt::releaseKey
 */
bool MDNSResponder::_stcMDNSServiceTxt::releaseKey(void) {
    
    if (m_pcKey) {
        delete[] m_pcKey;
        m_pcKey = 0;
    }
    return true;
}

/*
 * MDNSResponder::_stcMDNSServiceTxt::allocValue
 */
char* MDNSResponder::_stcMDNSServiceTxt::allocValue(size_t p_stLength) {
    
    releaseValue();
    if (p_stLength) {
        m_pcValue = new char[p_stLength + 1];
    }
    return m_pcValue;
}

/*
 * MDNSResponder::_stcMDNSServiceTxt::setValue
 */
bool MDNSResponder::_stcMDNSServiceTxt::setValue(const char* p_pcValue,
                                                 size_t p_stLength) {
    
    bool bResult = false;
    
    releaseValue();
    if (p_stLength) {
        if (allocValue(p_stLength)) {
            os_strncpy(m_pcValue, p_pcValue, p_stLength);
            m_pcValue[p_stLength] = 0;
            bResult = true;
        }
    }
    else {  // No value -> also OK
        bResult = true;
    }
    return bResult;
}

/*
 * MDNSResponder::_stcMDNSServiceTxt::setValue
 */
bool MDNSResponder::_stcMDNSServiceTxt::setValue(const char* p_pcValue) {
    
    return setValue(p_pcValue, (p_pcValue ? os_strlen(p_pcValue) : 0));
}

/*
 * MDNSResponder::_stcMDNSServiceTxt::releaseValue
 */
bool MDNSResponder::_stcMDNSServiceTxt::releaseValue(void) {
    
    if (m_pcValue) {
        delete[] m_pcValue;
        m_pcValue = 0;
    }
    return true;
}

/*
 * MDNSResponder::_stcMDNSServiceTxt::set
 */
bool MDNSResponder::_stcMDNSServiceTxt::set(const char* p_pcKey,
                                            const char* p_pcValue,
                                            bool p_bTemp /*= false*/) {
    
    m_bTemp = p_bTemp;
    return ((setKey(p_pcKey)) &&
            (setValue(p_pcValue)));
}

/*
 * MDNSResponder::_stcMDNSServiceTxt::update
 */
bool MDNSResponder::_stcMDNSServiceTxt::update(const char* p_pcValue) {
    
    return setValue(p_pcValue);
}

/*
 * MDNSResponder::_stcMDNSServiceTxt::length
 *
 * length of eg. 'c#=1' without any closing '\0'
 */
size_t MDNSResponder::_stcMDNSServiceTxt::length(void) const {

    size_t  stLength = 0;
    if (m_pcKey) {
        stLength += os_strlen(m_pcKey);                     // Key
        stLength += 1;                                      // '='
        stLength += (m_pcValue ? os_strlen(m_pcValue) : 0); // Value
    }
    return stLength;
}

    
/**
 * MDNSResponder::stcMDNSServiceTxts
 *
 * A list of zero or more MDNS TXT items.
 * Dynamic TXT items can be removed by 'removeTempTxts'.
 * A TXT item can be looke up by its 'key' member.
 * Export as ';'-separated byte array is supported.
 * Export as 'length byte coded' byte array is supported.
 * Comparision ((all A TXT items in B and equal) AND (all B TXT items in A and equal)) is supported.
 *
 */

/*
 * MDNSResponder::_stcMDNSServiceTxts::_stcMDNSServiceTxts contructor
 */
MDNSResponder::_stcMDNSServiceTxts::_stcMDNSServiceTxts(void)
:   m_pTxts(0) {
    
}

/*
 * MDNSResponder::_stcMDNSServiceTxts::_stcMDNSServiceTxts copy-constructor
 */
MDNSResponder::_stcMDNSServiceTxts::_stcMDNSServiceTxts(const _stcMDNSServiceTxts& p_Other)
:   m_pTxts(0) {
    
    operator=(p_Other);
}

/*
 * MDNSResponder::_stcMDNSServiceTxts::~_stcMDNSServiceTxts destructor
 */
MDNSResponder::_stcMDNSServiceTxts::~_stcMDNSServiceTxts(void) {
    
    clear();
}

/*
 * MDNSResponder::_stcMDNSServiceTxts::operator=
 */
MDNSResponder::_stcMDNSServiceTxts& MDNSResponder::_stcMDNSServiceTxts::operator=(const _stcMDNSServiceTxts& p_Other) {

    if (this != &p_Other) {
        clear();
        
        for (stcMDNSServiceTxt* pOtherTxt=p_Other.m_pTxts; pOtherTxt; pOtherTxt=pOtherTxt->m_pNext) {
            add(new stcMDNSServiceTxt(*pOtherTxt));
        }
    }
    return *this;
}

/*
 * MDNSResponder::_stcMDNSServiceTxts::clear
 */
bool MDNSResponder::_stcMDNSServiceTxts::clear(void) {
    
    while (m_pTxts) {
        stcMDNSServiceTxt* pNext = m_pTxts->m_pNext;
        delete m_pTxts;
        m_pTxts = pNext;
    }
    return true;
}

/*
 * MDNSResponder::_stcMDNSServiceTxts::add
 */
bool MDNSResponder::_stcMDNSServiceTxts::add(MDNSResponder::stcMDNSServiceTxt* p_pTxt) {
    
    bool bResult = false;
    
    if (p_pTxt) {
        p_pTxt->m_pNext = m_pTxts;
        m_pTxts = p_pTxt;
        bResult = true;
    }
    return bResult;
}

/*
 * MDNSResponder::_stcMDNSServiceTxts::remove
 */
bool MDNSResponder::_stcMDNSServiceTxts::remove(stcMDNSServiceTxt* p_pTxt) {
    
    bool    bResult = false;
    
    if (p_pTxt) {
        stcMDNSServiceTxt*  pPred = m_pTxts;
        while ((pPred) &&
               (pPred->m_pNext != p_pTxt)) {
            pPred = pPred->m_pNext;
        }
        if (pPred) {
            pPred->m_pNext = p_pTxt->m_pNext;
            delete p_pTxt;
            bResult = true;
        }
        else if (m_pTxts == p_pTxt) {   // No predecesor, but first item
            m_pTxts = p_pTxt->m_pNext;
            delete p_pTxt;
            bResult = true;
        }
    }
    return bResult;
}

/*
 * MDNSResponder::_stcMDNSServiceTxts::removeTempTxts
 */
bool MDNSResponder::_stcMDNSServiceTxts::removeTempTxts(void) {

    bool    bResult = true;
    
    stcMDNSServiceTxt*  pTxt = m_pTxts;
    while ((bResult) &&
           (pTxt)) {
        stcMDNSServiceTxt*  pNext = pTxt->m_pNext;
        if (pTxt->m_bTemp) {
            bResult = remove(pTxt);
        }
        pTxt = pNext;
    }
    return bResult;
}

/*
 * MDNSResponder::_stcMDNSServiceTxts::find
 */
MDNSResponder::stcMDNSServiceTxt* MDNSResponder::_stcMDNSServiceTxts::find(const char* p_pcKey) {
    
    _stcMDNSServiceTxt* pResult = 0;
    
    for (stcMDNSServiceTxt* pTxt=m_pTxts; pTxt; pTxt=pTxt->m_pNext) {
        if ((p_pcKey) &&
            (0 == strcmp(pTxt->m_pcKey, p_pcKey))) {
            pResult = pTxt;
            break;
        }
    }
    return pResult;
}

/*
 * MDNSResponder::_stcMDNSServiceTxts::find
 */
const MDNSResponder::stcMDNSServiceTxt* MDNSResponder::_stcMDNSServiceTxts::find(const char* p_pcKey) const {
    
    const _stcMDNSServiceTxt*   pResult = 0;
    
    for (const stcMDNSServiceTxt* pTxt=m_pTxts; pTxt; pTxt=pTxt->m_pNext) {
        if ((p_pcKey) &&
            (0 == strcmp(pTxt->m_pcKey, p_pcKey))) {
            
            pResult = pTxt;
            break;
        }
    }
    return pResult;
}

/*
 * MDNSResponder::_stcMDNSServiceTxts::find
 */
MDNSResponder::stcMDNSServiceTxt* MDNSResponder::_stcMDNSServiceTxts::find(const stcMDNSServiceTxt* p_pTxt) {
    
    _stcMDNSServiceTxt* pResult = 0;
    
    for (stcMDNSServiceTxt* pTxt=m_pTxts; pTxt; pTxt=pTxt->m_pNext) {
        if (p_pTxt == pTxt) {
            pResult = pTxt;
            break;
        }
    }
    return pResult;
}

/*
 * MDNSResponder::_stcMDNSServiceTxts::length
 */
uint16_t MDNSResponder::_stcMDNSServiceTxts::length(void) const {
    
    uint16_t    u16Length = 0;

    stcMDNSServiceTxt*  pTxt = m_pTxts;
    while (pTxt) {
        u16Length += 1;                 // Length byte
        u16Length += pTxt->length();    // Text
        pTxt = pTxt->m_pNext;
    }
    return u16Length;
}

/*
 * MDNSResponder::_stcMDNSServiceTxts::c_strLength
 *
 * (incl. closing '\0'). Length bytes place is used for delimiting ';' and closing '\0'
 */
size_t MDNSResponder::_stcMDNSServiceTxts::c_strLength(void) const {
    
    return length();
}

/*
 * MDNSResponder::_stcMDNSServiceTxts::c_str
 */
bool MDNSResponder::_stcMDNSServiceTxts::c_str(char* p_pcBuffer) {
    
    bool bResult = false;
    
    if (p_pcBuffer) {
        bResult = true;
        
        *p_pcBuffer = 0;
        for (stcMDNSServiceTxt* pTxt=m_pTxts; ((bResult) && (pTxt)); pTxt = pTxt->m_pNext) {
            size_t  stLength;
            if ((bResult = (0 != (stLength = (pTxt->m_pcKey ? os_strlen(pTxt->m_pcKey) : 0))))) {
                if (pTxt != m_pTxts) {
                    *p_pcBuffer++ = ';';
                }
                os_strncpy(p_pcBuffer, pTxt->m_pcKey, stLength); p_pcBuffer[stLength] = 0;
                p_pcBuffer += stLength;
                *p_pcBuffer++ = '=';
                if ((stLength = (pTxt->m_pcValue ? os_strlen(pTxt->m_pcValue) : 0))) {
                    os_strncpy(p_pcBuffer, pTxt->m_pcValue, stLength); p_pcBuffer[stLength] = 0;
                    p_pcBuffer += stLength;
                }
            }
        }
        *p_pcBuffer++ = 0;
    }
    return bResult;
}

/*
 * MDNSResponder::_stcMDNSServiceTxts::bufferLength
 *
 * (incl. closing '\0').
 */
size_t MDNSResponder::_stcMDNSServiceTxts::bufferLength(void) const {
    
    return (length() + 1);
}

/*
 * MDNSResponder::_stcMDNSServiceTxts::toBuffer
 */
bool MDNSResponder::_stcMDNSServiceTxts::buffer(char* p_pcBuffer) {
    
    bool bResult = false;

    if (p_pcBuffer) {
        bResult = true;
        
        *p_pcBuffer = 0;
        for (stcMDNSServiceTxt* pTxt=m_pTxts; ((bResult) && (pTxt)); pTxt = pTxt->m_pNext) {
            *(unsigned char*)p_pcBuffer++ = pTxt->length();
            size_t  stLength;
            if ((bResult = (0 != (stLength = (pTxt->m_pcKey ? os_strlen(pTxt->m_pcKey) : 0))))) {
                os_memcpy(p_pcBuffer, pTxt->m_pcKey, stLength);
                p_pcBuffer += stLength;
                *p_pcBuffer++ = '=';
                if ((stLength = (pTxt->m_pcValue ? os_strlen(pTxt->m_pcValue) : 0))) {
                    os_memcpy(p_pcBuffer, pTxt->m_pcValue, stLength);
                    p_pcBuffer += stLength;
                }
            }
        }
        *p_pcBuffer++ = 0;
    }
    return bResult;
}

/*
 * MDNSResponder::_stcMDNSServiceTxts::compare
 */
bool MDNSResponder::_stcMDNSServiceTxts::compare(const MDNSResponder::_stcMDNSServiceTxts& p_Other) const {

    bool    bResult = false;

    if ((bResult = (length() == p_Other.length()))) {
        // Compare A->B
        for (const stcMDNSServiceTxt* pTxt=m_pTxts; ((bResult) && (pTxt)); pTxt=pTxt->m_pNext) {
            const stcMDNSServiceTxt*    pOtherTxt = p_Other.find(pTxt->m_pcKey);
            bResult = ((pOtherTxt) &&
                       (pTxt->m_pcValue) &&
                       (pOtherTxt->m_pcValue) &&
                       (os_strlen(pTxt->m_pcValue) == os_strlen(pOtherTxt->m_pcValue)) &&
                       (0 == strcmp(pTxt->m_pcValue, pOtherTxt->m_pcValue)));
        }
        // Compare B->A
        for (const stcMDNSServiceTxt* pOtherTxt=p_Other.m_pTxts; ((bResult) && (pOtherTxt)); pOtherTxt=pOtherTxt->m_pNext) {
            const stcMDNSServiceTxt*    pTxt = find(pOtherTxt->m_pcKey);
            bResult = ((pTxt) &&
                       (pOtherTxt->m_pcValue) &&
                       (pTxt->m_pcValue) &&
                       (os_strlen(pOtherTxt->m_pcValue) == os_strlen(pTxt->m_pcValue)) &&
                       (0 == strcmp(pOtherTxt->m_pcValue, pTxt->m_pcValue)));
        }
    }
    return bResult;
}

/*
 * MDNSResponder::_stcMDNSServiceTxts::operator==
 */
bool MDNSResponder::_stcMDNSServiceTxts::operator==(const _stcMDNSServiceTxts& p_Other) const {
    
    return compare(p_Other);
}

/*
 * MDNSResponder::_stcMDNSServiceTxts::operator!=
 */
bool MDNSResponder::_stcMDNSServiceTxts::operator!=(const _stcMDNSServiceTxts& p_Other) const {
    
    return !compare(p_Other);
}


/**
 * MDNSResponder::stcMDNS_MsgHeader
 *
 * A MDNS message haeder.
 *
 */

/*
 * MDNSResponder::_stcMDNS_MsgHeader::_stcMDNS_MsgHeader
 */
MDNSResponder::_stcMDNS_MsgHeader::_stcMDNS_MsgHeader(uint16_t p_u16ID /*= 0*/,
                                                      bool p_bQR /*= false*/,
                                                      unsigned char p_ucOpcode /*= 0*/,
                                                      bool p_bAA /*= false*/,
                                                      bool p_bTC /*= false*/,
                                                      bool p_bRD /*= false*/,
                                                      bool p_bRA /*= false*/,
                                                      unsigned char p_ucRCode /*= 0*/,
                                                      uint16_t p_u16QDCount /*= 0*/,
                                                      uint16_t p_u16ANCount /*= 0*/,
                                                      uint16_t p_u16NSCount /*= 0*/,
                                                      uint16_t p_u16ARCount /*= 0*/)
:   m_u16ID(p_u16ID),
    m_1bQR(p_bQR), m_4bOpcode(p_ucOpcode), m_1bAA(p_bAA), m_1bTC(p_bTC), m_1bRD(p_bRD),
    m_1bRA(p_bRA), m_3bZ(0), m_4bRCode(p_ucRCode),
    m_u16QDCount(p_u16QDCount),
    m_u16ANCount(p_u16ANCount),
    m_u16NSCount(p_u16NSCount),
    m_u16ARCount(p_u16ARCount) {

}


/**
 * MDNSResponder::stcMDNS_RRDomain
 *
 * A MDNS domain object.
 * The labels of the domain are stored (DNS-like encoded) in 'm_acName':
 * [length byte]varlength label[length byte]varlength label[0]
 * 'm_u16NameLength' stores the used length of 'm_acName'.
 * Dynamic label addition is supported.
 * Comparison is supported.
 * Export as byte array 'esp8266.local' is supported.
 *
 */

/*
 * MDNSResponder::_stcMDNS_RRDomain::_stcMDNS_RRDomain constructor
 */
MDNSResponder::_stcMDNS_RRDomain::_stcMDNS_RRDomain(void)
:   m_u16NameLength(0) {
    
    clear();
}

/*
 * MDNSResponder::_stcMDNS_RRDomain::_stcMDNS_RRDomain copy-constructor
 */
MDNSResponder::_stcMDNS_RRDomain::_stcMDNS_RRDomain(const _stcMDNS_RRDomain& p_Other)
:   m_u16NameLength(0) {
    
    operator=(p_Other);
}

/*
 * MDNSResponder::_stcMDNS_RRDomain::operator =
 */
MDNSResponder::_stcMDNS_RRDomain& MDNSResponder::_stcMDNS_RRDomain::operator=(const _stcMDNS_RRDomain& p_Other) {
    
    if (&p_Other != this) {
        memcpy(m_acName, p_Other.m_acName, sizeof(m_acName));
        m_u16NameLength = p_Other.m_u16NameLength;
    }
    return *this;
}

/*
 * MDNSResponder::_stcMDNS_RRDomain::clear
 */
bool MDNSResponder::_stcMDNS_RRDomain::clear(void) {
    
    memset(m_acName, 0, sizeof(m_acName));
    m_u16NameLength = 0;
    return true;
}

/*
 * MDNSResponder::_stcMDNS_RRDomain::addLabel
 */
bool MDNSResponder::_stcMDNS_RRDomain::addLabel(const char* p_pcLabel,
                                                bool p_bPrependUnderline /*= false*/) {
    
    bool    bResult = false;    
    
    size_t  stLength = (p_pcLabel
                        ? (os_strlen(p_pcLabel) + (p_bPrependUnderline ? 1 : 0))
                        : 0);
    if ((MDNS_DOMAIN_LABEL_MAXLENGTH >= stLength) &&
        (MDNS_DOMAIN_MAXLENGTH >= (m_u16NameLength + (1 + stLength)))) {
        // Length byte
        m_acName[m_u16NameLength] = (unsigned char)stLength;    // Might be 0!
        ++m_u16NameLength;
        // Label
        if (stLength) {
            if (p_bPrependUnderline) {
                m_acName[m_u16NameLength++] = '_';
                --stLength;
            }
            os_strncpy(&(m_acName[m_u16NameLength]), p_pcLabel, stLength); m_acName[m_u16NameLength + stLength] = 0;
            m_u16NameLength += stLength;
        }
        bResult = true;
    }
    return bResult;
}

/*
 * MDNSResponder::_stcMDNS_RRDomain::compare
 */
bool MDNSResponder::_stcMDNS_RRDomain::compare(const _stcMDNS_RRDomain& p_Other) const {
    
    bool    bResult = false;
    
    if (m_u16NameLength == p_Other.m_u16NameLength) {
        const char* pT = m_acName;
        const char* pO = p_Other.m_acName;
        while ((pT) &&
               (pO) &&
               (*((unsigned char*)pT) == *((unsigned char*)pO)) &&                  // Same length AND
               (0 == strncasecmp((pT + 1), (pO + 1), *((unsigned char*)pT)))) {   // Same content
            if (*((unsigned char*)pT)) {            // Not 0
                pT += (1 + *((unsigned char*)pT));  // Shift by length byte and lenght
                pO += (1 + *((unsigned char*)pO));
            }
            else {                                  // Is 0 -> Successfully reached the end
                bResult = true;
                break;
            }
        }
    }
    return bResult;
}

/*
 * MDNSResponder::_stcMDNS_RRDomain::operator ==
 */
bool MDNSResponder::_stcMDNS_RRDomain::operator==(const _stcMDNS_RRDomain& p_Other) const {
    
    return compare(p_Other);
}

/*
 * MDNSResponder::_stcMDNS_RRDomain::operator !=
 */
bool MDNSResponder::_stcMDNS_RRDomain::operator!=(const _stcMDNS_RRDomain& p_Other) const {
    
    return !compare(p_Other);
}

/*
 * MDNSResponder::_stcMDNS_RRDomain::operator >
 */
bool MDNSResponder::_stcMDNS_RRDomain::operator>(const _stcMDNS_RRDomain& p_Other) const {

    // TODO: Check, if this is a good idea...
    return !compare(p_Other);
}

/*
 * MDNSResponder::_stcMDNS_RRDomain::c_strLength
 */
size_t MDNSResponder::_stcMDNS_RRDomain::c_strLength(void) const {
    
    size_t          stLength = 0;
    
    unsigned char*  pucLabelLength = (unsigned char*)m_acName;
    while (*pucLabelLength) {
        stLength += (*pucLabelLength + 1 /* +1 for '.' or '\0'*/);
        pucLabelLength += (*pucLabelLength + 1);
    }
    return stLength;
}

/*
 * MDNSResponder::_stcMDNS_RRDomain::c_str
 */
bool MDNSResponder::_stcMDNS_RRDomain::c_str(char* p_pcBuffer) {
    
    bool bResult = false;
    
    if (p_pcBuffer) {
        *p_pcBuffer = 0;
        unsigned char* pucLabelLength = (unsigned char*)m_acName;
        while (*pucLabelLength) {
            memcpy(p_pcBuffer, (const char*)(pucLabelLength + 1), *pucLabelLength);
            p_pcBuffer += *pucLabelLength;
            pucLabelLength += (*pucLabelLength + 1);
            *p_pcBuffer++ = (*pucLabelLength ? '.' : '\0');
        }
        bResult = true;
    }
    return bResult;
}


/**
 * MDNSResponder::stcMDNS_RRAttributes
 *
 * A MDNS attributes object.
 *
 */

/*
 * MDNSResponder::_stcMDNS_RRAttributes::_stcMDNS_RRAttributes constructor
 */
MDNSResponder::_stcMDNS_RRAttributes::_stcMDNS_RRAttributes(uint16_t p_u16Type /*= 0*/,
                                                            uint16_t p_u16Class /*= 1 DNS_RRCLASS_IN Internet*/)
:   m_u16Type(p_u16Type),
    m_u16Class(p_u16Class) {
    
}

/*
 * MDNSResponder::_stcMDNS_RRAttributes::_stcMDNS_RRAttributes copy-constructor
 */
MDNSResponder::_stcMDNS_RRAttributes::_stcMDNS_RRAttributes(const MDNSResponder::_stcMDNS_RRAttributes& p_Other) {
    
    operator=(p_Other);
}

/*
 * MDNSResponder::_stcMDNS_RRAttributes::operator =
 */
MDNSResponder::_stcMDNS_RRAttributes& MDNSResponder::_stcMDNS_RRAttributes::operator=(const MDNSResponder::_stcMDNS_RRAttributes& p_Other) {
    
    if (&p_Other != this) {
        m_u16Type = p_Other.m_u16Type;
        m_u16Class = p_Other.m_u16Class;
    }
    return *this;
}


/**
 * MDNSResponder::stcMDNS_RRHeader
 *
 * A MDNS record header (domain and attributes) object.
 *
 */

/*
 * MDNSResponder::_stcMDNS_RRHeader::_stcMDNS_RRHeader constructor
 */
MDNSResponder::_stcMDNS_RRHeader::_stcMDNS_RRHeader(void) {
    
}

/*
 * MDNSResponder::_stcMDNS_RRHeader::_stcMDNS_RRHeader copy-constructor
 */
MDNSResponder::_stcMDNS_RRHeader::_stcMDNS_RRHeader(const _stcMDNS_RRHeader& p_Other) {
    
    operator=(p_Other);
}

/*
 * MDNSResponder::_stcMDNS_RRHeader::operator =
 */
MDNSResponder::_stcMDNS_RRHeader& MDNSResponder::_stcMDNS_RRHeader::operator=(const MDNSResponder::_stcMDNS_RRHeader& p_Other) {
    
    if (&p_Other != this) {
        m_Domain = p_Other.m_Domain;
        m_Attributes = p_Other.m_Attributes;
    }
    return *this;
}

/*
 * MDNSResponder::_stcMDNS_RRHeader::clear
 */
bool MDNSResponder::_stcMDNS_RRHeader::clear(void) {
    
    m_Domain.clear();
    return true;
}


/**
 * MDNSResponder::stcMDNS_RRQuestion
 *
 * A MDNS question record object (header + question flags)
 *
 */
 
/*
 * MDNSResponder::_stcMDNS_RRQuestion::_stcMDNS_RRQuestion constructor
 */
MDNSResponder::_stcMDNS_RRQuestion::_stcMDNS_RRQuestion(void)
:   m_pNext(0),
    m_bUnicast(false) {
    
}


/**
 * MDNSResponder::stcMDNS_RRAnswer
 *
 * A MDNS answer record object (header + answer content).
 * This is a 'virtual' base class for all other MDNS answer classes.
 *
 */

/*
 * MDNSResponder::_stcMDNS_RRAnswer::_stcMDNS_RRAnswer constructor
 */
MDNSResponder::_stcMDNS_RRAnswer::_stcMDNS_RRAnswer(enuAnswerType p_AnswerType,
                                                    const MDNSResponder::stcMDNS_RRHeader& p_Header,
                                                    uint32_t p_u32TTL)
:   m_pNext(0),
    m_AnswerType(p_AnswerType),
    m_Header(p_Header),
    m_u32TTL(p_u32TTL) {

    // Extract 'cache flush'-bit
    m_bCacheFlush = (m_Header.m_Attributes.m_u16Class & 0x8000);
    m_Header.m_Attributes.m_u16Class &= (~0x8000);
}

/*
 * MDNSResponder::_stcMDNS_RRAnswer::~_stcMDNS_RRAnswer destructor
 */
MDNSResponder::_stcMDNS_RRAnswer::~_stcMDNS_RRAnswer(void) {
    
}

/*
 * MDNSResponder::_stcMDNS_RRAnswer::answerType
 */
MDNSResponder::enuAnswerType MDNSResponder::_stcMDNS_RRAnswer::answerType(void) const {
    
    return m_AnswerType;
}

/*
 * MDNSResponder::_stcMDNS_RRAnswer::clear
 */
bool MDNSResponder::_stcMDNS_RRAnswer::clear(void) {
    
    m_pNext = 0;
    m_Header.clear(); 
    return true;
}


/**
 * MDNSResponder::stcMDNS_RRAnswerA
 *
 * A MDNS A answer object.
 * Extends the base class by an IP4 address member.
 *
 */

#ifdef MDNS_IP4_SUPPORT
    /*
     * MDNSResponder::_stcMDNS_RRAnswerA::stcMDNS_RRAnswerA constructor
     */
    MDNSResponder::_stcMDNS_RRAnswerA::_stcMDNS_RRAnswerA(const MDNSResponder::stcMDNS_RRHeader& p_Header,
                                                          uint32_t p_u32TTL)
    :   stcMDNS_RRAnswer(AnswerType_A, p_Header, p_u32TTL),
        m_IPAddress(0, 0, 0, 0) {
        
    }

    /*
     * MDNSResponder::_stcMDNS_RRAnswerA::_stcMDNS_RRAnswerA destructor
     */
    MDNSResponder::_stcMDNS_RRAnswerA::~_stcMDNS_RRAnswerA(void) {
        
        clear();
    }

    /*
     * MDNSResponder::_stcMDNS_RRAnswerA::clear
     */
    bool MDNSResponder::_stcMDNS_RRAnswerA::clear(void) {
        
        m_IPAddress = IPAddress(0, 0, 0, 0);
        return true;
    }
#endif


/**
 * MDNSResponder::stcMDNS_RRAnswerPTR
 *
 * A MDNS PTR answer object.
 * Extends the base class by a MDNS domain member.
 *
 */
    
/*
 * MDNSResponder::_stcMDNS_RRAnswerPTR::_stcMDNS_RRAnswerPTR constructor
 */
MDNSResponder::_stcMDNS_RRAnswerPTR::_stcMDNS_RRAnswerPTR(const MDNSResponder::stcMDNS_RRHeader& p_Header,
                                                          uint32_t p_u32TTL)
:   stcMDNS_RRAnswer(AnswerType_PTR, p_Header, p_u32TTL) {

}

/*
 * MDNSResponder::_stcMDNS_RRAnswerPTR::~_stcMDNS_RRAnswerPTR destructor
 */
MDNSResponder::_stcMDNS_RRAnswerPTR::~_stcMDNS_RRAnswerPTR(void) {
    
    clear();
}

/*
 * MDNSResponder::_stcMDNS_RRAnswerPTR::clear
 */
bool MDNSResponder::_stcMDNS_RRAnswerPTR::clear(void) {
    
    m_PTRDomain.clear();
    return true;
}


/**
 * MDNSResponder::_stcMDNS_RRAnswerTXT
 *
 * A MDNS TXT answer object.
 * Extends the base class by a MDNS TXT items list member.
 *
 */
    
/*
 * MDNSResponder::_stcMDNS_RRAnswerTXT::_stcMDNS_RRAnswerTXT constructor
 */
MDNSResponder::_stcMDNS_RRAnswerTXT::_stcMDNS_RRAnswerTXT(const MDNSResponder::stcMDNS_RRHeader& p_Header,
                                                          uint32_t p_u32TTL)
:   stcMDNS_RRAnswer(AnswerType_TXT, p_Header, p_u32TTL) {
    
}
    
/*
 * MDNSResponder::_stcMDNS_RRAnswerTXT::~_stcMDNS_RRAnswerTXT destructor
 */
MDNSResponder::_stcMDNS_RRAnswerTXT::~_stcMDNS_RRAnswerTXT(void) {
    
    clear();
}
    
/*
 * MDNSResponder::_stcMDNS_RRAnswerTXT::clear
 */
bool MDNSResponder::_stcMDNS_RRAnswerTXT::clear(void) {
    
    m_Txts.clear();
    return true;
}


/**
 * MDNSResponder::stcMDNS_RRAnswerAAAA
 *
 * A MDNS AAAA answer object.
 * (Should) extend the base class by an IP6 address member.
 *
 */

#ifdef MDNS_IP6_SUPPORT
    /*
     * MDNSResponder::_stcMDNS_RRAnswerAAAA::_stcMDNS_RRAnswerAAAA constructor
     */
    MDNSResponder::_stcMDNS_RRAnswerAAAA::_stcMDNS_RRAnswerAAAA(const MDNSResponder::stcMDNS_RRHeader& p_Header,
                                                                uint32_t p_u32TTL)
    :   stcMDNS_RRAnswer(AnswerType_AAAA, p_Header, p_u32TTL) {
        
    }

    /*
     * MDNSResponder::_stcMDNS_RRAnswerAAAA::~_stcMDNS_RRAnswerAAAA destructor
     */
    MDNSResponder::_stcMDNS_RRAnswerAAAA::~_stcMDNS_RRAnswerAAAA(void) {
        
        clear();
    }

    /*
     * MDNSResponder::_stcMDNS_RRAnswerAAAA::clear
     */
    bool MDNSResponder::_stcMDNS_RRAnswerAAAA::clear(void) {
        
        return true;
    }
#endif


/**
 * MDNSResponder::stcMDNS_RRAnswerSRV
 *
 * A MDNS SRV answer object.
 * Extends the base class by a port member.
 *
 */

/*
 * MDNSResponder::_stcMDNS_RRAnswerSRV::_stcMDNS_RRAnswerSRV constructor
 */
MDNSResponder::_stcMDNS_RRAnswerSRV::_stcMDNS_RRAnswerSRV(const MDNSResponder::stcMDNS_RRHeader& p_Header,
                                                          uint32_t p_u32TTL)
:   stcMDNS_RRAnswer(AnswerType_SRV, p_Header, p_u32TTL),
    m_u16Priority(0),
    m_u16Weight(0),
    m_u16Port(0) {
    
}

/*
 * MDNSResponder::_stcMDNS_RRAnswerSRV::~_stcMDNS_RRAnswerSRV destructor
 */
MDNSResponder::_stcMDNS_RRAnswerSRV::~_stcMDNS_RRAnswerSRV(void) {
    
    clear();
}

/*
 * MDNSResponder::_stcMDNS_RRAnswerSRV::clear
 */
bool MDNSResponder::_stcMDNS_RRAnswerSRV::clear(void) {
    
    m_u16Priority = 0;
    m_u16Weight = 0;
    m_u16Port = 0;
    m_SRVDomain.clear();
    return true;
}


/**
 * MDNSResponder::stcMDNS_RRAnswerGeneric
 *
 * An unknown (generic) MDNS answer object.
 * Extends the base class by a RDATA buffer member.
 *
 */

/*
 * MDNSResponder::_stcMDNS_RRAnswerGeneric::_stcMDNS_RRAnswerGeneric constructor
 */
MDNSResponder::_stcMDNS_RRAnswerGeneric::_stcMDNS_RRAnswerGeneric(const stcMDNS_RRHeader& p_Header,
                                                                  uint32_t p_u32TTL)
:   stcMDNS_RRAnswer(AnswerType_Generic, p_Header, p_u32TTL),
    m_u16RDLength(0),
    m_pu8RDData(0) {
    
}

/*
 * MDNSResponder::_stcMDNS_RRAnswerGeneric::~_stcMDNS_RRAnswerGeneric destructor
 */
MDNSResponder::_stcMDNS_RRAnswerGeneric::~_stcMDNS_RRAnswerGeneric(void) {
    
    clear();
}

/*
 * MDNSResponder::_stcMDNS_RRAnswerGeneric::clear
 */
bool MDNSResponder::_stcMDNS_RRAnswerGeneric::clear(void) {
    
    if (m_pu8RDData) {
        delete[] m_pu8RDData;
        m_pu8RDData = 0;
    }
    m_u16RDLength = 0;
    
    return true;
}


/**
 * MDNSResponder::stcProbeInformation
 *
 * Probing status information for a host or service domain
 *
 */

/*
 * MDNSResponder::_stcProbeInformation::_stcProbeInformation constructor
 */
MDNSResponder::_stcProbeInformation::_stcProbeInformation(void)
:   m_ProbingStatus(ProbingStatus_WaitingForData),
    m_u8ProbesSent(0),
    //m_ulNextProbeTimeout(0),
    m_NextProbeTimeFlag(),
    m_bConflict(false),
    m_bTiebreakNeeded(false),
    m_fnProbeResultCallback(0),
    m_pProbeResultCallbackUserdata(0) {
}

/*
 * MDNSResponder::_stcProbeInformation::clear
 */
bool MDNSResponder::_stcProbeInformation::clear(bool p_bClearUserdata /*= false*/) {

    m_ProbingStatus = ProbingStatus_WaitingForData;
    m_u8ProbesSent = 0;
    //m_ulNextProbeTimeout = 0;
    m_NextProbeTimeFlag.reset();
    m_bConflict = false;
    m_bTiebreakNeeded = false;
    if (p_bClearUserdata) {
        m_fnProbeResultCallback = 0;
        m_pProbeResultCallbackUserdata = 0;
    }
    return true;
}

/**
 * MDNSResponder::stcMDNSService
 *
 * A MDNS service object (to be announced by the MDNS responder)
 * The service instance may be '\0'; in this case the hostname is used
 * and the flag m_bAutoName is set. If the hostname changes, all 'auto-
 * named' services are renamed also.
 * m_u8Replymask is used while preparing a response to a MDNS query. It is
 * resetted in '_sendMDNSMessage' afterwards.
 */

/*
 * MDNSResponder::_stcMDNSService::_stcMDNSService constructor
 */
MDNSResponder::_stcMDNSService::_stcMDNSService(const char* p_pcName /*= 0*/,
                                                const char* p_pcService /*= 0*/,
                                                const char* p_pcProtocol /*= 0*/)
:   m_pNext(0),
    m_pcName(0),
    m_bAutoName(false),
    m_pcService(0),
    m_pcProtocol(0),
    m_u16Port(0),
    m_u8ReplyMask(0),
    m_fnTxtCallback(0),
    m_pTxtCallbackUserdata(0) {
    
    setName(p_pcName);
    setService(p_pcService);
    setProtocol(p_pcProtocol);
}

/*
 * MDNSResponder::_stcMDNSService::~_stcMDNSService destructor
 */
MDNSResponder::_stcMDNSService::~_stcMDNSService(void) {
    
    releaseName();
    releaseService();
    releaseProtocol();
}

/*
 * MDNSResponder::_stcMDNSService::setName
 */
bool MDNSResponder::_stcMDNSService::setName(const char* p_pcName) {
    
    bool bResult = false;
    
    releaseName();
    size_t stLength = (p_pcName ? os_strlen(p_pcName) : 0);
    if (stLength) {
        if ((bResult = (0 != (m_pcName = new char[stLength + 1])))) {
            os_strncpy(m_pcName, p_pcName, stLength);
            m_pcName[stLength] = 0;
        }
    }
    else {
        bResult = true;
    }
    return bResult;
}

/*
 * MDNSResponder::_stcMDNSService::releaseName
 */
bool MDNSResponder::_stcMDNSService::releaseName(void) {
    
    if (m_pcName) {
        delete[] m_pcName;
        m_pcName = 0;
    }
    return true;
}

/*
 * MDNSResponder::_stcMDNSService::setService
 */
bool MDNSResponder::_stcMDNSService::setService(const char* p_pcService) {
    
    bool bResult = false;
    
    releaseService();
    size_t stLength = (p_pcService ? os_strlen(p_pcService) : 0);
    if (stLength) {
        if ((bResult = (0 != (m_pcService = new char[stLength + 1])))) {
            os_strncpy(m_pcService, p_pcService, stLength);
            m_pcService[stLength] = 0;
        }
    }
    else {
        bResult = true;
    }
    return bResult;
}

/*
 * MDNSResponder::_stcMDNSService::releaseService
 */
bool MDNSResponder::_stcMDNSService::releaseService(void) {

    if (m_pcService) {
        delete[] m_pcService;
        m_pcService = 0;
    }
    return true;
}

/*
 * MDNSResponder::_stcMDNSService::setProtocol
 */
bool MDNSResponder::_stcMDNSService::setProtocol(const char* p_pcProtocol) {

    bool bResult = false;
    
    releaseProtocol();
    size_t stLength = (p_pcProtocol ? os_strlen(p_pcProtocol) : 0);
    if (stLength) {
        if ((bResult = (0 != (m_pcProtocol = new char[stLength + 1])))) {
            os_strncpy(m_pcProtocol, p_pcProtocol, stLength);
            m_pcProtocol[stLength] = 0;
        }
    }
    else {
        bResult = true;
    }
    return bResult;
}

/*
 * MDNSResponder::_stcMDNSService::releaseProtocol
 */
bool MDNSResponder::_stcMDNSService::releaseProtocol(void) {

    if (m_pcProtocol) {
        delete[] m_pcProtocol;
        m_pcProtocol = 0;
    }
    return true;
}


/**
 * MDNSResponder::stcMDNSServiceQuery
 *
 * A MDNS service query object.
 * Service queries may be static or dynamic.
 * As the static service query is processed in the blocking function 'queryService',
 * only one static service service may exist. The processing of the answers is done
 * on the WiFi-stack side of the ESP stack structure (via 'UDPContext.onRx(_update)').
 *
 */

/**
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer
 *
 * One answer for a service query.
 * Every answer must contain
 * - a service instance entry (pivot),
 * and may contain
 * - a host domain,
 * - a port
 * - an IP4 address
 * (- an IP6 address)
 * - a MDNS TXTs
 * The existance of a component is flaged in 'm_u32ContentFlags'.
 * For every answer component a TTL value is maintained.
 * Answer objects can be connected to a linked list.
 *
 * For the host domain, service domain and TXTs components, a char array
 * representation can be retrieved (which is created on demand).
 *
 */

/**
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL
 *
 * The TTL (Time-To-Live) for an specific answer content.
 * The 80% and outdated states are calculated based on the current time (millis)
 * and the 'set' time (also millis).
 * If the answer is scheduled for an update, the corresponding flag should be set.
 *
 */

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcTTL::_stcTTL constructor
 */
MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcTTL::_stcTTL(uint32_t p_u32TTL /*= 0*/)
:   m_bUpdateScheduled(false) {
    
    set(p_u32TTL * 1000);
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcTTL::set
 */
bool MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcTTL::set(uint32_t p_u32TTL) {
    
    m_TTLTimeFlag.restart(p_u32TTL * 1000);
    m_bUpdateScheduled = false;

    return true;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcTTL::has80Percent
 */
bool MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcTTL::has80Percent(void) const {
    
    return ((m_TTLTimeFlag.timeout()) &&
            (!m_bUpdateScheduled) &&
            (m_TTLTimeFlag.hypotheticalTimeout((m_TTLTimeFlag.timeout() * 800) / 1000)));
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcTTL::isOutdated
 */
bool MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcTTL::isOutdated(void) const {
    
    return ((m_TTLTimeFlag.timeout()) &&
            (m_TTLTimeFlag.flagged()));
}


#ifdef MDNS_IP4_SUPPORT
/**
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcIP4Address
 *
 */

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcIP4Address::_stcIP4Address constructor
 */
MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcIP4Address::_stcIP4Address(IPAddress p_IPAddress,
                                                                                uint32_t p_u32TTL /*= 0*/)
:   m_pNext(0),
    m_IPAddress(p_IPAddress),
    m_TTL(p_u32TTL) {
}
#endif


/**
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer
 */

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcAnswer constructor
 */
MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcAnswer(void)
:   m_pNext(0),
    m_pcServiceDomain(0),
    m_pcHostDomain(0),
    m_u16Port(0),
    m_pcTxts(0),
#ifdef MDNS_IP4_SUPPORT
    m_pIP4Addresses(0),
#endif
#ifdef MDNS_IP6_SUPPORT
    m_pIP6Addresses(0),
#endif
    m_u32ContentFlags(0) {
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::~_stcAnswer destructor
 */
MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::~_stcAnswer(void) {
    
    clear();
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::clear
 */
bool MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::clear(void) {
    
    return ((releaseTxts()) &&
#ifdef MDNS_IP4_SUPPORT
            (releaseIP4Addresses()) &&
#endif
#ifdef MDNS_IP6_SUPPORT
            (releaseIP6Addresses())
#endif
            (releaseHostDomain()) &&
            (releaseServiceDomain()));
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::allocServiceDomain
 *
 * Alloc memory for the char array representation of the service domain.
 *
 */
char* MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::allocServiceDomain(size_t p_stLength) {
    
    releaseServiceDomain();
    if (p_stLength) {
        m_pcServiceDomain = new char[p_stLength];
    }
    return m_pcServiceDomain;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::releaseServiceDomain
 */
bool MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::releaseServiceDomain(void) {

    if (m_pcServiceDomain) {
        delete[] m_pcServiceDomain;
        m_pcServiceDomain = 0;
    }
    return true;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::allocHostDomain
 *
 * Alloc memory for the char array representation of the host domain.
 *
 */
char* MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::allocHostDomain(size_t p_stLength) {

    releaseHostDomain();
    if (p_stLength) {
        m_pcHostDomain = new char[p_stLength];
    }
    return m_pcHostDomain;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::releaseHostDomain
 */
bool MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::releaseHostDomain(void) {

    if (m_pcHostDomain) {
        delete[] m_pcHostDomain;
        m_pcHostDomain = 0;
    }
    return true;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::allocTxts
 *
 * Alloc memory for the char array representation of the TXT items.
 *
 */
char* MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::allocTxts(size_t p_stLength) {

    releaseTxts();
    if (p_stLength) {
        m_pcTxts = new char[p_stLength];
    }
    return m_pcTxts;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::releaseTxts
 */
bool MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::releaseTxts(void) {
    
    if (m_pcTxts) {
        delete[] m_pcTxts;
        m_pcTxts = 0;
    }
    return true;
}

#ifdef MDNS_IP4_SUPPORT
/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::releaseIP4Addresses
 */
bool MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::releaseIP4Addresses(void) {
    
    while (m_pIP4Addresses) {
        stcIP4Address*  pNext = m_pIP4Addresses->m_pNext;
        delete m_pIP4Addresses;
        m_pIP4Addresses = pNext;
    }
    return true;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::addIP4Address
 */
bool MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::addIP4Address(MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcIP4Address* p_pIP4Address) {
    
    bool bResult = false;
    
    if (p_pIP4Address) {
        p_pIP4Address->m_pNext = m_pIP4Addresses;
        m_pIP4Addresses = p_pIP4Address;
        bResult = true;
    }
    return bResult;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::removeIP4Address
 */
bool MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::removeIP4Address(MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcIP4Address* p_pIP4Address) {
    
    bool    bResult = false;
    
    if (p_pIP4Address) {
        stcIP4Address*  pPred = m_pIP4Addresses;
        while ((pPred) &&
               (pPred->m_pNext != p_pIP4Address)) {
            pPred = pPred->m_pNext;
        }
        if (pPred) {
            pPred->m_pNext = p_pIP4Address->m_pNext;
            delete p_pIP4Address;
            bResult = true;
        }
        else if (m_pIP4Addresses == p_pIP4Address) {   // No predecesor, but first item
            m_pIP4Addresses = p_pIP4Address->m_pNext;
            delete p_pIP4Address;
            bResult = true;
        }
    }
    return bResult;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::findIP4Address (const)
 */
const MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcIP4Address* MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::findIP4Address(const IPAddress& p_IPAddress) const {

    return (stcIP4Address*)(((const _stcAnswer*)this)->findIP4Address(p_IPAddress));
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::findIP4Address
 */
MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcIP4Address* MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::findIP4Address(const IPAddress& p_IPAddress) {
    
    stcIP4Address*  pIP4Address = m_pIP4Addresses;
    while (pIP4Address) {
        if (pIP4Address->m_IPAddress == p_IPAddress) {
            break;
        }
        pIP4Address = pIP4Address->m_pNext;
    }
    return pIP4Address;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::IP4AddressCount
 */
uint32_t MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::IP4AddressCount(void) const {
    
    uint32_t    u32Count = 0;
    
    stcIP4Address*  pIP4Address = m_pIP4Addresses;
    while (pIP4Address) {
        ++u32Count;
        pIP4Address = pIP4Address->m_pNext;
    }
    return u32Count;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::IP4AddressAtIndex
 */
MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcIP4Address* MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::IP4AddressAtIndex(uint32_t p_u32Index) {

    return (stcIP4Address*)(((const _stcAnswer*)this)->IP4AddressAtIndex(p_u32Index));
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::IP4AddressAtIndex (const)
 */
const MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcIP4Address* MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::IP4AddressAtIndex(uint32_t p_u32Index) const {
    
    const stcIP4Address*    pIP4Address = 0;
    
    if (((uint32_t)(-1) != p_u32Index) &&
        (m_pIP4Addresses)) {

        uint32_t    u32Index;
        for (pIP4Address=m_pIP4Addresses, u32Index=0; ((pIP4Address) && (u32Index<p_u32Index)); pIP4Address=pIP4Address->m_pNext, ++u32Index);
    }
    return pIP4Address;
}
#endif

#ifdef MDNS_IP6_SUPPORT
/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::releaseIP6Addresses
 */
bool MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::releaseIP6Addresses(void) {
    
    while (m_pIP6Addresses) {
        stcIP6Address*  pNext = m_pIP6Addresses->m_pNext;
        delete m_pIP6Addresses;
        m_pIP6Addresses = pNext;
    }
    return true;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::addIP6Address
 */
bool MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::addIP6Address(MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcIP6Address* p_pIP6Address) {
    
    bool bResult = false;
    
    if (p_pIP6Address) {
        p_pIP6Address->m_pNext = m_pIP6Addresses;
        m_pIP6Addresses = p_pIP6Address;
        bResult = true;
    }
    return bResult;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::removeIP6Address
 */
bool MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::removeIP6Address(MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcIP6Address* p_pIP6Address) {
    
    bool    bResult = false;
    
    if (p_pIP6Address) {
        stcIP6Address*  pPred = m_pIP6Addresses;
        while ((pPred) &&
               (pPred->m_pNext != p_pIP6Address)) {
            pPred = pPred->m_pNext;
        }
        if (pPred) {
            pPred->m_pNext = p_pIP6Address->m_pNext;
            delete p_pIP6Address;
            bResult = true;
        }
        else if (m_pIP6Addresses == p_pIP6Address) {   // No predecesor, but first item
            m_pIP6Addresses = p_pIP6Address->m_pNext;
            delete p_pIP6Address;
            bResult = true;
        }
    }
    return bResult;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::findIP6Address
 */
MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcIP6Address* MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::findIP6Address(const IP6Address& p_IPAddress) {

    return (stcIP6Address*)(((const _stcAnswer*)this)->findIP6Address(p_IPAddress));
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::findIP6Address (const)
 */
const MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcIP6Address* MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::findIP6Address(const IPAddress& p_IPAddress) const {
    
    const stcIP6Address*    pIP6Address = m_pIP6Addresses;
    while (pIP6Address) {
        if (p_IP6Address->m_IPAddress == p_IPAddress) {
            break;
        }
        pIP6Address = pIP6Address->m_pNext;
    }
    return pIP6Address;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::IP6AddressCount
 */
uint32_t MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::IP6AddressCount(void) const {
    
    uint32_t    u32Count = 0;
    
    stcIP6Address*  pIP6Address = m_pIP6Addresses;
    while (pIP6Address) {
        ++u32Count;
        pIP6Address = pIP6Address->m_pNext;
    }
    return u32Count;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::IP6AddressAtIndex (const)
 */
const MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcIP6Address* MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::IP6AddressAtIndex(uint32_t p_u32Index) const {

    return (stcIP6Address*)(((const _stcAnswer*)this)->IP6AddressAtIndex(p_u32Index));
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::IP6AddressAtIndex
 */
MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::_stcIP6Address* MDNSResponder::_stcMDNSServiceQuery::_stcAnswer::IP6AddressAtIndex(uint32_t p_u32Index) {
    
    stcIP6Address*    pIP6Address = 0;
    
    if (((uint32_t)(-1) != p_u32Index) &&
        (m_pIP6Addresses)) {

        uint32_t    u32Index;
        for (pIP6Address=m_pIP6Addresses, u32Index=0; ((pIP6Address) && (u32Index<p_u32Index)); pIP6Address=pIP6Address->m_pNext, ++u32Index);
    }
    return pIP6Address;
}
#endif


/**
 * MDNSResponder::stcMDNSServiceQuery
 *
 * A service query object.
 * A static query is flaged via 'm_bLegacyQuery'; while the function 'queryService'
 * is waiting for answers, the internal flag 'm_bAwaitingAnswers' is set. When the
 * timeout is reached, the flag is removed. These two flags are only used for static
 * service queries.
 * All answers to the service query are stored in 'm_pAnswers' list.
 * Individual answers may be addressed by index (in the list of answers).
 * Every time a answer component is added (or changes) in a dynamic service query,
 * the callback 'm_fnCallback' is called.
 * The answer list may be searched by service and host domain.
 *
 * Service query object may be connected to a linked list.
 */

/*
 * MDNSResponder::_stcMDNSServiceQuery::_stcMDNSServiceQuery constructor
 */
MDNSResponder::_stcMDNSServiceQuery::_stcMDNSServiceQuery(void)
:   m_pNext(0),
    m_fnCallback(0),
    m_pUserdata(0),
    m_bLegacyQuery(false),
    m_bAwaitingAnswers(true),
    m_pAnswers(0) {

    clear();
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::~_stcMDNSServiceQuery destructor
 */
MDNSResponder::_stcMDNSServiceQuery::~_stcMDNSServiceQuery(void) {
    
    clear();
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::clear
 */
bool MDNSResponder::_stcMDNSServiceQuery::clear(void) {
    
    m_fnCallback = 0;
    m_pUserdata = 0;
    m_bLegacyQuery = false;
    m_bAwaitingAnswers = true;
    while (m_pAnswers) {
        stcAnswer*  pNext = m_pAnswers->m_pNext;
        delete m_pAnswers;
        m_pAnswers = pNext;
    }
    return true;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::answerCount
 */
uint32_t MDNSResponder::_stcMDNSServiceQuery::answerCount(void) const {
    
    uint32_t    u32Count = 0;
    
    stcAnswer*  pAnswer = m_pAnswers;
    while (pAnswer) {
        ++u32Count;
        pAnswer = pAnswer->m_pNext;
    }
    return u32Count;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::answerAtIndex
 */
const MDNSResponder::_stcMDNSServiceQuery::_stcAnswer* MDNSResponder::_stcMDNSServiceQuery::answerAtIndex(uint32_t p_u32Index) const {
    
    const stcAnswer*    pAnswer = 0;
    
    if (((uint32_t)(-1) != p_u32Index) &&
        (m_pAnswers)) {

        uint32_t    u32Index;
        for (pAnswer=m_pAnswers, u32Index=0; ((pAnswer) && (u32Index<p_u32Index)); pAnswer=pAnswer->m_pNext, ++u32Index);
    }
    return pAnswer;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::answerAtIndex
 */
MDNSResponder::_stcMDNSServiceQuery::_stcAnswer* MDNSResponder::_stcMDNSServiceQuery::answerAtIndex(uint32_t p_u32Index) {
    
    return (stcAnswer*)(((const _stcMDNSServiceQuery*)this)->answerAtIndex(p_u32Index));
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::indexOfAnswer
 */
uint32_t MDNSResponder::_stcMDNSServiceQuery::indexOfAnswer(const MDNSResponder::_stcMDNSServiceQuery::_stcAnswer* p_pAnswer) const {
    
    uint32_t    u32Index = 0;
    
    for (const stcAnswer* pAnswer=m_pAnswers; pAnswer; pAnswer=pAnswer->m_pNext, ++u32Index) {
        if (pAnswer == p_pAnswer) {
            return u32Index;
        }
    }
    return ((uint32_t)(-1));
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::addAnswer
 */
bool MDNSResponder::_stcMDNSServiceQuery::addAnswer(MDNSResponder::_stcMDNSServiceQuery::_stcAnswer* p_pAnswer) {
    
    bool    bResult = false;
    
    if (p_pAnswer) {
        p_pAnswer->m_pNext = m_pAnswers;
        m_pAnswers = p_pAnswer;
        bResult = true;
    }
    return bResult;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::removeAnswer
 */
bool MDNSResponder::_stcMDNSServiceQuery::removeAnswer(MDNSResponder::_stcMDNSServiceQuery::_stcAnswer* p_pAnswer) {
    
    bool    bResult = false;
    
    if (p_pAnswer) {
        stcAnswer*  pPred = m_pAnswers;
        while ((pPred) &&
               (pPred->m_pNext != p_pAnswer)) {
            pPred = pPred->m_pNext;
        }
        if (pPred) {
            pPred->m_pNext = p_pAnswer->m_pNext;
            delete p_pAnswer;
            bResult = true;
        }
        else if (m_pAnswers == p_pAnswer) { // No predecesor, but first item
            m_pAnswers = p_pAnswer->m_pNext;
            delete p_pAnswer;
            bResult = true;
        }
    }
    return bResult;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::findAnswerForServiceDomain
 */
MDNSResponder::_stcMDNSServiceQuery::_stcAnswer* MDNSResponder::_stcMDNSServiceQuery::findAnswerForServiceDomain(const MDNSResponder::stcMDNS_RRDomain& p_ServiceDomain) {
    
    stcAnswer*  pAnswer = m_pAnswers;
    while (pAnswer) {
        if (pAnswer->m_ServiceDomain == p_ServiceDomain) {
            break;
        }
        pAnswer = pAnswer->m_pNext;
    }
    return pAnswer;
}

/*
 * MDNSResponder::_stcMDNSServiceQuery::findAnswerForHostDomain
 */
MDNSResponder::_stcMDNSServiceQuery::_stcAnswer* MDNSResponder::_stcMDNSServiceQuery::findAnswerForHostDomain(const MDNSResponder::stcMDNS_RRDomain& p_HostDomain) {

    stcAnswer*  pAnswer = m_pAnswers;
    while (pAnswer) {
        if (pAnswer->m_HostDomain == p_HostDomain) {
            break;
        }
        pAnswer = pAnswer->m_pNext;
    }
    return pAnswer;
}


/**
 * MDNSResponder::stcMDNSSendParameter
 *
 * A 'collection' of properties and flags for one MDNS query or response.
 * Mainly managed by the 'Control' functions.
 * The current offset in the UPD output buffer is tracked to be able to do
 * a simple host or service domain compression.
 * 
 */

/**
 * MDNSResponder::stcMDNSSendParameter::stcDomainCacheItem
 *
 * A cached host or service domain, incl. the offset in the UDP output buffer.
 *
 */

/*
 * MDNSResponder::stcMDNSSendParameter::_stcDomainCacheItem::_stcDomainCacheItem constructor
 */
MDNSResponder::stcMDNSSendParameter::_stcDomainCacheItem::_stcDomainCacheItem(const void* p_pHostnameOrService,
                                                                              bool p_bAdditionalData,
                                                                              uint32_t p_u16Offset)
:   m_pNext(0),
    m_pHostnameOrService(p_pHostnameOrService),
    m_bAdditionalData(p_bAdditionalData),
    m_u16Offset(p_u16Offset) {
    
}

/**
 * MDNSResponder::stcMDNSSendParameter
 */

/*
 * MDNSResponder::_stcMDNSSendParameter::_stcMDNSSendParameter constructor
 */
MDNSResponder::_stcMDNSSendParameter::_stcMDNSSendParameter(void)
:   m_pQuestions(0),
    m_pDomainCacheItems(0) {
    
    clear();
}

/*
 * MDNSResponder::_stcMDNSSendParameter::~_stcMDNSSendParameter destructor
 */
MDNSResponder::_stcMDNSSendParameter::~_stcMDNSSendParameter(void) {
    
    clear();
}

/*
 * MDNSResponder::_stcMDNSSendParameter::clear
 */
bool MDNSResponder::_stcMDNSSendParameter::clear(void) {

    m_u16ID = 0;
    m_u8HostReplyMask = 0;
    m_u16Offset = 0;

    m_bLegacyQuery = false;
    m_bResponse = false;
    m_bAuthorative = false;
    m_bUnicast = false;
    m_bUnannounce = false;
    
    m_bCacheFlush = true;
    
    while (m_pQuestions) {
        stcMDNS_RRQuestion* pNext = m_pQuestions->m_pNext;
        delete m_pQuestions;
        m_pQuestions = pNext;
    }
    while (m_pDomainCacheItems) {
        stcDomainCacheItem* pNext = m_pDomainCacheItems->m_pNext;
        delete m_pDomainCacheItems;
        m_pDomainCacheItems = pNext;
    }
    return true;
}

/*
 * MDNSResponder::_stcMDNSSendParameter::shiftOffset
 */
bool MDNSResponder::_stcMDNSSendParameter::shiftOffset(uint16_t p_u16Shift) {

    m_u16Offset += p_u16Shift;
    return true;
}

/*
 * MDNSResponder::_stcMDNSSendParameter::addDomainCacheItem
 */
bool MDNSResponder::_stcMDNSSendParameter::addDomainCacheItem(const void* p_pHostnameOrService,
                                                              bool p_bAdditionalData,
                                                              uint16_t p_u16Offset) {
    
    bool    bResult = false;
    
    stcDomainCacheItem* pNewItem = 0;
    if ((p_pHostnameOrService) &&
        (p_u16Offset) &&
        ((pNewItem = new stcDomainCacheItem(p_pHostnameOrService, p_bAdditionalData, p_u16Offset)))) {
        
        pNewItem->m_pNext = m_pDomainCacheItems;
        bResult = ((m_pDomainCacheItems = pNewItem));
    }
    return bResult;
}

/*
 * MDNSResponder::_stcMDNSSendParameter::findCachedDomainOffset
 */
uint16_t MDNSResponder::_stcMDNSSendParameter::findCachedDomainOffset(const void* p_pHostnameOrService,
                                                                      bool p_bAdditionalData) const {
    
    const stcDomainCacheItem*   pCacheItem = m_pDomainCacheItems;
    
    for (; pCacheItem; pCacheItem=pCacheItem->m_pNext) {
        if ((pCacheItem->m_pHostnameOrService == p_pHostnameOrService) &&
            (pCacheItem->m_bAdditionalData == p_bAdditionalData)) { // Found cache item
            break;
        }
    }
    return (pCacheItem ? pCacheItem->m_u16Offset : 0);
}

}   // namespace LEAmDNS




