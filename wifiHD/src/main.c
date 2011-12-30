/*
 * main.c
 *
 *  Created on: May 27, 2010
 *      Author: mlf by Metodo2 srl
 */

//#define _TEST_SPI_

#include <compiler.h>
#include "board.h"
#include "gpio.h"

#include "wl_api.h"
#include "wl_cm.h"

#include "lwip/init.h"
#include "lwip/dhcp.h"
#include "lwip/tcp.h"
#include "netif/etharp.h"
#include "netif/wlif.h"

#include "startup.h"
#include "trace.h"

#include "timer.h"
#include "wl_util.h"
#include "util.h"
#include "cmd_wl.h"
#include "httpd.h"
#include "ping.h"

#if BOARD == ARDUINO
#if !defined(DATAFLASH)
#include "wl_fw.h"

int fw_download_init(void) { return 0;}
void fw_download_cb(void* ctx, uint8_t** buf, uint32_t* len)
{
	//printk("Fw download not available!\n");
	/* remember accross different calls */
        static uint8_t* _fw_buf = (uint8_t*)&fw_buf[0];
        static uint32_t offset = 0;

        /* when firmware download is completed, this function will be invoked
         * on additional time with the input value of len set to 0. we can free
         * the firmware buffer at this time since it's no longer needed.
         */
        if (*len == 0) {
                return;
        }

        /* decide how much to read. we know *len bytes remains, but we only have
         * room for SECTOR_SIEZ bytes in our buffer (fw_buf)
         */
	uint32_t fw_len = *len;

	*buf = (_fw_buf+offset);
	*len = fw_len;

        /* we need to know where to start reading upon next call */
        offset += fw_len;

}
#else
#include "fw_download.h"
#endif
#endif

#include "spi.h"
#include "ard_spi.h"
#include "delay.h"
#include "tc.h"
#include "debug.h"
#include "ard_utils.h"

struct http_server {
	struct netif *netif;
	uint8_t wl_init_complete;
};

static bool initSpiComplete = false;

/**
 *
 */
static void
tcp_tmr_cb(void *ctx)
{
	tcp_tmr();
}


/**
 *
 */
static void
etharp_tmr_cb(void *ctx)
{
	etharp_tmr();
}


/**
 *
 */
static void
dhcp_fine_tmr_cb(void *ctx)
{
	dhcp_fine_tmr();
}


/**
 *
 */
static void
dhcp_coarse_tmr_cb(void *ctx)
{
	dhcp_coarse_tmr();
}

/**
 *
 */
static void
wl_cm_scan_cb(void* ctx)
{
#ifdef _APP_DEBUG_
	struct http_server* hs = ctx;

	uint8_t init = hs->wl_init_complete;

    INFO("Scan networks...[ OK ] %d\n", init);
#endif
    set_result(WL_SCAN_COMPLETED);
}

/**
 *
 */
static void
wl_cm_conn_cb(struct wl_network_t* net, void* ctx)
{
	struct http_server* hs = ctx;

        printk("link up, connected to \"%s\"\n", ssid2str(&net->ssid));
        printk("requesting dhcp ... ");

        dhcp_start(hs->netif);
}


/**
 *
 */
static void
wl_cm_disconn_cb(void* ctx)
{
	struct http_server* hs = ctx;

        if (netif_is_up(hs->netif)) {
                printk("link down, release dhcp\n");
                dhcp_release(hs->netif);
                dhcp_stop(hs->netif);
        } else {
                printk("link down\n");
        }

        set_result_cmd(WL_FAILURE);
}


static void wl_cm_err_cb(void* ctx)
{
    int err = *(int*)ctx;
    printk("Error: %d\n", err);
    set_result_cmd(err);
}

/**
 *
 */
static void
ip_status_cb(struct netif* netif)
{
        if (netif_is_up(netif)) {
            set_result_cmd(WL_SUCCESS);
                printk("bound to %s\n", ip2str(netif->ip_addr));
                printk("starting httpd ... ");
                if (httpd_start() == ERR_OK)
                        printk("ok\n");
                else
                        printk("fail\n");

        }
        else {
                printk("stopping httpd\n");
                httpd_stop();
        }
}


