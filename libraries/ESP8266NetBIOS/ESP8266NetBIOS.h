//
#ifndef __ESPNBNS_h__
#define __ESPNBNS_h__

#include <ESP8266WiFi.h>

#define NBNS_PORT 137
/**
* @def NBNS_MAX_HOSTNAME_LEN
* @brief maximalni delka NBNS jmena zarizeni
* @remarks
* Jmeno zarizeni musi byt uvedeno VELKYMI pismenami a nesmi obsahovat mezery (whitespaces).
*/
#define NBNS_MAX_HOSTNAME_LEN 16

struct udp_pcb;
struct pbuf;
struct ip_addr;

class ESP8266NetBIOS
{
protected:
    udp_pcb* _pcb;
    char _name[NBNS_MAX_HOSTNAME_LEN + 1];
    void _getnbname(char *nbname, char *name, uint8_t maxlen);
    void _makenbname(char *name, char *nbname, uint8_t outlen);
    void _recv(udp_pcb *upcb, pbuf *pb, struct ip_addr *addr, uint16_t port);
    static void _s_recv(void *arg, udp_pcb *upcb, pbuf *p, struct ip_addr *addr, uint16_t port);
public:
    ESP8266NetBIOS();
    ~ESP8266NetBIOS();
    bool begin(const char *name);
    void end();
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_NETBIOS)
extern ESP8266NetBIOS NBNS;
#endif

#endif
