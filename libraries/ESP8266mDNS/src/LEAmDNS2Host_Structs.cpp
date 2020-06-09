/*
    LEAmDNS2Host_Structs.cpp

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

#include <algorithm>

#include "ESP8266mDNS.h"
#include "LEAmDNS2Host.h"
#include "LEAmDNS2_Priv.h"

namespace esp8266
{


namespace experimental
{


/**
    Internal CLASSES & STRUCTS
*/

/**
    clsLEAMDNSHost::clsServiceTxt

    One MDNS TXT item.
    m_pcValue may be '\0'.
    Objects can be chained together (list).
    A 'm_bTemp' flag differentiates between static and dynamic items.
    Output as byte array 'c#=1' is supported.
*/

/*
    clsLEAMDNSHost::clsServiceTxt::clsServiceTxt constructor

*/
clsLEAMDNSHost::clsServiceTxt::clsServiceTxt(const char* p_pcKey /*= 0*/,
        const char* p_pcValue /*= 0*/,
        bool p_bTemp /*= false*/)
    :   m_pcKey(0),
        m_pcValue(0),
        m_bTemp(p_bTemp)
{
    setKey(p_pcKey);
    setValue(p_pcValue);
}

/*
    clsLEAMDNSHost::clsServiceTxt::clsServiceTxt copy-constructor

*/
clsLEAMDNSHost::clsServiceTxt::clsServiceTxt(const clsLEAMDNSHost::clsServiceTxt& p_Other)
    :   m_pcKey(0),
        m_pcValue(0),
        m_bTemp(false)
{
    operator=(p_Other);
}

/*
    clsLEAMDNSHost::clsServiceTxt::~stcServiceTxt destructor

*/
clsLEAMDNSHost::clsServiceTxt::~clsServiceTxt(void)
{
    clear();
}

/*
    clsLEAMDNSHost::clsServiceTxt::operator=

*/
clsLEAMDNSHost::clsServiceTxt& clsLEAMDNSHost::clsServiceTxt::operator=(const clsLEAMDNSHost::clsServiceTxt& p_Other)
{
    if (&p_Other != this)
    {
        clear();
        set(p_Other.m_pcKey, p_Other.m_pcValue, p_Other.m_bTemp);
    }
    return *this;
}

/*
    clsLEAMDNSHost::clsServiceTxt::clear

*/
bool clsLEAMDNSHost::clsServiceTxt::clear(void)
{
    releaseKey();
    releaseValue();
    return true;
}

/*
    clsLEAMDNSHost::clsServiceTxt::allocKey

*/
char* clsLEAMDNSHost::clsServiceTxt::allocKey(size_t p_stLength)
{
    releaseKey();
    if (p_stLength)
    {
        m_pcKey = new char[p_stLength + 1];
    }
    return m_pcKey;
}

