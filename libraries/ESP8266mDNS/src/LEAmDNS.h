/*
 * LEAmDNS.h
 * (c) 2018, LaborEtArs
 *
 * Version 0.9 beta
 *
 *  Some notes (from LaborEtArs, 2018):
 *  Essentially, this is an rewrite of the original EPS8266 Multicast DNS code (ESP8266mDNS).
 *  The target of this rewrite was to keep the existing interface as stable as possible while
 *  adding and extending the supported set of mDNS features.
 *  A lot of the additions were basicly taken from Erik Ekman's lwIP mdns app code.
 *
 *  Supported mDNS features (in some cases somewhat limited):
 *  - Presenting a DNS-SD service to interested observers, eg. a http server by presenting _http._tcp service
 *  - Support for multi-level compressed names in input; in output only a very simple one-leven full-name compression is implemented
 *  - Probing host and service domains for uniqueness in the local network
 *  - Tiebreaking while probing is supportet in a very minimalistic way (the 'higher' IP address wins the tiebreak)
 *  - Announcing available services after successful probing
 *  - Using fixed service TXT items or
 *  - Using dynamic service TXT items for presented services (via callback)
 *  - Remove services (and un-announcing them to the observers by sending goodbye-messages)
 *  - Static queries for DNS-SD services (creating a fixed answer set after a certain timeout period)
 *  - Dynamic queries for DNS-SD services with cached and updated answers and user notifications
 *
 *
 *  Usage:
 *  In most cases, this implementation should work as a 'drop-in' replacement for the original
 *  ESP8266 Multicast DNS code. Adjustments to the existing code would only be needed, if some
 *  of the new features should be used.
 *
 *  For presenting services:
 *  In 'setup()':
 *    Install a callback for the probing of host (and service) domains via 'MDNS.setProbeResultCallback(probeResultCallback, &userData);'
 *    Register DNS-SD services with 'MDNSResponder::hMDNSService hService = MDNS.addService("MyESP", "http", "tcp", 5000);'
 *    (Install additional callbacks for the probing of these service domains via 'MDNS.setServiceProbeResultCallback(hService, probeResultCallback, &userData);')
 *    Add service TXT items with 'MDNS.addServiceTxt(hService, "c#", "1");' or by installing a service TXT callback
 *    using 'MDNS.setDynamicServiceTxtCallback(dynamicServiceTxtCallback, &userData);' or service specific
 *    'MDNS.setDynamicServiceTxtCallback(hService, dynamicServiceTxtCallback, &userData);'
 *    Call MDNS.begin("MyHostname");
 *
 *  In 'probeResultCallback(MDNSResponder* p_MDNSResponder, const char* p_pcDomain, MDNSResponder:hMDNSService p_hService, bool p_bProbeResult, void* p_pUserdata)':
 *    Check the probe result and update the host or service domain name if the probe failed
 *
 *  In 'dynamicServiceTxtCallback(MDNSResponder* p_MDNSResponder, const hMDNSService p_hService, void* p_pUserdata)':
 *    Add dynamic TXT items by calling 'MDNS.addDynamicServiceTxt(p_hService, "c#", "1");'
 *
 *  In loop():
 *    Call 'MDNS.update();'
 *
 *
 *  For querying services:
 *  Static:
 *    Call 'uint32_t u32AnswerCount = MDNS.queryService("http", "tcp");'
 *    Iterate answers by: 'for (uint32_t u=0; u<u32AnswerCount; ++u) { const char* pHostname = MDNS.answerHostname(u); }'
 *    You should call MDNS.removeQuery() sometimes later (when the answers are nott needed anymore)
 *
 *  Dynamic:
 *    Install a dynamic query by calling 'DNSResponder::hMDNSServiceQuery hServiceQuery = MDNS.installServiceQuery("http", "tcp", serviceQueryCallback, &userData);'
 *    The callback 'serviceQueryCallback(MDNSResponder* p_MDNSResponder, const hMDNSServiceQuery p_hServiceQuery, uint32_t p_u32AnswerIndex,
 *                                       enuServiceQueryAnswerType p_ServiceQueryAnswerType, bool p_bSetContent, void* p_pUserdata)'
 *    is called for any change in the answer set.
 *    Call 'MDNS.removeServiceQuery(hServiceQuery);' when the answers are not needed anymore
 *
 *
 *  Reference:
 *  Used mDNS messages:
 *  A (0x01):               eg. esp8266.local A OP TTL 123.456.789.012
 *  AAAA (01Cx):            eg. esp8266.local AAAA OP TTL 1234:5678::90
 *  PTR (0x0C, srv name):   eg. _http._tcp.local PTR OP TTL MyESP._http._tcp.local
 *  PTR (0x0C, srv type):   eg. _services._dns-sd._udp.local PTR OP TTL _http._tcp.local
 *  PTR (0x0C, IP4):        eg. 012.789.456.123.in-addr.arpa PTR OP TTL esp8266.local
 *  PTR (0x0C, IP6):        eg. 90.0.0.0.0.0.0.0.0.0.0.0.78.56.34.12.ip6.arpa PTR OP TTL esp8266.local
 *  SRV (0x21):             eg. MyESP._http._tcp.local SRV OP TTL PRIORITY WEIGHT PORT esp8266.local
 *  TXT (0x10):             eg. MyESP._http._tcp.local TXT OP TTL c#=1
 *
 *  Some NOT used message types:
 *  OPT (0x29):             eDNS
 *  NSEC (0x2F):            DNSSEC
 *
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

#ifndef LEAMDNS_H
#define LEAMDNS_H

#include <functional>   // for UdpContext.h
#include "WiFiUdp.h"
#include "lwip/udp.h"
#include "debug.h"
#include "include/UdpContext.h"
#include "LEATimeFlag.h"

#include "ESP8266WiFi.h"

/**
 * LEAmDNS
 */