/**
 *
 */
void
led_init(void)
{
	gpio_enable_gpio_pin(LED1_GPIO);
	gpio_enable_gpio_pin(LED2_GPIO);
     LED_Off(LED1);
    LED_Off(LED2);
}


void tc_init(void)
{
	  // The timer/counter instance and channel number are used in several functions.
	  // It's defined as local variable for ease-of-use causes and readability.
	  volatile avr32_tc_t *tc = WIFI_TC;

	  // Options for waveform genration.
	  tc_waveform_opt_t waveform_opt =
	  {
	    .channel  = WIFI_TC_CHANNEL_ID,        // Channel selection.

	    .bswtrg   = TC_EVT_EFFECT_NOOP,           // Software trigger effect on TIOB.
	    .beevt    = TC_EVT_EFFECT_NOOP,           // External event effect on TIOB.
	    .bcpc     = TC_EVT_EFFECT_NOOP,           // RC compare effect on TIOB.
	    .bcpb     = TC_EVT_EFFECT_NOOP,           // RB compare effect on TIOB.

	    .aswtrg   = TC_EVT_EFFECT_NOOP,           // Software trigger effect on TIOA.
	    .aeevt    = TC_EVT_EFFECT_NOOP,           // External event effect on TIOA.
	    .acpc     = TC_EVT_EFFECT_TOGGLE,         // RC compare effect on TIOA: toggle.
	    .acpa     = TC_EVT_EFFECT_TOGGLE,         // RA compare effect on TIOA: toggle (other possibilities are none, set and clear).

	    .wavsel   = TC_WAVEFORM_SEL_UP_MODE_RC_TRIGGER,// Waveform selection: Up mode with automatic trigger(reset) on RC compare.
	    .enetrg   = FALSE,                        // External event trigger enable.
	    .eevt     = TC_EXT_EVENT_SEL_TIOB_INPUT,  // External event selection.
	    .eevtedg  = TC_SEL_NO_EDGE,               // External event edge selection.
	    .cpcdis   = FALSE,                        // Counter disable when RC compare.
	    .cpcstop  = FALSE,                        // Counter clock stopped with RC compare.

	    .burst    = TC_BURST_NOT_GATED,           // Burst signal selection.
	    .clki     = TC_CLOCK_RISING_EDGE,         // Clock inversion.
	    .tcclks   = TC_CLOCK_SOURCE_TC2           // Internal source clock 3, connected to fPBA / 2.
	  };

	  // Assign I/O to timer/counter channel pin & function.
	  gpio_enable_module_pin(WIFI_TC_CHANNEL_PIN, WIFI_TC_CHANNEL_FUNCTION);

	  // Initialize the timer/counter.
	  tc_init_waveform(tc, &waveform_opt);  // Initialize the timer/counter waveform.

	  // Set the compare triggers.
	  tc_write_ra(tc, WIFI_TC_CHANNEL_ID, 0x01A4);     // Set RA value.
	  tc_write_rc(tc, WIFI_TC_CHANNEL_ID, 0x0348);     // Set RC value.

	  // Start the timer/counter.
	  tc_start(tc, WIFI_TC_CHANNEL_ID);

}


void wifi_init()
{
}

/**
 *
 */
void 
poll(struct http_server* hs)
{
        /* this will trigger any scheduled timer callbacks */
        timer_poll();

        /* handle console input */
        console_poll();

        /* wl api 'tick' */
        wl_poll(timer_get_ms());

        /* lwip driver poll */
        wlif_poll(hs->netif);

        if (initSpiComplete) spi_poll(hs->netif);

#ifdef WITH_GUI
        gui_exec(timer_get_ms());
#endif
}

