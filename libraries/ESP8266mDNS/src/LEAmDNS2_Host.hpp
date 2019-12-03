/*
    LEAmDNS2_Host.hpp

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

/**
    clsHost & clsHostList
*/
class clsHost
{
public:

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

    /**
        stcServiceTxt
    */
    struct stcServiceTxt
    {
        stcServiceTxt*	m_pNext;
        char*           m_pcKey;
        char*           m_pcValue;
        bool            m_bTemp;

        stcServiceTxt(const char* p_pcKey = 0,
                      const char* p_pcValue = 0,
					  bool p_bTemp = false);
        stcServiceTxt(const stcServiceTxt& p_Other);
        ~stcServiceTxt(void);

        stcServiceTxt& operator=(const stcServiceTxt& p_Other);
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
    };

    /**
        stcServiceTxts
    */
    struct stcServiceTxts
    {
        stcServiceTxt*	m_pTxts;
        char*           m_pcCache;

        stcServiceTxts(void);
        stcServiceTxts(const stcServiceTxts& p_Other);
        ~stcServiceTxts(void);

        stcServiceTxts& operator=(const stcServiceTxts& p_Other);

        bool clear(void);
        bool clearCache(void);

        bool add(stcServiceTxt* p_pTxt);
        bool remove(stcServiceTxt* p_pTxt);

        bool removeTempTxts(void);

        stcServiceTxt* find(const char* p_pcKey);
        const stcServiceTxt* find(const char* p_pcKey) const;
        stcServiceTxt* find(const stcServiceTxt* p_pTxt);

        uint16_t length(void) const;

        size_t c_strLength(void) const;
        bool c_str(char* p_pcBuffer);
        const char* c_str(void) const;

        size_t bufferLength(void) const;
        bool buffer(char* p_pcBuffer);

        bool compare(const stcServiceTxts& p_Other) const;
        bool operator==(const stcServiceTxts& p_Other) const;
        bool operator!=(const stcServiceTxts& p_Other) const;
    };

    /**
        typeProbingStatus & enuProbingStatus
    */
    using typeProbingStatus = uint8_t;
    enum class enuProbingStatus : typeProbingStatus
    {
        WaitingForData,
        ReadyToStart,
        InProgress,
        Done
    };

    /**
        stcProbeInformation_Base
    */
    struct stcProbeInformation_Base
    {
        enuProbingStatus                m_ProbingStatus;
        uint8_t                         m_u8SentCount;  // Used for probes and announcements
        esp8266::polledTimeout::oneShot m_Timeout;      // Used for probes and announcements
        bool                            m_bConflict;
        bool                            m_bTiebreakNeeded;

        stcProbeInformation_Base(void);

        bool clear(void);  // No 'virtual' needed, no polymorphic use (save 4 bytes)
    };

    /**
        HostProbeResultCallbackFn
        Callback function for host domain probe results
    */
    using HostProbeResultCallbackFn = std::function<void(clsHost& p_rHost,
                                                         const char* p_pcDomainName,
                                                         bool p_bProbeResult)>;
    /**
        MDNSServiceProbeResultCallbackFn
        Callback function for service domain probe results
    */
    struct stcService;
    using ServiceProbeResultCallbackFn = std::function<void(clsHost& p_rHost,
                                                            stcService& p_rMDNSService,
                                                            const char* p_pcDomainName,
                                                            bool p_bProbeResult)>;

    /**
        stcProbeInformation_Host
    */
    struct stcProbeInformation_Host : public stcProbeInformation_Base
    {
        HostProbeResultCallbackFn		m_fnProbeResultCallback;

        stcProbeInformation_Host(void);

        bool clear(bool p_bClearUserdata = false);
    };

    /**
        stcProbeInformation_Service
    */
    struct stcProbeInformation_Service : public stcProbeInformation_Base
    {
        ServiceProbeResultCallbackFn    m_fnProbeResultCallback;

        stcProbeInformation_Service(void);

        bool clear(bool p_bClearUserdata = false);
    };

    /**
        DynamicServiceTxtCallbackFn
        Callback function for dynamic MDNS TXT items
    */
    struct stcService;
    using DynamicServiceTxtCallbackFn = std::function<void(clsHost& p_rHost,
                                                           stcService& p_rMDNSService)>;

