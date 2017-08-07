
/*

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met: 

1. Redistributions of source code must retain the above copyright notice, 
this list of conditions and the following disclaimer. 
2. Redistributions in binary form must reproduce the above copyright notice, 
this list of conditions and the following disclaimer in the documentation 
and/or other materials provided with the distribution. 
3. The name of the author may not be used to endorse or promote products 
derived from this software without specific prior written permission. 

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS AND ANY EXPRESS OR IMPLIED 
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT 
OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING 
IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.

author: d. gauchard

*/

// esp(lwip1.4) side of glue for esp8266
// - sdk-2.0.0(656edbf)
// - sdk-2.1.0(116b762)

// todo: get rid of esp_guess_netif_idx()

#include "arch/cc.h"
#include "lwip/timers.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/pbuf.h"
#include "netif/etharp.h"
#include "lwip/mem.h"

#include "glue.h"

#define DBG	"lwESP: "
#define STUB(x) do { uerror("STUB: " #x "\n"); } while (0)

// guessed interface, esp blobs
void system_pp_recycle_rx_pkt (void*);
void system_station_got_ip_set(ip_addr_t* ip, ip_addr_t* mask, ip_addr_t* gw);

// ethbroadcast linked from blobs
const struct eth_addr ethbroadcast = {{0xff,0xff,0xff,0xff,0xff,0xff}};
// linked from blobs
struct netif *netif_default;

///////////////////////////////////////
// from pbuf.c
#define SIZEOF_STRUCT_PBUF	LWIP_MEM_ALIGN_SIZE(sizeof(struct pbuf))
// from pbuf.h
#ifndef PBUF_RSV_FOR_WLAN
#error PBUF_RSV_FOR_WLAN should be defined
#endif
#ifndef EBUF_LWIP
#error EBUF_LWIP should be defined
#endif
#define EP_OFFSET 36

///////////////////////////////////////
// netif

#define netif_sta netif_esp[STATION_IF]		// hardly used
#define netif_ap  netif_esp[SOFTAP_IF]		// hardly used
static struct netif* netif_esp[2] = { NULL, NULL };

///////////////////////////////////////
// glue converters

err_t glue2esp_err (err_glue_t err)
{
	switch (err)
	{
	case GLUE_ERR_OK         : return ERR_OK;
	case GLUE_ERR_MEM        : return ERR_MEM;
	case GLUE_ERR_BUF        : return ERR_BUF;
	case GLUE_ERR_TIMEOUT    : return ERR_TIMEOUT;
	case GLUE_ERR_RTE        : return ERR_RTE;
	case GLUE_ERR_INPROGRESS : return ERR_INPROGRESS;
	case GLUE_ERR_VAL        : return ERR_VAL;
	case GLUE_ERR_WOULDBLOCK : return ERR_WOULDBLOCK;
	case GLUE_ERR_ABRT       : return ERR_ABRT;
	case GLUE_ERR_RST        : return ERR_RST;
	case GLUE_ERR_CLSD       : return ERR_CLSD;
	case GLUE_ERR_CONN       : return ERR_CONN;
	case GLUE_ERR_ARG        : return ERR_ARG;
	case GLUE_ERR_USE        : return ERR_USE;
	case GLUE_ERR_IF         : return ERR_IF;
	case GLUE_ERR_ISCONN     : return ERR_ISCONN;

	/* old does not have: */
	case GLUE_ERR_ALREADY    : return ERR_ABRT;

	default: return ERR_ABRT;
	}
};