namespace LEAmDNS {

//this should be defined at build time
#ifndef ARDUINO_BOARD
#define ARDUINO_BOARD "generic"
#endif

#define MDNS_IP4_SUPPORT
//#define MDNS_IP6_SUPPORT


#ifdef MDNS_IP4_SUPPORT
    #define MDNS_IP4_SIZE               4
#endif
#ifdef MDNS_IP6_SUPPORT
    #define MDNS_IP6_SIZE               16
#endif
/*
 * Maximum length for all service txts for one service
 */
#define MDNS_SERVICE_TXT_MAXLENGTH      1300
/*
 * Maximum length for a full domain name eg. MyESP._http._tcp.local
 */
#define MDNS_DOMAIN_MAXLENGTH           256
/*
 * Maximum length of on label in a domain name (length info fits into 6 bits)
 */
#define MDNS_DOMAIN_LABEL_MAXLENGTH     63
/*
 * Maximum length of a service name eg. http
 */
#define MDNS_SERVICE_NAME_LENGTH        15
/*
 * Maximum length of a service protocol name eg. tcp
 */
#define MDNS_SERVICE_PROTOCOL_LENGTH    3
/*
 * Default timeout for static service queries
 */
#define MDNS_QUERYSERVICES_WAIT_TIME    1000


/**
 * MDNSResponder
 */
class MDNSResponder {
public:
    /* INTERFACE */
    MDNSResponder(void);
    virtual ~MDNSResponder(void);
    
    // Start the MDNS responder by setting the default hostname
    // Later call MDNS::update() in every 'loop' to run the process loop
    // (probing, announcing, responding, ...)
    bool begin(const char* p_pcHostname);
        // for compatibility
        bool begin(const char* p_pcHostname,
                   IPAddress p_IPAddress,       // ignored
                   uint32_t p_u32TTL = 120);    // ignored
    // Finish MDNS processing
    bool close(void);

    // Change hostname (probing is restarted)
    bool setHostname(const char* p_pcHostname);
        // for compatibility...
        bool setHostname(String p_strHostname);
    
    /**
     * hMDNSService (opaque handle to access the service)
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

    /**
     * hMDNSTxt (opaque handle to access the TXT items)
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
     * MDNSDynamicServiceTxtCallbackFn
     * Callback function for dynamic MDNS TXT items
     */
    typedef bool (*MDNSDynamicServiceTxtCallbackFn)(MDNSResponder* p_pMDNSResponder,
                                                    const hMDNSService p_hService,
                                                    void* p_pUserdata);

    // Set a global callback for dynamic MDNS TXT items. The callback function is called
    // every time, a TXT item is needed for one of the installed services.
    bool setDynamicServiceTxtCallback(MDNSDynamicServiceTxtCallbackFn p_fnCallback,
                                      void* p_pUserdata);
    // Set a service specific callback for dynamic MDNS TXT items. The callback function
    // is called every time, a TXT item is needed for the given service.
    bool setDynamicServiceTxtCallback(const hMDNSService p_hService,
                                      MDNSDynamicServiceTxtCallbackFn p_fnCallback,
                                      void* p_pUserdata);
    
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
                          const uint16_t p_u16Timeout = MDNS_QUERYSERVICES_WAIT_TIME);
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
     * hMDNSServiceQuery (opaque handle to access dynamic service queries)
     */
    typedef const void*     hMDNSServiceQuery;
    
    /**
     * enuServiceQueryAnswerType
     */
    typedef enum _enuServiceQueryAnswerType {
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
     * MDNSServiceQueryCallbackFn
     * Callback function for received answers for dynamic service queries
     */
    typedef bool (*MDNSServiceQueryCallbackFn)(MDNSResponder* p_pMDNSResponder,
                                               const hMDNSServiceQuery p_hServiceQuery, // dynamic service query handle
                                               uint32_t p_u32AnswerIndex,               // index of the updated answer
                                               uint32_t p_u32ServiceQueryAnswerMask,    // flag for the updated answer item
                                               bool p_bSetContent,                      // true: Answer component set, false: component deleted
                                               void* p_pUserdata);                      // pUserdata set via 'installServiceQuery'

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
                                          MDNSServiceQueryCallbackFn p_fnCallback,
                                          void* p_pUserdata);
    // Remove a dynamic service query
    bool removeServiceQuery(hMDNSServiceQuery p_hServiceQuery);

    uint32_t answerCount(const hMDNSServiceQuery p_hServiceQuery);
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
     * MDNSProbeResultCallbackFn
     * Callback function for (host and service domain) probe results
     */
    typedef bool (*MDNSProbeResultCallbackFn)(MDNSResponder* p_pMDNSResponder,
                                              const char* p_pcDomainName,
                                              const hMDNSService p_hMDNSService,    // 0 for host domain
                                              bool p_bProbeResult,
                                              void* p_pUserdata);

