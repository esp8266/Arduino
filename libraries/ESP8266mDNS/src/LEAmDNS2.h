/*
    LEAmDNS2.h
    (c) 2018, LaborEtArs

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
    - Tiebreaking while probing is supportet in a very minimalistic way (the 'higher' IP address wins the tiebreak)
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
      Register DNS-SD services with 'MDNSResponder::hMDNSService hService = MDNS.addService("MyESP", "http", "tcp", 5000);'
      (Install additional callbacks for the probing of these service domains via 'MDNS.setServiceProbeResultCallback(hService, probeResultCallback, &userData);')
      Add service TXT items with 'MDNS.addServiceTxt(hService, "c#", "1");' or by installing a service TXT callback
      using 'MDNS.setDynamicServiceTxtCallback(dynamicServiceTxtCallback, &userData);' or service specific
      'MDNS.setDynamicServiceTxtCallback(hService, dynamicServiceTxtCallback, &userData);'
      Call MDNS.begin("MyHostName");

    In 'probeResultCallback(MDNSResponder* p_MDNSResponder, const char* p_pcDomain, MDNSResponder:hMDNSService p_hMDNSService, bool p_bProbeResult, void* p_pUserdata)':
      Check the probe result and update the host or service domain name if the probe failed

    In 'dynamicServiceTxtCallback(MDNSResponder* p_MDNSResponder, const hMDNSService p_hMDNSService, void* p_pUserdata)':
      Add dynamic TXT items by calling 'MDNS.addDynamicServiceTxt(p_hMDNSService, "c#", "1");'

    In loop():
      Call 'MDNS.update();'


    For querying services/hosts:
    Static:
      Call 'uint32_t u32AnswerCount = MDNS.queryService("http", "tcp");' or 'MDNS.queryHost("esp8266")';
      Iterate answers by: 'for (uint32_t u=0; u<u32AnswerCount; ++u) { const char* pHostName = MDNS.answerHostName(u); }'
      You should call MDNS.removeQuery() sometimes later (when the answers are not needed anymore)

    Dynamic:
      Install a dynamic service query by calling 'DNSResponder::hMDNSQuery hQuery = MDNS.installServiceQuery("http", "tcp", serviceQueryCallback, &userData);'
      The callback 'serviceQueryCallback(MDNSResponder* p_MDNSResponder, const stcAnswerAccessor& p_MDNSAnswerAccessor, typeQueryAnswerType p_QueryAnswerTypeFlags, bool p_bSetContent)'
      is called for any change in the answer set.
      Call 'MDNS.removeQuery(hServiceQuery);' when the answers are not needed anymore


    Reference:
    Used mDNS messages:
    A (0x01):               eg. esp8266.local A OP TTL 123.456.789.012
    AAAA (0x1C):            eg. esp8266.local AAAA OP TTL 1234:5678::90
    PTR (0x0C, srv name):   eg. _http._tcp.local PTR OP TTL MyESP._http._tcp.local
    PTR (0x0C, srv type):   eg. _services._dns-sd._udp.local PTR OP TTL _http._tcp.local
    PTR (0x0C, IPv4):        eg. 012.789.456.123.in-addr.arpa PTR OP TTL esp8266.local
    PTR (0x0C, IPv6):        eg. 90.0.0.0.0.0.0.0.0.0.0.0.78.56.34.12.ip6.arpa PTR OP TTL esp8266.local
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

#ifndef LEAMDNS2_H
#define LEAMDNS2_H

#include <functional>   // for UdpContext.h
#include <list>
#include <limits>
#include <map>

#include "lwip/netif.h"
#include "WiFiUdp.h"
#include "lwip/udp.h"
#include "debug.h"
#include "include/UdpContext.h"
#include <PolledTimeout.h>

#include "ESP8266WiFi.h"


namespace esp8266
{

/**
    LEAmDNS
*/
namespace experimental
{

//this should be user-defined at build time
#ifndef ARDUINO_BOARD
#define ARDUINO_BOARD "generic"
#endif

#define MDNS_IPV4_SUPPORT
#if LWIP_IPV6
#define MDNS_IPV6_SUPPORT	// If we've got IPv6 support, then we need IPv6 support :-)
#endif


#ifdef MDNS_IPV4_SUPPORT
#define MDNS_IPV4_SIZE              	4
#endif
#ifdef MDNS_IPV6_SUPPORT
#define MDNS_IPV6_SIZE              	16
#endif
/*
    Maximum length for all service txts for one service
*/
#define MDNS_SERVICE_TXT_MAXLENGTH      1300
/*
    Maximum length for a full domain name eg. MyESP._http._tcp.local
*/
#define MDNS_DOMAIN_MAXLENGTH           256
/*
    Maximum length of on label in a domain name (length info fits into 6 bits)
*/
#define MDNS_DOMAIN_LABEL_MAXLENGTH     63
/*
    Maximum length of a service name eg. http
*/
#define MDNS_SERVICE_NAME_LENGTH        15
/*
    Maximum length of a service protocol name eg. tcp
*/
#define MDNS_SERVICE_PROTOCOL_LENGTH    3
/*
    Default timeout for static service queries
*/
#define MDNS_QUERYSERVICES_WAIT_TIME    5000

/*
    DNS_RRTYPE_NSEC
*/
#ifndef DNS_RRTYPE_NSEC
#define DNS_RRTYPE_NSEC             	0x2F
#endif


/**
    MDNSResponder
*/
class MDNSResponder
{
protected:
#include "LEAmDNS2_Host.hpp"

public:
    // MISC HELPERS
    // Domain name helper
    static bool indexDomain(char*& p_rpcDomain,
                            const char* p_pcDivider = "-",
                            const char* p_pcDefaultDomain = 0);
    // Host name helper
    static bool setNetIfHostName(netif* p_pNetIf,
                                 const char* p_pcHostName);

    // INTERFACE
    MDNSResponder(void);
    virtual ~MDNSResponder(void);

    // HANDLEs for opaque access to responder objects
    /**
    	hMDNSHost
    */
    using hMDNSHost = const void*;
    /**
        hMDNSService
    */
    using hMDNSService = const void*;
    /**
        hMDNSTxt
    */
    using hMDNSTxt = const void*;
    /**
        hMDNSQuery
    */
    using hMDNSQuery = const void*;

    // CALLBACKS
    /**
        MDNSHostProbeResultCallbackFn
        Callback function for host domain probe results
    */
    using MDNSHostProbeResultCallbackFn = std::function<void(MDNSResponder& p_rMDNSResponder,
                                                             const hMDNSHost p_hMDNSHost,
                                                             const char* p_pcDomainName,
                                                             bool p_bProbeResult)>;

    // Create a MDNS netif responder netif by setting the default hostname
    // Later call 'update()' in every 'loop' to run the process loop
    // (probing, announcing, responding, ...)
    // If no callback is given, the (maybe) already installed callback stays set
    hMDNSHost begin(const char* p_pcHostName,
                    netif* p_pNetIf,
                    MDNSHostProbeResultCallbackFn p_fnCallback = 0);
    bool begin(const char* p_pcHostName,
               WiFiMode_t p_WiFiMode,
               MDNSHostProbeResultCallbackFn p_fnCallback = 0);
    bool begin(const char* p_pcHostName,
               MDNSHostProbeResultCallbackFn p_fnCallback = 0);

