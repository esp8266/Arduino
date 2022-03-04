
#include <ESP8266WiFi.h>  // tcp API
#include <debug.h>

#include <W5100lwIP.h>
#include <W5500lwIP.h>
#include <ENC28J60lwIP.h>

// One of them is to be declared in the main sketch
// and passed to ethInitDHCP() or ethInitStatic():
// Wiznet5500lwIP eth(CSPIN);
// Wiznet5100lwIP eth(CSPIN);
// ENC28J60lwIP eth(CSPIN);

void SPI4EthInit();

template<class EthImpl>
bool ethInitDHCP(EthImpl& eth)
{
    SPI4EthInit();

    if (!eth.begin())
    {
        // hardware not responding
        DEBUGV("ethInitDHCP: hardware not responding\n");
        return false;
    }

    return true;
}

template<class EthImpl>
bool ethInitStatic(EthImpl& eth, IPAddress IP, IPAddress gateway, IPAddress netmask, IPAddress dns1,
                   IPAddress dns2 = IPADDR_NONE)
{
    SPI4EthInit();

    if (!eth.config(IP, gateway, netmask, dns1, dns2))
    {
        // invalid arguments
        DEBUGV("ethInitStatic: invalid arguments\n");
        return false;
    }

    if (!eth.begin())
    {
        // hardware not responding
        DEBUGV("ethInitStatic: hardware not responding\n");
        return false;
    }

    return true;
}
