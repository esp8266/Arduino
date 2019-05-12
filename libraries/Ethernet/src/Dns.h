// Arduino DNS client for WizNet5100-based Ethernet shield
// (c) Copyright 2009-2010 MCQN Ltd.
// Released under Apache License, version 2.0

#ifndef DNSClient_h
#define DNSClient_h

#include <EthernetUdp.h>

class DNSClient
{
public:
    // ctor
    void begin(const IPAddress& aDNSServer);

    /** Convert a numeric IP address string into a four-byte IP address.
        @param aIPAddrString IP address to convert
        @param aResult IPAddress structure to store the returned IP address
        @result 1 if aIPAddrString was successfully converted to an IP address,
                else error code
    */
    int inet_aton_ethlib(const char *aIPAddrString, IPAddress& aResult);

    /** Resolve the given hostname to an IP address.
        @param aHostname Name to be resolved
        @param aResult IPAddress structure to store the returned IP address
        @result 1 if aIPAddrString was successfully converted to an IP address,
                else error code
    */
    int getHostByName(const char* aHostname, IPAddress& aResult);

protected:
    uint16_t BuildRequest(const char* aName);
    uint16_t ProcessResponse(uint16_t aTimeout, IPAddress& aAddress);

    IPAddress iDNSServer;
    uint16_t iRequestId;
    EthernetUDP iUdp;
};

#endif
