extern "C" {
  #include "types.h"
  #include "w5100.h"
}
  
#include "Ethernet.h"

// XXX: don't make assumptions about the value of MAX_SOCK_NUM.
uint8_t EthernetClass::_state[MAX_SOCK_NUM] = { 0, 0, 0, 0 };
uint16_t EthernetClass::_server_port[MAX_SOCK_NUM] = { 0, 0, 0, 0 };

void EthernetClass::begin(uint8_t *mac, uint8_t *ip)
{
  uint8_t gateway[4];
  gateway[0] = ip[0];
  gateway[1] = ip[1];
  gateway[2] = ip[2];
  gateway[3] = 1;
  begin(mac, ip, gateway);
}

void EthernetClass::begin(uint8_t *mac, uint8_t *ip, uint8_t *gateway)
{
  uint8_t subnet[] = { 255, 255, 255, 0 };
  begin(mac, ip, gateway, subnet);
}

void EthernetClass::begin(uint8_t *mac, uint8_t *ip, uint8_t *gateway, uint8_t *subnet)
{
	iinchip_init();
	sysinit(0x55, 0x55);
	setSHAR(mac);
	setSIPR(ip);
  setGAR(gateway);
  setSUBR(subnet);
}

EthernetClass Ethernet;
