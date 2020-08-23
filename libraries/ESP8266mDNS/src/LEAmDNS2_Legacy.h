/*
    LEAmDNS2_Legacy.h
    (c) 2020, LaborEtArs

    Version 0.9 beta

    Some notes (from LaborEtArs, 2018):
    Essentially, this is an rewrite of the original EPS8266 Multicast DNS code (ESP8266mDNS).
    The target of this rewrite was to keep the existing interface as stable as possible while
    adding and extending the supported set of mDNS features.
    A lot of the additions were basicly taken from Erik Ekman's lwIP mdns app code.

    Supported mDNS features (in some cases somewhat limited):
    - Presenting a DNS-SD service to interested observers, eg. a http server by presenting _http._tcp service
    - Support for multi-level compressed names in input; in output only a very simple one-leven full-name compression is implemented
    - Probing host and service domains for uniqueness in the local network
    - Tiebreaking while probing is supported in a very minimalistic way (the 'higher' IP address wins the tiebreak)
    - Announcing available services after successful probing
    - Using fixed service TXT items or
    - Using dynamic service TXT items for presented services (via callback)
    - Remove services (and un-announcing them to the observers by sending goodbye-messages)
    - Static queries for DNS-SD services (creating a fixed answer set after a certain timeout period)
    - Dynamic queries for DNS-SD services with cached and updated answers and user notifications


    Usage:
    In most cases, this implementation should work as a 'drop-in' replacement for the original
    ESP8266 Multicast DNS code. Adjustments to the existing code would only be needed, if some
    of the new features should be used.

    For presenting services:
    In 'setup()':
      Install a callback for the probing of host (and service) domains via 'MDNS.setProbeResultCallback(probeResultCallback, &userData);'
      Register DNS-SD services with 'clsLEAMDNSHost_Legacy::hMDNSService hService = MDNS.addService("MyESP", "http", "tcp", 5000);'
      (Install additional callbacks for the probing of these service domains via 'MDNS.setServiceProbeResultCallback(hService, probeResultCallback, &userData);')
      Add service TXT items with 'MDNS.addServiceTxt(hService, "c#", "1");' or by installing a service TXT callback
      using 'MDNS.setDynamicServiceTxtCallback(dynamicServiceTxtCallback, &userData);' or service specific
      'MDNS.setDynamicServiceTxtCallback(hService, dynamicServiceTxtCallback, &userData);'
      Call MDNS.begin("MyHostname");

    In 'probeResultCallback(clsLEAMDNSHost_Legacy* p_MDNSResponder, const char* p_pcDomain, clsLEAMDNSHost_Legacy:hMDNSService p_hService, bool p_bProbeResult, void* p_pUserdata)':
      Check the probe result and update the host or service domain name if the probe failed

    In 'dynamicServiceTxtCallback(clsLEAMDNSHost_Legacy* p_MDNSResponder, const hMDNSService p_hService, void* p_pUserdata)':
      Add dynamic TXT items by calling 'MDNS.addDynamicServiceTxt(p_hService, "c#", "1");'

    In loop():
      Call 'MDNS.update();'


    For querying services:
    Static:
      Call 'uint32_t u32AnswerCount = MDNS.queryService("http", "tcp");'
      Iterate answers by: 'for (uint32_t u=0; u<u32AnswerCount; ++u) { const char* pHostname = MDNS.answerHostname(u); }'
      You should call MDNS.removeQuery() sometimes later (when the answers are nott needed anymore)

    Dynamic:
      Install a dynamic query by calling 'DNSResponder::hMDNSServiceQuery hServiceQuery = MDNS.installServiceQuery("http", "tcp", serviceQueryCallback, &userData);'
      The callback 'serviceQueryCallback(clsLEAMDNSHost_Legacy* p_MDNSResponder, const hMDNSServiceQuery p_hServiceQuery, uint32_t p_u32AnswerIndex,
                                         enuServiceQueryAnswerType p_ServiceQueryAnswerType, bool p_bSetContent, void* p_pUserdata)'
      is called for any change in the answer set.
      Call 'MDNS.removeServiceQuery(hServiceQuery);' when the answers are not needed anymore


    Reference:
    Used mDNS messages:
    A (0x01):               eg. esp8266.local A OP TTL 123.456.789.012
    AAAA (0x1C):            eg. esp8266.local AAAA OP TTL 1234:5678::90
    PTR (0x0C, srv name):   eg. _http._tcp.local PTR OP TTL MyESP._http._tcp.local
    PTR (0x0C, srv type):   eg. _services._dns-sd._udp.local PTR OP TTL _http._tcp.local
    PTR (0x0C, IP4):        eg. 012.789.456.123.in-addr.arpa PTR OP TTL esp8266.local
    PTR (0x0C, IP6):        eg. 90.0.0.0.0.0.0.0.0.0.0.0.78.56.34.12.ip6.arpa PTR OP TTL esp8266.local
    SRV (0x21):             eg. MyESP._http._tcp.local SRV OP TTL PRIORITY WEIGHT PORT esp8266.local
    TXT (0x10):             eg. MyESP._http._tcp.local TXT OP TTL c#=1
    NSEC (0x2F):            eg. esp8266.local ... (DNSSEC)

    Some NOT used message types:
    OPT (0x29):             eDNS


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

#ifndef __LEAMDNS2HOST_LEGACY_H__
#define __LEAMDNS2HOST_LEGACY_H__

#include "LEAmDNS2Host.h"


namespace experimental
{

namespace MDNSImplementation
{

/**
    clsLEAMDNSHost_Legacy
*/
class clsLEAMDNSHost_Legacy
{
    using clsLEAMDNSHost = esp8266::experimental::clsLEAMDNSHost;

