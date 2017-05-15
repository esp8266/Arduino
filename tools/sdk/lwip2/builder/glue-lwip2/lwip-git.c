
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

// lwip2(git) side of glue

#include "lwipopts.h"
#include "lwip/err.h"
#include "lwip/init.h"
#include "lwip/netif.h"
#include "lwip/dhcp.h"
#include "lwip/etharp.h"
#include "netif/ethernet.h"
#include "lwip/apps/sntp.h"

// this is dhcpserver taken from lwip-1.4-espressif
#include "lwip/apps-esp/dhcpserver.h"

#include "glue.h"
#include "lwip-helper.h"

#define DBG "GLUE: "

static char hostname[32];
static char hostname[32];

#define netif_sta (&netif_git[STATION_IF])
#define netif_ap  (&netif_git[SOFTAP_IF])
struct netif netif_git[2];
const char netif_name[2][8] = { "station", "soft-ap" };

int doprint_allow = 0; // for doprint()

err_t glue2git_err (err_glue_t err)
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
	case GLUE_ERR_USE        : return ERR_USE;
	case GLUE_ERR_ALREADY    : return ERR_ALREADY;
	case GLUE_ERR_ISCONN     : return ERR_ISCONN;
	case GLUE_ERR_CONN       : return ERR_CONN;
	case GLUE_ERR_IF         : return ERR_IF;
	case GLUE_ERR_ABRT       : return ERR_ABRT;
	case GLUE_ERR_RST        : return ERR_RST;
	case GLUE_ERR_CLSD       : return ERR_CLSD;
	case GLUE_ERR_ARG        : return ERR_ARG;
	
	default: return ERR_ABRT;
	}
};	

err_glue_t git2glue_err (err_t err)
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
	case ERR_USE        : return GLUE_ERR_USE;
	case ERR_ALREADY    : return GLUE_ERR_ALREADY;
	case ERR_ISCONN     : return GLUE_ERR_ISCONN;
	case ERR_CONN       : return GLUE_ERR_CONN;
	case ERR_IF         : return GLUE_ERR_IF;
	case ERR_ABRT       : return GLUE_ERR_ABRT;
	case ERR_RST        : return GLUE_ERR_RST;
	case ERR_CLSD       : return GLUE_ERR_CLSD;
	case ERR_ARG        : return GLUE_ERR_ARG;

	default: return GLUE_ERR_ABRT;
	}
};	

u8_t glue2git_netif_flags (glue_netif_flags_t flags)
{

//XXXFIXME this is the paranoia mode
// make it simpler in non-debug mode

	glue_netif_flags_t copy = flags;
	u8_t nf = 0;
	#define CF(x)	do { if (flags & GLUE_NETIF_FLAG_##x) { nf |= NETIF_FLAG_##x; flags &= ~GLUE_NETIF_FLAG_##x; } } while (0)
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
		uerror("ERROR: glue2git_netif_flags: remaining flags not converted (0x%x->0x%x)\n", copy, flags);
	return nf;
}

#if UDEBUG

static void new_display_netif_flags (int flags)
{
	#define IFF(x)	do { if (flags & NETIF_FLAG_##x) uprint("|" #x); } while (0)
	IFF(UP);
	IFF(BROADCAST);
	IFF(LINK_UP);
	IFF(ETHARP);
	IFF(ETHERNET);
	IFF(IGMP);
	IFF(MLD6);
	#undef IFF
}

static const char* new_netif_name (struct netif* netif)
{
	uassert(netif == netif_sta || netif == netif_ap);
	return netif == netif_ap? "AP": "STA";
}

static void new_display_netif (struct netif* netif)
{
	
	uprint("lwip-@%p idx=%d %s name=%c%c%d mtu=%d state=%p ",
		netif,
		netif == netif_ap? SOFTAP_IF: STATION_IF,
		new_netif_name(netif),
		netif->name[0], netif->name[1], netif->num,
		netif->mtu,
		netif->state);
	if (netif->hwaddr_len == 6)
		display_mac(netif->hwaddr);
	new_display_netif_flags(netif->flags);
	display_ip32(" ip=", netif->ip_addr.addr);
	display_ip32(" mask=", netif->netmask.addr);
	display_ip32(" gw=", netif->gw.addr);
	uprint("\n");
}

