#include <ESP8266WiFi.h>
#include "DNSServer.h"
#include <lwip/def.h>
#include <Arduino.h>

extern struct rst_info resetInfo;

#ifdef DEBUG_ESP_PORT
#define CONSOLE DEBUG_ESP_PORT
#else
#define CONSOLE Serial
#endif

#define _ETS_PRINTF(a, ...) ets_uart_printf(a, ##__VA_ARGS__)
#define _ETS_PRINTFNL(a, ...) ets_uart_printf(a "\n", ##__VA_ARGS__)
#define _PRINTF(a, ...) printf_P(PSTR(a), ##__VA_ARGS__)
#define _PRINT(a)       print(String(F(a)))
#define _PRINTLN(a)     println(String(F(a)))
#define _PRINTLN2(a, b) println(String(F(a)) + b )

#define ETS_PRINTF       _ETS_PRINTF
#define ETS_PRINTFNL     _ETS_PRINTFNL
#define CONSOLE_PRINTF   CONSOLE._PRINTF
#define CONSOLE_PRINT    CONSOLE._PRINT
#define CONSOLE_PRINTLN  CONSOLE._PRINTLN
#define CONSOLE_PRINTLN2 CONSOLE._PRINTLN2


#ifdef DEBUG_DNSSERVER
#define DEBUG_PRINTF    CONSOLE_PRINTF
#define DEBUG_PRINT     CONSOLE_PRINT
#define DEBUG_PRINTLN   CONSOLE_PRINTLN
#define DEBUG_PRINTLN2  CONSOLE_PRINTLN2
#define DBGLOG_FAIL     LOG_FAIL

#define DEBUG_(...) do { (__VA_ARGS__); } while(false)
#define DEBUG__(...) __VA_ARGS__
#define LOG_FAIL(a, fmt, ...) do { if (!(a)) { CONSOLE.printf_P( PSTR(fmt " line: %d, function: %s\r\n"), ##__VA_ARGS__, __LINE__, __FUNCTION__ ); } } while(false);

#else
#define DEBUG_PRINTF(...) do { } while(false)
#define DEBUG_PRINT(...) do { } while(false)
#define DEBUG_PRINTLN(...) do { } while(false)
#define DEBUG_PRINTLN2(...) do { } while(false)
#define DEBUG_(...) do { } while(false)
#define DEBUG__(...) do { } while(false)
#define LOG_FAIL(a, ...) do { a; } while(false)
#define DBGLOG_FAIL(...) do { } while(false)
#endif

#define DNS_HEADER_SIZE sizeof(DNSHeader)

// Want to keep IDs unique across restarts and continquious
static uint32_t _ids __attribute__((section(".noinit")));

DNSServer::DNSServer()
{
  // I have observed that using 0 for captive and non-zero (600) when
  // forwarding, will help Android devices recognize the change in connectivity.
  // They will then report connected.
  _ttl = lwip_htonl(60);

#if !CORE_MOCK

  if (REASON_DEFAULT_RST      == resetInfo.reason ||
      REASON_DEEP_SLEEP_AWAKE <= resetInfo.reason) {
    _ids = random(0, BIT(16) - 1);
  }

#endif // !CORE_MOCK

  _ids += kDNSSQueSize;   // for the case of restart, ignore any inflight responses

  _errorReplyCode = DNSReplyCode::NonExistentDomain;
}

void DNSServer::disableForwarder(const String &domainName, bool freeResources)
{
  _forwarder = false;
  if (!domainName.isEmpty()) {
    _domainName = domainName;
    downcaseAndRemoveWwwPrefix(_domainName);
  }
  if (freeResources) {
    _dns = (uint32_t)0;
    if (_que) {
      _que = nullptr;
      DEBUG_PRINTF("from stop, deleted _que\r\n");
      DEBUG_(({
        if (_que_ov) {
          DEBUG_PRINTLN2("DNS forwarder que overflow or no reply to request: ", (_que_ov));
        }
        if (_que_drop) {
          DEBUG_PRINTLN2("DNS forwarder que wrapped, reply dropped: ", (_que_drop));
        }
      }));
    }
  }
}

