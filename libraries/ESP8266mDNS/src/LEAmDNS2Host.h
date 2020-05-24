/*
    LEAmDNS2Host.h
    (c) 2020, LaborEtArs

    Version 0.9 beta

    Some notes (from LaborEtArs, 2020):

    Supported mDNS features (in some cases somewhat limited):
    - Announcing a DNS-SD service to interested observers, eg. a http server by announcing a esp8266._http._tcp.local. service
    - Support for multi-level compressed names in input; in output only a very simple one-leven full-name compression is implemented
    - Probing host and service domains for uniqueness in the local network
    - Tiebreaking while probing is supportet in a very minimalistic way (the 'higher' IP address wins the tiebreak)
    - Announcing available services after successful probing
    - Using fixed service TXT items or
    - Using dynamic service TXT items for presented services (via callback)
    - Remove services (and un-announcing them to the observers by sending goodbye-messages)
    - Static queries for hosts or DNS-SD services (creating a fixed answer set after a certain timeout period)
    - Dynamic queries for hosts or DNS-SD services with cached and updated answers and user notifications


    Usage:
    A LEAmDNS2Host is attached to an existing netif (Network Interface).
    If more than one netif is used (eg. in WIFI_AP_STA mode) and mDNS support is needed, every used netif needs its own LEAmDNS2Host!

    For presenting services:
    In 'setup()':
      Create an clsLEAMDNSHost instance for every netif you plan to use.
      Call 'begin' on every instance with the intended hostname and the associated netif (or WiFi mode, WIFI_STA).
      The given hostname is the 'probed' for uniqueness in the netifs local link. If domain name conflicts occure, the host name
      will be automatically changed until it is unique in the local link.
      Optionally a callback can be registered in 'begin', to control the probing process manually.
      Next you can register DNS-SD services with 'addService("MyESP", "http", "tcp", 5000)'
      All added service domain names are also probed for uniqueness and updated if needed.
      Optionally a 'probe result' callback can be given for every service in 'addService', too.

      Finally you can add service TXT items with 'pService->addServiceTxt("c#", "1")' or by installing a service TXT callback
      using 'pService->setDynamicServiceTxtCallback()' and calling 'pService->addDynamicServiceTxt("c#", "1")' inside.

    In 'loop()':
      Call 'update()' for every clsLEAmDNS_Host instance.

    For querying services/hosts:
    Static:
      Call 'queryService("http", "tcp")' or 'queryHost("esp8266")';
      You should call MDNS.removeQuery() sometimes later (when the answers are not needed anymore)

    Dynamic:
      Install a dynamic service query by calling 'installService/HostQuery("http", "tcp", serviceQueryCallback);'
      The callback is called for any change in the answer set.
      Call 'MDNS.removeQuery(pQuery)' when the answers are not needed anymore


    Reference:
    Used mDNS messages:
    A (0x01):               eg. esp8266.local A OP TTL 123.456.789.012
    AAAA (0x1C):            eg. esp8266.local AAAA OP TTL 1234:5678::90
    PTR (0x0C, srv name):   eg. _http._tcp.local PTR OP TTL MyESP._http._tcp.local
    PTR (0x0C, srv type):   eg. _services._dns-sd._udp.local PTR OP TTL _http._tcp.local
    PTR (0x0C, IPv4):       eg. 012.789.456.123.in-addr.arpa PTR OP TTL esp8266.local
    PTR (0x0C, IPv6):       eg. 90.0.0.0.0.0.0.0.0.0.0.0.78.56.34.12.ip6.arpa PTR OP TTL esp8266.local
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

#ifndef __LEAMDNS2HOST_H__
#define __LEAMDNS2HOST_H__

#include <list>
#include <vector>
#include <map>
#include <functional>
#include <PolledTimeout.h>

#include <lwip/netif.h>
#include <WiFiUdp.h>
#include <lwip/udp.h>
#include <debug.h>
#include <include/UdpContext.h>

#include <ESP8266WiFi.h>

#include "LEAmDNS2_lwIPdefs.h"


#define MDNS_IPV4_SUPPORT
#if LWIP_IPV6
#define MDNS_IPV6_SUPPORT   // If we've got IPv6 support, then we need IPv6 support :-)
#endif

/*
    LWIP_OPEN_SRC
*/
#ifndef LWIP_OPEN_SRC
#define LWIP_OPEN_SRC
#endif

/*
    Enable class debug functions
*/
#define ESP_8266_MDNS_INCLUDE
#define DEBUG_ESP_MDNS_RESPONDER

/*
    Enable/disable debug trace macros
*/
#if defined(DEBUG_ESP_PORT) && defined(DEBUG_ESP_MDNS_RESPONDER)
#define DEBUG_ESP_MDNS_INFO
#define DEBUG_ESP_MDNS_INFO2
#define DEBUG_ESP_MDNS_ERR
#define DEBUG_ESP_MDNS_TX
#define DEBUG_ESP_MDNS_RX
#endif

#ifdef DEBUG_ESP_MDNS_RESPONDER
#ifdef DEBUG_ESP_MDNS_INFO
#define DEBUG_EX_INFO(A)    A
#define DEBUG_EX_INFO_IF(C,A...)    do if (C) { A; } while (0)
#else
#define DEBUG_EX_INFO(A)
#define DEBUG_EX_INFO_IF(C,A...)
#endif
#ifdef DEBUG_ESP_MDNS_INFO2
#define DEBUG_EX_INFO2(A)   A
#define DEBUG_EX_INFO2_IF(C,A...)    do if (C) { A; } while (0)
#else
#define DEBUG_EX_INFO2(A)
#define DEBUG_EX_INFO2_IF(C,A...)
#endif
#ifdef DEBUG_ESP_MDNS_ERR
#define DEBUG_EX_ERR(A) A
#define DEBUG_EX_ERR_IF(C,A...)    do if (C) { A; } while (0)
#else
#define DEBUG_EX_ERR(A)
#define DEBUG_EX_ERR_IF(C,A...)
#endif
#ifdef DEBUG_ESP_MDNS_TX
#define DEBUG_EX_TX(A)  A
#else
#define DEBUG_EX_TX(A)
#endif
#ifdef DEBUG_ESP_MDNS_RX
#define DEBUG_EX_RX(A)  A
#else
#define DEBUG_EX_RX(A)
#endif

#ifdef DEBUG_ESP_PORT
#define DEBUG_OUTPUT DEBUG_ESP_PORT
#else
#define DEBUG_OUTPUT Serial
#endif
#else
#define DEBUG_EX_INFO(A)
#define DEBUG_EX_INFO2(A)
#define DEBUG_EX_ERR(A)
#define DEBUG_EX_TX(A)
#define DEBUG_EX_RX(A)
#endif

/*
    Enable/disable the usage of the F() macro in debug trace printf calls.
    There needs to be an PGM comptible printf function to use this.

    USE_PGM_PRINTF and F
*/
#define USE_PGM_PRINTF

#ifdef USE_PGM_PRINTF
#else
#ifdef F
#undef F
#endif
#define F(A)    A
#endif