    // Finish MDNS processing
    bool close(const hMDNSHost p_hMDNSHost);
    bool close(void);

    hMDNSHost getMDNSHost(netif* p_pNetIf) const;
    hMDNSHost getMDNSHost(WiFiMode_t p_WiFiMode) const;

    // Change hostname (probing is restarted)
    // If no callback is given, the (maybe) already installed callback stays set
    bool setHostName(const hMDNSHost p_hMDNSHost,
                     const char* p_pcHostName,
                     MDNSHostProbeResultCallbackFn p_fnCallback = 0);

    const char* hostName(const hMDNSHost p_hMDNSHost) const;

    // Set a callback function for host probe results
    // The callback function is called, when the probeing for the host domain
    // succeededs or fails.
    // In case of failure, the failed domain name should be changed.
    bool setHostProbeResultCallback(const hMDNSHost p_hMDNSHost,
                                    MDNSHostProbeResultCallbackFn p_fnCallback);

    // Returns 'true' is host domain probing is done
    bool status(const hMDNSHost p_hMDNSHost) const;

    // Add a 'global' default' instance name for new services
    bool setInstanceName(const hMDNSHost p_hMDNSHost,
                         const char* p_pcInstanceName);
    const char* instanceName(const hMDNSHost p_hMDNSHost) const;

    /**
        MDNSServiceProbeResultCallbackFn
        Callback function for service domain probe results
    */
    using MDNSServiceProbeResultCallbackFn = std::function<void(MDNSResponder& p_rMDNSResponder,
                                                                const hMDNSHost p_hMDNSHost,
                                                                const hMDNSService p_hMDNSService,
                                                                const char* p_pcServiceName,
                                                                bool p_bProbeResult)>;
    // Add a new service to the MDNS responder. If no name (instance name) is given (p_pcName = 0)
    // the current hostname is used. If the hostname is changed later, the instance names for
    // these 'auto-named' services are changed to the new name also (and probing is restarted).
    // The usual '_' before p_pcService (eg. http) and protocol (eg. tcp) may be given.#
    // If no callback is given, the (maybe) already installed callback stays set
    hMDNSService addService(const hMDNSHost p_hMDNSHost,
                            const char* p_pcName,
                            const char* p_pcServiceType,
                            const char* p_pcProtocol,
                            uint16_t p_u16Port,
                            MDNSServiceProbeResultCallbackFn p_fnCallback = 0);
    // Removes a service from the MDNS responder
    bool removeService(const hMDNSHost p_hMDNSHost,
                       const hMDNSService p_hMDNSService);
    bool removeService(const hMDNSHost p_hMDNSHost,
                       const char* p_pcInstanceName,
                       const char* p_pcServiceType,
                       const char* p_pcProtocol);
    hMDNSService findService(const hMDNSHost p_hMDNSHost,
                             const char* p_pcName,
                             const char* p_pcService,
                             const char* p_pcProtocol);

    // Change the services instance name (and restart probing).
    // If no callback is given, the (maybe) already installed callback stays set
    bool setServiceName(const hMDNSHost p_hMDNSHost,
                        const hMDNSService p_hMDNSService,
                        const char* p_pcInstanceName,
                        MDNSServiceProbeResultCallbackFn p_fnCallback = 0);
    const char* serviceName(const hMDNSHost p_hMDNSHost,
                            const hMDNSService p_hMDNSService) const;
    const char* serviceType(const hMDNSHost p_hMDNSHost,
                            const hMDNSService p_hMDNSService) const;
    const char* serviceProtocol(const hMDNSHost p_hMDNSHost,
                                const hMDNSService p_hMDNSService) const;
    uint16_t servicePort(const hMDNSHost p_hMDNSHost,
                         const hMDNSService p_hMDNSService) const;

    // Set a service specific probe result callcack
    bool setServiceProbeResultCallback(const hMDNSHost p_hMDNSHost,
                                       const hMDNSService p_hMDNSService,
                                       MDNSServiceProbeResultCallbackFn p_fnCallback);

    bool serviceStatus(const hMDNSHost p_hMDNSHost,
                       const hMDNSService p_hMDNSService) const;

    // Add a (static) MDNS TXT item ('key' = 'value') to the service
    hMDNSTxt addServiceTxt(const hMDNSHost p_hMDNSHost,
                           const hMDNSService p_hMDNSService,
                           const char* p_pcKey,
                           const char* p_pcValue);
    hMDNSTxt addServiceTxt(const hMDNSHost p_hMDNSHost,
                           const hMDNSService p_hMDNSService,
                           const char* p_pcKey,
                           uint32_t p_u32Value);
    hMDNSTxt addServiceTxt(const hMDNSHost p_hMDNSHost,
                           const hMDNSService p_hMDNSService,
                           const char* p_pcKey,
                           uint16_t p_u16Value);
    hMDNSTxt addServiceTxt(const hMDNSHost p_hMDNSHost,
                           const hMDNSService p_hMDNSService,
                           const char* p_pcKey,
                           uint8_t p_u8Value);
    hMDNSTxt addServiceTxt(const hMDNSHost p_hMDNSHost,
                           const hMDNSService p_hMDNSService,
                           const char* p_pcKey,
                           int32_t p_i32Value);
    hMDNSTxt addServiceTxt(const hMDNSHost p_hMDNSHost,
                           const hMDNSService p_hMDNSService,
                           const char* p_pcKey,
                           int16_t p_i16Value);
    hMDNSTxt addServiceTxt(const hMDNSHost p_hMDNSHost,
                           const hMDNSService p_hMDNSService,
                           const char* p_pcKey,
                           int8_t p_i8Value);

    // Remove an existing (static) MDNS TXT item from the service
    bool removeServiceTxt(const hMDNSHost p_hMDNSHost,
                          const hMDNSService p_hMDNSService,
                          const hMDNSTxt p_hTxt);
    bool removeServiceTxt(const hMDNSHost p_hMDNSHost,
                          const hMDNSService p_hMDNSService,
                          const char* p_pcKey);

    /**
         MDNSDynamicServiceTxtCallbackFn
         Callback function for dynamic MDNS TXT items
    */
    using MDNSDynamicServiceTxtCallbackFn = std::function<void(MDNSResponder& p_rMDNSResponder,
                                                               const hMDNSHost p_hMDNSHost,
                                                               const hMDNSService p_hMDNSService)>;
    bool setDynamicServiceTxtCallback(const hMDNSHost p_hMDNSHost,
                                      MDNSDynamicServiceTxtCallbackFn p_fnCallback);
    bool setDynamicServiceTxtCallback(const hMDNSHost p_hMDNSHost,
                                      const hMDNSService p_hMDNSService,
                                      MDNSDynamicServiceTxtCallbackFn p_fnCallback);