err_glue_t esp2glue_err (err_glue_t err)
{
	switch (err)
	{
	case ERR_OK         : return GLUE_ERR_OK;
	case ERR_MEM        : return GLUE_ERR_MEM;
	case ERR_BUF        : return GLUE_ERR_BUF;
	case ERR_TIMEOUT    : return GLUE_ERR_TIMEOUT;
	case ERR_RTE        : return GLUE_ERR_RTE;
	case ERR_INPROGRESS : return GLUE_ERR_INPROGRESS;
	case ERR_VAL        : return GLUE_ERR_VAL;
	case ERR_WOULDBLOCK : return GLUE_ERR_WOULDBLOCK;
	case ERR_ABRT       : return GLUE_ERR_ABRT;
	case ERR_RST        : return GLUE_ERR_RST;
	case ERR_CLSD       : return GLUE_ERR_CLSD;
	case ERR_CONN       : return GLUE_ERR_CONN;
	case ERR_ARG        : return GLUE_ERR_ARG;
	case ERR_USE        : return GLUE_ERR_USE;
	case ERR_IF         : return GLUE_ERR_IF;
	case ERR_ISCONN     : return GLUE_ERR_ISCONN;

	default: return GLUE_ERR_ABRT;
	}
};


glue_netif_flags_t esp2glue_netif_flags (u8_t flags)
{

//XXXFIXME this is the paranoia mode
// make it simpler in non-debug mode

	u8_t copy = flags;
	glue_netif_flags_t gf = 0;
	#define CF(x)	do { if (flags & NETIF_FLAG_##x) { gf |= GLUE_NETIF_FLAG_##x; flags &= ~NETIF_FLAG_##x; } } while (0)
	CF(UP);
	CF(BROADCAST);
	//CF(POINTTOPOINT);
	//CF(DHCP);
	CF(LINK_UP);
	CF(ETHARP);
	//CF(ETHERNET);
	CF(IGMP);
	#undef CF

	if (flags)
		uerror("ERROR: esp2glue_netif_flags: remaining flags not converted (0x%x->0x%x)\n", copy, flags);
	return gf;
}

///////////////////////////////////////
// display helpers

#if UDEBUG

#define stub_display_ip(pre,ip) display_ip32(pre, (ip).addr)

static void stub_display_netif_flags (int flags)
{
	#define IFF(x)	do { if (flags & NETIF_FLAG_##x) uprint("|" #x); } while (0)
	IFF(UP);
	IFF(BROADCAST);
	IFF(POINTTOPOINT);
	IFF(DHCP);
	IFF(LINK_UP);
	IFF(ETHARP);
	IFF(ETHERNET);
	IFF(IGMP);
	#undef IFF
}

static void stub_display_netif (struct netif* netif)
{
	uprint("esp-@%p idx=%d %s name=%c%c%d state=%p ",
		netif, netif->num,
		netif->num == SOFTAP_IF? "AP": netif->num == STATION_IF? "STA": "???",
		netif->name[0], netif->name[1], netif->num,
		netif->state);
	if (netif->hwaddr_len == 6)
		display_mac(netif->hwaddr);
	else
		uprint("(no mac?)");
	stub_display_netif_flags(netif->flags);
	display_ip32(" ip=", netif->ip_addr.addr);
	display_ip32(" mask=", netif->netmask.addr);
	display_ip32(" gw=", netif->gw.addr);
	uprint("\n");
}

void pbuf_info (const char* what, pbuf_layer layer, u16_t length, pbuf_type type)
{
	uerror(DBG "%s layer=%s(%d) len=%d type=%s(%d)\n",
		what,
		layer==PBUF_TRANSPORT? "transport":
		layer==PBUF_IP? "ip":
		layer==PBUF_LINK? "link":
		layer==PBUF_RAW? "raw":
		"???", (int)layer,
		length,
		type==PBUF_RAM? "ram":
		type==PBUF_ROM? "rom":
		type==PBUF_REF? "ref":
		type==PBUF_POOL? "pool":
		type==PBUF_ESF_RX? "esp-wlan":
		"???", (int)type);
}

#else // !UDEBUG

#define stub_display_netif_flags(x) do { (void)0; } while (0)
#define stub_display_netif(x) do { (void)0; } while (0)
#define pbuf_info(x,y,z,w) do { (void)0; } while (0)

#endif // !UDEBUG

///////////////////////////////////////
// quick pool to store references to data sent

#define PBUF_CUSTOM_TYPE_POOLED 0x42 // must not conflict with PBUF_* (pbuf types)
#define PBUF_WRAPPER_BLOCK 8