    /**
        clsConsts
    */
    class clsConsts
    {
    public:
        static const uint16_t   u16StaticQueryWaitTime  = 1000;     // Default wait time for query responses
    };

public:

    /* INTERFACE */
    clsLEAMDNSHost_Legacy(void);
    virtual ~clsLEAMDNSHost_Legacy(void);

    // Start the MDNS responder by setting the default hostname
    // Later call MDNS::update() in every 'loop' to run the process loop
    // (probing, announcing, responding, ...)
    bool begin(const char* p_pcHostname);
    bool begin(const String& p_strHostname);
    // for compatibility
    bool begin(const char* p_pcHostname,
               IPAddress p_IPAddress /*= INADDR_ANY*/,	// ignored
               uint32_t p_u32TTL /*= 120*/);    		// ignored
    bool begin(const String& p_strHostname,
               IPAddress p_IPAddress /*= INADDR_ANY*/,	// ignored
               uint32_t p_u32TTL /*= 120*/);    		// ignored
    // Finish MDNS processing
    bool close(void);
    // for ESP32 compatibility
    bool end(void);
    // NEW! The ESP-default network interfaces are 'auto-added' via 'begin' when active
    // Additional netifs may be added, but if done so after calling 'update' for the
    // first time, 'notifyAPChange' should be called to restart the probing/announcing process
    bool addHostForNetIf(const char* p_pcHostname);

    // Change hostname (probing is restarted)
    // Caution! The final hostname (after probing) may be different for every host
    bool setHostname(const char* p_pcHostname);
    // for compatibility...
    bool setHostname(String p_strHostname);

    const char* hostname(void) const;

    bool status(void) const;

    /**
        hMDNSService (opaque handle to access the service)
    */
    typedef const void*     hMDNSService;

    // Add a new service to the MDNS responder. If no name (instance name) is given (p_pcName = 0)
    // the current hostname is used. If the hostname is changed later, the instance names for
    // these 'auto-named' services are changed to the new name also (and probing is restarted).
    // The usual '_' before p_pcService (eg. http) and protocol (eg. tcp) may be given.
    hMDNSService addService(const char* p_pcName,
                            const char* p_pcService,
                            const char* p_pcProtocol,
                            uint16_t p_u16Port);
    // Removes a service from the MDNS responder
    bool removeService(const hMDNSService p_hService);
    bool removeService(const char* p_pcInstanceName,
                       const char* p_pcServiceName,
                       const char* p_pcProtocol);
    // for compatibility...
    bool addService(String p_strServiceName,
                    String p_strProtocol,
                    uint16_t p_u16Port);