bool DNSServer::enableForwarder(const String &domainName, const IPAddress &dns)
{
  disableForwarder(domainName, false); // Just happens to have the same logic needed here.

  if (dns.isSet()) {
    _dns = dns;
  }

  if (_dns.isSet()) {
    if (!_que) {
      _que = std::unique_ptr<DNSS_REQUESTER[]> (new (std::nothrow) DNSS_REQUESTER[kDNSSQueSize]);
      DEBUG_PRINTF("Created new _que\r\n");
      if (_que) {
        for (size_t i = 0; i < kDNSSQueSize; i++) {
          _que[i].ip = 0;
        }
        DEBUG_((_que_ov = 0));
        DEBUG_((_que_drop = 0));
      }
    }
    if (_que) {
      _forwarder = true;
    }
  }
  return _forwarder;
}

bool DNSServer::start(const uint16_t &port, const String &domainName,
                     const IPAddress &resolvedIP, const IPAddress &dns)
{
  _port = (port) ? port : IANA_DNS_PORT;

  _resolvedIP[0] = resolvedIP[0];
  _resolvedIP[1] = resolvedIP[1];
  _resolvedIP[2] = resolvedIP[2];
  _resolvedIP[3] = resolvedIP[3];

  if (!enableForwarder(domainName, dns) && (dns.isSet() || _dns.isSet())) {
    return false;
  }

  return _udp.begin(_port) == 1;
}

void DNSServer::setErrorReplyCode(const DNSReplyCode &replyCode)
{
  _errorReplyCode = replyCode;
}

void DNSServer::setTTL(const uint32_t &ttl)
{
  _ttl = lwip_htonl(ttl);
}

uint32_t DNSServer::getTTL()
{
  return lwip_ntohl(_ttl);
}

void DNSServer::stop()
{
  _udp.stop();
  disableForwarder(emptyString, true);
}

void DNSServer::downcaseAndRemoveWwwPrefix(String &domainName)
{
  domainName.toLowerCase();
  if (domainName.startsWith("www."))
      domainName.remove(0, 4);
}

void DNSServer::forwardReply(uint8_t *buffer, size_t length)
{
  if (!_forwarder || !_que) {
    return;
  }
  DNSHeader *dnsHeader = (DNSHeader *)buffer;
  uint16_t id = dnsHeader->ID;
  // if (kDNSSQueSize <= (uint16_t)((uint16_t)_ids - id)) {
  if ((uint16_t)kDNSSQueSize <= (uint16_t)_ids - id) {
    DEBUG_((++_que_drop));
    DEBUG_PRINTLN2("Forward reply ID: 0x", (String(id, HEX) + F(" dropped!")));
    return;
  }
  size_t i = id & (kDNSSQueSize - 1);

  // Drop duplicate packets
  if (0 == _que[i].ip) {
    DEBUG_PRINTLN2("Duplicate reply dropped ID: 0x", String(id, HEX));
    return;
  }
  dnsHeader->ID = _que[i].id;
  _udp.beginPacket(_que[i].ip, _que[i].port);
  _udp.write(buffer, length);
  _udp.endPacket();
  DEBUG_PRINTLN2("Forward reply ID: 0x", (String(id, HEX) + F(" to ") + IPAddress(_que[i].ip).toString()));
  _que[i].ip = 0; // This gets used to detect duplicate packets and overflow
}

void DNSServer::forwardRequest(uint8_t *buffer, size_t length)
{
  if (!_forwarder || !_dns.isSet() || !_que) {
    return;
  }
  DNSHeader *dnsHeader = (DNSHeader *)buffer;
  ++_ids;
  size_t i = _ids & (kDNSSQueSize - 1);
  DEBUG_(({
    if (0  != _que[i].ip) {
      ++_que_ov;
    }
  }));
  _que[i].ip = _udp.remoteIP();
  _que[i].port = _udp.remotePort();
  _que[i].id = dnsHeader->ID;
  dnsHeader->ID = (uint16_t)_ids;
  _udp.beginPacket(_dns, IANA_DNS_PORT);
  _udp.write(buffer, length);
  _udp.endPacket();
  DEBUG_PRINTLN2("Forward request ID: 0x", (String(dnsHeader->ID, HEX) + F(" to ") + _dns.toString()));
}