struct pbuf_wrapper
{
	struct pbuf pbuf;		// must be first in pbuf_wrapper
	void* ref2save;			// pointer to keep aside this pbuf
	struct pbuf_wrapper* next;	// chain of unused
};

struct pbuf_wrapper* pbuf_wrapper_head = NULL;	// first free

struct pbuf_wrapper* pbuf_wrapper_get (void)
{
	os_intr_lock();

	if (!pbuf_wrapper_head)
	{
		struct pbuf_wrapper* p = (struct pbuf_wrapper*)os_malloc(sizeof(struct pbuf_wrapper) * PBUF_WRAPPER_BLOCK);
		if (!p)
		{
			os_intr_unlock();
			return NULL;
		}
		for (int i = 0; i < PBUF_WRAPPER_BLOCK; i++)
		{
			p->pbuf.type = PBUF_CUSTOM_TYPE_POOLED;	// constant
			p->pbuf.flags = 0;			// constant
			p->pbuf.next = NULL;			// constant
			p->pbuf.eb = NULL;			// constant
			p->next = i? p - 1: NULL;
			p++;
		}
		pbuf_wrapper_head = p - 1;
	}
	struct pbuf_wrapper* ret = pbuf_wrapper_head;
	pbuf_wrapper_head = pbuf_wrapper_head->next;

	os_intr_unlock();

	return ret;
}

static void pbuf_wrapper_release (struct pbuf_wrapper* p)
{
	// make it the new head in the chain of unused
	os_intr_lock();

	p->next = pbuf_wrapper_head;
	pbuf_wrapper_head = p;

	os_intr_unlock();
}

err_glue_t glue2esp_linkoutput (int netif_idx, void* ref2save, void* data, size_t size)
{
	struct pbuf_wrapper* p = pbuf_wrapper_get();
	if (!p)
		return GLUE_ERR_MEM;

	uassert(p->pbuf.type == PBUF_CUSTOM_TYPE_POOLED);
	uassert(p->pbuf.flags == 0);
	uassert(p->pbuf.next == NULL);
	uassert(p->pbuf.eb == NULL);
	
	p->pbuf.payload = data;
	p->pbuf.len = p->pbuf.tot_len = size;
	p->pbuf.ref = 0;
	p->ref2save = ref2save;

	uprint(DBG "LINKOUTPUT: real pbuf sent to wilderness (len=%dB esp-pbuf=%p glue-pbuf=%p netifidx=%d)\n",
		p->pbuf.len,
		&p->pbuf,
		ref2save,
		netif_idx);
	
	// call blobs
	// blobs will call pbuf_free() back later
	// we will retrieve our ref2save and give it back to glue

	struct netif* netif = netif_esp[netif_idx];
	err_t err = netif->linkoutput(netif, &p->pbuf);
	if (err != ERR_OK)
		// blob/phy is exhausted, release memory
		pbuf_wrapper_release(p);
	return esp2glue_err(err);
}

#if 1
#define esp_guess_netif_idx(netif) ((netif)->num)
#else
int esp_guess_netif_idx (struct netif* netif)
{
	struct netif* test_netif_sta = eagle_lwip_getif(STATION_IF);
	struct netif* test_netif_ap = eagle_lwip_getif(SOFTAP_IF);
	int ret = netif->num;

	if (test_netif_sta)
	{
		uassert(!netif_sta || test_netif_sta == netif_sta);
		uassert(test_netif_sta->input == ethernet_input);
		uassert(test_netif_sta->output == etharp_output);
		if (netif == test_netif_sta)
			ret = STATION_IF;
	}
	
	if (test_netif_ap)
	{
		uassert(!netif_ap || test_netif_ap == netif_ap);
		uassert(test_netif_ap->input == ethernet_input);
		uassert(test_netif_ap->output == etharp_output);
		if (netif == test_netif_ap)
			ret = SOFTAP_IF;
	}
	
	if (ret < 0 || ret > 1)
	{
		uerror(DBG "guess netif: ERROR default STA");
		ret = STATION_IF;
	}
	return ret;
}
#endif

