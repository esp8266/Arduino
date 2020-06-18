
#ifndef _LWIPINTF_H
#define _LWIPINTF_H

#include <lwip/netif.h>

#include <functional>

class LwipIntf
{
private:

    LwipIntf () { }

protected:

    static bool stateChangeSysCB (std::function<void(netif*)>&& cb);

public:

    static bool stateUpCB (std::function<void(netif*)>&& cb);

};

#endif // _LWIPINTF_H
