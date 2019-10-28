
#include "lwIPIntf.h"

// args      | esp order    arduino order
// ----      + ---------    -------------
// local_ip  | local_ip     local_ip
// arg1      | gateway      dns1
// arg2      | netmask      gateway
// arg3      | dns1         netmask
//
// result stored into gateway/netmask/dns1

bool LwipIntf::ipAddressReorder(const IPAddress& local_ip, const IPAddress& arg1, const IPAddress& arg2, const IPAddress& arg3,
                                                                IPAddress& gateway, IPAddress& netmask, IPAddress& dns1)
{
    //To allow compatibility, check first octet of 3rd arg. If 255, interpret as ESP order, otherwise Arduino order.
    gateway = arg1;
    netmask = arg2;
    dns1 = arg3;

    if(netmask[0] != 255)
    {
        //octet is not 255 => interpret as Arduino order
        gateway = arg2;
        netmask = arg3[0] == 0 ? IPAddress(255,255,255,0) : arg3; //arg order is arduino and 4th arg not given => assign it arduino default
        dns1 = arg1;
    }

    // check whether all is IPv4 (or gateway not set)
    if (!(local_ip.isV4() && netmask.isV4() && (!gateway.isSet() || gateway.isV4()))) {
        return false;
    }

    //ip and gateway must be in the same netmask
    if(gateway.isSet() && (local_ip.v4() & netmask.v4()) != (gateway.v4() & netmask.v4())) {
        return false;
    }
    
    return true;
}