    // Change the services instance name (and restart probing).
    bool setServiceName(const hMDNSService p_hService,
                        const char* p_pcInstanceName);
    //for compatibility
    void setInstanceName(const char* p_pcHostname);

    // for ESP32 compatibility
    void setInstanceName(const String& p_strHostname);

    const char* serviceName(const hMDNSService p_hService) const;
    const char* service(const hMDNSService p_hService) const;
    const char* serviceProtocol(const hMDNSService p_hService) const;

    bool serviceStatus(const hMDNSService p_hService) const;

    /**
        hMDNSTxt (opaque handle to access the TXT items)
    */
    typedef void*   hMDNSTxt;

    // Add a (static) MDNS TXT item ('key' = 'value') to the service
    hMDNSTxt addServiceTxt(const hMDNSService p_hService,
                           const char* p_pcKey,
                           const char* p_pcValue);
    hMDNSTxt addServiceTxt(const hMDNSService p_hService,
                           const char* p_pcKey,
                           uint32_t p_u32Value);
    hMDNSTxt addServiceTxt(const hMDNSService p_hService,
                           const char* p_pcKey,
                           uint16_t p_u16Value);
    hMDNSTxt addServiceTxt(const hMDNSService p_hService,
                           const char* p_pcKey,
                           uint8_t p_u8Value);
    hMDNSTxt addServiceTxt(const hMDNSService p_hService,
                           const char* p_pcKey,
                           int32_t p_i32Value);
    hMDNSTxt addServiceTxt(const hMDNSService p_hService,
                           const char* p_pcKey,
                           int16_t p_i16Value);
    hMDNSTxt addServiceTxt(const hMDNSService p_hService,
                           const char* p_pcKey,
                           int8_t p_i8Value);

    // Remove an existing (static) MDNS TXT item from the service
    bool removeServiceTxt(const hMDNSService p_hService,
                          const hMDNSTxt p_hTxt);
    bool removeServiceTxt(const hMDNSService p_hService,
                          const char* p_pcKey);
    bool removeServiceTxt(const char* p_pcinstanceName,
                          const char* p_pcServiceName,
                          const char* p_pcProtocol,
                          const char* p_pcKey);
    // for compatibility...
    bool addServiceTxt(const char* p_pcService,
                       const char* p_pcProtocol,
                       const char* p_pcKey,
                       const char* p_pcValue);
    bool addServiceTxt(String p_strService,
                       String p_strProtocol,
                       String p_strKey,
                       String p_strValue);

    /**
        MDNSDynamicServiceTxtCallbackFn
        Callback function for dynamic MDNS TXT items
    */
    typedef std::function<void(const hMDNSService p_hService)>  MDNSDynamicServiceTxtCallbackFn;

    // Set a global callback for dynamic MDNS TXT items. The callback function is called
    // every time, a TXT item is needed for one of the installed services.
    bool setDynamicServiceTxtCallback(MDNSDynamicServiceTxtCallbackFn p_fnCallback);

    // Set a service specific callback for dynamic MDNS TXT items. The callback function
    // is called every time, a TXT item is needed for the given service.
    bool setDynamicServiceTxtCallback(const hMDNSService p_hService,
                                      MDNSDynamicServiceTxtCallbackFn p_fnCallback);