    /**
        stcService
    */
    struct stcService
    {
        stcService*                	m_pNext;
        char*                       m_pcName;
        bool                        m_bAutoName;		// Name was set automatically to hostname (if no name was supplied)
        char*                       m_pcServiceType;
        char*                       m_pcProtocol;
        uint16_t                    m_u16Port;
        uint32_t                    m_u32ReplyMask;
        stcServiceTxts              m_Txts;
        DynamicServiceTxtCallbackFn m_fnTxtCallback;
        stcProbeInformation_Service m_ProbeInformation;

        stcService(const char* p_pcName = 0,
                   const char* p_pcServiceType = 0,
                   const char* p_pcProtocol = 0);
        ~stcService(void);

        bool setName(const char* p_pcName);
        bool releaseName(void);

        bool setServiceType(const char* p_pcService);
        bool releaseServiceType(void);

        bool setProtocol(const char* p_pcProtocol);
        bool releaseProtocol(void);

        bool probeStatus(void) const;
    };

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
        stcMsgHeader
    */
    struct stcMsgHeader
    {
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

        stcMsgHeader(uint16_t p_u16ID = 0,
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
        stcRRDomain
    */
    struct stcRRDomain
    {
        char            m_acName[MDNS_DOMAIN_MAXLENGTH];    // Encoded domain name
        uint16_t        m_u16NameLength;                    // Length (incl. '\0')
        char*           m_pcDecodedName;

        stcRRDomain(void);
        stcRRDomain(const stcRRDomain& p_Other);
        ~stcRRDomain(void);

        stcRRDomain& operator=(const stcRRDomain& p_Other);

        bool clear(void);
        bool clearNameCache(void);

        bool addLabel(const char* p_pcLabel,
                      bool p_bPrependUnderline = false);

        bool compare(const stcRRDomain& p_Other) const;
        bool operator==(const stcRRDomain& p_Other) const;
        bool operator!=(const stcRRDomain& p_Other) const;
        bool operator>(const stcRRDomain& p_Other) const;

        size_t c_strLength(void) const;
        bool c_str(char* p_pcBuffer) const;
        const char* c_str(void) const;
    };

    /**
        stcRRAttributes
    */
    struct stcRRAttributes
    {
        uint16_t            m_u16Type;      // Type
        uint16_t            m_u16Class;     // Class, nearly always 'IN'

        stcRRAttributes(uint16_t p_u16Type = 0,
                        uint16_t p_u16Class = 1 /*DNS_RRCLASS_IN Internet*/);
        stcRRAttributes(const stcRRAttributes& p_Other);

        stcRRAttributes& operator=(const stcRRAttributes& p_Other);
    };

    /**
        stcRRHeader
    */
    struct stcRRHeader
    {
        stcRRDomain			m_Domain;
        stcRRAttributes    	m_Attributes;

        stcRRHeader(void);
        stcRRHeader(const stcRRHeader& p_Other);

        stcRRHeader& operator=(const stcRRHeader& p_Other);

        bool clear(void);
    };

    /**
        stcRRQuestion
    */
    struct stcRRQuestion
    {
        stcRRQuestion*     	m_pNext;
        stcRRHeader        	m_Header;
        bool                m_bUnicast;		// Unicast reply requested

        stcRRQuestion(void);
    };

    /**
        stcNSECBitmap
    */
    struct stcNSECBitmap
    {
        uint8_t m_au8BitmapData[6]; // 6 bytes data

        stcNSECBitmap(void);

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
        stcRRAnswer
    */
    struct stcRRAnswer
    {
        stcRRAnswer*   		m_pNext;
        const enuAnswerType	m_AnswerType;
        stcRRHeader    		m_Header;
        bool                m_bCacheFlush;	// Cache flush command bit
        uint32_t            m_u32TTL;       // Validity time in seconds

        virtual ~stcRRAnswer(void);

        enuAnswerType answerType(void) const;

        bool clear(void);

    protected:
        stcRRAnswer(enuAnswerType p_AnswerType,
                    const stcRRHeader& p_Header,
					uint32_t p_u32TTL);
    };

#ifdef MDNS_IPV4_SUPPORT
    /**
        stcRRAnswerA
    */
    struct stcRRAnswerA : public stcRRAnswer
    {
        IPAddress         	m_IPAddress;

        stcRRAnswerA(const stcRRHeader& p_Header,
                     uint32_t p_u32TTL);
        ~stcRRAnswerA(void);

        bool clear(void);
    };
#endif

    /**
        stcRRAnswerPTR
    */
    struct stcRRAnswerPTR : public stcRRAnswer
    {
        stcRRDomain  		m_PTRDomain;

        stcRRAnswerPTR(const stcRRHeader& p_Header,
                       uint32_t p_u32TTL);
        ~stcRRAnswerPTR(void);

        bool clear(void);
    };

    /**
        stcRRAnswerTXT
    */
    struct stcRRAnswerTXT : public stcRRAnswer
    {
        stcServiceTxts 		m_Txts;

        stcRRAnswerTXT(const stcRRHeader& p_Header,
                       uint32_t p_u32TTL);
        ~stcRRAnswerTXT(void);

        bool clear(void);
    };

#ifdef MDNS_IPV6_SUPPORT
    /**
        stcRRAnswerAAAA
    */
    struct stcRRAnswerAAAA : public stcRRAnswer
    {
        IPAddress           m_IPAddress;

        stcRRAnswerAAAA(const stcRRHeader& p_Header,
                        uint32_t p_u32TTL);
        ~stcRRAnswerAAAA(void);

        bool clear(void);
    };
#endif

    /**
        stcRRAnswerSRV
    */
    struct stcRRAnswerSRV : public stcRRAnswer
    {
        uint16_t            m_u16Priority;
        uint16_t            m_u16Weight;
        uint16_t            m_u16Port;
        stcRRDomain    		m_SRVDomain;

        stcRRAnswerSRV(const stcRRHeader& p_Header,
                       uint32_t p_u32TTL);
        ~stcRRAnswerSRV(void);

        bool clear(void);
    };

    /**
        stcRRAnswerGeneric
    */
    struct stcRRAnswerGeneric : public stcRRAnswer
    {
        uint16_t            m_u16RDLength;  // Length of variable answer
        uint8_t*            m_pu8RDData;    // Offset of start of variable answer in packet

        stcRRAnswerGeneric(const stcRRHeader& p_Header,
                           uint32_t p_u32TTL);
        ~stcRRAnswerGeneric(void);

        bool clear(void);
    };


    /**
        stcSendParameter
    */
    struct stcSendParameter
    {
    protected:
        /**
            stcDomainCacheItem
        */
        struct stcDomainCacheItem
        {
            stcDomainCacheItem*     m_pNext;
            const void*             m_pHostNameOrService;   // Opaque id for host or service domain (pointer)
            bool                    m_bAdditionalData;      // Opaque flag for special info (service domain included)
            uint16_t                m_u16Offset;            // Offset in UDP output buffer

            stcDomainCacheItem(const void* p_pHostNameOrService,
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

        uint16_t                m_u16ID;                    // Query ID (used only in lagacy queries)
        stcRRQuestion*     		m_pQuestions;               // A list of queries
        uint32_t                m_u32HostReplyMask;         // Flags for reply components/answers
        bool                    m_bLegacyQuery;             // Flag: Legacy query
        enuResponseType         m_Response;                 // Enum: Response to a query
        bool                    m_bAuthorative;             // Flag: Authorative (owner) response
        bool                    m_bCacheFlush;              // Flag: Clients should flush their caches
        bool                    m_bUnicast;                 // Flag: Unicast response
        bool                    m_bUnannounce;              // Flag: Unannounce service

        // Temp content; created while processing _prepareMessage
        uint16_t                m_u16Offset;                // Current offset in UDP write buffer (mainly for domain cache)
        stcDomainCacheItem*     m_pDomainCacheItems;        // Cached host and service domains

        stcSendParameter(void);
        ~stcSendParameter(void);

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


    // QUERIES & ANSWERS
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
        stcQuery
    */
    struct stcQuery
    {
        /**
            stcAnswer
        */
        struct stcAnswer
        {
            /**
                stcTTL
            */
            struct stcTTL
            {
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

                stcTTL(void);
                bool set(uint32_t p_u32TTL);

                bool flagged(void) const;
                bool restart(void);

                bool prepareDeletion(void);
                bool finalTimeoutLevel(void) const;

                unsigned long timeout(void) const;
            };
            /**
                stcIPAddress
            */
            struct stcIPAddress
            {
                stcIPAddress*   m_pNext;
                IPAddress       m_IPAddress;
                stcTTL          m_TTL;

                stcIPAddress(IPAddress p_IPAddress,
                             uint32_t p_u32TTL = 0);
            };

            stcAnswer*        	m_pNext;
            // The service domain is the first 'answer' (from PTR answer, using service and protocol) to be set
            // Defines the key for additional answer, like host domain, etc.
            stcRRDomain        	m_ServiceDomain;    // 1. level answer (PTR), eg. MyESP._http._tcp.local
            stcTTL              m_TTLServiceDomain;
            stcRRDomain        	m_HostDomain;       // 2. level answer (SRV, using service domain), eg. esp8266.local
            uint16_t            m_u16Port;          // 2. level answer (SRV, using service domain), eg. 5000
            stcTTL              m_TTLHostDomainAndPort;
            stcServiceTxts      m_Txts;             // 2. level answer (TXT, using service domain), eg. c#=1
            stcTTL              m_TTLTxts;
#ifdef MDNS_IPV4_SUPPORT
            stcIPAddress*       m_pIPv4Addresses;   // 3. level answer (A, using host domain), eg. 123.456.789.012
#endif
#ifdef MDNS_IPV6_SUPPORT
            stcIPAddress*       m_pIPv6Addresses;   // 3. level answer (AAAA, using host domain), eg. 1234::09
#endif
            typeQueryAnswerType m_QueryAnswerFlags; // enuQueryAnswerType

            stcAnswer(void);
            ~stcAnswer(void);

            bool clear(void);

#ifdef MDNS_IPV4_SUPPORT
            bool releaseIPv4Addresses(void);
            bool addIPv4Address(stcIPAddress* p_pIPAddress);
            bool removeIPv4Address(stcIPAddress* p_pIPAddress);
            const stcIPAddress* findIPv4Address(const IPAddress& p_IPAddress) const;
            stcIPAddress* findIPv4Address(const IPAddress& p_IPAddress);
            uint32_t IPv4AddressCount(void) const;
            const stcIPAddress* IPv4AddressAtIndex(uint32_t p_u32Index) const;
            stcIPAddress* IPv4AddressAtIndex(uint32_t p_u32Index);
#endif
#ifdef MDNS_IPV6_SUPPORT
            bool releaseIPv6Addresses(void);
            bool addIPv6Address(stcIPAddress* p_pIPAddress);
            bool removeIPv6Address(stcIPAddress* p_pIPAddress);
            const stcIPAddress* findIPv6Address(const IPAddress& p_IPAddress) const;
            stcIPAddress* findIPv6Address(const IPAddress& p_IPAddress);
            uint32_t IPv6AddressCount(void) const;
            const stcIPAddress* IPv6AddressAtIndex(uint32_t p_u32Index) const;
            stcIPAddress* IPv6AddressAtIndex(uint32_t p_u32Index);
#endif
        };  //stcAnswer

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
        using _QueryCallbackFn = std::function<void(clsHost& p_rHost,
                                                    const stcQuery& p_Query,
                                                    const stcAnswer& p_Answer,
                                                    typeQueryAnswerType p_QueryAnswerTypeFlags, // flags for the updated answer item
                                                    bool p_bSetContent)>;                       // true: Answer component set, false: component deleted

        stcQuery*                   	m_pNext;
        enuQueryType                    m_QueryType;
        stcRRDomain                		m_Domain;		// Type:Service -> _http._tcp.local; Type:Host -> esp8266.local
        _QueryCallbackFn                m_fnCallback;
        bool                            m_bLegacyQuery;
        uint8_t                         m_u8SentCount;
        esp8266::polledTimeout::oneShot m_ResendTimeout;
        bool                            m_bAwaitingAnswers;
        stcAnswer*                      m_pAnswers;

        stcQuery(const enuQueryType p_QueryType);
        ~stcQuery(void);

        bool clear(void);

        uint32_t answerCount(void) const;
        const stcAnswer* answerAtIndex(uint32_t p_u32Index) const;
        stcAnswer* answerAtIndex(uint32_t p_u32Index);
        uint32_t indexOfAnswer(const stcAnswer* p_pAnswer) const;

        bool addAnswer(stcAnswer* p_pAnswer);
        bool removeAnswer(stcAnswer* p_pAnswer);

        stcAnswer* findAnswerForServiceDomain(const stcRRDomain& p_ServiceDomain);
        stcAnswer* findAnswerForHostDomain(const stcRRDomain& p_HostDomain);
    };
    /**
        QueryCallbackFn

        Callback function for received answers for dynamic queries
    */
    using QueryCallbackFn   = stcQuery::_QueryCallbackFn;

public:
    clsHost(netif& p_rNetIf,
            UdpContext& p_rUDPContext);
    ~clsHost(void);

    bool init(void);

    // HOST
    bool setHostName(const char* p_pcHostName);
    const char* hostName(void) const;

    bool setHostProbeResultCallback(HostProbeResultCallbackFn p_fnCallback);

    // Returns 'true' is host domain probing is done
    bool probeStatus(void) const;

    // SERVICE
    bool setInstanceName(const char* p_pcInstanceName);
    const char* instanceName(void) const;

    stcService* addService(const char* p_pcInstanceName,
                           const char* p_pcServiceType,
						   const char* p_pcProtocol,
						   uint16_t p_u16Port);
    bool removeService(stcService* p_pMDNSService);

    const stcService* findService(const char* p_pcInstanceName,
                                  const char* p_pcServiceType,
								  const char* p_pcProtocol,
								  uint16_t p_u16Port = 0) const;
    stcService* findService(const char* p_pcInstanceName,
                            const char* p_pcServiceType,
                            const char* p_pcProtocol,
                            uint16_t p_u16Port = 0);
    bool validateService(const stcService* p_pService) const;

    bool setServiceName(stcService* p_pMDNSService,
                        const char* p_pcInstanceName);
    const char* serviceName(const stcService* p_pMDNSService) const;
    const char* serviceType(const stcService* p_pMDNSService) const;
    const char* serviceProtocol(const stcService* p_pMDNSService) const;
    uint16_t servicePort(const stcService* p_pMDNSService) const;

    // Set a service specific probe result callcack
    bool setServiceProbeResultCallback(stcService* p_pMDNSService,
                                       ServiceProbeResultCallbackFn p_fnCallback);

    bool serviceProbeStatus(const stcService* p_pMDNSService) const;

    // SERVICE TXT
    // Add a (static) MDNS TXT item ('key' = 'value') to the service
    stcServiceTxt* addServiceTxt(stcService* p_pMDNSService,
                                 const char* p_pcKey,
								 const char* p_pcValue);
    bool removeServiceTxt(stcService* p_pMDNSService,
                          stcServiceTxt* p_pTxt);
    const stcServiceTxt* findServiceTxt(stcService* p_pMDNSService,
                                        const char* p_pcKey) const;
    stcServiceTxt* findServiceTxt(stcService* p_pMDNSService,
                                  const char* p_pcKey);

    bool setDynamicServiceTxtCallback(DynamicServiceTxtCallbackFn p_fnCallback);
    bool setDynamicServiceTxtCallback(stcService* p_pMDNSService,
                                      DynamicServiceTxtCallbackFn p_fnCallback);

    // Add a (dynamic) MDNS TXT item ('key' = 'value') to the service
    // Dynamic TXT items are removed right after one-time use. So they need to be added
    // every time the value s needed (via callback).
    stcServiceTxt* addDynamicServiceTxt(stcService* p_pMDNSService,
                                        const char* p_pcKey,
                                        const char* p_pcValue);

    // QUERIES

    // - STATIC
    // Perform a (static) service/host query. The function returns after p_u16Timeout milliseconds
    // The answers (the number of received answers is returned) can be retrieved by calling
    // - answerHostName (or hostname)
    // - answerIP (or IP)
    // - answerPort (or port)
    uint32_t queryService(const char* p_pcService,
                          const char* p_pcProtocol,
                          const uint16_t p_u16Timeout = MDNS_QUERYSERVICES_WAIT_TIME);
    uint32_t queryHost(const char* p_pcHostName,
                       const uint16_t p_u16Timeout = MDNS_QUERYSERVICES_WAIT_TIME);
    bool removeQuery(void);
    bool hasQuery(void);
    stcQuery* getQuery(void);

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
    stcQuery* installServiceQuery(const char* p_pcServiceType,
                                  const char* p_pcProtocol,
                                  QueryCallbackFn p_fnCallback);
    stcQuery* installHostQuery(const char* p_pcHostName,
                               QueryCallbackFn p_fnCallback);
    // Remove a dynamic service query
    bool removeQuery(stcQuery* p_pMDNSQuery);





    // PROCESSING
    bool processUDPInput(void);
    bool update(void);

    bool announce(bool p_bAnnounce,
                  bool p_bIncludeServices);
    bool announceService(stcService* p_pService,
                         bool p_bAnnounce = true);

    bool restart(void);

protected:
    // File: ..._Host
    bool _close(void);

    // NETIF
    typeNetIfState _getNetIfState(void) const;
    bool _checkNetIfState(void);

    // DOMAIN NAMES
    bool _allocDomainName(const char* p_pcNewDomainName,
                          char*& p_rpcDomainName);
    bool _releaseDomainName(char*& p_rpcDomainName);
    bool _allocHostName(const char* p_pcHostName);
    bool _releaseHostName(void);
    bool _allocInstanceName(const char* p_pcInstanceName);
    bool _releaseInstanceName(void);

    // SERVICE
    stcService* _allocService(const char* p_pcName,
                              const char* p_pcServiceType,
                              const char* p_pcProtocol,
                              uint16_t p_u16Port);
    bool _releaseService(stcService* p_pService);

    // SERVICE TXT
    stcServiceTxt* _allocServiceTxt(stcService* p_pService,
                                    const char* p_pcKey,
                                    const char* p_pcValue,
                                    bool p_bTemp);
    bool _releaseServiceTxt(stcService* p_pService,
                            stcServiceTxt* p_pTxt);
    stcServiceTxt* _updateServiceTxt(stcService* p_pService,
                                     stcServiceTxt* p_pTxt,
									 const char* p_pcValue,
									 bool p_bTemp);
    stcServiceTxt* _findServiceTxt(stcService* p_pService,
                                   const char* p_pcKey);
    stcServiceTxt* _addServiceTxt(stcService* p_pService,
                                  const char* p_pcKey,
								  const char* p_pcValue,
								  bool p_bTemp);
    stcServiceTxt* _answerKeyValue(const stcQuery p_pQuery,
                                   const uint32_t p_u32AnswerIndex);
    bool _collectServiceTxts(stcService& p_rService);
    bool _releaseTempServiceTxts(stcService& p_rService);

    // QUERIES
    stcQuery* _allocQuery(stcQuery::enuQueryType p_QueryType);
    bool _removeQuery(stcQuery* p_pQuery);
    bool _removeLegacyQuery(void);
    stcQuery* _findLegacyQuery(void);
    bool _releaseQueries(void);
    stcQuery* _findNextQueryByDomain(const stcRRDomain& p_Domain,
                                     const stcQuery::enuQueryType p_QueryType,
									 const stcQuery* p_pPrevQuery);
    stcQuery* _installDomainQuery(stcRRDomain& p_Domain,
                                  stcQuery::enuQueryType p_QueryType,
                                  QueryCallbackFn p_fnCallback);
    bool _hasQueriesWaitingForAnswers(void) const;
    bool _executeQueryCallback(const stcQuery& p_Query,
                               const stcQuery::stcAnswer& p_Answer,
                               typeQueryAnswerType p_QueryAnswerTypeFlags,
                               bool p_SetContent);


    // File: ..._Host_Control
    // RECEIVING
    bool _parseMessage(void);
    bool _parseQuery(const stcMsgHeader& p_Header);

    bool _parseResponse(const stcMsgHeader& p_Header);
    bool _processAnswers(const stcRRAnswer* p_pPTRAnswers);
    bool _processPTRAnswer(const stcRRAnswerPTR* p_pPTRAnswer,
                           bool& p_rbFoundNewKeyAnswer);
    bool _processSRVAnswer(const stcRRAnswerSRV* p_pSRVAnswer,
                           bool& p_rbFoundNewKeyAnswer);
    bool _processTXTAnswer(const stcRRAnswerTXT* p_pTXTAnswer);
#ifdef MDNS_IPV4_SUPPORT
    bool _processAAnswer(const stcRRAnswerA* p_pAAnswer);
#endif
#ifdef MDNS_IPV6_SUPPORT
    bool _processAAAAAnswer(const stcRRAnswerAAAA* p_pAAAAAnswer);
#endif

    // PROBING
    bool _updateProbeStatus(void);
    bool _resetProbeStatus(bool p_bRestart = true);
    bool _hasProbesWaitingForAnswers(void) const;
    bool _sendHostProbe(void);
    bool _sendServiceProbe(stcService& p_rService);
    bool _cancelProbingForHost(void);
    bool _cancelProbingForService(stcService& p_rService);
    bool _callHostProbeResultCallback(bool p_bResult);
    bool _callServiceProbeResultCallback(stcService& p_rService,
                                         bool p_bResult);

    // ANNOUNCE
    bool _announce(bool p_bAnnounce,
                   bool p_bIncludeServices);
    bool _announceService(stcService& p_pService,
                          bool p_bAnnounce = true);

    // QUERY CACHE
    bool _checkQueryCache(void);

    uint32_t _replyMaskForHost(const stcRRHeader& p_RRHeader,
                               bool* p_pbFullNameMatch = 0) const;
    uint32_t _replyMaskForService(const stcRRHeader& p_RRHeader,
                                  const stcService& p_Service,
                                  bool* p_pbFullNameMatch = 0) const;


    // File: ..._Host_Transfer
    // SENDING
    bool _sendMDNSMessage(stcSendParameter& p_SendParameter);
    bool _sendMDNSMessage_Multicast(stcSendParameter& p_rSendParameter,
                                    uint8_t p_IPProtocolTypes);
    bool _prepareMDNSMessage(stcSendParameter& p_SendParameter);
    bool _addMDNSQueryRecord(stcSendParameter& p_rSendParameter,
                             const stcRRDomain& p_QueryDomain,
                             uint16_t p_u16QueryType);
    bool _sendMDNSQuery(const stcQuery& p_Query,
                        stcQuery::stcAnswer* p_pKnownAnswers = 0);
    bool _sendMDNSQuery(const stcRRDomain& p_QueryDomain,
                        uint16_t p_u16RecordType,
                        stcQuery::stcAnswer* p_pKnownAnswers = 0);

    IPAddress _getResponderIPAddress(enuIPProtocolType p_IPProtocolType) const;

    // RESOURCE RECORD
    bool _readRRQuestion(stcRRQuestion& p_rQuestion);
    bool _readRRAnswer(stcRRAnswer*& p_rpAnswer);
#ifdef MDNS_IPV4_SUPPORT
    bool _readRRAnswerA(stcRRAnswerA& p_rRRAnswerA,
                        uint16_t p_u16RDLength);
#endif
    bool _readRRAnswerPTR(stcRRAnswerPTR& p_rRRAnswerPTR,
                          uint16_t p_u16RDLength);
    bool _readRRAnswerTXT(stcRRAnswerTXT& p_rRRAnswerTXT,
                          uint16_t p_u16RDLength);
#ifdef MDNS_IPV6_SUPPORT
    bool _readRRAnswerAAAA(stcRRAnswerAAAA& p_rRRAnswerAAAA,
                           uint16_t p_u16RDLength);
#endif
    bool _readRRAnswerSRV(stcRRAnswerSRV& p_rRRAnswerSRV,
                          uint16_t p_u16RDLength);
    bool _readRRAnswerGeneric(stcRRAnswerGeneric& p_rRRAnswerGeneric,
                              uint16_t p_u16RDLength);

    bool _readRRHeader(stcRRHeader& p_rHeader);
    bool _readRRDomain(stcRRDomain& p_rRRDomain);
    bool _readRRDomain_Loop(stcRRDomain& p_rRRDomain,
                            uint8_t p_u8Depth);
    bool _readRRAttributes(stcRRAttributes& p_rAttributes);

    // DOMAIN NAMES
    bool _buildDomainForHost(const char* p_pcHostName,
                             stcRRDomain& p_rHostDomain) const;
    bool _buildDomainForDNSSD(stcRRDomain& p_rDNSSDDomain) const;
    bool _buildDomainForService(const stcService& p_Service,
                                bool p_bIncludeName,
                                stcRRDomain& p_rServiceDomain) const;
    bool _buildDomainForService(const char* p_pcService,
                                const char* p_pcProtocol,
                                stcRRDomain& p_rServiceDomain) const;
#ifdef MDNS_IPV4_SUPPORT
    bool _buildDomainForReverseIPv4(IPAddress p_IPv4Address,
                                    stcRRDomain& p_rReverseIPv4Domain) const;
#endif
#ifdef MDNS_IPV6_SUPPORT
    bool _buildDomainForReverseIPv6(IPAddress p_IPv4Address,
                                    stcRRDomain& p_rReverseIPv6Domain) const;
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
    bool _readMDNSMsgHeader(stcMsgHeader& p_rMsgHeader);

    bool _write8(uint8_t p_u8Value,
                 stcSendParameter& p_rSendParameter);
    bool _write16(uint16_t p_u16Value,
                  stcSendParameter& p_rSendParameter);
    bool _write32(uint32_t p_u32Value,
                  stcSendParameter& p_rSendParameter);

    bool _writeMDNSMsgHeader(const stcMsgHeader& p_MsgHeader,
                             stcSendParameter& p_rSendParameter);
    bool _writeMDNSRRAttributes(const stcRRAttributes& p_Attributes,
                                stcSendParameter& p_rSendParameter);
    bool _writeMDNSRRDomain(const stcRRDomain& p_Domain,
                            stcSendParameter& p_rSendParameter);
    bool _writeMDNSHostDomain(const char* m_pcHostName,
                              bool p_bPrependRDLength,
                              uint16_t p_u16AdditionalLength,
                              stcSendParameter& p_rSendParameter);
    bool _writeMDNSServiceDomain(const stcService& p_Service,
                                 bool p_bIncludeName,
                                 bool p_bPrependRDLength,
                                 uint16_t p_u16AdditionalLength,
                                 stcSendParameter& p_rSendParameter);

    bool _writeMDNSQuestion(stcRRQuestion& p_Question,
                            stcSendParameter& p_rSendParameter);

#ifdef MDNS_IPV4_SUPPORT
    bool _writeMDNSAnswer_A(IPAddress p_IPAddress,
                            stcSendParameter& p_rSendParameter);
    bool _writeMDNSAnswer_PTR_IPv4(IPAddress p_IPAddress,
                                   stcSendParameter& p_rSendParameter);
#endif
    bool _writeMDNSAnswer_PTR_TYPE(stcService& p_rService,
                                   stcSendParameter& p_rSendParameter);
    bool _writeMDNSAnswer_PTR_NAME(stcService& p_rService,
                                   stcSendParameter& p_rSendParameter);
    bool _writeMDNSAnswer_TXT(stcService& p_rService,
                              stcSendParameter& p_rSendParameter);
#ifdef MDNS_IPV6_SUPPORT
    bool _writeMDNSAnswer_AAAA(IPAddress p_IPAddress,
                               stcSendParameter& p_rSendParameter);
    bool _writeMDNSAnswer_PTR_IPv6(IPAddress p_IPAddress,
                                   stcSendParameter& p_rSendParameter);
#endif
    bool _writeMDNSAnswer_SRV(stcService& p_rService,
                              stcSendParameter& p_rSendParameter);
    stcNSECBitmap* _createNSECBitmap(uint32_t p_u32NSECContent);
    bool _writeMDNSNSECBitmap(const stcNSECBitmap& p_NSECBitmap,
                              stcSendParameter& p_rSendParameter);
    bool _writeMDNSAnswer_NSEC(uint32_t p_u32NSECContent,
                               stcSendParameter& p_rSendParameter);
#ifdef MDNS_IPV4_SUPPORT
    bool _writeMDNSAnswer_NSEC_PTR_IPv4(IPAddress p_IPAddress,
                                        stcSendParameter& p_rSendParameter);
#endif
#ifdef MDNS_IPV6_SUPPORT
    bool _writeMDNSAnswer_NSEC_PTR_IPv6(IPAddress p_IPAddress,
                                        stcSendParameter& p_rSendParameter);
#endif
    bool _writeMDNSAnswer_NSEC(stcService& p_rService,
                               uint32_t p_u32NSECContent,
                               stcSendParameter& p_rSendParameter);


    // File: ..._Host_Debug
#if not defined ESP_8266_MDNS_INCLUDE || defined DEBUG_ESP_MDNS_RESPONDER
    const char* _DH(const stcService* p_pMDNSService = 0) const;
    const char* _service2String(const stcService* p_pMDNSService) const;

    bool _printRRDomain(const stcRRDomain& p_rRRDomain) const;
    bool _printRRAnswer(const stcRRAnswer& p_RRAnswer) const;
    const char* _RRType2Name(uint16_t p_u16RRType) const;
    const char* _RRClass2String(uint16_t p_u16RRClass,
                                bool p_bIsQuery) const;
    const char* _replyFlags2String(uint32_t p_u32ReplyFlags) const;
    const char* _NSECBitmap2String(const stcNSECBitmap* p_pNSECBitmap) const;
#endif


public:
    netif&                      m_rNetIf;
    typeNetIfState              m_NetIfState;
    UdpContext&                 m_rUDPContext;

    char*                       m_pcHostName;
    char*                       m_pcInstanceName;
    stcService*                 m_pServices;
    stcQuery*                   m_pQueries;
    DynamicServiceTxtCallbackFn m_fnServiceTxtCallback;
    stcProbeInformation_Host    m_HostProbeInformation;
};
using clsHostList = std::list<clsHost*>;