    // Add a (dynamic) MDNS TXT item ('key' = 'value') to the service
    // Dynamic TXT items are removed right after one-time use. So they need to be added
    // every time the value s needed (via callback).
    hMDNSTxt addDynamicServiceTxt(const hMDNSHost p_hMDNSHost,
                                  const hMDNSService p_hMDNSService,
                                  const char* p_pcKey,
                                  const char* p_pcValue);
    hMDNSTxt addDynamicServiceTxt(const hMDNSHost p_hMDNSHost,
                                  const hMDNSService p_hMDNSService,
                                  const char* p_pcKey,
                                  uint32_t p_u32Value);
    hMDNSTxt addDynamicServiceTxt(const hMDNSHost p_hMDNSHost,
                                  const hMDNSService p_hMDNSService,
                                  const char* p_pcKey,
                                  uint16_t p_u16Value);
    hMDNSTxt addDynamicServiceTxt(const hMDNSHost p_hMDNSHost,
                                  const hMDNSService p_hMDNSService,
                                  const char* p_pcKey,
                                  uint8_t p_u8Value);
    hMDNSTxt addDynamicServiceTxt(const hMDNSHost p_hMDNSHost,
                                  const hMDNSService p_hMDNSService,
                                  const char* p_pcKey,
                                  int32_t p_i32Value);
    hMDNSTxt addDynamicServiceTxt(const hMDNSHost p_hMDNSHost,
                                  const hMDNSService p_hMDNSService,
                                  const char* p_pcKey,
                                  int16_t p_i16Value);
    hMDNSTxt addDynamicServiceTxt(const hMDNSHost p_hMDNSHost,
                                  const hMDNSService p_hMDNSService,
                                  const char* p_pcKey,
                                  int8_t p_i8Value);

    // QUERIES & ANSWERS
    /**
        clsMDNSAnswerAccessor & clsAnswerAccessorVector
    */
    struct clsMDNSAnswerAccessor
    {
    protected:
        /**
            stcCompareTxtKey
        */
        struct stcCompareTxtKey
        {
            bool operator()(char const* p_pA, char const* p_pB) const;
        };
    public:
        clsMDNSAnswerAccessor(const clsHost::stcQuery::stcAnswer* p_pAnswer);
        ~clsMDNSAnswerAccessor(void);

        /**
            clsTxtKeyValueMap
        */
        using clsTxtKeyValueMap = std::map<const char*, const char*, stcCompareTxtKey>;

        bool serviceDomainAvailable(void) const;
        const char* serviceDomain(void) const;
        bool hostDomainAvailable(void) const;
        const char* hostDomain(void) const;
        bool hostPortAvailable(void) const;
        uint16_t hostPort(void) const;
#ifdef MDNS_IPV4_SUPPORT
        bool IPv4AddressAvailable(void) const;
        std::vector<IPAddress> IPv4Addresses(void) const;
#endif
#ifdef MDNS_IPV6_SUPPORT
        bool IPv6AddressAvailable(void) const;
        std::vector<IPAddress> IPv6Addresses(void) const;
#endif
        bool txtsAvailable(void) const;
        const char* txts(void) const;
        const clsTxtKeyValueMap& txtKeyValues(void) const;
        const char* txtValue(const char* p_pcKey) const;

        size_t printTo(Print& p_Print) const;

    protected:
        const clsHost::stcQuery::stcAnswer*   m_pAnswer;
        clsTxtKeyValueMap                                   m_TxtKeyValueMap;
    };
    using clsMDNSAnswerAccessorVector   = std::vector<clsMDNSAnswerAccessor>;
    using typeQueryAnswerType           = clsHost::typeQueryAnswerType;
    using enuQueryAnswerType            = clsHost::enuQueryAnswerType;

    // STATIC QUERY
    // Perform a (static) service/host query. The function returns after p_u16Timeout milliseconds
    // The answers (the number of received answers is returned) can be retrieved by calling
    // - answerHostName (or hostname)
    // - answerIP (or IP)
    // - answerPort (or port)
    uint32_t queryService(const hMDNSHost p_hMDNSHost,
                          const char* p_pcService,
                          const char* p_pcProtocol,
                          const uint16_t p_u16Timeout = MDNS_QUERYSERVICES_WAIT_TIME);
    uint32_t queryHost(const hMDNSHost p_hMDNSHost,
                       const char* p_pcHostName,
                       const uint16_t p_u16Timeout = MDNS_QUERYSERVICES_WAIT_TIME);
    bool removeQuery(const hMDNSHost p_hMDNSHost);
    bool hasQuery(const hMDNSHost p_hMDNSHost);
    hMDNSQuery getQuery(const hMDNSHost p_hMDNSHost);

    clsMDNSAnswerAccessorVector answerAccessors(const hMDNSHost p_hMDNSHost);
    uint32_t answerCount(const hMDNSHost p_hMDNSHost);
    clsMDNSAnswerAccessor answerAccessor(const hMDNSHost p_hMDNSHost,
                                         uint32_t p_u32AnswerIndex);

    // DYNAMIC QUERIES
    /**
        MDNSQueryCallbackFn

        Callback function for received answers for dynamic queries
    */
    using MDNSQueryCallbackFn = std::function<void(MDNSResponder& p_pMDNSResponder,
                                                   hMDNSHost p_hMDNSHost,
                                                   const hMDNSQuery p_hMDNSQuery,
                                                   const clsMDNSAnswerAccessor& p_MDNSAnswerAccessor,
                                                   typeQueryAnswerType p_QueryAnswerTypeFlags,          // flags for the updated answer item
                                                   bool p_bSetContent)>;                                // true: Answer component set, false: component deleted

    // Install a dynamic service/host query. For every received answer (part) the given callback
    // function is called. The query will be updated every time, the TTL for an answer
    // has timed-out.
    // The answers can also be retrieved by calling
    // - answerCount                                service/host (for host queries, this should never be >1)
    // - answerServiceDomain                        service
    // - hasAnswerHostDomain/answerHostDomain       service/host
    // - hasAnswerIPv4Address/answerIPv4Address     service/host
    // - hasAnswerIPv6Address/answerIPv6Address     service/host
    // - hasAnswerPort/answerPort                   service
    // - hasAnswerTxts/answerTxts                   service
    hMDNSQuery installServiceQuery(const hMDNSHost p_hMDNSHost,
                                   const char* p_pcServiceType,
                                   const char* p_pcProtocol,
                                   MDNSQueryCallbackFn p_fnCallback);
    hMDNSQuery installHostQuery(const hMDNSHost p_hMDNSHost,
                                const char* p_pcHostName,
                                MDNSQueryCallbackFn p_fnCallback);
    // Remove a dynamic service query
    bool removeQuery(const hMDNSHost p_hMDNSHost,
                     const hMDNSQuery p_hMDNSQuery);


    uint32_t answerCount(const hMDNSHost p_hMDNSHost,
                         const hMDNSQuery p_hMDNSQuery);
    clsMDNSAnswerAccessorVector answerAccessors(const hMDNSHost p_hMDNSHost,
                                                const hMDNSQuery p_hMDNSQuery);
    clsMDNSAnswerAccessor answerAccessor(const hMDNSHost p_hMDNSHost,
                                         const hMDNSQuery p_hMDNSQuery,
                                         uint32 p_u32AnswerIndex);