///////////////////////////////////////
// STUBS / wrappers

void lwip_init (void)
{
	uprint(DBG "lwip_init\n");
	esp2glue_lwip_init();
}

/**
 * Resolve and fill-in Ethernet address header for outgoing IP packet.
 *
 * For IP multicast and broadcast, corresponding Ethernet addresses
 * are selected and the packet is transmitted on the link.
 *
 * For unicast addresses, the packet is submitted to etharp_query(). In
 * case the IP address is outside the local network, the IP address of 
 * the gateway is used.
 *
 * @param netif The lwIP network interface which the IP packet will be sent on.
 * @param q The pbuf(s) containing the IP packet to be sent.
 * @param ipaddr The IP address of the packet destination.  
 *
 * @return
 * - ERR_RTE No route to destination (no gateway to external networks),
 * or the return type of either etharp_query() or etharp_send_ip().
 */
err_t etharp_output (struct netif* netif, struct pbuf* q, ip_addr_t* ipaddr)
{
	(void)netif; (void)q; (void)ipaddr;
	uerror("ERROR: STUB etharp_output should not be called\n");
	return ERR_ABRT;
}
                   
 /**
 * Process received ethernet frames. Using this function instead of directly
 * calling ip_input and passing ARP frames through etharp in ethernetif_input,
 * the ARP cache is protected from concurrent access.
 *
 * @param p the recevied packet, p->payload pointing to the ethernet header
 * @param netif the network interface on which the packet was received
 */
// this is called maybe through netif->input()
// maybe we could try to short-circuit netif->input
// but so far ethernet_input() is fine with AP and STA
err_t ethernet_input (struct pbuf* p, struct netif* netif)
{
	uprint(DBG "received pbuf@%p (pbuf: %dB ref=%d eb=%p) on netif ", p, p->tot_len, p->ref, p->eb);
	stub_display_netif(netif);
	
	uassert(p->tot_len == p->len && p->ref == 1);
	
#if UDUMP
	// dump packets for me (direct or broadcast)
	if (   memcmp((const char*)p->payload, netif->hwaddr, 6) == 0
	    || memcmp((const char*)p->payload, ethbroadcast.addr, 6) == 0)
	{
		dump("ethinput", p->payload, p->len);
	}
#endif

	// copy esp pbuf to glue pbuf

	void* glue_pbuf;
	void* glue_data;

	// ask glue for space to store payload into
	esp2glue_alloc_for_recv(p->len, &glue_pbuf, &glue_data);

	if (glue_pbuf)
		// copy data
		os_memcpy(glue_data, p->payload, p->len);

	// release blob's buffer asap
	pbuf_free(p);

	if (!glue_pbuf)
		// packet lost
		return ERR_MEM;

	// pass to new ip stack
	uassert(netif->num == 0 || netif->num == 1);
	return glue2esp_err(esp2glue_ethernet_input(netif->num, glue_pbuf));
}

void dhcps_start (struct ip_info* info)
{
	uprint(DBG "dhcps_start ");
	display_ip_info(info);
	uprint("\n");
	
	if (netif_ap)
		///XXX this is mandatory for blobs to be happy
		// but we should get this info back through glue
	 	netif_ap->flags |= NETIF_FLAG_UP | NETIF_FLAG_LINK_UP;

 	esp2glue_dhcps_start(info);
}

void dhcps_stop (void)
{
	// not implemented yet
	STUB(dhcps_stop);
}

void espconn_init (void)
{
	// not implemented yet
	// called at boot/reset
	// annoying message to hide:
	//STUB(espconn_init);
}

void dhcp_cleanup (struct netif* netif)
{
	// not implemented yet
	(void)netif;
	STUB(dhcp_cleanup);
}

err_t dhcp_release (struct netif* netif)
{
	// not implemented yet
	(void)netif;
	STUB(dhcp_release);
	return ERR_ABRT;
}

/**
 * Start DHCP negotiation for a network interface.
 *
 * If no DHCP client instance was attached to this interface,
 * a new client is created first. If a DHCP client instance
 * was already present, it restarts negotiation.
 *
 * @param netif The lwIP network interface
 * @return lwIP error code
 * - ERR_OK - No error
 * - ERR_MEM - Out of memory
 */
