/* Klient sluzby NBNS
 */

#include "ESP8266NetBIOS.h"

#include <functional>

extern "C" {
#include "osapi.h"
#include "ets_sys.h"
#include "user_interface.h"
}

#include "lwip/opt.h"
#include "lwip/inet.h"
#include "lwip/udp.h"

#define NBNSQ_TYPE_NB (0x0020)
#define NBNSQ_CLASS_IN (0x0001)
#ifndef LWIP_PLATFORM_HTONS
#define LWIP_PLATFORM_HTONS(_n)  ((u16_t)((((_n) & 0xff) << 8) | (((_n) >> 8) & 0xff)))
#endif
#ifndef LWIP_PLATFORM_HTONL
#define LWIP_PLATFORM_HTONL(_n)  ((u32_t)( (((_n) & 0xff) << 24) | (((_n) & 0xff00) << 8) | (((_n) >> 8)  & 0xff00) | (((_n) >> 24) & 0xff) ))
#endif

// Definice struktury NBNS dotazu (alespon veci, ktere jsem vypozoroval):
struct NBNSQUESTION {
    uint16_t NBNSQ_ID; // ID dotazu
    uint8_t NBNSQ_FLAGS1;
    uint8_t NBNSQ_FLAGS2;
    uint16_t NBNSQ_QUESTIONCOUNT;
    uint16_t NBNSQ_ANSWERCOUNT;
    uint16_t NBNSQ_AUTHORITYCOUNT;
    uint16_t NBNSQ_ADDITIONALRECORDCOUNT;
    uint8_t NBNSQ_NAMESIZE; // delka nasledujiciho retezce
    char NBNSQ_NAME[32+1]; // POZOR!!! mozna tato polozka muze byt ruzne dlouha
    uint16_t NBNSQ_TYPE;
    uint16_t NBNSQ_CLASS;
} __attribute__((packed));

// Definice struktury NBNS odpovedi (stejne jako u dotazu)
struct NBNSANSWER {
    uint16_t NBNSA_ID; // ID dotazu
    uint8_t NBNSA_FLAGS1;
    uint8_t NBNSA_FLAGS2;
    uint16_t NBNSA_QUESTIONCOUNT;
    uint16_t NBNSA_ANSWERCOUNT;
    uint16_t NBNSA_AUTHORITYCOUNT;
    uint16_t NBNSA_ADDITIONALRECORDCOUNT;
    uint8_t NBNSA_NAMESIZE; // delka nasledujiciho retezce
    char NBNSA_NAME[32 + 1]; // POZOR!!! mozna tato polozka muze byt ruzne dlouha
    uint16_t NBNSA_TYPE;
    uint16_t NBNSA_CLASS;
    uint32_t NBNSA_TIMETOLIVE;
    uint16_t NBNSA_LENGTH;
    uint16_t NBNSA_NODEFLAGS; // POZOR!!! tady si nejsem moc jisty
    uint32_t NBNSA_NODEADDRESS;
} __attribute__((packed));

// Definice struktury NBNS odpovedi na dotaz na jmeno
struct NBNSANSWERN {
    uint16_t NBNSAN_ID; // ID dotazu
    uint8_t NBNSAN_FLAGS1;
    uint8_t NBNSAN_FLAGS2;
    uint16_t NBNSAN_QUESTIONCOUNT;
    uint16_t NBNSAN_ANSWERCOUNT;
    uint16_t NBNSAN_AUTHORITYCOUNT;
    uint16_t NBNSAN_ADDITIONALRECORDCOUNT;
    uint8_t NBNSAN_NAMESIZE; // delka nasledujiciho retezce
    char NBNSAN_NAME[32 + 1]; // POZOR!!! mozna tato polozka muze byt ruzne dlouha
    uint16_t NBNSAN_TYPE;
    uint16_t NBNSAN_CLASS;
    uint32_t NBNSAN_TIMETOLIVE;
    uint16_t NBNSAN_LENGTH;
    uint8_t NBNSAN_NUMBER; // number of names
    char NBNSAN_NNAME[15]; // jmeno nodu
    uint8_t NBNSAN_NTYPE; // typ jmena
    uint16_t NBNSAN_NFLAGS; // node flags
} __attribute__((packed));

/** Metoda pro ziskani jmena z kodovani NETBIOS.
 *	\param nbname Ukazatel na jmeno v NETBIOS kodovani.
 *	\param name Ukazatel na misto, kam prevadime jmeno.
 *	\param maxlen Maximalni pocet znaku v nbname.
 */
void ESP8266NetBIOS::_getnbname(char *nbname, char *name, uint8_t maxlen)
{
    uint8_t b;
    uint8_t c = 0;

    while ((*nbname != 0x0) && (c < maxlen)) {
        b = (*nbname++ - 'A') << 4; // opravime nibble a prevedeme ho do vyssich bitu
        c++; // pocitame pocet odebranych bytu
        if (*nbname != 0x0) {
            b |= *nbname++ - 'A'; // pridame nizsi nibble
            c++; // opet spocitame pocet odebranych znaku
        }
        *name++ = b; // ulozime znak do vysledku a posuneme ukazatel
    }
    *name = 0x0; // ulozime ukoncovaci 0
}

