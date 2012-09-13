#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/sys.h"
#include "netif/etharp.h"
#include "netif/wlif.h"
#include <wl_api.h>
#include <wlap_api.h>

#define IFNAME0 'w'
#define IFNAME1 'l'

/* the queue size will affect the tx performance when using power save.
 * A small queue will quickly become filled up if we have to wake the device
 * before the actual transmission can occur. When the queue is filled up, the
 * packets will be discarded and retransmission will be handled by the upper
 * layers. In case of TCP, the retransmission time might be quite long.
 *
 * If the packets can be put in the pqueue instead, all the packets
 * (if possible) will be transmitted when the device wakes up, so we don't have
 * to wait for retransmission from upper layers.
 */
#define PQUEUE_SIZE 8

struct wlif_t {
	volatile uint8_t rx_pending;

	struct {
		struct pbuf* buf[PQUEUE_SIZE];
		uint8_t first;
		uint8_t last;
	} pqueue;
};

#define PQUEUE_EMPTY(q) (q.last == q.first)
#define PQUEUE_FULL(q) ((q.last + 1) % PQUEUE_SIZE == q.first)
#define PQUEUE_FIRST(q) (q.buf[q.first])
#define PQUEUE_DEQUEUE(q)                                               \
        ({                                                              \
                struct pbuf* __p = PQUEUE_FIRST(q);                     \
                q.first = (q.first + 1) % PQUEUE_SIZE;                  \
                __p;                                                    \
        })
#define PQUEUE_ENQUEUE(q, p)                                            \
        ({                                                              \
                q.buf[q.last] = p;                                      \
                q.last = (q.last + 1) % PQUEUE_SIZE;                    \
        })


static err_t process_pqueue(struct netif* netif)
{
        struct pbuf *p;
        struct pbuf *q;
        int status;
	struct wlif_t *priv = (struct wlif_t*) netif->state;

        /* queue empty? finished */
        if (PQUEUE_EMPTY(priv->pqueue))
                return ERR_OK;

        /* get first packet in queue */
        p = PQUEUE_FIRST(priv->pqueue);

        status = wl_process_tx(
                p->payload + WL_HEADER_SIZE, /* ptr to eth hdr */
                p->len - WL_HEADER_SIZE,     /* input buffer len */
                p->tot_len - WL_HEADER_SIZE, /* pkt len */
                p->payload,                  /* ptr to WE hdr */
                0,                           /* prio */
                p);                          /* pkt handle */

        /* if we fail due to power save mode, leave packet in queue and
         * try again when target is awake again (upon WL_RX_EVENT_WAKEUP).
         */
	if (status == WL_RESOURCES)
		return ERR_IF;

        /* if we fail for another reason, just discard the packet */
        if (status != WL_SUCCESS) {
                PQUEUE_DEQUEUE(priv->pqueue);
                pbuf_free(p);
                return ERR_IF;
        }

        /* Send the data from the pbuf to the interface, one pbuf at a
         * time. The size of the data in each pbuf is kept in the ->len
         * variable.
         */
        for (q = p; q != NULL; q = q->next)
		wl_tx(q->payload, q->len);

        /* remove packet from queue and dec refcnt */
        PQUEUE_DEQUEUE(priv->pqueue);
        pbuf_free(p);
        
        LINK_STATS_INC(link.xmit);

        /* tell caller to process next packet */
        return ERR_INPROGRESS;
}


/**
 * Called in interrupt context when we can read more data from the mac.
 *
 */
static void
rx_isr(void* ctx)
{
	struct netif *netif = ctx;
	struct wlif_t *priv = (struct wlif_t*) netif->state;
	priv->rx_pending = 1;
}


/**
 * In this function, the hardware should be initialized.
 * Called from wlif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */
static err_t
low_level_init(struct netif *netif)
{
        /* device capabilities */
        netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP |
                NETIF_FLAG_IGMP;
        
        /* NETIF_FLAG_LINK_UP must be set only when we have an wlan assoc */

        /* set MAC hardware address length */
        netif->hwaddr_len = ETHARP_HWADDR_LEN;

	if (wl_get_mac_addr(netif->hwaddr) != WL_SUCCESS)
		return ERR_IF;

        /* maximum transfer unit */
        netif->mtu = 1500;
        
        return ERR_OK;
}


/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and
 *        type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become availale since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */
static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{  
	struct wlif_t* priv = (struct wlif_t*) netif->state;

        /* must have a linear buffer containing up to and including
         * the ethernet header
         */
        if (p->len < sizeof(struct eth_hdr))
                return ERR_IF;
        
        /* queue full? drop packet */
        if (PQUEUE_FULL(priv->pqueue))
                return ERR_INPROGRESS; /* no one seems to check this anyway */
		
        /* queue packet */
        PQUEUE_ENQUEUE(priv->pqueue, p);
        pbuf_ref(p);
        while (process_pqueue(netif) == ERR_INPROGRESS);
        return ERR_OK; /* no one seems to check this anyway */
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_input(struct netif *netif)
{
        struct pbuf *p;
	struct wlif_t *priv = (struct wlif_t*) netif->state;

        char *stripped_pkt;
        size_t stripped_pkt_len;
        u16_t vlan;
        u8_t rx_hdr_size;
        int status;
        u16_t len;

        /* maximum packet length from wl_rx() */
        len = WL_MAX_PKT_LEN;

        /* We allocate a continous pbuf */
        p = pbuf_alloc(PBUF_RAW, len, PBUF_RAM);
        if (p == NULL) {
                LWIP_DEBUGF(NETIF_DEBUG, ("low_level_input: fail to alloc "
                                          "pbuf of len:%"S32_F"\n", len));
                return NULL;
        }

        /* Read the entire msg */
	priv->rx_pending = 0;
        wl_rx(p->payload, &len);
        if (len == 0) {
                LWIP_DEBUGF(NETIF_DEBUG, ("low_level_input: len was 0"));
                return NULL;
        }

        status = wl_process_rx(
                p->payload,             /* input buf */
                len,                    /* input buf length */
                &stripped_pkt,          
                &stripped_pkt_len,      
                &vlan);

        if (status == WL_ABSORBED) {
                LWIP_DEBUGF(NETIF_DEBUG, ("low_level_input: absorbed"));
                pbuf_free(p);
                return NULL;
        }
		
        /* Data packet, remove padding */
        rx_hdr_size = stripped_pkt - (char*) p->payload;
        pbuf_realloc(p, stripped_pkt_len + rx_hdr_size);
        
        LINK_STATS_INC(link.recv);
        return p;  
}


/**
 * This function will be called by wlif_poll() when a packet has been received
 * from the mac. Then the type of the received packet is determined and
 * the appropriate input function is called.
 *
 * @param netif the lwip network interface structure for this ethernetif
 */
static void
wlif_input(struct netif *netif)
{
        struct eth_hdr *ethhdr;
        struct pbuf *p;

        /* move received packet into a new pbuf */
        p = low_level_input(netif);
        
        /* no packet could be read, silently ignore this */
        if (p == NULL)
                return;

        /* points to packet payload, which starts with an Ethernet header */
        ethhdr = p->payload;
        switch (htons(ethhdr->type)) {
                /* IP or ARP packet? */
        case ETHTYPE_IP:
        case ETHTYPE_ARP:
#if PPPOE_SUPPORT
                /* PPPoE packet? */
        case ETHTYPE_PPPOEDISC:
        case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
                /* full packet send to tcpip_thread to process */
                if (netif->input(p, netif) != ERR_OK) {
                        LWIP_DEBUGF(NETIF_DEBUG,
                                    ("wlif_input: IP input error\n"));
                        pbuf_free(p);
                        p = NULL;
                }
                break;

        default:
                pbuf_free(p);
                p = NULL;
                break;
        }
}

static ssize_t pkt_read_cb(char *dst,
                           void *src_handle,
                           size_t read_len,
                           int offset) {
        ssize_t rc;

        rc = pbuf_copy_partial((struct pbuf *)src_handle,
                               dst,
                               read_len,
                               offset + WL_HEADER_SIZE);
        if ( 0 == rc ) {
                return -1;
        }

        return rc;
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
wlif_init(struct netif *netif)
{
        static struct wlif_t wlif;

        LWIP_ASSERT("netif != NULL", (netif != NULL));

#if LWIP_NETIF_HOSTNAME
        /* Initialize interface hostname */
        if ( NULL == netif->hostname ) {
                netif->hostname = "wlif";
        }
#endif /* LWIP_NETIF_HOSTNAME */

	netif->state = &wlif;
        netif->name[0] = IFNAME0;
        netif->name[1] = IFNAME1;

        /* We directly use etharp_output() here to save a function call.
         * You can instead declare your own function an call etharp_output()
         * from it if you have to do some checks before sending (e.g. if link
         * is available...) */
        netif->output = etharp_output;
        netif->linkoutput = low_level_output;
	
	wl_register_rx_isr(rx_isr, netif);
        wl_register_pkt_read_cb(pkt_read_cb);

        /* initialize the hardware */
        return low_level_init(netif);
}


/**
 *
 */
void
wlif_poll(struct netif* netif)
{
        struct wlif_t* priv = NULL;

        /* wl api forward progress */
        wl_poll();

        if (netif)
                priv = (struct wlif_t*) netif->state;

        /* wlif_init() not called yet? */
        if (priv == NULL)
                return;

	/* no packets pending? */
	if (!priv->rx_pending)
		return;

	/* read the pending packet */
	wlif_input(netif);

	/* send any packets that was queued due to filled up target queue
	 * or power save mode.
	 */
	process_pqueue(netif);
}