    // Add a (dynamic) MDNS TXT item ('key' = 'value') to the service
    // Dynamic TXT items are removed right after one-time use. So they need to be added
    // every time the value s needed (via callback).
    hMDNSTxt addDynamicServiceTxt(hMDNSService p_hService,
                                  const char* p_pcKey,
                                  const char* p_pcValue);
    hMDNSTxt addDynamicServiceTxt(hMDNSService p_hService,
                                  const char* p_pcKey,
                                  uint32_t p_u32Value);
    hMDNSTxt addDynamicServiceTxt(hMDNSService p_hService,
                                  const char* p_pcKey,
                                  uint16_t p_u16Value);
    hMDNSTxt addDynamicServiceTxt(hMDNSService p_hService,
                                  const char* p_pcKey,
                                  uint8_t p_u8Value);
    hMDNSTxt addDynamicServiceTxt(hMDNSService p_hService,
                                  const char* p_pcKey,
                                  int32_t p_i32Value);
    hMDNSTxt addDynamicServiceTxt(hMDNSService p_hService,
                                  const char* p_pcKey,
                                  int16_t p_i16Value);
    hMDNSTxt addDynamicServiceTxt(hMDNSService p_hService,
                                  const char* p_pcKey,
                                  int8_t p_i8Value);

    // Perform a (static) service query. The function returns after p_u16Timeout milliseconds
    // The answers (the number of received answers is returned) can be retrieved by calling
    // - answerHostname (or hostname)
    // - answerIP (or IP)
    // - answerPort (or port)
    uint32_t queryService(const char* p_pcService,
                          const char* p_pcProtocol,
                          const uint16_t p_u16Timeout = clsConsts::u16StaticQueryWaitTime);
    bool removeQuery(void);
    // for compatibility...
    uint32_t queryService(String p_strService,
                          String p_strProtocol);

    const char* answerHostname(const uint32_t p_u32AnswerIndex);
    IPAddress answerIP(const uint32_t p_u32AnswerIndex);
    uint16_t answerPort(const uint32_t p_u32AnswerIndex);
    // for compatibility...
    String hostname(const uint32_t p_u32AnswerIndex);
    IPAddress IP(const uint32_t p_u32AnswerIndex);
    uint16_t port(const uint32_t p_u32AnswerIndex);

    /**
        hMDNSServiceQuery (opaque handle to access dynamic service queries)
    */
    typedef const void*     hMDNSServiceQuery;

    /**
        enuServiceQueryAnswerType
    */
    typedef enum _enuServiceQueryAnswerType
    {
        ServiceQueryAnswerType_Unknown              = 0,
        ServiceQueryAnswerType_ServiceDomain        = (1 << 0), // Service instance name
        ServiceQueryAnswerType_HostDomainAndPort    = (1 << 1), // Host domain and service port
        ServiceQueryAnswerType_Txts                 = (1 << 2), // TXT items
#ifdef MDNS_IP4_SUPPORT
        ServiceQueryAnswerType_IP4Address           = (1 << 3), // IP4 address
#endif
#ifdef MDNS_IP6_SUPPORT
        ServiceQueryAnswerType_IP6Address           = (1 << 4), // IP6 address
#endif
    } enuServiceQueryAnswerType;

    /**
        AnswerType (std::map compatible version)
    */
    enum class AnswerType : uint32_t
    {
        Unknown             = ServiceQueryAnswerType_Unknown,
        ServiceDomain       = ServiceQueryAnswerType_ServiceDomain,
        HostDomainAndPort   = ServiceQueryAnswerType_HostDomainAndPort,
        Txt                 = ServiceQueryAnswerType_Txts,
#ifdef MDNS_IP4_SUPPORT
        IP4Address          = ServiceQueryAnswerType_IP4Address,
#endif
#ifdef MDNS_IP6_SUPPORT
        IP6Address          = ServiceQueryAnswerType_IP6Address
#endif
    };

    /**
        stcMDNSServiceInfo
    */
    struct stcMDNSServiceInfo
    {
        stcMDNSServiceInfo(const clsLEAMDNSHost::clsQuery::clsAnswerAccessor& p_rAnswerAccessor)
            :   m_rAnswerAccessor(p_rAnswerAccessor) {};
        /**
            stcCompareKey
        */
        struct stcCompareKey
        {
            /*
                operator ()
            */
            bool operator()(char const* p_pA, char const* p_pB) const
            {
                return (0 > strcmp(p_pA, p_pB));
            }
        };
        /**
            clsKeyValueMap
        */
        using clsKeyValueMap = std::map<const char*, const char*, stcCompareKey>;