namespace esp8266
{


namespace experimental
{


/**
    clsLEAMDNSHost
*/
class clsLEAMDNSHost
{
protected:
    /*
        clsConsts
    */
    class clsConsts
    {
    public:
#ifdef MDNS_IPV4_SUPPORT
        static const uint16_t   u16IPv4Size                     = 4;        // IPv4 address size in bytes
#endif
#ifdef MDNS_IPV6_SUPPORT
        static const uint16_t   u16IPv6Size                     = 16;       // IPv6 address size in bytes
#endif
        static const size_t     stServiceTxtMaxLength           = 1300;     // Maximum length for all service txts for one service
        static const size_t     stDomainMaxLength               = 256;      // Maximum length for a full domain name eg. MyESP._http._tcp.local
        static const size_t     stDomainLabelMaxLength          = 63;       // Maximum length of on label in a domain name (length info fits into 6 bits)
        static const size_t     stServiceTypeMaxLength          = 15;       // Maximum length of a service name eg. http
        static const size_t     stServiceProtocolMaxLength      = 3;        // Maximum length of a service protocol name eg. tcp

        static const uint32_t   u32LegacyTTL                    = 10;       // Legacy DNS record TTL
        static const uint32_t   u32HostTTL                      = 120;      // Host level records are set to 2min (120s)
        static const uint32_t   u32ServiceTTL                   = 4500;     // Service level records are set to 75min (4500s)

        static const uint16_t   u16SRVPriority                  = 0;        // Default service priority and weight in SRV answers
        static const uint16_t   u16SRVWeight                    = 0;        //
        static const uint8_t    u8DomainCompressMark            = 0xC0;     // Compressed labels are flaged by the two topmost bits of the length byte being set
        static const uint8_t    u8DomainMaxRedirections         = 6;        // Avoid endless recursion because of malformed compressed labels

        static const uint32_t   u32ProbeDelay                   = 1000;     // Default 250, but ESP is slow...; delay between and number of probes for host and service domains
        static const uint32_t   u32ProbeCount                   = 3;
        static const uint32_t   u32AnnounceDelay                = 1000;     // Delay between and number of announces for host and service domains
        static const uint32_t   u32AnnounceCount                = 3;
        static const uint32_t   u32DynamicQueryResendDelay      = 1000;     // Delay between and number of queries; the delay is multiplied by the resent number in '_checkQueryCache'

        static const char*      pcLocal;                                    // "local";
        static const char*      pcServices;                                 // "services";
        static const char*      pcDNSSD;                                    // "dns-sd";
        static const char*      pcUDP;                                      // "udp";
        //static const char*      pcTCP;                                      // "tcp";

#ifdef MDNS_IPV4_SUPPORT
        static const char*      pcReverseIPv4Domain;                        // "in-addr";
#endif
#ifdef MDNS_IPV6_SUPPORT
        static const char*      pcReverseIPv6Domain;                        // "ip6";
#endif
        static const char*      pcReverseTopDomain;                         // "arpa";

#ifdef DNS_RRTYPE_NSEC
        static const uint8_t    u8DNS_RRTYPE_NSEC               = DNS_RRTYPE_NSEC;
#else
        static const uint8_t    u8DNS_RRTYPE_NSEC               = 0x2F;
#endif
        static const uint32_t   u32SendCooldown                 = 50;       // Delay (ms) between to 'UDPContext->send()' calls

    };

    /**
         list
    */
    //using list = std::list<clsLEAMDNSHost*>;

    // File: ..._Backbone
    /**
        clsBackbone
        XXXX should be merged with holder clsLEAMDNSHost because there is no list anymore in it
    */
    class clsBackbone
    {
    public:
        static clsBackbone* sm_pBackbone;
        clsBackbone(void);
        ~clsBackbone(void);

        bool init(void);

        UdpContext* addHost(clsLEAMDNSHost* p_pHost);
        bool removeHost(clsLEAMDNSHost* p_pHost);
        size_t hostCount(void) const;
        bool setDelayUDPProcessing(bool p_bDelayProcessing);
        
        clsLEAMDNSHost* getUniqueHost() { return m_uniqueHost; }

    protected:
        UdpContext*         m_pUDPContext;
        bool                m_bDelayUDPProcessing;
        uint32_t            m_u32DelayedDatagrams;
        //list                m_HostList;
        clsLEAMDNSHost*     m_uniqueHost;

        bool _allocUDPContext(void);
        bool _releaseUDPContext(void);

        bool _processUDPInput(void);

        const clsLEAMDNSHost* _findHost() const { return m_uniqueHost; }
        clsLEAMDNSHost* _findHost() { return m_uniqueHost; }

        const char* _DH(void) const;
    };


    // File: ..._Host_Structs
    /**
        typeIPProtocolType & enuIPProtocolType
    */
    using typeIPProtocolType = uint8_t;
    enum class enuIPProtocolType : typeIPProtocolType
    {
#ifdef MDNS_IPV4_SUPPORT
        V4  =   0x01,
#endif
#ifdef MDNS_IPV6_SUPPORT
        V6  =   0x02,
#endif
    };

    /**
        typeNetIfState & enuNetIfState
    */
    using   typeNetIfState = uint8_t;
    enum class enuNetIfState : typeNetIfState
    {
        None        = 0x00,

        IsUp        = 0x01,
        UpMask      = (IsUp),

        LinkIsUp    = 0x02,
        LinkMask    = (LinkIsUp),

        IPv4        = 0x04,
        IPv6        = 0x08,
        IPMask      = (IPv4 | IPv6),
    };

public:

    static constexpr auto ApiVersion = MDNSApiVersion::LEAv2;

    /**
        clsServiceTxt
    */
    class clsServiceTxt
    {
    public:
        char*           m_pcKey;
        char*           m_pcValue;
        bool            m_bTemp;

        clsServiceTxt(const char* p_pcKey = 0,
                      const char* p_pcValue = 0,
                      bool p_bTemp = false);
        clsServiceTxt(const clsServiceTxt& p_Other);
        ~clsServiceTxt(void);

        clsServiceTxt& operator=(const clsServiceTxt& p_Other);
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

        /**
            list
        */
        using list = std::list<clsServiceTxt*>;
    };

    /**
        clsServiceTxts
    */
    class clsServiceTxts
    {
    public:
        clsServiceTxt::list     m_Txts;
        char*                   m_pcCache;

        clsServiceTxts(void);
        clsServiceTxts(const clsServiceTxts& p_Other);
        ~clsServiceTxts(void);

        clsServiceTxts& operator=(const clsServiceTxts& p_Other);

        bool clear(void);
        bool clearCache(void);

        bool add(clsServiceTxt* p_pTxt);
        bool remove(clsServiceTxt* p_pTxt);

        size_t count(void) const;

        bool removeTempTxts(void);

        clsServiceTxt* find(const char* p_pcKey);
        const clsServiceTxt* find(const char* p_pcKey) const;
        clsServiceTxt* find(const clsServiceTxt* p_pTxt);

        size_t length(void) const;

        size_t c_strLength(void) const;
        bool c_str(char* p_pcBuffer);
        const char* c_str(void) const;

        size_t bufferLength(void) const;
        bool buffer(char* p_pcBuffer);