err_t dhcp_start (struct netif* netif)
{
	uprint(DBG "dhcp_start ");
	stub_display_netif(netif);

	return glue2esp_err(esp2glue_dhcp_start(esp_guess_netif_idx(netif)));
}

void dhcp_stop (struct netif* netif)
{
	(void)netif;
	// not implemented yet
	STUB(dhcp_stop);
}

/**
 * Add a network interface to the list of lwIP netifs.
 *
 * @param netif a pre-allocated netif structure
 * @param ipaddr IP address for the new netif
 * @param netmask network mask for the new netif
 * @param gw default gateway IP address for the new netif
 * @param state opaque data passed to the new netif
 * @param init callback function that initializes the interface
 * @param input callback function that is called to pass
 * ingress packets up in the protocol layer stack.
 *
 * @return netif, or NULL if failed.
 */
 
static int esp_netif_num = 0;
static struct netif* esp_netif_list = NULL;

struct netif* netif_add (
	struct netif* netif,
	ip_addr_t* ipaddr,
	ip_addr_t* netmask,
	ip_addr_t* gw,
	void* state,
	netif_init_fn init,
	netif_input_fn packet_incoming)
{
	uprint(DBG "netif_add ");
	stub_display_netif(netif);
	
	//////////////////////////////
	// this is revisited ESP lwip implementation
	netif->ip_addr.addr = 0;
	netif->netmask.addr = 0;
	netif->gw.addr = 0;
	netif->flags = 0;
	#if LWIP_DHCP
	// ok
	netif->dhcp = NULL;
	netif->dhcps_pcb = NULL;
	#endif /* LWIP_DHCP */
		#if LWIP_AUTOIP
		#error
		netif->autoip = NULL;
		#endif /* LWIP_AUTOIP */
		#if LWIP_NETIF_STATUS_CALLBACK
		#error
		netif->status_callback = NULL;
		#endif /* LWIP_NETIF_STATUS_CALLBACK */
		#if LWIP_NETIF_LINK_CALLBACK
		#error
		netif->link_callback = NULL;
		#endif /* LWIP_NETIF_LINK_CALLBACK */
	#if LWIP_IGMP
	// ok
	netif->igmp_mac_filter = NULL;
	#endif /* LWIP_IGMP */
		#if ENABLE_LOOPBACK
		#error
		netif->loop_first = NULL;
		netif->loop_last = NULL;
		#endif /* ENABLE_LOOPBACK */
	netif->state = state;

	uassert(packet_incoming = ethernet_input);
	netif->input = ethernet_input;

		#if LWIP_NETIF_HWADDRHINT
		#error
		netif->addr_hint = NULL;
		#endif /* LWIP_NETIF_HWADDRHINT*/
		#if ENABLE_LOOPBACK && LWIP_LOOPBACK_MAX_PBUFS
		#error
		netif->loop_cnt_current = 0;
		#endif /* ENABLE_LOOPBACK && LWIP_LOOPBACK_MAX_PBUFS */

	//XXX init() is from blobs to call blobs, unknown effect yet
	if (init(netif) != ERR_OK)
	{
		uprint("ERROR netif_add: caller's init() failed\n");
		return NULL;
	}

	if (esp_netif_num == 2)
	{
		if (netif == netif_sta)
		{
			uprint(DBG "esp trying to re-add STA\n");
			uassert(netif->num == STATION_IF);
		}
		else if (netif == netif_ap)
		{
			uprint(DBG "esp trying to re-add AP\n");
			uassert(netif->num == SOFTAP_IF);
		}
		else
			uerror(DBG "esp is messing with me\n");

		// assume hwaddr has not changed
		esp2glue_netif_set_addr(netif->num, ipaddr->addr, netmask->addr, gw->addr);
	}
	else
	{
		netif->num = esp_netif_num++;
		netif->next = esp_netif_list;
		esp_netif_list = netif;

		uassert(!netif_esp[netif->num]);
		netif_esp[netif->num] = netif;

		esp2glue_netif_add(netif->num, ipaddr->addr, netmask->addr, gw->addr, netif->hwaddr_len, netif->hwaddr, netif->mtu);
	}

	//////////////////////////////
	
	netif->flags |= NETIF_FLAG_LINK_UP;
	netif_set_addr(netif, ipaddr, netmask, gw);
	
	return netif;
}