    // Set a global callback function for host and service probe results
    // The callback function is called, when the probeing for the host domain
    // (or a service domain, which hasn't got a service specific callback)
    // Succeededs or fails.
    // In case of failure, the failed domain name should be changed.
    bool setProbeResultCallback(MDNSProbeResultCallbackFn p_fnCallback,
                                void* p_pUserdata);
    // Set a service specific probe result callcack
    bool setServiceProbeResultCallback(const hMDNSService p_hService,
                                       MDNSProbeResultCallbackFn p_fnCallback,
                                       void* p_pUserdata);
    
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
    /** STRUCTS **/
    /**
     * stcMDNSServiceTxt
     */
    typedef struct _stcMDNSServiceTxt {
        _stcMDNSServiceTxt* m_pNext;
        char*               m_pcKey;
        char*               m_pcValue;
        bool                m_bTemp;
        
        _stcMDNSServiceTxt(const char* p_pcKey = 0,
                           const char* p_pcValue = 0,
                           bool p_bTemp = false);
        _stcMDNSServiceTxt(const _stcMDNSServiceTxt& p_Other);
        ~_stcMDNSServiceTxt(void);
        
        _stcMDNSServiceTxt& operator=(const _stcMDNSServiceTxt& p_Other);
        bool clear(void);
        
        char* allocKey(size_t p_stLength);
        bool setKey(const char* p_pcKey,
                    size_t p_stLength);
        bool setKey(const char* p_pcKey);
        bool releaseKey(void);
        
        char* allocValue(size_t p_stLength);
        bool setValue(const char* p_pcValue,
                      size_t p_stLength);
        bool setValue(const char* p_pcValue);
        bool releaseValue(void);
        
        bool set(const char* p_pcKey,
                 const char* p_pcValue,
                 bool p_bTemp = false);
        
        bool update(const char* p_pcValue);
        
        size_t length(void) const;
    } stcMDNSServiceTxt;
    
    /**
     * stcMDNSTxts
     */
    typedef struct _stcMDNSServiceTxts {
        stcMDNSServiceTxt*  m_pTxts;
        
        _stcMDNSServiceTxts(void);
        _stcMDNSServiceTxts(const _stcMDNSServiceTxts& p_Other);
        ~_stcMDNSServiceTxts(void);
        
        _stcMDNSServiceTxts& operator=(const _stcMDNSServiceTxts& p_Other);
        
        bool clear(void);
        
        bool add(stcMDNSServiceTxt* p_pTxt);
        bool remove(stcMDNSServiceTxt* p_pTxt);

        bool removeTempTxts(void);
        
        stcMDNSServiceTxt* find(const char* p_pcKey);
        const stcMDNSServiceTxt* find(const char* p_pcKey) const;
        stcMDNSServiceTxt* find(const stcMDNSServiceTxt* p_pTxt);
        
        uint16_t length(void) const;
        
        size_t c_strLength(void) const;
        bool c_str(char* p_pcBuffer);
        
        size_t bufferLength(void) const;
        bool buffer(char* p_pcBuffer);
        
        bool compare(const _stcMDNSServiceTxts& p_Other) const;
        bool operator==(const _stcMDNSServiceTxts& p_Other) const;
        bool operator!=(const _stcMDNSServiceTxts& p_Other) const;
    } stcMDNSServiceTxts;
    
    /**
     * enuContentFlags
     */
    typedef enum _enuContentFlags {
        // Host
        ContentFlag_A           = 0x01,
        ContentFlag_PTR_IP4     = 0x02,
        ContentFlag_PTR_IP6     = 0x04,
        ContentFlag_AAAA        = 0x08,
        // Service
        ContentFlag_PTR_TYPE    = 0x10,
        ContentFlag_PTR_NAME    = 0x20,
        ContentFlag_TXT         = 0x40,
        ContentFlag_SRV         = 0x80,
    } enuContentFlags;

    /**
     * stcMDNS_MsgHeader
     */
    typedef struct _stcMDNS_MsgHeader {
        uint16_t        m_u16ID;            // Identifier
        bool            m_1bQR      : 1;    // Query/Response flag
        unsigned char   m_4bOpcode  : 4;    // Operation code
        bool            m_1bAA      : 1;    // Authoritative Answer flag
        bool            m_1bTC      : 1;    // Truncation flag
        bool            m_1bRD      : 1;    // Recursion desired
        bool            m_1bRA      : 1;    // Recursion available
        unsigned char   m_3bZ       : 3;    // Zero
        unsigned char   m_4bRCode   : 4;    // Response code
        uint16_t        m_u16QDCount;       // Question count
        uint16_t        m_u16ANCount;       // Answer count
        uint16_t        m_u16NSCount;       // Authority Record count
        uint16_t        m_u16ARCount;       // Additional Record count
        
        _stcMDNS_MsgHeader(uint16_t p_u16ID = 0,
                           bool p_bQR = false,
                           unsigned char p_ucOpcode = 0,
                           bool p_bAA = false,
                           bool p_bTC = false,
                           bool p_bRD = false,
                           bool p_bRA = false,
                           unsigned char p_ucRCode = 0,
                           uint16_t p_u16QDCount = 0,
                           uint16_t p_u16ANCount = 0,
                           uint16_t p_u16NSCount = 0,
                           uint16_t p_u16ARCount = 0);
    } stcMDNS_MsgHeader;
        
    /**
     * stcMDNS_RRDomain
     */
    typedef struct _stcMDNS_RRDomain {
        char            m_acName[MDNS_DOMAIN_MAXLENGTH];    // Encoded domain name
        uint16_t        m_u16NameLength;                    // Length (incl. '\0')
        
        _stcMDNS_RRDomain(void);
        _stcMDNS_RRDomain(const _stcMDNS_RRDomain& p_Other);
        
        _stcMDNS_RRDomain& operator=(const _stcMDNS_RRDomain& p_Other);
        
        bool clear(void);
        
        bool addLabel(const char* p_pcLabel,
                      bool p_bPrependUnderline = false);
        
        bool compare(const _stcMDNS_RRDomain& p_Other) const;
        bool operator==(const _stcMDNS_RRDomain& p_Other) const;
        bool operator!=(const _stcMDNS_RRDomain& p_Other) const;
        bool operator>(const _stcMDNS_RRDomain& p_Other) const;
        
        size_t c_strLength(void) const;
        bool c_str(char* p_pcBuffer);
    } stcMDNS_RRDomain;
    