        bool compare(const clsServiceTxts& p_Other) const;
        bool operator==(const clsServiceTxts& p_Other) const;
        bool operator!=(const clsServiceTxts& p_Other) const;
    };

protected:
    /**
        clsProbeInformation_Base
    */
    class clsProbeInformation_Base
    {
    public:
        /**
            typeProbingStatus & enuProbingStatus
        */
        using typeProbingStatus = uint8_t;
        enum class enuProbingStatus : typeProbingStatus
        {
            WaitingForData,
            ReadyToStart,
            InProgress,
            ReadyToAnnounce,
            DoneFinally
        };

        enuProbingStatus                m_ProbingStatus;
        uint8_t                         m_u8SentCount;  // Used for probes and announcements
        esp8266::polledTimeout::oneShot m_Timeout;      // Used for probes and announcements
        bool                            m_bConflict;
        bool                            m_bTiebreakNeeded;

        clsProbeInformation_Base(void);

        bool clear(void);  // No 'virtual' needed, no polymorphic use (save 4 bytes)
    };

public:
    /**
        fnProbeResultCallback
        Callback function for host domain probe results
    */
    using fnProbeResultCallback = std::function<void(clsLEAMDNSHost& p_rHost,
                                  const char* p_pcDomainName,
                                  bool p_bProbeResult)>;

protected:
    /**
        clsProbeInformation
    */
    class clsProbeInformation : public clsProbeInformation_Base
    {
    public:
        fnProbeResultCallback   m_fnProbeResultCallback;

        clsProbeInformation(void);

        bool clear(bool p_bClearUserdata = false);
    };

public:
    /**
        clsService
    */
    struct clsService
    {
    public:
        /**
            fnDynamicServiceTxtCallback
        */
        using fnDynamicServiceTxtCallback = std::function<void(clsService& p_rMDNSService)>;

        /**
            fnProbeResultCallback
        */
        using fnProbeResultCallback = std::function<void(clsService& p_rMDNSService,
                                      const char* p_pcInstanceName,
                                      bool p_bProbeResult)>;

    protected:
        friend clsLEAMDNSHost;
        /**
            clsProbeInformation
        */
        class clsProbeInformation : public clsProbeInformation_Base
        {
        public:
            fnProbeResultCallback   m_fnProbeResultCallback;

            clsProbeInformation(void);

            bool clear(bool p_bClearUserdata = false);
        };

        char*                       m_pcInstanceName;
        bool                        m_bAutoName;		// Name was set automatically to hostname (if no name was supplied)
        char*                       m_pcType;
        char*                       m_pcProtocol;
        uint16_t                    m_u16Port;
        uint32_t                    m_u32ReplyMask;
        clsServiceTxts              m_Txts;
        fnDynamicServiceTxtCallback m_fnTxtCallback;
        clsProbeInformation         m_ProbeInformation;

        clsService(void);
        ~clsService(void);

        bool _releaseInstanceName(void);
        bool _releaseType(void);
        bool _releaseProtocol(void);

        void _resetProbeStatus(void);

        clsServiceTxt* _addServiceTxt(const char* p_pcKey,
                                      const char* p_pcValue,
                                      bool p_bTemp);
        clsServiceTxt* _addServiceTxt(const char* p_pcKey,
                                      uint32_t p_u32Value,
                                      bool p_bTemp);
        clsServiceTxt* _addServiceTxt(const char* p_pcKey,
                                      uint16_t p_u16Value,
                                      bool p_bTemp);
        clsServiceTxt* _addServiceTxt(const char* p_pcKey,
                                      uint8_t p_u8Value,
                                      bool p_bTemp);
        clsServiceTxt* _addServiceTxt(const char* p_pcKey,
                                      int32_t p_i32Value,
                                      bool p_bTemp);
        clsServiceTxt* _addServiceTxt(const char* p_pcKey,
                                      int16_t p_i16Value,
                                      bool p_bTemp);
        clsServiceTxt* _addServiceTxt(const char* p_pcKey,
                                      int8_t p_i8Value,
                                      bool p_bTemp);

    public:
        bool setInstanceName(const char* p_pcInstanceName);
        bool indexInstanceName(void);
        const char* instanceName(void) const;

        bool setType(const char* p_pcType);
        const char* type(void) const;

        bool setProtocol(const char* p_pcProtocol);
        const char* protocol(void) const;

        bool setPort(uint16_t p_i16Port);
        uint16_t port(void) const;

        bool setProbeResultCallback(fnProbeResultCallback p_fnProbeResultCallback);
        bool probeStatus(void) const;

        // TXT
        // Add a (static) MDNS TXT item ('key' = 'value') to the service
        clsServiceTxt* addServiceTxt(const char* p_pcKey,
                                     const char* p_pcValue);
        clsServiceTxt* addServiceTxt(const char* p_pcKey,
                                     uint32_t p_u32Value);
        clsServiceTxt* addServiceTxt(const char* p_pcKey,
                                     uint16_t p_u16Value);
        clsServiceTxt* addServiceTxt(const char* p_pcKey,
                                     uint8_t p_u8Value);
        clsServiceTxt* addServiceTxt(const char* p_pcKey,
                                     int32_t p_i32Value);
        clsServiceTxt* addServiceTxt(const char* p_pcKey,
                                     int16_t p_i16Value);
        clsServiceTxt* addServiceTxt(const char* p_pcKey,
                                     int8_t p_i8Value);

        bool removeServiceTxt(const char* p_pcKey);
        bool removeServiceTxt(clsServiceTxt* p_pTxt);
        const clsServiceTxt* findServiceTxt(const char* p_pcKey) const;
        clsServiceTxt* findServiceTxt(const char* p_pcKey);

        // Add a (dynamic) MDNS TXT item ('key' = 'value') to the service
        // Dynamic TXT items are removed right after one-time use. So they need to be added
        // every time the value s needed (via callback).
        clsServiceTxt* addDynamicServiceTxt(const char* p_pcKey,
                                            const char* p_pcValue);
        clsServiceTxt* addDynamicServiceTxt(const char* p_pcKey,
                                            uint32_t p_u32Value);
        clsServiceTxt* addDynamicServiceTxt(const char* p_pcKey,
                                            uint16_t p_u16Value);
        clsServiceTxt* addDynamicServiceTxt(const char* p_pcKey,
                                            uint8_t p_u8Value);
        clsServiceTxt* addDynamicServiceTxt(const char* p_pcKey,
                                            int32_t p_i32Value);
        clsServiceTxt* addDynamicServiceTxt(const char* p_pcKey,
                                            int16_t p_i16Value);
        clsServiceTxt* addDynamicServiceTxt(const char* p_pcKey,
                                            int8_t p_i8Value);

        bool setDynamicServiceTxtCallback(fnDynamicServiceTxtCallback p_fnCallback);

        /**
            list
        */
        using list = std::list<clsService*>;
    };
    using hMDNSService = clsService; // backward compatibility with LEAmDNS

protected:
    /**
        typeContentFlag & enuContentFlag
    */
    using typeContentFlag = uint16_t;
    enum class enuContentFlag : typeContentFlag
    {
        // Host
        A           = 0x0001,
        PTR_IPv4    = 0x0002,
        PTR_IPv6    = 0x0004,
        AAAA        = 0x0008,
        // Service
        PTR_TYPE    = 0x0010,
        PTR_NAME    = 0x0020,
        TXT         = 0x0040,
        SRV         = 0x0080,
        // DNSSEC
        NSEC        = 0x0100,

