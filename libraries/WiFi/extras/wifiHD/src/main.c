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

#include <stdint.h>
#include "wl_api.h"
#include "wl_cm.h"

#include "lwip/init.h"
#include "lwip/dhcp.h"
#include "lwip/dns.h"
#include "lwip/tcp.h"
#include "netif/etharp.h"
#include "netif/wlif.h"

#include "board_init.h"
#include "trace.h"

#include "timer.h"
#include "util.h"
#include "cmd_wl.h"
#include "ping.h"
#include "ard_tcp.h"
#include "spi.h"
#include "ard_spi.h"
#include "delay.h"
#include "tc.h"
#include "debug.h"
#include "ard_utils.h"
#include <lwip_setup.h>

/* FIRMWARE version */
const char* fwVersion = "1.1.0";

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

bool ifStatus = false;
bool scanNetCompleted = false;

static bool initSpiComplete = false;

// variable used as enable flag for debug prints
DEFINE_DEBUG_VARIABLES();

/**
 *
 */
static void
wl_cm_scan_cb(void* ctx)
{
	INFO_INIT("Scan Completed!\n");
    scanNetCompleted=true;
}

/**
 *
 */
static void
wl_cm_conn_cb(struct wl_network_t* net, void* ctx)
{
	struct ctx_server* hs = ctx;

	LINK_LED_ON();

	INFO_INIT("Connection cb...\n");

	printk("link up, connected to \"%s\"\n", ssid2str(&net->ssid));
    if ( hs->net_cfg.dhcp_enabled == DYNAMIC_IP_CONFIG ) {
			INFO_INIT("Start DHCP...\n");
		    printk("requesting dhcp ... ");
            int8_t result = dhcp_start(hs->net_cfg.netif);
            printk((result==ERR_OK)?"OK\n":"FAILED\n");
            hs->net_cfg.dhcp_running = 1;
    }
    else {
        netif_set_up(hs->net_cfg.netif);
    }

    INFO_INIT("Start DNS...\n");
    dns_init();
}


/**
 *
 */
static void
wl_cm_disconn_cb(void* ctx)
{
	struct ctx_server* hs = ctx;

	LINK_LED_OFF();
	INFO_INIT("Disconnection cb...\n");

    if (hs->net_cfg.dhcp_running) {
    	printk("link down, release dhcp\n");
        dhcp_release(hs->net_cfg.netif);
        dhcp_stop(hs->net_cfg.netif);
        hs->net_cfg.dhcp_running = 0;
     } else {
    	 printk("link down\n");
    	 netif_set_down(hs->net_cfg.netif);
     }

     set_result_cmd(WL_FAILURE);
}

#if 0
static void wl_cm_err_cb(void* ctx)
{
    int err = *(int*)ctx;
    WARN("Error: %d\n", err);
    set_result_cmd(err);
}
#endif

/**
 *
 */
static void
ip_status_cb(struct netif* netif)
{
	INFO_INIT("IP status cb...\n");
        if (netif_is_up(netif)) {
            set_result_cmd(WL_SUCCESS);
            printk("bound to %s\n", ip2str(netif->ip_addr));
            ifStatus = true;
        }else{
        	ifStatus = false;
        	closeConnections();
        	WARN("Interface not up!\n");
        }
}


/**
 *
 */