    /*  bool hasAnswerServiceDomain(const hMDNSHost p_hMDNSHost,
                                const hMDNSQuery p_hQuery,
                                const uint32_t p_u32AnswerIndex);
        const char* answerServiceDomain(const hMDNSHost p_hMDNSHost,
                                    const hMDNSQuery p_hQuery,
                                    const uint32_t p_u32AnswerIndex);
        bool hasAnswerHostDomain(const hMDNSHost p_hMDNSHost,
                             const hMDNSQuery p_hQuery,
                             const uint32_t p_u32AnswerIndex);
        const char* answerHostDomain(const hMDNSHost p_hMDNSHost,
                                 const hMDNSQuery p_hQuery,
                                 const uint32_t p_u32AnswerIndex);
        #ifdef MDNS_IPV4_SUPPORT
        bool hasAnswerIPv4Address(const hMDNSHost p_hMDNSHost,
                              const hMDNSQuery p_hQuery,
                              const uint32_t p_u32AnswerIndex);
        uint32_t answerIPv4AddressCount(const hMDNSHost p_hMDNSHost,
                                    const hMDNSQuery p_hQuery,
                                    const uint32_t p_u32AnswerIndex);
        IPAddress answerIPv4Address(const hMDNSHost p_hMDNSHost,
                                const hMDNSQuery p_hQuery,
                                const uint32_t p_u32AnswerIndex,
                                const uint32_t p_u32AddressIndex);
        #endif
        #ifdef MDNS_IPV6_SUPPORT
        bool hasAnswerIPv6Address(const hMDNSHost p_hMDNSHost,
                              const hMDNSQuery p_hQuery,
                              const uint32_t p_u32AnswerIndex);
        uint32_t answerIPv6AddressCount(const hMDNSHost p_hMDNSHost,
                                    const hMDNSQuery p_hQuery,
                                    const uint32_t p_u32AnswerIndex);
        IPAddress answerIPv6Address(const hMDNSHost p_hMDNSHost,
                                const hMDNSQuery p_hQuery,
                                const uint32_t p_u32AnswerIndex,
                                const uint32_t p_u32AddressIndex);
        #endif
        bool hasAnswerPort(const hMDNSHost p_hMDNSHost,
                       const hMDNSQuery p_hQuery,
                       const uint32_t p_u32AnswerIndex);
        uint16_t answerPort(const hMDNSHost p_hMDNSHost,
                        const hMDNSQuery p_hQuery,
                        const uint32_t p_u32AnswerIndex);
        bool hasAnswerTxts(const hMDNSHost p_hMDNSHost,
                       const hMDNSQuery p_hQuery,
                       const uint32_t p_u32AnswerIndex);
        // Get the TXT items as a ';'-separated string
        const char* answerTxts(const hMDNSHost p_hMDNSHost,
                           const hMDNSQuery p_hQuery,
                           const uint32_t p_u32AnswerIndex);*/

    // GENERAL MANAGEMENT
    // Application should call this whenever AP is configured/disabled
    bool notifyNetIfChange(netif* p_pNetIf);

    // 'update' should be called in every 'loop' to run the MDNS processing
    bool update(const hMDNSHost p_hMDNSHost);
    bool update(void);    // Convenience

    // 'announce' can be called every time, the configuration of some service
    // changes. Mainly, this would be changed content of TXT items.
    bool announce(const hMDNSHost p_hMDNSHost);
    bool announce(void);    // Convenience

    // MISC
    // Enable OTA update
    hMDNSService enableArduino(const hMDNSHost p_hMDNSHost,
                               uint16_t p_u16Port,
                               bool p_bAuthUpload = false);

protected:
    /** Internal CLASSES & STRUCTS **/

    // InstanceData
    UdpContext*		m_pUDPContext;
    clsHostList		m_HostList;

    // UDP CONTEXT
    bool _allocUDPContext(void);
    bool _releaseUDPContext(void);
    bool _processUDPInput(void);

    // NETIF
    clsHost* _createHost(netif* p_pNetIf);
    bool _releaseHost(clsHost* p_pHost);

    const clsHost* _findHost(netif* p_pNetIf) const;
    clsHost* _findHost(netif* p_pNetIf);
    const clsHost* _findHost(const hMDNSHost p_hMDNSHost) const;
    clsHost* _findHost(const hMDNSHost p_hMDNSHost);


    // HANDLE HELPERS
    bool _validateMDNSHostHandle(const hMDNSHost p_hMDNSHost) const;
    bool _validateMDNSHostHandle(const hMDNSHost p_hMDNSHost,
                                 const hMDNSService p_hMDNSService) const;

    clsHost* _NRH2Ptr(const hMDNSHost p_hMDNSHost);
    const clsHost* _NRH2Ptr(const hMDNSHost p_hMDNSHost) const;
    clsHost::stcService* _SH2Ptr(const hMDNSService p_hMDNSService);
    const clsHost::stcService* _SH2Ptr(const hMDNSService p_hMDNSService) const;

    // INIT
    clsHost* _begin(const char* p_pcHostName,
                    netif* p_pNetIf,
                    MDNSHostProbeResultCallbackFn p_fnCallback);
    bool _close(clsHost& p_rHost);


#if not defined ESP_8266_MDNS_INCLUDE || defined DEBUG_ESP_MDNS_RESPONDER

    const char* _DH(hMDNSHost p_hMDNSResponder = 0) const;

#endif  // not defined ESP_8266_MDNS_INCLUDE || defined DEBUG_ESP_MDNS_RESPONDER

};

#ifdef __MDNS_USE_LEGACY
/**
    MDNSResponder_Legacy
*/
class MDNSResponder_Legacy //: public MDNSResponder
{
public:
    /* INTERFACE */
    MDNSResponder_Legacy(void);
    virtual ~MDNSResponder_Legacy(void);

    /**
        hMDNSHost (opaque handle to access a netif binding)
    */
    using hMDNSHost = const void*;
    /**
        hMDNSService (opaque handle to access the service)
    */
    using hMDNSService = const void*;
    /**
        MDNSHostProbeResultCallbackFn
        Callback function for host domain probe results
    */
    using MDNSHostProbeResultCallbackFn = std::function<void(MDNSResponder* p_pMDNSResponder,
                                                             const hMDNSHost p_hMDNSHost,
                                                             const char* p_pcDomainName,
                                                             bool p_bProbeResult)>;
    /* LEGACY 2 */
    using MDNSServiceProbeResultCallbackFn1 = std::function<void(MDNSResponder* p_pMDNSResponder,
                                                                 const hMDNSService p_hMDNSService,
                                                                 const char* p_pcServiceName,
                                                                 bool p_bProbeResult)>;
    using MDNSServiceProbeResultCallbackFn2 = std::function<void(const hMDNSService p_hMDNSService,
                                                                 const char* p_pcServiceName,
                                                                 bool p_bProbeResult)>;
    /**
        hMDNSTxt (opaque handle to access the TXT items)
    */
    using hMDNSTxt = const void*;
    /**
        MDNSDynamicServiceTxtCallbackFn
        Callback function for dynamic MDNS TXT items
    */
    using MDNSDynamicServiceTxtCallbackFn = std::function<void(MDNSResponder* p_pMDNSResponder,
                                                               const hMDNSHost p_hMDNSHost,
                                                               const hMDNSService p_hMDNSService)>;
    // LEGACY
    using MDNSDynamicServiceTxtCallbackFn1 = std::function<void(MDNSResponder* p_pMDNSResponder,
                                                                const hMDNSService p_hMDNSService)>;
    using MDNSDynamicServiceTxtCallbackFn2 = std::function<void(const hMDNSService p_hMDNSService)>;


    hMDNSHost getNetIfBinding(netif* p_pNetIf) const;
    hMDNSHost getNetIfBinding(WiFiMode_t p_WiFiMode) const;