        PTR         = (PTR_IPv4 | PTR_IPv6 | PTR_TYPE | PTR_NAME)
    };

    /**
        clsMsgHeader
    */
    class clsMsgHeader
    {
    public:
        uint16_t        m_u16ID;            // Identifier
        bool            m_1bQR      : 1;    // Query/Response flag
        uint8_t         m_4bOpcode  : 4;    // Operation code
        bool            m_1bAA      : 1;    // Authoritative Answer flag
        bool            m_1bTC      : 1;    // Truncation flag
        bool            m_1bRD      : 1;    // Recursion desired
        bool            m_1bRA      : 1;    // Recursion available
        uint8_t         m_3bZ       : 3;    // Zero
        uint8_t         m_4bRCode   : 4;    // Response code
        uint16_t        m_u16QDCount;       // Question count
        uint16_t        m_u16ANCount;       // Answer count
        uint16_t        m_u16NSCount;       // Authority Record count
        uint16_t        m_u16ARCount;       // Additional Record count

        clsMsgHeader(uint16_t p_u16ID = 0,
                     bool p_bQR = false,
                     uint8_t p_u8Opcode = 0,
                     bool p_bAA = false,
                     bool p_bTC = false,
                     bool p_bRD = false,
                     bool p_bRA = false,
                     uint8_t p_u8RCode = 0,
                     uint16_t p_u16QDCount = 0,
                     uint16_t p_u16ANCount = 0,
                     uint16_t p_u16NSCount = 0,
                     uint16_t p_u16ARCount = 0);
    };

    /**
        clsRRDomain
    */
    class clsRRDomain
    {
    public:
        char            m_acName[clsConsts::stDomainMaxLength];    // Encoded domain name
        uint16_t        m_u16NameLength;                        // Length (incl. '\0')
        char*           m_pcDecodedName;

        clsRRDomain(void);
        clsRRDomain(const clsRRDomain& p_Other);
        ~clsRRDomain(void);

        clsRRDomain& operator=(const clsRRDomain& p_Other);

        bool clear(void);
        bool clearNameCache(void);

        bool addLabel(const char* p_pcLabel,
                      bool p_bPrependUnderline = false);

        bool compare(const clsRRDomain& p_Other) const;
        bool operator==(const clsRRDomain& p_Other) const;
        bool operator!=(const clsRRDomain& p_Other) const;
        bool operator>(const clsRRDomain& p_Other) const;

        size_t c_strLength(void) const;
        bool c_str(char* p_pcBuffer) const;
        const char* c_str(void) const;
    };

    /**
        clsRRAttributes
    */
    class clsRRAttributes
    {
    public:
        uint16_t            m_u16Type;      // Type
        uint16_t            m_u16Class;     // Class, nearly always 'IN'

        clsRRAttributes(uint16_t p_u16Type = 0,
                        uint16_t p_u16Class = 1 /*DNS_RRCLASS_IN Internet*/);
        clsRRAttributes(const clsRRAttributes& p_Other);

        clsRRAttributes& operator=(const clsRRAttributes& p_Other);
    };

    /**
        clsRRHeader
    */
    class clsRRHeader
    {
    public:
        clsRRDomain			m_Domain;
        clsRRAttributes    	m_Attributes;

        clsRRHeader(void);
        clsRRHeader(const clsRRHeader& p_Other);

        clsRRHeader& operator=(const clsRRHeader& p_Other);

        bool clear(void);
    };

    /**
        clsRRQuestion
    */
    struct clsRRQuestion
    {
        clsRRHeader        	m_Header;
        bool                m_bUnicast;		// Unicast reply requested

        /**
            list
        */
        using list = std::list<clsRRQuestion*>;

        clsRRQuestion(void);
    };

    /**
        clsNSECBitmap
    */
    class clsNSECBitmap
    {
    public:
        uint8_t m_au8BitmapData[6]; // 6 bytes data

        clsNSECBitmap(void);

        bool clear(void);
        uint16_t length(void) const;
        bool setBit(uint16_t p_u16Bit);
        bool getBit(uint16_t p_u16Bit) const;
    };

    /**
        typeAnswerType & enuAnswerType
    */
    using typeAnswerType = uint8_t;
    enum class enuAnswerType : typeAnswerType
    {
        A,
        PTR,
        TXT,
        AAAA,
        SRV,
        //NSEC,
        Generic
    };

    /**
        clsRRAnswer
    */
    struct clsRRAnswer
    {
        clsRRAnswer*   		m_pNext;
        const enuAnswerType	m_AnswerType;
        clsRRHeader    		m_Header;
        bool                m_bCacheFlush;	// Cache flush command bit
        uint32_t            m_u32TTL;       // Validity time in seconds

        virtual ~clsRRAnswer(void);

        enuAnswerType answerType(void) const;

        bool clear(void);

    protected:
        clsRRAnswer(enuAnswerType p_AnswerType,
                    const clsRRHeader& p_Header,
                    uint32_t p_u32TTL);
    };

#ifdef MDNS_IPV4_SUPPORT
    /**
        clsRRAnswerA
    */
    class clsRRAnswerA : public clsRRAnswer
    {
    public:
        IPAddress         	m_IPAddress;

        clsRRAnswerA(const clsRRHeader& p_Header,
                     uint32_t p_u32TTL);
        ~clsRRAnswerA(void);

        bool clear(void);
    };
#endif

    /**
        clsRRAnswerPTR
    */
    class clsRRAnswerPTR : public clsRRAnswer
    {
    public:
        clsRRDomain  		m_PTRDomain;

        clsRRAnswerPTR(const clsRRHeader& p_Header,
                       uint32_t p_u32TTL);
        ~clsRRAnswerPTR(void);

        bool clear(void);
    };

    /**
        clsRRAnswerTXT
    */
    class clsRRAnswerTXT : public clsRRAnswer
    {
    public:
        clsServiceTxts 		m_Txts;

        clsRRAnswerTXT(const clsRRHeader& p_Header,
                       uint32_t p_u32TTL);
        ~clsRRAnswerTXT(void);

        bool clear(void);
    };

#ifdef MDNS_IPV6_SUPPORT
    /**
        clsRRAnswerAAAA
    */
    class clsRRAnswerAAAA : public clsRRAnswer
    {
    public:
        IPAddress           m_IPAddress;

        clsRRAnswerAAAA(const clsRRHeader& p_Header,
                        uint32_t p_u32TTL);
        ~clsRRAnswerAAAA(void);

        bool clear(void);
    };
#endif

    /**
        clsRRAnswerSRV
    */
    class clsRRAnswerSRV : public clsRRAnswer
    {
    public:
        uint16_t            m_u16Priority;
        uint16_t            m_u16Weight;
        uint16_t            m_u16Port;
        clsRRDomain    		m_SRVDomain;

        clsRRAnswerSRV(const clsRRHeader& p_Header,
                       uint32_t p_u32TTL);
        ~clsRRAnswerSRV(void);

        bool clear(void);
    };