/**
 * Remove a network interface from the list of lwIP netifs.
 *
 * @param netif the network interface to remove
 */
void netif_remove (struct netif* netif)
{
	(void)netif;
	uprint(DBG "trying to remove netif ");
	stub_display_netif(netif);
	
	// don't, see netif_set_down()
	//esp2glue_netif_set_updown(netif->num, 0);
	//netif->flags &= ~NETIF_FLAG_LINK_UP;
	(void)netif;
}

/**
 * Change IP address configuration for a network interface (including netmask
 * and default gateway).
 *
 * @param netif the network interface to change
 * @param ipaddr the new IP address
 * @param netmask the new netmask
 * @param gw the new default gateway
 */
void netif_set_addr (struct netif* netif, ip_addr_t* ipaddr, ip_addr_t* netmask, ip_addr_t* gw)
{
	netif->ip_addr.addr = ipaddr->addr;
	netif->netmask.addr = netmask->addr;
	netif->gw.addr = gw->addr;
	int netif_idx = esp_guess_netif_idx(netif);

	// tell blobs
	struct ip_info set;
	set.ip.addr = ipaddr->addr;
	set.netmask.addr = netmask->addr;
	set.gw.addr = gw->addr;
	wifi_set_ip_info(netif_idx, &set);

	uprint(DBG "netif_set_addr ");
	stub_display_netif(netif);
	
	esp2glue_netif_set_addr(netif_idx, ipaddr->addr, netmask->addr, gw->addr);
}

/**
 * Set a network interface as the default network interface
 * (used to output all packets for which no specific route is found)
 *
 * @param netif the default network interface
 */
void netif_set_default (struct netif* netif)
{
	uprint(DBG "netif_set_default %d\n", esp_guess_netif_idx(netif));
	netif_default = netif;
	esp2glue_netif_set_default(esp_guess_netif_idx(netif));
}

/**
 * Bring an interface down, disabling any traffic processing.
 *
 * @note: Enabling DHCP on a down interface will make it come
 * up once configured.
 * 
 * @see dhcp_start()
 */ 
void netif_set_down (struct netif* netif)
{
	uprint(DBG "netif_set_down  ");
	stub_display_netif(netif);
	
	// dont set down. some esp8266 (wemos D1 not mini) will:
	// * esp2glue_netif_set_updown
	// * restart dhcp-client _without_ netif_set_up.
	// another one (D1 mini) does not call set_down()

	// netif->flags &= ~(NETIF_FLAG_UP |  NETIF_FLAG_LINK_UP);
	// esp2glue_netif_set_updown(netif->num, 0);
	(void)netif;
}

/**
 * Bring an interface up, available for processing
 * traffic.
 * 
 * @note: Enabling DHCP on a down interface will make it come
 * up once configured.
 * 
 * @see dhcp_start()
 */ 
void netif_set_up (struct netif* netif)
{
	uerror(DBG "netif_set_up is called??");
	stub_display_netif(netif);

	netif->flags |= (NETIF_FLAG_UP |  NETIF_FLAG_LINK_UP);
	esp2glue_netif_set_updown(netif->num, 1);
}