/*
    clsLEAMDNSHost::clsServiceTxt::setKey

*/
bool clsLEAMDNSHost::clsServiceTxt::setKey(const char* p_pcKey,
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
    clsLEAMDNSHost::clsServiceTxt::setKey

*/
bool clsLEAMDNSHost::clsServiceTxt::setKey(const char* p_pcKey)
{
    return setKey(p_pcKey, (p_pcKey ? strlen(p_pcKey) : 0));
}

/*
    clsLEAMDNSHost::clsServiceTxt::releaseKey

*/
bool clsLEAMDNSHost::clsServiceTxt::releaseKey(void)
{
    if (m_pcKey)
    {
        delete[] m_pcKey;
        m_pcKey = 0;
    }
    return true;
}

/*
    clsLEAMDNSHost::clsServiceTxt::allocValue

*/
char* clsLEAMDNSHost::clsServiceTxt::allocValue(size_t p_stLength)
{
    releaseValue();
    if (p_stLength)
    {
        m_pcValue = new char[p_stLength + 1];
    }
    return m_pcValue;
}

/*
    clsLEAMDNSHost::clsServiceTxt::setValue

*/
bool clsLEAMDNSHost::clsServiceTxt::setValue(const char* p_pcValue,
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
    else
    {
        // No value -> also OK
        bResult = true;
    }
    return bResult;
}

/*
    clsLEAMDNSHost::clsServiceTxt::setValue

*/
bool clsLEAMDNSHost::clsServiceTxt::setValue(const char* p_pcValue)
{
    return setValue(p_pcValue, (p_pcValue ? strlen(p_pcValue) : 0));
}

/*
    clsLEAMDNSHost::clsServiceTxt::releaseValue

*/
bool clsLEAMDNSHost::clsServiceTxt::releaseValue(void)
{
    if (m_pcValue)
    {
        delete[] m_pcValue;
        m_pcValue = 0;
    }
    return true;
}

/*
    clsLEAMDNSHost::clsServiceTxt::set

*/
bool clsLEAMDNSHost::clsServiceTxt::set(const char* p_pcKey,
                                        const char* p_pcValue,
                                        bool p_bTemp /*= false*/)
{
    m_bTemp = p_bTemp;
    return ((setKey(p_pcKey)) &&
            (setValue(p_pcValue)));
}

/*
    clsLEAMDNSHost::clsServiceTxt::update

*/
bool clsLEAMDNSHost::clsServiceTxt::update(const char* p_pcValue)
{
    return setValue(p_pcValue);
}

/*
    clsLEAMDNSHost::clsServiceTxt::length

    length of eg. 'c#=1' without any closing '\0'

*/
size_t clsLEAMDNSHost::clsServiceTxt::length(void) const
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
    clsLEAMDNSHost::clsServiceTxts

    A list of zero or more MDNS TXT (stcServiceTxt) items.
    Dynamic TXT items can be removed by 'removeTempTxts'.
    A TXT item can be looked up by its 'key' member.
    Export as ';'-separated byte array is supported.
    Export as 'length byte coded' byte array is supported.
    Comparison ((all A TXT items in B and equal) AND (all B TXT items in A and equal)) is supported.

*/

/*
    clsLEAMDNSHost::clsServiceTxts::clsServiceTxts contructor

*/
clsLEAMDNSHost::clsServiceTxts::clsServiceTxts(void)
    :   m_pcCache(0)
{
}

/*
    clsLEAMDNSHost::clsServiceTxts::clsServiceTxts copy-constructor

*/
clsLEAMDNSHost::clsServiceTxts::clsServiceTxts(const clsServiceTxts& p_Other)
    :   m_pcCache(0)
{
    operator=(p_Other);
}

/*
    clsLEAMDNSHost::clsServiceTxts::~stcServiceTxts destructor

*/
clsLEAMDNSHost::clsServiceTxts::~clsServiceTxts(void)
{
    clear();
}

/*
    clsLEAMDNSHost::clsServiceTxts::operator=

*/
clsLEAMDNSHost::clsServiceTxts& clsLEAMDNSHost::clsServiceTxts::operator=(const clsServiceTxts& p_Other)
{
    if (this != &p_Other)
    {
        clear();

        for (const clsServiceTxt* pOtherTxt : p_Other.m_Txts)
        {
            add(new clsServiceTxt(*pOtherTxt));
        }
    }
    return *this;
}

/*
    clsLEAMDNSHost::clsServiceTxts::clear

*/
bool clsLEAMDNSHost::clsServiceTxts::clear(void)
{
    for (clsServiceTxt* pTxt : m_Txts)
    {
        delete pTxt;
    }
    m_Txts.clear();

    return clearCache();
}

/*
    clsLEAMDNSHost::clsServiceTxts::clearCache

*/
bool clsLEAMDNSHost::clsServiceTxts::clearCache(void)
{
    if (m_pcCache)
    {
        delete[] m_pcCache;
        m_pcCache = 0;
    }
    return true;
}

/*
    clsLEAMDNSHost::clsServiceTxts::add

*/
bool clsLEAMDNSHost::clsServiceTxts::add(clsLEAMDNSHost::clsServiceTxt* p_pTxt)
{
    bool bResult = false;

    if (p_pTxt)
    {
        m_Txts.push_back(p_pTxt);
        bResult = true;
    }
    return ((clearCache()) &&
            (bResult));
}

/*
    clsLEAMDNSHost::clsServiceTxts::remove

*/
bool clsLEAMDNSHost::clsServiceTxts::remove(clsServiceTxt* p_pTxt)
{
    bool                            bResult = false;

    clsServiceTxt::list::iterator   it(p_pTxt
                                       ? std::find(m_Txts.begin(), m_Txts.end(), p_pTxt)
                                       : m_Txts.end());
    if (m_Txts.end() != it)
    {
        m_Txts.erase(it);
        delete p_pTxt;

        bResult = true;
    }
    return ((clearCache()) &&
            (bResult));
}

/*
    clsLEAMDNSHost::clsServiceTxts::count

*/
size_t clsLEAMDNSHost::clsServiceTxts::count(void) const
{
    size_t  stResult = m_Txts.size();
    return stResult;
}

/*
    clsLEAMDNSHost::clsServiceTxts::removeTempTxts

*/
bool clsLEAMDNSHost::clsServiceTxts::removeTempTxts(void)
{
    bool            bResult = true;

    // Delete content
    clsServiceTxt::list tempTxts;
    for (clsServiceTxt* pTxt : m_Txts)
    {
        if (pTxt->m_bTemp)
        {
            tempTxts.push_back(pTxt);
            delete pTxt;
        }
    }
    // Remove objects from list
    for (clsServiceTxt* pTempTxt : tempTxts)
    {
        m_Txts.remove(pTempTxt);
    }
    return ((clearCache()) &&
            (bResult));
}

/*
    clsLEAMDNSHost::clsServiceTxts::find

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsServiceTxts::find(const char* p_pcKey)
{
    clsServiceTxt* pResult = 0;

    for (clsServiceTxt* pTxt : m_Txts)
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
    clsLEAMDNSHost::clsServiceTxts::find (const)

*/
const clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsServiceTxts::find(const char* p_pcKey) const
{
    const clsServiceTxt*   pResult = 0;

    for (const clsServiceTxt* pTxt : m_Txts)
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
    clsLEAMDNSHost::clsServiceTxts::find

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsServiceTxts::find(const clsServiceTxt* p_pTxt)
{
    clsServiceTxt* pResult = 0;

    for (clsServiceTxt* pTxt : m_Txts)
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
    clsLEAMDNSHost::clsServiceTxts::length

*/
size_t clsLEAMDNSHost::clsServiceTxts::length(void) const
{
    size_t  szLength = 0;

    for (clsServiceTxt* pTxt : m_Txts)
    {
        szLength += 1;                 // Length byte
        szLength += pTxt->length();    // Text
    }
    return szLength;
}

/*
    clsLEAMDNSHost::clsServiceTxts::c_strLength

    (incl. closing '\0'). Length bytes place is used for delimiting ';' and closing '\0'

*/
size_t clsLEAMDNSHost::clsServiceTxts::c_strLength(void) const
{
    return length();
}

/*
    clsLEAMDNSHost::clsServiceTxts::c_str

*/
bool clsLEAMDNSHost::clsServiceTxts::c_str(char* p_pcBuffer)
{
    bool bResult = false;

    if (p_pcBuffer)
    {
        bResult = true;

        char*   pcCursor = p_pcBuffer;
        *pcCursor = 0;
        for (const clsServiceTxt* pTxt : m_Txts)
        {
            size_t  stLength;
            if ((bResult = (0 != (stLength = (pTxt->m_pcKey ? strlen(pTxt->m_pcKey) : 0)))))
            {
                if (pcCursor != p_pcBuffer)
                {
                    *pcCursor++ = ';';
                }
                strncpy(pcCursor, pTxt->m_pcKey, stLength); pcCursor[stLength] = 0;
                pcCursor += stLength;
                *pcCursor++ = '=';
                if ((stLength = (pTxt->m_pcValue ? strlen(pTxt->m_pcValue) : 0)))
                {
                    strncpy(pcCursor, pTxt->m_pcValue, stLength); pcCursor[stLength] = 0;
                    pcCursor += stLength;
                }
            }
            else
            {
                break;
            }
        }
        *pcCursor++ = 0;
    }
    return bResult;
}

/*
    clsLEAMDNSHost::clsServiceTxts::c_str

*/
const char* clsLEAMDNSHost::clsServiceTxts::c_str(void) const
{

    if ((!m_pcCache) &&
            (m_Txts.size()) &&
            ((((clsServiceTxts*)this)->m_pcCache = new char[c_strLength()])))   // TRANSPARENT caching
    {
        ((clsServiceTxts*)this)->c_str(m_pcCache);
    }
    return m_pcCache;
}

/*
    clsLEAMDNSHost::clsServiceTxts::bufferLength

    (incl. closing '\0').

*/
size_t clsLEAMDNSHost::clsServiceTxts::bufferLength(void) const
{
    return (length() + 1);
}

/*
    clsLEAMDNSHost::clsServiceTxts::toBuffer

*/
bool clsLEAMDNSHost::clsServiceTxts::buffer(char* p_pcBuffer)
{
    bool bResult = false;

    if (p_pcBuffer)
    {
        bResult = true;

        *p_pcBuffer = 0;
        for (const clsServiceTxt* pTxt : m_Txts)
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
            else
            {
                break;
            }
        }
        *p_pcBuffer++ = 0;
    }
    return bResult;
}

/*
    clsLEAMDNSHost::clsServiceTxts::compare

*/
bool clsLEAMDNSHost::clsServiceTxts::compare(const clsLEAMDNSHost::clsServiceTxts& p_Other) const
{
    bool    bResult = false;

    if ((bResult = (length() == p_Other.length())))
    {
        // Compare A->B
        for (const clsServiceTxt* pTxt : m_Txts)
        {
            const clsServiceTxt*    pOtherTxt = p_Other.find(pTxt->m_pcKey);
            if (!((bResult = ((pOtherTxt) &&
                              (pTxt->m_pcValue) &&
                              (pOtherTxt->m_pcValue) &&
                              (strlen(pTxt->m_pcValue) == strlen(pOtherTxt->m_pcValue)) &&
                              (0 == strcmp(pTxt->m_pcValue, pOtherTxt->m_pcValue))))))
            {
                break;
            }
        }
        // Compare B->A
        for (const clsServiceTxt* pOtherTxt : p_Other.m_Txts)
        {
            const clsServiceTxt*    pTxt = find(pOtherTxt->m_pcKey);
            if (!((bResult = ((pTxt) &&
                              (pOtherTxt->m_pcValue) &&
                              (pTxt->m_pcValue) &&
                              (strlen(pOtherTxt->m_pcValue) == strlen(pTxt->m_pcValue)) &&
                              (0 == strcmp(pOtherTxt->m_pcValue, pTxt->m_pcValue))))))
            {
                break;
            }
        }
    }
    return bResult;
}

/*
    clsLEAMDNSHost::clsServiceTxts::operator==

*/
bool clsLEAMDNSHost::clsServiceTxts::operator==(const clsServiceTxts& p_Other) const
{
    return compare(p_Other);
}

/*
    clsLEAMDNSHost::clsServiceTxts::operator!=

*/
bool clsLEAMDNSHost::clsServiceTxts::operator!=(const clsServiceTxts& p_Other) const
{
    return !compare(p_Other);
}


/**
    clsLEAMDNSHost::clsProbeInformation_Base

    Probing status information for a host or service domain

*/

/*
    clsLEAMDNSHost::clsProbeInformation_Base::clsProbeInformation_Base constructor
*/
clsLEAMDNSHost::clsProbeInformation_Base::clsProbeInformation_Base(void)
    :   m_ProbingStatus(enuProbingStatus::WaitingForData),
        m_u8SentCount(0),
        m_Timeout(std::numeric_limits<esp8266::polledTimeout::oneShot::timeType>::max()),
        m_bConflict(false),
        m_bTiebreakNeeded(false)
{
}

/*
    clsLEAMDNSHost::clsProbeInformation_Base::clear
*/
bool clsLEAMDNSHost::clsProbeInformation_Base::clear(void)
{
    m_ProbingStatus = enuProbingStatus::WaitingForData;
    m_u8SentCount = 0;
    m_Timeout.reset(std::numeric_limits<esp8266::polledTimeout::oneShot::timeType>::max());
    m_bConflict = false;
    m_bTiebreakNeeded = false;

    return true;
}


/**
    clsLEAMDNSHost::clsProbeInformation_Host

    Probing status information for a host or service domain

*/

/*
    clsLEAMDNSHost::clsProbeInformation::clsProbeInformation constructor
*/
clsLEAMDNSHost::clsProbeInformation::clsProbeInformation(void)
    :   m_fnProbeResultCallback(0)
{
}

/*
    clsLEAMDNSHost::clsProbeInformation::clear
*/
bool clsLEAMDNSHost::clsProbeInformation::clear(bool p_bClearUserdata /*= false*/)
{
    if (p_bClearUserdata)
    {
        m_fnProbeResultCallback = 0;
    }
    return clsProbeInformation_Base::clear();
}


/**
    clsLEAMDNSHost::clsService::clsProbeInformation

    Probing status information for a host or service domain

*/

/*
    clsLEAMDNSHost::clsService::clsProbeInformation::clsProbeInformation constructor
*/
clsLEAMDNSHost::clsService::clsProbeInformation::clsProbeInformation(void)
    :   m_fnProbeResultCallback(0)
{
}

/*
    clsLEAMDNSHost::clsService::clsProbeInformation::clear
*/
bool clsLEAMDNSHost::clsService::clsProbeInformation::clear(bool p_bClearUserdata /*= false*/)
{
    if (p_bClearUserdata)
    {
        m_fnProbeResultCallback = 0;
    }
    return clsProbeInformation_Base::clear();
}


/**
    clsLEAMDNSHost::clsService

    A MDNS service object (to be announced by the MDNS responder)
    The service instance may be '\0'; in this case the hostname is used
    and the flag m_bAutoName is set. If the hostname changes, all 'auto-
    named' services are renamed also.
    m_u8Replymask is used while preparing a response to a MDNS query. It is
    resetted in '_sendMDNSMessage' afterwards.
*/

/*
    clsLEAMDNSHost::clsService::clsService constructor

*/
clsLEAMDNSHost::clsService::clsService(void)
    :   m_pcInstanceName(0),
        m_bAutoName(false),
        m_pcType(0),
        m_pcProtocol(0),
        m_u16Port(0),
        m_u32ReplyMask(0),
        m_fnTxtCallback(0)
{
}

/*
    clsLEAMDNSHost::clsService::~clsService destructor

*/
clsLEAMDNSHost::clsService::~clsService(void)
{
    _releaseInstanceName();
    _releaseType();
    _releaseProtocol();
}

/*
    clsLEAMDNSHost::clsService::setInstanceName

*/
bool clsLEAMDNSHost::clsService::setInstanceName(const char* p_pcInstanceName)
{
    bool bResult = false;

    _releaseInstanceName();
    size_t stLength = (p_pcInstanceName ? strlen(p_pcInstanceName) : 0);
    if ((stLength) &&
            (stLength <= clsConsts::stDomainLabelMaxLength))
    {
        if ((bResult = (0 != (m_pcInstanceName = new char[stLength + 1]))))
        {
            strncpy(m_pcInstanceName, p_pcInstanceName, stLength);
            m_pcInstanceName[stLength] = 0;

            _resetProbeStatus();
        }
    }
    return bResult;
}

/*
    clsLEAMDNSHost::clsService::indexInstanceName

*/
bool clsLEAMDNSHost::clsService::indexInstanceName(void)
{
    bool    bResult = false;

    if ((bResult = setInstanceName(clsLEAMDNSHost::indexDomainName(m_pcInstanceName, "#", 0))))
    {
        _resetProbeStatus();
    }
    return bResult;
}

/*
    clsLEAMDNSHost::clsService::instanceName

*/
const char* clsLEAMDNSHost::clsService::instanceName(void) const
{
    return m_pcInstanceName;
}

/*
    clsLEAMDNSHost::clsService::_releaseInstanceName

*/
bool clsLEAMDNSHost::clsService::_releaseInstanceName(void)
{
    if (m_pcInstanceName)
    {
        delete[] m_pcInstanceName;
        m_pcInstanceName = 0;
    }
    return true;
}

/*
    clsLEAMDNSHost::clsService::setType

*/
bool clsLEAMDNSHost::clsService::setType(const char* p_pcType)
{
    bool bResult = false;

    _releaseType();
    size_t stLength = (p_pcType ? strlen(p_pcType) : 0);
    if ((stLength) &&
            (stLength <= clsConsts::stServiceTypeMaxLength))
    {
        if ((bResult = (0 != (m_pcType = new char[stLength + 1]))))
        {
            strncpy(m_pcType, p_pcType, stLength);
            m_pcType[stLength] = 0;

            _resetProbeStatus();
        }
    }
    return bResult;
}

/*
    clsLEAMDNSHost::clsService::type

*/
const char* clsLEAMDNSHost::clsService::type(void) const
{
    return m_pcType;
}

/*
    clsLEAMDNSHost::clsService::_releaseType

*/
bool clsLEAMDNSHost::clsService::_releaseType(void)
{
    if (m_pcType)
    {
        delete[] m_pcType;
        m_pcType = 0;
    }
    return true;
}

/*
    clsLEAMDNSHost::clsService::setProtocol

*/
bool clsLEAMDNSHost::clsService::setProtocol(const char* p_pcProtocol)
{
    bool bResult = false;

    _releaseProtocol();
    size_t stLength = (p_pcProtocol ? strlen(p_pcProtocol) : 0);
    if ((stLength) &&
            (stLength <= clsConsts::stServiceProtocolMaxLength))
    {
        if ((bResult = (0 != (m_pcProtocol = new char[stLength + 1]))))
        {
            strncpy(m_pcProtocol, p_pcProtocol, stLength);
            m_pcProtocol[stLength] = 0;

            _resetProbeStatus();
        }
    }
    return bResult;
}

/*
    clsLEAMDNSHost::clsService::protocol

*/
const char* clsLEAMDNSHost::clsService::protocol(void) const
{
    return m_pcProtocol;
}

/*
    clsLEAMDNSHost::clsService::_releaseProtocol

*/
bool clsLEAMDNSHost::clsService::_releaseProtocol(void)
{
    if (m_pcProtocol)
    {
        delete[] m_pcProtocol;
        m_pcProtocol = 0;
    }
    return true;
}

/*
    clsLEAMDNSHost::clsService::setPort

*/
bool clsLEAMDNSHost::clsService::setPort(uint16_t p_u16Port)
{
    bool bResult = false;

    if ((bResult = (0 != p_u16Port)))
    {
        m_u16Port = p_u16Port;

        _resetProbeStatus();
    }
    return bResult;
}

/*
    clsLEAMDNSHost::clsService::port

*/
uint16_t clsLEAMDNSHost::clsService::port(void) const
{
    return m_u16Port;
}

/*
    clsLEAMDNSHost::clsService::setProbeResultCallback

*/
bool clsLEAMDNSHost::clsService::setProbeResultCallback(fnProbeResultCallback p_fnProbeResultCallback)
{
    m_ProbeInformation.m_fnProbeResultCallback = p_fnProbeResultCallback;
    return true;
}

/*
    clsLEAMDNSHost::clsService::probeStatus

*/
bool clsLEAMDNSHost::clsService::probeStatus(void) const
{
    return (clsProbeInformation_Base::enuProbingStatus::DoneFinally == m_ProbeInformation.m_ProbingStatus);
}

/*
    clsLEAMDNSHost::clsService::_resetProbeStatus

*/
void clsLEAMDNSHost::clsService::_resetProbeStatus(void)
{
    m_ProbeInformation.clear(false);
    m_ProbeInformation.m_ProbingStatus = clsProbeInformation_Base::enuProbingStatus::ReadyToStart;
}

/*
    clsLEAMDNSHost::clsService::addServiceTxt (const char*)

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsService::addServiceTxt(const char* p_pcKey,
        const char* p_pcValue)
{
    return _addServiceTxt(p_pcKey, p_pcValue, false);
}

/*
    clsLEAMDNSHost::clsService::addServiceTxt (uint32_t)

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsService::addServiceTxt(const char* p_pcKey,
        uint32_t p_u32Value)
{
    return _addServiceTxt(p_pcKey, p_u32Value, false);
}

/*
    clsLEAMDNSHost::clsService::addServiceTxt (uint16_t)

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsService::addServiceTxt(const char* p_pcKey,
        uint16_t p_u16Value)
{
    return _addServiceTxt(p_pcKey, p_u16Value, false);
}

/*
    clsLEAMDNSHost::clsService::addServiceTxt (uint8_t)

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsService::addServiceTxt(const char* p_pcKey,
        uint8_t p_u8Value)
{
    return _addServiceTxt(p_pcKey, p_u8Value, false);
}

/*
    clsLEAMDNSHost::clsService::addServiceTxt (int32_t)

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsService::addServiceTxt(const char* p_pcKey,
        int32_t p_i32Value)
{
    return _addServiceTxt(p_pcKey, p_i32Value, false);
}

/*
    clsLEAMDNSHost::clsService::addServiceTxt (int16_t)

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsService::addServiceTxt(const char* p_pcKey,
        int16_t p_i16Value)
{
    return _addServiceTxt(p_pcKey, p_i16Value, false);
}

/*
    clsLEAMDNSHost::clsService::addServiceTxt (int8_t)

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsService::addServiceTxt(const char* p_pcKey,
        int8_t p_i8Value)
{
    return _addServiceTxt(p_pcKey, p_i8Value, false);
}

/*
    clsLEAMDNSHost::clsService::addDynamicServiceTxt (const char*)

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsService::addDynamicServiceTxt(const char* p_pcKey,
        const char* p_pcValue)
{
    return _addServiceTxt(p_pcKey, p_pcValue, true);
}

/*
    clsLEAMDNSHost::clsService::addDynamicServiceTxt (uint32_t)

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsService::addDynamicServiceTxt(const char* p_pcKey,
        uint32_t p_u32Value)
{
    return _addServiceTxt(p_pcKey, p_u32Value, true);
}

/*
    clsLEAMDNSHost::clsService::addDynamicServiceTxt (uint16_t)

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsService::addDynamicServiceTxt(const char* p_pcKey,
        uint16_t p_u16Value)
{
    return _addServiceTxt(p_pcKey, p_u16Value, true);
}

/*
    clsLEAMDNSHost::clsService::addDynamicServiceTxt (uint8_t)

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsService::addDynamicServiceTxt(const char* p_pcKey,
        uint8_t p_u8Value)
{
    return _addServiceTxt(p_pcKey, p_u8Value, true);
}

/*
    clsLEAMDNSHost::clsService::addDynamicServiceTxt (int32_t)

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsService::addDynamicServiceTxt(const char* p_pcKey,
        int32_t p_i32Value)
{
    return _addServiceTxt(p_pcKey, p_i32Value, true);
}

/*
    clsLEAMDNSHost::clsService::addDynamicServiceTxt (int16_t)

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsService::addDynamicServiceTxt(const char* p_pcKey,
        int16_t p_i16Value)
{
    return _addServiceTxt(p_pcKey, p_i16Value, true);
}

/*
    clsLEAMDNSHost::clsService::addDynamicServiceTxt (int8_t)

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsService::addDynamicServiceTxt(const char* p_pcKey,
        int8_t p_i8Value)
{
    return _addServiceTxt(p_pcKey, p_i8Value, true);
}

/*
    clsLEAMDNSHost::clsService::setDynamicServiceTxtCallback

*/
bool clsLEAMDNSHost::clsService::setDynamicServiceTxtCallback(fnDynamicServiceTxtCallback p_fnCallback)
{
    m_fnTxtCallback = p_fnCallback;
    return true;
}

/*
    clsLEAMDNSHost::clsService::_addServiceTxt (const char*)

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsService::_addServiceTxt(const char* p_pcKey,
        const char* p_pcValue,
        bool p_bTemp)
{
    clsServiceTxt*  pServiceTxt = 0;

    if ((p_pcKey) &&
            (*p_pcKey))
    {
        if ((pServiceTxt = m_Txts.find(p_pcKey)))
        {
            // Change existing TXT
            if (clsConsts::stServiceTxtMaxLength > (m_Txts.length() -
                                                    (pServiceTxt->m_pcValue ? strlen(pServiceTxt->m_pcValue) : 0) +
                                                    (p_pcValue ? strlen(p_pcValue) : 0)))
            {
                // Enough space left for changed content
                if (!pServiceTxt->update(p_pcValue))
                {
                    // FAILED to update
                    DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[LEAmDNS2_Host] clsService::_addServiceTxt: FAILED to update TXT item '%s'!\n"), p_pcKey));
                    pServiceTxt = 0;
                }
            }
            else
            {
                // NOT enough space for changed TXT content
                DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[LEAmDNS2_Host] clsService::_addServiceTxt: FAILED to change TXT item '%s' (too large)!\n"), p_pcKey));
                pServiceTxt = 0;
            }
        }
        else
        {
            // Create new TXT
            if (clsConsts::stServiceTxtMaxLength > (m_Txts.length() +
                                                    1 +                                // Length byte
                                                    (p_pcKey ? strlen(p_pcKey) : 0) +
                                                    1 +                                // '='
                                                    (p_pcValue ? strlen(p_pcValue) : 0)))
            {
                if (!(((pServiceTxt = new clsServiceTxt)) &&
                        (pServiceTxt->set(p_pcKey, p_pcValue, p_bTemp)) &&
                        (m_Txts.add(pServiceTxt))))
                {
                    DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[LEAmDNS2_Host] clsService::_addServiceTxt: FAILED to add TXT item '%s'!\n"), p_pcKey));
                    if (pServiceTxt)
                    {
                        delete pServiceTxt;
                        pServiceTxt = 0;
                    }
                }
            }
            else
            {
                // NOT enough space for added TXT item
                DEBUG_EX_ERR(DEBUG_OUTPUT.printf_P(PSTR("[LEAmDNS2_Host] clsService::_addServiceTxt: FAILED to add TXT item '%s' (too large)!\n"), p_pcKey));
                pServiceTxt = 0;
            }
        }
    }
    return pServiceTxt;
}

/*
    clsLEAMDNSHost::clsService::_addServiceTxt (uint32_t)

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsService::_addServiceTxt(const char* p_pcKey,
        uint32_t p_u32Value,
        bool p_bTemp)
{
    char    acValueBuffer[16];  // 32-bit max 10 digits
    *acValueBuffer = 0;
    sprintf(acValueBuffer, "%u", p_u32Value);

    return _addServiceTxt(p_pcKey, acValueBuffer, p_bTemp);
}

/*
    clsLEAMDNSHost::clsService::_addServiceTxt (uint16_t)

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsService::_addServiceTxt(const char* p_pcKey,
        uint16_t p_u16Value,
        bool p_bTemp)
{
    char    acValueBuffer[8];   // 16-bit max 5 digits
    *acValueBuffer = 0;
    sprintf(acValueBuffer, "%hu", p_u16Value);

    return _addServiceTxt(p_pcKey, acValueBuffer, p_bTemp);
}

/*
    clsLEAMDNSHost::clsService::_addServiceTxt (uint8_t)

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsService::_addServiceTxt(const char* p_pcKey,
        uint8_t p_u8Value,
        bool p_bTemp)
{
    char    acValueBuffer[8];   // 8-bit max 3 digits
    *acValueBuffer = 0;
    sprintf(acValueBuffer, "%hhu", p_u8Value);

    return _addServiceTxt(p_pcKey, acValueBuffer, p_bTemp);
}

/*
    clsLEAMDNSHost::clsService::_addServiceTxt (int32_t)

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsService::_addServiceTxt(const char* p_pcKey,
        int32_t p_i32Value,
        bool p_bTemp)
{
    char    acValueBuffer[16];  // 32-bit max 10 digits
    *acValueBuffer = 0;
    sprintf(acValueBuffer, "%i", p_i32Value);

    return _addServiceTxt(p_pcKey, acValueBuffer, p_bTemp);
}

/*
    clsLEAMDNSHost::clsService::_addServiceTxt (int16_t)

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsService::_addServiceTxt(const char* p_pcKey,
        int16_t p_i16Value,
        bool p_bTemp)
{
    char    acValueBuffer[8];   // 16-bit max 5 digits
    *acValueBuffer = 0;
    sprintf(acValueBuffer, "%hi", p_i16Value);

    return _addServiceTxt(p_pcKey, acValueBuffer, p_bTemp);
}

/*
    clsLEAMDNSHost::clsService::_addServiceTxt (int8_t)

*/
clsLEAMDNSHost::clsServiceTxt* clsLEAMDNSHost::clsService::_addServiceTxt(const char* p_pcKey,
        int8_t p_i8Value,
        bool p_bTemp)
{
    char    acValueBuffer[8];   // 8-bit max 3 digits
    *acValueBuffer = 0;
    sprintf(acValueBuffer, "%hhi", p_i8Value);

    return _addServiceTxt(p_pcKey, acValueBuffer, p_bTemp);
}


/**
    clsLEAMDNSHost::clsMsgHeader

    A MDNS message header.

*/

/*
    clsLEAMDNSHost::clsMsgHeader::clsMsgHeader

*/
clsLEAMDNSHost::clsMsgHeader::clsMsgHeader(uint16_t p_u16ID /*= 0*/,
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
    clsLEAMDNSHost::clsRRDomain

    A MDNS domain object.
    The labels of the domain are stored (DNS-like encoded) in 'm_acName':
    [length byte]varlength label[length byte]varlength label[0]
    'm_u16NameLength' stores the used length of 'm_acName'.
    Dynamic label addition is supported.
    Comparison is supported.
    Export as byte array 'esp8266.local' is supported.

*/

/*
    clsLEAMDNSHost::clsRRDomain::clsRRDomain constructor

*/
clsLEAMDNSHost::clsRRDomain::clsRRDomain(void)
    :   m_u16NameLength(0),
        m_pcDecodedName(0)
{
    clear();
}

/*
    clsLEAMDNSHost::clsRRDomain::clsRRDomain copy-constructor

*/
clsLEAMDNSHost::clsRRDomain::clsRRDomain(const clsRRDomain& p_Other)
    :   m_u16NameLength(0),
        m_pcDecodedName(0)
{
    operator=(p_Other);
}

/*
    clsLEAMDNSHost::clsRRDomain::clsRRDomain destructor

*/
clsLEAMDNSHost::clsRRDomain::~clsRRDomain(void)
{
    clear();
}

/*
    clsLEAMDNSHost::clsRRDomain::operator =

*/
clsLEAMDNSHost::clsRRDomain& clsLEAMDNSHost::clsRRDomain::operator=(const clsRRDomain& p_Other)
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
    clsLEAMDNSHost::clsRRDomain::clear

*/
bool clsLEAMDNSHost::clsRRDomain::clear(void)
{
    memset(m_acName, 0, sizeof(m_acName));
    m_u16NameLength = 0;
    return clearNameCache();
}

/*
    clsLEAMDNSHost::clsRRDomain::clearNameCache

*/
bool clsLEAMDNSHost::clsRRDomain::clearNameCache(void)
{
    if (m_pcDecodedName)
    {
        delete[] m_pcDecodedName;
        m_pcDecodedName = 0;
    }
    return true;
}

/*
    clsLEAMDNSHost::clsRRDomain::addLabel

*/
bool clsLEAMDNSHost::clsRRDomain::addLabel(const char* p_pcLabel,
        bool p_bPrependUnderline /*= false*/)
{
    bool    bResult = false;

    size_t  stLength = (p_pcLabel
                        ? (strlen(p_pcLabel) + (p_bPrependUnderline ? 1 : 0))
                        : 0);
    if ((clsConsts::stDomainLabelMaxLength >= stLength) &&
            (clsConsts::stDomainMaxLength >= (m_u16NameLength + (1 + stLength))))
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
    clsLEAMDNSHost::clsRRDomain::compare

*/
bool clsLEAMDNSHost::clsRRDomain::compare(const clsRRDomain& p_Other) const
{
    bool    bResult = false;

    if (m_u16NameLength == p_Other.m_u16NameLength)
    {
        const char* pT = m_acName;
        const char* pO = p_Other.m_acName;
        while ((pT) &&
                (pO) &&
                (*((unsigned char*)pT) == *((unsigned char*)pO)) &&              // Same length AND
                (0 == strncasecmp((pT + 1), (pO + 1), *((unsigned char*)pT))))   // Same content
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
    clsLEAMDNSHost::clsRRDomain::operator ==

*/
bool clsLEAMDNSHost::clsRRDomain::operator==(const clsRRDomain& p_Other) const
{
    return compare(p_Other);
}

/*
    clsLEAMDNSHost::clsRRDomain::operator !=

*/
bool clsLEAMDNSHost::clsRRDomain::operator!=(const clsRRDomain& p_Other) const
{
    return !compare(p_Other);
}

/*
    clsLEAMDNSHost::clsRRDomain::operator >

*/
bool clsLEAMDNSHost::clsRRDomain::operator>(const clsRRDomain& p_Other) const
{
    // TODO: Check, if this is a good idea...
    return !compare(p_Other);
}

/*
    clsLEAMDNSHost::clsRRDomain::c_strLength

*/
size_t clsLEAMDNSHost::clsRRDomain::c_strLength(void) const
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
    clsLEAMDNSHost::clsRRDomain::c_str (const)

*/
bool clsLEAMDNSHost::clsRRDomain::c_str(char* p_pcBuffer) const
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
    clsLEAMDNSHost::clsRRDomain::c_str

*/
const char* clsLEAMDNSHost::clsRRDomain::c_str(void) const
{
    if ((!m_pcDecodedName) &&
            (m_u16NameLength) &&
            ((((clsRRDomain*)this)->m_pcDecodedName = new char[c_strLength()])))   // TRANSPARENT caching
    {
        ((clsRRDomain*)this)->c_str(m_pcDecodedName);
    }
    return m_pcDecodedName;
}


/**
    clsLEAMDNSHost::clsRRAttributes

    A MDNS attributes object.

*/

/*
    clsLEAMDNSHost::clsRRAttributes::clsRRAttributes constructor

*/
clsLEAMDNSHost::clsRRAttributes::clsRRAttributes(uint16_t p_u16Type /*= 0*/,
        uint16_t p_u16Class /*= 1 DNS_RRCLASS_IN Internet*/)
    :   m_u16Type(p_u16Type),
        m_u16Class(p_u16Class)
{
}

/*
    clsLEAMDNSHost::clsRRAttributes::clsRRAttributes copy-constructor

*/
clsLEAMDNSHost::clsRRAttributes::clsRRAttributes(const clsLEAMDNSHost::clsRRAttributes& p_Other)
{
    operator=(p_Other);
}

/*
    clsLEAMDNSHost::clsRRAttributes::operator =

*/
clsLEAMDNSHost::clsRRAttributes& clsLEAMDNSHost::clsRRAttributes::operator=(const clsLEAMDNSHost::clsRRAttributes& p_Other)
{
    if (&p_Other != this)
    {
        m_u16Type = p_Other.m_u16Type;
        m_u16Class = p_Other.m_u16Class;
    }
    return *this;
}


/**
    clsLEAMDNSHost::clsRRHeader

    A MDNS record header (domain and attributes) object.

*/

/*
    clsLEAMDNSHost::clsRRHeader::clsRRHeader constructor

*/
clsLEAMDNSHost::clsRRHeader::clsRRHeader(void)
{
}

/*
    clsLEAMDNSHost::clsRRHeader::clsRRHeader copy-constructor

*/
clsLEAMDNSHost::clsRRHeader::clsRRHeader(const clsRRHeader& p_Other)
{
    operator=(p_Other);
}

/*
    clsLEAMDNSHost::clsRRHeader::operator =

*/
clsLEAMDNSHost::clsRRHeader& clsLEAMDNSHost::clsRRHeader::operator=(const clsLEAMDNSHost::clsRRHeader& p_Other)
{
    if (&p_Other != this)
    {
        m_Domain = p_Other.m_Domain;
        m_Attributes = p_Other.m_Attributes;
    }
    return *this;
}

/*
    clsLEAMDNSHost::clsRRHeader::clear

*/
bool clsLEAMDNSHost::clsRRHeader::clear(void)
{
    m_Domain.clear();
    return true;
}


/**
    clsLEAMDNSHost::clsRRQuestion

    A MDNS question record object (header + question flags)

*/

/*
    clsLEAMDNSHost::clsRRQuestion::clsRRQuestion constructor
*/
clsLEAMDNSHost::clsRRQuestion::clsRRQuestion(void)
    :   m_bUnicast(false)
{
}


/**
    clsLEAMDNSHost::clsNSECBitmap

    A MDNS question record object (header + question flags)

*/

/*
    clsLEAMDNSHost::clsNSECBitmap::clsNSECBitmap constructor

*/
clsLEAMDNSHost::clsNSECBitmap::clsNSECBitmap(void)
{
    clear();
}

/*
    clsLEAMDNSHost::clsNSECBitmap::clsNSECBitmap destructor

*/
bool clsLEAMDNSHost::clsNSECBitmap::clear(void)
{
    memset(m_au8BitmapData, 0, sizeof(m_au8BitmapData));
    return true;
}

/*
    clsLEAMDNSHost::clsNSECBitmap::length

*/
uint16_t clsLEAMDNSHost::clsNSECBitmap::length(void) const
{
    return sizeof(m_au8BitmapData); // 6
}

/*
    clsLEAMDNSHost::clsNSECBitmap::setBit

*/
bool clsLEAMDNSHost::clsNSECBitmap::setBit(uint16_t p_u16Bit)
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
    clsLEAMDNSHost::clsNSECBitmap::getBit

*/
bool clsLEAMDNSHost::clsNSECBitmap::getBit(uint16_t p_u16Bit) const
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
    clsLEAMDNSHost::clsRRAnswer

    A MDNS answer record object (header + answer content).
    This is a 'virtual' base class for all other MDNS answer classes.

*/

/*
    clsLEAMDNSHost::clsRRAnswer::clsRRAnswer constructor

*/
clsLEAMDNSHost::clsRRAnswer::clsRRAnswer(enuAnswerType p_AnswerType,
        const clsLEAMDNSHost::clsRRHeader& p_Header,
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
    clsLEAMDNSHost::clsRRAnswer::~stcRRAnswer destructor

*/
clsLEAMDNSHost::clsRRAnswer::~clsRRAnswer(void)
{
}

/*
    clsLEAMDNSHost::clsRRAnswer::answerType

*/
clsLEAMDNSHost::enuAnswerType clsLEAMDNSHost::clsRRAnswer::answerType(void) const
{
    return m_AnswerType;
}

/*
    clsLEAMDNSHost::clsRRAnswer::clear

*/
bool clsLEAMDNSHost::clsRRAnswer::clear(void)
{
    m_pNext = 0;
    m_Header.clear();
    return true;
}


/**
    clsLEAMDNSHost::clsRRAnswerA

    A MDNS A answer object.
    Extends the base class by an IPv4 address member.

*/

#ifdef MDNS_IPV4_SUPPORT
/*
    clsLEAMDNSHost::clsRRAnswerA::clsRRAnswerA constructor

*/
clsLEAMDNSHost::clsRRAnswerA::clsRRAnswerA(const clsLEAMDNSHost::clsRRHeader& p_Header,
        uint32_t p_u32TTL)
    :   clsRRAnswer(enuAnswerType::A, p_Header, p_u32TTL),
        m_IPAddress()
{
}

/*
    clsLEAMDNSHost::clsRRAnswerA::clsRRAnswerA destructor

*/
clsLEAMDNSHost::clsRRAnswerA::~clsRRAnswerA(void)
{
    clear();
}

/*
    clsLEAMDNSHost::clsRRAnswerA::clear

*/
bool clsLEAMDNSHost::clsRRAnswerA::clear(void)
{
    m_IPAddress = IPAddress();
    return true;
}
#endif


/**
    clsLEAMDNSHost::clsRRAnswerPTR

    A MDNS PTR answer object.
    Extends the base class by a MDNS domain member.

*/

/*
    clsLEAMDNSHost::clsRRAnswerPTR::clsRRAnswerPTR constructor

*/
clsLEAMDNSHost::clsRRAnswerPTR::clsRRAnswerPTR(const clsLEAMDNSHost::clsRRHeader& p_Header,
        uint32_t p_u32TTL)
    :   clsRRAnswer(enuAnswerType::PTR, p_Header, p_u32TTL)
{
}

/*
    clsLEAMDNSHost::clsRRAnswerPTR::~stcRRAnswerPTR destructor

*/
clsLEAMDNSHost::clsRRAnswerPTR::~clsRRAnswerPTR(void)
{
    clear();
}

/*
    clsLEAMDNSHost::clsRRAnswerPTR::clear

*/
bool clsLEAMDNSHost::clsRRAnswerPTR::clear(void)
{
    m_PTRDomain.clear();
    return true;
}


/**
    clsLEAMDNSHost::clsRRAnswerTXT

    A MDNS TXT answer object.
    Extends the base class by a MDNS TXT items list member.

*/

/*
    clsLEAMDNSHost::clsRRAnswerTXT::clsRRAnswerTXT constructor

*/
clsLEAMDNSHost::clsRRAnswerTXT::clsRRAnswerTXT(const clsLEAMDNSHost::clsRRHeader& p_Header,
        uint32_t p_u32TTL)
    :   clsRRAnswer(enuAnswerType::TXT, p_Header, p_u32TTL)
{
}

/*
    clsLEAMDNSHost::clsRRAnswerTXT::~stcRRAnswerTXT destructor

*/
clsLEAMDNSHost::clsRRAnswerTXT::~clsRRAnswerTXT(void)
{
    clear();
}

/*
    clsLEAMDNSHost::clsRRAnswerTXT::clear

*/
bool clsLEAMDNSHost::clsRRAnswerTXT::clear(void)
{
    m_Txts.clear();
    return true;
}


/**
    clsLEAMDNSHost::clsRRAnswerAAAA

    A MDNS AAAA answer object.
    Extends the base class by an IPv6 address member.

*/

#ifdef MDNS_IPV6_SUPPORT
/*
    clsLEAMDNSHost::clsRRAnswerAAAA::clsRRAnswerAAAA constructor

*/
clsLEAMDNSHost::clsRRAnswerAAAA::clsRRAnswerAAAA(const clsLEAMDNSHost::clsRRHeader& p_Header,
        uint32_t p_u32TTL)
    :   clsRRAnswer(enuAnswerType::AAAA, p_Header, p_u32TTL),
        m_IPAddress()
{
}

/*
    clsLEAMDNSHost::clsRRAnswerAAAA::~stcRRAnswerAAAA destructor

*/
clsLEAMDNSHost::clsRRAnswerAAAA::~clsRRAnswerAAAA(void)
{
    clear();
}

/*
    clsLEAMDNSHost::clsRRAnswerAAAA::clear

*/
bool clsLEAMDNSHost::clsRRAnswerAAAA::clear(void)
{
    m_IPAddress = IPAddress();
    return true;
}
#endif


/**
    clsLEAMDNSHost::clsRRAnswerSRV

    A MDNS SRV answer object.
    Extends the base class by a port member.

*/

/*
    clsLEAMDNSHost::clsRRAnswerSRV::clsRRAnswerSRV constructor

*/
clsLEAMDNSHost::clsRRAnswerSRV::clsRRAnswerSRV(const clsLEAMDNSHost::clsRRHeader& p_Header,
        uint32_t p_u32TTL)
    :   clsRRAnswer(enuAnswerType::SRV, p_Header, p_u32TTL),
        m_u16Priority(0),
        m_u16Weight(0),
        m_u16Port(0)
{
}

/*
    clsLEAMDNSHost::clsRRAnswerSRV::~stcRRAnswerSRV destructor

*/
clsLEAMDNSHost::clsRRAnswerSRV::~clsRRAnswerSRV(void)
{
    clear();
}

/*
    clsLEAMDNSHost::clsRRAnswerSRV::clear

*/
bool clsLEAMDNSHost::clsRRAnswerSRV::clear(void)
{
    m_u16Priority = 0;
    m_u16Weight = 0;
    m_u16Port = 0;
    m_SRVDomain.clear();
    return true;
}


/**
    clsLEAMDNSHost::clsRRAnswerGeneric

    An unknown (generic) MDNS answer object.
    Extends the base class by a RDATA buffer member.

*/

/*
    clsLEAMDNSHost::clsRRAnswerGeneric::clsRRAnswerGeneric constructor

*/
clsLEAMDNSHost::clsRRAnswerGeneric::clsRRAnswerGeneric(const clsRRHeader& p_Header,
        uint32_t p_u32TTL)
    :   clsRRAnswer(enuAnswerType::Generic, p_Header, p_u32TTL),
        m_u16RDLength(0),
        m_pu8RDData(0)
{
}

/*
    clsLEAMDNSHost::clsRRAnswerGeneric::~stcRRAnswerGeneric destructor

*/
clsLEAMDNSHost::clsRRAnswerGeneric::~clsRRAnswerGeneric(void)
{
    clear();
}

/*
    clsLEAMDNSHost::clsRRAnswerGeneric::clear

*/
bool clsLEAMDNSHost::clsRRAnswerGeneric::clear(void)
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
    clsLEAMDNSHost::clsSendParameter

    A 'collection' of properties and flags for one MDNS query or response.
    Mainly managed by the 'Control' functions.
    The current offset in the UPD output buffer is tracked to be able to do
    a simple host or service domain compression.

*/

/**
    clsLEAMDNSHost::clsSendParameter::clsDomainCacheItem

    A cached host or service domain, incl. the offset in the UDP output buffer.

*/

/*
    clsLEAMDNSHost::clsSendParameter::clsDomainCacheItem::clsDomainCacheItem constructor

*/
clsLEAMDNSHost::clsSendParameter::clsDomainCacheItem::clsDomainCacheItem(const void* p_pHostNameOrService,
        bool p_bAdditionalData,
        uint32_t p_u16Offset)
    :   m_pHostNameOrService(p_pHostNameOrService),
        m_bAdditionalData(p_bAdditionalData),
        m_u16Offset(p_u16Offset)
{
}

/**
    clsLEAMDNSHost::clsSendParameter

*/

/*
    clsLEAMDNSHost::clsSendParameter::clsSendParameter constructor

*/
clsLEAMDNSHost::clsSendParameter::clsSendParameter(void)
    :   m_u16ID(0),
        m_u32HostReplyMask(0),
        m_bLegacyDNSQuery(false),
        m_Response(enuResponseType::None),
        m_bAuthorative(false),
        m_bCacheFlush(false),
        m_bUnicast(false),
        m_bUnannounce(false),
        m_u16Offset(0)
{
    clear();
}

/*
    clsLEAMDNSHost::clsSendParameter::~stcSendParameter destructor

*/
clsLEAMDNSHost::clsSendParameter::~clsSendParameter(void)
{
    clear();
}

/*
    clsLEAMDNSHost::clsSendParameter::clear

*/
bool clsLEAMDNSHost::clsSendParameter::clear(void)
{
    m_u16ID = 0;
    flushQuestions();
    m_u32HostReplyMask = 0;

    m_bLegacyDNSQuery = false;
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
    clsLEAMDNSHost::clsSendParameter::flushQuestions

*/
bool clsLEAMDNSHost::clsSendParameter::flushQuestions(void)
{
    for (clsRRQuestion* pRRQuestion : m_RRQuestions)
    {
        delete pRRQuestion;
    }
    m_RRQuestions.clear();
    return true;
}

/*
    clsLEAMDNSHost::clsSendParameter::flushDomainCache

*/
bool clsLEAMDNSHost::clsSendParameter::flushDomainCache(void)
{
    for (clsDomainCacheItem* pDomainCacheItem : m_DomainCacheItems)
    {
        delete pDomainCacheItem;
    }
    m_DomainCacheItems.clear();
    return true;
}

/*
    clsLEAMDNSHost::clsSendParameter::flushTempContent

*/
bool clsLEAMDNSHost::clsSendParameter::flushTempContent(void)
{
    m_u16Offset = 0;
    flushDomainCache();
    return true;
}

/*
    clsLEAMDNSHost::clsSendParameter::shiftOffset

*/
bool clsLEAMDNSHost::clsSendParameter::shiftOffset(uint16_t p_u16Shift)
{
    m_u16Offset += p_u16Shift;
    return true;
}

/*
    clsLEAMDNSHost::clsSendParameter::addDomainCacheItem

*/
bool clsLEAMDNSHost::clsSendParameter::addDomainCacheItem(const void* p_pHostNameOrService,
        bool p_bAdditionalData,
        uint16_t p_u16Offset)
{
    bool    bResult = false;

    clsDomainCacheItem* pNewItem = 0;
    if ((p_pHostNameOrService) &&
            (p_u16Offset) &&
            ((pNewItem = new clsDomainCacheItem(p_pHostNameOrService, p_bAdditionalData, p_u16Offset))))
    {
        m_DomainCacheItems.push_back(pNewItem);
        bResult = true;
    }
    return bResult;
}

/*
    clsLEAMDNSHost::clsSendParameter::findCachedDomainOffset

*/
uint16_t clsLEAMDNSHost::clsSendParameter::findCachedDomainOffset(const void* p_pHostNameOrService,
        bool p_bAdditionalData) const
{
    const clsDomainCacheItem*   pMatchingCacheItem = 0;

    for (const clsDomainCacheItem* pCacheItem : m_DomainCacheItems)
    {
        if ((pCacheItem->m_pHostNameOrService == p_pHostNameOrService) &&
                (pCacheItem->m_bAdditionalData == p_bAdditionalData))   // Found cache item
        {
            pMatchingCacheItem = pCacheItem;
            break;
        }
    }
    return (pMatchingCacheItem ? pMatchingCacheItem->m_u16Offset : 0);
}


/**
    clsLEAMDNSHost::clsQuery

    A MDNS service query object.
    Service queries may be static or dynamic.
    As the static service query is processed in the blocking function 'queryService',
    only one static service service may exist. The processing of the answers is done
    on the WiFi-stack side of the ESP stack structure (via 'UDPContext.onRx(_update)').

*/

/**
    clsLEAMDNSHost::clsQuery::clsAnswer

    One answer for a query.
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
    clsLEAMDNSHost::clsQuery::clsAnswer::clsTTL

    The TTL (Time-To-Live) for an specific answer content.
    If the answer is scheduled for an update, the corresponding flag should be set.

*/

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::clsTTL::clsTTL constructor

*/
clsLEAMDNSHost::clsQuery::clsAnswer::clsTTL::clsTTL(void)
    :   m_u32TTL(0),
        m_TTLTimeout(std::numeric_limits<esp8266::polledTimeout::oneShot::timeType>::max()),
        m_TimeoutLevel(static_cast<typeTimeoutLevel>(enuTimeoutLevel::None))
{
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::clsTTL::set

*/
bool clsLEAMDNSHost::clsQuery::clsAnswer::clsTTL::set(uint32_t p_u32TTL)
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
    clsLEAMDNSHost::clsQuery::clsAnswer::clsTTL::flagged

*/
bool clsLEAMDNSHost::clsQuery::clsAnswer::clsTTL::flagged(void) const
{
    return ((m_u32TTL) &&
            (static_cast<typeTimeoutLevel>(enuTimeoutLevel::None) != m_TimeoutLevel) &&
            (((esp8266::polledTimeout::timeoutTemplate<false>*)&m_TTLTimeout)->expired())); // Cast-away the const; in case of oneShot-timer OK (but ugly...)
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::clsTTL::restart

*/
bool clsLEAMDNSHost::clsQuery::clsAnswer::clsTTL::restart(void)
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
    clsLEAMDNSHost::clsQuery::clsAnswer::clsTTL::prepareDeletion

*/
bool clsLEAMDNSHost::clsQuery::clsAnswer::clsTTL::prepareDeletion(void)
{
    m_TimeoutLevel = static_cast<typeTimeoutLevel>(enuTimeoutLevel::Final);
    m_TTLTimeout.reset(1 * 1000);   // See RFC 6762, 10.1

    return true;
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::clsTTL::finalTimeoutLevel

*/
bool clsLEAMDNSHost::clsQuery::clsAnswer::clsTTL::finalTimeoutLevel(void) const
{
    return (static_cast<typeTimeoutLevel>(enuTimeoutLevel::Final) == m_TimeoutLevel);
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::clsTTL::timeout

*/
unsigned long clsLEAMDNSHost::clsQuery::clsAnswer::clsTTL::timeout(void) const
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
    clsLEAMDNSHost::clsQuery::clsAnswer::clsIPAddress

*/

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::clsIPAddress::clsIPAddress constructor

*/
clsLEAMDNSHost::clsQuery::clsAnswer::clsIPAddressWithTTL::clsIPAddressWithTTL(IPAddress p_IPAddress,
        uint32_t p_u32TTL /*= 0*/)
    :   m_IPAddress(p_IPAddress)
{
    m_TTL.set(p_u32TTL);
}


/**
    clsLEAMDNSHost::clsQuery::clsAnswer

*/

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::clsAnswer constructor

*/
clsLEAMDNSHost::clsQuery::clsAnswer::clsAnswer(void)
    :   m_u16Port(0),
        m_QueryAnswerFlags(0)
{
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::~clsAnswer destructor

*/
clsLEAMDNSHost::clsQuery::clsAnswer::~clsAnswer(void)
{
    clear();
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::clear

*/
bool clsLEAMDNSHost::clsQuery::clsAnswer::clear(void)
{
    return (
#ifdef MDNS_IPV4_SUPPORT
               (releaseIPv4Addresses())
#else
               (true)
#endif
               &&
#ifdef MDNS_IPV6_SUPPORT
               (releaseIPv6Addresses())
#else
               (true)
#endif
           );
}

#ifdef MDNS_IPV4_SUPPORT
/*
    clsLEAMDNSHost::clsQuery::clsAnswer::releaseIPv4Addresses

*/
bool clsLEAMDNSHost::clsQuery::clsAnswer::releaseIPv4Addresses(void)
{
    for (clsIPAddressWithTTL* pIPAddress : m_IPv4Addresses)
    {
        delete pIPAddress;
    }
    m_IPv4Addresses.clear();
    return true;
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::addIPv4Address

*/
bool clsLEAMDNSHost::clsQuery::clsAnswer::addIPv4Address(clsLEAMDNSHost::clsQuery::clsAnswer::clsIPAddressWithTTL* p_pIPv4Address)
{
    bool bResult = false;

    if (p_pIPv4Address)
    {
        m_IPv4Addresses.push_back(p_pIPv4Address);
        bResult = true;
    }
    return bResult;
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::removeIPv4Address

*/
bool clsLEAMDNSHost::clsQuery::clsAnswer::removeIPv4Address(clsLEAMDNSHost::clsQuery::clsAnswer::clsIPAddressWithTTL* p_pIPv4Address)
{
    bool    bResult = false;

    clsIPAddressWithTTL::list::iterator    it(p_pIPv4Address
            ? std::find(m_IPv4Addresses.begin(), m_IPv4Addresses.end(), p_pIPv4Address)
            : m_IPv4Addresses.end());
    if (m_IPv4Addresses.end() != it)
    {
        m_IPv4Addresses.erase(it);
        delete p_pIPv4Address;

        bResult = true;
    }
    return bResult;
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::findIPv4Address (const)

*/
const clsLEAMDNSHost::clsQuery::clsAnswer::clsIPAddressWithTTL* clsLEAMDNSHost::clsQuery::clsAnswer::findIPv4Address(const IPAddress& p_IPAddress) const
{
    return (clsIPAddressWithTTL*)(((const clsAnswer*)this)->findIPv4Address(p_IPAddress));
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::findIPv4Address

*/
clsLEAMDNSHost::clsQuery::clsAnswer::clsIPAddressWithTTL* clsLEAMDNSHost::clsQuery::clsAnswer::findIPv4Address(const IPAddress& p_IPAddress)
{
    clsIPAddressWithTTL*   pMatchingIPv4Address = 0;

    for (clsIPAddressWithTTL* pIPv4Address : m_IPv4Addresses)
    {
        if (pIPv4Address->m_IPAddress == p_IPAddress)
        {
            pMatchingIPv4Address = pIPv4Address;
            break;
        }
    }
    return pMatchingIPv4Address;
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::IPv4AddressCount

*/
uint32_t clsLEAMDNSHost::clsQuery::clsAnswer::IPv4AddressCount(void) const
{
    uint32_t        u32Count = m_IPv4Addresses.size();
    return u32Count;
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::IPv4AddressAtIndex

*/
clsLEAMDNSHost::clsQuery::clsAnswer::clsIPAddressWithTTL* clsLEAMDNSHost::clsQuery::clsAnswer::IPv4AddressAtIndex(uint32_t p_u32Index)
{
    return (clsIPAddressWithTTL*)(((const clsAnswer*)this)->IPv4AddressAtIndex(p_u32Index));
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::IPv4AddressAtIndex (const)

*/
const clsLEAMDNSHost::clsQuery::clsAnswer::clsIPAddressWithTTL* clsLEAMDNSHost::clsQuery::clsAnswer::IPv4AddressAtIndex(uint32_t p_u32Index) const
{
    const clsIPAddressWithTTL*	pIPv4AddressAtIndex = 0;

    uint32_t    u32CurIndex = 0;
    for (clsIPAddressWithTTL::list::const_iterator it = m_IPv4Addresses.begin();
            (((uint32_t)(-1) != p_u32Index) && (u32CurIndex <= p_u32Index) && (it != m_IPv4Addresses.end()));
            it++, u32CurIndex++)
    {
        if (p_u32Index == u32CurIndex++)
        {
            pIPv4AddressAtIndex = *it;
            break;
        }
    }
    return pIPv4AddressAtIndex;
}
#endif

#ifdef MDNS_IPV6_SUPPORT
/*
    clsLEAMDNSHost::clsQuery::clsAnswer::releaseIPv6Addresses

*/
bool clsLEAMDNSHost::clsQuery::clsAnswer::releaseIPv6Addresses(void)
{
    for (clsIPAddressWithTTL* pIPAddress : m_IPv6Addresses)
    {
        delete pIPAddress;
    }
    m_IPv6Addresses.clear();
    return true;
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::addIPv6Address

*/
bool clsLEAMDNSHost::clsQuery::clsAnswer::addIPv6Address(clsLEAMDNSHost::clsQuery::clsAnswer::clsIPAddressWithTTL* p_pIPv6Address)
{
    bool bResult = false;

    if (p_pIPv6Address)
    {
        m_IPv6Addresses.push_back(p_pIPv6Address);
        bResult = true;
    }
    return bResult;
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::removeIPv6Address

*/
bool clsLEAMDNSHost::clsQuery::clsAnswer::removeIPv6Address(clsLEAMDNSHost::clsQuery::clsAnswer::clsIPAddressWithTTL* p_pIPv6Address)
{
    bool    bResult = false;

    clsIPAddressWithTTL::list::iterator    it(p_pIPv6Address
            ? std::find(m_IPv6Addresses.begin(), m_IPv6Addresses.end(), p_pIPv6Address)
            : m_IPv6Addresses.end());
    if (m_IPv6Addresses.end() != it)
    {
        m_IPv6Addresses.erase(it);
        delete p_pIPv6Address;

        bResult = true;
    }
    return bResult;
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::findIPv6Address

*/
clsLEAMDNSHost::clsQuery::clsAnswer::clsIPAddressWithTTL* clsLEAMDNSHost::clsQuery::clsAnswer::findIPv6Address(const IPAddress& p_IPAddress)
{
    return (clsIPAddressWithTTL*)(((const clsAnswer*)this)->findIPv6Address(p_IPAddress));
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::findIPv6Address (const)

*/
const clsLEAMDNSHost::clsQuery::clsAnswer::clsIPAddressWithTTL* clsLEAMDNSHost::clsQuery::clsAnswer::findIPv6Address(const IPAddress& p_IPAddress) const
{
    clsIPAddressWithTTL*   pMatchingIPv6Address = 0;

    for (clsIPAddressWithTTL* pIPv6Address : m_IPv6Addresses)
    {
        if (pIPv6Address->m_IPAddress == p_IPAddress)
        {
            pMatchingIPv6Address = pIPv6Address;
            break;
        }
    }
    return pMatchingIPv6Address;
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::IPv6AddressCount

*/
uint32_t clsLEAMDNSHost::clsQuery::clsAnswer::IPv6AddressCount(void) const
{
    uint32_t        u32Count = m_IPv6Addresses.size();
    return u32Count;
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::IPv6AddressAtIndex

*/
clsLEAMDNSHost::clsQuery::clsAnswer::clsIPAddressWithTTL* clsLEAMDNSHost::clsQuery::clsAnswer::IPv6AddressAtIndex(uint32_t p_u32Index)
{
    return (clsIPAddressWithTTL*)(((const clsAnswer*)this)->IPv6AddressAtIndex(p_u32Index));
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswer::IPv6AddressAtIndex (const)

*/
const clsLEAMDNSHost::clsQuery::clsAnswer::clsIPAddressWithTTL* clsLEAMDNSHost::clsQuery::clsAnswer::IPv6AddressAtIndex(uint32_t p_u32Index) const
{
    const clsIPAddressWithTTL* pIPv6AddressAtIndex = 0;

    uint32_t    u32CurIndex = 0;
    for (clsIPAddressWithTTL::list::const_iterator it = m_IPv6Addresses.begin();
            (((uint32_t)(-1) != p_u32Index) && (u32CurIndex <= p_u32Index) && (it != m_IPv6Addresses.end()));
            it++, u32CurIndex++)
    {
        if (p_u32Index == u32CurIndex++)
        {
            pIPv6AddressAtIndex = *it;
            break;
        }
    }
    return pIPv6AddressAtIndex;
}
#endif


/**
    clsLEAMDNSHost::clsQuery::clsAnswerAccessor

*/

/*
    clsLEAMDNSHost::clsQuery::clsAnswerAccessor::clsAnswerAccessor constructor

*/
clsLEAMDNSHost::clsQuery::clsAnswerAccessor::clsAnswerAccessor(const clsLEAMDNSHost::clsQuery::clsAnswer* p_pAnswer)
    :   m_pAnswer(p_pAnswer)
{
    if ((m_pAnswer) &&
            (txtsAvailable()))
    {
        // Prepare m_TxtKeyValueMap
        for (const clsLEAMDNSHost::clsServiceTxt* pTxt : m_pAnswer->m_Txts.m_Txts)
        {
            m_TxtKeyValueMap.emplace(std::pair<const char*, const char*>(pTxt->m_pcKey, pTxt->m_pcValue));
        }
    }
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswerAccessor::~clsAnswerAccessor destructor
*/
clsLEAMDNSHost::clsQuery::clsAnswerAccessor::~clsAnswerAccessor(void)
{
}

/**
    clsLEAMDNSHost::clsQuery::clsAnswerAccessor::clsCompareTxtKey

*/

/*
    clsLEAMDNSHost::clsQuery::clsAnswerAccessor::stcCompareTxtKey::operator()

*/
bool clsLEAMDNSHost::clsQuery::clsAnswerAccessor::stcCompareTxtKey::operator()(char const* p_pA,
        char const* p_pB) const
{
    return (0 > strcasecmp(p_pA, p_pB));
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswerAccessor::serviceDomainAvailable

*/
bool clsLEAMDNSHost::clsQuery::clsAnswerAccessor::serviceDomainAvailable(void) const
{
    return ((m_pAnswer) &&
            (m_pAnswer->m_QueryAnswerFlags & static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::ServiceDomain)));
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswerAccessor::serviceDomain

*/
const char* clsLEAMDNSHost::clsQuery::clsAnswerAccessor::serviceDomain(void) const
{
    return ((m_pAnswer)
            ?   m_pAnswer->m_ServiceDomain.c_str()
            : 0);
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswerAccessor::hostDomainAvailable

*/
bool clsLEAMDNSHost::clsQuery::clsAnswerAccessor::hostDomainAvailable(void) const
{
    return ((m_pAnswer) &&
            (m_pAnswer->m_QueryAnswerFlags & static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::HostDomain)));
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswerAccessor::hostDomain

*/
const char* clsLEAMDNSHost::clsQuery::clsAnswerAccessor::hostDomain(void) const
{
    return ((m_pAnswer)
            ?   m_pAnswer->m_HostDomain.c_str()
            : 0);
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswerAccessor::hostPortAvailable

*/
bool clsLEAMDNSHost::clsQuery::clsAnswerAccessor::hostPortAvailable(void) const
{
    return ((m_pAnswer) &&
            (m_pAnswer->m_QueryAnswerFlags & static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::Port)));
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswerAccessor::hostPort

*/
uint16_t clsLEAMDNSHost::clsQuery::clsAnswerAccessor::hostPort(void) const
{
    return ((m_pAnswer)
            ? (m_pAnswer->m_u16Port)
            : 0);
}

#ifdef MDNS_IPV4_SUPPORT
/*
    clsLEAMDNSHost::clsQuery::clsAnswerAccessor::IPv4AddressAvailable

*/
bool clsLEAMDNSHost::clsQuery::clsAnswerAccessor::IPv4AddressAvailable(void) const
{
    return ((m_pAnswer) &&
            (m_pAnswer->m_QueryAnswerFlags & static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::IPv4Address)));
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswerAccessor::IPv4Addresses

*/
clsLEAMDNSHost::clsQuery::clsAnswerAccessor::clsIPAddressVector clsLEAMDNSHost::clsQuery::clsAnswerAccessor::IPv4Addresses(void) const
{
    clsIPAddressVector  internalIP;
    if ((m_pAnswer) &&
            (IPv4AddressAvailable()))
    {
        for (uint32_t u = 0; u < m_pAnswer->IPv4AddressCount(); ++u)
        {
            const clsLEAMDNSHost::clsQuery::clsAnswer::clsIPAddressWithTTL*    pIPAddr = m_pAnswer->IPv4AddressAtIndex(u);
            if (pIPAddr)
            {
                internalIP.emplace_back(pIPAddr->m_IPAddress);
            }
        }
    }
    return internalIP;
}
#endif

#ifdef MDNS_IPV6_SUPPORT
/*
    clsLEAMDNSHost::clsQuery::clsAnswerAccessor::IPv6AddressAvailable

*/
bool clsLEAMDNSHost::clsQuery::clsAnswerAccessor::IPv6AddressAvailable(void) const
{
    return ((m_pAnswer) &&
            (m_pAnswer->m_QueryAnswerFlags & static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::IPv6Address)));
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswerAccessor::IPv6Addresses

*/
clsLEAMDNSHost::clsQuery::clsAnswerAccessor::clsIPAddressVector clsLEAMDNSHost::clsQuery::clsAnswerAccessor::IPv6Addresses(void) const
{
    clsIPAddressVector  internalIP;
    if ((m_pAnswer) &&
            (IPv6AddressAvailable()))
    {
        for (uint32_t u = 0; u < m_pAnswer->IPv6AddressCount(); ++u)
        {
            const clsLEAMDNSHost::clsQuery::clsAnswer::clsIPAddressWithTTL*    pIPAddr = m_pAnswer->IPv6AddressAtIndex(u);
            if (pIPAddr)
            {
                internalIP.emplace_back(pIPAddr->m_IPAddress);
            }
        }
    }
    return internalIP;
}
#endif

/*
    clsLEAMDNSHost::clsQuery::clsAnswerAccessor::txtsAvailable

*/
bool clsLEAMDNSHost::clsQuery::clsAnswerAccessor::txtsAvailable(void) const
{
    return ((m_pAnswer) &&
            (m_pAnswer->m_QueryAnswerFlags & static_cast<clsQuery::clsAnswer::typeQueryAnswerType>(clsQuery::clsAnswer::enuQueryAnswerType::Txts)));
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswerAccessor::txts

    Returns all TXT items for the given service as a ';'-separated string.
    If not already existing; the string is alloced, filled and attached to the answer.

*/
const char* clsLEAMDNSHost::clsQuery::clsAnswerAccessor::txts(void) const
{
    return ((m_pAnswer)
            ?   m_pAnswer->m_Txts.c_str()
            : 0);
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswerAccessor::txtKeyValues

*/
const clsLEAMDNSHost::clsQuery::clsAnswerAccessor::clsTxtKeyValueMap& clsLEAMDNSHost::clsQuery::clsAnswerAccessor::txtKeyValues(void) const
{
    return m_TxtKeyValueMap;
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswerAccessor::txtValue

*/
const char* clsLEAMDNSHost::clsQuery::clsAnswerAccessor::txtValue(const char* p_pcKey) const
{
    char*   pcResult = 0;

    if (m_pAnswer)
    {
        for (const clsLEAMDNSHost::clsServiceTxt* pTxt : m_pAnswer->m_Txts.m_Txts)
        {
            if ((p_pcKey) &&
                    (0 == strcasecmp(pTxt->m_pcKey, p_pcKey)))
            {
                pcResult = pTxt->m_pcValue;
                break;
            }
        }
    }
    return pcResult;
}

/*
    clsLEAMDNSHost::clsQuery::clsAnswerAccessor::printTo

*/
size_t clsLEAMDNSHost::clsQuery::clsAnswerAccessor::printTo(Print& p_Print) const
{
    size_t      stLen = 0;
    const char* cpcI = " * ";
    const char* cpcS = "  ";

    stLen += p_Print.println(" * * * * *");
    if (hostDomainAvailable())
    {
        stLen += p_Print.print(cpcI);
        stLen += p_Print.print("Host domain: ");
        stLen += p_Print.println(hostDomain());
    }
#ifdef MDNS_IPV4_SUPPORT
    if (IPv4AddressAvailable())
    {
        stLen += p_Print.print(cpcI);
        stLen += p_Print.println("IPv4 address(es):");
        for (const IPAddress& addr : IPv4Addresses())
        {
            stLen += p_Print.print(cpcI);
            stLen += p_Print.print(cpcS);
            stLen += p_Print.println(addr);
        }
    }
#endif
#ifdef MDNS_IPV6_SUPPORT
    if (IPv6AddressAvailable())
    {
        stLen += p_Print.print(cpcI);
        stLen += p_Print.println("IPv6 address(es):");
        for (const IPAddress& addr : IPv6Addresses())
        {
            stLen += p_Print.print(cpcI);
            stLen += p_Print.print(cpcS);
            stLen += p_Print.println(addr);
        }
    }
#endif
    if (serviceDomainAvailable())
    {
        stLen += p_Print.print(cpcI);
        stLen += p_Print.print("Service domain: ");
        stLen += p_Print.println(serviceDomain());
    }
    if (hostPortAvailable())
    {
        stLen += p_Print.print(cpcI);
        stLen += p_Print.print("Host port: ");
        stLen += p_Print.println(hostPort());
    }
    if (txtsAvailable())
    {
        stLen += p_Print.print(cpcI);
        stLen += p_Print.print("TXTs:");
        for (auto const& x : txtKeyValues())
        {
            stLen += p_Print.print(cpcI);
            stLen += p_Print.print(cpcS);
            stLen += p_Print.print(x.first);
            stLen += p_Print.print("=");
            stLen += p_Print.println(x.second);
        }
    }
    stLen += p_Print.println(" * * * * *");

    return stLen;
}


/**
    clsLEAMDNSHost::clsQuery

    A service or host query object.
    A static query is flaged via 'm_bLegacyQuery'; while the function 'queryService'
    is waiting for answers, the internal flag 'm_bAwaitingAnswers' is set. When the
    timeout is reached, the flag is removed. These two flags are only used for static
    service queries.
    All answers to the query are stored in the 'm_Answers' list.
    Individual answers may be addressed by index (in the list of answers).
    Every time a answer component is added (or changed) in a dynamic query,
    the callback 'm_fnCallback' is called.
    The answer list may be searched by service and host domain.

    Query object may be connected to a linked list.

*/

/*
    clsLEAMDNSHost::clsQuery::clsQuery constructor

*/
clsLEAMDNSHost::clsQuery::clsQuery(const enuQueryType p_QueryType)
    :   m_QueryType(p_QueryType),
        m_fnCallbackAnswer(0),
        m_fnCallbackAccessor(0),
        m_bStaticQuery(false),
        m_u8SentCount(0),
        m_ResendTimeout(std::numeric_limits<esp8266::polledTimeout::oneShot::timeType>::max()),
        m_bAwaitingAnswers(true)
{
    clear();
    m_QueryType = p_QueryType;
}

/*
    clsLEAMDNSHost::clsQuery::~stcQuery destructor

*/
clsLEAMDNSHost::clsQuery::~clsQuery(void)
{
    clear();
}

/*
    clsLEAMDNSHost::clsQuery::clear

*/
bool clsLEAMDNSHost::clsQuery::clear(void)
{
    m_QueryType = enuQueryType::None;
    m_fnCallbackAnswer = 0;
    m_fnCallbackAccessor = 0;
    m_bStaticQuery = false;
    m_u8SentCount = 0;
    m_ResendTimeout.reset(std::numeric_limits<esp8266::polledTimeout::oneShot::timeType>::max());
    m_bAwaitingAnswers = true;
    for (clsAnswer* pAnswer : m_Answers)
    {
        delete pAnswer;
    }
    m_Answers.clear();
    return true;
}

/*
    clsLEAMDNSHost::clsQuery::answerCount

*/
uint32_t clsLEAMDNSHost::clsQuery::answerCount(void) const
{
    uint32_t    u32Count = m_Answers.size();
    return u32Count;
}

/*
    clsLEAMDNSHost::clsQuery::answer

*/
const clsLEAMDNSHost::clsQuery::clsAnswer* clsLEAMDNSHost::clsQuery::answer(uint32_t p_u32Index) const
{
    const clsAnswer*    pAnswerAtIndex = 0;

    uint32_t    u32CurIndex = 0;
    for (clsAnswer::list::const_iterator it = m_Answers.begin();
            (((uint32_t)(-1) != p_u32Index) && (u32CurIndex <= p_u32Index) && (it != m_Answers.end()));
            it++, u32CurIndex++)
    {
        if (p_u32Index == u32CurIndex++)
        {
            pAnswerAtIndex = *it;
            break;
        }
    }
    return pAnswerAtIndex;
}

/*
    clsLEAMDNSHost::clsQuery::indexOfAnswer

*/
uint32_t clsLEAMDNSHost::clsQuery::indexOfAnswer(const clsLEAMDNSHost::clsQuery::clsAnswer* p_pAnswer) const
{
    uint32_t    u32IndexOfAnswer = ((uint32_t)(-1));

    uint32_t    u32CurIndex = 0;
    for (const clsAnswer* pAnswer : m_Answers)
    {
        if (pAnswer == p_pAnswer)
        {
            u32IndexOfAnswer = u32CurIndex;
            break;
        }
        ++u32CurIndex;
    }
    return u32IndexOfAnswer;
}

/*
    clsLEAMDNSHost::clsQuery::answerAccessors

*/
clsLEAMDNSHost::clsQuery::clsAnswerAccessor::vector clsLEAMDNSHost::clsQuery::answerAccessors(void) const
{
    clsAnswerAccessor::vector   tempAccessors;
    for (const clsAnswer* pAnswer : m_Answers)
    {
        tempAccessors.emplace_back(pAnswer);
    }
    return tempAccessors;
}

/*
    clsLEAMDNSHost::clsQuery::answerAccessor

*/
clsLEAMDNSHost::clsQuery::clsAnswerAccessor clsLEAMDNSHost::clsQuery::answerAccessor(uint32 p_u32AnswerIndex) const
{
    return clsAnswerAccessor(answer(p_u32AnswerIndex));
}

/*
    clsLEAMDNSHost::clsQuery::addAnswer

*/
bool clsLEAMDNSHost::clsQuery::addAnswer(clsLEAMDNSHost::clsQuery::clsAnswer* p_pAnswer)
{
    bool    bResult = false;

    if (p_pAnswer)
    {
        m_Answers.push_back(p_pAnswer);
        bResult = true;
    }
    return bResult;
}

/*
    clsLEAMDNSHost::clsQuery::removeAnswer

*/
bool clsLEAMDNSHost::clsQuery::removeAnswer(clsLEAMDNSHost::clsQuery::clsAnswer* p_pAnswer)
{
    bool    bResult = false;

    clsAnswer::list::iterator   it(p_pAnswer
                                   ? std::find(m_Answers.begin(), m_Answers.end(), p_pAnswer)
                                   : m_Answers.end());
    if (m_Answers.end() != it)
    {
        m_Answers.erase(it);
        delete p_pAnswer;

        bResult = true;
    }
    return bResult;
}

/*
    clsLEAMDNSHost::clsQuery::findAnswerForServiceDomain

*/
clsLEAMDNSHost::clsQuery::clsAnswer* clsLEAMDNSHost::clsQuery::findAnswerForServiceDomain(const clsLEAMDNSHost::clsRRDomain& p_ServiceDomain)
{
    clsAnswer*  pAnswerForServiceDomain = 0;

    for (clsAnswer* pAnswer : m_Answers)
    {
        if (pAnswer->m_ServiceDomain == p_ServiceDomain)
        {
            pAnswerForServiceDomain = pAnswer;
            break;
        }
    }
    return pAnswerForServiceDomain;
}

/*
    clsLEAMDNSHost::clsQuery::findAnswerForHostDomain

*/
clsLEAMDNSHost::clsQuery::clsAnswer* clsLEAMDNSHost::clsQuery::findAnswerForHostDomain(const clsLEAMDNSHost::clsRRDomain& p_HostDomain)
{
    clsAnswer*  pAnswerForHostDomain = 0;

    for (clsAnswer* pAnswer : m_Answers)
    {
        if (pAnswer->m_HostDomain == p_HostDomain)
        {
            pAnswerForHostDomain = pAnswer;
            break;
        }
    }
    return pAnswerForHostDomain;
}


}   // namespace MDNSImplementation


}   // namespace esp8266