    /**
        clsRRAnswerGeneric
    */
    class clsRRAnswerGeneric : public clsRRAnswer
    {
    public:
        uint16_t            m_u16RDLength;  // Length of variable answer
        uint8_t*            m_pu8RDData;    // Offset of start of variable answer in packet

        clsRRAnswerGeneric(const clsRRHeader& p_Header,
                           uint32_t p_u32TTL);
        ~clsRRAnswerGeneric(void);

        bool clear(void);
    };


    /**
        clsSendParameter
    */
    class clsSendParameter
    {
    protected:
        /**
            clsDomainCacheItem
        */
        class clsDomainCacheItem
        {
        public:
            const void*             m_pHostNameOrService;   // Opaque id for host or service domain (pointer)
            bool                    m_bAdditionalData;      // Opaque flag for special info (service domain included)
            uint16_t                m_u16Offset;            // Offset in UDP output buffer

            /**
                list
            */
            using list = std::list<clsDomainCacheItem*>;

            clsDomainCacheItem(const void* p_pHostNameOrService,
                               bool p_bAdditionalData,
                               uint32_t p_u16Offset);
        };

    public:
        /**
            typeResponseType & enuResponseType
        */
        using typeResponseType = uint8_t;
        enum class enuResponseType : typeResponseType
        {
            None,
            Response,
            Unsolicited
        };

        uint16_t                    m_u16ID;                // Query ID (used only in lagacy queries)
        clsRRQuestion::list         m_RRQuestions;          // A list of queries
        uint32_t                    m_u32HostReplyMask;     // Flags for reply components/answers
        bool                        m_bLegacyDNSQuery;      // Flag: Legacy query
        enuResponseType             m_Response;             // Enum: Response to a query
        bool                        m_bAuthorative;         // Flag: Authorative (owner) response
        bool                        m_bCacheFlush;          // Flag: Clients should flush their caches
        bool                        m_bUnicast;             // Flag: Unicast response
        bool                        m_bUnannounce;          // Flag: Unannounce service

        // Temp content; created while processing _prepareMessage
        uint16_t                    m_u16Offset;            // Current offset in UDP write buffer (mainly for domain cache)
        clsDomainCacheItem::list    m_DomainCacheItems;     // Cached host and service domains

        clsSendParameter(void);
        ~clsSendParameter(void);

        bool clear(void);
        bool flushQuestions(void);
        bool flushDomainCache(void);
        bool flushTempContent(void);

        bool shiftOffset(uint16_t p_u16Shift);

        bool addDomainCacheItem(const void* p_pHostNameOrService,
                                bool p_bAdditionalData,
                                uint16_t p_u16Offset);
        uint16_t findCachedDomainOffset(const void* p_pHostNameOrService,
                                        bool p_bAdditionalData) const;
    };

public:
    // QUERIES & ANSWERS

    /**
        clsQuery
    */
    class clsQuery
    {
    public:
        /**
            clsAnswer
        */
        class clsAnswer
        {
        public:
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

            /**
                stcTTL
            */
            class clsTTL
            {
            public:
                /**
                    typeTimeoutLevel & enuTimeoutLevel
                */
                using typeTimeoutLevel = uint8_t;
                enum class enuTimeoutLevel : typeTimeoutLevel
                {
                    None        = 0,
                    Base        = 80,
                    Interval    = 5,
                    Final       = 100
                };

                uint32_t                        m_u32TTL;
                esp8266::polledTimeout::oneShot m_TTLTimeout;
                typeTimeoutLevel                m_TimeoutLevel;

                clsTTL(void);
                bool set(uint32_t p_u32TTL);

                bool flagged(void) const;
                bool restart(void);

                bool prepareDeletion(void);
                bool finalTimeoutLevel(void) const;

                unsigned long timeout(void) const;
            };

            /**
                clsIPAddressWithTTL
            */
            class clsIPAddressWithTTL
            {
            public:
                IPAddress       m_IPAddress;
                clsTTL          m_TTL;

                /**
                    list
                */
                using list = std::list<clsIPAddressWithTTL*>;

                clsIPAddressWithTTL(IPAddress p_IPAddress,
                                    uint32_t p_u32TTL = 0);
            };

            // The service domain is the first 'answer' (from PTR answer, using service and protocol) to be set
            // Defines the key for additional answer, like host domain, etc.
            clsRRDomain        	        m_ServiceDomain;    // 1. level answer (PTR), eg. MyESP._http._tcp.local
            clsTTL                      m_TTLServiceDomain;
            clsRRDomain        	        m_HostDomain;       // 2. level answer (SRV, using service domain), eg. esp8266.local
            uint16_t                    m_u16Port;          // 2. level answer (SRV, using service domain), eg. 5000
            clsTTL                      m_TTLHostDomainAndPort;
            clsServiceTxts              m_Txts;             // 2. level answer (TXT, using service domain), eg. c#=1
            clsTTL                      m_TTLTxts;
#ifdef MDNS_IPV4_SUPPORT
            clsIPAddressWithTTL::list   m_IPv4Addresses;    // 3. level answer (A, using host domain), eg. 123.456.789.012
#endif
#ifdef MDNS_IPV6_SUPPORT
            clsIPAddressWithTTL::list   m_IPv6Addresses;    // 3. level answer (AAAA, using host domain), eg. 1234::09
#endif
            typeQueryAnswerType         m_QueryAnswerFlags; // enuQueryAnswerType

            /**
                list
            */
            using list = std::list<clsAnswer*>;

            clsAnswer(void);
            ~clsAnswer(void);

            bool clear(void);

#ifdef MDNS_IPV4_SUPPORT
            bool releaseIPv4Addresses(void);
            bool addIPv4Address(clsIPAddressWithTTL* p_pIPAddress);
            bool removeIPv4Address(clsIPAddressWithTTL* p_pIPAddress);
            const clsIPAddressWithTTL* findIPv4Address(const IPAddress& p_IPAddress) const;
            clsIPAddressWithTTL* findIPv4Address(const IPAddress& p_IPAddress);
            uint32_t IPv4AddressCount(void) const;
            const clsIPAddressWithTTL* IPv4AddressAtIndex(uint32_t p_u32Index) const;
            clsIPAddressWithTTL* IPv4AddressAtIndex(uint32_t p_u32Index);
#endif
#ifdef MDNS_IPV6_SUPPORT
            bool releaseIPv6Addresses(void);
            bool addIPv6Address(clsIPAddressWithTTL* p_pIPAddress);
            bool removeIPv6Address(clsIPAddressWithTTL* p_pIPAddress);
            const clsIPAddressWithTTL* findIPv6Address(const IPAddress& p_IPAddress) const;
            clsIPAddressWithTTL* findIPv6Address(const IPAddress& p_IPAddress);
            uint32_t IPv6AddressCount(void) const;
            const clsIPAddressWithTTL* IPv6AddressAtIndex(uint32_t p_u32Index) const;
            clsIPAddressWithTTL* IPv6AddressAtIndex(uint32_t p_u32Index);
#endif
        };  // clsAnswer

        /**
            clsAnswerAccessor
        */
        class clsAnswerAccessor
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
            clsAnswerAccessor(const clsAnswer* p_pAnswer);
            ~clsAnswerAccessor(void);