/** Prevod zadaneho textu do NETBIOS kodovani
 *	\param name Ukazatel na prevadene jmeno.
 *	\param nbname Ukazatel na misto, kam vytvarime jmeno.
 *	\param outlen Pocet vystupnich znaku (mimo ukoncovaci 0) musi byt delitelne 2
 */
void ESP8266NetBIOS::_makenbname(char *name, char *nbname, uint8_t outlen)
{
    uint8_t b;
    uint8_t c = 0;

    while (c < (outlen - 2)) {
        b = *name; // prevadeny znak
        if (b) {
            name++;    // zatim se posunujeme
        } else {
            b = 0x20;    // konec retezce je nahrazeny mezerou
        }
        *nbname++ = (b >> 4) + 'A'; // jeden nibble ze znaku
        *nbname++ = (b & 0xf) + 'A'; // druhy nibble ze znaku
        c += 2; // pocet prevedenych znaku
    }
    *nbname++ = 'A';
    *nbname++ = 'A'; // ulozime ukoncovaci 0 v NBNS kodovani
    *nbname = 0; // ulozime ukoncovaci 0 retezce
}

ESP8266NetBIOS::ESP8266NetBIOS():_pcb(NULL)
{

}
ESP8266NetBIOS::~ESP8266NetBIOS()
{
    end();
}

// Vytvoreni a otevreni UDP soketu, pokud jeste neni...
bool ESP8266NetBIOS::begin(const char *name)
{
    size_t n = strlen(name);
    if (n > sizeof(_name)) {
        // prilis dlouhe jmeno
        return false;
    }

    // presuneme jmeno zarizeni se soucasnou upravou na UPPER case
    for (size_t  i = 0; i < n; ++i) {
        _name[i] = toupper(name[i]);
    }
    _name[n] = '\0';

    if(_pcb != NULL) {
        return true;
    }
    _pcb = udp_new();
    udp_recv(_pcb, &_s_recv, (void *) this);
    err_t err = udp_bind(_pcb, (ip_addr_t*)INADDR_ANY, NBNS_PORT);
    if(err != ERR_OK) {
        end();
        return false;
    }
    return true;
}

void ESP8266NetBIOS::end()
{
    if(_pcb != NULL) {
        udp_remove(_pcb);
        _pcb = NULL;
    }
}