#else // !UDEBUG

#define new_display_netif_flags(x) do { (void)0; } while (0)
#define new_display_netif(x) do { (void)0; } while (0)

#endif // !UDEBUG

int lwiperr_check (const char* what, err_t err)
{
	if (err != ERR_OK)
	{
		uerror("ERROR: %s (error %d)\n", what, err);
		return 0;
	}
	return 1;
}

err_glue_t esp2glue_dhcp_start (int netif_idx)
{
	uprint(DBG "dhcp_start netif: ");
	new_display_netif(&netif_git[netif_idx]);
	err_t err = dhcp_start(&netif_git[netif_idx]);
	uprint(DBG "new_dhcp_start returns %d\n", err);
	return git2glue_err(err);
}

err_t new_linkoutput (struct netif* netif, struct pbuf* p)
{
	#if !LWIP_NETIF_TX_SINGLE_PBUF
	#warning ESP netif->linkoutput cannot handle pbuf chains.
	#error LWIP_NETIF_TX_SINGLE_PBUF must be 1 in lwipopts.h
	#endif
	uassert(p->next == NULL);
	uassert(p->len == p->tot_len);

	// protect pbuf, so lwip2(git) won't free it before phy(esp) finishes sending
	pbuf_ref(p);
	
	uassert(netif->num == STATION_IF || netif->num == SOFTAP_IF);

	uprint(DBG "linkoutput: netif@%p (%s)\n", netif, netif_name[netif->num]);
	uprint(DBG "linkoutput default netif was: %d\n", netif_default? netif_default->num: -1);

	err_t err = glue2git_err(glue2esp_linkoutput(
		netif->num,
		p, p->payload, p->len));

	if (err != ERR_OK)
	{
		pbuf_free(p);
		uprint(DBG "linkoutput error sending pbuf@%p\n", p);
	}

	return err;
}

void esp2glue_pbuf_freed (void* pbuf)
{
	uprint(DBG "blobs release lwip-pbuf (ref=%d) @%p\n", ((struct pbuf*)pbuf)->ref, pbuf);
	pbuf_free((struct pbuf*)pbuf);
}

static err_t new_input (struct pbuf *p, struct netif *inp)
{
	(void)p;
	(void)inp;
	uerror("internal error, new-netif->input() cannot be called\n");
	return ERR_ABRT;
}

void esp2glue_netif_set_default (int netif_idx)
{
	uprint(DBG "netif set default %s\n", netif_name[netif_idx]);
	netif_set_default(netif_idx == STATION_IF || netif_idx == SOFTAP_IF? &netif_git[netif_idx]: NULL);
}

static void netif_sta_status_callback (struct netif* netif)
{
	uprint(DBG "netif status callback ");
	new_display_netif(netif);
	
	if (netif->flags & NETIF_FLAG_LINK_UP)
	{
		// tell ESP that link is up
		glue2esp_ifup(netif == netif_sta? STATION_IF: SOFTAP_IF, netif->ip_addr.addr, netif->netmask.addr, netif->gw.addr);

		if (netif == netif_sta)
		{
			// this is our default route
			netif_set_default(netif);
			
			// start sntp
			sntp_init();
		}
	}
}

static void netif_init_common (struct netif* netif)
{
	netif->flags |= NETIF_FLAG_IGMP;
	// irrelevant,not used since esp-lwip receive data and call esp2glue_ethernet_input()
	netif->input = new_input;
	// meaningfull:
	netif->output = etharp_output;
	netif->linkoutput = new_linkoutput;
	
	netif->hostname = hostname;
	netif->chksum_flags = NETIF_CHECKSUM_ENABLE_ALL;
	// netif->mtu given by glue
	//netif->mtu = 1500;//TCP_MSS + 40;
}

