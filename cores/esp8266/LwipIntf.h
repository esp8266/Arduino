
#ifndef _LWIPINTF_H
#define _LWIPINTF_H

#include <lwip/netif.h>

#include <functional>

class LwipIntf
{
public:

    using CBType = std::function <void(netif*)>;

    static bool stateUpCB (LwipIntf::CBType&& cb);

private:

    LwipIntf () { } // private, cannot be directly allocated

protected:

    static bool stateChangeSysCB (LwipIntf::CBType&& cb);
};

#endif // _LWIPINTF_H