void ESP8266NetBIOS::_recv(udp_pcb *upcb, pbuf *pb, CONST ip_addr_t *addr, uint16_t port)
{
    (void)upcb;
    (void)addr;
    (void)port;
    while(pb != NULL) {
        uint8_t * data = (uint8_t*)((pb)->payload);
        size_t len = pb->len;
#if LWIP_VERSION_MAJOR == 1
        // check UdpContext.h
        ip_addr_t* saddr = &current_iphdr_src;
#else
        // check UdpContext.h
        const ip_addr_t* saddr = &ip_data.current_iphdr_src;
#endif

        if (len >= sizeof(struct NBNSQUESTION)) {
            struct NBNSQUESTION * question = (struct NBNSQUESTION *)data;
            if (0 == (question->NBNSQ_FLAGS1 & 0x80)) {
                char name[ NBNS_MAX_HOSTNAME_LEN + 1 ]; // dekodovane dotazovane jmeno
                char *str; // pomocna promenna, pouze pro praci s retezcem

                _getnbname(&question->NBNSQ_NAME[0], (char *)&name, question->NBNSQ_NAMESIZE); // prevedeme dotazovane jmeno
                if ((str = strchr(name, ' ')) != NULL) { // jmeno hledaneho zarizeni v tomto pripade ukoncuje i mezera
                    *str = '\0';    // ukoncime retezec na vyskytu prvni mezery
                }

                if (0 == strcmp(name, _name)) {
                    // dotaz primo na nas
                    struct NBNSANSWER nbnsa; // buffer, do ktereho je sestavena odpoved na dotaz

                    nbnsa.NBNSA_ID = question->NBNSQ_ID;// ID dotazu kopirujeme do ID odpovedi
                    nbnsa.NBNSA_FLAGS1 = 0x85;	// priznak odpovedi
                    nbnsa.NBNSA_FLAGS2 = 0; // vlajky 2 a response code
                    nbnsa.NBNSA_QUESTIONCOUNT = LWIP_PLATFORM_HTONS(0);
                    nbnsa.NBNSA_ANSWERCOUNT = LWIP_PLATFORM_HTONS(1);// poradove cislo odpovedi
                    nbnsa.NBNSA_AUTHORITYCOUNT = LWIP_PLATFORM_HTONS(0);
                    nbnsa.NBNSA_ADDITIONALRECORDCOUNT = LWIP_PLATFORM_HTONS(0);
                    nbnsa.NBNSA_NAMESIZE = sizeof(nbnsa.NBNSA_NAME) - 1; // prekopirujeme delku jmena stanice
                    _makenbname(_name, &nbnsa.NBNSA_NAME[0], sizeof(nbnsa.NBNSA_NAME) - 1); // prevedeme jmeno
                    nbnsa.NBNSA_TYPE = LWIP_PLATFORM_HTONS(0x20); // NetBIOS name
                    nbnsa.NBNSA_CLASS = LWIP_PLATFORM_HTONS(1); // Internet name
                    nbnsa.NBNSA_TIMETOLIVE = LWIP_PLATFORM_HTONL(300000UL);// Time to live (30000 sekund)
                    nbnsa.NBNSA_LENGTH = LWIP_PLATFORM_HTONS(6);
                    nbnsa.NBNSA_NODEFLAGS = LWIP_PLATFORM_HTONS(0);
                    nbnsa.NBNSA_NODEADDRESS = WiFi.localIP(); // ulozime nasi IP adresu

                    pbuf* pbt = pbuf_alloc(PBUF_TRANSPORT, sizeof(nbnsa), PBUF_RAM);
                    if(pbt != NULL) {
                        uint8_t* dst = reinterpret_cast<uint8_t*>(pbt->payload);
                        memcpy(dst, (uint8_t *)&nbnsa, sizeof(nbnsa));
                        udp_sendto(_pcb, pbt, saddr, NBNS_PORT);
                        pbuf_free(pbt);
                    }
                } else if (0 == strcmp(name, "*")) {
                    // obecny dotaz - mireny nejspis na nasi IP adresu
                    struct NBNSANSWERN nbnsan; // buffer, do ktereho je sestavena odpoved na dotaz

                    nbnsan.NBNSAN_ID = question->NBNSQ_ID;// ID dotazu kopirujeme do ID odpovedi
                    nbnsan.NBNSAN_FLAGS1 = 0x84;	// priznak odpovedi
                    nbnsan.NBNSAN_FLAGS2 = 0; // vlajky 2 a response code
                    nbnsan.NBNSAN_QUESTIONCOUNT = LWIP_PLATFORM_HTONS(0);
                    nbnsan.NBNSAN_ANSWERCOUNT = LWIP_PLATFORM_HTONS(1);// poradove cislo odpovedi
                    nbnsan.NBNSAN_AUTHORITYCOUNT = LWIP_PLATFORM_HTONS(0);
                    nbnsan.NBNSAN_ADDITIONALRECORDCOUNT = LWIP_PLATFORM_HTONS(0);
                    nbnsan.NBNSAN_NAMESIZE = question->NBNSQ_NAMESIZE; // prekopirujeme delku jmena stanice
                    memcpy(nbnsan.NBNSAN_NAME, question->NBNSQ_NAME, sizeof(nbnsan.NBNSAN_NAME)); // prekopirujeme dotazovane jmeno
                    nbnsan.NBNSAN_TYPE = LWIP_PLATFORM_HTONS(0x21); // NBSTAT
                    nbnsan.NBNSAN_CLASS = LWIP_PLATFORM_HTONS(1); // Internet name
                    nbnsan.NBNSAN_TIMETOLIVE = LWIP_PLATFORM_HTONL(0);
                    nbnsan.NBNSAN_LENGTH = LWIP_PLATFORM_HTONS(4 + sizeof(nbnsan.NBNSAN_NNAME));
                    nbnsan.NBNSAN_NUMBER = 1; // Number of names
                    memset(nbnsan.NBNSAN_NNAME, 0x20, sizeof(nbnsan.NBNSAN_NNAME));
                    memcpy(nbnsan.NBNSAN_NNAME, _name, strlen(_name));
                    nbnsan.NBNSAN_NTYPE = 0; // Workstation/Redirector
                    nbnsan.NBNSAN_NFLAGS = LWIP_PLATFORM_HTONS(0x400); // b-node, unique, active

                    pbuf* pbt = pbuf_alloc(PBUF_TRANSPORT, sizeof(nbnsan), PBUF_RAM);
                    if(pbt != NULL) {
                        uint8_t* dst = reinterpret_cast<uint8_t*>(pbt->payload);
                        memcpy(dst, (uint8_t *)&nbnsan, sizeof(nbnsan));
                        udp_sendto(_pcb, pbt, saddr, NBNS_PORT);
                        pbuf_free(pbt);
                    }
                }
            }
        }

        pbuf * this_pb = pb;
        pb = pb->next;
        this_pb->next = NULL;
        pbuf_free(this_pb);
    }
}

void ESP8266NetBIOS::_s_recv(void *arg, udp_pcb *upcb, pbuf *p, CONST ip_addr_t *addr, uint16_t port)
{
    reinterpret_cast<ESP8266NetBIOS*>(arg)->_recv(upcb, p, addr, port);
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_NETBIOS)
ESP8266NetBIOS NBNS;
#endif

// EOF