    /**
     * stcMDNS_RRAttributes
     */
    typedef struct _stcMDNS_RRAttributes {
        uint16_t            m_u16Type;      // Type
        uint16_t            m_u16Class;     // Class, nearly always 'IN'
        
        _stcMDNS_RRAttributes(uint16_t p_u16Type = 0,
                              uint16_t p_u16Class = 1 /*DNS_RRCLASS_IN Internet*/);
        _stcMDNS_RRAttributes(const _stcMDNS_RRAttributes& p_Other);
        
        _stcMDNS_RRAttributes& operator=(const _stcMDNS_RRAttributes& p_Other);
    } stcMDNS_RRAttributes;
    
    /**
     * stcMDNS_RRHeader
     */
    typedef struct _stcMDNS_RRHeader {
        stcMDNS_RRDomain        m_Domain;
        stcMDNS_RRAttributes    m_Attributes;
        
        _stcMDNS_RRHeader(void);
        _stcMDNS_RRHeader(const _stcMDNS_RRHeader& p_Other);
        
        _stcMDNS_RRHeader& operator=(const _stcMDNS_RRHeader& p_Other);

        bool clear(void);
    } stcMDNS_RRHeader;
    
    /**
     * stcMDNS_RRQuestion
     */
    typedef struct _stcMDNS_RRQuestion {
        _stcMDNS_RRQuestion*    m_pNext;
        _stcMDNS_RRHeader       m_Header;
        bool                    m_bUnicast;     // Unicast reply requested
        
        _stcMDNS_RRQuestion(void);
    } stcMDNS_RRQuestion;
    
    /**
     * enuAnswerType
     */
    typedef enum _enuAnswerType {
        AnswerType_A,
        AnswerType_PTR,
        AnswerType_TXT,
        AnswerType_AAAA,
        AnswerType_SRV,
        AnswerType_Generic
    } enuAnswerType;
    
    /**
     * stcMDNS_RRAnswer
     */
    typedef struct _stcMDNS_RRAnswer {
        _stcMDNS_RRAnswer*  m_pNext;
        const enuAnswerType m_AnswerType;
        _stcMDNS_RRHeader   m_Header;
        bool                m_bCacheFlush;  // Cache flush command bit
        uint32_t            m_u32TTL;       // Validity time in seconds
        
        virtual ~_stcMDNS_RRAnswer(void);
        
        enuAnswerType answerType(void) const;
        
        bool clear(void);
        
    protected:
        _stcMDNS_RRAnswer(enuAnswerType p_AnswerType,
                          const stcMDNS_RRHeader& p_Header,
                          uint32_t p_u32TTL);
    } stcMDNS_RRAnswer;
    
#ifdef MDNS_IP4_SUPPORT
    /**
     * stcMDNS_RRAnswerA
     */
    typedef struct _stcMDNS_RRAnswerA : public stcMDNS_RRAnswer {
        IPAddress           m_IPAddress;
        
        _stcMDNS_RRAnswerA(const stcMDNS_RRHeader& p_Header,
                           uint32_t p_u32TTL);
        ~_stcMDNS_RRAnswerA(void);
        
        bool clear(void);
    } stcMDNS_RRAnswerA;
#endif

    /**
     * stcMDNS_RRAnswerPTR
     */
    typedef struct _stcMDNS_RRAnswerPTR : public stcMDNS_RRAnswer {
        stcMDNS_RRDomain    m_PTRDomain;
        
        _stcMDNS_RRAnswerPTR(const stcMDNS_RRHeader& p_Header,
                             uint32_t p_u32TTL);
        ~_stcMDNS_RRAnswerPTR(void);
        
        bool clear(void);
    } stcMDNS_RRAnswerPTR;
    
    /**
     * stcMDNS_RRAnswerTXT
     */
    typedef struct _stcMDNS_RRAnswerTXT : public stcMDNS_RRAnswer {
        stcMDNSServiceTxts  m_Txts;
        
        _stcMDNS_RRAnswerTXT(const stcMDNS_RRHeader& p_Header,
                             uint32_t p_u32TTL);
        ~_stcMDNS_RRAnswerTXT(void);
        
        bool clear(void);
    } stcMDNS_RRAnswerTXT;
    
#ifdef MDNS_IP6_SUPPORT
    /**
     * stcMDNS_RRAnswerAAAA
     */
    typedef struct _stcMDNS_RRAnswerAAAA : public stcMDNS_RRAnswer {
        //TODO: IP6Address          m_IPAddress;
        
        _stcMDNS_RRAnswerAAAA(const stcMDNS_RRHeader& p_Header,
                              uint32_t p_u32TTL);
        ~_stcMDNS_RRAnswerAAAA(void);
        
        bool clear(void);
    } stcMDNS_RRAnswerAAAA;
#endif

    /**
     * stcMDNS_RRAnswerSRV
     */
    typedef struct _stcMDNS_RRAnswerSRV : public stcMDNS_RRAnswer {
        uint16_t            m_u16Priority;
        uint16_t            m_u16Weight;
        uint16_t            m_u16Port;
        stcMDNS_RRDomain    m_SRVDomain;
        
        _stcMDNS_RRAnswerSRV(const stcMDNS_RRHeader& p_Header,
                             uint32_t p_u32TTL);
        ~_stcMDNS_RRAnswerSRV(void);
        
        bool clear(void);
    } stcMDNS_RRAnswerSRV;
    
    /**
     * stcMDNS_RRAnswerGeneric
     */
    typedef struct _stcMDNS_RRAnswerGeneric : public stcMDNS_RRAnswer {
        uint16_t            m_u16RDLength;  // Length of variable answer
        uint8_t*            m_pu8RDData;    // Offset of start of variable answer in packet
        
        _stcMDNS_RRAnswerGeneric(const stcMDNS_RRHeader& p_Header,
                                 uint32_t p_u32TTL);
        ~_stcMDNS_RRAnswerGeneric(void);
        
        bool clear(void);
    } stcMDNS_RRAnswerGeneric;


