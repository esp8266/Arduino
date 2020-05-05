/*
    LEAmDNS2_Host_Structs.cpp

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

#include "LEAmDNS2_Priv.h"
#include "LEAmDNS2_lwIPdefs.h"

namespace esp8266
{

/*
    LEAmDNS
*/
namespace experimental
{

/**
    Internal CLASSES & STRUCTS
*/

/**
    MDNSResponder::clsHost::stcServiceTxt

    One MDNS TXT item.
    m_pcValue may be '\0'.
    Objects can be chained together (list, m_pNext).
    A 'm_bTemp' flag differentiates between static and dynamic items.
    Output as byte array 'c#=1' is supported.
*/

/*
    MDNSResponder::clsHost::stcServiceTxt::stcServiceTxt constructor
*/
MDNSResponder::clsHost::stcServiceTxt::stcServiceTxt(const char* p_pcKey /*= 0*/,
                                                     const char* p_pcValue /*= 0*/,
                                                     bool p_bTemp /*= false*/)
    :   m_pNext(0),
        m_pcKey(0),
        m_pcValue(0),
        m_bTemp(p_bTemp)
{
    setKey(p_pcKey);
    setValue(p_pcValue);
}

/*
    MDNSResponder::clsHost::stcServiceTxt::stcServiceTxt copy-constructor
*/
MDNSResponder::clsHost::stcServiceTxt::stcServiceTxt(const MDNSResponder::clsHost::stcServiceTxt& p_Other)
    :   m_pNext(0),
        m_pcKey(0),
        m_pcValue(0),
        m_bTemp(false)
{
    operator=(p_Other);
}

/*
    MDNSResponder::clsHost::stcServiceTxt::~stcServiceTxt destructor
*/
MDNSResponder::clsHost::stcServiceTxt::~stcServiceTxt(void)
{
    clear();
}

/*
    MDNSResponder::clsHost::stcServiceTxt::operator=
*/
MDNSResponder::clsHost::stcServiceTxt& MDNSResponder::clsHost::stcServiceTxt::operator=(const MDNSResponder::clsHost::stcServiceTxt& p_Other)
{
    if (&p_Other != this)
    {
        clear();
        set(p_Other.m_pcKey, p_Other.m_pcValue, p_Other.m_bTemp);
    }
    return *this;
}

/*
    MDNSResponder::clsHost::stcServiceTxt::clear
*/
bool MDNSResponder::clsHost::stcServiceTxt::clear(void)
{
    releaseKey();
    releaseValue();
    return true;
}

/*
    MDNSResponder::clsHost::stcServiceTxt::allocKey
*/
char* MDNSResponder::clsHost::stcServiceTxt::allocKey(size_t p_stLength)
{
    releaseKey();
    if (p_stLength)
    {
        m_pcKey = new char[p_stLength + 1];
    }
    return m_pcKey;
}

/*
    MDNSResponder::clsHost::stcServiceTxt::setKey
*/
bool MDNSResponder::clsHost::stcServiceTxt::setKey(const char* p_pcKey,
                                                   size_t p_stLength)
{
    bool bResult = false;

    releaseKey();
    if (p_stLength)
    {
        if (allocKey(p_stLength))
        {
            strncpy(m_pcKey, p_pcKey, p_stLength);
            m_pcKey[p_stLength] = 0;
            bResult = true;
        }
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::stcServiceTxt::setKey
*/
bool MDNSResponder::clsHost::stcServiceTxt::setKey(const char* p_pcKey)
{
    return setKey(p_pcKey, (p_pcKey ? strlen(p_pcKey) : 0));
}

/*
    MDNSResponder::clsHost::stcServiceTxt::releaseKey
*/
bool MDNSResponder::clsHost::stcServiceTxt::releaseKey(void)
{
    if (m_pcKey)
    {
        delete[] m_pcKey;
        m_pcKey = 0;
    }
    return true;
}

/*
    MDNSResponder::clsHost::stcServiceTxt::allocValue
*/
char* MDNSResponder::clsHost::stcServiceTxt::allocValue(size_t p_stLength)
{
    releaseValue();
    if (p_stLength)
    {
        m_pcValue = new char[p_stLength + 1];
    }
    return m_pcValue;
}

/*
    MDNSResponder::clsHost::stcServiceTxt::setValue
*/
bool MDNSResponder::clsHost::stcServiceTxt::setValue(const char* p_pcValue,
                                                     size_t p_stLength)
{
    bool bResult = false;

    releaseValue();
    if (p_stLength)
    {
        if (allocValue(p_stLength))
        {
            strncpy(m_pcValue, p_pcValue, p_stLength);
            m_pcValue[p_stLength] = 0;
            bResult = true;
        }
    }
    else    // No value -> also OK
    {
        bResult = true;
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::stcServiceTxt::setValue
*/
bool MDNSResponder::clsHost::stcServiceTxt::setValue(const char* p_pcValue)
{
    return setValue(p_pcValue, (p_pcValue ? strlen(p_pcValue) : 0));
}

/*
    MDNSResponder::clsHost::stcServiceTxt::releaseValue
*/
bool MDNSResponder::clsHost::stcServiceTxt::releaseValue(void)
{
    if (m_pcValue)
    {
        delete[] m_pcValue;
        m_pcValue = 0;
    }
    return true;
}

/*
    MDNSResponder::clsHost::stcServiceTxt::set
*/
bool MDNSResponder::clsHost::stcServiceTxt::set(const char* p_pcKey,
                                                const char* p_pcValue,
                                                bool p_bTemp /*= false*/)
{
    m_bTemp = p_bTemp;
    return ((setKey(p_pcKey)) &&
            (setValue(p_pcValue)));
}

/*
    MDNSResponder::clsHost::stcServiceTxt::update
*/
bool MDNSResponder::clsHost::stcServiceTxt::update(const char* p_pcValue)
{
    return setValue(p_pcValue);
}

/*
    MDNSResponder::clsHost::stcServiceTxt::length

    length of eg. 'c#=1' without any closing '\0'
*/
size_t MDNSResponder::clsHost::stcServiceTxt::length(void) const
{
    size_t  stLength = 0;
    if (m_pcKey)
    {
        stLength += strlen(m_pcKey);                     // Key
        stLength += 1;                                      // '='
        stLength += (m_pcValue ? strlen(m_pcValue) : 0); // Value
    }
    return stLength;
}


/**
    MDNSResponder::clsHost::stcServiceTxts

    A list of zero or more MDNS TXT items.
    Dynamic TXT items can be removed by 'removeTempTxts'.
    A TXT item can be looke up by its 'key' member.
    Export as ';'-separated byte array is supported.
    Export as 'length byte coded' byte array is supported.
    Comparision ((all A TXT items in B and equal) AND (all B TXT items in A and equal)) is supported.

*/

/*
    MDNSResponder::clsHost::stcServiceTxts::stcServiceTxts contructor
*/
MDNSResponder::clsHost::stcServiceTxts::stcServiceTxts(void)
    :   m_pTxts(0),
        m_pcCache(0)
{
}

/*
    MDNSResponder::clsHost::stcServiceTxts::stcServiceTxts copy-constructor
*/
MDNSResponder::clsHost::stcServiceTxts::stcServiceTxts(const stcServiceTxts& p_Other)
    :   m_pTxts(0),
        m_pcCache(0)
{
    operator=(p_Other);
}

/*
    MDNSResponder::clsHost::stcServiceTxts::~stcServiceTxts destructor
*/
MDNSResponder::clsHost::stcServiceTxts::~stcServiceTxts(void)
{
    clear();
}

/*
    MDNSResponder::clsHost::stcServiceTxts::operator=
*/
MDNSResponder::clsHost::stcServiceTxts& MDNSResponder::clsHost::stcServiceTxts::operator=(const stcServiceTxts& p_Other)
{
    if (this != &p_Other)
    {
        clear();

        for (stcServiceTxt* pOtherTxt = p_Other.m_pTxts; pOtherTxt; pOtherTxt = pOtherTxt->m_pNext)
        {
            add(new stcServiceTxt(*pOtherTxt));
        }
    }
    return *this;
}

/*
    MDNSResponder::clsHost::stcServiceTxts::clear
*/
bool MDNSResponder::clsHost::stcServiceTxts::clear(void)
{
    while (m_pTxts)
    {
        stcServiceTxt* pNext = m_pTxts->m_pNext;
        delete m_pTxts;
        m_pTxts = pNext;
    }
    return clearCache();
}

/*
    MDNSResponder::clsHost::stcServiceTxts::clearCache
*/
bool MDNSResponder::clsHost::stcServiceTxts::clearCache(void)
{
    if (m_pcCache)
    {
        delete[] m_pcCache;
        m_pcCache = 0;
    }
    return true;
}

/*
    MDNSResponder::clsHost::stcServiceTxts::add
*/
bool MDNSResponder::clsHost::stcServiceTxts::add(MDNSResponder::clsHost::stcServiceTxt* p_pTxt)
{
    bool bResult = false;

    if (p_pTxt)
    {
        p_pTxt->m_pNext = m_pTxts;
        m_pTxts = p_pTxt;
        bResult = true;
    }
    return ((clearCache()) &&
            (bResult));
}

/*
    MDNSResponder::clsHost::stcServiceTxts::remove
*/
bool MDNSResponder::clsHost::stcServiceTxts::remove(stcServiceTxt* p_pTxt)
{
    bool    bResult = false;

    if (p_pTxt)
    {
        stcServiceTxt*  pPred = m_pTxts;
        while ((pPred) &&
               (pPred->m_pNext != p_pTxt))
        {
            pPred = pPred->m_pNext;
        }
        if (pPred)
        {
            pPred->m_pNext = p_pTxt->m_pNext;
            delete p_pTxt;
            bResult = true;
        }
        else if (m_pTxts == p_pTxt)     // No predecesor, but first item
        {
            m_pTxts = p_pTxt->m_pNext;
            delete p_pTxt;
            bResult = true;
        }
    }
    return ((clearCache()) &&
            (bResult));
}

/*
    MDNSResponder::clsHost::stcServiceTxts::removeTempTxts
*/
bool MDNSResponder::clsHost::stcServiceTxts::removeTempTxts(void)
{
    bool    bResult = true;

    stcServiceTxt*  pTxt = m_pTxts;
    while ((bResult) &&
           (pTxt))
    {
        stcServiceTxt*  pNext = pTxt->m_pNext;
        if (pTxt->m_bTemp)
        {
            bResult = remove(pTxt);
        }
        pTxt = pNext;
    }
    return ((clearCache()) &&
            (bResult));
}

/*
    MDNSResponder::clsHost::stcServiceTxts::find
*/
MDNSResponder::clsHost::stcServiceTxt* MDNSResponder::clsHost::stcServiceTxts::find(const char* p_pcKey)
{
    stcServiceTxt* pResult = 0;

    for (stcServiceTxt* pTxt = m_pTxts; pTxt; pTxt = pTxt->m_pNext)
    {
        if ((p_pcKey) &&
            (0 == strcmp(pTxt->m_pcKey, p_pcKey)))
        {
            pResult = pTxt;
            break;
        }
    }
    return pResult;
}

/*
    MDNSResponder::clsHost::stcServiceTxts::find
*/
const MDNSResponder::clsHost::stcServiceTxt* MDNSResponder::clsHost::stcServiceTxts::find(const char* p_pcKey) const
{
    const stcServiceTxt*   pResult = 0;

    for (const stcServiceTxt* pTxt = m_pTxts; pTxt; pTxt = pTxt->m_pNext)
    {
        if ((p_pcKey) &&
            (0 == strcmp(pTxt->m_pcKey, p_pcKey)))
        {

            pResult = pTxt;
            break;
        }
    }
    return pResult;
}

/*
    MDNSResponder::clsHost::stcServiceTxts::find
*/
MDNSResponder::clsHost::stcServiceTxt* MDNSResponder::clsHost::stcServiceTxts::find(const stcServiceTxt* p_pTxt)
{
    stcServiceTxt* pResult = 0;

    for (stcServiceTxt* pTxt = m_pTxts; pTxt; pTxt = pTxt->m_pNext)
    {
        if (p_pTxt == pTxt)
        {
            pResult = pTxt;
            break;
        }
    }
    return pResult;
}

/*
    MDNSResponder::clsHost::stcServiceTxts::length
*/
uint16_t MDNSResponder::clsHost::stcServiceTxts::length(void) const
{
    uint16_t    u16Length = 0;

    stcServiceTxt*  pTxt = m_pTxts;
    while (pTxt)
    {
        u16Length += 1;                 // Length byte
        u16Length += pTxt->length();    // Text
        pTxt = pTxt->m_pNext;
    }
    return u16Length;
}

/*
    MDNSResponder::clsHost::stcServiceTxts::c_strLength

    (incl. closing '\0'). Length bytes place is used for delimiting ';' and closing '\0'
*/
size_t MDNSResponder::clsHost::stcServiceTxts::c_strLength(void) const
{
    return length();
}

/*
    MDNSResponder::clsHost::stcServiceTxts::c_str
*/
bool MDNSResponder::clsHost::stcServiceTxts::c_str(char* p_pcBuffer)
{
    bool bResult = false;

    if (p_pcBuffer)
    {
        bResult = true;

        *p_pcBuffer = 0;
        for (stcServiceTxt* pTxt = m_pTxts; ((bResult) && (pTxt)); pTxt = pTxt->m_pNext)
        {
            size_t  stLength;
            if ((bResult = (0 != (stLength = (pTxt->m_pcKey ? strlen(pTxt->m_pcKey) : 0)))))
            {
                if (pTxt != m_pTxts)
                {
                    *p_pcBuffer++ = ';';
                }
                strncpy(p_pcBuffer, pTxt->m_pcKey, stLength); p_pcBuffer[stLength] = 0;
                p_pcBuffer += stLength;
                *p_pcBuffer++ = '=';
                if ((stLength = (pTxt->m_pcValue ? strlen(pTxt->m_pcValue) : 0)))
                {
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
    MDNSResponder::clsHost::stcServiceTxts::c_str
*/
const char* MDNSResponder::clsHost::stcServiceTxts::c_str(void) const
{

    if ((!m_pcCache) &&
        (m_pTxts) &&
        ((((stcServiceTxts*)this)->m_pcCache = new char[c_strLength()])))   // TRANSPARENT caching
    {
        ((stcServiceTxts*)this)->c_str(m_pcCache);
    }
    return m_pcCache;
}

/*
    MDNSResponder::clsHost::stcServiceTxts::bufferLength

    (incl. closing '\0').
*/
size_t MDNSResponder::clsHost::stcServiceTxts::bufferLength(void) const
{
    return (length() + 1);
}

/*
    MDNSResponder::clsHost::stcServiceTxts::toBuffer
*/
bool MDNSResponder::clsHost::stcServiceTxts::buffer(char* p_pcBuffer)
{
    bool bResult = false;

    if (p_pcBuffer)
    {
        bResult = true;

        *p_pcBuffer = 0;
        for (stcServiceTxt* pTxt = m_pTxts; ((bResult) && (pTxt)); pTxt = pTxt->m_pNext)
        {
            *(unsigned char*)p_pcBuffer++ = pTxt->length();
            size_t  stLength;
            if ((bResult = (0 != (stLength = (pTxt->m_pcKey ? strlen(pTxt->m_pcKey) : 0)))))
            {
                memcpy(p_pcBuffer, pTxt->m_pcKey, stLength);
                p_pcBuffer += stLength;
                *p_pcBuffer++ = '=';
                if ((stLength = (pTxt->m_pcValue ? strlen(pTxt->m_pcValue) : 0)))
                {
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
    MDNSResponder::clsHost::stcServiceTxts::compare
*/
bool MDNSResponder::clsHost::stcServiceTxts::compare(const MDNSResponder::clsHost::stcServiceTxts& p_Other) const
{
    bool    bResult = false;

    if ((bResult = (length() == p_Other.length())))
    {
        // Compare A->B
        for (const stcServiceTxt* pTxt = m_pTxts; ((bResult) && (pTxt)); pTxt = pTxt->m_pNext)
        {
            const stcServiceTxt*    pOtherTxt = p_Other.find(pTxt->m_pcKey);
            bResult = ((pOtherTxt) &&
                       (pTxt->m_pcValue) &&
                       (pOtherTxt->m_pcValue) &&
                       (strlen(pTxt->m_pcValue) == strlen(pOtherTxt->m_pcValue)) &&
                       (0 == strcmp(pTxt->m_pcValue, pOtherTxt->m_pcValue)));
        }
        // Compare B->A
        for (const stcServiceTxt* pOtherTxt = p_Other.m_pTxts; ((bResult) && (pOtherTxt)); pOtherTxt = pOtherTxt->m_pNext)
        {
            const stcServiceTxt*    pTxt = find(pOtherTxt->m_pcKey);
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
    MDNSResponder::clsHost::stcServiceTxts::operator==
*/
bool MDNSResponder::clsHost::stcServiceTxts::operator==(const stcServiceTxts& p_Other) const
{
    return compare(p_Other);
}

/*
    MDNSResponder::clsHost::stcServiceTxts::operator!=
*/
bool MDNSResponder::clsHost::stcServiceTxts::operator!=(const stcServiceTxts& p_Other) const
{
    return !compare(p_Other);
}


/**
    MDNSResponder::clsHost::stcProbeInformation_Base

    Probing status information for a host or service domain

*/

/*
    MDNSResponder::clsHost::stcProbeInformation_Base::stcProbeInformation_Base constructor
*/
MDNSResponder::clsHost::stcProbeInformation_Base::stcProbeInformation_Base(void)
    :   m_ProbingStatus(enuProbingStatus::WaitingForData),
        m_u8SentCount(0),
        m_Timeout(std::numeric_limits<esp8266::polledTimeout::oneShot::timeType>::max()),
        m_bConflict(false),
        m_bTiebreakNeeded(false)
{
}

/*
    MDNSResponder::clsHost::stcProbeInformation_Base::clear
*/
bool MDNSResponder::clsHost::stcProbeInformation_Base::clear(void)
{
    m_ProbingStatus = enuProbingStatus::WaitingForData;
    m_u8SentCount = 0;
    m_Timeout.reset(std::numeric_limits<esp8266::polledTimeout::oneShot::timeType>::max());
    m_bConflict = false;
    m_bTiebreakNeeded = false;

    return true;
}


/**
    MDNSResponder::clsHost::stcProbeInformation_Host

    Probing status information for a host or service domain

*/

/*
    MDNSResponder::clsHost::stcProbeInformation_Host::stcProbeInformation_Host constructor
*/
MDNSResponder::clsHost::stcProbeInformation_Host::stcProbeInformation_Host(void)
    :   m_fnProbeResultCallback(0)
{
}

/*
    MDNSResponder::clsHost::stcProbeInformation_Host::clear
*/
bool MDNSResponder::clsHost::stcProbeInformation_Host::clear(bool p_bClearUserdata /*= false*/)
{
    if (p_bClearUserdata)
    {
        m_fnProbeResultCallback = 0;
    }
    return stcProbeInformation_Base::clear();
}


/**
    MDNSResponder::clsHost::stcProbeInformation_Service

    Probing status information for a host or service domain

*/

/*
    MDNSResponder::clsHost::stcProbeInformation_Service::stcProbeInformation_Service constructor
*/
MDNSResponder::clsHost::stcProbeInformation_Service::stcProbeInformation_Service(void)
    :   m_fnProbeResultCallback(0)
{
}

/*
    MDNSResponder::clsHost::stcProbeInformation_Service::clear
*/
bool MDNSResponder::clsHost::stcProbeInformation_Service::clear(bool p_bClearUserdata /*= false*/)
{
    if (p_bClearUserdata)
    {
        m_fnProbeResultCallback = 0;
    }
    return stcProbeInformation_Base::clear();
}


/**
    MDNSResponder::clsHost::stcService

    A MDNS service object (to be announced by the MDNS responder)
    The service instance may be '\0'; in this case the hostname is used
    and the flag m_bAutoName is set. If the hostname changes, all 'auto-
    named' services are renamed also.
    m_u8Replymask is used while preparing a response to a MDNS query. It is
    resetted in '_sendMDNSMessage' afterwards.
*/

/*
    MDNSResponder::clsHost::stcService::stcService constructor
*/
MDNSResponder::clsHost::stcService::stcService(const char* p_pcName /*= 0*/,
                                               const char* p_pcServiceType /*= 0*/,
                                               const char* p_pcProtocol /*= 0*/)
    :   m_pNext(0),
        m_pcName(0),
        m_bAutoName(false),
        m_pcServiceType(0),
        m_pcProtocol(0),
        m_u16Port(0),
        m_u32ReplyMask(0),
        m_fnTxtCallback(0)
{
    setName(p_pcName);
    setServiceType(p_pcServiceType);
    setProtocol(p_pcProtocol);
}

/*
    MDNSResponder::clsHost::stcService::~stcService destructor
*/
MDNSResponder::clsHost::stcService::~stcService(void)
{
    releaseName();
    releaseServiceType();
    releaseProtocol();
}

/*
    MDNSResponder::clsHost::stcService::setName
*/
bool MDNSResponder::clsHost::stcService::setName(const char* p_pcName)
{
    bool bResult = false;

    releaseName();
    size_t stLength = (p_pcName ? strlen(p_pcName) : 0);
    if (stLength)
    {
        if ((bResult = (0 != (m_pcName = new char[stLength + 1]))))
        {
            strncpy(m_pcName, p_pcName, stLength);
            m_pcName[stLength] = 0;
        }
    }
    else
    {
        bResult = true;
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::stcService::releaseName
*/
bool MDNSResponder::clsHost::stcService::releaseName(void)
{
    if (m_pcName)
    {
        delete[] m_pcName;
        m_pcName = 0;
    }
    return true;
}

/*
    MDNSResponder::clsHost::stcService::setServiceType
*/
bool MDNSResponder::clsHost::stcService::setServiceType(const char* p_pcServiceType)
{
    bool bResult = false;

    releaseServiceType();
    size_t stLength = (p_pcServiceType ? strlen(p_pcServiceType) : 0);
    if (stLength)
    {
        if ((bResult = (0 != (m_pcServiceType = new char[stLength + 1]))))
        {
            strncpy(m_pcServiceType, p_pcServiceType, stLength);
            m_pcServiceType[stLength] = 0;
        }
    }
    else
    {
        bResult = true;
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::stcService::releaseServiceType
*/
bool MDNSResponder::clsHost::stcService::releaseServiceType(void)
{
    if (m_pcServiceType)
    {
        delete[] m_pcServiceType;
        m_pcServiceType = 0;
    }
    return true;
}

/*
    MDNSResponder::clsHost::stcService::setProtocol
*/
bool MDNSResponder::clsHost::stcService::setProtocol(const char* p_pcProtocol)
{
    bool bResult = false;

    releaseProtocol();
    size_t stLength = (p_pcProtocol ? strlen(p_pcProtocol) : 0);
    if (stLength)
    {
        if ((bResult = (0 != (m_pcProtocol = new char[stLength + 1]))))
        {
            strncpy(m_pcProtocol, p_pcProtocol, stLength);
            m_pcProtocol[stLength] = 0;
        }
    }
    else
    {
        bResult = true;
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::stcService::releaseProtocol
*/
bool MDNSResponder::clsHost::stcService::releaseProtocol(void)
{
    if (m_pcProtocol)
    {
        delete[] m_pcProtocol;
        m_pcProtocol = 0;
    }
    return true;
}

/*
    MDNSResponder::clsHost::stcService::probeStatus
*/
bool MDNSResponder::clsHost::stcService::probeStatus(void) const
{
    return (enuProbingStatus::Done == m_ProbeInformation.m_ProbingStatus);
}


/**
    MDNSResponder::clsHost::stcMsgHeader

    A MDNS message haeder.

*/

/*
    MDNSResponder::clsHost::stcMsgHeader::stcMsgHeader
*/
MDNSResponder::clsHost::stcMsgHeader::stcMsgHeader(uint16_t p_u16ID /*= 0*/,
                                                   bool p_bQR /*= false*/,
                                                   uint8_t p_u8Opcode /*= 0*/,
                                                   bool p_bAA /*= false*/,
                                                   bool p_bTC /*= false*/,
                                                   bool p_bRD /*= false*/,
                                                   bool p_bRA /*= false*/,
                                                   uint8_t p_u8RCode /*= 0*/,
                                                   uint16_t p_u16QDCount /*= 0*/,
                                                   uint16_t p_u16ANCount /*= 0*/,
                                                   uint16_t p_u16NSCount /*= 0*/,
                                                   uint16_t p_u16ARCount /*= 0*/)
    :   m_u16ID(p_u16ID),
        m_1bQR(p_bQR), m_4bOpcode(p_u8Opcode), m_1bAA(p_bAA), m_1bTC(p_bTC), m_1bRD(p_bRD),
        m_1bRA(p_bRA), m_3bZ(0), m_4bRCode(p_u8RCode),
        m_u16QDCount(p_u16QDCount),
        m_u16ANCount(p_u16ANCount),
        m_u16NSCount(p_u16NSCount),
        m_u16ARCount(p_u16ARCount)
{
}


/**
    MDNSResponder::clsHost::stcRRDomain

    A MDNS domain object.
    The labels of the domain are stored (DNS-like encoded) in 'm_acName':
    [length byte]varlength label[length byte]varlength label[0]
    'm_u16NameLength' stores the used length of 'm_acName'.
    Dynamic label addition is supported.
    Comparison is supported.
    Export as byte array 'esp8266.local' is supported.

*/

/*
    MDNSResponder::clsHost::stcRRDomain::stcRRDomain constructor
*/
MDNSResponder::clsHost::stcRRDomain::stcRRDomain(void)
    :   m_u16NameLength(0),
        m_pcDecodedName(0)
{
    clear();
}

/*
    MDNSResponder::clsHost::stcRRDomain::stcRRDomain copy-constructor
*/
MDNSResponder::clsHost::stcRRDomain::stcRRDomain(const stcRRDomain& p_Other)
    :   m_u16NameLength(0),
        m_pcDecodedName(0)
{
    operator=(p_Other);
}

/*
    MDNSResponder::clsHost::stcRRDomain::stcRRDomain destructor
*/
MDNSResponder::clsHost::stcRRDomain::~stcRRDomain(void)
{
    clear();
}

/*
    MDNSResponder::clsHost::stcRRDomain::operator =
*/
MDNSResponder::clsHost::stcRRDomain& MDNSResponder::clsHost::stcRRDomain::operator=(const stcRRDomain& p_Other)
{
    if (&p_Other != this)
    {
        clear();
        memcpy(m_acName, p_Other.m_acName, sizeof(m_acName));
        m_u16NameLength = p_Other.m_u16NameLength;
    }
    return *this;
}

/*
    MDNSResponder::clsHost::stcRRDomain::clear
*/
bool MDNSResponder::clsHost::stcRRDomain::clear(void)
{
    memset(m_acName, 0, sizeof(m_acName));
    m_u16NameLength = 0;
    return clearNameCache();
}

/*
    MDNSResponder::clsHost::stcRRDomain::clearNameCache
*/
bool MDNSResponder::clsHost::stcRRDomain::clearNameCache(void)
{
    if (m_pcDecodedName)
    {
        delete[] m_pcDecodedName;
        m_pcDecodedName = 0;
    }
    return true;
}

/*
    MDNSResponder::clsHost::stcRRDomain::addLabel
*/
bool MDNSResponder::clsHost::stcRRDomain::addLabel(const char* p_pcLabel,
                                                   bool p_bPrependUnderline /*= false*/)
{
    bool    bResult = false;

    size_t  stLength = (p_pcLabel
                        ? (strlen(p_pcLabel) + (p_bPrependUnderline ? 1 : 0))
                        : 0);
    if ((MDNS_DOMAIN_LABEL_MAXLENGTH >= stLength) &&
        (MDNS_DOMAIN_MAXLENGTH >= (m_u16NameLength + (1 + stLength))))
    {
        // Length byte
        m_acName[m_u16NameLength] = (unsigned char)stLength;    // Might be 0!
        ++m_u16NameLength;
        // Label
        if (stLength)
        {
            if (p_bPrependUnderline)
            {
                m_acName[m_u16NameLength++] = '_';
                --stLength;
            }
            strncpy(&(m_acName[m_u16NameLength]), p_pcLabel, stLength); m_acName[m_u16NameLength + stLength] = 0;
            m_u16NameLength += stLength;
        }
        bResult = clearNameCache();
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::stcRRDomain::compare
*/
bool MDNSResponder::clsHost::stcRRDomain::compare(const stcRRDomain& p_Other) const
{
    bool    bResult = false;

    if (m_u16NameLength == p_Other.m_u16NameLength)
    {
        const char* pT = m_acName;
        const char* pO = p_Other.m_acName;
        while ((pT) &&
               (pO) &&
               (*((unsigned char*)pT) == *((unsigned char*)pO)) &&                  // Same length AND
               (0 == strncasecmp((pT + 1), (pO + 1), *((unsigned char*)pT))))     // Same content
        {
            if (*((unsigned char*)pT))              // Not 0
            {
                pT += (1 + * ((unsigned char*)pT)); // Shift by length byte and lenght
                pO += (1 + * ((unsigned char*)pO));
            }
            else                                    // Is 0 -> Successfully reached the end
            {
                bResult = true;
                break;
            }
        }
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::stcRRDomain::operator ==
*/
bool MDNSResponder::clsHost::stcRRDomain::operator==(const stcRRDomain& p_Other) const
{
    return compare(p_Other);
}

/*
    MDNSResponder::clsHost::stcRRDomain::operator !=
*/
bool MDNSResponder::clsHost::stcRRDomain::operator!=(const stcRRDomain& p_Other) const
{
    return !compare(p_Other);
}

/*
    MDNSResponder::clsHost::stcRRDomain::operator >
*/
bool MDNSResponder::clsHost::stcRRDomain::operator>(const stcRRDomain& p_Other) const
{
    // TODO: Check, if this is a good idea...
    return !compare(p_Other);
}

/*
    MDNSResponder::clsHost::stcRRDomain::c_strLength
*/
size_t MDNSResponder::clsHost::stcRRDomain::c_strLength(void) const
{
    size_t          stLength = 0;

    unsigned char*  pucLabelLength = (unsigned char*)m_acName;
    while (*pucLabelLength)
    {
        stLength += (*pucLabelLength + 1 /* +1 for '.' or '\0'*/);
        pucLabelLength += (*pucLabelLength + 1);
    }
    return stLength;
}

/*
    MDNSResponder::clsHost::stcRRDomain::c_str (const)
*/
bool MDNSResponder::clsHost::stcRRDomain::c_str(char* p_pcBuffer) const
{
    bool bResult = false;

    if (p_pcBuffer)
    {
        *p_pcBuffer = 0;
        unsigned char* pucLabelLength = (unsigned char*)m_acName;
        while (*pucLabelLength)
        {
            memcpy(p_pcBuffer, (const char*)(pucLabelLength + 1), *pucLabelLength);
            p_pcBuffer += *pucLabelLength;
            pucLabelLength += (*pucLabelLength + 1);
            *p_pcBuffer++ = (*pucLabelLength ? '.' : '\0');
        }
        bResult = true;
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::stcRRDomain::c_str
*/
const char* MDNSResponder::clsHost::stcRRDomain::c_str(void) const
{
    if ((!m_pcDecodedName) &&
        (m_u16NameLength) &&
        ((((stcRRDomain*)this)->m_pcDecodedName = new char[c_strLength()])))   // TRANSPARENT caching
    {
        ((stcRRDomain*)this)->c_str(m_pcDecodedName);
    }
    return m_pcDecodedName;
}


/**
    MDNSResponder::clsHost::stcRRAttributes

    A MDNS attributes object.

*/

/*
    MDNSResponder::clsHost::stcRRAttributes::stcRRAttributes constructor
*/
MDNSResponder::clsHost::stcRRAttributes::stcRRAttributes(uint16_t p_u16Type /*= 0*/,
                                                         uint16_t p_u16Class /*= 1 DNS_RRCLASS_IN Internet*/)
    :   m_u16Type(p_u16Type),
        m_u16Class(p_u16Class)
{
}

/*
    MDNSResponder::clsHost::stcRRAttributes::stcRRAttributes copy-constructor
*/
MDNSResponder::clsHost::stcRRAttributes::stcRRAttributes(const MDNSResponder::clsHost::stcRRAttributes& p_Other)
{
    operator=(p_Other);
}

/*
    MDNSResponder::clsHost::stcRRAttributes::operator =
*/
MDNSResponder::clsHost::stcRRAttributes& MDNSResponder::clsHost::stcRRAttributes::operator=(const MDNSResponder::clsHost::stcRRAttributes& p_Other)
{
    if (&p_Other != this)
    {
        m_u16Type = p_Other.m_u16Type;
        m_u16Class = p_Other.m_u16Class;
    }
    return *this;
}


/**
    MDNSResponder::clsHost::stcRRHeader

    A MDNS record header (domain and attributes) object.

*/

/*
    MDNSResponder::clsHost::stcRRHeader::stcRRHeader constructor
*/
MDNSResponder::clsHost::stcRRHeader::stcRRHeader(void)
{
}

/*
    MDNSResponder::clsHost::stcRRHeader::stcRRHeader copy-constructor
*/
MDNSResponder::clsHost::stcRRHeader::stcRRHeader(const stcRRHeader& p_Other)
{
    operator=(p_Other);
}

/*
    MDNSResponder::clsHost::stcRRHeader::operator =
*/
MDNSResponder::clsHost::stcRRHeader& MDNSResponder::clsHost::stcRRHeader::operator=(const MDNSResponder::clsHost::stcRRHeader& p_Other)
{
    if (&p_Other != this)
    {
        m_Domain = p_Other.m_Domain;
        m_Attributes = p_Other.m_Attributes;
    }
    return *this;
}

/*
    MDNSResponder::clsHost::stcRRHeader::clear
*/
bool MDNSResponder::clsHost::stcRRHeader::clear(void)
{
    m_Domain.clear();
    return true;
}


/**
    MDNSResponder::clsHost::stcRRQuestion

    A MDNS question record object (header + question flags)

*/

/*
    MDNSResponder::clsHost::stcRRQuestion::stcRRQuestion constructor
*/
MDNSResponder::clsHost::stcRRQuestion::stcRRQuestion(void)
    :   m_pNext(0),
        m_bUnicast(false)
{
}


/**
    MDNSResponder::clsHost::stcNSECBitmap

    A MDNS question record object (header + question flags)

*/

/*
    MDNSResponder::clsHost::stcNSECBitmap::stcNSECBitmap constructor
*/
MDNSResponder::clsHost::stcNSECBitmap::stcNSECBitmap(void)
{
    clear();
}

/*
    MDNSResponder::clsHost::stcNSECBitmap::stcNSECBitmap destructor
*/
bool MDNSResponder::clsHost::stcNSECBitmap::clear(void)
{
    memset(m_au8BitmapData, 0, sizeof(m_au8BitmapData));
    return true;
}

/*
    MDNSResponder::clsHost::stcNSECBitmap::length
*/
uint16_t MDNSResponder::clsHost::stcNSECBitmap::length(void) const
{
    return sizeof(m_au8BitmapData); // 6
}

/*
    MDNSResponder::clsHost::stcNSECBitmap::setBit
*/
bool MDNSResponder::clsHost::stcNSECBitmap::setBit(uint16_t p_u16Bit)
{
    bool    bResult = false;

    if ((p_u16Bit) &&
        (length() > (p_u16Bit / 8)))                    // bit between 0..47(2F)
    {

        uint8_t&    ru8Byte = m_au8BitmapData[p_u16Bit / 8];
        uint8_t     u8Flag = 1 << (7 - (p_u16Bit % 8)); // (7 - (0..7)) = 7..0

        ru8Byte |= u8Flag;

        bResult = true;
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::stcNSECBitmap::getBit
*/
bool MDNSResponder::clsHost::stcNSECBitmap::getBit(uint16_t p_u16Bit) const
{
    bool    bResult = false;

    if ((p_u16Bit) &&
        (length() > (p_u16Bit / 8)))                    // bit between 0..47(2F)
    {

        uint8_t u8Byte = m_au8BitmapData[p_u16Bit / 8];
        uint8_t u8Flag = 1 << (7 - (p_u16Bit % 8));     // (7 - (0..7)) = 7..0

        bResult = (u8Byte & u8Flag);
    }
    return bResult;
}


/**
    MDNSResponder::clsHost::stcRRAnswer

    A MDNS answer record object (header + answer content).
    This is a 'virtual' base class for all other MDNS answer classes.

*/

/*
    MDNSResponder::clsHost::stcRRAnswer::stcRRAnswer constructor
*/
MDNSResponder::clsHost::stcRRAnswer::stcRRAnswer(enuAnswerType p_AnswerType,
                                                 const MDNSResponder::clsHost::stcRRHeader& p_Header,
                                                 uint32_t p_u32TTL)
    :   m_pNext(0),
        m_AnswerType(p_AnswerType),
        m_Header(p_Header),
        m_u32TTL(p_u32TTL)
{
    // Extract 'cache flush'-bit
    m_bCacheFlush = (m_Header.m_Attributes.m_u16Class & 0x8000);
    m_Header.m_Attributes.m_u16Class &= (~0x8000);
}

/*
    MDNSResponder::clsHost::stcRRAnswer::~stcRRAnswer destructor
*/
MDNSResponder::clsHost::stcRRAnswer::~stcRRAnswer(void)
{
}

/*
    MDNSResponder::clsHost::stcRRAnswer::answerType
*/
MDNSResponder::clsHost::enuAnswerType MDNSResponder::clsHost::stcRRAnswer::answerType(void) const
{
    return m_AnswerType;
}

/*
    MDNSResponder::clsHost::stcRRAnswer::clear
*/
bool MDNSResponder::clsHost::stcRRAnswer::clear(void)
{
    m_pNext = 0;
    m_Header.clear();
    return true;
}


/**
    MDNSResponder::clsHost::stcRRAnswerA

    A MDNS A answer object.
    Extends the base class by an IPv4 address member.

*/

#ifdef MDNS_IPV4_SUPPORT
/*
    MDNSResponder::clsHost::stcRRAnswerA::stcRRAnswerA constructor
*/
MDNSResponder::clsHost::stcRRAnswerA::stcRRAnswerA(const MDNSResponder::clsHost::stcRRHeader& p_Header,
                                                   uint32_t p_u32TTL)
    :   stcRRAnswer(enuAnswerType::A, p_Header, p_u32TTL),
        m_IPAddress()
{
}

/*
    MDNSResponder::clsHost::stcRRAnswerA::stcRRAnswerA destructor
*/
MDNSResponder::clsHost::stcRRAnswerA::~stcRRAnswerA(void)
{
    clear();
}

/*
    MDNSResponder::clsHost::stcRRAnswerA::clear
*/
bool MDNSResponder::clsHost::stcRRAnswerA::clear(void)
{
    m_IPAddress = IPAddress();
    return true;
}
#endif


/**
    MDNSResponder::clsHost::stcRRAnswerPTR

    A MDNS PTR answer object.
    Extends the base class by a MDNS domain member.

*/

/*
    MDNSResponder::clsHost::stcRRAnswerPTR::stcRRAnswerPTR constructor
*/
MDNSResponder::clsHost::stcRRAnswerPTR::stcRRAnswerPTR(const MDNSResponder::clsHost::stcRRHeader& p_Header,
                                                       uint32_t p_u32TTL)
    :   stcRRAnswer(enuAnswerType::PTR, p_Header, p_u32TTL)
{
}

/*
    MDNSResponder::clsHost::stcRRAnswerPTR::~stcRRAnswerPTR destructor
*/
MDNSResponder::clsHost::stcRRAnswerPTR::~stcRRAnswerPTR(void)
{
    clear();
}

/*
    MDNSResponder::clsHost::stcRRAnswerPTR::clear
*/
bool MDNSResponder::clsHost::stcRRAnswerPTR::clear(void)
{
    m_PTRDomain.clear();
    return true;
}


/**
    MDNSResponder::clsHost::stcRRAnswerTXT

    A MDNS TXT answer object.
    Extends the base class by a MDNS TXT items list member.

*/

/*
    MDNSResponder::clsHost::stcRRAnswerTXT::stcRRAnswerTXT constructor
*/
MDNSResponder::clsHost::stcRRAnswerTXT::stcRRAnswerTXT(const MDNSResponder::clsHost::stcRRHeader& p_Header,
                                                       uint32_t p_u32TTL)
    :   stcRRAnswer(enuAnswerType::TXT, p_Header, p_u32TTL)
{
}

/*
    MDNSResponder::clsHost::stcRRAnswerTXT::~stcRRAnswerTXT destructor
*/
MDNSResponder::clsHost::stcRRAnswerTXT::~stcRRAnswerTXT(void)
{
    clear();
}

/*
    MDNSResponder::clsHost::stcRRAnswerTXT::clear
*/
bool MDNSResponder::clsHost::stcRRAnswerTXT::clear(void)
{
    m_Txts.clear();
    return true;
}


/**
    MDNSResponder::clsHost::stcRRAnswerAAAA

    A MDNS AAAA answer object.
    Extends the base class by an IPv6 address member.

*/

#ifdef MDNS_IPV6_SUPPORT
/*
    MDNSResponder::clsHost::stcRRAnswerAAAA::stcRRAnswerAAAA constructor
*/
MDNSResponder::clsHost::stcRRAnswerAAAA::stcRRAnswerAAAA(const MDNSResponder::clsHost::stcRRHeader& p_Header,
                                                         uint32_t p_u32TTL)
    :   stcRRAnswer(enuAnswerType::AAAA, p_Header, p_u32TTL),
        m_IPAddress()
{
}

/*
    MDNSResponder::clsHost::stcRRAnswerAAAA::~stcRRAnswerAAAA destructor
*/
MDNSResponder::clsHost::stcRRAnswerAAAA::~stcRRAnswerAAAA(void)
{
    clear();
}

/*
    MDNSResponder::clsHost::stcRRAnswerAAAA::clear
*/
bool MDNSResponder::clsHost::stcRRAnswerAAAA::clear(void)
{
    m_IPAddress = IPAddress();
    return true;
}
#endif


/**
    MDNSResponder::clsHost::stcRRAnswerSRV

    A MDNS SRV answer object.
    Extends the base class by a port member.

*/

/*
    MDNSResponder::clsHost::stcRRAnswerSRV::stcRRAnswerSRV constructor
*/
MDNSResponder::clsHost::stcRRAnswerSRV::stcRRAnswerSRV(const MDNSResponder::clsHost::stcRRHeader& p_Header,
                                                       uint32_t p_u32TTL)
    :   stcRRAnswer(enuAnswerType::SRV, p_Header, p_u32TTL),
        m_u16Priority(0),
        m_u16Weight(0),
        m_u16Port(0)
{
}

/*
    MDNSResponder::clsHost::stcRRAnswerSRV::~stcRRAnswerSRV destructor
*/
MDNSResponder::clsHost::stcRRAnswerSRV::~stcRRAnswerSRV(void)
{
    clear();
}

/*
    MDNSResponder::clsHost::stcRRAnswerSRV::clear
*/
bool MDNSResponder::clsHost::stcRRAnswerSRV::clear(void)
{
    m_u16Priority = 0;
    m_u16Weight = 0;
    m_u16Port = 0;
    m_SRVDomain.clear();
    return true;
}


/**
    MDNSResponder::clsHost::stcRRAnswerGeneric

    An unknown (generic) MDNS answer object.
    Extends the base class by a RDATA buffer member.

*/

/*
    MDNSResponder::clsHost::stcRRAnswerGeneric::stcRRAnswerGeneric constructor
*/
MDNSResponder::clsHost::stcRRAnswerGeneric::stcRRAnswerGeneric(const stcRRHeader& p_Header,
                                                               uint32_t p_u32TTL)
    :   stcRRAnswer(enuAnswerType::Generic, p_Header, p_u32TTL),
        m_u16RDLength(0),
        m_pu8RDData(0)
{
}

/*
    MDNSResponder::clsHost::stcRRAnswerGeneric::~stcRRAnswerGeneric destructor
*/
MDNSResponder::clsHost::stcRRAnswerGeneric::~stcRRAnswerGeneric(void)
{
    clear();
}

/*
    MDNSResponder::clsHost::stcRRAnswerGeneric::clear
*/
bool MDNSResponder::clsHost::stcRRAnswerGeneric::clear(void)
{
    if (m_pu8RDData)
    {
        delete[] m_pu8RDData;
        m_pu8RDData = 0;
    }
    m_u16RDLength = 0;

    return true;
}


/**
    MDNSResponder::clsHost::stcSendParameter

    A 'collection' of properties and flags for one MDNS query or response.
    Mainly managed by the 'Control' functions.
    The current offset in the UPD output buffer is tracked to be able to do
    a simple host or service domain compression.

*/

/**
    MDNSResponder::clsHost::stcSendParameter::stcDomainCacheItem

    A cached host or service domain, incl. the offset in the UDP output buffer.

*/

/*
    MDNSResponder::clsHost::stcSendParameter::stcDomainCacheItem::stcDomainCacheItem constructor
*/
MDNSResponder::clsHost::stcSendParameter::stcDomainCacheItem::stcDomainCacheItem(const void* p_pHostNameOrService,
                                                                                 bool p_bAdditionalData,
                                                                                 uint32_t p_u16Offset)
    :   m_pNext(0),
        m_pHostNameOrService(p_pHostNameOrService),
        m_bAdditionalData(p_bAdditionalData),
        m_u16Offset(p_u16Offset)
{
}

/**
    MDNSResponder::clsHost::stcSendParameter
*/

/*
    MDNSResponder::clsHost::stcSendParameter::stcSendParameter constructor
*/
MDNSResponder::clsHost::stcSendParameter::stcSendParameter(void)
    :   m_pQuestions(0),
        m_Response(enuResponseType::None),
        m_pDomainCacheItems(0)
{
    clear();
}

/*
    MDNSResponder::clsHost::stcSendParameter::~stcSendParameter destructor
*/
MDNSResponder::clsHost::stcSendParameter::~stcSendParameter(void)
{
    clear();
}

/*
    MDNSResponder::clsHost::stcSendParameter::clear
*/
bool MDNSResponder::clsHost::stcSendParameter::clear(void)
{
    m_u16ID = 0;
    flushQuestions();
    m_u32HostReplyMask = 0;

    m_bLegacyQuery = false;
    m_Response = enuResponseType::None;
    m_bAuthorative = false;
    m_bCacheFlush = true;
    m_bUnicast = false;
    m_bUnannounce = false;

    m_u16Offset = 0;
    flushDomainCache();
    return true;
}

/*
    MDNSResponder::clsHost::stcSendParameter::flushQuestions
*/
bool MDNSResponder::clsHost::stcSendParameter::flushQuestions(void)
{
    while (m_pQuestions)
    {
        stcRRQuestion* pNext = m_pQuestions->m_pNext;
        delete m_pQuestions;
        m_pQuestions = pNext;
    }
    return true;
}

/*
    MDNSResponder::clsHost::stcSendParameter::flushDomainCache
*/
bool MDNSResponder::clsHost::stcSendParameter::flushDomainCache(void)
{
    while (m_pDomainCacheItems)
    {
        stcDomainCacheItem* pNext = m_pDomainCacheItems->m_pNext;
        delete m_pDomainCacheItems;
        m_pDomainCacheItems = pNext;
    }
    return true;
}

/*
    MDNSResponder::clsHost::stcSendParameter::flushTempContent
*/
bool MDNSResponder::clsHost::stcSendParameter::flushTempContent(void)
{
    m_u16Offset = 0;
    flushDomainCache();
    return true;
}

/*
    MDNSResponder::clsHost::stcSendParameter::shiftOffset
*/
bool MDNSResponder::clsHost::stcSendParameter::shiftOffset(uint16_t p_u16Shift)
{
    m_u16Offset += p_u16Shift;
    return true;
}

/*
    MDNSResponder::clsHost::stcSendParameter::addDomainCacheItem
*/
bool MDNSResponder::clsHost::stcSendParameter::addDomainCacheItem(const void* p_pHostNameOrService,
                                                                  bool p_bAdditionalData,
                                                                  uint16_t p_u16Offset)
{
    bool    bResult = false;

    stcDomainCacheItem* pNewItem = 0;
    if ((p_pHostNameOrService) &&
        (p_u16Offset) &&
        ((pNewItem = new stcDomainCacheItem(p_pHostNameOrService, p_bAdditionalData, p_u16Offset))))
    {

        pNewItem->m_pNext = m_pDomainCacheItems;
        bResult = ((m_pDomainCacheItems = pNewItem));
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::stcSendParameter::findCachedDomainOffset
*/
uint16_t MDNSResponder::clsHost::stcSendParameter::findCachedDomainOffset(const void* p_pHostNameOrService,
                                                                          bool p_bAdditionalData) const
{
    const stcDomainCacheItem*   pCacheItem = m_pDomainCacheItems;

    for (; pCacheItem; pCacheItem = pCacheItem->m_pNext)
    {
        if ((pCacheItem->m_pHostNameOrService == p_pHostNameOrService) &&
            (pCacheItem->m_bAdditionalData == p_bAdditionalData))   // Found cache item
        {
            break;
        }
    }
    return (pCacheItem ? pCacheItem->m_u16Offset : 0);
}


/**
    MDNSResponder::clsHost::stcQuery

    A MDNS service query object.
    Service queries may be static or dynamic.
    As the static service query is processed in the blocking function 'queryService',
    only one static service service may exist. The processing of the answers is done
    on the WiFi-stack side of the ESP stack structure (via 'UDPContext.onRx(_update)').

*/

/**
    MDNSResponder::clsHost::stcQuery::stcAnswer

    One answer for a service query.
    Every answer must contain
    - a service instance entry (pivot),
    and may contain
    - a host domain,
    - a port
    - an IPv4 address
    (- an IPv6 address)
    - a MDNS TXTs
    The existance of a component is flaged in 'm_u32ContentFlags'.
    For every answer component a TTL value is maintained.
    Answer objects can be connected to a linked list.

    For the host domain, service domain and TXTs components, a char array
    representation can be retrieved (which is created on demand).

*/

/**
    MDNSResponder::clsHost::stcQuery::stcAnswer::stcTTL

    The TTL (Time-To-Live) for an specific answer content.
    The 80% and outdated states are calculated based on the current time (millis)
    and the 'set' time (also millis).
    If the answer is scheduled for an update, the corresponding flag should be set.

*/

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::stcTTL::stcTTL constructor
*/
MDNSResponder::clsHost::stcQuery::stcAnswer::stcTTL::stcTTL(void)
    :   m_u32TTL(0),
        m_TTLTimeout(std::numeric_limits<esp8266::polledTimeout::oneShot::timeType>::max()),
        m_TimeoutLevel(static_cast<typeTimeoutLevel>(enuTimeoutLevel::None))
{
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::stcTTL::set
*/
bool MDNSResponder::clsHost::stcQuery::stcAnswer::stcTTL::set(uint32_t p_u32TTL)
{
    m_u32TTL = p_u32TTL;
    if (m_u32TTL)
    {
        m_TimeoutLevel = static_cast<typeTimeoutLevel>(enuTimeoutLevel::Base);  // Set to 80%
        m_TTLTimeout.reset(timeout());
    }
    else
    {
        m_TimeoutLevel = static_cast<typeTimeoutLevel>(enuTimeoutLevel::None);  // undef
        m_TTLTimeout.reset(std::numeric_limits<esp8266::polledTimeout::oneShot::timeType>::max());
    }
    return true;
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::stcTTL::flagged
*/
bool MDNSResponder::clsHost::stcQuery::stcAnswer::stcTTL::flagged(void) const
{
    return ((m_u32TTL) &&
            (static_cast<typeTimeoutLevel>(enuTimeoutLevel::None) != m_TimeoutLevel) &&
            (((esp8266::polledTimeout::timeoutTemplate<false>*)&m_TTLTimeout)->expired())); // Cast-away the const; in case of oneShot-timer OK (but ugly...)
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::stcTTL::restart
*/
bool MDNSResponder::clsHost::stcQuery::stcAnswer::stcTTL::restart(void)
{
    bool    bResult = true;

    if ((static_cast<typeTimeoutLevel>(enuTimeoutLevel::Base) <= m_TimeoutLevel) &&     // >= 80% AND
        (static_cast<typeTimeoutLevel>(enuTimeoutLevel::Final) > m_TimeoutLevel))       // < 100%
    {

        m_TimeoutLevel += static_cast<typeTimeoutLevel>(enuTimeoutLevel::Interval);     // increment by 5%
        m_TTLTimeout.reset(timeout());
    }
    else
    {
        bResult = false;
        m_TTLTimeout.reset(std::numeric_limits<esp8266::polledTimeout::oneShot::timeType>::max());
        m_TimeoutLevel = static_cast<typeTimeoutLevel>(enuTimeoutLevel::None);
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::stcTTL::prepareDeletion
*/
bool MDNSResponder::clsHost::stcQuery::stcAnswer::stcTTL::prepareDeletion(void)
{
    m_TimeoutLevel = static_cast<typeTimeoutLevel>(enuTimeoutLevel::Final);
    m_TTLTimeout.reset(1 * 1000);   // See RFC 6762, 10.1

    return true;
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::stcTTL::finalTimeoutLevel
*/
bool MDNSResponder::clsHost::stcQuery::stcAnswer::stcTTL::finalTimeoutLevel(void) const
{
    return (static_cast<typeTimeoutLevel>(enuTimeoutLevel::Final) == m_TimeoutLevel);
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::stcTTL::timeout
*/
unsigned long MDNSResponder::clsHost::stcQuery::stcAnswer::stcTTL::timeout(void) const
{
    uint32_t    u32Timeout = std::numeric_limits<esp8266::polledTimeout::oneShot::timeType>::max();

    if (static_cast<typeTimeoutLevel>(enuTimeoutLevel::Base) == m_TimeoutLevel)             // 80%
    {
        u32Timeout = (m_u32TTL * 800);                                                      // to milliseconds
    }
    else if ((static_cast<typeTimeoutLevel>(enuTimeoutLevel::Base) < m_TimeoutLevel) &&     // >80% AND
             (static_cast<typeTimeoutLevel>(enuTimeoutLevel::Final) >= m_TimeoutLevel))     // <= 100%
    {

        u32Timeout = (m_u32TTL * 50);
    }   // else: invalid
    return u32Timeout;
}


/**
    MDNSResponder::clsHost::stcQuery::stcAnswer::stcIPAddress

*/

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::stcIPAddress::stcIPAddress constructor
*/
MDNSResponder::clsHost::stcQuery::stcAnswer::stcIPAddress::stcIPAddress(IPAddress p_IPAddress,
                                                                        uint32_t p_u32TTL /*= 0*/)
    :   m_pNext(0),
        m_IPAddress(p_IPAddress)
{
    m_TTL.set(p_u32TTL);
}


/**
    MDNSResponder::clsHost::stcQuery::stcAnswer
*/

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::stcAnswer constructor
*/
MDNSResponder::clsHost::stcQuery::stcAnswer::stcAnswer(void)
    :   m_pNext(0),
        m_u16Port(0),
#ifdef MDNS_IPV4_SUPPORT
        m_pIPv4Addresses(0),
#endif
#ifdef MDNS_IPV6_SUPPORT
        m_pIPv6Addresses(0),
#endif
        m_QueryAnswerFlags(0)
{
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::~stcAnswer destructor
*/
MDNSResponder::clsHost::stcQuery::stcAnswer::~stcAnswer(void)
{
    clear();
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::clear
*/
bool MDNSResponder::clsHost::stcQuery::stcAnswer::clear(void)
{
    return (
#ifdef MDNS_IPV4_SUPPORT
               (releaseIPv4Addresses()) &&
#endif
#ifdef MDNS_IPV6_SUPPORT
               (releaseIPv6Addresses())
#endif
           );
}

#ifdef MDNS_IPV4_SUPPORT
/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::releaseIPv4Addresses
*/
bool MDNSResponder::clsHost::stcQuery::stcAnswer::releaseIPv4Addresses(void)
{
    while (m_pIPv4Addresses)
    {
        stcIPAddress*	pNext = m_pIPv4Addresses->m_pNext;
        delete m_pIPv4Addresses;
        m_pIPv4Addresses = pNext;
    }
    return true;
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::addIPv4Address
*/
bool MDNSResponder::clsHost::stcQuery::stcAnswer::addIPv4Address(MDNSResponder::clsHost::stcQuery::stcAnswer::stcIPAddress* p_pIPv4Address)
{
    bool bResult = false;

    if (p_pIPv4Address)
    {
        p_pIPv4Address->m_pNext = m_pIPv4Addresses;
        m_pIPv4Addresses = p_pIPv4Address;
        bResult = true;
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::removeIPv4Address
*/
bool MDNSResponder::clsHost::stcQuery::stcAnswer::removeIPv4Address(MDNSResponder::clsHost::stcQuery::stcAnswer::stcIPAddress* p_pIPv4Address)
{
    bool    bResult = false;

    if (p_pIPv4Address)
    {
        stcIPAddress*   pPred = m_pIPv4Addresses;
        while ((pPred) &&
               (pPred->m_pNext != p_pIPv4Address))
        {
            pPred = pPred->m_pNext;
        }
        if (pPred)
        {
            pPred->m_pNext = p_pIPv4Address->m_pNext;
            delete p_pIPv4Address;
            bResult = true;
        }
        else if (m_pIPv4Addresses == p_pIPv4Address)     // No predecesor, but first item
        {
            m_pIPv4Addresses = p_pIPv4Address->m_pNext;
            delete p_pIPv4Address;
            bResult = true;
        }
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::findIPv4Address (const)
*/
const MDNSResponder::clsHost::stcQuery::stcAnswer::stcIPAddress* MDNSResponder::clsHost::stcQuery::stcAnswer::findIPv4Address(const IPAddress& p_IPAddress) const
{
    return (stcIPAddress*)(((const stcAnswer*)this)->findIPv4Address(p_IPAddress));
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::findIPv4Address
*/
MDNSResponder::clsHost::stcQuery::stcAnswer::stcIPAddress* MDNSResponder::clsHost::stcQuery::stcAnswer::findIPv4Address(const IPAddress& p_IPAddress)
{
    stcIPAddress*	pIPv4Address = m_pIPv4Addresses;
    while (pIPv4Address)
    {
        if (pIPv4Address->m_IPAddress == p_IPAddress)
        {
            break;
        }
        pIPv4Address = pIPv4Address->m_pNext;
    }
    return pIPv4Address;
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::IPv4AddressCount
*/
uint32_t MDNSResponder::clsHost::stcQuery::stcAnswer::IPv4AddressCount(void) const
{
    uint32_t        u32Count = 0;

    stcIPAddress*   pIPv4Address = m_pIPv4Addresses;
    while (pIPv4Address)
    {
        ++u32Count;
        pIPv4Address = pIPv4Address->m_pNext;
    }
    return u32Count;
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::IPv4AddressAtIndex
*/
MDNSResponder::clsHost::stcQuery::stcAnswer::stcIPAddress* MDNSResponder::clsHost::stcQuery::stcAnswer::IPv4AddressAtIndex(uint32_t p_u32Index)
{
    return (stcIPAddress*)(((const stcAnswer*)this)->IPv4AddressAtIndex(p_u32Index));
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::IPv4AddressAtIndex (const)
*/
const MDNSResponder::clsHost::stcQuery::stcAnswer::stcIPAddress* MDNSResponder::clsHost::stcQuery::stcAnswer::IPv4AddressAtIndex(uint32_t p_u32Index) const
{
    const stcIPAddress*	pIPv4Address = 0;

    if (((uint32_t)(-1) != p_u32Index) &&
        (m_pIPv4Addresses))
    {

        uint32_t    u32Index;
        for (pIPv4Address = m_pIPv4Addresses, u32Index = 0; ((pIPv4Address) && (u32Index < p_u32Index)); pIPv4Address = pIPv4Address->m_pNext, ++u32Index);
    }
    return pIPv4Address;
}
#endif

#ifdef MDNS_IPV6_SUPPORT
/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::releaseIPv6Addresses
*/
bool MDNSResponder::clsHost::stcQuery::stcAnswer::releaseIPv6Addresses(void)
{
    while (m_pIPv6Addresses)
    {
        stcIPAddress*	pNext = m_pIPv6Addresses->m_pNext;
        delete m_pIPv6Addresses;
        m_pIPv6Addresses = pNext;
    }
    return true;
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::addIPv6Address
*/
bool MDNSResponder::clsHost::stcQuery::stcAnswer::addIPv6Address(MDNSResponder::clsHost::stcQuery::stcAnswer::stcIPAddress* p_pIPv6Address)
{
    bool bResult = false;

    if (p_pIPv6Address)
    {
        p_pIPv6Address->m_pNext = m_pIPv6Addresses;
        m_pIPv6Addresses = p_pIPv6Address;
        bResult = true;
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::removeIPv6Address
*/
bool MDNSResponder::clsHost::stcQuery::stcAnswer::removeIPv6Address(MDNSResponder::clsHost::stcQuery::stcAnswer::stcIPAddress* p_pIPv6Address)
{
    bool    bResult = false;

    if (p_pIPv6Address)
    {
        stcIPAddress*	pPred = m_pIPv6Addresses;
        while ((pPred) &&
               (pPred->m_pNext != p_pIPv6Address))
        {
            pPred = pPred->m_pNext;
        }
        if (pPred)
        {
            pPred->m_pNext = p_pIPv6Address->m_pNext;
            delete p_pIPv6Address;
            bResult = true;
        }
        else if (m_pIPv6Addresses == p_pIPv6Address)     // No predecesor, but first item
        {
            m_pIPv6Addresses = p_pIPv6Address->m_pNext;
            delete p_pIPv6Address;
            bResult = true;
        }
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::findIPv6Address
*/
MDNSResponder::clsHost::stcQuery::stcAnswer::stcIPAddress* MDNSResponder::clsHost::stcQuery::stcAnswer::findIPv6Address(const IPAddress& p_IPAddress)
{
    return (stcIPAddress*)(((const stcAnswer*)this)->findIPv6Address(p_IPAddress));
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::findIPv6Address (const)
*/
const MDNSResponder::clsHost::stcQuery::stcAnswer::stcIPAddress* MDNSResponder::clsHost::stcQuery::stcAnswer::findIPv6Address(const IPAddress& p_IPAddress) const
{
    const stcIPAddress*	pIPv6Address = m_pIPv6Addresses;
    while (pIPv6Address)
    {
        if (pIPv6Address->m_IPAddress == p_IPAddress)
        {
            break;
        }
        pIPv6Address = pIPv6Address->m_pNext;
    }
    return pIPv6Address;
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::IPv6AddressCount
*/
uint32_t MDNSResponder::clsHost::stcQuery::stcAnswer::IPv6AddressCount(void) const
{
    uint32_t        u32Count = 0;

    stcIPAddress*	pIPv6Address = m_pIPv6Addresses;
    while (pIPv6Address)
    {
        ++u32Count;
        pIPv6Address = pIPv6Address->m_pNext;
    }
    return u32Count;
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::IPv6AddressAtIndex (const)
*/
const MDNSResponder::clsHost::stcQuery::stcAnswer::stcIPAddress* MDNSResponder::clsHost::stcQuery::stcAnswer::IPv6AddressAtIndex(uint32_t p_u32Index) const
{
    return (stcIPAddress*)(((const stcAnswer*)this)->IPv6AddressAtIndex(p_u32Index));
}

/*
    MDNSResponder::clsHost::stcQuery::stcAnswer::IPv6AddressAtIndex
*/
MDNSResponder::clsHost::stcQuery::stcAnswer::stcIPAddress* MDNSResponder::clsHost::stcQuery::stcAnswer::IPv6AddressAtIndex(uint32_t p_u32Index)
{
    stcIPAddress*	pIPv6Address = 0;

    if (((uint32_t)(-1) != p_u32Index) &&
        (m_pIPv6Addresses))
    {

        uint32_t    u32Index;
        for (pIPv6Address = m_pIPv6Addresses, u32Index = 0; ((pIPv6Address) && (u32Index < p_u32Index)); pIPv6Address = pIPv6Address->m_pNext, ++u32Index);
    }
    return pIPv6Address;
}
#endif


/**
    MDNSResponder::clsHost::stcQuery

    A service query object.
    A static query is flaged via 'm_bLegacyQuery'; while the function 'queryService'
    is waiting for answers, the internal flag 'm_bAwaitingAnswers' is set. When the
    timeout is reached, the flag is removed. These two flags are only used for static
    service queries.
    All answers to the service query are stored in 'm_pAnswers' list.
    Individual answers may be addressed by index (in the list of answers).
    Every time a answer component is added (or changes) in a dynamic service query,
    the callback 'm_fnCallback' is called.
    The answer list may be searched by service and host domain.

    Service query object may be connected to a linked list.
*/

/*
    MDNSResponder::clsHost::stcQuery::stcQuery constructor
*/
MDNSResponder::clsHost::stcQuery::stcQuery(const enuQueryType p_QueryType)
    :   m_pNext(0),
        m_QueryType(p_QueryType),
        m_fnCallback(0),
        m_bLegacyQuery(false),
        m_u8SentCount(0),
        m_ResendTimeout(std::numeric_limits<esp8266::polledTimeout::oneShot::timeType>::max()),
        m_bAwaitingAnswers(true),
        m_pAnswers(0)
{
    clear();
    m_QueryType = p_QueryType;
}

/*
    MDNSResponder::clsHost::stcQuery::~stcQuery destructor
*/
MDNSResponder::clsHost::stcQuery::~stcQuery(void)
{
    clear();
}

/*
    MDNSResponder::clsHost::stcQuery::clear
*/
bool MDNSResponder::clsHost::stcQuery::clear(void)
{
    m_pNext = 0;
    m_QueryType = enuQueryType::None;
    m_fnCallback = 0;
    m_bLegacyQuery = false;
    m_u8SentCount = 0;
    m_ResendTimeout.reset(std::numeric_limits<esp8266::polledTimeout::oneShot::timeType>::max());
    m_bAwaitingAnswers = true;
    while (m_pAnswers)
    {
        stcAnswer*  pNext = m_pAnswers->m_pNext;
        delete m_pAnswers;
        m_pAnswers = pNext;
    }
    return true;
}

/*
    MDNSResponder::clsHost::stcQuery::answerCount
*/
uint32_t MDNSResponder::clsHost::stcQuery::answerCount(void) const
{
    uint32_t    u32Count = 0;

    stcAnswer*  pAnswer = m_pAnswers;
    while (pAnswer)
    {
        ++u32Count;
        pAnswer = pAnswer->m_pNext;
    }
    return u32Count;
}

/*
    MDNSResponder::clsHost::stcQuery::answerAtIndex
*/
const MDNSResponder::clsHost::stcQuery::stcAnswer* MDNSResponder::clsHost::stcQuery::answerAtIndex(uint32_t p_u32Index) const
{
    const stcAnswer*    pAnswer = 0;

    if (((uint32_t)(-1) != p_u32Index) &&
        (m_pAnswers))
    {

        uint32_t    u32Index;
        for (pAnswer = m_pAnswers, u32Index = 0; ((pAnswer) && (u32Index < p_u32Index)); pAnswer = pAnswer->m_pNext, ++u32Index);
    }
    return pAnswer;
}

/*
    MDNSResponder::clsHost::stcQuery::answerAtIndex
*/
MDNSResponder::clsHost::stcQuery::stcAnswer* MDNSResponder::clsHost::stcQuery::answerAtIndex(uint32_t p_u32Index)
{
    return (stcAnswer*)(((const stcQuery*)this)->answerAtIndex(p_u32Index));
}

/*
    MDNSResponder::clsHost::stcQuery::indexOfAnswer
*/
uint32_t MDNSResponder::clsHost::stcQuery::indexOfAnswer(const MDNSResponder::clsHost::stcQuery::stcAnswer* p_pAnswer) const
{
    uint32_t    u32Index = 0;

    for (const stcAnswer* pAnswer = m_pAnswers; pAnswer; pAnswer = pAnswer->m_pNext, ++u32Index)
    {
        if (pAnswer == p_pAnswer)
        {
            return u32Index;
        }
    }
    return ((uint32_t)(-1));
}

/*
    MDNSResponder::clsHost::stcQuery::addAnswer
*/
bool MDNSResponder::clsHost::stcQuery::addAnswer(MDNSResponder::clsHost::stcQuery::stcAnswer* p_pAnswer)
{
    bool    bResult = false;

    if (p_pAnswer)
    {
        p_pAnswer->m_pNext = m_pAnswers;
        m_pAnswers = p_pAnswer;
        bResult = true;
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::stcQuery::removeAnswer
*/
bool MDNSResponder::clsHost::stcQuery::removeAnswer(MDNSResponder::clsHost::stcQuery::stcAnswer* p_pAnswer)
{
    bool    bResult = false;

    if (p_pAnswer)
    {
        stcAnswer*  pPred = m_pAnswers;
        while ((pPred) &&
               (pPred->m_pNext != p_pAnswer))
        {
            pPred = pPred->m_pNext;
        }
        if (pPred)
        {
            pPred->m_pNext = p_pAnswer->m_pNext;
            delete p_pAnswer;
            bResult = true;
        }
        else if (m_pAnswers == p_pAnswer)   // No predecesor, but first item
        {
            m_pAnswers = p_pAnswer->m_pNext;
            delete p_pAnswer;
            bResult = true;
        }
    }
    return bResult;
}

/*
    MDNSResponder::clsHost::stcQuery::findAnswerForServiceDomain
*/
MDNSResponder::clsHost::stcQuery::stcAnswer* MDNSResponder::clsHost::stcQuery::findAnswerForServiceDomain(const MDNSResponder::clsHost::stcRRDomain& p_ServiceDomain)
{
    stcAnswer*  pAnswer = m_pAnswers;
    while (pAnswer)
    {
        if (pAnswer->m_ServiceDomain == p_ServiceDomain)
        {
            break;
        }
        pAnswer = pAnswer->m_pNext;
    }
    return pAnswer;
}

/*
    MDNSResponder::clsHost::stcQuery::findAnswerForHostDomain
*/
MDNSResponder::clsHost::stcQuery::stcAnswer* MDNSResponder::clsHost::stcQuery::findAnswerForHostDomain(const MDNSResponder::clsHost::stcRRDomain& p_HostDomain)
{
    stcAnswer*  pAnswer = m_pAnswers;
    while (pAnswer)
    {
        if (pAnswer->m_HostDomain == p_HostDomain)
        {
            break;
        }
        pAnswer = pAnswer->m_pNext;
    }
    return pAnswer;
}


}   // namespace MDNSImplementation

} // namespace esp8266



