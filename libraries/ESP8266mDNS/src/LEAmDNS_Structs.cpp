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

namespace esp8266 {

/*
 * LEAmDNS
 */
namespace MDNSImplementation {

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
 * MDNSResponder::stcMDNSServiceTxt::stcMDNSServiceTxt constructor
 */
MDNSResponder::stcMDNSServiceTxt::stcMDNSServiceTxt(const char* p_pcKey /*= 0*/,
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
 * MDNSResponder::stcMDNSServiceTxt::stcMDNSServiceTxt copy-constructor
 */
MDNSResponder::stcMDNSServiceTxt::stcMDNSServiceTxt(const MDNSResponder::stcMDNSServiceTxt& p_Other)
:   m_pNext(0),
    m_pcKey(0),
    m_pcValue(0),
    m_bTemp(false) {
    
    operator=(p_Other);
}

/*
 * MDNSResponder::stcMDNSServiceTxt::~stcMDNSServiceTxt destructor
 */
MDNSResponder::stcMDNSServiceTxt::~stcMDNSServiceTxt(void) {
    
    clear();
}

/*
 * MDNSResponder::stcMDNSServiceTxt::operator=
 */
MDNSResponder::stcMDNSServiceTxt& MDNSResponder::stcMDNSServiceTxt::operator=(const MDNSResponder::stcMDNSServiceTxt& p_Other) {

    if (&p_Other != this) {
        clear();
        set(p_Other.m_pcKey, p_Other.m_pcValue, p_Other.m_bTemp);
    }
    return *this;
}

/*
 * MDNSResponder::stcMDNSServiceTxt::clear
 */
bool MDNSResponder::stcMDNSServiceTxt::clear(void) {
    
    releaseKey();
    releaseValue();
    return true;
}

/*
 * MDNSResponder::stcMDNSServiceTxt::allocKey
 */
char* MDNSResponder::stcMDNSServiceTxt::allocKey(size_t p_stLength) {
    
    releaseKey();
    if (p_stLength) {
        m_pcKey = new char[p_stLength + 1];
    }
    return m_pcKey;
}

/*
 * MDNSResponder::stcMDNSServiceTxt::setKey
 */
bool MDNSResponder::stcMDNSServiceTxt::setKey(const char* p_pcKey,
                                               size_t p_stLength) {
    
    bool bResult = false;
    
    releaseKey();
    if (p_stLength) {
        if (allocKey(p_stLength)) {
            strncpy(m_pcKey, p_pcKey, p_stLength);
            m_pcKey[p_stLength] = 0;
            bResult = true;
        }
    }
    return bResult;
}

/*
 * MDNSResponder::stcMDNSServiceTxt::setKey
 */
bool MDNSResponder::stcMDNSServiceTxt::setKey(const char* p_pcKey) {
    
    return setKey(p_pcKey, (p_pcKey ? strlen(p_pcKey) : 0));
}

/*
 * MDNSResponder::stcMDNSServiceTxt::releaseKey
 */
bool MDNSResponder::stcMDNSServiceTxt::releaseKey(void) {
    
    if (m_pcKey) {
        delete[] m_pcKey;
        m_pcKey = 0;
    }
    return true;
}

/*
 * MDNSResponder::stcMDNSServiceTxt::allocValue
 */
char* MDNSResponder::stcMDNSServiceTxt::allocValue(size_t p_stLength) {
    
    releaseValue();
    if (p_stLength) {
        m_pcValue = new char[p_stLength + 1];
    }
    return m_pcValue;
}

/*
 * MDNSResponder::stcMDNSServiceTxt::setValue
 */
bool MDNSResponder::stcMDNSServiceTxt::setValue(const char* p_pcValue,
                                                 size_t p_stLength) {
    
    bool bResult = false;
    
    releaseValue();
    if (p_stLength) {
        if (allocValue(p_stLength)) {
            strncpy(m_pcValue, p_pcValue, p_stLength);
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
 * MDNSResponder::stcMDNSServiceTxt::setValue
 */
bool MDNSResponder::stcMDNSServiceTxt::setValue(const char* p_pcValue) {
    
    return setValue(p_pcValue, (p_pcValue ? strlen(p_pcValue) : 0));
}

/*
 * MDNSResponder::stcMDNSServiceTxt::releaseValue
 */
bool MDNSResponder::stcMDNSServiceTxt::releaseValue(void) {
    
    if (m_pcValue) {
        delete[] m_pcValue;
        m_pcValue = 0;
    }
    return true;
}

/*
 * MDNSResponder::stcMDNSServiceTxt::set
 */
bool MDNSResponder::stcMDNSServiceTxt::set(const char* p_pcKey,
                                            const char* p_pcValue,
                                            bool p_bTemp /*= false*/) {
    
    m_bTemp = p_bTemp;
    return ((setKey(p_pcKey)) &&
            (setValue(p_pcValue)));
}

/*
 * MDNSResponder::stcMDNSServiceTxt::update
 */
bool MDNSResponder::stcMDNSServiceTxt::update(const char* p_pcValue) {
    
    return setValue(p_pcValue);
}

/*
 * MDNSResponder::stcMDNSServiceTxt::length
 *
 * length of eg. 'c#=1' without any closing '\0'
 */
size_t MDNSResponder::stcMDNSServiceTxt::length(void) const {

    size_t  stLength = 0;
    if (m_pcKey) {
        stLength += strlen(m_pcKey);                     // Key
        stLength += 1;                                      // '='
        stLength += (m_pcValue ? strlen(m_pcValue) : 0); // Value
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
 * MDNSResponder::stcMDNSServiceTxts::stcMDNSServiceTxts contructor
 */
MDNSResponder::stcMDNSServiceTxts::stcMDNSServiceTxts(void)
:   m_pTxts(0) {
    
}

/*
 * MDNSResponder::stcMDNSServiceTxts::stcMDNSServiceTxts copy-constructor
 */
MDNSResponder::stcMDNSServiceTxts::stcMDNSServiceTxts(const stcMDNSServiceTxts& p_Other)
:   m_pTxts(0) {
    
    operator=(p_Other);
}

/*
 * MDNSResponder::stcMDNSServiceTxts::~stcMDNSServiceTxts destructor
 */
MDNSResponder::stcMDNSServiceTxts::~stcMDNSServiceTxts(void) {
    
    clear();
}

/*
 * MDNSResponder::stcMDNSServiceTxts::operator=
 */
MDNSResponder::stcMDNSServiceTxts& MDNSResponder::stcMDNSServiceTxts::operator=(const stcMDNSServiceTxts& p_Other) {

    if (this != &p_Other) {
        clear();
        
        for (stcMDNSServiceTxt* pOtherTxt=p_Other.m_pTxts; pOtherTxt; pOtherTxt=pOtherTxt->m_pNext) {
            add(new stcMDNSServiceTxt(*pOtherTxt));
        }
    }
    return *this;
}

/*
 * MDNSResponder::stcMDNSServiceTxts::clear
 */
bool MDNSResponder::stcMDNSServiceTxts::clear(void) {
    
    while (m_pTxts) {
        stcMDNSServiceTxt* pNext = m_pTxts->m_pNext;
        delete m_pTxts;
        m_pTxts = pNext;
    }
    return true;
}

/*
 * MDNSResponder::stcMDNSServiceTxts::add
 */
bool MDNSResponder::stcMDNSServiceTxts::add(MDNSResponder::stcMDNSServiceTxt* p_pTxt) {
    
    bool bResult = false;
    
    if (p_pTxt) {
        p_pTxt->m_pNext = m_pTxts;
        m_pTxts = p_pTxt;
        bResult = true;
    }
    return bResult;
}

/*
 * MDNSResponder::stcMDNSServiceTxts::remove
 */
bool MDNSResponder::stcMDNSServiceTxts::remove(stcMDNSServiceTxt* p_pTxt) {
    
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
 * MDNSResponder::stcMDNSServiceTxts::removeTempTxts
 */
bool MDNSResponder::stcMDNSServiceTxts::removeTempTxts(void) {

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
 * MDNSResponder::stcMDNSServiceTxts::find
 */
MDNSResponder::stcMDNSServiceTxt* MDNSResponder::stcMDNSServiceTxts::find(const char* p_pcKey) {
    
    stcMDNSServiceTxt* pResult = 0;
    
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
 * MDNSResponder::stcMDNSServiceTxts::find
 */
const MDNSResponder::stcMDNSServiceTxt* MDNSResponder::stcMDNSServiceTxts::find(const char* p_pcKey) const {
    
    const stcMDNSServiceTxt*   pResult = 0;
    
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
 * MDNSResponder::stcMDNSServiceTxts::find
 */
MDNSResponder::stcMDNSServiceTxt* MDNSResponder::stcMDNSServiceTxts::find(const stcMDNSServiceTxt* p_pTxt) {
    
    stcMDNSServiceTxt* pResult = 0;
    
    for (stcMDNSServiceTxt* pTxt=m_pTxts; pTxt; pTxt=pTxt->m_pNext) {
        if (p_pTxt == pTxt) {
            pResult = pTxt;
            break;
        }
    }
    return pResult;
}

/*
 * MDNSResponder::stcMDNSServiceTxts::length
 */
uint16_t MDNSResponder::stcMDNSServiceTxts::length(void) const {
    
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
 * MDNSResponder::stcMDNSServiceTxts::c_strLength
 *
 * (incl. closing '\0'). Length bytes place is used for delimiting ';' and closing '\0'
 */
size_t MDNSResponder::stcMDNSServiceTxts::c_strLength(void) const {
    
    return length();
}

/*
 * MDNSResponder::stcMDNSServiceTxts::c_str
 */
bool MDNSResponder::stcMDNSServiceTxts::c_str(char* p_pcBuffer) {
    
    bool bResult = false;
    
    if (p_pcBuffer) {
        bResult = true;
        
        *p_pcBuffer = 0;
        for (stcMDNSServiceTxt* pTxt=m_pTxts; ((bResult) && (pTxt)); pTxt = pTxt->m_pNext) {
            size_t  stLength;
            if ((bResult = (0 != (stLength = (pTxt->m_pcKey ? strlen(pTxt->m_pcKey) : 0))))) {
                if (pTxt != m_pTxts) {
                    *p_pcBuffer++ = ';';
                }
                strncpy(p_pcBuffer, pTxt->m_pcKey, stLength); p_pcBuffer[stLength] = 0;
                p_pcBuffer += stLength;
                *p_pcBuffer++ = '=';
                if ((stLength = (pTxt->m_pcValue ? strlen(pTxt->m_pcValue) : 0))) {
                    strncpy(p_pcBuffer, pTxt->m_pcValue, stLength); p_pcBuffer[stLength] = 0;
                    p_pcBuffer += stLength;
                }
            }
        }
        *p_pcBuffer++ = 0;
    }
    return bResult;
}

/*
 * MDNSResponder::stcMDNSServiceTxts::bufferLength
 *
 * (incl. closing '\0').
 */
size_t MDNSResponder::stcMDNSServiceTxts::bufferLength(void) const {
    
    return (length() + 1);
}

/*
 * MDNSResponder::stcMDNSServiceTxts::toBuffer
 */
bool MDNSResponder::stcMDNSServiceTxts::buffer(char* p_pcBuffer) {
    
    bool bResult = false;

    if (p_pcBuffer) {
        bResult = true;
        
        *p_pcBuffer = 0;
        for (stcMDNSServiceTxt* pTxt=m_pTxts; ((bResult) && (pTxt)); pTxt = pTxt->m_pNext) {
            *(unsigned char*)p_pcBuffer++ = pTxt->length();
            size_t  stLength;
            if ((bResult = (0 != (stLength = (pTxt->m_pcKey ? strlen(pTxt->m_pcKey) : 0))))) {
                memcpy(p_pcBuffer, pTxt->m_pcKey, stLength);
                p_pcBuffer += stLength;
                *p_pcBuffer++ = '=';
                if ((stLength = (pTxt->m_pcValue ? strlen(pTxt->m_pcValue) : 0))) {
                    memcpy(p_pcBuffer, pTxt->m_pcValue, stLength);
                    p_pcBuffer += stLength;
                }
            }
        }
        *p_pcBuffer++ = 0;
    }
    return bResult;
}

/*
 * MDNSResponder::stcMDNSServiceTxts::compare
 */
bool MDNSResponder::stcMDNSServiceTxts::compare(const MDNSResponder::stcMDNSServiceTxts& p_Other) const {

    bool    bResult = false;

    if ((bResult = (length() == p_Other.length()))) {
        // Compare A->B
        for (const stcMDNSServiceTxt* pTxt=m_pTxts; ((bResult) && (pTxt)); pTxt=pTxt->m_pNext) {
            const stcMDNSServiceTxt*    pOtherTxt = p_Other.find(pTxt->m_pcKey);
            bResult = ((pOtherTxt) &&
                       (pTxt->m_pcValue) &&
                       (pOtherTxt->m_pcValue) &&
                       (strlen(pTxt->m_pcValue) == strlen(pOtherTxt->m_pcValue)) &&
                       (0 == strcmp(pTxt->m_pcValue, pOtherTxt->m_pcValue)));
        }
        // Compare B->A
        for (const stcMDNSServiceTxt* pOtherTxt=p_Other.m_pTxts; ((bResult) && (pOtherTxt)); pOtherTxt=pOtherTxt->m_pNext) {
            const stcMDNSServiceTxt*    pTxt = find(pOtherTxt->m_pcKey);
            bResult = ((pTxt) &&
                       (pOtherTxt->m_pcValue) &&
                       (pTxt->m_pcValue) &&
                       (strlen(pOtherTxt->m_pcValue) == strlen(pTxt->m_pcValue)) &&
                       (0 == strcmp(pOtherTxt->m_pcValue, pTxt->m_pcValue)));
        }
    }
    return bResult;
}

/*
 * MDNSResponder::stcMDNSServiceTxts::operator==
 */
bool MDNSResponder::stcMDNSServiceTxts::operator==(const stcMDNSServiceTxts& p_Other) const {
    
    return compare(p_Other);
}

/*
 * MDNSResponder::stcMDNSServiceTxts::operator!=
 */
bool MDNSResponder::stcMDNSServiceTxts::operator!=(const stcMDNSServiceTxts& p_Other) const {
    
    return !compare(p_Other);
}


/**
 * MDNSResponder::stcMDNS_MsgHeader
 *
 * A MDNS message haeder.
 *
 */

/*
 * MDNSResponder::stcMDNS_MsgHeader::stcMDNS_MsgHeader
 */
MDNSResponder::stcMDNS_MsgHeader::stcMDNS_MsgHeader(uint16_t p_u16ID /*= 0*/,
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
 * MDNSResponder::stcMDNS_RRDomain::stcMDNS_RRDomain constructor
 */
MDNSResponder::stcMDNS_RRDomain::stcMDNS_RRDomain(void)
:   m_u16NameLength(0) {
    
    clear();
}

/*
 * MDNSResponder::stcMDNS_RRDomain::stcMDNS_RRDomain copy-constructor
 */
MDNSResponder::stcMDNS_RRDomain::stcMDNS_RRDomain(const stcMDNS_RRDomain& p_Other)
:   m_u16NameLength(0) {
    
    operator=(p_Other);
}

/*
 * MDNSResponder::stcMDNS_RRDomain::operator =
 */
MDNSResponder::stcMDNS_RRDomain& MDNSResponder::stcMDNS_RRDomain::operator=(const stcMDNS_RRDomain& p_Other) {
    
    if (&p_Other != this) {
        memcpy(m_acName, p_Other.m_acName, sizeof(m_acName));
        m_u16NameLength = p_Other.m_u16NameLength;
    }
    return *this;
}

/*
 * MDNSResponder::stcMDNS_RRDomain::clear
 */
bool MDNSResponder::stcMDNS_RRDomain::clear(void) {
    
    memset(m_acName, 0, sizeof(m_acName));
    m_u16NameLength = 0;
    return true;
}

/*
 * MDNSResponder::stcMDNS_RRDomain::addLabel
 */
bool MDNSResponder::stcMDNS_RRDomain::addLabel(const char* p_pcLabel,
                                                bool p_bPrependUnderline /*= false*/) {
    
    bool    bResult = false;    
    
    size_t  stLength = (p_pcLabel
                        ? (strlen(p_pcLabel) + (p_bPrependUnderline ? 1 : 0))
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
            strncpy(&(m_acName[m_u16NameLength]), p_pcLabel, stLength); m_acName[m_u16NameLength + stLength] = 0;
            m_u16NameLength += stLength;
        }
        bResult = true;
    }
    return bResult;
}

/*
 * MDNSResponder::stcMDNS_RRDomain::compare
 */
bool MDNSResponder::stcMDNS_RRDomain::compare(const stcMDNS_RRDomain& p_Other) const {
    
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
 * MDNSResponder::stcMDNS_RRDomain::operator ==
 */
bool MDNSResponder::stcMDNS_RRDomain::operator==(const stcMDNS_RRDomain& p_Other) const {
    
    return compare(p_Other);
}

/*
 * MDNSResponder::stcMDNS_RRDomain::operator !=
 */
bool MDNSResponder::stcMDNS_RRDomain::operator!=(const stcMDNS_RRDomain& p_Other) const {
    
    return !compare(p_Other);
}

/*
 * MDNSResponder::stcMDNS_RRDomain::operator >
 */
bool MDNSResponder::stcMDNS_RRDomain::operator>(const stcMDNS_RRDomain& p_Other) const {

    // TODO: Check, if this is a good idea...
    return !compare(p_Other);
}

/*
 * MDNSResponder::stcMDNS_RRDomain::c_strLength
 */
size_t MDNSResponder::stcMDNS_RRDomain::c_strLength(void) const {
    
    size_t          stLength = 0;
    
    unsigned char*  pucLabelLength = (unsigned char*)m_acName;
    while (*pucLabelLength) {
        stLength += (*pucLabelLength + 1 /* +1 for '.' or '\0'*/);
        pucLabelLength += (*pucLabelLength + 1);
    }
    return stLength;
}

/*
 * MDNSResponder::stcMDNS_RRDomain::c_str
 */
bool MDNSResponder::stcMDNS_RRDomain::c_str(char* p_pcBuffer) {
    
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
 * MDNSResponder::stcMDNS_RRAttributes::stcMDNS_RRAttributes constructor
 */
MDNSResponder::stcMDNS_RRAttributes::stcMDNS_RRAttributes(uint16_t p_u16Type /*= 0*/,
                                                            uint16_t p_u16Class /*= 1 DNS_RRCLASS_IN Internet*/)
:   m_u16Type(p_u16Type),
    m_u16Class(p_u16Class) {
    
}

/*
 * MDNSResponder::stcMDNS_RRAttributes::stcMDNS_RRAttributes copy-constructor
 */
MDNSResponder::stcMDNS_RRAttributes::stcMDNS_RRAttributes(const MDNSResponder::stcMDNS_RRAttributes& p_Other) {
    
    operator=(p_Other);
}

/*
 * MDNSResponder::stcMDNS_RRAttributes::operator =
 */
MDNSResponder::stcMDNS_RRAttributes& MDNSResponder::stcMDNS_RRAttributes::operator=(const MDNSResponder::stcMDNS_RRAttributes& p_Other) {
    
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
 * MDNSResponder::stcMDNS_RRHeader::stcMDNS_RRHeader constructor
 */
MDNSResponder::stcMDNS_RRHeader::stcMDNS_RRHeader(void) {
    
}

/*
 * MDNSResponder::stcMDNS_RRHeader::stcMDNS_RRHeader copy-constructor
 */
MDNSResponder::stcMDNS_RRHeader::stcMDNS_RRHeader(const stcMDNS_RRHeader& p_Other) {
    
    operator=(p_Other);
}

/*
 * MDNSResponder::stcMDNS_RRHeader::operator =
 */
MDNSResponder::stcMDNS_RRHeader& MDNSResponder::stcMDNS_RRHeader::operator=(const MDNSResponder::stcMDNS_RRHeader& p_Other) {
    
    if (&p_Other != this) {
        m_Domain = p_Other.m_Domain;
        m_Attributes = p_Other.m_Attributes;
    }
    return *this;
}

/*
 * MDNSResponder::stcMDNS_RRHeader::clear
 */
bool MDNSResponder::stcMDNS_RRHeader::clear(void) {
    
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
 * MDNSResponder::stcMDNS_RRQuestion::stcMDNS_RRQuestion constructor
 */
MDNSResponder::stcMDNS_RRQuestion::stcMDNS_RRQuestion(void)
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
 * MDNSResponder::stcMDNS_RRAnswer::stcMDNS_RRAnswer constructor
 */
MDNSResponder::stcMDNS_RRAnswer::stcMDNS_RRAnswer(enuAnswerType p_AnswerType,
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
 * MDNSResponder::stcMDNS_RRAnswer::~stcMDNS_RRAnswer destructor
 */
MDNSResponder::stcMDNS_RRAnswer::~stcMDNS_RRAnswer(void) {
    
}

/*
 * MDNSResponder::stcMDNS_RRAnswer::answerType
 */
MDNSResponder::enuAnswerType MDNSResponder::stcMDNS_RRAnswer::answerType(void) const {
    
    return m_AnswerType;
}

/*
 * MDNSResponder::stcMDNS_RRAnswer::clear
 */
bool MDNSResponder::stcMDNS_RRAnswer::clear(void) {
    
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
     * MDNSResponder::stcMDNS_RRAnswerA::stcMDNS_RRAnswerA constructor
     */
    MDNSResponder::stcMDNS_RRAnswerA::stcMDNS_RRAnswerA(const MDNSResponder::stcMDNS_RRHeader& p_Header,
                                                          uint32_t p_u32TTL)
    :   stcMDNS_RRAnswer(AnswerType_A, p_Header, p_u32TTL),
        m_IPAddress(0, 0, 0, 0) {
        
    }

    /*
     * MDNSResponder::stcMDNS_RRAnswerA::stcMDNS_RRAnswerA destructor
     */
    MDNSResponder::stcMDNS_RRAnswerA::~stcMDNS_RRAnswerA(void) {
        
        clear();
    }

    /*
     * MDNSResponder::stcMDNS_RRAnswerA::clear
     */
    bool MDNSResponder::stcMDNS_RRAnswerA::clear(void) {
        
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
 * MDNSResponder::stcMDNS_RRAnswerPTR::stcMDNS_RRAnswerPTR constructor
 */
MDNSResponder::stcMDNS_RRAnswerPTR::stcMDNS_RRAnswerPTR(const MDNSResponder::stcMDNS_RRHeader& p_Header,
                                                          uint32_t p_u32TTL)
:   stcMDNS_RRAnswer(AnswerType_PTR, p_Header, p_u32TTL) {

}

/*
 * MDNSResponder::stcMDNS_RRAnswerPTR::~stcMDNS_RRAnswerPTR destructor
 */
MDNSResponder::stcMDNS_RRAnswerPTR::~stcMDNS_RRAnswerPTR(void) {
    
    clear();
}

/*
 * MDNSResponder::stcMDNS_RRAnswerPTR::clear
 */
bool MDNSResponder::stcMDNS_RRAnswerPTR::clear(void) {
    
    m_PTRDomain.clear();
    return true;
}


/**
 * MDNSResponder::stcMDNS_RRAnswerTXT
 *
 * A MDNS TXT answer object.
 * Extends the base class by a MDNS TXT items list member.
 *
 */
    
/*
 * MDNSResponder::stcMDNS_RRAnswerTXT::stcMDNS_RRAnswerTXT constructor
 */
MDNSResponder::stcMDNS_RRAnswerTXT::stcMDNS_RRAnswerTXT(const MDNSResponder::stcMDNS_RRHeader& p_Header,
                                                          uint32_t p_u32TTL)
:   stcMDNS_RRAnswer(AnswerType_TXT, p_Header, p_u32TTL) {
    
}
    
/*
 * MDNSResponder::stcMDNS_RRAnswerTXT::~stcMDNS_RRAnswerTXT destructor
 */
MDNSResponder::stcMDNS_RRAnswerTXT::~stcMDNS_RRAnswerTXT(void) {
    
    clear();
}
    
/*
 * MDNSResponder::stcMDNS_RRAnswerTXT::clear
 */
bool MDNSResponder::stcMDNS_RRAnswerTXT::clear(void) {
    
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
     * MDNSResponder::stcMDNS_RRAnswerAAAA::stcMDNS_RRAnswerAAAA constructor
     */
    MDNSResponder::stcMDNS_RRAnswerAAAA::stcMDNS_RRAnswerAAAA(const MDNSResponder::stcMDNS_RRHeader& p_Header,
                                                                uint32_t p_u32TTL)
    :   stcMDNS_RRAnswer(AnswerType_AAAA, p_Header, p_u32TTL) {
        
    }

    /*
     * MDNSResponder::stcMDNS_RRAnswerAAAA::~stcMDNS_RRAnswerAAAA destructor
     */
    MDNSResponder::stcMDNS_RRAnswerAAAA::~stcMDNS_RRAnswerAAAA(void) {
        
        clear();
    }

    /*
     * MDNSResponder::stcMDNS_RRAnswerAAAA::clear
     */
    bool MDNSResponder::stcMDNS_RRAnswerAAAA::clear(void) {
        
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
 * MDNSResponder::stcMDNS_RRAnswerSRV::stcMDNS_RRAnswerSRV constructor
 */
MDNSResponder::stcMDNS_RRAnswerSRV::stcMDNS_RRAnswerSRV(const MDNSResponder::stcMDNS_RRHeader& p_Header,
                                                          uint32_t p_u32TTL)
:   stcMDNS_RRAnswer(AnswerType_SRV, p_Header, p_u32TTL),
    m_u16Priority(0),
    m_u16Weight(0),
    m_u16Port(0) {
    
}

/*
 * MDNSResponder::stcMDNS_RRAnswerSRV::~stcMDNS_RRAnswerSRV destructor
 */
MDNSResponder::stcMDNS_RRAnswerSRV::~stcMDNS_RRAnswerSRV(void) {
    
    clear();
}

/*
 * MDNSResponder::stcMDNS_RRAnswerSRV::clear
 */
bool MDNSResponder::stcMDNS_RRAnswerSRV::clear(void) {
    
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
 * MDNSResponder::stcMDNS_RRAnswerGeneric::stcMDNS_RRAnswerGeneric constructor
 */
MDNSResponder::stcMDNS_RRAnswerGeneric::stcMDNS_RRAnswerGeneric(const stcMDNS_RRHeader& p_Header,
                                                                  uint32_t p_u32TTL)
:   stcMDNS_RRAnswer(AnswerType_Generic, p_Header, p_u32TTL),
    m_u16RDLength(0),
    m_pu8RDData(0) {
    
}

/*
 * MDNSResponder::stcMDNS_RRAnswerGeneric::~stcMDNS_RRAnswerGeneric destructor
 */
MDNSResponder::stcMDNS_RRAnswerGeneric::~stcMDNS_RRAnswerGeneric(void) {
    
    clear();
}

/*
 * MDNSResponder::stcMDNS_RRAnswerGeneric::clear
 */
bool MDNSResponder::stcMDNS_RRAnswerGeneric::clear(void) {
    
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
 * MDNSResponder::stcProbeInformation::stcProbeInformation constructor
 */
MDNSResponder::stcProbeInformation::stcProbeInformation(void)
:   m_ProbingStatus(ProbingStatus_WaitingForData),
    m_u8SentCount(0),
    m_Timeout(esp8266::polledTimeout::oneShotMs::neverExpires),
    m_bConflict(false),
    m_bTiebreakNeeded(false),
    m_fnHostProbeResultCallback(0),
	m_fnServiceProbeResultCallback(0) {
}

/*
 * MDNSResponder::stcProbeInformation::clear
 */
bool MDNSResponder::stcProbeInformation::clear(bool p_bClearUserdata /*= false*/) {

    m_ProbingStatus = ProbingStatus_WaitingForData;
    m_u8SentCount = 0;
    m_Timeout.reset(esp8266::polledTimeout::oneShotMs::neverExpires);
    m_bConflict = false;
    m_bTiebreakNeeded = false;
    if (p_bClearUserdata) {
        m_fnHostProbeResultCallback = 0;
        m_fnServiceProbeResultCallback = 0;
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
 * MDNSResponder::stcMDNSService::stcMDNSService constructor
 */
MDNSResponder::stcMDNSService::stcMDNSService(const char* p_pcName /*= 0*/,
                                              const char* p_pcService /*= 0*/,
                                              const char* p_pcProtocol /*= 0*/)
:   m_pNext(0),
    m_pcName(0),
    m_bAutoName(false),
    m_pcService(0),
    m_pcProtocol(0),
    m_u16Port(0),
    m_u8ReplyMask(0),
    m_fnTxtCallback(0) {
    
    setName(p_pcName);
    setService(p_pcService);
    setProtocol(p_pcProtocol);
}

/*
 * MDNSResponder::stcMDNSService::~stcMDNSService destructor
 */
MDNSResponder::stcMDNSService::~stcMDNSService(void) {
    
    releaseName();
    releaseService();
    releaseProtocol();
}

/*
 * MDNSResponder::stcMDNSService::setName
 */
bool MDNSResponder::stcMDNSService::setName(const char* p_pcName) {
    
    bool bResult = false;
    
    releaseName();
    size_t stLength = (p_pcName ? strlen(p_pcName) : 0);
    if (stLength) {
        if ((bResult = (0 != (m_pcName = new char[stLength + 1])))) {
            strncpy(m_pcName, p_pcName, stLength);
            m_pcName[stLength] = 0;
        }
    }
    else {
        bResult = true;
    }
    return bResult;
}

/*
 * MDNSResponder::stcMDNSService::releaseName
 */
bool MDNSResponder::stcMDNSService::releaseName(void) {
    
    if (m_pcName) {
        delete[] m_pcName;
        m_pcName = 0;
    }
    return true;
}

/*
 * MDNSResponder::stcMDNSService::setService
 */
bool MDNSResponder::stcMDNSService::setService(const char* p_pcService) {
    
    bool bResult = false;
    
    releaseService();
    size_t stLength = (p_pcService ? strlen(p_pcService) : 0);
    if (stLength) {
        if ((bResult = (0 != (m_pcService = new char[stLength + 1])))) {
            strncpy(m_pcService, p_pcService, stLength);
            m_pcService[stLength] = 0;
        }
    }
    else {
        bResult = true;
    }
    return bResult;
}

/*
 * MDNSResponder::stcMDNSService::releaseService
 */
bool MDNSResponder::stcMDNSService::releaseService(void) {

    if (m_pcService) {
        delete[] m_pcService;
        m_pcService = 0;
    }
    return true;
}

/*
 * MDNSResponder::stcMDNSService::setProtocol
 */
bool MDNSResponder::stcMDNSService::setProtocol(const char* p_pcProtocol) {

    bool bResult = false;
    
    releaseProtocol();
    size_t stLength = (p_pcProtocol ? strlen(p_pcProtocol) : 0);
    if (stLength) {
        if ((bResult = (0 != (m_pcProtocol = new char[stLength + 1])))) {
            strncpy(m_pcProtocol, p_pcProtocol, stLength);
            m_pcProtocol[stLength] = 0;
        }
    }
    else {
        bResult = true;
    }
    return bResult;
}

/*
 * MDNSResponder::stcMDNSService::releaseProtocol
 */
bool MDNSResponder::stcMDNSService::releaseProtocol(void) {

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
 * /

/ *
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::stcTTL constructor
 * /
MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::stcTTL(uint32_t p_u32TTL / *= 0* /)
:   m_bUpdateScheduled(false) {
    
    set(p_u32TTL * 1000);
}

/ *
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::set
 * /
bool MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::set(uint32_t p_u32TTL) {
    
    m_TTLTimeFlag.restart(p_u32TTL * 1000);
    m_bUpdateScheduled = false;

    return true;
}

/ *
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::has80Percent
 * /
bool MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::has80Percent(void) const {
    
    return ((m_TTLTimeFlag.getTimeout()) &&
            (!m_bUpdateScheduled) &&
            (m_TTLTimeFlag.hypotheticalTimeout((m_TTLTimeFlag.getTimeout() * 800) / 1000)));
}

/ *
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::isOutdated
 * /
bool MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::isOutdated(void) const {
    
    return ((m_TTLTimeFlag.getTimeout()) &&
            (m_TTLTimeFlag.flagged()));
}*/


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
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::stcTTL constructor
 */
MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::stcTTL(void)
:   m_u32TTL(0),
    m_TTLTimeout(esp8266::polledTimeout::oneShotMs::neverExpires),
    m_timeoutLevel(TIMEOUTLEVEL_UNSET) {

}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::set
 */
bool MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::set(uint32_t p_u32TTL) {

    m_u32TTL = p_u32TTL;
    if (m_u32TTL) {
        m_timeoutLevel = TIMEOUTLEVEL_BASE;             // Set to 80%
        m_TTLTimeout.reset(timeout());
    }
    else {
        m_timeoutLevel = TIMEOUTLEVEL_UNSET;            // undef
        m_TTLTimeout.reset(esp8266::polledTimeout::oneShotMs::neverExpires);
    }
    return true;
}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::flagged
 */
bool MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::flagged(void) const {

    return ((m_u32TTL) &&
            (TIMEOUTLEVEL_UNSET != m_timeoutLevel) &&
            (m_TTLTimeout.expired()));
}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::restart
 */
bool MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::restart(void) {

    bool    bResult = true;

    if ((TIMEOUTLEVEL_BASE <= m_timeoutLevel) &&    // >= 80% AND
        (TIMEOUTLEVEL_FINAL > m_timeoutLevel)) {    // < 100%

        m_timeoutLevel += TIMEOUTLEVEL_INTERVAL;    // increment by 5%
        m_TTLTimeout.reset(timeout());
    }
    else {
        bResult = false;
        m_TTLTimeout.reset(esp8266::polledTimeout::oneShotMs::neverExpires);
        m_timeoutLevel = TIMEOUTLEVEL_UNSET;
    }
    return bResult;
}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::prepareDeletion
 */
bool MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::prepareDeletion(void) {

    m_timeoutLevel = TIMEOUTLEVEL_FINAL;
    m_TTLTimeout.reset(1 * 1000);   // See RFC 6762, 10.1

    return true;
}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::finalTimeoutLevel
 */
bool MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::finalTimeoutLevel(void) const {

    return (TIMEOUTLEVEL_FINAL == m_timeoutLevel);
}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::timeout
 */
unsigned long MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcTTL::timeout(void) const {

    uint32_t    u32Timeout = esp8266::polledTimeout::oneShotMs::neverExpires;

    if (TIMEOUTLEVEL_BASE == m_timeoutLevel) {          // 80%
        u32Timeout = (m_u32TTL * 800);                  // to milliseconds
    }
    else if ((TIMEOUTLEVEL_BASE < m_timeoutLevel) &&    // >80% AND
             (TIMEOUTLEVEL_FINAL >= m_timeoutLevel)) {  // <= 100%

        u32Timeout = (m_u32TTL * 50);
    }   // else: invalid
    return u32Timeout;
}


#ifdef MDNS_IP4_SUPPORT
/**
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcIP4Address
 *
 */

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcIP4Address::stcIP4Address constructor
 */
MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcIP4Address::stcIP4Address(IPAddress p_IPAddress,
                                                                                uint32_t p_u32TTL /*= 0*/)
:   m_pNext(0),
    m_IPAddress(p_IPAddress) {

    m_TTL.set(p_u32TTL);
}
#endif


/**
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer
 */

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcAnswer constructor
 */
MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcAnswer(void)
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
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::~stcAnswer destructor
 */
MDNSResponder::stcMDNSServiceQuery::stcAnswer::~stcAnswer(void) {
    
    clear();
}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::clear
 */
bool MDNSResponder::stcMDNSServiceQuery::stcAnswer::clear(void) {
    
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
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::allocServiceDomain
 *
 * Alloc memory for the char array representation of the service domain.
 *
 */
char* MDNSResponder::stcMDNSServiceQuery::stcAnswer::allocServiceDomain(size_t p_stLength) {
    
    releaseServiceDomain();
    if (p_stLength) {
        m_pcServiceDomain = new char[p_stLength];
    }
    return m_pcServiceDomain;
}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::releaseServiceDomain
 */
bool MDNSResponder::stcMDNSServiceQuery::stcAnswer::releaseServiceDomain(void) {

    if (m_pcServiceDomain) {
        delete[] m_pcServiceDomain;
        m_pcServiceDomain = 0;
    }
    return true;
}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::allocHostDomain
 *
 * Alloc memory for the char array representation of the host domain.
 *
 */
char* MDNSResponder::stcMDNSServiceQuery::stcAnswer::allocHostDomain(size_t p_stLength) {

    releaseHostDomain();
    if (p_stLength) {
        m_pcHostDomain = new char[p_stLength];
    }
    return m_pcHostDomain;
}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::releaseHostDomain
 */
bool MDNSResponder::stcMDNSServiceQuery::stcAnswer::releaseHostDomain(void) {

    if (m_pcHostDomain) {
        delete[] m_pcHostDomain;
        m_pcHostDomain = 0;
    }
    return true;
}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::allocTxts
 *
 * Alloc memory for the char array representation of the TXT items.
 *
 */
char* MDNSResponder::stcMDNSServiceQuery::stcAnswer::allocTxts(size_t p_stLength) {

    releaseTxts();
    if (p_stLength) {
        m_pcTxts = new char[p_stLength];
    }
    return m_pcTxts;
}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::releaseTxts
 */
bool MDNSResponder::stcMDNSServiceQuery::stcAnswer::releaseTxts(void) {
    
    if (m_pcTxts) {
        delete[] m_pcTxts;
        m_pcTxts = 0;
    }
    return true;
}

#ifdef MDNS_IP4_SUPPORT
/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::releaseIP4Addresses
 */
bool MDNSResponder::stcMDNSServiceQuery::stcAnswer::releaseIP4Addresses(void) {
    
    while (m_pIP4Addresses) {
        stcIP4Address*  pNext = m_pIP4Addresses->m_pNext;
        delete m_pIP4Addresses;
        m_pIP4Addresses = pNext;
    }
    return true;
}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::addIP4Address
 */
bool MDNSResponder::stcMDNSServiceQuery::stcAnswer::addIP4Address(MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcIP4Address* p_pIP4Address) {
    
    bool bResult = false;
    
    if (p_pIP4Address) {
        p_pIP4Address->m_pNext = m_pIP4Addresses;
        m_pIP4Addresses = p_pIP4Address;
        bResult = true;
    }
    return bResult;
}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::removeIP4Address
 */
bool MDNSResponder::stcMDNSServiceQuery::stcAnswer::removeIP4Address(MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcIP4Address* p_pIP4Address) {
    
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
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::findIP4Address (const)
 */
const MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcIP4Address* MDNSResponder::stcMDNSServiceQuery::stcAnswer::findIP4Address(const IPAddress& p_IPAddress) const {

    return (stcIP4Address*)(((const stcAnswer*)this)->findIP4Address(p_IPAddress));
}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::findIP4Address
 */
MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcIP4Address* MDNSResponder::stcMDNSServiceQuery::stcAnswer::findIP4Address(const IPAddress& p_IPAddress) {
    
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
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::IP4AddressCount
 */
uint32_t MDNSResponder::stcMDNSServiceQuery::stcAnswer::IP4AddressCount(void) const {
    
    uint32_t    u32Count = 0;
    
    stcIP4Address*  pIP4Address = m_pIP4Addresses;
    while (pIP4Address) {
        ++u32Count;
        pIP4Address = pIP4Address->m_pNext;
    }
    return u32Count;
}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::IP4AddressAtIndex
 */
MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcIP4Address* MDNSResponder::stcMDNSServiceQuery::stcAnswer::IP4AddressAtIndex(uint32_t p_u32Index) {

    return (stcIP4Address*)(((const stcAnswer*)this)->IP4AddressAtIndex(p_u32Index));
}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::IP4AddressAtIndex (const)
 */
const MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcIP4Address* MDNSResponder::stcMDNSServiceQuery::stcAnswer::IP4AddressAtIndex(uint32_t p_u32Index) const {
    
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
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::releaseIP6Addresses
 */
bool MDNSResponder::stcMDNSServiceQuery::stcAnswer::releaseIP6Addresses(void) {
    
    while (m_pIP6Addresses) {
        stcIP6Address*  pNext = m_pIP6Addresses->m_pNext;
        delete m_pIP6Addresses;
        m_pIP6Addresses = pNext;
    }
    return true;
}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::addIP6Address
 */
bool MDNSResponder::stcMDNSServiceQuery::stcAnswer::addIP6Address(MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcIP6Address* p_pIP6Address) {
    
    bool bResult = false;
    
    if (p_pIP6Address) {
        p_pIP6Address->m_pNext = m_pIP6Addresses;
        m_pIP6Addresses = p_pIP6Address;
        bResult = true;
    }
    return bResult;
}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::removeIP6Address
 */
bool MDNSResponder::stcMDNSServiceQuery::stcAnswer::removeIP6Address(MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcIP6Address* p_pIP6Address) {
    
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
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::findIP6Address
 */
MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcIP6Address* MDNSResponder::stcMDNSServiceQuery::stcAnswer::findIP6Address(const IP6Address& p_IPAddress) {

    return (stcIP6Address*)(((const stcAnswer*)this)->findIP6Address(p_IPAddress));
}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::findIP6Address (const)
 */
const MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcIP6Address* MDNSResponder::stcMDNSServiceQuery::stcAnswer::findIP6Address(const IPAddress& p_IPAddress) const {
    
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
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::IP6AddressCount
 */
uint32_t MDNSResponder::stcMDNSServiceQuery::stcAnswer::IP6AddressCount(void) const {
    
    uint32_t    u32Count = 0;
    
    stcIP6Address*  pIP6Address = m_pIP6Addresses;
    while (pIP6Address) {
        ++u32Count;
        pIP6Address = pIP6Address->m_pNext;
    }
    return u32Count;
}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::IP6AddressAtIndex (const)
 */
const MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcIP6Address* MDNSResponder::stcMDNSServiceQuery::stcAnswer::IP6AddressAtIndex(uint32_t p_u32Index) const {

    return (stcIP6Address*)(((const stcAnswer*)this)->IP6AddressAtIndex(p_u32Index));
}

/*
 * MDNSResponder::stcMDNSServiceQuery::stcAnswer::IP6AddressAtIndex
 */
MDNSResponder::stcMDNSServiceQuery::stcAnswer::stcIP6Address* MDNSResponder::stcMDNSServiceQuery::stcAnswer::IP6AddressAtIndex(uint32_t p_u32Index) {
    
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
 * MDNSResponder::stcMDNSServiceQuery::stcMDNSServiceQuery constructor
 */
MDNSResponder::stcMDNSServiceQuery::stcMDNSServiceQuery(void)
:   m_pNext(0),
    m_fnCallback(0),
    m_bLegacyQuery(false),
    m_u8SentCount(0),
    m_ResendTimeout(esp8266::polledTimeout::oneShotMs::neverExpires),
    m_bAwaitingAnswers(true),
    m_pAnswers(0) {

    clear();
}

/*
 * MDNSResponder::stcMDNSServiceQuery::~stcMDNSServiceQuery destructor
 */
MDNSResponder::stcMDNSServiceQuery::~stcMDNSServiceQuery(void) {
    
    clear();
}

/*
 * MDNSResponder::stcMDNSServiceQuery::clear
 */
bool MDNSResponder::stcMDNSServiceQuery::clear(void) {
    
    m_fnCallback = 0;
    m_bLegacyQuery = false;
    m_u8SentCount = 0;
    m_ResendTimeout.reset(esp8266::polledTimeout::oneShotMs::neverExpires);
    m_bAwaitingAnswers = true;
    while (m_pAnswers) {
        stcAnswer*  pNext = m_pAnswers->m_pNext;
        delete m_pAnswers;
        m_pAnswers = pNext;
    }
    return true;
}

/*
 * MDNSResponder::stcMDNSServiceQuery::answerCount
 */
uint32_t MDNSResponder::stcMDNSServiceQuery::answerCount(void) const {
    
    uint32_t    u32Count = 0;
    
    stcAnswer*  pAnswer = m_pAnswers;
    while (pAnswer) {
        ++u32Count;
        pAnswer = pAnswer->m_pNext;
    }
    return u32Count;
}

/*
 * MDNSResponder::stcMDNSServiceQuery::answerAtIndex
 */
const MDNSResponder::stcMDNSServiceQuery::stcAnswer* MDNSResponder::stcMDNSServiceQuery::answerAtIndex(uint32_t p_u32Index) const {
    
    const stcAnswer*    pAnswer = 0;
    
    if (((uint32_t)(-1) != p_u32Index) &&
        (m_pAnswers)) {

        uint32_t    u32Index;
        for (pAnswer=m_pAnswers, u32Index=0; ((pAnswer) && (u32Index<p_u32Index)); pAnswer=pAnswer->m_pNext, ++u32Index);
    }
    return pAnswer;
}

/*
 * MDNSResponder::stcMDNSServiceQuery::answerAtIndex
 */
MDNSResponder::stcMDNSServiceQuery::stcAnswer* MDNSResponder::stcMDNSServiceQuery::answerAtIndex(uint32_t p_u32Index) {
    
    return (stcAnswer*)(((const stcMDNSServiceQuery*)this)->answerAtIndex(p_u32Index));
}

/*
 * MDNSResponder::stcMDNSServiceQuery::indexOfAnswer
 */
uint32_t MDNSResponder::stcMDNSServiceQuery::indexOfAnswer(const MDNSResponder::stcMDNSServiceQuery::stcAnswer* p_pAnswer) const {
    
    uint32_t    u32Index = 0;
    
    for (const stcAnswer* pAnswer=m_pAnswers; pAnswer; pAnswer=pAnswer->m_pNext, ++u32Index) {
        if (pAnswer == p_pAnswer) {
            return u32Index;
        }
    }
    return ((uint32_t)(-1));
}

/*
 * MDNSResponder::stcMDNSServiceQuery::addAnswer
 */
bool MDNSResponder::stcMDNSServiceQuery::addAnswer(MDNSResponder::stcMDNSServiceQuery::stcAnswer* p_pAnswer) {
    
    bool    bResult = false;
    
    if (p_pAnswer) {
        p_pAnswer->m_pNext = m_pAnswers;
        m_pAnswers = p_pAnswer;
        bResult = true;
    }
    return bResult;
}

/*
 * MDNSResponder::stcMDNSServiceQuery::removeAnswer
 */
bool MDNSResponder::stcMDNSServiceQuery::removeAnswer(MDNSResponder::stcMDNSServiceQuery::stcAnswer* p_pAnswer) {
    
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
 * MDNSResponder::stcMDNSServiceQuery::findAnswerForServiceDomain
 */
MDNSResponder::stcMDNSServiceQuery::stcAnswer* MDNSResponder::stcMDNSServiceQuery::findAnswerForServiceDomain(const MDNSResponder::stcMDNS_RRDomain& p_ServiceDomain) {
    
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
 * MDNSResponder::stcMDNSServiceQuery::findAnswerForHostDomain
 */
MDNSResponder::stcMDNSServiceQuery::stcAnswer* MDNSResponder::stcMDNSServiceQuery::findAnswerForHostDomain(const MDNSResponder::stcMDNS_RRDomain& p_HostDomain) {

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
 * MDNSResponder::stcMDNSSendParameter::stcDomainCacheItem::stcDomainCacheItem constructor
 */
MDNSResponder::stcMDNSSendParameter::stcDomainCacheItem::stcDomainCacheItem(const void* p_pHostnameOrService,
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
 * MDNSResponder::stcMDNSSendParameter::stcMDNSSendParameter constructor
 */
MDNSResponder::stcMDNSSendParameter::stcMDNSSendParameter(void)
:   m_pQuestions(0),
    m_pDomainCacheItems(0) {
    
    clear();
}

/*
 * MDNSResponder::stcMDNSSendParameter::~stcMDNSSendParameter destructor
 */
MDNSResponder::stcMDNSSendParameter::~stcMDNSSendParameter(void) {
    
    clear();
}

/*
 * MDNSResponder::stcMDNSSendParameter::clear
 */
bool MDNSResponder::stcMDNSSendParameter::clear(void) {

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
 * MDNSResponder::stcMDNSSendParameter::shiftOffset
 */
bool MDNSResponder::stcMDNSSendParameter::shiftOffset(uint16_t p_u16Shift) {

    m_u16Offset += p_u16Shift;
    return true;
}

/*
 * MDNSResponder::stcMDNSSendParameter::addDomainCacheItem
 */
bool MDNSResponder::stcMDNSSendParameter::addDomainCacheItem(const void* p_pHostnameOrService,
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
 * MDNSResponder::stcMDNSSendParameter::findCachedDomainOffset
 */
uint16_t MDNSResponder::stcMDNSSendParameter::findCachedDomainOffset(const void* p_pHostnameOrService,
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

}   // namespace MDNSImplementation

} // namespace esp8266