    // Create a MDNS responder netif binding by setting the default hostname
    // Later call 'update()' in every 'loop' to run the process loop
    // (probing, announcing, responding, ...)

    hMDNSHost begin(const char* p_pcHostName,
                    netif* p_pNetIf,
                    MDNSHostProbeResultCallbackFn p_fnCallback = 0);
    bool begin(const char* p_pcHostName,
               WiFiMode_t p_WiFiMode,
               MDNSHostProbeResultCallbackFn p_fnCallback = 0);
    bool begin(const char* p_pcHostName,
               MDNSHostProbeResultCallbackFn p_fnCallback = 0);

    /*  bool begin(const String& p_strHostName) {return begin(p_strHostName.c_str());}
        // for compatibility
        bool begin(const char* p_pcHostName,
                   IPAddress p_IPAddress,       // ignored
                   uint32_t p_u32TTL = 120);    // ignored
        bool begin(const String& p_strHostName,
                   IPAddress p_IPAddress,       // ignored
                   uint32_t p_u32TTL = 120) {   // ignored
            return begin(p_strHostName.c_str(), p_IPAddress, p_u32TTL);
        }*/
    // Finish MDNS processing
    bool close(const hMDNSHost p_hMDNSHost);
    bool close(void);
    // for ESP32 compatibility
    bool end(void);

    // Change hostname (probing is restarted)
    bool setHostName(const hMDNSHost p_hMDNSHost,
                     const char* p_pcHostName);
    // for compatibility...
    bool setHostname(const char* p_pcHostName);
    bool setHostname(String p_strHostName);

    const char* hostName(const hMDNSHost p_hMDNSHost) const;
    const char* hostname(void) const;

    // Returns 'true' is host domain probing is done
    bool status(const hMDNSHost p_hMDNSHost) const;
    bool status(void) const;

    bool setInstanceName(const hMDNSHost p_hMDNSHost,
                         const char* p_pcInstanceName);
    bool setInstanceName(const char* p_pcInstanceName);
    // for ESP32 compatibility
    bool setInstanceName(const String& p_strHostName);

    // Add a new service to the MDNS responder. If no name (instance name) is given (p_pcName = 0)
    // the current hostname is used. If the hostname is changed later, the instance names for
    // these 'auto-named' services are changed to the new name also (and probing is restarted).
    // The usual '_' before p_pcService (eg. http) and protocol (eg. tcp) may be given.
    hMDNSService addService(const hMDNSHost p_hMDNSHost,
                            const char* p_pcName,
                            const char* p_pcService,
                            const char* p_pcProtocol,
                            uint16_t p_u16Port);
    hMDNSService addService(const char* p_pcName,
                            const char* p_pcService,
                            const char* p_pcProtocol,
                            uint16_t p_u16Port);
    // Removes a service from the MDNS responder
    bool removeService(const hMDNSHost p_hMDNSHost,
                       const hMDNSService p_hMDNSService);
    bool removeService(const hMDNSHost p_hMDNSHost,
                       const char* p_pcInstanceName,
                       const char* p_pcServiceName,
                       const char* p_pcProtocol);
    bool removeService(const hMDNSService p_hMDNSService);
    bool removeService(const char* p_pcInstanceName,
                       const char* p_pcServiceName,
                       const char* p_pcProtocol);
    // for compatibility...
    bool addService(String p_strServiceName,
                    String p_strProtocol,
                    uint16_t p_u16Port);
    hMDNSService findService(const hMDNSHost p_hMDNSHost,
                             const char* p_pcName,
                             const char* p_pcService,
                             const char* p_pcProtocol);
    hMDNSService findService(const char* p_pcName,
                             const char* p_pcService,
                             const char* p_pcProtocol);

    // Change the services instance name (and restart probing).
    bool setServiceName(const hMDNSHost p_hMDNSHost,
                        const hMDNSService p_hMDNSService,
                        const char* p_pcInstanceName);
    bool setServiceName(const hMDNSService p_hMDNSService,
                        const char* p_pcInstanceName);

    const char* serviceName(const hMDNSHost p_hMDNSHost,
                            const hMDNSService p_hMDNSService) const;
    const char* service(const hMDNSHost p_hMDNSHost,
                        const hMDNSService p_hMDNSService) const;
    const char* serviceProtocol(const hMDNSHost p_hMDNSHost,
                                const hMDNSService p_hMDNSService) const;
    /* LEGACY */
    const char* serviceName(const hMDNSService p_hMDNSService) const;
    const char* service(const hMDNSService p_hMDNSService) const;
    const char* serviceProtocol(const hMDNSService p_hMDNSService) const;

    bool serviceStatus(const hMDNSHost p_hMDNSHost,
                       const hMDNSService p_hMDNSService) const;
    bool serviceStatus(const hMDNSService p_hMDNSService) const;

    // Add a (static) MDNS TXT item ('key' = 'value') to the service
    hMDNSTxt addServiceTxt(const hMDNSHost p_hMDNSHost,
                           const hMDNSService p_hMDNSService,
                           const char* p_pcKey,
                           const char* p_pcValue);
    hMDNSTxt addServiceTxt(const hMDNSHost p_hMDNSHost,
                           const hMDNSService p_hMDNSService,
                           const char* p_pcKey,
                           uint32_t p_u32Value);
    hMDNSTxt addServiceTxt(const hMDNSHost p_hMDNSHost,
                           const hMDNSService p_hMDNSService,
                           const char* p_pcKey,
                           uint16_t p_u16Value);
    hMDNSTxt addServiceTxt(const hMDNSHost p_hMDNSHost,
                           const hMDNSService p_hMDNSService,
                           const char* p_pcKey,
                           uint8_t p_u8Value);
    hMDNSTxt addServiceTxt(const hMDNSHost p_hMDNSHost,
                           const hMDNSService p_hMDNSService,
                           const char* p_pcKey,
                           int32_t p_i32Value);
    hMDNSTxt addServiceTxt(const hMDNSHost p_hMDNSHost,
                           const hMDNSService p_hMDNSService,
                           const char* p_pcKey,
                           int16_t p_i16Value);
    hMDNSTxt addServiceTxt(const hMDNSHost p_hMDNSHost,
                           const hMDNSService p_hMDNSService,
                           const char* p_pcKey,
                           int8_t p_i8Value);
    // LEGACY
    hMDNSTxt addServiceTxt(const hMDNSService p_hMDNSService,
                           const char* p_pcKey,
                           const char* p_pcValue);
    hMDNSTxt addServiceTxt(const hMDNSService p_hMDNSService,
                           const char* p_pcKey,
                           uint32_t p_u32Value);
    hMDNSTxt addServiceTxt(const hMDNSService p_hMDNSService,
                           const char* p_pcKey,
                           uint16_t p_u16Value);
    hMDNSTxt addServiceTxt(const hMDNSService p_hMDNSService,
                           const char* p_pcKey,
                           uint8_t p_u8Value);
    hMDNSTxt addServiceTxt(const hMDNSService p_hMDNSService,
                           const char* p_pcKey,
                           int32_t p_i32Value);
    hMDNSTxt addServiceTxt(const hMDNSService p_hMDNSService,
                           const char* p_pcKey,
                           int16_t p_i16Value);
    hMDNSTxt addServiceTxt(const hMDNSService p_hMDNSService,
                           const char* p_pcKey,
                           int8_t p_i8Value);

