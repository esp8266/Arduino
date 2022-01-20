
#ifndef _LWIPINTF_H
#define _LWIPINTF_H

#include <lwip/netif.h>
#include <IPAddress.h>

#include <functional>

class LwipIntf
{
public:
    using CBType = std::function<void(netif*)>;

    static bool stateUpCB(LwipIntf::CBType&& cb);

    // reorder WiFi.config() parameters for a esp8266/official Arduino dual-compatibility API
    // args     | esp order  arduino order
    // ----     + ---------  -------------
    // local_ip | local_ip   local_ip
    // arg1     | gateway    dns1
    // arg2     | netmask    [Agateway
    // arg3     | dns1       netmask
    //
    // result stored into gateway/netmask/dns1
    static bool ipAddressReorder(const IPAddress& local_ip, const IPAddress& arg1, const IPAddress& arg2, const IPAddress& arg3,
                                 IPAddress& gateway, IPAddress& netmask, IPAddress& dns1);

    String hostname();
    bool   hostname(const String& aHostname)
    {
        return hostname(aHostname.c_str());
    }
    bool hostname(const char* aHostname);
    // ESP32 API compatibility
    bool setHostname(const char* aHostName)
    {
        return hostname(aHostName);
    }
    const char* getHostname();

protected:
    static bool stateChangeSysCB(LwipIntf::CBType&& cb);
};

#endif  // _LWIPINTF_H