    protected:
        const clsLEAMDNSHost::clsQuery::clsAnswerAccessor&	m_rAnswerAccessor;
        clsKeyValueMap      				                m_KeyValueMap;

    public:
        /*
            serviceDomain
        */
        const char* serviceDomain(void) const
        {
            return (m_rAnswerAccessor.serviceDomainAvailable()
                    ? m_rAnswerAccessor.serviceDomain()
                    : nullptr);
        }
        /*
            hostDomainAvailable
        */
        bool hostDomainAvailable(void) const
        {
            return m_rAnswerAccessor.hostDomainAvailable();
        }
        /*
            hostDomain
        */
        const char* hostDomain(void) const
        {
            return (hostDomainAvailable()
                    ? m_rAnswerAccessor.hostDomain()
                    : nullptr);
        }
        /*
            hostPortAvailable
        */
        bool hostPortAvailable(void) const
        {
            return m_rAnswerAccessor.hostPortAvailable();
        }
        /*
            hostPort
        */
        uint16_t hostPort(void) const
        {
            return (hostPortAvailable()
                    ? m_rAnswerAccessor.hostPort()
                    : 0);
        }
#ifdef MDNS_IP4_SUPPORT
        /*
            IP4AddressAvailable
        */
        bool IP4AddressAvailable(void) const
        {
            return m_rAnswerAccessor.IPv4AddressAvailable();
        }
        /*
            IP4Addresses
        */
        std::vector<IPAddress> IP4Adresses(void) const
        {
            return (IP4AddressAvailable()
                    ? m_rAnswerAccessor.IPv4Addresses()
                    : std::vector<IPAddress>());
        }
#endif
#ifdef MDNS_IP6_SUPPORT
        /*
            IP6AddressAvailable
        */
        bool IP6AddressAvailable(void) const
        {
            return m_rAnswerAccessor.IPv6AddressAvailable();
        }
        /*
            IP6Addresses
        */
        std::vector<IPAddress> IP6Adresses(void) const
        {
            return (IP6AddressAvailable()
                    ? m_rAnswerAccessor.IPv6Addresses()
                    : std::vector<IPAddress>());
        }
#endif
        /*
            txtAvailable
        */
        bool txtAvailable(void) const
        {
            return m_rAnswerAccessor.txtsAvailable();
        }
        /*
            strKeyValue -> abc=def;hij=klm;
        */
        const char* strKeyValue(void) const
        {
            // TODO
            return nullptr;
        }
        /*
            keyValues -> abc=def hij=klm ...
        */
        const clsKeyValueMap& keyValues(void)
        {
            if ((txtAvailable()) &&
                    (0 == m_KeyValueMap.size()))
            {
                for (auto kv : m_rAnswerAccessor.txtKeyValues())
                {
                    m_KeyValueMap.emplace(std::pair<const char*, const char*>(kv.first, kv.second));
                }
                //for (auto kv=m_rMDNSResponder._answerKeyValue(m_hServiceQuery, m_u32AnswerIndex); kv!=nullptr; kv=kv->m_pNext) {
                //    m_KeyValueMap.emplace(std::pair<const char*,const char*>(kv->m_pcKey, kv->m_pcValue));
                //}
            }
            return m_KeyValueMap;
        }
        /*
            value (abc)->def
        */
        const char* value(const char* p_pcKey) const
        {
            return m_rAnswerAccessor.txtValue(p_pcKey);
        }
    };

    /**
        MDNSServiceQueryCallbackFn

        Callback function for received answers for dynamic service queries
    */
    typedef std::function<void(const stcMDNSServiceInfo& p_MDNSServiceInfo,
                               AnswerType p_AnswerType,						// flags for the updated answer item
                               bool p_bSetContent                     		// true: Answer component set, false: component deleted
                              )>    MDNSServiceQueryCallbackFn;