    // Remove an existing (static) MDNS TXT item from the service
    bool removeServiceTxt(const hMDNSHost p_hMDNSHost,
                          const hMDNSService p_hMDNSService,
                          const hMDNSTxt p_hTxt);
    bool removeServiceTxt(const hMDNSHost p_hMDNSHost,
                          const hMDNSService p_hMDNSService,
                          const char* p_pcKey);
    bool removeServiceTxt(const hMDNSHost p_hMDNSHost,
                          const char* p_pcinstanceName,
                          const char* p_pcServiceName,
                          const char* p_pcProtocol,
                          const char* p_pcKey);
    bool removeServiceTxt(const hMDNSService p_hMDNSService,
                          const hMDNSTxt p_hTxt);
    bool removeServiceTxt(const hMDNSService p_hMDNSService,
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

    bool setDynamicServiceTxtCallback(const hMDNSHost p_hMDNSHost,
                                      MDNSDynamicServiceTxtCallbackFn p_fnCallback);
    bool setDynamicServiceTxtCallback(const hMDNSHost p_hMDNSHost,
                                      const hMDNSService p_hMDNSService,
                                      MDNSDynamicServiceTxtCallbackFn p_fnCallback);

    // Set a global callback for dynamic MDNS TXT items. The callback function is called
    // every time, a TXT item is needed for one of the installed services.
    bool setDynamicServiceTxtCallback(MDNSDynamicServiceTxtCallbackFn1 p_fnCallback);
    bool setDynamicServiceTxtCallback(MDNSDynamicServiceTxtCallbackFn2 p_fnCallback);

    // Set a service specific callback for dynamic MDNS TXT items. The callback function
    // is called every time, a TXT item is needed for the given service.
    bool setDynamicServiceTxtCallback(const hMDNSService p_hMDNSService,
                                      MDNSDynamicServiceTxtCallbackFn1 p_fnCallback);
    bool setDynamicServiceTxtCallback(const hMDNSService p_hMDNSService,
                                      MDNSDynamicServiceTxtCallbackFn2 p_fnCallback);

    // Add a (dynamic) MDNS TXT item ('key' = 'value') to the service
    // Dynamic TXT items are removed right after one-time use. So they need to be added
    // every time the value s needed (via callback).
    hMDNSTxt addDynamicServiceTxt(const hMDNSHost p_hMDNSHost,
                                  const hMDNSService p_hMDNSService,
                                  const char* p_pcKey,
                                  const char* p_pcValue);
    hMDNSTxt addDynamicServiceTxt(const hMDNSHost p_hMDNSHost,
                                  const hMDNSService p_hMDNSService,
                                  const char* p_pcKey,
                                  uint32_t p_u32Value);
    hMDNSTxt addDynamicServiceTxt(const hMDNSHost p_hMDNSHost,
                                  const hMDNSService p_hMDNSService,
                                  const char* p_pcKey,
                                  uint16_t p_u16Value);
    hMDNSTxt addDynamicServiceTxt(const hMDNSHost p_hMDNSHost,
                                  const hMDNSService p_hMDNSService,
                                  const char* p_pcKey,
                                  uint8_t p_u8Value);
    hMDNSTxt addDynamicServiceTxt(const hMDNSHost p_hMDNSHost,
                                  const hMDNSService p_hMDNSService,
                                  const char* p_pcKey,
                                  int32_t p_i32Value);
    hMDNSTxt addDynamicServiceTxt(const hMDNSHost p_hMDNSHost,
                                  const hMDNSService p_hMDNSService,
                                  const char* p_pcKey,
                                  int16_t p_i16Value);
    hMDNSTxt addDynamicServiceTxt(const hMDNSHost p_hMDNSHost,
                                  const hMDNSService p_hMDNSService,
                                  const char* p_pcKey,
                                  int8_t p_i8Value);
    /* LEGACY */
    hMDNSTxt addDynamicServiceTxt(const hMDNSService p_hMDNSService,
                                  const char* p_pcKey,
                                  const char* p_pcValue);
    hMDNSTxt addDynamicServiceTxt(const hMDNSService p_hMDNSService,
                                  const char* p_pcKey,
                                  uint32_t p_u32Value);
    hMDNSTxt addDynamicServiceTxt(const hMDNSService p_hMDNSService,
                                  const char* p_pcKey,
                                  uint16_t p_u16Value);
    hMDNSTxt addDynamicServiceTxt(const hMDNSService p_hMDNSService,
                                  const char* p_pcKey,
                                  uint8_t p_u8Value);
    hMDNSTxt addDynamicServiceTxt(const hMDNSService p_hMDNSService,
                                  const char* p_pcKey,
                                  int32_t p_i32Value);
    hMDNSTxt addDynamicServiceTxt(const hMDNSService p_hMDNSService,
                                  const char* p_pcKey,
                                  int16_t p_i16Value);
    hMDNSTxt addDynamicServiceTxt(const hMDNSService p_hMDNSService,
                                  const char* p_pcKey,
                                  int8_t p_i8Value);

    /**
        hMDNSQuery (opaque handle to access dynamic service queries)
    */
    using hMDNSQuery = const void*;
    //using hMDNSServiceQuery = hMDNSQuery;   // for compatibility with V1

    // Perform a (static) service/host query. The function returns after p_u16Timeout milliseconds
    // The answers (the number of received answers is returned) can be retrieved by calling
    // - answerHostName (or hostname)
    // - answerIP (or IP)
    // - answerPort (or port)
    uint32_t queryService(const hMDNSHost p_hMDNSHost,
                          const char* p_pcService,
                          const char* p_pcProtocol,
                          const uint16_t p_u16Timeout = MDNS_QUERYSERVICES_WAIT_TIME);
    uint32_t queryService(const char* p_pcService,
                          const char* p_pcProtocol,
                          const uint16_t p_u16Timeout = MDNS_QUERYSERVICES_WAIT_TIME);
    // for compatibility...
    uint32_t queryService(const String& p_strService,
                          const String& p_strProtocol);
    uint32_t queryHost(const hMDNSHost p_hMDNSHost,
                       const char* p_pcHostName,
                       const uint16_t p_u16Timeout = MDNS_QUERYSERVICES_WAIT_TIME);
    uint32_t queryHost(const char* p_pcHostName,
                       const uint16_t p_u16Timeout = MDNS_QUERYSERVICES_WAIT_TIME);
    bool removeQuery(const hMDNSHost p_hMDNSHost);
    bool removeQuery(void);
    bool hasQuery(const hMDNSHost p_hMDNSHost);
    bool hasQuery(void);
    hMDNSQuery getQuery(const hMDNSHost p_hMDNSHost);
    hMDNSQuery getQuery(void);

    const char* answerHostName(const hMDNSHost p_hMDNSHost,
                               const uint32_t p_u32AnswerIndex);
    const char* answerHostName(const uint32_t p_u32AnswerIndex);
    // for compatibility...
    String hostname(const uint32_t p_u32AnswerIndex);
#ifdef MDNS_IPV4_SUPPORT
    IPAddress answerIPv4(const hMDNSHost p_hMDNSHost,
                         const uint32_t p_u32AnswerIndex);
    IPAddress answerIPv4(const uint32_t p_u32AnswerIndex);
    // for compatibility
    IPAddress answerIP(const uint32_t p_u32AnswerIndex);
    IPAddress IP(const uint32_t p_u32AnswerIndex);
#endif
#ifdef MDNS_IPV6_SUPPORT
    IPAddress answerIPv6(const hMDNSHost p_hMDNSHost,
                         const uint32_t p_u32AnswerIndex);
    IPAddress answerIPv6(const uint32_t p_u32AnswerIndex);
#endif
    uint16_t answerPort(const hMDNSHost p_hMDNSHost,
                        const uint32_t p_u32AnswerIndex);
    uint16_t answerPort(const uint32_t p_u32AnswerIndex);
    // for compatibility
    uint16_t port(const uint32_t p_u32AnswerIndex);

    /**
        typeQueryAnswerType & enuQueryAnswerType
    */
    using typeQueryAnswerType = uint8_t;
    enum class enuQueryAnswerType : typeQueryAnswerType
    {
        Unknown             = 0x00,
        ServiceDomain       = 0x01,     // Service domain
        HostDomain          = 0x02,     // Host domain
        Port                = 0x04,     // Port
        Txts                = 0x08,     // TXT items
#ifdef MDNS_IPV4_SUPPORT
        IPv4Address         = 0x10,     // IPv4 address
#endif
#ifdef MDNS_IPV6_SUPPORT
        IPv6Address         = 0x20,     // IPv6 address
#endif
    };
    //using AnswerType = enuQueryAnswerType;   // for compatibility with V1

    /**
        stcAnswerAccessor
    */
    struct stcAnswerAccessor
    {
    protected:
        /**
            stcCompareTxtKey
        */
        struct stcCompareTxtKey
        {
            bool operator()(char const* p_pA, char const* p_pB) const;
        };
    public:
        stcAnswerAccessor(MDNSResponder& p_rMDNSResponder,
                          hMDNSQuery p_hQuery,
                          uint32_t p_u32AnswerIndex);
        /**
            clsTxtKeyValueMap
        */
        using clsTxtKeyValueMap = std::map<const char*, const char*, stcCompareTxtKey>;

        bool serviceDomainAvailable(void) const;
        const char* serviceDomain(void) const;
        bool hostDomainAvailable(void) const;
        const char* hostDomain(void) const;
        bool hostPortAvailable(void) const;
        uint16_t hostPort(void) const;
#ifdef MDNS_IPV4_SUPPORT
        bool IPv4AddressAvailable(void) const;
        std::vector<IPAddress> IPv4Addresses(void) const;
#endif
#ifdef MDNS_IPV6_SUPPORT
        bool IPv6AddressAvailable(void) const;
        std::vector<IPAddress> IPv6Addresses(void) const;
#endif
        bool txtsAvailable(void) const;
        const char* txts(void) const;
        const clsTxtKeyValueMap& txtKeyValues(void) const;
        const char* txtValue(const char* p_pcKey) const;

        size_t printTo(Print& p_Print) const;

    protected:
        MDNSResponder&      m_rMDNSResponder;
        hMDNSQuery          m_hQuery;
        uint32_t            m_u32AnswerIndex;
        clsTxtKeyValueMap   m_TxtKeyValueMap;
    };

    /**
        MDNSQueryCallbackFn

        Callback function for received answers for dynamic queries
    */
    using MDNSQueryCallbackFn = std::function<void(MDNSResponder* p_pMDNSResponder,
                                                   hMDNSHost p_hMDNSHost,
                                                   const stcAnswerAccessor& p_MDNSAnswerAccessor,
                                                   typeQueryAnswerType p_QueryAnswerTypeFlags,          // flags for the updated answer item
                                                   bool p_bSetContent)>;                                // true: Answer component set, false: component deleted
    // LEGACY
    using MDNSQueryCallbackFn1 = std::function<void(MDNSResponder* p_pMDNSResponder,
                                                    const stcAnswerAccessor& p_MDNSAnswerAccessor,
                                                    typeQueryAnswerType p_QueryAnswerTypeFlags,          // flags for the updated answer item
                                                    bool p_bSetContent)>;                                // true: Answer component set, false: component deleted
    using MDNSQueryCallbackFn2 = std::function<void(const stcAnswerAccessor& p_MDNSAnswerAccessor,
                                                    typeQueryAnswerType p_QueryAnswerTypeFlags,         // flags for the updated answer item
                                                    bool p_bSetContent)>;                               // true: Answer component set, false: component deleted
    //using MDNSServiceInfo = stcAnswerAccessor;  // for compatibility with V1

    // Install a dynamic service/host query. For every received answer (part) the given callback
    // function is called. The query will be updated every time, the TTL for an answer
    // has timed-out.
    // The answers can also be retrieved by calling
    // - answerCount                                service/host (for host queries, this should never be >1)
    // - answerServiceDomain                        service
    // - hasAnswerHostDomain/answerHostDomain       service/host
    // - hasAnswerIPv4Address/answerIPv4Address     service/host
    // - hasAnswerIPv6Address/answerIPv6Address     service/host
    // - hasAnswerPort/answerPort                   service
    // - hasAnswerTxts/answerTxts                   service
    hMDNSQuery installServiceQuery(const hMDNSHost p_hMDNSHost,
                                   const char* p_pcService,
                                   const char* p_pcProtocol,
                                   MDNSQueryCallbackFn p_fnCallback);
    hMDNSQuery installHostQuery(const hMDNSHost p_hMDNSHost,
                                const char* p_pcHostName,
                                MDNSQueryCallbackFn p_fnCallback);

    hMDNSQuery installServiceQuery(const char* p_pcService,
                                   const char* p_pcProtocol,
                                   MDNSQueryCallbackFn1 p_fnCallback);
    hMDNSQuery installServiceQuery(const char* p_pcService,
                                   const char* p_pcProtocol,
                                   MDNSQueryCallbackFn2 p_fnCallback);

    hMDNSQuery installHostQuery(const char* p_pcHostName,
                                MDNSQueryCallbackFn1 p_fnCallback);
    hMDNSQuery installHostQuery(const char* p_pcHostName,
                                MDNSQueryCallbackFn2 p_fnCallback);
    // Remove a dynamic service query
    bool removeDynamicQuery(const hMDNSHost p_hMDNSHost,
                            const hMDNSQuery p_hMDNSQuery);
    bool removeDynamicQuery(const hMDNSQuery p_hQuery);

    /**
        clsMDNSAnswerAccessorVector
    */
    using clsMDNSAnswerAccessorVector = std::vector<stcAnswerAccessor>;

    clsMDNSAnswerAccessorVector answerAccessors(const hMDNSHost p_hMDNSHost,
                                                const hMDNSQuery p_hMDNSQuery);
    clsMDNSAnswerAccessorVector answerAccessors(const hMDNSQuery p_hQuery);

    uint32_t answerCount(const hMDNSHost p_hMDNSHost,
                         const hMDNSQuery p_hMDNSQuery);
    uint32_t answerCount(const hMDNSQuery p_hQuery);

    bool hasAnswerServiceDomain(const hMDNSHost p_hMDNSHost,
                                const hMDNSQuery p_hQuery,
                                const uint32_t p_u32AnswerIndex);
    bool hasAnswerServiceDomain(const hMDNSQuery p_hQuery,
                                const uint32_t p_u32AnswerIndex);
    const char* answerServiceDomain(const hMDNSHost p_hMDNSHost,
                                    const hMDNSQuery p_hQuery,
                                    const uint32_t p_u32AnswerIndex);
    const char* answerServiceDomain(const hMDNSQuery p_hQuery,
                                    const uint32_t p_u32AnswerIndex);
    bool hasAnswerHostDomain(const hMDNSHost p_hMDNSHost,
                             const hMDNSQuery p_hQuery,
                             const uint32_t p_u32AnswerIndex);
    bool hasAnswerHostDomain(const hMDNSQuery p_hQuery,
                             const uint32_t p_u32AnswerIndex);
    const char* answerHostDomain(const hMDNSHost p_hMDNSHost,
                                 const hMDNSQuery p_hQuery,
                                 const uint32_t p_u32AnswerIndex);
    const char* answerHostDomain(const hMDNSQuery p_hQuery,
                                 const uint32_t p_u32AnswerIndex);
#ifdef MDNS_IPV4_SUPPORT
    bool hasAnswerIPv4Address(const hMDNSHost p_hMDNSHost,
                              const hMDNSQuery p_hQuery,
                              const uint32_t p_u32AnswerIndex);
    uint32_t answerIPv4AddressCount(const hMDNSHost p_hMDNSHost,
                                    const hMDNSQuery p_hQuery,
                                    const uint32_t p_u32AnswerIndex);
    IPAddress answerIPv4Address(const hMDNSHost p_hMDNSHost,
                                const hMDNSQuery p_hQuery,
                                const uint32_t p_u32AnswerIndex,
                                const uint32_t p_u32AddressIndex);
    bool hasAnswerIPv4Address(const hMDNSQuery p_hQuery,
                              const uint32_t p_u32AnswerIndex);
    uint32_t answerIPv4AddressCount(const hMDNSQuery p_hQuery,
                                    const uint32_t p_u32AnswerIndex);
    IPAddress answerIPv4Address(const hMDNSQuery p_hQuery,
                                const uint32_t p_u32AnswerIndex,
                                const uint32_t p_u32AddressIndex);
#endif
#ifdef MDNS_IPV6_SUPPORT
    bool hasAnswerIPv6Address(const hMDNSHost p_hMDNSHost,
                              const hMDNSQuery p_hQuery,
                              const uint32_t p_u32AnswerIndex);
    uint32_t answerIPv6AddressCount(const hMDNSHost p_hMDNSHost,
                                    const hMDNSQuery p_hQuery,
                                    const uint32_t p_u32AnswerIndex);
    IPAddress answerIPv6Address(const hMDNSHost p_hMDNSHost,
                                const hMDNSQuery p_hQuery,
                                const uint32_t p_u32AnswerIndex,
                                const uint32_t p_u32AddressIndex);
    bool hasAnswerIPv6Address(const hMDNSQuery p_hQuery,
                              const uint32_t p_u32AnswerIndex);
    uint32_t answerIPv6AddressCount(const hMDNSQuery p_hQuery,
                                    const uint32_t p_u32AnswerIndex);
    IPAddress answerIPv6Address(const hMDNSQuery p_hQuery,
                                const uint32_t p_u32AnswerIndex,
                                const uint32_t p_u32AddressIndex);
#endif
    bool hasAnswerPort(const hMDNSHost p_hMDNSHost,
                       const hMDNSQuery p_hQuery,
                       const uint32_t p_u32AnswerIndex);
    bool hasAnswerPort(const hMDNSQuery p_hQuery,
                       const uint32_t p_u32AnswerIndex);
    uint16_t answerPort(const hMDNSHost p_hMDNSHost,
                        const hMDNSQuery p_hQuery,
                        const uint32_t p_u32AnswerIndex);
    /*  uint16_t answerPort(const hMDNSQuery p_hQuery,
                        const uint32_t p_u32AnswerIndex);*/
    bool hasAnswerTxts(const hMDNSHost p_hMDNSHost,
                       const hMDNSQuery p_hQuery,
                       const uint32_t p_u32AnswerIndex);
    bool hasAnswerTxts(const hMDNSQuery p_hQuery,
                       const uint32_t p_u32AnswerIndex);
    // Get the TXT items as a ';'-separated string
    const char* answerTxts(const hMDNSHost p_hMDNSHost,
                           const hMDNSQuery p_hQuery,
                           const uint32_t p_u32AnswerIndex);
    const char* answerTxts(const hMDNSQuery p_hQuery,
                           const uint32_t p_u32AnswerIndex);

    // Set a callback function for host probe results
    // The callback function is called, when the probeing for the host domain
    // succeededs or fails.
    // In case of failure, the failed domain name should be changed.
    bool setHostProbeResultCallback(MDNSHostProbeResultCallbackFn p_fnCallback);
    /* LEGACY 2 */
    using MDNSHostProbeResultCallbackFn1 = std::function<void(MDNSResponder* p_pMDNSResponder,
                                                              const char* p_pcDomainName,
                                                              bool p_bProbeResult)>;
    using MDNSHostProbeResultCallbackFn2 = std::function<void(const char* p_pcDomainName,
                                                              bool p_bProbeResult)>;

    bool setHostProbeResultCallback(MDNSHostProbeResultCallbackFn1 p_fnCallback);
    bool setHostProbeResultCallback(MDNSHostProbeResultCallbackFn2 p_fnCallback);

    /**
        MDNSServiceProbeResultCallbackFn
        Callback function for service domain probe results
    */
    using MDNSServiceProbeResultCallbackFn = std::function<void(MDNSResponder* p_pMDNSResponder,
                                                                const hMDNSHost p_hMDNSHost,
                                                                const hMDNSService p_hMDNSService,
                                                                const char* p_pcServiceName,
                                                                bool p_bProbeResult)>;
    // Set a service specific probe result callcack
    bool setServiceProbeResultCallback(const hMDNSService p_hMDNSService,
                                       MDNSServiceProbeResultCallbackFn p_fnCallback);

    bool setServiceProbeResultCallback(const hMDNSService p_hMDNSService,
                                       MDNSServiceProbeResultCallbackFn1 p_fnCallback);
    bool setServiceProbeResultCallback(const hMDNSService p_hMDNSService,
                                       MDNSServiceProbeResultCallbackFn2 p_fnCallback);

    // Application should call this whenever AP is configured/disabled
    bool notifyNetIfChange(netif* p_pNetIf);

    // 'update' should be called in every 'loop' to run the MDNS processing
    bool update(const hMDNSHost p_hMDNSHost);
    bool update(void);

    // 'announce' can be called every time, the configuration of some service
    // changes. Mainly, this would be changed content of TXT items.
    bool announce(const hMDNSHost p_hMDNSHost);
    bool announce(void);

    // Enable OTA update
    hMDNSService enableArduino(const hMDNSHost p_hMDNSHost,
                               uint16_t p_u16Port,
                               bool p_bAuthUpload = false);
    hMDNSService enableArduino(uint16_t p_u16Port,
                               bool p_bAuthUpload = false);

    // Domain name helper
    static bool indexDomain(char*& p_rpcDomain,
                            const char* p_pcDivider = "-",
                            const char* p_pcDefaultDomain = 0);
    // Host name helper
    static bool setNetIfHostName(netif* p_pNetIf,
                                 const char* p_pcHostName);
};
#endif

}	// namespace MDNSImplementation

}	// namespace esp8266

#endif // LEAMDNS2_H



