#include <LwipDhcpServer.h>
#include <LwipDhcpServer-NonOS.h>

DhcpServer& getNonOSDhcpServer()
{
    static DhcpServer instance(nullptr);
    return instance;
}

bool DhcpServer::set_dhcps_lease(struct dhcps_lease* please)
{
    (void)please;
    return false;
}

void DhcpServer::end() { }

bool DhcpServer::begin()
{
    return false;
}

DhcpServer::DhcpServer(netif*) { }

DhcpServer::~DhcpServer()
{
    end();
}

extern "C"
{
#include <user_interface.h>

    bool wifi_softap_dhcps_start(void)
    {
        return true;
    }

    enum dhcp_status wifi_softap_dhcps_status(void)
    {
        return DHCP_STARTED;
    }

    bool wifi_softap_dhcps_stop(void)
    {
        return true;
    }

    bool wifi_softap_set_dhcps_lease(struct dhcps_lease* please)
    {
        (void)please;
        return true;
    }
}