    /**
     * enuProbingStatus
     */
    typedef enum _enuProbingStatus {
        ProbingStatus_WaitingForData,
        ProbingStatus_ReadyToStart,
        ProbingStatus_InProgress,
        ProbingStatus_Done
    } enuProbingStatus;

    /**
     * stcProbeInformation
     */
    typedef struct _stcProbeInformation {
        enuProbingStatus                m_ProbingStatus;
        uint8_t                         m_u8ProbesSent;
        clsLEATimeFlag                  m_NextProbeTimeFlag;
        bool                            m_bConflict;
        bool                            m_bTiebreakNeeded;
        MDNSProbeResultCallbackFn       m_fnProbeResultCallback;
        void*                           m_pProbeResultCallbackUserdata;

        _stcProbeInformation(void);

        bool clear(bool p_bClearUserdata = false);
    } stcProbeInformation;


    /**
     * stcMDNSService
     */
    typedef struct _stcMDNSService {
        _stcMDNSService*                m_pNext;
        char*                           m_pcName;
        bool                            m_bAutoName;    // Name was set automatically to hostname (if no name was supplied)
        char*                           m_pcService;
        char*                           m_pcProtocol;
        uint16_t                        m_u16Port;
        uint8_t                         m_u8ReplyMask;
        stcMDNSServiceTxts              m_Txts;
        MDNSDynamicServiceTxtCallbackFn m_fnTxtCallback;
        void*                           m_pTxtCallbackUserdata;
        stcProbeInformation             m_ProbeInformation;

        _stcMDNSService(const char* p_pcName = 0,
                        const char* p_pcService = 0,
                        const char* p_pcProtocol = 0);
        ~_stcMDNSService(void);

        bool setName(const char* p_pcName);
        bool releaseName(void);
        
        bool setService(const char* p_pcService);
        bool releaseService(void);
        
        bool setProtocol(const char* p_pcProtocol);
        bool releaseProtocol(void);
    } stcMDNSService;

    /**
     * stcMDNSServiceQuery
     */
    typedef struct _stcMDNSServiceQuery {
        /**
         * stcAnswer
         */
        typedef struct _stcAnswer {
            /**
             * stcTTL
             */
            typedef struct _stcTTL {
                clsLEATimeFlag  m_TTLTimeFlag;
                bool            m_bUpdateScheduled;

                _stcTTL(uint32_t p_u32TTL = 0);
                bool set(uint32_t p_u32TTL);

                bool has80Percent(void) const;
                bool isOutdated(void) const;
            } stcTTL;
#ifdef MDNS_IP4_SUPPORT
            /**
             * stcIP4Address
             */
            typedef struct _stcIP4Address {
                _stcIP4Address* m_pNext;
                IPAddress       m_IPAddress;
                stcTTL          m_TTL;
                
                _stcIP4Address(IPAddress p_IPAddress,
                               uint32_t p_u32TTL = 0);
            } stcIP4Address;
#endif
#ifdef MDNS_IP6_SUPPORT
            /**
             * stcIP6Address
             */
            typedef struct _stcIP6Address {
                _stcIP6Address* m_pNext;
                IP6Address      m_IPAddress;
                stcTTL          m_TTL;
            } stcIP6Address;
#endif

            _stcAnswer*         m_pNext;
            // The service domain is the first 'answer' (from PTR answer, using service and protocol) to be set
            // Defines the key for additional answer, like host domain, etc.
            stcMDNS_RRDomain    m_ServiceDomain;    // 1. level answer (PTR), eg. MyESP._http._tcp.local
            char*               m_pcServiceDomain;
            stcTTL              m_TTLServiceDomain;
            stcMDNS_RRDomain    m_HostDomain;       // 2. level answer (SRV, using service domain), eg. esp8266.local
            char*               m_pcHostDomain;
            uint16_t            m_u16Port;          // 2. level answer (SRV, using service domain), eg. 5000
            stcTTL              m_TTLHostDomainAndPort;
            stcMDNSServiceTxts  m_Txts;             // 2. level answer (TXT, using service domain), eg. c#=1
            char*               m_pcTxts;
            stcTTL              m_TTLTxts;
#ifdef MDNS_IP4_SUPPORT
            stcIP4Address*      m_pIP4Addresses;    // 3. level answer (A, using host domain), eg. 123.456.789.012
#endif
#ifdef MDNS_IP6_SUPPORT
            stcIP6Address*      m_pIP6Addresses;    // 3. level answer (AAAA, using host domain), eg. 1234::09
#endif
            uint32_t            m_u32ContentFlags;

            _stcAnswer(void);
            ~_stcAnswer(void);

            bool clear(void);
            
            char* allocServiceDomain(size_t p_stLength);
            bool releaseServiceDomain(void);
            
            char* allocHostDomain(size_t p_stLength);
            bool releaseHostDomain(void);
            
            char* allocTxts(size_t p_stLength);
            bool releaseTxts(void);
            
#ifdef MDNS_IP4_SUPPORT
            bool releaseIP4Addresses(void);
            bool addIP4Address(_stcIP4Address* p_pIP4Address);
            bool removeIP4Address(_stcIP4Address* p_pIP4Address);
            const stcIP4Address* findIP4Address(const IPAddress& p_IPAddress) const;
            stcIP4Address* findIP4Address(const IPAddress& p_IPAddress);
            uint32_t IP4AddressCount(void) const;
            const stcIP4Address* IP4AddressAtIndex(uint32_t p_u32Index) const;
            stcIP4Address* IP4AddressAtIndex(uint32_t p_u32Index);
#endif
#ifdef MDNS_IP6_SUPPORT
            bool releaseIP6Addresses(void);
            bool addIP6Address(_stcIP6Address* p_pIP6Address);
            bool removeIP6Address(_stcIP6Address* p_pIP6Address);
            const stcIP6Address* findIP6Address(const IPAddress& p_IPAddress) const;
            stcIP6Address* findIP6Address(const IPAddress& p_IPAddress);
            uint32_t IP6AddressCount(void) const;
            const stcIP6Address* IP6AddressAtIndex(uint32_t p_u32Index) const;
            stcIP6Address* IP6AddressAtIndex(uint32_t p_u32Index);
#endif
        } stcAnswer;

        _stcMDNSServiceQuery*       m_pNext;
        stcMDNS_RRDomain            m_ServiceTypeDomain;    // eg. _http._tcp.local
        MDNSServiceQueryCallbackFn  m_fnCallback;
        void*                       m_pUserdata;
        bool                        m_bLegacyQuery;
        bool                        m_bAwaitingAnswers;
        stcAnswer*                  m_pAnswers;

        _stcMDNSServiceQuery(void);
        ~_stcMDNSServiceQuery(void);

        bool clear(void);
        
        uint32_t answerCount(void) const;
        const stcAnswer* answerAtIndex(uint32_t p_u32Index) const;
        stcAnswer* answerAtIndex(uint32_t p_u32Index);
        uint32_t indexOfAnswer(const stcAnswer* p_pAnswer) const;
        
        bool addAnswer(stcAnswer* p_pAnswer);
        bool removeAnswer(stcAnswer* p_pAnswer);
        
        stcAnswer* findAnswerForServiceDomain(const stcMDNS_RRDomain& p_ServiceDomain);
        stcAnswer* findAnswerForHostDomain(const stcMDNS_RRDomain& p_HostDomain);
    } stcMDNSServiceQuery;