            /**
                clsTxtKeyValueMap
            */
            using clsTxtKeyValueMap = std::map<const char*, const char*, stcCompareTxtKey>;
            /**
                clsIPAddressVector
            */
            using clsIPAddressVector = std::vector<IPAddress>;
            /**
                 vector
            */
            using vector = std::vector<clsAnswerAccessor>;

            bool serviceDomainAvailable(void) const;
            const char* serviceDomain(void) const;
            bool hostDomainAvailable(void) const;
            const char* hostDomain(void) const;
            bool hostPortAvailable(void) const;
            uint16_t hostPort(void) const;
#ifdef MDNS_IPV4_SUPPORT
            bool IPv4AddressAvailable(void) const;
            clsIPAddressVector IPv4Addresses(void) const;
#endif
#ifdef MDNS_IPV6_SUPPORT
            bool IPv6AddressAvailable(void) const;
            clsIPAddressVector IPv6Addresses(void) const;
#endif
            bool txtsAvailable(void) const;
            const char* txts(void) const;
            const clsTxtKeyValueMap& txtKeyValues(void) const;
            const char* txtValue(const char* p_pcKey) const;

            size_t printTo(Print& p_Print) const;

        protected:
            const clsAnswer*    m_pAnswer;
            clsTxtKeyValueMap   m_TxtKeyValueMap;
        };

        /**
            typeQueryType & enuQueryType
        */
        using   typeQueryType = uint8_t;
        enum class enuQueryType : typeQueryType
        {
            None,
            Service,
            Host
        };

        /**
            QueryCallbackAnswerFn
        */
        using QueryCallbackAnswerFn = std::function<void(const clsQuery& p_Query,
                                      const clsAnswer& p_Answer,
                                      clsAnswer::typeQueryAnswerType p_QueryAnswerTypeFlags, // flags for the updated answer item
                                      bool p_bSetContent)>;                                  // true: Answer component set, false: component deleted
        /**
            QueryCallbackAccessorFn
        */
        using QueryCallbackAccessorFn = std::function<void(const clsQuery& p_Query,
                                        const clsAnswerAccessor& p_AnswerAccessor,
                                        clsAnswer::typeQueryAnswerType p_QueryAnswerTypeFlags,   // flags for the updated answer item
                                        bool p_bSetContent)>;                                    // true: Answer component set, false: component deleted

    protected:
        friend  clsLEAMDNSHost;

        enuQueryType                    m_QueryType;
        clsRRDomain                		m_Domain;           // Type:Service -> _http._tcp.local; Type:Host -> esp8266.local
        QueryCallbackAnswerFn           m_fnCallbackAnswer;
        QueryCallbackAccessorFn         m_fnCallbackAccessor;
        bool                            m_bStaticQuery;
        uint8_t                         m_u8SentCount;
        esp8266::polledTimeout::oneShot m_ResendTimeout;
        bool                            m_bAwaitingAnswers;
        clsAnswer::list                 m_Answers;

        /**
            list
        */
        using list = std::list<clsQuery*>;

        clsQuery(const enuQueryType p_QueryType);
        ~clsQuery(void);

        bool clear(void);

        bool addAnswer(clsAnswer* p_pAnswer);
        bool removeAnswer(clsAnswer* p_pAnswer);

        clsAnswer* findAnswerForServiceDomain(const clsRRDomain& p_ServiceDomain);
        clsAnswer* findAnswerForHostDomain(const clsRRDomain& p_HostDomain);

    public:
        uint32_t answerCount(void) const;
        const clsAnswer* answer(uint32_t p_u32Index) const;
        uint32_t indexOfAnswer(const clsAnswer* p_pAnswer) const;

        clsAnswerAccessor::vector answerAccessors(void) const;
        clsAnswerAccessor answerAccessor(uint32 p_u32AnswerIndex) const;
    };

public:
    static const char* indexDomainName(const char* p_pcDomainName,
                                       const char* p_pcDivider = "-",
                                       const char* p_pcDefaultDomainName = 0);
    static bool setNetIfHostName(const char* p_pcHostName);

    clsLEAMDNSHost(void);
    ~clsLEAMDNSHost(void);

    // INIT
    // Create a MDNS host by setting the default hostname
    // Later call 'update()' in every 'loop' to run the process loop
    // (probing, announcing, responding, ...)
    // If no callback is given, the (maybe) already installed callback stays set
    bool begin(const char* p_pcHostName,
               fnProbeResultCallback p_fnCallback = 0);

    bool close(void);

    // HOST
    bool setHostName(const char* p_pcHostName);
    bool indexHostName(void);
    const char* hostName(void) const;

    bool setProbeResultCallback(fnProbeResultCallback p_fnCallback);

    // Returns 'true' is host domain probing is done
    bool probeStatus(void) const;

    // SERVICE
    bool setDefaultInstanceName(const char* p_pcInstanceName);
    const char* defaultInstanceName(void) const;

    clsService* addService(const char* p_pcInstanceName,
                           const char* p_pcServiceType,
                           const char* p_pcProtocol,
                           uint16_t p_u16Port,
                           clsService::fnProbeResultCallback p_fnCallback = 0);
    bool removeService(clsService* p_pMDNSService);

    const clsService* findService(const char* p_pcInstanceName,
                                  const char* p_pcType,
                                  const char* p_pcProtocol,
                                  uint16_t p_u16Port = (uint16_t)(-1)) const;
    clsService* findService(const char* p_pcInstanceName,
                            const char* p_pcType,
                            const char* p_pcProtocol,
                            uint16_t p_u16Port = (uint16_t)(-1));
    const clsService::list& services(void) const;

    // QUERIES

    // - STATIC
    // Perform a (static) service/host query. The function returns after p_u16Timeout milliseconds
    // The answers (the number of received answers is returned) can be retrieved by calling
    // - answerHostName (or hostname)
    // - answerIP (or IP)
    // - answerPort (or port)
    clsQuery::clsAnswerAccessor::vector queryService(const char* p_pcService,
            const char* p_pcProtocol,
            const uint16_t p_u16Timeout);
    clsQuery::clsAnswerAccessor::vector queryHost(const char* p_pcHostName,
            const uint16_t p_u16Timeout);
    bool removeQuery(void);
    bool hasQuery(void);
    clsQuery* getQuery(void);

    // - DYNAMIC
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
    
    /*
       install*Query() creates several queries on the interfaces.
       it no more returns a single query but a boolean until the API is adapted
    */
    /*clsQuery**/bool installServiceQuery(const char* p_pcServiceType,
                                  const char* p_pcProtocol,
                                  clsQuery::QueryCallbackAnswerFn p_fnCallbackAnswer);
    /*clsQuery**/bool installServiceQuery(const char* p_pcServiceType,
                                  const char* p_pcProtocol,
                                  clsQuery::QueryCallbackAccessorFn p_fnCallbackAccessor);
    /*clsQuery**/bool installHostQuery(const char* p_pcHostName,
                               clsQuery::QueryCallbackAnswerFn p_fnCallbackAnswer);
    /*clsQuery**/bool installHostQuery(const char* p_pcHostName,
                               clsQuery::QueryCallbackAccessorFn p_fnCallbackAccessor);
    // Remove a dynamic service query
    /*bool removeQuery(clsQuery* p_pQuery);*/