    // Install a dynamic service query. For every received answer (part) the given callback
    // function is called. The query will be updated every time, the TTL for an answer
    // has timed-out.
    // The answers can also be retrieved by calling
    // - answerCount
    // - answerServiceDomain
    // - hasAnswerHostDomain/answerHostDomain
    // - hasAnswerIP4Address/answerIP4Address
    // - hasAnswerIP6Address/answerIP6Address
    // - hasAnswerPort/answerPort
    // - hasAnswerTxts/answerTxts
    hMDNSServiceQuery installServiceQuery(const char* p_pcService,
                                          const char* p_pcProtocol,
                                          MDNSServiceQueryCallbackFn p_fnCallback);
    // Remove a dynamic service query
    bool removeServiceQuery(hMDNSServiceQuery p_hServiceQuery);

    uint32_t answerCount(const hMDNSServiceQuery p_hServiceQuery);
    std::vector<stcMDNSServiceInfo> answerInfo(const clsLEAMDNSHost_Legacy::hMDNSServiceQuery p_hServiceQuery);
    const char* answerServiceDomain(const hMDNSServiceQuery p_hServiceQuery,
                                    const uint32_t p_u32AnswerIndex);
    bool hasAnswerHostDomain(const hMDNSServiceQuery p_hServiceQuery,
                             const uint32_t p_u32AnswerIndex);
    const char* answerHostDomain(const hMDNSServiceQuery p_hServiceQuery,
                                 const uint32_t p_u32AnswerIndex);
#ifdef MDNS_IP4_SUPPORT
    bool hasAnswerIP4Address(const hMDNSServiceQuery p_hServiceQuery,
                             const uint32_t p_u32AnswerIndex);
    uint32_t answerIP4AddressCount(const hMDNSServiceQuery p_hServiceQuery,
                                   const uint32_t p_u32AnswerIndex);
    IPAddress answerIP4Address(const hMDNSServiceQuery p_hServiceQuery,
                               const uint32_t p_u32AnswerIndex,
                               const uint32_t p_u32AddressIndex);
#endif
#ifdef MDNS_IP6_SUPPORT
    bool hasAnswerIP6Address(const hMDNSServiceQuery p_hServiceQuery,
                             const uint32_t p_u32AnswerIndex);
    uint32_t answerIP6AddressCount(const hMDNSServiceQuery p_hServiceQuery,
                                   const uint32_t p_u32AnswerIndex);
    IPAddress answerIP6Address(const hMDNSServiceQuery p_hServiceQuery,
                               const uint32_t p_u32AnswerIndex,
                               const uint32_t p_u32AddressIndex);
#endif
    bool hasAnswerPort(const hMDNSServiceQuery p_hServiceQuery,
                       const uint32_t p_u32AnswerIndex);
    uint16_t answerPort(const hMDNSServiceQuery p_hServiceQuery,
                        const uint32_t p_u32AnswerIndex);
    bool hasAnswerTxts(const hMDNSServiceQuery p_hServiceQuery,
                       const uint32_t p_u32AnswerIndex);
    // Get the TXT items as a ';'-separated string
    const char* answerTxts(const hMDNSServiceQuery p_hServiceQuery,
                           const uint32_t p_u32AnswerIndex);

    /**
        MDNSHostProbeResultCallbackFn/2
        Callback function for host domain probe results
    */
    typedef std::function<void(const char* p_pcDomainName,
                               bool p_bProbeResult)> MDNSHostProbeResultCallbackFn;

    typedef std::function<void(clsLEAMDNSHost_Legacy* p_pMDNSResponder,
                               const char* p_pcDomainName,
                               bool p_bProbeResult)> MDNSHostProbeResultCallbackFn2;

    // Set a callback function for host probe results
    // The callback function is called, when the probeing for the host domain
    // succeededs or fails.
    // In case of failure, the failed domain name should be changed.
    bool setHostProbeResultCallback(MDNSHostProbeResultCallbackFn p_fnCallback);
    bool setHostProbeResultCallback(MDNSHostProbeResultCallbackFn2 p_fnCallback);