void initShell()
{
	/* initialize shell */
        console_init();
        console_add_cmd("scan", cmd_scan, NULL);
        console_add_cmd("connect", cmd_connect, NULL);
        console_add_cmd("setkey", cmd_setkey, NULL);
        console_add_cmd("status", cmd_status, NULL);
        console_add_cmd("powersave", cmd_power, NULL);
        console_add_cmd("psconf", cmd_psconf, NULL);
        console_add_cmd("ping", cmd_ping, NULL);
#ifdef WITH_WPA
        console_add_cmd("wpass", cmd_setpass, NULL);
        console_add_cmd("dpass", cmd_delpass, NULL);
#endif
#if BOARD == ARDUINO
        console_add_cmd("spiStat", cmd_statSpi, NULL);
        console_add_cmd("resetSpiStat", cmd_resetStatSpi, NULL);
#endif

}

/**
 *
 */
void 
wl_init_complete_cb(void* ctx) 
{
	struct http_server *hs = ctx;
        struct ip_addr ipaddr, netmask, gw;
	wl_err_t wl_status;
	
        IP4_ADDR(&gw, 0,0,0,0);
        IP4_ADDR(&ipaddr, 0,0,0,0);
        IP4_ADDR(&netmask, 0,0,0,0);
        
	/* add wl to lwip interface list and set as default */
        hs->netif = netif_add(hs->netif, &ipaddr, &netmask, &gw, NULL,
			      wlif_init, /* init */
			      ethernet_input /* handles ARP and IP packets */);
	ASSERT(hs->netif, "failed to add netif");
        netif_set_default(hs->netif);
        netif_set_status_callback(hs->netif, ip_status_cb);

	/* register lwip timer callbacks for tcp, arp and dhcp protocols */
        timer_sched_timeout_cb(5000, TIMEOUT_PERIODIC, 
			       etharp_tmr_cb, hs);
        timer_sched_timeout_cb(TCP_TMR_INTERVAL, TIMEOUT_PERIODIC, 
			       tcp_tmr_cb, hs);
	timer_sched_timeout_cb(DHCP_FINE_TIMER_MSECS, TIMEOUT_PERIODIC, 
			       dhcp_fine_tmr_cb, hs);
        timer_sched_timeout_cb(DHCP_COARSE_TIMER_MSECS, TIMEOUT_PERIODIC,
			       dhcp_coarse_tmr_cb, hs);

        initShell();

        if (initSpi())
        	WARN("Spi not initialized\n");
        else
        {
        	initSpiComplete = true;
        	AVAIL_FOR_SPI();
        }
        /* start connection manager */
	wl_status = wl_cm_start(wl_cm_scan_cb, wl_cm_conn_cb, wl_cm_disconn_cb, wl_cm_err_cb, hs);
	ASSERT(wl_status == WL_SUCCESS, "failed to init wl conn mgr");
}

/**
 *
 */
int
main(void)
{
	wl_err_t wl_status;
	int status;
	struct http_server *hs;

    startup_init();

    led_init();

    wifi_init();

    tc_init();

    delay_init(FOSC0);

#ifdef _TEST_SPI_
    for (;;)
    {
    	 /* handle console input */

    	console_poll();

    	spi_poll(NULL);

     }
    printk("END Arduino Wifi\n");
#else
    printk("Arduino Wifi Startup... [%s]\n", __TIMESTAMP__);

	hs = calloc(1, sizeof(struct http_server));
	ASSERT(hs, "out of memory");

	hs->netif = calloc(1, sizeof(struct netif));
	ASSERT(hs->netif, "out of memory");

        timer_init(NULL, NULL);
        lwip_init();
        
	status = fw_download_init();
	ASSERT(status == 0, "failed to prepare for firmware download\n");

        wl_status = wl_init(hs, fw_download_cb, wl_init_complete_cb);
        switch (wl_status) {
        case WL_SUCCESS:
                /* ok */
                break;

        case WL_CARD_FAILURE:
                printk("Could not detect wl device, aborting\n");
                return -1;

        case WL_FIRMWARE_INVALID:
                printk("Invalid firmware data, aborting\n");
                return -1;

        default:
                printk("Failed to start wl initialization\n");
                return -1;
        }

	/* start main loop */
        for (;;)
                poll(hs);
#endif
}