static err_t netif_init_sta (struct netif* netif)
{
	uprint(DBG "netif_sta_init\n");
	
	netif->name[0] = 's';
	netif->name[1] = 't';
	netif->status_callback = netif_sta_status_callback; // need to tell esp-netif-sta is up
	
	netif_init_common(netif);
	
	return ERR_OK;
}

static err_t netif_init_ap (struct netif* netif)
{
	uprint(DBG "netif_ap_init\n");

	netif->name[0] = 'a';
	netif->name[1] = 'p';
	netif->status_callback = NULL; // esp-netif-ap is made up by esp
	
	netif_init_common(netif);

	return ERR_OK;
}

void esp2glue_netif_add (int netif_idx, uint32_t ip, uint32_t mask, uint32_t gw, size_t hwlen, const uint8_t* hwaddr, uint16_t mtu)
{
	static int check_idx = 0;
	uassert(netif_idx == check_idx);
	check_idx++;
	
	struct netif* netif = &netif_git[netif_idx];
	netif->hwaddr_len = hwlen;
	if (hwlen && hwlen <= sizeof(netif->hwaddr))
		memcpy(netif->hwaddr, hwaddr, netif->hwaddr_len = hwlen);
	netif->mtu = mtu;

	ip4_addr_t aip = { ip }, amask = { mask }, agw = { gw };
	netif_add(
		&netif_git[netif_idx],
		&aip, &amask, &agw, /*state*/NULL,
		netif_idx == STATION_IF? netif_init_sta: netif_init_ap,
		/*useless input*/NULL);

	//XXX get these through esp2glue~flags() ?
	netif_git[netif_idx].flags |= NETIF_FLAG_ETHARP;
	netif_git[netif_idx].flags |= NETIF_FLAG_BROADCAST;

	// this was not done in old lwip and is needed at least for lwip2 dhcp client
#if 0
	uprint(DBG "set up now idx=%d\n", netif_idx);
	netif_set_link_up(&netif_git[netif_idx]);
	netif_set_up(&netif_git[netif_idx]);
#else
	netif_git[netif_idx].flags |= NETIF_FLAG_UP;
	netif_git[netif_idx].flags |= NETIF_FLAG_LINK_UP;
#endif
}

void esp2glue_netif_set_addr (int netif_idx, uint32_t ip, uint32_t mask, uint32_t gw)
{
	ip4_addr_t aip = { ip }, amask = { mask }, agw = { gw };
	netif_set_addr(&netif_git[netif_idx], &aip, &amask, &agw);
}

void esp2glue_lwip_init (void)
{
	lwip_init();
	
	sntp_servermode_dhcp(1); /* get SNTP server via DHCP */
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	// do not start sntp here, but when we got our address
	//sntp_init();
}

void esp2glue_alloc_for_recv (size_t len, void** pbuf, void** data)
{
	*pbuf = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);
	if (*pbuf)
		*data = ((struct pbuf*)*pbuf)->payload;
}

err_glue_t esp2glue_ethernet_input (int netif_idx, void* received)
{
	// this input is allocated by esp2glue_alloc_for_recv()

	//uprint(DBG "input idx=%d netif-flags=0x%x ", netif_idx, netif_git[netif_idx].flags);
	//display_ip32(" ip=", netif_git[netif_idx].ip_addr.addr);
	//nl();
	
	return git2glue_err(ethernet_input((struct pbuf*)received, &netif_git[netif_idx]));
}

void esp2glue_dhcps_start (struct ip_info* info)
{
	dhcps_start(info);
}

void esp2glue_netif_set_updown (int netif_idx, int up1_or_down0)
{
	struct netif* netif = &netif_git[netif_idx];
	if (up1_or_down0)
	{
		netif_set_link_up(netif);
		netif_set_up(netif);
	}
	else
	{
		netif_set_link_down(netif);
		netif_set_down(netif);
	}
}