    // PROCESSING
    bool update(void);

    bool announce(bool p_bAnnounce = true,
                  bool p_bIncludeServices = true);
    bool announceService(clsService* p_pService,
                         bool p_bAnnounce = true);

    bool restart(void);

    clsService* enableArduino(uint16_t p_u16Port, bool p_bAuthUpload = false);

protected:
    // File: ..._Host
    UdpContext* _allocBackbone(void);
    bool _releaseBackbone(void);

    bool _joinMulticastGroups(void);
    bool _leaveMulticastGroups(void);

    // NETIF
    typeNetIfState _getNetIfState(void) const;
    bool _checkNetIfState(void);

    // PROCESSING
    bool _processUDPInput(void);

    // DOMAIN NAMES
    bool _allocDomainName(const char* p_pcNewDomainName,
                          char*& p_rpcDomainName);
    bool _releaseDomainName(char*& p_rpcDomainName);
    bool _allocHostName(const char* p_pcHostName);
    bool _releaseHostName(void);

    bool _allocDefaultInstanceName(const char* p_pcInstanceName);
    bool _releaseDefaultInstanceName(void);
    const char* _instanceName(const char* p_pcInstanceName,
                              bool p_bReturnZero = true) const;

    // SERVICE
    clsService* _allocService(const char* p_pcName,
                              const char* p_pcServiceType,
                              const char* p_pcProtocol,
                              uint16_t p_u16Port);
    bool _releaseService(clsService* p_pService);

    // SERVICE TXT
    clsServiceTxt* _allocServiceTxt(clsService* p_pService,
                                    const char* p_pcKey,
                                    const char* p_pcValue,
                                    bool p_bTemp);
    bool _releaseServiceTxt(clsService* p_pService,
                            clsServiceTxt* p_pTxt);
    clsServiceTxt* _updateServiceTxt(clsService* p_pService,
                                     clsServiceTxt* p_pTxt,
                                     const char* p_pcValue,
                                     bool p_bTemp);
    clsServiceTxt* _findServiceTxt(clsService* p_pService,
                                   const char* p_pcKey);
    clsServiceTxt* _addServiceTxt(clsService* p_pService,
                                  const char* p_pcKey,
                                  const char* p_pcValue,
                                  bool p_bTemp);
    clsServiceTxt* _answerKeyValue(const clsQuery p_pQuery,
                                   const uint32_t p_u32AnswerIndex);
    bool _collectServiceTxts(clsService& p_rService);
    bool _releaseTempServiceTxts(clsService& p_rService);


    // QUERIES
    clsQuery* _allocQuery(clsQuery::enuQueryType p_QueryType);
    bool _removeQuery(clsQuery* p_pQuery);
    bool _removeLegacyQuery(void);
    clsQuery* _findLegacyQuery(void);
    bool _releaseQueries(void);
    clsQuery* _findNextQueryByDomain(const clsRRDomain& p_Domain,
                                     const clsQuery::enuQueryType p_QueryType,
                                     const clsQuery* p_pPrevQuery);
    clsQuery* _installServiceQuery(netif* pNetIf,
                                   const char* p_pcService,
                                   const char* p_pcProtocol);
    clsQuery* _installDomainQuery(netif *pNetIf,
                                  clsRRDomain& p_Domain,
                                  clsQuery::enuQueryType p_QueryType);
    bool _hasQueriesWaitingForAnswers(void) const;
    bool _executeQueryCallback(const clsQuery& p_Query,
                               const clsQuery::clsAnswer& p_Answer,
                               clsQuery::clsAnswer::typeQueryAnswerType p_QueryAnswerTypeFlags,
                               bool p_SetContent);


    // File: ..._Host_Control
    // RECEIVING
    bool _parseMessage();
    bool _parseQuery(netif* pNetIf,
                     const clsMsgHeader& p_Header);

    bool _parseResponse(netif* pNetIf, const clsMsgHeader& p_Header);
    bool _processAnswers(netif* pNetIf, const clsRRAnswer* p_pPTRAnswers);
    bool _processPTRAnswer(const clsRRAnswerPTR* p_pPTRAnswer,
                           bool& p_rbFoundNewKeyAnswer);
    bool _processSRVAnswer(const clsRRAnswerSRV* p_pSRVAnswer,
                           bool& p_rbFoundNewKeyAnswer);
    bool _processTXTAnswer(const clsRRAnswerTXT* p_pTXTAnswer);
#ifdef MDNS_IPV4_SUPPORT
    bool _processAAnswer(const clsRRAnswerA* p_pAAnswer);
#endif
#ifdef MDNS_IPV6_SUPPORT
    bool _processAAAAAnswer(const clsRRAnswerAAAA* p_pAAAAAnswer);
#endif

    // PROBING
    bool _updateProbeStatus(netif* pNetIf);
    bool _resetProbeStatus(bool p_bRestart = true);
    bool _hasProbesWaitingForAnswers(void) const;
    bool _sendHostProbe(netif* pNetIf);
    bool _sendServiceProbe(netif* pNetIf, clsService& p_rService);
    bool _cancelProbingForHost(void);
    bool _cancelProbingForService(clsService& p_rService);
    bool _callHostProbeResultCallback(bool p_bResult);
    bool _callServiceProbeResultCallback(clsService& p_rService,
                                         bool p_bResult);

    // ANNOUNCE
    bool _announce(netif* pNetIf,
                   bool p_bAnnounce,
                   bool p_bIncludeServices);
    bool _announceService(netif* pNetIf,
                          clsService& p_pService,
                          bool p_bAnnounce = true);

    // QUERY CACHE
    bool _checkQueryCache(netif* pNetIf);

    uint32_t _replyMaskForHost(netif* pNetIf,
                               const clsRRHeader& p_RRHeader,
                               bool* p_pbFullNameMatch = 0) const;
    uint32_t _replyMaskForService(const clsRRHeader& p_RRHeader,
                                  clsService& p_rService,
                                  bool* p_pbFullNameMatch = 0);


    // File: ..._Host_Transfer
    // SENDING
    bool _sendMessage(netif* pNetIf, clsSendParameter& p_SendParameter);
    bool _sendMessage_Multicast(netif* pNetIf,
                                clsSendParameter& p_rSendParameter,
                                uint8_t p_IPProtocolTypes);
    bool _prepareMessage(netif* pNetIf, clsSendParameter& p_SendParameter);
    bool _addQueryRecord(clsSendParameter& p_rSendParameter,
                         const clsRRDomain& p_QueryDomain,
                         uint16_t p_u16QueryType);
    bool _sendQuery(netif* netif,
                    const clsQuery& p_Query,
                    clsQuery::clsAnswer::list* p_pKnownAnswers = 0);
    bool _sendQuery(netif* netif,
                    const clsRRDomain& p_QueryDomain,
                    uint16_t p_u16RecordType,
                    clsQuery::clsAnswer::list* p_pKnownAnswers = 0);

    IPAddress _getResponderIPAddress(netif* pNetIf,
                                     enuIPProtocolType p_IPProtocolType) const;