void
led_init(void)
{
	gpio_enable_gpio_pin(LED0_GPIO);
	gpio_enable_gpio_pin(LED1_GPIO);
	gpio_enable_gpio_pin(LED2_GPIO);
	LINK_LED_OFF();
	ERROR_LED_OFF();
	DATA_LED_OFF();
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

/**
 *
 */
void 
poll(struct ctx_server* hs)
{
        /* this will trigger any scheduled timer callbacks */
        timer_poll();

        /* handle console input */
        console_poll();

        /* wl api 'tick' */
        wl_tick(timer_get_ms());

        /* lwip driver poll */
        wlif_poll(hs->net_cfg.netif);

        if (initSpiComplete) spi_poll(hs->net_cfg.netif);

#ifdef WITH_GUI
        gui_exec(timer_get_ms());
#endif
}

void initShell(void* ctx)
{
	/* initialize shell */
	INFO_INIT("Shell init...\n");
        console_init();
        console_add_cmd("scan", cmd_scan, NULL);
        console_add_cmd("connect", cmd_connect, NULL);
        console_add_cmd("setkey", cmd_setkey, NULL);
        console_add_cmd("status", cmd_status, ctx);
        console_add_cmd("debug", cmd_debug, NULL);
        console_add_cmd("dumpBuf", cmd_dumpBuf, NULL);
		console_add_cmd("ipconfig", cmd_set_ip, ctx);
#ifdef ADD_CMDS
        console_add_cmd("powersave", cmd_power, NULL);
        console_add_cmd("psconf", cmd_psconf, NULL);
#endif
#ifdef PING_CMD
        console_add_cmd("ping", cmd_ping, NULL);
#endif
        console_add_cmd("ttcp", cmd_ttcp, NULL);
#ifdef WITH_WPA
        console_add_cmd("wpass", cmd_setpass, NULL);
        console_add_cmd("dpass", cmd_delpass, NULL);
#endif
#ifdef _SPI_STATS_
        console_add_cmd("spiStat", cmd_statSpi, NULL);
        console_add_cmd("resetSpiStat", cmd_resetStatSpi, NULL);
#endif
#ifdef _DNS_CMD_
        console_add_cmd("getHost", cmd_gethostbyname, NULL);
        console_add_cmd("setDNS", cmd_setDnsServer, NULL);
#endif
        console_add_cmd("startSrv", cmd_startSrv, NULL);
        console_add_cmd("startCli", cmd_startCli, NULL);
        console_add_cmd("sendUdp", cmd_sendUdpData, NULL);

}

/**
 *
 */
void 
wl_init_complete_cb(void* ctx) 
{
	struct ctx_server *hs = ctx;
    struct ip_addr ipaddr, netmask, gw;
	wl_err_t wl_status;
	
	if (hs->net_cfg.dhcp_enabled == INIT_IP_CONFIG)
	{
		IP4_ADDR(&gw, 0,0,0,0);
		IP4_ADDR(&ipaddr, 0,0,0,0);
		IP4_ADDR(&netmask, 0,0,0,0);
			
		/* default is dhcp enabled */
		hs->net_cfg.dhcp_enabled = DYNAMIC_IP_CONFIG;
	}

    start_ip_stack(&hs->net_cfg,
                   ipaddr,
                   netmask,
                   gw);
    netif_set_status_callback(hs->net_cfg.netif, ip_status_cb);

    INFO_INIT("Starting CM...\n");
    /* start connection manager */
    wl_status = wl_cm_init(wl_cm_scan_cb, wl_cm_conn_cb, wl_cm_disconn_cb, hs);
    ASSERT(wl_status == WL_SUCCESS, "failed to init wl conn mgr");
    wl_cm_start();

    wl_scan();

    if (initSpi(hs)){
    	WARN("Spi not initialized\n");
    }else
    {
    	initSpiComplete = true;
        AVAIL_FOR_SPI();
    }

    hs->wl_init_complete = 1;
}

void startup_init(void)
{
	INIT_SIGNAL_FOR_SPI();
	BUSY_FOR_SPI();

	// if DEBUG enabled use DEB_PIN_GPIO for debug purposes
    DEB_PIN_ENA();
	DEB_PIN_ENA(2);
    DEB_PIN_UP();
	DEB_PIN_UP(2);
}

const char timestamp[] = __TIMESTAMP__;

/**
 *
 */
int
main(void)
{
	wl_err_t wl_status;
	int status;
	struct ctx_server *hs;
    enum wl_host_attention_mode mode;

    startup_init();

    board_init();

    led_init();

    tc_init();

    delay_init(FOSC0);

#ifdef _TEST_SPI_
    for (;;)
    {
    	 /* handle console input */

    	console_poll();

    	spi_poll(NULL);

     }
#else
    printk("Arduino Wifi Startup... [%s]\n", timestamp);

    size_t size_ctx_server = sizeof(struct ctx_server);
	hs = calloc(1, size_ctx_server);
	ASSERT(hs, "out of memory");

	size_t size_netif = sizeof(struct netif);
	hs->net_cfg.netif = calloc(1, size_netif);
	ASSERT(hs->net_cfg.netif, "out of memory");
	hs->net_cfg.dhcp_enabled = INIT_IP_CONFIG;

	INFO_INIT("hs:%p size:0x%x netif:%p size:0x%x\n", hs, size_ctx_server,
			hs->net_cfg.netif, size_netif);
    initShell(hs);
	timer_init(NULL, NULL);
    lwip_init();
        
	status = fw_download_init();
	ASSERT(status == 0, "failed to prepare for firmware download\n");

    wl_status = wl_transport_init(fw_read_cb, hs, &mode);
    if (wl_status != WL_SUCCESS)
            goto err;
    INFO_INIT("Mode: 0x%x\n", mode);
    wl_status = wl_init(hs, wl_init_complete_cb, mode);
    if (wl_status != WL_SUCCESS)
            goto err;

    /* start main loop */
    for (;;)
            poll(hs);


err:
    /* show error message on console and display if wlan initialization fails */

#define WL_CARD_FAILURE_STR     "Could not detect wl device, aborting\n"
#define WL_FIRMWARE_INVALID_STR "Invalid firmware data, aborting\n"
#define WL_OTHER_FAILURE_STR    "Failed to start wl initialization\n"

    switch (wl_status) {
    case WL_CARD_FAILURE:
            printk(WL_CARD_FAILURE_STR);
            break;

    case WL_FIRMWARE_INVALID:
            printk(WL_FIRMWARE_INVALID_STR);
            break;

    default:
            printk(WL_OTHER_FAILURE_STR);
            break;
    }
    for (;;) {
            timer_poll();
    }
#endif
}