bool DNSServer::respondToRequest(uint8_t *buffer, size_t length)
{
  DNSHeader *dnsHeader;
  uint8_t *query, *start;
  const char *matchString;
  size_t remaining, labelLength, queryLength;
  uint16_t qtype, qclass;

  dnsHeader = (DNSHeader *)buffer;

  // Must be a query for us to do anything with it
  if (dnsHeader->QR != DNS_QR_QUERY) {
    return false;
  }

  // If operation is anything other than query, we don't do it
  if (dnsHeader->OPCode != DNS_OPCODE_QUERY) {
    replyWithError(dnsHeader, DNSReplyCode::NotImplemented);
    return false;
  }

  // Only support requests containing single queries - everything else
  // is badly defined
  if (dnsHeader->QDCount != lwip_htons(1)) {
    replyWithError(dnsHeader, DNSReplyCode::FormError);
    return false;
  }

  // We must return a FormError in the case of a non-zero ARCount to
  // be minimally compatible with EDNS resolvers
  if (dnsHeader->ANCount != 0 || dnsHeader->NSCount != 0
      || dnsHeader->ARCount != 0) {
    replyWithError(dnsHeader, DNSReplyCode::FormError);
    return false;
  }

  // Even if we're not going to use the query, we need to parse it
  // so we can check the address type that's being queried

  query = start = buffer + DNS_HEADER_SIZE;
  remaining = length - DNS_HEADER_SIZE;
  while (remaining != 0 && *start != 0) {
    labelLength = *start;
    if (labelLength + 1 > remaining) {
      replyWithError(dnsHeader, DNSReplyCode::FormError);
      return false;
    }
    remaining -= (labelLength + 1);
    start += (labelLength + 1);
  }

  // 1 octet labelLength, 2 octet qtype, 2 octet qclass
  if (remaining < 5)  {
    replyWithError(dnsHeader, DNSReplyCode::FormError);
    return false;
  }

  start += 1; // Skip the 0 length label that we found above

  memcpy(&qtype, start, sizeof(qtype));
  start += 2;
  memcpy(&qclass, start, sizeof(qclass));
  start += 2;

  queryLength = start - query;

  if (qclass != lwip_htons(DNS_QCLASS_ANY)
      && qclass != lwip_htons(DNS_QCLASS_IN)) {
    replyWithError(dnsHeader, DNSReplyCode::NonExistentDomain, query, queryLength);
    return false;
  }

  if (qtype != lwip_htons(DNS_QTYPE_A)
      && qtype != lwip_htons(DNS_QTYPE_ANY)) {
    replyWithError(dnsHeader, DNSReplyCode::NonExistentDomain, query, queryLength);
    return false;
  }

  // If we have no domain name configured, just return an error
  if (_domainName.isEmpty()) {
    if (_forwarder) {
      return true;
    } else {
      replyWithError(dnsHeader, _errorReplyCode, query, queryLength);
      return false;
    }
  }

  // If we're running with a wildcard we can just return a result now
  if (_domainName == "*") {
    DEBUG_PRINTF("dnsServer - replyWithIP\r\n");
    replyWithIP(dnsHeader, query, queryLength);
    return false;
  }

  matchString = _domainName.c_str();

  start = query;

  // If there's a leading 'www', skip it
  if (*start == 3 && strncasecmp("www", (char *) start + 1, 3) == 0)
      start += 4;

  while (*start != 0) {
    labelLength = *start;
    start += 1;
    while (labelLength > 0) {
      if (tolower(*start) != *matchString) {
        if (_forwarder) {
          return true;
        } else {
          replyWithError(dnsHeader, _errorReplyCode, query, queryLength);
          return false;
        }
      }
      ++start;
      ++matchString;
      --labelLength;
    }
    if (*start == 0 && *matchString == '\0') {
      replyWithIP(dnsHeader, query, queryLength);
      return false;
    }

    if (*matchString != '.') {
      replyWithError(dnsHeader, _errorReplyCode, query, queryLength);
      return false;
    }
    ++matchString;
  }

  replyWithError(dnsHeader, _errorReplyCode, query, queryLength);
  return false;
}

