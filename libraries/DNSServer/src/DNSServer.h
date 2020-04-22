#ifndef DNSServer_h
#define DNSServer_h
#include <WiFiUdp.h>

// #define DEBUG_DNSSERVER

// https://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.txt
#ifndef IANA_DNS_PORT
#define IANA_DNS_PORT 53        // AKA domain
constexpr uint16_t kIanaDnsPort = 53;
#endif

#define DNS_QR_QUERY 0
#define DNS_QR_RESPONSE 1
#define DNS_OPCODE_QUERY 0

#define DNS_QCLASS_IN 1
#define DNS_QCLASS_ANY 255

#define DNS_QTYPE_A 1
#define DNS_QTYPE_ANY 255

#define MAX_DNSNAME_LENGTH 253
#define MAX_DNS_PACKETSIZE 512

enum class DNSReplyCode
{
  NoError = 0,
  FormError = 1,
  ServerFailure = 2,
  NonExistentDomain = 3,
  NotImplemented = 4,
  Refused = 5,
  YXDomain = 6,
  YXRRSet = 7,
  NXRRSet = 8
};

struct DNSHeader
{
  uint16_t ID;               // identification number
  unsigned char RD : 1;      // recursion desired
  unsigned char TC : 1;      // truncated message
  unsigned char AA : 1;      // authoritive answer
  unsigned char OPCode : 4;  // message_type
  unsigned char QR : 1;      // query/response flag
  unsigned char RCode : 4;   // response code
  unsigned char Z : 3;       // its z! reserved
  unsigned char RA : 1;      // recursion available
  uint16_t QDCount;          // number of question entries
  uint16_t ANCount;          // number of answer entries
  uint16_t NSCount;          // number of authority entries
  uint16_t ARCount;          // number of resource entries
};

constexpr size_t kDNSSQueSizeAddrBits = 3; // The number of bits used to address que entries
constexpr size_t kDNSSQueSize = BIT(kDNSSQueSizeAddrBits);

typedef struct DNSS_REQUESTER {
  uint32_t ip;
  uint16_t port;
  uint16_t id;
} dnss_requester_t;

class DNSServer
{
  public:
    DNSServer();
    ~DNSServer() {
        stop();
    };
    /*
      If specified, `enableForwarder` will update the `domainName` that is used
      to match DNS request to this AP's IP Address. A non-matching request will
      be forwarded to the DNS server specified by `dns`.

      Returns `true` on success.

      Returns `false`,
        * when forwarding `dns` is not set, or
        * unable to allocate resources for managing the DNS forward function.
    */
    bool enableForwarder(const String &domainName = emptyString, const IPAddress &dns = (uint32_t)0);
    /*
      `disableForwarder` will stop forwarding DNS requests. If specified,
      updates the `domainName` that is matched for returning this AP's IP Address.
      Optionally, resources used for the DNS forward function can be freed.
    */
    void disableForwarder(const String &domainName = emptyString, bool freeResources = false);
    bool isForwarding() { return _forwarder && _dns.isSet(); }
    void setDNS(const IPAddress& dns) { _dns = dns; }
    IPAddress getDNS() { return _dns; }
    bool isDNSSet() { return _dns.isSet(); }

    void processNextRequest();
    void setErrorReplyCode(const DNSReplyCode &replyCode);
    void setTTL(const uint32_t &ttl);
    uint32_t getTTL();
    String getDomainName() { return _domainName; }

    // Returns true if successful, false if there are no sockets available
    bool start(const uint16_t &port,
              const String &domainName,
              const IPAddress &resolvedIP,
              const IPAddress &dns = (uint32_t)0);
    // stops the DNS server
    void stop();

  private:
    WiFiUDP _udp;
    String _domainName;
    IPAddress _dns;
    std::unique_ptr<dnss_requester_t[]> _que;
    uint32_t _ttl;
#ifdef DEBUG_DNSSERVER
    // There are 2 possiblities for OverFlow:
    //   1) we have more than kDNSSQueSize request already outstanding.
    //   2) we have request that never received a reply.
    uint32_t _que_ov;
    uint32_t _que_drop;
#endif
    DNSReplyCode _errorReplyCode;
    bool _forwarder;
    unsigned char _resolvedIP[4];
    uint16_t _port;

    void downcaseAndRemoveWwwPrefix(String &domainName);
    void replyWithIP(DNSHeader *dnsHeader,
		     unsigned char * query,
		     size_t queryLength);
    void replyWithError(DNSHeader *dnsHeader,
			DNSReplyCode rcode,
			unsigned char *query,
			size_t queryLength);
    void replyWithError(DNSHeader *dnsHeader,
			DNSReplyCode rcode);
    bool respondToRequest(uint8_t *buffer, size_t length);
    void forwardRequest(uint8_t *buffer, size_t length);
    void forwardReply(uint8_t *buffer, size_t length);
    void writeNBOShort(uint16_t value);
};
#endif