    // RESOURCE RECORD
    bool _readRRQuestion(clsRRQuestion& p_rQuestion);
    bool _readRRAnswer(clsRRAnswer*& p_rpAnswer);
#ifdef MDNS_IPV4_SUPPORT
    bool _readRRAnswerA(clsRRAnswerA& p_rRRAnswerA,
                        uint16_t p_u16RDLength);
#endif
    bool _readRRAnswerPTR(clsRRAnswerPTR& p_rRRAnswerPTR,
                          uint16_t p_u16RDLength);
    bool _readRRAnswerTXT(clsRRAnswerTXT& p_rRRAnswerTXT,
                          uint16_t p_u16RDLength);
#ifdef MDNS_IPV6_SUPPORT
    bool _readRRAnswerAAAA(clsRRAnswerAAAA& p_rRRAnswerAAAA,
                           uint16_t p_u16RDLength);
#endif
    bool _readRRAnswerSRV(clsRRAnswerSRV& p_rRRAnswerSRV,
                          uint16_t p_u16RDLength);
    bool _readRRAnswerGeneric(clsRRAnswerGeneric& p_rRRAnswerGeneric,
                              uint16_t p_u16RDLength);

    bool _readRRHeader(clsRRHeader& p_rHeader);
    bool _readRRDomain(clsRRDomain& p_rRRDomain);
    bool _readRRDomain_Loop(clsRRDomain& p_rRRDomain,
                            uint8_t p_u8Depth);
    bool _readRRAttributes(clsRRAttributes& p_rAttributes);

    // DOMAIN NAMES
    bool _buildDomainForHost(const char* p_pcHostName,
                             clsRRDomain& p_rHostDomain) const;
    bool _buildDomainForDNSSD(clsRRDomain& p_rDNSSDDomain) const;
    bool _buildDomainForService(const clsService& p_Service,
                                bool p_bIncludeName,
                                clsRRDomain& p_rServiceDomain) const;
    bool _buildDomainForService(const char* p_pcService,
                                const char* p_pcProtocol,
                                clsRRDomain& p_rServiceDomain) const;
#ifdef MDNS_IPV4_SUPPORT
    bool _buildDomainForReverseIPv4(IPAddress p_IPv4Address,
                                    clsRRDomain& p_rReverseIPv4Domain) const;
#endif
#ifdef MDNS_IPV6_SUPPORT
    bool _buildDomainForReverseIPv6(IPAddress p_IPv4Address,
                                    clsRRDomain& p_rReverseIPv6Domain) const;
#endif

    // UDP
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

    // READ/WRITE MDNS STRUCTS
    bool _readMDNSMsgHeader(clsMsgHeader& p_rMsgHeader);

    bool _write8(uint8_t p_u8Value,
                 clsSendParameter& p_rSendParameter);
    bool _write16(uint16_t p_u16Value,
                  clsSendParameter& p_rSendParameter);
    bool _write32(uint32_t p_u32Value,
                  clsSendParameter& p_rSendParameter);

    bool _writeMDNSMsgHeader(const clsMsgHeader& p_MsgHeader,
                             clsSendParameter& p_rSendParameter);
    bool _writeMDNSRRAttributes(const clsRRAttributes& p_Attributes,
                                clsSendParameter& p_rSendParameter);
    bool _writeMDNSRRDomain(const clsRRDomain& p_Domain,
                            clsSendParameter& p_rSendParameter);
    bool _writeMDNSHostDomain(const char* m_pcHostName,
                              bool p_bPrependRDLength,
                              uint16_t p_u16AdditionalLength,
                              clsSendParameter& p_rSendParameter);
    bool _writeMDNSServiceDomain(const clsService& p_Service,
                                 bool p_bIncludeName,
                                 bool p_bPrependRDLength,
                                 uint16_t p_u16AdditionalLength,
                                 clsSendParameter& p_rSendParameter);

    bool _writeMDNSQuestion(clsRRQuestion& p_Question,
                            clsSendParameter& p_rSendParameter);

#ifdef MDNS_IPV4_SUPPORT
    bool _writeMDNSAnswer_A(IPAddress p_IPAddress,
                            clsSendParameter& p_rSendParameter);
    bool _writeMDNSAnswer_PTR_IPv4(IPAddress p_IPAddress,
                                   clsSendParameter& p_rSendParameter);
#endif
    bool _writeMDNSAnswer_PTR_TYPE(clsService& p_rService,
                                   clsSendParameter& p_rSendParameter);
    bool _writeMDNSAnswer_PTR_NAME(clsService& p_rService,
                                   clsSendParameter& p_rSendParameter);
    bool _writeMDNSAnswer_TXT(clsService& p_rService,
                              clsSendParameter& p_rSendParameter);
#ifdef MDNS_IPV6_SUPPORT
    bool _writeMDNSAnswer_AAAA(IPAddress p_IPAddress,
                               clsSendParameter& p_rSendParameter);
    bool _writeMDNSAnswer_PTR_IPv6(IPAddress p_IPAddress,
                                   clsSendParameter& p_rSendParameter);
#endif
    bool _writeMDNSAnswer_SRV(clsService& p_rService,
                              clsSendParameter& p_rSendParameter);
    clsNSECBitmap* _createNSECBitmap(uint32_t p_u32NSECContent);
    bool _writeMDNSNSECBitmap(const clsNSECBitmap& p_NSECBitmap,
                              clsSendParameter& p_rSendParameter);
    bool _writeMDNSAnswer_NSEC(uint32_t p_u32NSECContent,
                               clsSendParameter& p_rSendParameter);
#ifdef MDNS_IPV4_SUPPORT
    bool _writeMDNSAnswer_NSEC_PTR_IPv4(IPAddress p_IPAddress,
                                        clsSendParameter& p_rSendParameter);
#endif
#ifdef MDNS_IPV6_SUPPORT
    bool _writeMDNSAnswer_NSEC_PTR_IPv6(IPAddress p_IPAddress,
                                        clsSendParameter& p_rSendParameter);
#endif
    bool _writeMDNSAnswer_NSEC(clsService& p_rService,
                               uint32_t p_u32NSECContent,
                               clsSendParameter& p_rSendParameter);


    // File: ..._Host_Debug
#if not defined ESP_8266_MDNS_INCLUDE || defined DEBUG_ESP_MDNS_RESPONDER
    const char* _DH(const clsService* p_pMDNSService = 0) const;
    const char* _service2String(const clsService* p_pMDNSService) const;

    bool _printRRDomain(const clsRRDomain& p_rRRDomain) const;
    bool _printRRAnswer(const clsRRAnswer& p_RRAnswer) const;
    const char* _RRType2Name(uint16_t p_u16RRType) const;
    const char* _RRClass2String(uint16_t p_u16RRClass,
                                bool p_bIsQuery) const;
    const char* _replyFlags2String(uint32_t p_u32ReplyFlags) const;
    const char* _NSECBitmap2String(const clsNSECBitmap* p_pNSECBitmap) const;
#endif


protected:
    UdpContext*                 m_pUDPContext;

    char*                       m_pcHostName;
    char*                       m_pcDefaultInstanceName;
    clsService::list            m_Services;
    clsQuery::list              m_Queries;
    clsProbeInformation         m_ProbeInformation;
};


}   // namespace MDNSImplementation


}   // namespace esp8266


#endif  // __LEAMDNS2HOST_H__