    /**
     * stcMDNSSendParameter
     */
    typedef struct _stcMDNSSendParameter {
    protected:
        /**
         * stcDomainCacheItem
         */
        typedef struct _stcDomainCacheItem {
            _stcDomainCacheItem*    m_pNext;
            const void*             m_pHostnameOrService;   // Opaque id for host or service domain (pointer)
            bool                    m_bAdditionalData;      // Opaque flag for special info (service domain included)
            uint16_t                m_u16Offset;            // Offset in UDP output buffer
            
            _stcDomainCacheItem(const void* p_pHostnameOrService,
                                bool p_bAdditionalData,
                                uint32_t p_u16Offset);
        } stcDomainCacheItem;
    
    public: 
        uint16_t                m_u16ID;                    // Query ID (used only in lagacy queries)
        stcMDNS_RRQuestion*     m_pQuestions;               // A list of queries
        uint8_t                 m_u8HostReplyMask;          // Flags for reply components/answers
        bool                    m_bLegacyQuery;             // Flag: Legacy query
        bool                    m_bResponse;                // Flag: Response to a query
        bool                    m_bAuthorative;             // Flag: Authorative (owner) response
        bool                    m_bCacheFlush;              // Flag: Clients should flush their caches
        bool                    m_bUnicast;                 // Flag: Unicast response
        bool                    m_bUnannounce;              // Flag: Unannounce service
        uint16_t                m_u16Offset;                // Current offset in UDP write buffer (mainly for domain cache)
        stcDomainCacheItem*     m_pDomainCacheItems;        // Cached host and service domains
        
        _stcMDNSSendParameter(void);
        ~_stcMDNSSendParameter(void);
        
        bool clear(void);
        
        bool shiftOffset(uint16_t p_u16Shift);
        
        bool addDomainCacheItem(const void* p_pHostnameOrService,
                                bool p_bAdditionalData,
                                uint16_t p_u16Offset);
        uint16_t findCachedDomainOffset(const void* p_pHostnameOrService,
                                        bool p_bAdditionalData) const;
    } stcMDNSSendParameter;
    
    // Instance variables
    stcMDNSService*                 m_pServices;
    UdpContext*                     m_pUDPContext;
    char*                           m_pcHostname;
    stcMDNSServiceQuery*            m_pServiceQueries;
    WiFiEventHandler                m_DisconnectedHandler;
    WiFiEventHandler                m_GotIPHandler;
    MDNSDynamicServiceTxtCallbackFn m_fnServiceTxtCallback;
    void*                           m_pServiceTxtCallbackUserdata;
    stcProbeInformation             m_HostProbeInformation;

    /** CONTROL **/
    /* MAINTENANCE */
    bool _process(bool p_bUserContext);
    bool _restart(void);
    
    /* RECEIVING */
    bool _parseMessage(void);
    bool _parseQuery(const stcMDNS_MsgHeader& p_Header);
    
    bool _parseResponse(const stcMDNS_MsgHeader& p_Header);
    bool _processAnswers(const stcMDNS_RRAnswer* p_pPTRAnswers);
    bool _processPTRAnswer(const stcMDNS_RRAnswerPTR* p_pPTRAnswer,
                           bool& p_rbFoundNewKeyAnswer);
    bool _processSRVAnswer(const stcMDNS_RRAnswerSRV* p_pSRVAnswer,
                           bool& p_rbFoundNewKeyAnswer);
    bool _processTXTAnswer(const stcMDNS_RRAnswerTXT* p_pTXTAnswer);
#ifdef MDNS_IP4_SUPPORT
    bool _processAAnswer(const stcMDNS_RRAnswerA* p_pAAnswer);
#endif
#ifdef MDNS_IP6_SUPPORT
    bool _processAAAAAnswer(const stcMDNS_RRAnswerAAAA* p_pAAAAAnswer);
#endif
    