/**
 * Allocates a pbuf of the given type (possibly a chain for PBUF_POOL type).
 *
 * The actual memory allocated for the pbuf is determined by the
 * layer at which the pbuf is allocated and the requested size
 * (from the size parameter).
 *
 * @param layer flag to define header size
 * @param length size of the pbuf's payload
 * @param type this parameter decides how and where the pbuf
 * should be allocated as follows:
 *
 * - PBUF_RAM: buffer memory for pbuf is allocated as one large
 *             chunk. This includes protocol headers as well.
 * - PBUF_ROM: no buffer memory is allocated for the pbuf, even for
 *             protocol headers. Additional headers must be prepended
 *             by allocating another pbuf and chain in to the front of
 *             the ROM pbuf. It is assumed that the memory used is really
 *             similar to ROM in that it is immutable and will not be
 *             changed. Memory which is dynamic should generally not
 *             be attached to PBUF_ROM pbufs. Use PBUF_REF instead.
 * - PBUF_REF: no buffer memory is allocated for the pbuf, even for
 *             protocol headers. It is assumed that the pbuf is only
 *             being used in a single thread. If the pbuf gets queued,
 *             then pbuf_take should be called to copy the buffer.
 * - PBUF_POOL: the pbuf is allocated as a pbuf chain, with pbufs from
 *              the pbuf pool that is allocated during pbuf_init().
 *
 * @return the allocated pbuf. If multiple pbufs where allocated, this
 * is the first pbuf of a pbuf chain.
 */

struct pbuf* pbuf_alloc (pbuf_layer layer, u16_t length, pbuf_type type)
{
	// pbuf creation from blobs
	// copy parts of original code matching specific requests

	//STUB(pbuf_alloc);
	//pbuf_info("pbuf_alloc", layer, length, type);
	
	u16_t offset = 0;
	if (layer == PBUF_RAW && type == PBUF_RAM)
	{
		offset += EP_OFFSET;
		
		/* If pbuf is to be allocated in RAM, allocate memory for it. */
		size_t alloclen = LWIP_MEM_ALIGN_SIZE(SIZEOF_STRUCT_PBUF + offset) + LWIP_MEM_ALIGN_SIZE(length);
		struct pbuf* p = (struct pbuf*)mem_malloc(alloclen);
		if (p == NULL)
			return NULL;
		/* Set up internal structure of the pbuf. */
		p->payload = LWIP_MEM_ALIGN((void *)((u8_t *)p + SIZEOF_STRUCT_PBUF + offset));
		p->len = p->tot_len = length;
		p->next = NULL;
		p->type = type;
		p->eb = NULL;
		p->ref = 1;
		p->flags = 0;
		uprint(DBG "pbuf_alloc(RAW/RAM)-> %p %dB type=%d\n", p, alloclen, type);
		return p;
	}
	
	if (layer == PBUF_RAW && type == PBUF_REF)
	{
		//unused: offset += EP_OFFSET;
		size_t alloclen = LWIP_MEM_ALIGN_SIZE(SIZEOF_STRUCT_PBUF);
		struct pbuf* p = (struct pbuf*)mem_malloc(alloclen);
		if (p == NULL)
			return NULL;
		p->payload = NULL;
		p->len = p->tot_len = length;
		p->next = NULL;
		p->type = type;
		p->eb = NULL;
		p->ref = 1;
		p->flags = 0;
		uprint(DBG "pbuf_alloc(RAW/REF)-> %p %dB type=%d\n", p, alloclen, type);
		return p;
	}

	uerror(DBG "pbuf_alloc BAD CASE\n");
		
	return NULL;
}

/**
 * Dereference a pbuf chain or queue and deallocate any no-longer-used
 * pbufs at the head of this chain or queue.
 *
 * Decrements the pbuf reference count. If it reaches zero, the pbuf is
 * deallocated.
 *
 * For a pbuf chain, this is repeated for each pbuf in the chain,
 * up to the first pbuf which has a non-zero reference count after
 * decrementing. So, when all reference counts are one, the whole
 * chain is free'd.
 *
 * @param p The pbuf (chain) to be dereferenced.
 *
 * @return the number of pbufs that were de-allocated
 * from the head of the chain.
 *
 * @note MUST NOT be called on a packet queue (Not verified to work yet).
 * @note the reference counter of a pbuf equals the number of pointers
 * that refer to the pbuf (or into the pbuf).
 *
 * @internal examples:
 *
 * Assuming existing chains a->b->c with the following reference
 * counts, calling pbuf_free(a) results in:
 * 
 * 1->2->3 becomes ...1->3
 * 3->3->3 becomes 2->3->3
 * 1->1->2 becomes ......1
 * 2->1->1 becomes 1->1->1
 * 1->1->1 becomes .......
 *
 */