void DNSServer::processNextRequest()
{
  size_t currentPacketSize;

  currentPacketSize = _udp.parsePacket();
  if (currentPacketSize == 0)
    return;

  // The DNS RFC requires that DNS packets be less than 512 bytes in size,
  // so just discard them if they are larger
  if (currentPacketSize > MAX_DNS_PACKETSIZE)
    return;

  // If the packet size is smaller than the DNS header, then someone is
  // messing with us
  if (currentPacketSize < DNS_HEADER_SIZE)
    return;

  std::unique_ptr<uint8_t[]> buffer(new (std::nothrow) uint8_t[currentPacketSize]);
  if (buffer == nullptr)
    return;

  _udp.read(buffer.get(), currentPacketSize);
  if (_dns.isSet() && _udp.remoteIP() == _dns) {
    // _forwarder may have been set to false; however, for now allow inflight
    // replys  to finish. //??
    forwardReply(buffer.get(), currentPacketSize);
  } else
  if (respondToRequest(buffer.get(), currentPacketSize)) {
    forwardRequest(buffer.get(), currentPacketSize);
  }
}

void DNSServer::writeNBOShort(uint16_t value)
{
   _udp.write((unsigned char *)&value, 2);
}

void DNSServer::replyWithIP(DNSHeader *dnsHeader,
			    unsigned char * query,
			    size_t queryLength)
{
  uint16_t value;

  dnsHeader->QR = DNS_QR_RESPONSE;
  dnsHeader->QDCount = lwip_htons(1);
  dnsHeader->ANCount = lwip_htons(1);
  dnsHeader->NSCount = 0;
  dnsHeader->ARCount = 0;

  _udp.beginPacket(_udp.remoteIP(), _udp.remotePort());
  _udp.write((unsigned char *) dnsHeader, sizeof(DNSHeader));
  _udp.write(query, queryLength);

  // Rather than restate the name here, we use a pointer to the name contained
  // in the query section. Pointers have the top two bits set.
  value = 0xC000 | DNS_HEADER_SIZE;
  writeNBOShort(lwip_htons(value));

  // Answer is type A (an IPv4 address)
  writeNBOShort(lwip_htons(DNS_QTYPE_A));

  // Answer is in the Internet Class
  writeNBOShort(lwip_htons(DNS_QCLASS_IN));

  // Output TTL (already NBO)
  _udp.write((unsigned char*)&_ttl, 4);

  // Length of RData is 4 bytes (because, in this case, RData is IPv4)
  writeNBOShort(lwip_htons(sizeof(_resolvedIP)));
  _udp.write(_resolvedIP, sizeof(_resolvedIP));
  _udp.endPacket();
}

void DNSServer::replyWithError(DNSHeader *dnsHeader,
			       DNSReplyCode rcode,
			       unsigned char *query,
			       size_t queryLength)
{
  dnsHeader->QR = DNS_QR_RESPONSE;
  dnsHeader->RCode = (unsigned char) rcode;
  if (query)
     dnsHeader->QDCount = lwip_htons(1);
  else
     dnsHeader->QDCount = 0;
  dnsHeader->ANCount = 0;
  dnsHeader->NSCount = 0;
  dnsHeader->ARCount = 0;

  _udp.beginPacket(_udp.remoteIP(), _udp.remotePort());
  _udp.write((unsigned char *)dnsHeader, sizeof(DNSHeader));
  if (query != NULL)
     _udp.write(query, queryLength);
  _udp.endPacket();
}

void DNSServer::replyWithError(DNSHeader *dnsHeader,
			       DNSReplyCode rcode)
{
  replyWithError(dnsHeader, rcode, NULL, 0);
}