    /* PROBING */
    bool _updateProbeStatus(void);
    bool _resetProbeStatus(bool p_bRestart = true);
    bool _hasProbesWaitingForAnswers(void) const;
    bool _sendHostProbe(void);
    bool _sendServiceProbe(stcMDNSService& p_rService);
    bool _cancelProbingForHost(void);
    bool _cancelProbingForService(stcMDNSService& p_rService);
    
    /* ANNOUNCE */
    bool _announce(bool p_bAnnounce = true);
    bool _announceService(stcMDNSService& p_rService,
                          bool p_bAnnounce = true);
    
    /* SERVICE QUERY CACHE */
    bool _hasServiceQueriesWaitingForAnswers(void) const;
    bool _checkServiceQueryCache(void);
    
    /** TRANSFER **/
    /* SENDING */   
    bool _sendMDNSMessage(stcMDNSSendParameter& p_SendParameter);
    bool _sendMDNSMessage_Multicast(MDNSResponder::stcMDNSSendParameter& p_rSendParameter,
                                    int p_iWiFiOpMode);
    bool _prepareMDNSMessage(stcMDNSSendParameter& p_SendParameter,
                             IPAddress p_IPAddress);
    bool _sendMDNSServiceQuery(const stcMDNSServiceQuery& p_ServiceQuery);
    bool _sendMDNSQuery(const stcMDNS_RRDomain& p_QueryDomain,
                        uint16_t p_u16QueryType);
                        
    IPAddress _getResponseMulticastInterface(int p_iWiFiOpModes) const;

    uint8_t _replyMaskForHost(const stcMDNS_RRHeader& p_RRHeader,
                              bool* p_pbFullNameMatch = 0) const;
    uint8_t _replyMaskForService(const stcMDNS_RRHeader& p_RRHeader,
                                 const stcMDNSService& p_Service,
                                 bool* p_pbFullNameMatch = 0) const;
    
    /* RESOURCE RECORD */
    bool _readRRQuestion(stcMDNS_RRQuestion& p_rQuestion);
    bool _readRRAnswer(stcMDNS_RRAnswer*& p_rpAnswer);
#ifdef MDNS_IP4_SUPPORT
    bool _readRRAnswerA(stcMDNS_RRAnswerA& p_rRRAnswerA,
                        uint16_t p_u16RDLength);
#endif
    bool _readRRAnswerPTR(stcMDNS_RRAnswerPTR& p_rRRAnswerPTR,
                          uint16_t p_u16RDLength);
    bool _readRRAnswerTXT(stcMDNS_RRAnswerTXT& p_rRRAnswerTXT,
                          uint16_t p_u16RDLength);
#ifdef MDNS_IP6_SUPPORT
    bool _readRRAnswerAAAA(stcMDNS_RRAnswerAAAA& p_rRRAnswerAAAA,
                           uint16_t p_u16RDLength);
#endif
    bool _readRRAnswerSRV(stcMDNS_RRAnswerSRV& p_rRRAnswerSRV,
                          uint16_t p_u16RDLength);
    bool _readRRAnswerGeneric(stcMDNS_RRAnswerGeneric& p_rRRAnswerGeneric,
                              uint16_t p_u16RDLength);

    bool _readRRHeader(stcMDNS_RRHeader& p_rHeader);
    bool _readRRDomain(stcMDNS_RRDomain& p_rRRDomain);
    bool _readRRDomain_Loop(stcMDNS_RRDomain& p_rRRDomain,
                            uint8_t p_u8Depth);
    bool _readRRAttributes(stcMDNS_RRAttributes& p_rAttributes);
    
    /* DOMAIN NAMES */
    bool _buildDomainForHost(const char* p_pcHostname,
                             stcMDNS_RRDomain& p_rHostDomain) const;
    bool _buildDomainForDNSSD(stcMDNS_RRDomain& p_rDNSSDDomain) const;
    bool _buildDomainForService(const stcMDNSService& p_Service,
                                bool p_bIncludeName,
                                stcMDNS_RRDomain& p_rServiceDomain) const;
    bool _buildDomainForService(const char* p_pcService,
                                const char* p_pcProtocol,
                                stcMDNS_RRDomain& p_rServiceDomain) const;
#ifdef MDNS_IP4_SUPPORT
    bool _buildDomainForReverseIP4(IPAddress p_IP4Address,
                                   stcMDNS_RRDomain& p_rReverseIP4Domain) const;
#endif
#ifdef MDNS_IP6_SUPPORT
    bool _buildDomainForReverseIP6(IPAddress p_IP4Address,
                                   stcMDNS_RRDomain& p_rReverseIP6Domain) const;
#endif

    /* UDP */
    bool _udpReadBuffer(unsigned char* p_pBuffer,
                        size_t p_stLength);
    bool _udpRead8(uint8_t& p_ru8Value);
   bool _udpRead16(uint16_t& p_ru16Value);
    bool _udpRead32(uint32_t& p_ru32Value);

   bool _udpAppendBuffer(const unsigned char* p_pcBuffer,
                       size_t p_stLength);
  bool _udpAppend8(uint8_t p_u8Value);
    bool _udpAppend16(uint16_t p_u16Value);
    bool _udpAppend32(uint32_t p_u32Value);

#if not defined ESP_8266_MDNS_INCLUDE || defined DEBUG_ESP_MDNS_RESPONDER
    bool _udpDump(bool p_bMovePointer = false);
    bool _udpDump(unsigned p_uOffset,
                  unsigned p_uLength);
#endif
    
    /* READ/WRITE MDNS STRUCTS */
    bool _readMDNSMsgHeader(stcMDNS_MsgHeader& p_rMsgHeader);
    
    bool _write8(uint8_t p_u8Value,
                 stcMDNSSendParameter& p_rSendParameter);
    bool _write16(uint16_t p_u16Value,
                  stcMDNSSendParameter& p_rSendParameter);
    bool _write32(uint32_t p_u32Value,
                  stcMDNSSendParameter& p_rSendParameter);

