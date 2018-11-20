#include "DNSServer.h"
#include <lwip/def.h>
#include <Arduino.h>

#ifdef DEBUG_ESP_PORT
#define DEBUG_OUTPUT DEBUG_ESP_PORT
#else
#define DEBUG_OUTPUT Serial
#endif

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
  domainName.replace("www.", "");
}

void DNSServer::processNextRequest()
{
  size_t packetSize = _udp.parsePacket();

  if (packetSize >= sizeof(DNSHeader))
  {
    uint8_t* buffer = reinterpret_cast<uint8_t*>(malloc(packetSize));
    if (buffer == NULL) return;

    _udp.read(buffer, packetSize);

    DNSHeader* dnsHeader = reinterpret_cast<DNSHeader*>(buffer);

    if (dnsHeader->QR == DNS_QR_QUERY &&
        dnsHeader->OPCode == DNS_OPCODE_QUERY &&
        requestIncludesOnlyOneQuestion(dnsHeader) &&
        (_domainName == "*" || getDomainNameWithoutWwwPrefix(buffer, packetSize) == _domainName)
       )
    {
      replyWithIP(buffer, packetSize);
    }
    else if (dnsHeader->QR == DNS_QR_QUERY)
    {
      replyWithCustomCode(buffer, packetSize);
    }

    free(buffer);
  }
}

bool DNSServer::requestIncludesOnlyOneQuestion(const DNSHeader* dnsHeader)
{
  return lwip_ntohs(dnsHeader->QDCount) == 1 &&
         dnsHeader->ANCount == 0 &&
         dnsHeader->NSCount == 0 &&
         dnsHeader->ARCount == 0;
}

String DNSServer::getDomainNameWithoutWwwPrefix(const uint8_t* buffer, size_t packetSize)
{
  String parsedDomainName;
  
  const uint8_t* pos = buffer + sizeof(DNSHeader);
  const uint8_t* end = buffer + packetSize;

  // to minimize reallocations due to concats below
  // we reserve enough space that a median or average domain
  // name size cold be easily contained without a reallocation
  // - max size would be 253, in 2013, average is 11 and max was 42
  //
  parsedDomainName.reserve(32); 

  uint8_t labelLength = *pos;

  while (true)
  {
    if (labelLength == 0)
    {
        // no more labels
        downcaseAndRemoveWwwPrefix(parsedDomainName);
        return parsedDomainName;
    }

    // append next label
    for (int i = 0; i < labelLength && pos < end; i++)
    {
      pos++;
      parsedDomainName += static_cast<char>(*pos);
    }

    if (pos >= end)
    {
        // malformed packet, return an empty domain name
        parsedDomainName = "";
        return parsedDomainName;
    }
    else
    {
        // next label
        pos++;
        labelLength = *pos;

        // if there is another label, add delimiter
        if (labelLength != 0)
        {
            parsedDomainName += ".";
        }
    }
  }
}

void DNSServer::replyWithIP(uint8_t* buffer, size_t packetSize)
{
  DNSHeader* dnsHeader = reinterpret_cast<DNSHeader*>(buffer);

  dnsHeader->QR = DNS_QR_RESPONSE;
  dnsHeader->ANCount = dnsHeader->QDCount;
  dnsHeader->QDCount = dnsHeader->QDCount; 
  //dnsHeader->RA = 1;  

  _udp.beginPacket(_udp.remoteIP(), _udp.remotePort());
  _udp.write(buffer, packetSize);

  _udp.write((uint8_t)192); //  answer name is a pointer
  _udp.write((uint8_t)12);  // pointer to offset at 0x00c

  _udp.write((uint8_t)0);   // 0x0001  answer is type A query (host address)
  _udp.write((uint8_t)1);

  _udp.write((uint8_t)0);   //0x0001 answer is class IN (internet address)
  _udp.write((uint8_t)1);
 
  _udp.write((unsigned char*)&_ttl, 4);

  // Length of RData is 4 bytes (because, in this case, RData is IPv4)
  _udp.write((uint8_t)0);
  _udp.write((uint8_t)4);
  _udp.write(_resolvedIP, sizeof(_resolvedIP));
  _udp.endPacket();

  #ifdef DEBUG_ESP_DNS
    DEBUG_OUTPUT.printf("DNS responds: %s for %s\n",
            IPAddress(_resolvedIP).toString().c_str(), getDomainNameWithoutWwwPrefix(buffer, packetSize).c_str() );
  #endif
}

void DNSServer::replyWithCustomCode(uint8_t* buffer, size_t packetSize)
{
  if (packetSize < sizeof(DNSHeader))
  {
    return;
  }

  DNSHeader* dnsHeader = reinterpret_cast<DNSHeader*>(buffer);

  dnsHeader->QR = DNS_QR_RESPONSE;
  dnsHeader->RCode = (unsigned char)_errorReplyCode;
  dnsHeader->QDCount = 0;

  _udp.beginPacket(_udp.remoteIP(), _udp.remotePort());
  _udp.write(buffer, sizeof(DNSHeader));
  _udp.endPacket();
}
