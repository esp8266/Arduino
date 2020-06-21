/*
 	LEAmDNS2_Legacy.cpp


*/

#include "ESP8266mDNS.h"
#include "LEAmDNS2_Legacy.h"


namespace experimental // esp8266
{

/**
    LEAmDNS
*/
namespace MDNSImplementation
{

/**
    STRINGIZE
*/
#ifndef STRINGIZE
#define STRINGIZE(x) #x
#endif
#ifndef STRINGIZE_VALUE_OF
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)
#endif


/*
    clsLEAMDNSHost_Legacy::clsLEAMDNSHost_Legacy constructor

*/
clsLEAMDNSHost_Legacy::clsLEAMDNSHost_Legacy(void)
{
}

/*
    clsLEAMDNSHost_Legacy::clsLEAMDNSHost_Legacy destructor

*/
clsLEAMDNSHost_Legacy::~clsLEAMDNSHost_Legacy(void)
{
}

/*

    HOST SETUP

*/

/*
    clsLEAMDNSHost_Legacy::begin

*/
bool clsLEAMDNSHost_Legacy::begin(const char* p_pcHostname)
{
    return addHostForNetIf(p_pcHostname)
           && (0 != m_HostInformations.size());
}


/*
    clsLEAMDNSHost_Legacy::begin (String)

*/
bool clsLEAMDNSHost_Legacy::begin(const String& p_strHostname)
{
    return begin(p_strHostname.c_str());
}

/*
    clsLEAMDNSHost_Legacy::begin (Ignored Options)

*/
bool clsLEAMDNSHost_Legacy::begin(const char* p_pcHostname,
                                  IPAddress /*p_IPAddress = INADDR_ANY*/,	// ignored
                                  uint32_t /*p_u32TTL = 120*/)				// ignored
{
    return begin(p_pcHostname);
}

/*
    clsLEAMDNSHost_Legacy::begin (String & Ignored Options)

*/
bool clsLEAMDNSHost_Legacy::begin(const String& p_strHostname,
                                  IPAddress /*p_IPAddress = INADDR_ANY*/,	// ignored
                                  uint32_t /*p_u32TTL = 120*/)  			// ignored
{
    return begin(p_strHostname.c_str());
}

/*
    clsLEAMDNSHost_Legacy::close

*/
bool clsLEAMDNSHost_Legacy::close(void)
{
    bool	bResult = true;

    for (stcHostInformation::list::iterator it = m_HostInformations.begin(); ((bResult) && (it != m_HostInformations.end())); ++it)
    {
        if ((bResult = (*it).m_pHost->close()))
        {
            delete (*it).m_pHost;
            (*it).m_pHost = 0;
        }
    }
    return ((bResult)
            && (m_HostInformations.clear(), true));
}

/*
    clsLEAMDNSHost_Legacy::end

*/
bool clsLEAMDNSHost_Legacy::end(void)
{
    return close();
}

/*
    clsLEAMDNSHost_Legacy::addHostForNetIf

    NEW!

*/
bool clsLEAMDNSHost_Legacy::addHostForNetIf(const char* p_pcHostname)
{
    bool    bResult = true;

    if (m_HostInformations.size() > 0)
    {
        //XXXFIXME only one pHost instance, many things can be simplified
        bResult = false;
    }
    else
    {
        clsLEAMDNSHost* pHost = new esp8266::experimental::clsLEAMDNSHost;
        if (pHost
                && (!((pHost->begin(p_pcHostname /*, default callback*/))
                      && (m_HostInformations.push_back(stcHostInformation(pHost)), true))))
        {
            bResult = false;
        }
    }
    return bResult;
}

/*
    clsLEAMDNSHost_Legacy::setHostname

*/
bool clsLEAMDNSHost_Legacy::setHostname(const char* p_pcHostname)
{
    bool	bResult = true;

    for (stcHostInformation::list::iterator it = m_HostInformations.begin(); ((bResult) && (it != m_HostInformations.end())); ++it)
    {
        bResult = (*it).m_pHost->setHostName(p_pcHostname);
    }
    return bResult;
}

/*
    clsLEAMDNSHost_Legacy::setHostname

*/
bool clsLEAMDNSHost_Legacy::setHostname(String p_strHostname)
{
    return setHostname(p_strHostname.c_str());
}

/*
    clsLEAMDNSHost_Legacy::hostname

*/
const char* clsLEAMDNSHost_Legacy::hostname(void) const
{
    return (m_HostInformations.empty()
            ? 0
            : m_HostInformations.front().m_pHost->hostName());
}

/*
    clsLEAMDNSHost_Legacy::status

*/
bool clsLEAMDNSHost_Legacy::status(void) const
{
    bool	bStatus = true;

    for (const stcHostInformation& hostInformation : m_HostInformations)
    {
        if (!((bStatus = hostInformation.m_pHost->probeStatus())))
        {
            break;
        }
    }
    return bStatus;
}


/*

    SERVICE MANAGEMENT

*/

/*
    clsLEAMDNSHost_Legacy::addService

*/
clsLEAMDNSHost_Legacy::hMDNSService clsLEAMDNSHost_Legacy::addService(const char* p_pcName,
        const char* p_pcService,
        const char* p_pcProtocol,
        uint16_t p_u16Port)
{
    hMDNSService	hResult = 0;

    for (stcHostInformation& hostInformation : m_HostInformations)
    {
        clsLEAMDNSHost::clsService*	pService = hostInformation.m_pHost->addService(p_pcName, p_pcService, p_pcProtocol, p_u16Port /*, default callback*/);
        if (pService)
        {
            if (!hResult)
            {
                // Store first service handle as result and key
                hResult = (hMDNSService)pService;
            }
            hostInformation.m_HandleToPtr[hResult] = pService;
        }
    }
    return hResult;
}

/*
    clsLEAMDNSHost_Legacy::addService (String)

*/
bool clsLEAMDNSHost_Legacy::addService(String p_strServiceName,
                                       String p_strProtocol,
                                       uint16_t p_u16Port)
{
    return (0 != addService(0, p_strServiceName.c_str(), p_strProtocol.c_str(), p_u16Port));
}

/*
    clsLEAMDNSHost_Legacy::removeService (hService)

*/
bool clsLEAMDNSHost_Legacy::removeService(const hMDNSService p_hService)
{
    bool	bResult = true;

    for (stcHostInformation::list::iterator it = m_HostInformations.begin(); ((bResult) && (it != m_HostInformations.end())); ++it)
    {
        clsLEAMDNSHost::clsService*	pService = (clsLEAMDNSHost::clsService*)(*it).m_HandleToPtr[p_hService];
        if ((bResult = ((pService)
                        && ((*it).m_pHost->removeService(pService)))))
        {
            (*it).m_HandleToPtr.erase(p_hService);
        }
    }
    return bResult;
}

/*
    clsLEAMDNSHost_Legacy::removeService (name)

*/
bool clsLEAMDNSHost_Legacy::removeService(const char* p_pcInstanceName,
        const char* p_pcServiceName,
        const char* p_pcProtocol)
{
    hMDNSService	hService = 0;
    return (((hService = (m_HostInformations.empty()
                          ? 0
                          : (hMDNSService)m_HostInformations.front().m_pHost->findService(p_pcInstanceName, p_pcServiceName, p_pcProtocol))))
            && (removeService(hService)));
}

/*
    clsLEAMDNSHost_Legacy::setServiceName

*/
bool clsLEAMDNSHost_Legacy::setServiceName(const hMDNSService p_hService,
        const char* p_pcInstanceName)
{
    bool	bResult = true;

    for (stcHostInformation::list::iterator it = m_HostInformations.begin(); ((bResult) && (it != m_HostInformations.end())); ++it)
    {
        clsLEAMDNSHost::clsService*	pService = (clsLEAMDNSHost::clsService*)(*it).m_HandleToPtr[p_hService];
        bResult = ((pService)
                   && (pService->setInstanceName(p_pcInstanceName)));
    }
    return bResult;
}

/*
    clsLEAMDNSHost_Legacy::setInstanceName

*/
void clsLEAMDNSHost_Legacy::setInstanceName(const char* p_pcInstanceName)
{
    for (stcHostInformation& hostInformation : m_HostInformations)
    {
        hostInformation.m_pHost->setDefaultInstanceName(p_pcInstanceName);
    }
}

/*
    clsLEAMDNSHost_Legacy::setInstanceName (String)

*/
void clsLEAMDNSHost_Legacy::setInstanceName(const String& p_strHostname)
{
    setInstanceName(p_strHostname.c_str());
}

/*
    clsLEAMDNSHost_Legacy::serviceName

*/
const char* clsLEAMDNSHost_Legacy::serviceName(const hMDNSService p_hService) const
{
    const clsLEAMDNSHost::clsService*	pService = 0;
    return (m_HostInformations.empty()
            ? 0
            : (((pService = (const clsLEAMDNSHost::clsService*)(m_HostInformations.front().m_HandleToPtr.at(p_hService))))
               ? pService->instanceName()
               : 0));
}

/*
    clsLEAMDNSHost_Legacy::service

*/
const char* clsLEAMDNSHost_Legacy::service(const hMDNSService p_hService) const
{
    const clsLEAMDNSHost::clsService*	pService = 0;
    return (m_HostInformations.empty()
            ? 0
            : (((pService = (const clsLEAMDNSHost::clsService*)m_HostInformations.front().m_HandleToPtr.at(p_hService)))
               ? pService->type()
               : 0));
}

/*
    clsLEAMDNSHost_Legacy::serviceProtocol

*/
const char* clsLEAMDNSHost_Legacy::serviceProtocol(const hMDNSService p_hService) const
{
    const clsLEAMDNSHost::clsService*	pService = 0;
    return (m_HostInformations.empty()
            ? 0
            : (((pService = (const clsLEAMDNSHost::clsService*)m_HostInformations.front().m_HandleToPtr.at(p_hService)))
               ? pService->protocol()
               : 0));
}

/*
    clsLEAMDNSHost_Legacy::serviceStatus

*/
bool clsLEAMDNSHost_Legacy::serviceStatus(const hMDNSService p_hService) const
{
    const clsLEAMDNSHost::clsService*	pService = 0;
    return (m_HostInformations.empty()
            ? false
            : (((pService = (const clsLEAMDNSHost::clsService*)m_HostInformations.front().m_HandleToPtr.at(p_hService)))
               ? pService->probeStatus()
               : false));
}


/*

    SERVICE TXT MANAGEMENT

*/

/*
    clsLEAMDNSHost_Legacy::addServiceTxt (char*)

*/
clsLEAMDNSHost_Legacy::hMDNSTxt clsLEAMDNSHost_Legacy::addServiceTxt(const hMDNSService p_hService,
        const char* p_pcKey,
        const char* p_pcValue)
{
    return _addServiceTxt(p_hService, p_pcKey, p_pcValue, false);
}

/*
    clsLEAMDNSHost_Legacy::addServiceTxt (uint32_t)

*/
clsLEAMDNSHost_Legacy::hMDNSTxt clsLEAMDNSHost_Legacy::addServiceTxt(const hMDNSService p_hService,
        const char* p_pcKey,
        uint32_t p_u32Value)
{
    return _addServiceTxt(p_hService, p_pcKey, p_u32Value, false);
}

/*
    clsLEAMDNSHost_Legacy::addServiceTxt (uint16_t)

*/
clsLEAMDNSHost_Legacy::hMDNSTxt clsLEAMDNSHost_Legacy::addServiceTxt(const hMDNSService p_hService,
        const char* p_pcKey,
        uint16_t p_u16Value)
{
    return _addServiceTxt(p_hService, p_pcKey, p_u16Value, false);
}

/*
    clsLEAMDNSHost_Legacy::addServiceTxt (uint8_t)

*/
clsLEAMDNSHost_Legacy::hMDNSTxt clsLEAMDNSHost_Legacy::addServiceTxt(const hMDNSService p_hService,
        const char* p_pcKey,
        uint8_t p_u8Value)
{
    return _addServiceTxt(p_hService, p_pcKey, p_u8Value, false);
}

/*
    clsLEAMDNSHost_Legacy::addServiceTxt (int32_t)

*/
clsLEAMDNSHost_Legacy::hMDNSTxt clsLEAMDNSHost_Legacy::addServiceTxt(const hMDNSService p_hService,
        const char* p_pcKey,
        int32_t p_i32Value)
{
    return _addServiceTxt(p_hService, p_pcKey, p_i32Value, false);
}

/*
    clsLEAMDNSHost_Legacy::addServiceTxt (int16_t)

*/
clsLEAMDNSHost_Legacy::hMDNSTxt clsLEAMDNSHost_Legacy::addServiceTxt(const hMDNSService p_hService,
        const char* p_pcKey,
        int16_t p_i16Value)
{
    return _addServiceTxt(p_hService, p_pcKey, p_i16Value, false);
}

/*
    clsLEAMDNSHost_Legacy::addServiceTxt (int8_t)

*/
clsLEAMDNSHost_Legacy::hMDNSTxt clsLEAMDNSHost_Legacy::addServiceTxt(const hMDNSService p_hService,
        const char* p_pcKey,
        int8_t p_i8Value)
{
    return _addServiceTxt(p_hService, p_pcKey, p_i8Value, false);
}

/*
    clsLEAMDNSHost_Legacy::addServiceTxt (legacy)

*/
bool clsLEAMDNSHost_Legacy::addServiceTxt(const char* p_pcService,
        const char* p_pcProtocol,
        const char* p_pcKey,
        const char* p_pcValue)
{
    hMDNSService	hService = 0;
    return (((hService = (m_HostInformations.empty()
                          ? 0
                          : (hMDNSService)m_HostInformations.front().m_pHost->findService(0, p_pcService, p_pcProtocol))))
            && (_addServiceTxt(hService, p_pcKey, p_pcValue, false)));
}

/*
    clsLEAMDNSHost_Legacy::addServiceTxt (legacy, String)

*/
bool clsLEAMDNSHost_Legacy::addServiceTxt(String p_strService,
        String p_strProtocol,
        String p_strKey,
        String p_strValue)
{
    return addServiceTxt(p_strService.c_str(), p_strProtocol.c_str(), p_strKey.c_str(), p_strValue.c_str());
}

/*
    clsLEAMDNSHost_Legacy::removeServiceTxt (hTxt)

*/
bool clsLEAMDNSHost_Legacy::removeServiceTxt(const hMDNSService p_hService,
        const hMDNSTxt p_hTxt)
{
    bool	bResult = true;

    for (stcHostInformation::list::iterator it = m_HostInformations.begin(); ((bResult) && (it != m_HostInformations.end())); ++it)
    {
        clsLEAMDNSHost::clsService*		pService = (clsLEAMDNSHost::clsService*)(*it).m_HandleToPtr[p_hService];
        clsLEAMDNSHost::clsServiceTxt*	pTxt = (clsLEAMDNSHost::clsServiceTxt*)(*it).m_HandleToPtr[p_hTxt];
        if ((bResult = ((pService)
                        && (pTxt)
                        && (pService->removeServiceTxt(pTxt)))))
        {
            (*it).m_HandleToPtr.erase(p_hTxt);
        }
    }
    return bResult;
}

/*
    clsLEAMDNSHost_Legacy::removeServiceTxt (char*)

*/
bool clsLEAMDNSHost_Legacy::removeServiceTxt(const hMDNSService p_hService,
        const char* p_pcKey)
{
    clsLEAMDNSHost::clsService*		pService = 0;
    clsLEAMDNSHost::clsServiceTxt*	pTxt = 0;
    return (((pService = (m_HostInformations.empty()
                          ? 0
                          : (clsLEAMDNSHost::clsService*)m_HostInformations.front().m_HandleToPtr[p_hService])))
            && ((pTxt = pService->findServiceTxt(p_pcKey)))
            && (removeServiceTxt(p_hService, (const hMDNSTxt)pTxt)));
}

/*
    clsLEAMDNSHost_Legacy::removeServiceTxt (char*)

*/
bool clsLEAMDNSHost_Legacy::removeServiceTxt(const char* p_pcInstanceName,
        const char* p_pcServiceName,
        const char* p_pcProtocol,
        const char* p_pcKey)
{
    hMDNSService	hService = 0;
    return (((hService = (m_HostInformations.empty()
                          ? 0
                          : (hMDNSService)m_HostInformations.front().m_pHost->findService(p_pcInstanceName, p_pcServiceName, p_pcProtocol))))
            && (removeServiceTxt(hService, p_pcKey)));
}

/*
    clsLEAMDNSHost_Legacy::setDynamicServiceTxtCallback (global)

*/
bool clsLEAMDNSHost_Legacy::setDynamicServiceTxtCallback(MDNSDynamicServiceTxtCallbackFn p_fnCallback)
{
    bool	bResult = true;

    if ((bResult = m_HostInformations.empty()))
    {
        // The service handles of the first host are the keys in the HostInformations.HandleToPtr map
        for (const clsLEAMDNSHost::clsService* pService : m_HostInformations.front().m_pHost->services())
        {
            if (!((bResult = setDynamicServiceTxtCallback((hMDNSService)pService, p_fnCallback))))
            {
                break;
            }
        }
    }
    return bResult;
}

/*
    clsLEAMDNSHost_Legacy::setDynamicServiceTxtCallback (service)

*/
bool clsLEAMDNSHost_Legacy::setDynamicServiceTxtCallback(const hMDNSService p_hService,
        MDNSDynamicServiceTxtCallbackFn p_fnCallback)
{
    bool	bResult = true;

    for (stcHostInformation::list::iterator it = m_HostInformations.begin(); ((bResult) && (it != m_HostInformations.end())); ++it)
    {
        clsLEAMDNSHost::clsService*		pService = (clsLEAMDNSHost::clsService*)(*it).m_HandleToPtr[p_hService];
        bResult = pService->setDynamicServiceTxtCallback([p_hService, p_fnCallback](clsLEAMDNSHost::clsService& /*p_rMDNSService*/)->void
        {
            if (p_fnCallback)	// void(const hMDNSService p_hService)
            {
                p_fnCallback(p_hService);
            }
        });
    }
    return bResult;
}

/*
    clsLEAMDNSHost_Legacy::addDynamicServiceTxt (char*)

*/
clsLEAMDNSHost_Legacy::hMDNSTxt clsLEAMDNSHost_Legacy::addDynamicServiceTxt(hMDNSService p_hService,
        const char* p_pcKey,
        const char* p_pcValue)
{
    return _addServiceTxt(p_hService, p_pcKey, p_pcValue, true);
}

/*
    clsLEAMDNSHost_Legacy::addDynamicServiceTxt (uint32)

*/
clsLEAMDNSHost_Legacy::hMDNSTxt clsLEAMDNSHost_Legacy::addDynamicServiceTxt(hMDNSService p_hService,
        const char* p_pcKey,
        uint32_t p_u32Value)
{
    return _addServiceTxt(p_hService, p_pcKey, p_u32Value, true);
}

/*
    clsLEAMDNSHost_Legacy::addDynamicServiceTxt (uint16)

*/
clsLEAMDNSHost_Legacy::hMDNSTxt clsLEAMDNSHost_Legacy::addDynamicServiceTxt(hMDNSService p_hService,
        const char* p_pcKey,
        uint16_t p_u16Value)
{
    return _addServiceTxt(p_hService, p_pcKey, p_u16Value, true);
}

/*
    clsLEAMDNSHost_Legacy::addDynamicServiceTxt (uint8)

*/
clsLEAMDNSHost_Legacy::hMDNSTxt clsLEAMDNSHost_Legacy::addDynamicServiceTxt(hMDNSService p_hService,
        const char* p_pcKey,
        uint8_t p_u8Value)
{
    return _addServiceTxt(p_hService, p_pcKey, p_u8Value, true);
}

/*
    clsLEAMDNSHost_Legacy::addDynamicServiceTxt (int32)

*/
clsLEAMDNSHost_Legacy::hMDNSTxt clsLEAMDNSHost_Legacy::addDynamicServiceTxt(hMDNSService p_hService,
        const char* p_pcKey,
        int32_t p_i32Value)
{
    return _addServiceTxt(p_hService, p_pcKey, p_i32Value, true);
}

/*
    clsLEAMDNSHost_Legacy::addDynamicServiceTxt (int16)

*/
clsLEAMDNSHost_Legacy::hMDNSTxt clsLEAMDNSHost_Legacy::addDynamicServiceTxt(hMDNSService p_hService,
        const char* p_pcKey,
        int16_t p_i16Value)
{
    return _addServiceTxt(p_hService, p_pcKey, p_i16Value, true);
}

/*
    clsLEAMDNSHost_Legacy::addDynamicServiceTxt (int8)

*/
clsLEAMDNSHost_Legacy::hMDNSTxt clsLEAMDNSHost_Legacy::addDynamicServiceTxt(hMDNSService p_hService,
        const char* p_pcKey,
        int8_t p_i8Value)
{
    return _addServiceTxt(p_hService, p_pcKey, p_i8Value, true);
}


/*

    STATIC QUERY

*/

/*
    clsLEAMDNSHost_Legacy::queryService

    This will take p_u16Timeout millisec for every host!

*/
uint32_t clsLEAMDNSHost_Legacy::queryService(const char* p_pcService,
        const char* p_pcProtocol,
        const uint16_t p_u16Timeout /*= MDNS_QUERYSERVICES_WAIT_TIME*/)
{
    uint32_t	u32Answers = 0;

    for (stcHostInformation& hostInformation : m_HostInformations)
    {
        u32Answers += (hostInformation.m_pHost->queryService(p_pcService, p_pcProtocol, p_u16Timeout)).size();
    }
    return u32Answers;
}

/*
    clsLEAMDNSHost_Legacy::removeQuery

*/
bool clsLEAMDNSHost_Legacy::removeQuery(void)
{
    bool	bResult = true;

    for (stcHostInformation::list::iterator it = m_HostInformations.begin(); ((bResult) && (it != m_HostInformations.end())); ++it)
    {
        bResult = (*it).m_pHost->removeQuery();
    }
    return bResult;
}

/*
    clsLEAMDNSHost_Legacy::queryService

*/
uint32_t clsLEAMDNSHost_Legacy::queryService(String p_strService,
        String p_strProtocol)
{
    return queryService(p_strService.c_str(), p_strProtocol.c_str());
}

/*
    clsLEAMDNSHost_Legacy::answerHostname

*/
const char* clsLEAMDNSHost_Legacy::answerHostname(const uint32_t p_u32AnswerIndex)
{
    const clsLEAMDNSHost::clsQuery::clsAnswerAccessor	answerAccessor = _getAnswerAccessor(p_u32AnswerIndex);
    return (answerAccessor.serviceDomainAvailable()
            ? answerAccessor.serviceDomain()
            : 0);
}

/*
    clsLEAMDNSHost_Legacy::answerIP

*/
IPAddress clsLEAMDNSHost_Legacy::answerIP(const uint32_t p_u32AnswerIndex)
{
    const clsLEAMDNSHost::clsQuery::clsAnswerAccessor	answerAccessor = _getAnswerAccessor(p_u32AnswerIndex);
    return (answerAccessor.IPv4AddressAvailable()
            ? answerAccessor.IPv4Addresses()[0]
            : IPAddress());
}

/*
    clsLEAMDNSHost_Legacy::answerPort

*/
uint16_t clsLEAMDNSHost_Legacy::answerPort(const uint32_t p_u32AnswerIndex)
{
    const clsLEAMDNSHost::clsQuery::clsAnswerAccessor	answerAccessor = _getAnswerAccessor(p_u32AnswerIndex);
    return (answerAccessor.hostPortAvailable()
            ? answerAccessor.hostPort()
            : 0);
}

/*
    clsLEAMDNSHost_Legacy::hostname

*/
String clsLEAMDNSHost_Legacy::hostname(const uint32_t p_u32AnswerIndex)
{
    return String(answerHostname(p_u32AnswerIndex));
}

/*
    clsLEAMDNSHost_Legacy::IP

*/
IPAddress clsLEAMDNSHost_Legacy::IP(const uint32_t p_u32AnswerIndex)
{
    return answerIP(p_u32AnswerIndex);
}

/*
    clsLEAMDNSHost_Legacy::port

*/
uint16_t clsLEAMDNSHost_Legacy::port(const uint32_t p_u32AnswerIndex)
{
    return answerPort(p_u32AnswerIndex);
}


/*

    DYNAMIC QUERY

*/

/*
    clsLEAMDNSHost_Legacy::installServiceQuery

*/
clsLEAMDNSHost_Legacy::hMDNSServiceQuery clsLEAMDNSHost_Legacy::installServiceQuery(const char* p_pcService,
        const char* p_pcProtocol,
        MDNSServiceQueryCallbackFn p_fnCallback)
{
    hMDNSServiceQuery	hResult = 0;

    for (stcHostInformation& hostInformation : m_HostInformations)
    {
        std::list<clsLEAMDNSHost::clsQuery*> queries;

        (void)p_pcService;
        (void)p_pcProtocol;
        (void)p_fnCallback;
        /*clsLEAMDNSHost::clsQuery*	pQuery =*/
//        hostInformation.m_pHost->installServiceQuery(p_pcService, p_pcProtocol, [this, p_fnCallback](const clsLEAMDNSHost::clsQuery& /*p_Query*/,
//                const clsLEAMDNSHost::clsQuery::clsAnswerAccessor & p_AnswerAccessor,
//                clsLEAMDNSHost::clsQuery::clsAnswer::typeQueryAnswerType p_QueryAnswerTypeFlags,   // flags for the updated answer item
//                bool p_bSetContent)->void
//        {
//            if (p_fnCallback)	// void(const stcMDNSServiceInfo& p_MDNSServiceInfo, MDNSResponder::AnswerType p_AnswerType, bool p_bSetContent)
//            {
//                p_fnCallback(stcMDNSServiceInfo(p_AnswerAccessor), _answerFlagsToAnswerType(p_QueryAnswerTypeFlags), p_bSetContent);
//            }
//        }, &queries);

///XXX Aways false
        if (queries.size())
        {
            if (!hResult)
            {
                // - hMDNSServiceQuery handle is 'const void*'
                //   used to retrieve pQuery when updating or removing.

                // - unexplained - before multi interface change:
                //   there is a loop, only the first is returned, why ?

                // Store first query as result and key
                //hResult = (hMDNSServiceQuery)pQuery; <- before netif, only the first query is returned

                // - netif transformation: even more returned values (a list per loop),
                //   still, only the first handle is returned.
                hResult = (hMDNSServiceQuery) * queries.begin(); // take the first
            }
            // this was overwritten ?
            //hostInformation.m_HandleToPtr[hResult] = pQuery;
            // ... overwritten with only the first query
            hostInformation.m_HandleToPtr[hResult] = *queries.begin();
        }
    }
    return hResult;
}

/*
    clsLEAMDNSHost_Legacy::removeServiceQuery

*/
bool clsLEAMDNSHost_Legacy::removeServiceQuery(clsLEAMDNSHost_Legacy::hMDNSServiceQuery p_hServiceQuery)
{
    bool	bResult = true;

    for (stcHostInformation::list::iterator it = m_HostInformations.begin(); ((bResult) && (it != m_HostInformations.end())); ++it)
    {
        if ((bResult = (*it).m_pHost->removeQuery((clsLEAMDNSHost::clsQuery*)(*it).m_HandleToPtr[p_hServiceQuery])))
        {
            (*it).m_HandleToPtr.erase(p_hServiceQuery);
        }
    }
    return bResult;
}

/*
    clsLEAMDNSHost_Legacy::answerCount

*/
uint32_t clsLEAMDNSHost_Legacy::answerCount(const clsLEAMDNSHost_Legacy::hMDNSServiceQuery p_hServiceQuery)
{
    uint32_t	u32AnswerCount = 0;

    for (stcHostInformation& hostInformation : m_HostInformations)
    {
        clsLEAMDNSHost::clsQuery*   pQuery = (clsLEAMDNSHost::clsQuery*)hostInformation.m_HandleToPtr[p_hServiceQuery];
        if (pQuery)
        {
            u32AnswerCount += pQuery->answerCount();
        }
        else
        {
            u32AnswerCount = 0;
            break;
        }
    }
    return u32AnswerCount;
}

/*
    clsLEAMDNSHost_Legacy::answerInfo

*/
std::vector<clsLEAMDNSHost_Legacy::stcMDNSServiceInfo> clsLEAMDNSHost_Legacy::answerInfo(const clsLEAMDNSHost_Legacy::hMDNSServiceQuery p_hServiceQuery)
{
    std::vector<stcMDNSServiceInfo>	serviceInfos;

    if (p_hServiceQuery)
    {
        for (stcHostInformation& hostInformation : m_HostInformations)
        {
            clsLEAMDNSHost::clsQuery*   pQuery = (clsLEAMDNSHost::clsQuery*)hostInformation.m_HandleToPtr[p_hServiceQuery];
            if (pQuery)
            {
                for (clsLEAMDNSHost::clsQuery::clsAnswerAccessor& answerAccessor : pQuery->answerAccessors())
                {
                    serviceInfos.push_back(stcMDNSServiceInfo(answerAccessor));
                }
            }
            else
            {
                serviceInfos.clear();
                break;
            }
        }
    }
    return serviceInfos;
}

/*
    clsLEAMDNSHost_Legacy::answerServiceDomain

*/
const char* clsLEAMDNSHost_Legacy::answerServiceDomain(const clsLEAMDNSHost_Legacy::hMDNSServiceQuery p_hServiceQuery,
        const uint32_t p_u32AnswerIndex)
{
    const clsLEAMDNSHost::clsQuery::clsAnswerAccessor	answerAccessor = _getAnswerAccessor(p_hServiceQuery, p_u32AnswerIndex);
    return (answerAccessor.serviceDomainAvailable()
            ? answerAccessor.serviceDomain()
            : 0);
}

/*
    clsLEAMDNSHost_Legacy::hasAnswerHostDomain

*/
bool clsLEAMDNSHost_Legacy::hasAnswerHostDomain(const clsLEAMDNSHost_Legacy::hMDNSServiceQuery p_hServiceQuery,
        const uint32_t p_u32AnswerIndex)
{
    return _getAnswerAccessor(p_hServiceQuery, p_u32AnswerIndex).hostDomainAvailable();
}

/*
    clsLEAMDNSHost_Legacy::answerHostDomain

*/
const char* clsLEAMDNSHost_Legacy::answerHostDomain(const clsLEAMDNSHost_Legacy::hMDNSServiceQuery p_hServiceQuery,
        const uint32_t p_u32AnswerIndex)
{
    const clsLEAMDNSHost::clsQuery::clsAnswerAccessor	answerAccessor = _getAnswerAccessor(p_hServiceQuery, p_u32AnswerIndex);
    return (answerAccessor.hostDomainAvailable()
            ? answerAccessor.hostDomain()
            : 0);
}

#ifdef MDNS_IP4_SUPPORT
/*
    clsLEAMDNSHost_Legacy::hasAnswerIP4Address

*/
bool clsLEAMDNSHost_Legacy::hasAnswerIP4Address(const clsLEAMDNSHost_Legacy::hMDNSServiceQuery p_hServiceQuery,
        const uint32_t p_u32AnswerIndex)
{
    return _getAnswerAccessor(p_hServiceQuery, p_u32AnswerIndex).IPv4AddressAvailable();
}

/*
    clsLEAMDNSHost_Legacy::answerIP4AddressCount

*/
uint32_t clsLEAMDNSHost_Legacy::answerIP4AddressCount(const clsLEAMDNSHost_Legacy::hMDNSServiceQuery p_hServiceQuery,
        const uint32_t p_u32AnswerIndex)
{
    const clsLEAMDNSHost::clsQuery::clsAnswerAccessor	answerAccessor = _getAnswerAccessor(p_hServiceQuery, p_u32AnswerIndex);
    return (answerAccessor.IPv4AddressAvailable()
            ? answerAccessor.IPv4Addresses().size()
            : 0);
}

/*
    clsLEAMDNSHost_Legacy::answerIP4Address

*/
IPAddress clsLEAMDNSHost_Legacy::answerIP4Address(const clsLEAMDNSHost_Legacy::hMDNSServiceQuery p_hServiceQuery,
        const uint32_t p_u32AnswerIndex,
        const uint32_t p_u32AddressIndex)
{
    const clsLEAMDNSHost::clsQuery::clsAnswerAccessor	answerAccessor = _getAnswerAccessor(p_hServiceQuery, p_u32AnswerIndex);
    return (answerAccessor.IPv4AddressAvailable()
            ? answerAccessor.IPv4Addresses()[p_u32AddressIndex]
            : IPAddress());
}
#endif
#ifdef MDNS_IP6_SUPPORT
/*
    clsLEAMDNSHost_Legacy::hasAnswerIP6Address

*/
bool clsLEAMDNSHost_Legacy::hasAnswerIP6Address(const clsLEAMDNSHost_Legacy::hMDNSServiceQuery p_hServiceQuery,
        const uint32_t p_u32AnswerIndex)
{
    return _getAnswerAccessor(p_hServiceQuery, p_u32AnswerIndex).IPv6AddressAvailable();
}

/*
    clsLEAMDNSHost_Legacy::answerIP6AddressCount

*/
uint32_t clsLEAMDNSHost_Legacy::answerIP6AddressCount(const clsLEAMDNSHost_Legacy::hMDNSServiceQuery p_hServiceQuery,
        const uint32_t p_u32AnswerIndex)
{
    const clsLEAMDNSHost::clsQuery::clsAnswerAccessor	answerAccessor = _getAnswerAccessor(p_hServiceQuery, p_u32AnswerIndex);
    return (answerAccessor.IPv6AddressAvailable()
            ? answerAccessor.IPv6Addresses().size()
            : 0);
}

/*
    clsLEAMDNSHost_Legacy::answerIP6Address

*/
IPAddress clsLEAMDNSHost_Legacy::answerIP6Address(const clsLEAMDNSHost_Legacy::hMDNSServiceQuery p_hServiceQuery,
        const uint32_t p_u32AnswerIndex,
        const uint32_t p_u32AddressIndex)
{
    const clsLEAMDNSHost::clsQuery::clsAnswerAccessor	answerAccessor = _getAnswerAccessor(p_hServiceQuery, p_u32AnswerIndex);
    return (answerAccessor.IPv6AddressAvailable()
            ? answerAccessor.IPv6Addresses()[p_u32AddressIndex]
            : IPAddress());
}
#endif

/*
    clsLEAMDNSHost_Legacy::hasAnswerPort

*/
bool clsLEAMDNSHost_Legacy::hasAnswerPort(const clsLEAMDNSHost_Legacy::hMDNSServiceQuery p_hServiceQuery,
        const uint32_t p_u32AnswerIndex)
{
    return _getAnswerAccessor(p_hServiceQuery, p_u32AnswerIndex).hostPortAvailable();
}

/*
    clsLEAMDNSHost_Legacy::answerPort

*/
uint16_t clsLEAMDNSHost_Legacy::answerPort(const clsLEAMDNSHost_Legacy::hMDNSServiceQuery p_hServiceQuery,
        const uint32_t p_u32AnswerIndex)
{
    const clsLEAMDNSHost::clsQuery::clsAnswerAccessor	answerAccessor = _getAnswerAccessor(p_hServiceQuery, p_u32AnswerIndex);
    return (answerAccessor.hostPortAvailable()
            ? answerAccessor.hostPort()
            : 0);
}

/*
    clsLEAMDNSHost_Legacy::hasAnswerTxts

*/
bool clsLEAMDNSHost_Legacy::hasAnswerTxts(const clsLEAMDNSHost_Legacy::hMDNSServiceQuery p_hServiceQuery,
        const uint32_t p_u32AnswerIndex)
{
    return _getAnswerAccessor(p_hServiceQuery, p_u32AnswerIndex).txtsAvailable();
}

/*
    clsLEAMDNSHost_Legacy::answerHostDomain

    Get the TXT items as a ';'-separated string
*/
const char* clsLEAMDNSHost_Legacy::answerTxts(const clsLEAMDNSHost_Legacy::hMDNSServiceQuery p_hServiceQuery,
        const uint32_t p_u32AnswerIndex)
{
    const clsLEAMDNSHost::clsQuery::clsAnswerAccessor	answerAccessor = _getAnswerAccessor(p_hServiceQuery, p_u32AnswerIndex);
    return (answerAccessor.txtsAvailable()
            ? answerAccessor.txts()
            : 0);
}


/*

    HOST/SERVICE PROBE CALLBACKS

*/

/*
    clsLEAMDNSHost_Legacy::setHostProbeResultCallback

*/
bool clsLEAMDNSHost_Legacy::setHostProbeResultCallback(clsLEAMDNSHost_Legacy::MDNSHostProbeResultCallbackFn p_fnCallback)
{
    bool	bResult = true;

    clsLEAMDNSHost::stProbeResultCallback =
        [p_fnCallback](clsLEAMDNSHost& /*p_rHost*/,
                       const char* p_pcDomainName,
                       bool p_bProbeResult)->void
    {
        if (p_fnCallback)	// void(const char* p_pcDomainName, bool p_bProbeResult)
        {
            p_fnCallback(p_pcDomainName, p_bProbeResult);
        }
    };


    for (stcHostInformation::list::iterator it = m_HostInformations.begin(); ((bResult) && (it != m_HostInformations.end())); ++it)
    {
        bResult = (*it).m_pHost->setProbeResultCallback(clsLEAMDNSHost::stProbeResultCallback);
    }
    return bResult;
}

/*
    clsLEAMDNSHost_Legacy::setHostProbeResultCallback

*/
bool clsLEAMDNSHost_Legacy::setHostProbeResultCallback(clsLEAMDNSHost_Legacy::MDNSHostProbeResultCallbackFn2 p_fnCallback)
{
    bool	bResult = true;

    clsLEAMDNSHost::stProbeResultCallback =
        [this, p_fnCallback](clsLEAMDNSHost& /*p_rHost*/,
                             const char* p_pcDomainName,
                             bool p_bProbeResult)->void
    {
        if (p_fnCallback)	// void(clsLEAMDNSHost_Legacy* p_pMDNSResponder, const char* p_pcDomainName, bool p_bProbeResult)
        {
            p_fnCallback(this, p_pcDomainName, p_bProbeResult);
        }
    };

    for (stcHostInformation::list::iterator it = m_HostInformations.begin(); ((bResult) && (it != m_HostInformations.end())); ++it)
    {
        bResult = (*it).m_pHost->setProbeResultCallback(clsLEAMDNSHost::stProbeResultCallback);
    }
    return bResult;
}

/*
    clsLEAMDNSHost_Legacy::setServiceProbeResultCallback

*/
bool clsLEAMDNSHost_Legacy::setServiceProbeResultCallback(const clsLEAMDNSHost_Legacy::hMDNSService p_hService,
        clsLEAMDNSHost_Legacy::MDNSServiceProbeResultCallbackFn p_fnCallback)
{
    bool	bResult = true;

    for (stcHostInformation::list::iterator it = m_HostInformations.begin(); ((bResult) && (it != m_HostInformations.end())); ++it)
    {
        clsLEAMDNSHost::clsService*	pService = 0;
        bResult = (((pService = (clsLEAMDNSHost::clsService*)(*it).m_HandleToPtr[p_hService]))
                   && (pService->setProbeResultCallback(

                           [this, p_hService, p_fnCallback](clsLEAMDNSHost::clsService& /*p_rMDNSService*/,
                                   const char* p_pcInstanceName,
                                   bool p_bProbeResult)->void
        {
            if (p_fnCallback)	// void(const char* p_pcServiceName, const hMDNSService p_hMDNSService, bool p_bProbeResult)
            {
                p_fnCallback(p_pcInstanceName, p_hService, p_bProbeResult);
            }
        }
                       )));
    }
    return bResult;
}

/*
    clsLEAMDNSHost_Legacy::setServiceProbeResultCallback

*/
bool clsLEAMDNSHost_Legacy::setServiceProbeResultCallback(const clsLEAMDNSHost_Legacy::hMDNSService p_hService,
        clsLEAMDNSHost_Legacy::MDNSServiceProbeResultCallbackFn2 p_fnCallback)
{
    bool	bResult = true;

    for (stcHostInformation::list::iterator it = m_HostInformations.begin(); ((bResult) && (it != m_HostInformations.end())); ++it)
    {
        clsLEAMDNSHost::clsService*	pService = 0;
        bResult = (((pService = (clsLEAMDNSHost::clsService*)(*it).m_HandleToPtr[p_hService]))
                   && (pService->setProbeResultCallback([this, p_hService, p_fnCallback](clsLEAMDNSHost::clsService& /*p_rMDNSService*/,
                           const char* p_pcInstanceName,
                           bool p_bProbeResult)->void
        {
            if (p_fnCallback)	// void((clsLEAMDNSHost_Legacy* p_pMDNSResponder, const char* p_pcServiceName, const hMDNSService p_hMDNSService, bool p_bProbeResult)
            {
                p_fnCallback(this, p_pcInstanceName, p_hService, p_bProbeResult);
            }
        })));
    }
    return bResult;
}


/*

    PROCESS

*/

/*
    clsLEAMDNSHost_Legacy::notifyAPChange

*/
bool clsLEAMDNSHost_Legacy::notifyAPChange(void)
{
    bool	bResult = true;

    for (stcHostInformation::list::iterator it = m_HostInformations.begin(); ((bResult) && (it != m_HostInformations.end())); ++it)
    {
        bResult = (*it).m_pHost->restart();
    }
    return bResult;
}

/*
    clsLEAMDNSHost_Legacy::update

*/
bool clsLEAMDNSHost_Legacy::update(void)
{
    bool	bResult = true;

    for (stcHostInformation::list::iterator it = m_HostInformations.begin(); ((bResult) && (it != m_HostInformations.end())); ++it)
    {
        bResult = (*it).m_pHost->update();
    }
    return bResult;
}

/*
    clsLEAMDNSHost_Legacy::announce

*/
bool clsLEAMDNSHost_Legacy::announce(void)
{
    bool	bResult = true;

    for (stcHostInformation::list::iterator it = m_HostInformations.begin(); ((bResult) && (it != m_HostInformations.end())); ++it)
    {
        bResult = (*it).m_pHost->announce(true, true);
    }
    return bResult;
}

/*
    clsLEAMDNSHost_Legacy::enableArduino

*/
clsLEAMDNSHost_Legacy::hMDNSService clsLEAMDNSHost_Legacy::enableArduino(uint16_t p_u16Port,
        bool p_bAuthUpload /*= false*/)
{
    hMDNSService    hService = addService(0, "arduino", "tcp", p_u16Port);
    if (hService)
    {
        if ((!addServiceTxt(hService, "tcp_check", "no"))
                || (!addServiceTxt(hService, "ssh_upload", "no"))
                || (!addServiceTxt(hService, "board", STRINGIZE_VALUE_OF(ARDUINO_BOARD)))
                || (!addServiceTxt(hService, "auth_upload", (p_bAuthUpload) ? "yes" : "no")))
        {
            removeService(hService);
            hService = 0;
        }
    }
    return hService;
}

/*
    clsLEAMDNSHost_Legacy::indexDomain

*/
bool clsLEAMDNSHost_Legacy::indexDomain(char*& p_rpcDomain,
                                        const char* p_pcDivider /*= "-"*/,
                                        const char* p_pcDefaultDomain /*= 0*/)
{
    bool	bResult = false;

    const char*	cpcDomainName = clsLEAMDNSHost::indexDomainName(p_rpcDomain, p_pcDivider, p_pcDefaultDomain);
    delete[] p_rpcDomain;
    p_rpcDomain = 0;
    if ((cpcDomainName)
            && ((p_rpcDomain = new char[strlen(cpcDomainName) + 1])))
    {
        strcpy(p_rpcDomain, cpcDomainName);
        bResult = true;
    }
    return bResult;
}


/*

    INTERNAL HELPERS

*/

/*
    clsLEAMDNSHost_Legacy::_addServiceTxt (char*)

*/
clsLEAMDNSHost_Legacy::hMDNSTxt clsLEAMDNSHost_Legacy::_addServiceTxt(const hMDNSService p_hService,
        const char* p_pcKey,
        const char* p_pcValue,
        bool p_bDynamic)
{
    hMDNSTxt	hResult = 0;

    for (stcHostInformation& hostInformation : m_HostInformations)
    {
        clsLEAMDNSHost::clsService*		pService = 0;
        clsLEAMDNSHost::clsServiceTxt*	pTxt = 0;
        if (((pService = (clsLEAMDNSHost::clsService*)hostInformation.m_HandleToPtr[p_hService]))
                && ((pTxt = (p_bDynamic
                             ? pService->addDynamicServiceTxt(p_pcKey, p_pcValue)
                             : pService->addServiceTxt(p_pcKey, p_pcValue)))))
        {
            if (!hResult)
            {
                hResult = (hMDNSTxt)pTxt;
            }
            hostInformation.m_HandleToPtr[hResult] = pTxt;
        }
    }
    return hResult;
}

/*
    clsLEAMDNSHost_Legacy::_addServiceTxt (uint32_t)

*/
clsLEAMDNSHost_Legacy::hMDNSTxt clsLEAMDNSHost_Legacy::_addServiceTxt(const hMDNSService p_hService,
        const char* p_pcKey,
        uint32_t p_u32Value,
        bool p_bDynamic)
{
    char    acValueBuffer[16];  // 32-bit max 10 digits
    *acValueBuffer = 0;
    sprintf(acValueBuffer, "%u", p_u32Value);

    return _addServiceTxt(p_hService, p_pcKey, acValueBuffer, p_bDynamic);
}

/*
    clsLEAMDNSHost_Legacy::_addServiceTxt (uint16_t)

*/
clsLEAMDNSHost_Legacy::hMDNSTxt clsLEAMDNSHost_Legacy::_addServiceTxt(const hMDNSService p_hService,
        const char* p_pcKey,
        uint16_t p_u16Value,
        bool p_bDynamic)
{
    char    acValueBuffer[8];  // 16-bit max 5 digits
    *acValueBuffer = 0;
    sprintf(acValueBuffer, "%hu", p_u16Value);

    return _addServiceTxt(p_hService, p_pcKey, acValueBuffer, p_bDynamic);
}

/*
    clsLEAMDNSHost_Legacy::_addServiceTxt (uint8_t)

*/
clsLEAMDNSHost_Legacy::hMDNSTxt clsLEAMDNSHost_Legacy::_addServiceTxt(const hMDNSService p_hService,
        const char* p_pcKey,
        uint8_t p_u8Value,
        bool p_bDynamic)
{
    char    acValueBuffer[8];  // 8-bit max 3 digits
    *acValueBuffer = 0;
    sprintf(acValueBuffer, "%hhu", p_u8Value);

    return _addServiceTxt(p_hService, p_pcKey, acValueBuffer, p_bDynamic);
}

/*
    clsLEAMDNSHost_Legacy::_addServiceTxt (int32_t)

*/
clsLEAMDNSHost_Legacy::hMDNSTxt clsLEAMDNSHost_Legacy::_addServiceTxt(const hMDNSService p_hService,
        const char* p_pcKey,
        int32_t p_i32Value,
        bool p_bDynamic)
{
    char    acValueBuffer[16];  // 32-bit max 11 digits
    *acValueBuffer = 0;
    sprintf(acValueBuffer, "%i", p_i32Value);

    return _addServiceTxt(p_hService, p_pcKey, acValueBuffer, p_bDynamic);
}

/*
    clsLEAMDNSHost_Legacy::_addServiceTxt (int16_t)

*/
clsLEAMDNSHost_Legacy::hMDNSTxt clsLEAMDNSHost_Legacy::_addServiceTxt(const hMDNSService p_hService,
        const char* p_pcKey,
        int16_t p_i16Value,
        bool p_bDynamic)
{
    char    acValueBuffer[8];  // 16-bit max 6 digits
    *acValueBuffer = 0;
    sprintf(acValueBuffer, "%hi", p_i16Value);

    return _addServiceTxt(p_hService, p_pcKey, acValueBuffer, p_bDynamic);
}

/*
    clsLEAMDNSHost_Legacy::_addServiceTxt (int8_t)

*/
clsLEAMDNSHost_Legacy::hMDNSTxt clsLEAMDNSHost_Legacy::_addServiceTxt(const hMDNSService p_hService,
        const char* p_pcKey,
        int8_t p_i8Value,
        bool p_bDynamic)
{
    char    acValueBuffer[8];  // 8-bit max 4 digits
    *acValueBuffer = 0;
    sprintf(acValueBuffer, "%hhi", p_i8Value);

    return _addServiceTxt(p_hService, p_pcKey, acValueBuffer, p_bDynamic);
}

/*
    clsLEAMDNSHost_Legacy::_answerFlagsToAnswerType

*/
clsLEAMDNSHost_Legacy::AnswerType clsLEAMDNSHost_Legacy::_answerFlagsToAnswerType(clsLEAMDNSHost::clsQuery::clsAnswer::typeQueryAnswerType p_QueryAnswerTypeFlags) const
{
    AnswerType    answerType = AnswerType::Unknown;

    if (static_cast<clsLEAMDNSHost::clsQuery::clsAnswer::typeQueryAnswerType>(clsLEAMDNSHost::clsQuery::clsAnswer::enuQueryAnswerType::Unknown) & p_QueryAnswerTypeFlags)
    {
        answerType = AnswerType::Unknown;
    }
    else if (static_cast<clsLEAMDNSHost::clsQuery::clsAnswer::typeQueryAnswerType>(clsLEAMDNSHost::clsQuery::clsAnswer::enuQueryAnswerType::ServiceDomain) & p_QueryAnswerTypeFlags)
    {
        answerType = AnswerType::ServiceDomain;
    }
    else if (static_cast<clsLEAMDNSHost::clsQuery::clsAnswer::typeQueryAnswerType>(clsLEAMDNSHost::clsQuery::clsAnswer::enuQueryAnswerType::HostDomain) & p_QueryAnswerTypeFlags)
    {
        answerType = AnswerType::HostDomainAndPort;
    }
    else if (static_cast<clsLEAMDNSHost::clsQuery::clsAnswer::typeQueryAnswerType>(clsLEAMDNSHost::clsQuery::clsAnswer::enuQueryAnswerType::Port) & p_QueryAnswerTypeFlags)
    {
        answerType = AnswerType::HostDomainAndPort;
    }
    else if (static_cast<clsLEAMDNSHost::clsQuery::clsAnswer::typeQueryAnswerType>(clsLEAMDNSHost::clsQuery::clsAnswer::enuQueryAnswerType::Txts) & p_QueryAnswerTypeFlags)
    {
        answerType = AnswerType::Txt;
    }
#ifdef MDNS_IP4_SUPPORT
    else if (static_cast<clsLEAMDNSHost::clsQuery::clsAnswer::typeQueryAnswerType>(clsLEAMDNSHost::clsQuery::clsAnswer::enuQueryAnswerType::IPv4Address) & p_QueryAnswerTypeFlags)
    {
        answerType = AnswerType::IP4Address;
    }
#endif
#ifdef MDNS_IP6_SUPPORT
    else if (static_cast<clsLEAMDNSHost::clsQuery::clsAnswer::typeQueryAnswerType>(clsLEAMDNSHost::clsQuery::clsAnswer::enuQueryAnswerType::IPv6Address) & p_QueryAnswerTypeFlags)
    {
        answerType = AnswerType::IP6Address;
    }
#endif
    return answerType;
}

/*
    clsLEAMDNSHost_Legacy::_getAnswerAccessor

*/
clsLEAMDNSHost_Legacy::clsLEAMDNSHost::clsQuery::clsAnswerAccessor clsLEAMDNSHost_Legacy::_getAnswerAccessor(const uint32_t p_u32AnswerIndex)
{
    uint32_t	u32AnswerIndexWithoutOffset = p_u32AnswerIndex;
    for (stcHostInformation& hostInformation : m_HostInformations)
    {
        clsLEAMDNSHost::clsQuery*	pQuery = hostInformation.m_pHost->getQuery();
        if (pQuery)
        {
            if (pQuery->answerCount() > u32AnswerIndexWithoutOffset)
            {
                return pQuery->answerAccessor(u32AnswerIndexWithoutOffset);
            }
            else
            {
                u32AnswerIndexWithoutOffset -= pQuery->answerCount();
            }
        }
        else
        {
            break;
        }
    }
    return clsLEAMDNSHost::clsQuery::clsAnswerAccessor(0);
}

/*
    clsLEAMDNSHost_Legacy::_getAnswerAccessor

*/
clsLEAMDNSHost_Legacy::clsLEAMDNSHost::clsQuery::clsAnswerAccessor clsLEAMDNSHost_Legacy::_getAnswerAccessor(const clsLEAMDNSHost_Legacy::hMDNSServiceQuery p_hServiceQuery,
        const uint32_t p_u32AnswerIndex)
{
    uint32_t	u32AnswerIndexWithoutOffset = p_u32AnswerIndex;
    for (stcHostInformation& hostInformation : m_HostInformations)
    {
        clsLEAMDNSHost::clsQuery*	pQuery = (clsLEAMDNSHost::clsQuery*)hostInformation.m_HandleToPtr[p_hServiceQuery];
        if (pQuery)
        {
            if (pQuery->answerCount() > u32AnswerIndexWithoutOffset)
            {
                return pQuery->answerAccessor(u32AnswerIndexWithoutOffset);
            }
            else
            {
                u32AnswerIndexWithoutOffset -= pQuery->answerCount();
            }
        }
        else
        {
            break;
        }
    }
    return clsLEAMDNSHost::clsQuery::clsAnswerAccessor(0);
}


}	// namespace MDNSImplementation


}	// namespace experimental // esp8266