    bool _writeMDNSMsgHeader(const stcMDNS_MsgHeader& p_MsgHeader,
                             stcMDNSSendParameter& p_rSendParameter);
    bool _writeMDNSRRAttributes(const stcMDNS_RRAttributes& p_Attributes,
                                stcMDNSSendParameter& p_rSendParameter);
    bool _writeMDNSRRDomain(const stcMDNS_RRDomain& p_Domain,
                            stcMDNSSendParameter& p_rSendParameter);
    bool _writeMDNSHostDomain(const char* m_pcHostname,
                              bool p_bPrependRDLength,
                              stcMDNSSendParameter& p_rSendParameter);
    bool _writeMDNSServiceDomain(const stcMDNSService& p_Service,
                                 bool p_bIncludeName,
                                 bool p_bPrependRDLength,
                                 stcMDNSSendParameter& p_rSendParameter);
    
    bool _writeMDNSQuestion(stcMDNS_RRQuestion& p_Question,
                            stcMDNSSendParameter& p_rSendParameter);

#ifdef MDNS_IP4_SUPPORT                     
    bool _writeMDNSAnswer_A(IPAddress p_IPAddress,
                            stcMDNSSendParameter& p_rSendParameter);
    bool _writeMDNSAnswer_PTR_IP4(IPAddress p_IPAddress,
                                  stcMDNSSendParameter& p_rSendParameter);
#endif
    bool _writeMDNSAnswer_PTR_TYPE(stcMDNSService& p_rService,
                                   stcMDNSSendParameter& p_rSendParameter);
    bool _writeMDNSAnswer_PTR_NAME(stcMDNSService& p_rService,
                                   stcMDNSSendParameter& p_rSendParameter);
    bool _writeMDNSAnswer_TXT(stcMDNSService& p_rService,
                              stcMDNSSendParameter& p_rSendParameter);
#ifdef MDNS_IP6_SUPPORT
    bool _writeMDNSAnswer_AAAA(IPAddress p_IPAddress,
                               stcMDNSSendParameter& p_rSendParameter);
    bool _writeMDNSAnswer_PTR_IP6(IPAddress p_IPAddress,
                                  stcMDNSSendParameter& p_rSendParameter);
#endif
    bool _writeMDNSAnswer_SRV(stcMDNSService& p_rService,
                              stcMDNSSendParameter& p_rSendParameter);

    /** HELPERS **/
    /* UDP CONTEXT */
    bool _callProcess(void);
    bool _allocUDPContext(void);
    bool _releaseUDPContext(void);

    /* SERVICE QUERY */
    stcMDNSServiceQuery* _allocServiceQuery(void);
    bool _removeServiceQuery(stcMDNSServiceQuery* p_pServiceQuery);
    bool _removeLegacyServiceQuery(void);
    stcMDNSServiceQuery* _findServiceQuery(hMDNSServiceQuery p_hServiceQuery);
    stcMDNSServiceQuery* _findLegacyServiceQuery(void);
    bool _releaseServiceQueries(void);
    stcMDNSServiceQuery* _findNextServiceQueryByServiceType(const stcMDNS_RRDomain& p_ServiceDomain,
                                                            const stcMDNSServiceQuery* p_pPrevServiceQuery);
    
    /* HOSTNAME */
    bool _setHostname(const char* p_pcHostname);
    bool _releaseHostname(void);
    
    /* SERVICE */
    stcMDNSService* _allocService(const char* p_pcName,
                                  const char* p_pcService,
                                  const char* p_pcProtocol,
                                  uint16_t p_u16Port);
    bool _releaseService(stcMDNSService* p_pService);
    bool _releaseServices(void);
    
    stcMDNSService* _findService(const char* p_pcName,
                                 const char* p_pcService,
                                 const char* p_pcProtocol);
    stcMDNSService* _findService(const hMDNSService p_hService);
    
    size_t _countServices(void) const;
    
    /* SERVICE TXT */
    stcMDNSServiceTxt* _allocServiceTxt(stcMDNSService* p_pService,
                                        const char* p_pcKey,
                                        const char* p_pcValue,
                                        bool p_bTemp);
    bool _releaseServiceTxt(stcMDNSService* p_pService,
                            stcMDNSServiceTxt* p_pTxt);
    stcMDNSServiceTxt* _updateServiceTxt(stcMDNSService* p_pService,
                                         stcMDNSServiceTxt* p_pTxt,
                                         const char* p_pcValue,
                                         bool p_bTemp);
    
    stcMDNSServiceTxt* _findServiceTxt(stcMDNSService* p_pService,
                                       const char* p_pcKey);
    stcMDNSServiceTxt* _findServiceTxt(stcMDNSService* p_pService,
                                       const hMDNSTxt p_hTxt);
    
    stcMDNSServiceTxt* _addServiceTxt(stcMDNSService* p_pService,
                                      const char* p_pcKey,
                                      const char* p_pcValue,
                                      bool p_bTemp);

    bool _collectServiceTxts(stcMDNSService& p_rService);
    bool _releaseTempServiceTxts(stcMDNSService& p_rService);
    const stcMDNSServiceTxt* _serviceTxts(const char* p_pcName,
                                          const char* p_pcService,
                                          const char* p_pcProtocol);
                                   
    /* MISC */
#if not defined ESP_8266_MDNS_INCLUDE || defined DEBUG_ESP_MDNS_RESPONDER
        bool _printRRDomain(const stcMDNS_RRDomain& p_rRRDomain) const;
        bool _printRRAnswer(const MDNSResponder::stcMDNS_RRAnswer& p_RRAnswer) const;
#endif
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_MDNS)
    extern MDNSResponder MDNS;
#endif

}   // namespace LEAmDNS

#endif // LEAMDNS_H







