#include "DNSServer.h"
#include <lwip/def.h>
#include <Arduino.h>
#include <memory>

#ifdef DEBUG_ESP_PORT
#define DEBUG_OUTPUT DEBUG_ESP_PORT
#else
#define DEBUG_OUTPUT Serial
#endif

#define DNS_HEADER_SIZE sizeof(DNSHeader)

DNSServer::DNSServer()
{
    _ttl = lwip_htonl(60);
    _errorReplyCode = DNSReplyCode::NonExistentDomain;
}

bool DNSServer::start(const uint16_t &port, const String &domainName,
                      const IPAddress &resolvedIP)
{
    _port = port;

    _domainName = domainName;
    _resolvedIP[0] = resolvedIP[0];
    _resolvedIP[1] = resolvedIP[1];
    _resolvedIP[2] = resolvedIP[2];
    _resolvedIP[3] = resolvedIP[3];
    downcaseAndRemoveWwwPrefix(_domainName);
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

void DNSServer::stop()
{
    _udp.stop();
}

void DNSServer::downcaseAndRemoveWwwPrefix(String &domainName)
{
    domainName.toLowerCase();
    if (domainName.startsWith("www."))
    {
        domainName.remove(0, 4);
    }
}

void DNSServer::respondToRequest(uint8_t *buffer, size_t length)
{
    DNSHeader *dnsHeader;
    uint8_t *query, *start;
    const char *matchString;
    size_t remaining, labelLength, queryLength;
    uint16_t qtype, qclass;

    dnsHeader = (DNSHeader *)buffer;

    // Must be a query for us to do anything with it
    if (dnsHeader->QR != DNS_QR_QUERY)
    {
        return;
    }

    // If operation is anything other than query, we don't do it
    if (dnsHeader->OPCode != DNS_OPCODE_QUERY)
    {
        return replyWithError(dnsHeader, DNSReplyCode::NotImplemented);
    }

    // Only support requests containing single queries - everything else
    // is badly defined
    if (dnsHeader->QDCount != lwip_htons(1))
    {
        return replyWithError(dnsHeader, DNSReplyCode::FormError);
    }

    // We must return a FormError in the case of a non-zero ARCount to
    // be minimally compatible with EDNS resolvers
    if (dnsHeader->ANCount != 0 || dnsHeader->NSCount != 0
            || dnsHeader->ARCount != 0)
    {
        return replyWithError(dnsHeader, DNSReplyCode::FormError);
    }

    // Even if we're not going to use the query, we need to parse it
    // so we can check the address type that's being queried

    query = start = buffer + DNS_HEADER_SIZE;
    remaining = length - DNS_HEADER_SIZE;
    while (remaining != 0 && *start != 0)
    {
        labelLength = *start;
        if (labelLength + 1 > remaining)
        {
            return replyWithError(dnsHeader, DNSReplyCode::FormError);
        }
        remaining -= (labelLength + 1);
        start += (labelLength + 1);
    }

    // 1 octet labelLength, 2 octet qtype, 2 octet qclass
    if (remaining < 5)
    {
        return replyWithError(dnsHeader, DNSReplyCode::FormError);
    }

    start += 1; // Skip the 0 length label that we found above

    memcpy(&qtype, start, sizeof(qtype));
    start += 2;
    memcpy(&qclass, start, sizeof(qclass));
    start += 2;

    queryLength = start - query;

    if (qclass != lwip_htons(DNS_QCLASS_ANY)
            && qclass != lwip_htons(DNS_QCLASS_IN))
        return replyWithError(dnsHeader, DNSReplyCode::NonExistentDomain,
                              query, queryLength);

    if (qtype != lwip_htons(DNS_QTYPE_A)
            && qtype != lwip_htons(DNS_QTYPE_ANY))
        return replyWithError(dnsHeader, DNSReplyCode::NonExistentDomain,
                              query, queryLength);

    // If we have no domain name configured, just return an error
    if (_domainName == "")
        return replyWithError(dnsHeader, _errorReplyCode,
                              query, queryLength);

    // If we're running with a wildcard we can just return a result now
    if (_domainName == "*")
    {
        return replyWithIP(dnsHeader, query, queryLength);
    }

    matchString = _domainName.c_str();

    start = query;

    // If there's a leading 'www', skip it
    if (*start == 3 && strncasecmp("www", (char *) start + 1, 3) == 0)
    {
        start += 4;
    }

    while (*start != 0)
    {
        labelLength = *start;
        start += 1;
        while (labelLength > 0)
        {
            if (tolower(*start) != *matchString)
                return replyWithError(dnsHeader, _errorReplyCode,
                                      query, queryLength);
            ++start;
            ++matchString;
            --labelLength;
        }
        if (*start == 0 && *matchString == '\0')
        {
            return replyWithIP(dnsHeader, query, queryLength);
        }

        if (*matchString != '.')
            return replyWithError(dnsHeader, _errorReplyCode,
                                  query, queryLength);
        ++matchString;
    }

    return replyWithError(dnsHeader, _errorReplyCode,
                          query, queryLength);
}

void DNSServer::processNextRequest()
{
    size_t currentPacketSize;

    currentPacketSize = _udp.parsePacket();
    if (currentPacketSize == 0)
    {
        return;
    }

    // The DNS RFC requires that DNS packets be less than 512 bytes in size,
    // so just discard them if they are larger
    if (currentPacketSize > MAX_DNS_PACKETSIZE)
    {
        return;
    }

    // If the packet size is smaller than the DNS header, then someone is
    // messing with us
    if (currentPacketSize < DNS_HEADER_SIZE)
    {
        return;
    }

    std::unique_ptr<uint8_t[]> buffer(new (std::nothrow) uint8_t[currentPacketSize]);

    if (buffer == NULL)
    {
        return;
    }

    _udp.read(buffer.get(), currentPacketSize);
    respondToRequest(buffer.get(), currentPacketSize);
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
    {
        dnsHeader->QDCount = lwip_htons(1);
    }
    else
    {
        dnsHeader->QDCount = 0;
    }
    dnsHeader->ANCount = 0;
    dnsHeader->NSCount = 0;
    dnsHeader->ARCount = 0;

    _udp.beginPacket(_udp.remoteIP(), _udp.remotePort());
    _udp.write((unsigned char *)dnsHeader, sizeof(DNSHeader));
    if (query != NULL)
    {
        _udp.write(query, queryLength);
    }
    _udp.endPacket();
}

void DNSServer::replyWithError(DNSHeader *dnsHeader,
                               DNSReplyCode rcode)
{
    replyWithError(dnsHeader, rcode, NULL, 0);
}
