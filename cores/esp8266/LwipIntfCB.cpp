
#include <LwipIntf.h>
#include <Schedule.h>

#define NETIF_STATUS_CB_SIZE 3

static int netifStatusChangeListLength = 0;
std::function<void(netif*)> netifStatusChangeList [NETIF_STATUS_CB_SIZE];

extern "C" void netif_status_changed (struct netif* netif)
{
    // override the default empty weak function
    for (int i = 0; i < netifStatusChangeListLength; i++)
        netifStatusChangeList[i](netif);
}

bool LwipIntf::stateChangeSysCB (std::function<void(netif*)>&& cb)
{
    if (netifStatusChangeListLength >= NETIF_STATUS_CB_SIZE)
    {
#if defined(DEBUG_ESP_CORE)
        DEBUGV("NETIF_STATUS_CB_SIZE is too low\n");
#endif
        return false;
    }

    netifStatusChangeList[netifStatusChangeListLength++] = cb;
    return true;
}

bool LwipIntf::stateUpCB (std::function<void(netif*)>&& cb)
{
    return stateChangeSysCB([cb](netif* nif)
    {
        if (netif_is_up(nif))
            schedule_function([cb, nif]()
            {
                cb(nif);
            });
    });
}
