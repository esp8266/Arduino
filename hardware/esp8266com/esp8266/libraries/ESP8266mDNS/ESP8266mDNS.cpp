/*

ESP8266 Multicast DNS (port of CC3000 Multicast DNS library)
Version 1.1
Copyright (c) 2013 Tony DiCola (tony@tonydicola.com)
ESP8266 port (c) 2015 Ivan Grokhotkov (ivan@esp8266.com)

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

// Important RFC's for reference:
// - DNS request and response: http://www.ietf.org/rfc/rfc1035.txt
// - Multicast DNS: http://www.ietf.org/rfc/rfc6762.txt

#include "ESP8266mDNS.h"


// #define MDNS_DEBUG

#define HEADER_SIZE 12
#define QDCOUNT_OFFSET 4
#define A_RECORD_SIZE 14
#define NSEC_RECORD_SIZE 20
#define TTL_OFFSET 4
#define IP_OFFSET 10


MDNSResponder::MDNSResponder()
  : _expected(NULL)
  , _expectedLen(0)
  , _response(NULL)
  , _responseLen(0)
  , _index(0)
{ }

MDNSResponder::~MDNSResponder() {
  if (_expected) {
    delete[] _expected;
  }
  if (_response) {
    delete[] _response;
  }
}

bool MDNSResponder::begin(const char* domain, IPAddress addr, uint32_t ttlSeconds)
{ 
  _localAddr = addr;
  // Construct DNS request/response fully qualified domain name of form:
  // <domain length>, <domain characters>, 5, "local"
  size_t n = strlen(domain);
  if (n > 255) {
    // Can only handle domains that are 255 chars in length.
    return false;
  }
  _expectedLen = 12 + n;
  if (_expected) {
    delete[] _expected;
  }
  _expected = new uint8_t[_expectedLen];
  _expected[0] = (uint8_t)n;

  // Copy in domain characters as lowercase
  for (int i = 0; i < n; ++i) {
    _expected[1+i] = tolower(domain[i]);
  }

  // Values for: 
  //  - 5 (length) 
  //  - "local"
  //  - 0x00 (end of domain)
  //  - 0x00 0x01 (A record query)
  //  - 0x00 0x01 (Class IN)
  const uint8_t local[] = { 0x05, 0x6C, 0x6F, 0x63, 0x61, 0x6C, 0x00, 0x00, 0x01, 0x00, 0x01 };
  memcpy(&_expected[1+n], local, 11);

  // Construct DNS query response
  // TODO: Move these to flash or just construct in code.
  const uint8_t respHeader[] = { 0x00, 0x00,   // ID = 0
                           0x84, 0x00,   // Flags = response + authoritative answer
                           0x00, 0x00,   // Question count = 0
                           0x00, 0x01,   // Answer count = 1
                           0x00, 0x00,   // Name server records = 0
                           0x00, 0x01    // Additional records = 1
  };
  // Generate positive response for IPV4 address
  const uint8_t aRecord[] = { 0x00, 0x01,                // Type = 1, A record/IPV4 address
                        0x80, 0x01,                // Class = Internet, with cache flush bit
                        0x00, 0x00, 0x00, 0x00,    // TTL in seconds, to be filled in later
                        0x00, 0x04,                // Length of record
                        0x00, 0x00, 0x00, 0x00     // IP address, to be filled in later
  };
  // Generate negative response for IPV6 address (CC3000 doesn't support IPV6)
  const uint8_t nsecRecord[] = {  0xC0, 0x0C,                // Name offset
                            0x00, 0x2F,                // Type = 47, NSEC (overloaded by MDNS)
                            0x80, 0x01,                // Class = Internet, with cache flush bit
                            0x00, 0x00, 0x00, 0x00,    // TTL in seconds, to be filled in later
                            0x00, 0x08,                // Length of record
                            0xC0, 0x0C,                // Next domain = offset to FQDN
                            0x00,                      // Block number = 0
                            0x04,                      // Length of bitmap = 4 bytes
                            0x40, 0x00, 0x00, 0x00     // Bitmap value = Only first bit (A record/IPV4) is set
  }; 

  // Allocate memory for response.
  int queryFQDNLen = _expectedLen - 4;
  _responseLen = HEADER_SIZE + queryFQDNLen + A_RECORD_SIZE + NSEC_RECORD_SIZE;
  if (_response) {
    delete[] _response;
  }
  _response = new uint8_t[_responseLen];

  // Copy data into response.
  memcpy(_response, respHeader, HEADER_SIZE);
  memcpy(_response + HEADER_SIZE, _expected, queryFQDNLen);
  uint8_t* records = _response + HEADER_SIZE + queryFQDNLen;
  memcpy(records, aRecord, A_RECORD_SIZE);
  memcpy(records + A_RECORD_SIZE, nsecRecord, NSEC_RECORD_SIZE);

  // Add TTL to records.
  uint8_t ttl[4] = { (uint8_t)(ttlSeconds >> 24), (uint8_t)(ttlSeconds >> 16), (uint8_t)(ttlSeconds >> 8), (uint8_t)ttlSeconds };
  memcpy(records + TTL_OFFSET, ttl, 4);
  memcpy(records + A_RECORD_SIZE + 2 + TTL_OFFSET, ttl, 4);

  // Add IP address to response

  uint32_t ipAddress = (uint32_t) addr;

  records[IP_OFFSET + 3] = (uint8_t)(ipAddress >> 24);
  records[IP_OFFSET + 2] = (uint8_t)(ipAddress >> 16);
  records[IP_OFFSET + 1] = (uint8_t)(ipAddress >> 8);
  records[IP_OFFSET + 0] = (uint8_t) ipAddress;
  
  // Open the MDNS socket if it isn't already open.
  if (!_mdnsConn) {
    if (!_mdnsConn.beginMulticast(addr, IPAddress(224, 0, 0, 251), 5353)) {
      return false;
    }
  }

  return true;
}

void MDNSResponder::update() {
  if (!_mdnsConn.parsePacket())
    return;

  // Read available data.
  int n = _mdnsConn.available();

  _index = 0;

#ifdef MDNS_DEBUG
  Serial.println("{");
#endif
  // Look for domain name in request and respond with canned response if found.
  for (int i = 0; i < n; ++i) {
    uint8_t ch = tolower(_mdnsConn.read());

#ifdef MDNS_DEBUG
    String str(ch, 16);
    Serial.print("0x");
    Serial.print(str);
    Serial.print(", ");
#endif
    // Check character matches expected.
    if (ch == _expected[_index]) 
    {
      _index++;
      // Check if domain name was found and send a response.
      if (_index == _expectedLen) {
        // Send response to multicast address.
#ifdef MDNS_DEBUG
        Serial.print("responding, i=");
        Serial.println(i);
#endif
        _mdnsConn.beginPacketMulticast(IPAddress(224, 0, 0, 251), 5353, _localAddr);
        _mdnsConn.write(_response, _responseLen);
        _mdnsConn.endPacket();
        _index = 0;
      }
    }
    else if (ch == _expected[0]) {
      // Found a character that doesn't match, but does match the start of the domain.
      _index = 1;
    }
    else {
      // Found a character that doesn't match the expected character or start of domain.
      _index = 0;
    }
  }
#ifdef MDNS_DEBUG
  Serial.println("}");
#endif
}
