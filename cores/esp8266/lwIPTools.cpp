
#include "lwIP_tools.h"

bool ipAddressReorder(const IPAddress& local_ip, const IPAddress& arg1, const IPAddress& arg2, const IPAddress& arg3,
                      IPAddress& gateway, IPAddress& subnet, IPAddress& dns1)
{
  //To allow compatibility, check first octet of 3rd arg. If 255, interpret as ESP order, otherwise Arduino order.
  gateway = arg1;
  subnet = arg2;
  dns1 = arg3;

  if(subnet[0] != 255)
  {
    //octet is not 255 => interpret as Arduino order
    gateway = arg2;
    subnet = arg3[0] == 0 ? IPAddress(255,255,255,0) : arg3; //arg order is arduino and 4th arg not given => assign it arduino default
    dns1 = arg1;
  }

  // check whether all is IPv4 (or gateway not set)
  if (!(local_ip.isV4() && subnet.isV4() && (!gateway.isSet() || gateway.isV4()))) {
    return false;
  }

  //ip and gateway must be in the same subnet
  if((local_ip.v4() & subnet.v4()) != (gateway.v4() & subnet.v4())) {
    return false;
  }
  
  return true;
}