u8_t pbuf_free (struct pbuf *p)
{
	//STUB(pbuf_free);
	uprint(DBG "pbuf_free(%p) ref=%d type=%d\n", p, p->ref, p->type);
	//pbuf_info("pbuf_free", -1, p->len, p->type);
	//uprint("pbuf@%p ref=%d tot_len=%d eb=%p\n", p, p->ref, p->tot_len, p->eb);
	
	#if LWIP_SUPPORT_CUSTOM_PBUF
	#error LWIP_SUPPORT_CUSTOM_PBUF is defined
	#endif
	
	uassert(p->ref == 1);

	if (p->type == PBUF_CUSTOM_TYPE_POOLED)
	{
		// allocated by glue for sending packets
		uassert(!p->eb);
		// retrieve glue structure to be freed
		struct pbuf_wrapper* pw = (struct pbuf_wrapper*)p;
		// pw->ref2save is the glue structure to release
		uprint(DBG "pbuf_free chain release glue-pbuf %p lwip1-pbuf %p\n", pw->ref2save, (char*)p);
		uassert(pw->ref2save);
		esp2glue_pbuf_freed(pw->ref2save);
		pbuf_wrapper_release(pw);
			
		return 1;
	}
		
	if (   !p->next
	    && p->ref == 1
	    && (
		   p->type == PBUF_RAM
		|| p->type == PBUF_REF
	      //|| p->type == PBUF_ESF_RX
	       ))
	{
		if (p->eb)
			system_pp_recycle_rx_pkt(p->eb);
		// allocated by blobs for received packets
		mem_free(p);
		return 1;
	}

	uerror("BAD CASE %p ref=%d tot_len=%d eb=%p\n", p, p->ref, p->tot_len, p->eb);
	return 0;
}

/**
 * Increment the reference count of the pbuf.
 *
 * @param p pbuf to increase reference counter of
 *
 */
void pbuf_ref (struct pbuf *p)
{
	uprint(DBG "pbuf_ref(%p) ref=%d->%d\n", p, p->ref, p->ref + 1);
	++(p->ref);
}

/**
 * Create a one-shot timer (aka timeout). Timeouts are processed in the
 * following cases:
 * - while waiting for a message using sys_timeouts_mbox_fetch()
 * - by calling sys_check_timeouts() (NO_SYS==1 only)
 *
 * @param msecs time in milliseconds after that the timer should expire
 * @param handler callback function to call when msecs have elapsed
 * @param arg argument to pass to the callback function
 */
void sys_timeout(u32_t msecs, sys_timeout_handler handler, void *arg)
{
	(void)msecs; (void)handler; (void)arg;
	STUB(sys_timeout);
}

/**
 * Go through timeout list (for this task only) and remove the first matching
 * entry, even though the timeout has not triggered yet.
 *
 * @note This function only works as expected if there is only one timeout
 * calling 'handler' in the list of timeouts.
 *
 * @param handler callback function that would be called by the timeout
 * @param arg callback argument that would be passed to handler
*/
void sys_untimeout(sys_timeout_handler handler, void *arg)
{
	(void)handler; (void)arg;
	STUB(sys_untimeout);
}

void glue2esp_ifup (int netif_idx, uint32_t ip, uint32_t mask, uint32_t gw)
{
	struct netif* netif = netif_esp[netif_idx];

	// backup old esp ips
	ip_addr_t oldip, oldmask, oldgw;
	oldip = netif->ip_addr;
	oldmask = netif->netmask;
	oldgw = netif->gw;
	        
	// change ips
	netif->ip_addr.addr = ip;
	netif->netmask.addr = mask;
	netif->gw.addr = gw;
	// set up
	netif->flags |= NETIF_FLAG_UP;

	// tell esp to check it has changed (by giving old ones)
	system_station_got_ip_set(&oldip, &oldmask, &oldgw);
}