    /**
        MDNSServiceProbeResultCallbackFn/2
        Callback function for service domain probe results
    */
    typedef std::function<void(const char* p_pcServiceName,
                               const hMDNSService p_hMDNSService,
                               bool p_bProbeResult)> MDNSServiceProbeResultCallbackFn;

    typedef std::function<void(clsLEAMDNSHost_Legacy* p_pMDNSResponder,
                               const char* p_pcServiceName,
                               const hMDNSService p_hMDNSService,
                               bool p_bProbeResult)> MDNSServiceProbeResultCallbackFn2;

    // Set a service specific probe result callcack
    bool setServiceProbeResultCallback(const hMDNSService p_hService,
                                       MDNSServiceProbeResultCallbackFn p_fnCallback);
    bool setServiceProbeResultCallback(const hMDNSService p_hService,
                                       MDNSServiceProbeResultCallbackFn2 p_fnCallback);

    // Application should call this whenever AP is configured/disabled
    bool notifyAPChange(void);

    // 'update' should be called in every 'loop' to run the MDNS processing
    bool update(void);

    // 'announce' can be called every time, the configuration of some service
    // changes. Mainly, this would be changed content of TXT items.
    bool announce(void);

    // Enable OTA update
    hMDNSService enableArduino(uint16_t p_u16Port,
                               bool p_bAuthUpload = false);

    // Domain name helper
    static bool indexDomain(char*& p_rpcDomain,
                            const char* p_pcDivider = "-",
                            const char* p_pcDefaultDomain = 0);

protected:
    /**
        stcHostInformation
    */
    struct stcHostInformation
    {
        /**
            clsHandleToPtrMap
        */
        using clsHandleToPtrMap = std::map<const void*, const void*>;

        clsLEAMDNSHost*  	m_pHost;
        clsHandleToPtrMap	m_HandleToPtr;

        stcHostInformation(clsLEAMDNSHost* p_pHost)
            :	m_pHost(p_pHost)
        {}

        /**
            list
        */
        using list = std::list<stcHostInformation>;
    };

    stcHostInformation::list	m_HostInformations;

    // HELPERS
    hMDNSTxt _addServiceTxt(const hMDNSService p_hService,
                            const char* p_pcKey,
                            const char* p_pcValue,
                            bool p_bDynamic);
    hMDNSTxt _addServiceTxt(const hMDNSService p_hService,
                            const char* p_pcKey,
                            uint32_t p_u32Value,
                            bool p_bDynamic);
    hMDNSTxt _addServiceTxt(const hMDNSService p_hService,
                            const char* p_pcKey,
                            uint16_t p_u16Value,
                            bool p_bDynamic);
    hMDNSTxt _addServiceTxt(const hMDNSService p_hService,
                            const char* p_pcKey,
                            uint8_t p_u8Value,
                            bool p_bDynamic);
    hMDNSTxt _addServiceTxt(const hMDNSService p_hService,
                            const char* p_pcKey,
                            int32_t p_i32Value,
                            bool p_bDynamic);
    hMDNSTxt _addServiceTxt(const hMDNSService p_hService,
                            const char* p_pcKey,
                            int16_t p_i16Value,
                            bool p_bDynamic);
    hMDNSTxt _addServiceTxt(const hMDNSService p_hService,
                            const char* p_pcKey,
                            int8_t p_i8Value,
                            bool p_bDynamic);

    AnswerType _answerFlagsToAnswerType(clsLEAMDNSHost::clsQuery::clsAnswer::typeQueryAnswerType p_QueryAnswerTypeFlags) const;

    clsLEAMDNSHost::clsQuery::clsAnswerAccessor _getAnswerAccessor(const uint32_t p_u32AnswerIndex);
    clsLEAMDNSHost::clsQuery::clsAnswerAccessor _getAnswerAccessor(const hMDNSServiceQuery p_hServiceQuery,
            const uint32_t p_u32AnswerIndex);

};


}   // namespace MDNSImplementation


}   // namespace experimental


#endif // __LEAMDNS2HOST_LEGACY_H__






