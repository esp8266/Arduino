/*
 * ard_spi.c
 *
 *  Created on: May 27, 2010
 *      Author: mlf by Metodo2 srl
 */

//#define _APP_DEBUG_

#include <avr32/io.h>
#include "board.h"
#include "gpio.h"
#include "usart.h"
#include "ard_spi.h"
#include "ard_tcp.h"
#include "wifi_spi.h"
#include "wl_cm.h"
#include "ard_utils.h"
#include "intc.h"
#include "spi.h"
#include "debug.h"
#include "delay.h"
#include "eic.h"
#include "timer.h"
#include "lwip/dns.h"
#include <board_init.h>
#include "util.h"
#include "lwip/udp.h"
#include "lwip_setup.h"

extern const char* fwVersion;

/*! \name USART Settings
 */
//! @{
#if BOARD == EVK1105
#  define ARD_USART_SPI                 (&AVR32_USART1)
#  define ARD_USART_SPI_SCK_PIN         AVR32_USART1_CLK_0_PIN
#  define ARD_USART_SPI_SCK_FUNCTION    AVR32_USART1_CLK_0_FUNCTION
#  define ARD_USART_SPI_MISO_PIN        AVR32_USART1_TXD_0_0_PIN
#  define ARD_USART_SPI_MISO_FUNCTION   AVR32_USART1_TXD_0_0_FUNCTION
#  define ARD_USART_SPI_MOSI_PIN        AVR32_USART1_RXD_0_0_PIN
#  define ARD_USART_SPI_MOSI_FUNCTION   AVR32_USART1_RXD_0_0_FUNCTION
#  define ARD_USART_SPI_NSS_PIN         AVR32_USART1_CTS_0_0_PIN
#  define ARD_USART_SPI_NSS_FUNCTION    AVR32_USART1_CTS_0_0_FUNCTION
#  define ARD_USART_SPI_IRQ             AVR32_USART1_IRQ 
#endif
#if BOARD == ARDUINO
#  define ARD_SPI                 (&AVR32_SPI0)
#define EXT_INT_PIN_LINE1               AVR32_EIC_EXTINT_5_PIN
#define EXT_INT_FUNCTION_LINE1          AVR32_EIC_EXTINT_5_FUNCTION
#define EXT_INT_LINE1                   EXT_INT5
#define EXT_INT_IRQ_LINE1               AVR32_EIC_IRQ_5
#define EXT_INT_NB_LINES                1
#endif

/* These defines should be adjusted to match the application */
/*! \brief CPU core speed in Hz */
#define CPUHZ      60000000
/*! \brief Number of bytes in the receive buffer when operating in slave mode */
#define BUFFERSIZE    64
/*! \brief A adjustable delay avoiding multiple requests on the switches */
//#define TIMEOUT 150000
#define TIMEOUT      CPUHZ/200
/*! \brief Number of bits in each SPI package*/
#define SPI_BITS    8
/*! \brief SPI slave speed in Hz */
#define SPI_SLAVE_SPEED    1000000


#ifndef CMD_MAX_LEN
#define CMD_MAX_LEN 1024
#endif
#ifndef REPLY_MAX_LEN
#define REPLY_MAX_LEN 1024
#endif

#define  _BUFFERSIZE 100

extern void tcp_debug_print_pcbs(void);
extern bool ifStatus;
extern bool scanNetCompleted;

static char buf[CMD_MAX_LEN];
static char reply[REPLY_MAX_LEN];
static uint16_t cmdCorr = 0;
static uint16_t count = 0;
static uint16_t replyCount = 0;
static cmd_spi_state_t state = SPI_CMD_IDLE;
int receivedChars = 0;
static uint8_t _receiveBuffer[_BUFFERSIZE];
bool startReply = false;
bool end_write = false;	//TODO only for debug

// Signal indicating a new command is coming from SPI interface
static volatile Bool startRecvCmdSignal = FALSE;

#define MAX_CMD_NUM 36
typedef struct sCmd_spi_list{
	cmd_spi_cb_t cb;
	char cmd_id;
	cmd_spi_rcb_t reply_cb;
	void* ctx;
	char flags;
}tCmd_spi_list;

static tCmd_spi_list cmd_spi_list[MAX_CMD_NUM] = { {0} };

#ifdef _SPI_STATS_
typedef struct sStatSpi
{
	int	timeoutIntErr;
	int	timeoutErr;
	int txErr;
	int	rxErr;
	int wrongFrame;
	int frameDisalign;
	int overrideFrame;
	int lastCmd;
	int lastError;
	unsigned long status;
}tStatSpi;

tStatSpi statSpi = {0};

void initStatSpi()
{
	statSpi.lastCmd = 0;
	statSpi.lastError = 0;
	statSpi.status= 0;
	statSpi.txErr = 0;
	statSpi.rxErr = 0;
	statSpi.timeoutErr= 0;
	statSpi.timeoutIntErr= 0;
	statSpi.wrongFrame = 0;
	statSpi.frameDisalign = 0;
	statSpi.overrideFrame = 0;
}

void printStatSpi()
{
	printk("totSpiCmds\t: 0x%x\n", cmdCorr);
	printk("lastCmd  \t: 0x%x\n", statSpi.lastCmd);
	printk("lastErr  \t: 0x%x\n", statSpi.lastError);
	printk("spiStatus\t: 0x%X\n", statSpi.status);
	printk("spiTxErr \t: 0x%x\n", statSpi.txErr);
	printk("spiRxErr \t: 0x%x\n", statSpi.rxErr);
	printk("spiTmoErr\t: 0x%x\n", statSpi.timeoutErr);
	printk("spiTmoIntErr\t: 0x%x\n", statSpi.timeoutIntErr);
	printk("wrongFrame\t: 0x%x\n", statSpi.wrongFrame);
	printk("disalFrame\t: 0x%x\n", statSpi.frameDisalign);
	printk("overrideFrame\t: 0x%x\n", statSpi.overrideFrame);
}

cmd_state_t
cmd_statSpi(int argc, char* argv[], void* ctx)
{
	printStatSpi();
	return CMD_DONE;
}

cmd_state_t
cmd_resetStatSpi(int argc, char* argv[], void* ctx)
{
	initStatSpi();
	return CMD_DONE;
}
#endif

#define ARRAY_SIZE(a) sizeof(a) / sizeof(a[0])
#define RETURN_ERR(e) return (e==WL_SUCCESS) ? WIFI_SPI_ACK : WIFI_SPI_ERR;
#define RESET_USART_CSR(usart) usart->cr = AVR32_USART_CR_RSTSTA_MASK;

int result = WL_CONNECT_FAILED; //Store the result of the last operation

void* mapSockTCP[MAX_SOCK_NUM][MAX_MODE_NUM];

//Udp RemoteIp and remote Port
static tRemoteClient remoteClients[MAX_SOCK_NUM] = {{0,0}};

void setRemoteClient(uint16_t sock, uint32_t _ipaddr, uint16_t _port)
{
	if (sock < MAX_SOCK_NUM)
	{
		remoteClients[sock].ipaddr = _ipaddr;
		remoteClients[sock].port = _port;
	}
}

tRemoteClient* getRemoteClient(uint16_t sock)
{
	if (sock < MAX_SOCK_NUM)
	{
		return &remoteClients[sock];
	}
	return NULL;
}

struct netif* ard_netif = NULL;

// Network list retrived in the last scanNetwork
static struct wl_network_list_t network_list = { 0 };

struct ip_addr _hostIpAddr;

static bool hostIpAddrFound = false;

void* getTTCP(uint8_t sock, uint8_t mode)
{
	if (sock < MAX_SOCK_NUM)
		return mapSockTCP[sock][mode];
	return NULL;
}

int getSock(void * _ttcp)
{
	if (_ttcp != NULL)
	{
		int i = 0;
		for (; i<MAX_SOCK_NUM; i++)
		{
			if (_ttcp == mapSockTCP[i][GET_TCP_MODE(_ttcp)])
				return i;
		}
	}
	return -1;
}

void setMapSockMode(uint8_t sock, void* _ttcp, uint8_t _tcp_mode)
{
	if ((IS_VALID_SOCK(sock))&&(_ttcp!=NULL))
		mapSockTCP[sock][_tcp_mode]=_ttcp;
	INFO_TCP("Map [%d, %p, %s]\n", sock, _ttcp, Mode2Str(_tcp_mode));
}

void setMapSock(uint8_t sock, void* _ttcp)
{
	setMapSockMode(sock, _ttcp, GET_TCP_MODE(_ttcp));
}

void clearMapSockTcp(uint8_t sock, uint8_t mode)
{
	if (sock < MAX_SOCK_NUM)
	{
		//printk("UnMap [%d, %p]\n", sock, mapSockTCP[sock]);
		mapSockTCP[sock][mode] = NULL;
	}

}

void initMapSockTcp()
{
	memset(mapSockTCP, 0, sizeof(mapSockTCP));
}

#if 0
/**
 * Calculate bitrate based on number of bytes transmitted and elapsed time
 */
static void ard_tcp_print_stats(struct ttcp *ttcp) {
	uint32_t ms = timer_get_ms() - ttcp->start_time;
	uint32_t bytes = ttcp->mode == TTCP_MODE_TRANSMIT ? ttcp->nbuf
			* ttcp->buflen : ttcp->recved;

	if (ttcp->verbose)
		printk("\n");

	printk("TTCP [%p]: %d bytes processed, %d.%d KB/s (%s/%s)\n", ttcp, bytes,
			bytes / ms, bytes % ms, ProtMode2Str(ttcp->udp),
					Mode2Str(ttcp->mode));
}
#endif

void showTTCPstatus()
{
	printk("IF   status: %s\n", (ifStatus) ? "UP":"DOWN");
	printk("CONN status: %s\n", (_connected) ? "UP":"DOWN");

	int i = 0;
	for (; i<MAX_SOCK_NUM; i++)
	{
		int ii=0;
		for (; ii<MAX_MODE_NUM; ii++)
		{
			void* p = getTTCP(i, ii);
			if (p)
			{
				ttcp_t* _ttcp = (ttcp_t* )p;
				printk("Socket n.:%d(%d) [0x%x] %s %s addr:%s port:%d\n", i, ii, _ttcp, 
					ProtMode2Str(_ttcp->udp), Mode2Str(_ttcp->mode), ip2str(_ttcp->addr), _ttcp->port);
				if (_ttcp->udp == TCP_MODE)
				{
					int j = 0;
					for (; j<MAX_CLIENT_ACCEPTED; ++j)
					{
						if (_ttcp->tpcb[j]){
							printk("[%d tpcp-%p]-Status:%d\n", j, _ttcp->tpcb[j], _ttcp->tpcb[j]->state);
						}
					}

					if (_ttcp->lpcb){
						printk("[tlcp-%p]-Status:%d\n", _ttcp->lpcb, _ttcp->lpcb->state);
					}
				}else{
					if (_ttcp->upcb){
						struct ip_addr loc = _ttcp->upcb->local_ip;
						printk("[upcp-%p] flags:0x%x  local:%s[0x%x]-%d\n", 
								_ttcp->upcb, _ttcp->upcb->flags,
								ip2str(loc), loc, _ttcp->upcb->local_port);				
						tRemoteClient remote = {0,0};;
						getRemoteData(i, ii, &remote);
						struct ip_addr ipaddr = { remote.ipaddr };
						printk("remote:%s(0x%x)-%d\n", ip2str(ipaddr), remote.ipaddr, remote.port);
						}					
				}
				//ard_tcp_print_stats(_ttcp);
				printk("Data avail:%s\n", isAvailTcpDataByte(i)?"YES":"NO");
				printk("------------------------------\n");
			}
		}			
	}

	tcp_debug_print_pcbs();
}

int write_stream(volatile avr32_spi_t *spi, const char *stream, uint16_t len)
{
	uint16_t _len = 0;
	unsigned short dummy=0;

     	do {
            //SIGN1_DN();
    		if (spi_write(spi, *stream) == SPI_ERROR_TIMEOUT)
    		{
#ifdef _SPI_STATS_
    			statSpi.timeoutErr++;
    			statSpi.txErr++;
    			statSpi.lastError = SPI_ERROR_TIMEOUT;
    			statSpi.status = spi_getStatus(spi);
#endif
    			return SPI_ERROR_TIMEOUT;
    		}
    		else
    		{
    			stream++;
    			_len++;
    			spi_read(spi,&dummy);
    		}
    		//SIGN1_UP();
    }while (_len < len);
	return SPI_OK;
}

void sendError()
{
	AVAIL_FOR_SPI();
	if (spi_write(&AVR32_SPI, ERR_CMD) != SPI_ERROR_TIMEOUT)
	{
		//Wait to empty the buffer
		while(!spi_writeRegisterEmptyCheck(&AVR32_SPI));
	}
	BUSY_FOR_SPI();
	WARN("Send SPI error!\n");
}

#define ENABLE_SPI_INT() do {										\
	volatile avr32_spi_t *spi = ARD_SPI;							\
    Bool global_interrupt_enabled = Is_global_interrupt_enabled();	\
    if (global_interrupt_enabled) Disable_global_interrupt();		\
    spi->IER.rdrf = 1; spi->IER.rxbuff = 1;	spi->IER.endrx = 1;		\
    if (global_interrupt_enabled) Enable_global_interrupt();		\
}while(0);

#define DISABLE_SPI_INT() do {										\
	volatile avr32_spi_t *spi = ARD_SPI;							\
    Bool global_interrupt_enabled = Is_global_interrupt_enabled();	\
    if (global_interrupt_enabled) Disable_global_interrupt();		\
    spi->IDR.rdrf = 1; spi->IDR.rxbuff = 1;	spi->IDR.endrx = 1;										\
    if (global_interrupt_enabled) Enable_global_interrupt();		\
}while(0);

#define CLEAR_SPI_INT() do {	\
		eic_clear_interrupt_line(&AVR32_EIC, AVR32_SPI0_IRQ);	\
	}while(0);

int spi_add_cmd(char _cmd_id, cmd_spi_cb_t cb, cmd_spi_rcb_t rcb, void* ctx,
		char flag) {
	U32 i;
	for (i = 0; i < ARRAY_SIZE(cmd_spi_list); i++)
		if (!cmd_spi_list[i].cb)
			break;

	if (i == ARRAY_SIZE(cmd_spi_list))
	{
		printk("List Commands full!\n");
		return -1;
	}
	cmd_spi_list[i].cmd_id = _cmd_id;
	cmd_spi_list[i].cb = cb;
	cmd_spi_list[i].reply_cb = rcb;
	cmd_spi_list[i].ctx = ctx;
	cmd_spi_list[i].flags = flag;
	return 0;
}

int set_net_cmd_cb(int numParam, char* buf, void* ctx) {
	struct wl_ssid_t ssid;
	wl_err_t err = WL_FAILURE;
	tParam* param = (tParam*) buf;

	if (param->paramLen < WL_SSID_MAX_LENGTH) {
		memcpy(ssid.ssid, &param->param, param->paramLen);
		ssid.len = param->paramLen;
		ssid.ssid[ssid.len] = 0;
		INFO_SPI("SSID:%s\n", ssid.ssid);
		//dump(ssid.ssid, ssid.len);
		err = wl_cm_set_network(&ssid, NULL);
		if (err != 1)
			WARN("err=%d\n", err);
	} else {
		WARN("SSID len out of range");
	}
	return err;
}

extern uint8_t ascii_to_key(char *outp, const char *inp);

int set_key_cmd_cb(int numParam, char* buf, void* ctx) {
	struct wl_ssid_t ssid;
    struct wl_mac_addr_t bssid;
    uint8_t idx=0, len=0;
    char key[13], key_hex[27];
    char keyIdx[2];
	wl_err_t err = WL_SUCCESS;
	tParam* params = (tParam*) buf;

    INFO_SPI("%s params=%d\n", __FUNCTION__, numParam);

    // SSID
    memset(&ssid, 0, sizeof ssid);

	if (params->paramLen < WL_SSID_MAX_LENGTH) {
		memcpy(ssid.ssid, &params->param, params->paramLen);
		ssid.len = params->paramLen;
		INFO_SPI("%s\n", ssid.ssid);
	} else {
		//printk("SSID len out of range");
		RETURN_ERR(WL_FAILURE)
	}

    params = (tParam*)((char*)buf+PARAM_LEN_SIZE+params->paramLen);
    strncpy(keyIdx, (const char*)&params->param, params->paramLen);
    keyIdx[(uint8_t)params->paramLen]='\0';

    idx = (uint8_t)atoi(keyIdx);
    // KEY IDX
    if ((params->paramLen != 1)||(idx < 0)||(idx > 3)){
        //printk("KEY IDX out of range %d\n", idx);
        RETURN_ERR(WL_FAILURE)
    }

    params = (tParam*)((char*)params+PARAM_LEN_SIZE+params->paramLen);
    strncpy(key_hex, (const char*)&params->param, params->paramLen);
    key_hex[(uint8_t)params->paramLen]='\0';
    len = ascii_to_key(key, key_hex);
    // KEY
    if (( len != 5)&&(len != 13))
    {
        //printk("KEY len out of range %d", len);
        RETURN_ERR(WL_FAILURE)
    }
#if 0
    printk("KEY IDX = %d\n", idx);
    dump(key, len);
    printk("KEY len %d\n", len);
#endif
    memset(&bssid.octet, 0xff, sizeof bssid.octet);

    wl_add_wep_key(idx, len, key, &bssid);
    //wl_set_auth_mode(AUTH_MODE_SHARED_KEY);
    wl_set_default_wep_key(idx);

    //Connect
    err = wl_cm_set_network(&ssid, NULL);
    if (err != 1)
        WARN("err=%d\n", err);
    RETURN_ERR(err)
}

int set_passphrase_cmd_cb(int numParam, char* buf, void* ctx) {
    struct wl_network_t net;
    char pass[64];
	wl_err_t err = WL_SUCCESS;
	tParam* params = (tParam*) buf;

    INFO_SPI("%s params=%d\n", __FUNCTION__, numParam);

    memset(&net, 0, sizeof net);
    memset(net.bssid.octet, 0xFF, sizeof net.bssid.octet);

    net.enc_type = ENC_TYPE_AUTO;

    // SSID
	if (params->paramLen < WL_SSID_MAX_LENGTH) {
		memcpy(net.ssid.ssid, &params->param, params->paramLen);
		net.ssid.len = params->paramLen;
		INFO_SPI("%s %d\n", net.ssid.ssid, net.ssid.len);
	} else {
		//printk("SSID len out of range");
		RETURN_ERR(WL_FAILURE)
	}
    params = (tParam*)((char*)buf+PARAM_LEN_SIZE+params->paramLen);
    // PASSPHRASE     
   
    strncpy(pass, (const char*)&params->param, params->paramLen);
    pass[(uint8_t)params->paramLen]='\0';
    INFO_SPI("Pass: %s %d\n", pass, params->paramLen);

    if (wl_set_passphrase(&net, 
                          pass, 
                          params->paramLen, 
                          ENC_TYPE_AUTO,
                          AUTH_MODE_AUTO) 
        != WL_SUCCESS) {
            WARN("%s : Failed to add passphrase\n", __func__);

            RETURN_ERR(WL_FAILURE)
    }
    printk("Connect to network...");
    //Connect
    err = wl_cm_set_network(&net.ssid, NULL);
    if (err != 1)
        printk("err=%d\n", err);
    else
    	printk("OK\n");
    RETURN_ERR(err)
}

int set_ip_config_cmd_cb(int numParam, char* buf, void* ctx) {
    struct ip_addr lwip_addr;
    struct ctx_server *hs = ctx;
    struct net_cfg *ncfg = &(hs->net_cfg);
    struct netif *nif = ncfg->netif;
    uint8_t parmsToChange=0;
    const uint8_t MAX_IP_CONFIG_PARAMS = 3;

	wl_err_t err = WL_SUCCESS;
	tParam* params = (tParam*) buf;

    if (params->paramLen == 1)
	{
		GET_PARAM_NEXT(BYTE, params, _parmsToChange);
		parmsToChange = _parmsToChange;
	}		
    else
    	RETURN_ERR(WL_FAILURE)

    INFO_SPI("%p numParam=%d parmsToChange=%d\n", ctx, numParam, parmsToChange);

    if (parmsToChange <= MAX_IP_CONFIG_PARAMS)
    {
		int i=0;
    	for (; i<parmsToChange; ++i)
    	{
    	    if (params->paramLen == 4)
    	    {
				GET_PARAM_NEXT(LONG, params, _ip_addr);
				lwip_addr.addr = _ip_addr;
				INFO_SPI("%d] nif:%p lwip_addr=0x%x\n", i, nif, lwip_addr.addr);
    	    	switch (i)
    	    	{
    	    		case 0:	// local_ip
					{ 	    			
    	    			netif_set_ipaddr(nif, &lwip_addr);
    	    			break;
					}								
    	    		case 1:	// gateway
					{
    	    			netif_set_gw(nif, &lwip_addr);
    	    			break;
					}					
    	    		case 2:	// subnet
					{
    	    			netif_set_netmask(nif, &lwip_addr);
    	    			break;
					}					
    	    	}
    	    }else{
    	    	RETURN_ERR(WL_FAILURE)
    	    }

    	}
    	/* Disable DHCP */
    	ncfg->dhcp_enabled = STATIC_IP_CONFIG;
    }else
    	RETURN_ERR(WL_FAILURE)
		
    RETURN_ERR(err)
}

int set_dns_config_cmd_cb(int numParam, char* buf, void* ctx) {
    struct ip_addr lwip_addr;
    struct ctx_server *hs = ctx;
    struct net_cfg *ncfg = &(hs->net_cfg);
    struct netif *nif = ncfg->netif;
    uint8_t parmsToChange=0;
    const uint8_t MAX_DNS_CONFIG_PARAMS = 2;

	wl_err_t err = WL_SUCCESS;
	tParam* params = (tParam*) buf;

    if (params->paramLen == 1)
	{
		GET_PARAM_NEXT(BYTE, params, _parmsToChange);
		parmsToChange = _parmsToChange;
	}		
    else
    	RETURN_ERR(WL_FAILURE)

    INFO_SPI("%p numParam=%d parmsToChange=%d\n", ctx, numParam, parmsToChange);

    if (parmsToChange <= MAX_DNS_CONFIG_PARAMS)
    {
		int i=0;
    	for (; i<parmsToChange; ++i)
    	{
    	    if (params->paramLen == 4)
    	    {
				GET_PARAM_NEXT(LONG, params, _ip_addr);
				lwip_addr.addr = _ip_addr;
				INFO_SPI("%d] nif:%p lwip_addr=0x%x\n", i, nif, lwip_addr.addr);
				dns_setserver(i, &lwip_addr);
    	    }else{
    	    	RETURN_ERR(WL_FAILURE)
    	    }
    	}
    	/* Disable DHCP */
    	ncfg->dhcp_enabled = STATIC_IP_CONFIG;
    }else
    	RETURN_ERR(WL_FAILURE)
		
    RETURN_ERR(err)
}



void set_result(wl_status_t _status)
{
	result = _status;
}


void set_result_cmd(int err) 
{
    wl_err_t _err = (wl_err_t)err;
    switch (_err)
    {
    case WL_SUCCESS:
    	set_result(WL_CONNECTED);
    	ERROR_LED_OFF();
        break;
    default:
    case WL_OOM:
    case WL_INVALID_LENGTH:
    case WL_NOT_SUPPORTED:
    case WL_ABSORBED:
    case WL_RESOURCES:
    case WL_BUSY:
    case WL_RETRY:
    case WL_FAILURE:
    	set_result(WL_CONNECT_FAILED);
    	ERROR_LED_ON();
        break;
    }
    INFO_SPI("%s %d\n", __FUNCTION__, result);
}



extern int ttcp_start(struct ip_addr addr, uint16_t port, void *opaque,
           void *done_cb, int mode, uint16_t nbuf, uint16_t buflen, int udp, int verbose);


int start_server_tcp(uint16_t port, uint8_t sock, uint8_t protMode)
{
	struct ip_addr addr = { 0 };
    uint16_t buflen = 1024;
    uint16_t nbuf = 1024;
    wl_err_t err = WL_FAILURE;

#ifdef _APP_DEBUG_
    int verbose = 1;
#else
    int verbose = 0;
#endif
    int udp = protMode;
    int mode = 1;   //RECEIVE
    void* _ttcp = NULL;

    if (sock >= MAX_SOCK_NUM)
    	return WIFI_SPI_ERR;

    if (_connected)
    {
    	WARN("Still connected...wait\n");
    	return WIFI_SPI_ERR;
    }

    if (!ifStatus)
     {
    	WARN_VER("IF down...wait\n");
     	return WIFI_SPI_ERR;
     }


    if (ard_tcp_start(addr, port, NULL, NULL, mode, nbuf, buflen, udp, verbose, sock, &_ttcp) == 0)
    {
    	INFO_SPI("Start Server %s [%d, %d] OK!\n", ProtMode2Str(protMode), port, sock);
    	setMapSock(sock, _ttcp);
        err = WL_SUCCESS;
    }else{

    	WARN("Start Server %s [%d, %d] FAILED!\n", ProtMode2Str(protMode), port, sock);
    	clearMapSockTcp(sock, TTCP_MODE_RECEIVE);
    }
    return err;
}


int start_server_tcp_cmd_cb(int numParam, char* buf, void* ctx) {
	wl_err_t err = WL_FAILURE;
	tParam* params = (tParam*) buf;
    if (numParam == 3)
    {
    	GET_PARAM_NEXT(INT, params, port);
    	GET_PARAM_NEXT(BYTE, params, sock);
    	GET_PARAM_NEXT(BYTE, params, protMode);
    	err = start_server_tcp(port, sock, protMode);
    }
    return (err==WL_SUCCESS) ? WIFI_SPI_ACK : WIFI_SPI_ERR;
}

int start_client_tcp(uint32_t _addr, uint16_t port, uint8_t sock, uint8_t protMode)
{
    uint16_t buflen = 1024;
    uint16_t nbuf = 1024;
    wl_err_t err = WL_FAILURE;
    struct ip_addr addr = { .addr = _addr};

    INFO_SPI("Addr:0x%x, port:%d, sock:%d, prot:%s\n", _addr, port, sock,  ProtMode2Str(protMode));

	#ifdef _APP_DEBUG_
    int verbose = 1;
	#else
    int verbose = 0;
	#endif

    int udp = protMode;
    int mode = 0;   //TRANSMIT
    void* _ttcp = NULL;

        if (sock >= MAX_SOCK_NUM)
        	return WIFI_SPI_ERR;

    // Check previous connection
	_ttcp = getTTCP(sock, TTCP_MODE_TRANSMIT);
	if (_ttcp != NULL)
	{
		WARN("Previous client %p not stopped !\n", _ttcp);
		ard_tcp_stop(_ttcp);
		clearMapSockTcp(sock, TTCP_MODE_TRANSMIT);
	}

	if (ard_tcp_start(addr, port, NULL, NULL, mode, nbuf, buflen, udp, verbose, sock, &_ttcp) == 0)
	{
		INFO_SPI("Start Client %s %p [0x%x, %d, %d] OK!\n", ProtMode2Str(protMode),
				_ttcp, addr, port, sock);
		setMapSock(sock, _ttcp);
		err = WL_SUCCESS;
	}else{
		INFO_SPI("Start Client %s %p [0x%x, %d, %d] FAILED!\n", ProtMode2Str(protMode), 
				_ttcp, addr, port, sock);
		clearMapSockTcp(sock, TTCP_MODE_TRANSMIT);
	}
	return err;
}


int start_client_tcp_cmd_cb(int numParam, char* buf, void* ctx) {
	wl_err_t err = WL_FAILURE;
	tParam* params = (tParam*) buf;
    if (numParam == 4)
    {
    	GET_PARAM_NEXT(LONG, params, _addr);
    	GET_PARAM_NEXT(INT, params, port);
     	GET_PARAM_NEXT(BYTE, params, sock);
     	GET_PARAM_NEXT(BYTE, params, protMode);
     	err = start_client_tcp(_addr, port, sock, protMode);
    }
    return (err==WL_SUCCESS) ? WIFI_SPI_ACK : WIFI_SPI_ERR;
}

int stop_client_tcp_cmd_cb(int numParam, char* buf, void* ctx) {
	wl_err_t err = WL_FAILURE;
	tParam* params = (tParam*) buf;
	void* _ttcp = NULL;

    if (numParam == 1)
    {
     	GET_PARAM_NEXT(BYTE, params, sock);

        INFO_SPI("Stop client sock:%d\n", sock);

        if (sock < MAX_SOCK_NUM)
        {
        	_ttcp = getTTCP(sock, TTCP_MODE_TRANSMIT);
        	ard_tcp_stop(_ttcp);
            err = WL_SUCCESS;
        }
    }
    return (err==WL_SUCCESS) ? WIFI_SPI_ACK : WIFI_SPI_ERR;
}

int insert_data_cmd_cb(int numParam, char* buf, void* ctx) {

	tDataParam* msg = (tDataParam*) buf;
    if ((numParam == 2)&&(msg->dataLen == 1))
    {
        GET_DATA_BYTE(sock, buf+2);
        GET_DATA_INT(len, buf+3);
        //printk("tcp:%p buf:%p len:%d\n", getTTCP(sock), (uint8_t*)(buf+5), len);
        insertBuf(sock, (uint8_t*)(buf+5), len);
    }
    return WIFI_SPI_ACK;
}

int send_data_udp_cmd_cb(int numParam, char* buf, void* ctx) {
	wl_err_t err = WL_FAILURE;

	tParam* params = (tParam*) buf;
    if ((numParam == 1)&&(params->paramLen == 1))
    {
    	GET_PARAM_NEXT(BYTE, params, sock);
        uint16_t len = 0;
        uint8_t* p = mergeBuf(sock, NULL, &len);
        err = sendUdpData(getTTCP(sock, TTCP_MODE_TRANSMIT), p, len);
		clearBuf(sock);
        free(p);
    }

    return (err==WL_SUCCESS) ? WIFI_SPI_ACK : WIFI_SPI_ERR;
}


int send_data_tcp_cmd_cb(int numParam, char* buf, void* ctx) {
	wl_err_t err = WL_FAILURE;
	DATA_LED_ON();
	tDataParam* msg = (tDataParam*) buf;
    if ((numParam == 2)&&(msg->dataLen == 1))
    {
        GET_DATA_BYTE(sock, buf+2);
        GET_DATA_INT(len, buf+3);
        //printk("tcp:%p buf:%p len:%d\n", getTTCP(sock), (uint8_t*)(buf+5), len);
        err = sendTcpData(getTTCP(sock, TTCP_MODE_TRANSMIT), (uint8_t*)(buf+5), len);
    }
    DATA_LED_OFF();
    return (err==WL_SUCCESS) ? WIFI_SPI_ACK : WIFI_SPI_ERR;
}

int ack_cmd_cb(int numParam, char* buf, void* ctx) {
	return WIFI_SPI_ACK;
}

int get_result_cmd_cb(int numParam, char* buf, void* ctx) {
	INFO_SPI("ifStatus:%d result:%d\n", ifStatus, result);
	return WIFI_SPI_ACK;
}

int disconnect_cmd_cb(int numParam, char* buf, void* ctx) 
{
	return ((wl_disconnect()==WL_SUCCESS)? WIFI_SPI_ACK : WIFI_SPI_ERR);
}


cmd_spi_state_t get_reply_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

    CREATE_HEADER_REPLY(reply, recv, 1);

    reply[3] = 1; // paramLen
	if (ctx != NULL) {
		reply[4] = (*(uint8_t*)ctx); //param
	} else {
		reply[4] = (ifStatus)?WL_CONNECTED:result; //param
	}

    END_HEADER_REPLY(reply, 5, *count);

    //INFO_SPI("result:%d\n", result);
	return SPI_CMD_DONE;
}

cmd_spi_state_t ack_reply_cb(char* recv, char* reply, void* ctx, uint16_t* count) {
     
    CREATE_HEADER_REPLY(reply, recv, 1);

    reply[3] = 1; // paramLen
	if (ctx != NULL) {
		reply[4] = (*(uint8_t*) ctx != 1) ? WIFI_SPI_ERR : WIFI_SPI_ACK; //param
	} else {
		reply[4] = WIFI_SPI_ACK; //param
	}

    END_HEADER_REPLY(reply, 5, *count);

    return SPI_CMD_DONE;
}

cmd_spi_state_t get_reply_ipaddr_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

    CHECK_ARD_NETIF(recv, reply, count);

    CREATE_HEADER_REPLY(reply, recv, 3);

    PUT_LONG_IN_BYTE_NO(ard_netif->ip_addr.addr, reply, 3);
    PUT_LONG_IN_BYTE_NO(ard_netif->netmask.addr, reply, 8);
    PUT_LONG_IN_BYTE_NO(ard_netif->gw.addr, reply, 13);

    END_HEADER_REPLY(reply, 18, *count);

    return SPI_CMD_DONE;
}

void getRemoteData(uint8_t sock, uint8_t mode, tRemoteClient* remoteData)
{
	if ((sock>=0) && (sock<MAX_SOCK_NUM))
	{
		void* p = getTTCP(sock, mode);
		if (p)
		{
			ttcp_t* _ttcp = (ttcp_t* )p;
			if ((_ttcp->udp == UDP_MODE))
			{
				if (_ttcp->mode == TTCP_MODE_RECEIVE)
				{
					remoteData->ipaddr = getRemoteClient(sock)->ipaddr;
					remoteData->port = getRemoteClient(sock)->port;
				}else{
					remoteData->ipaddr = (_ttcp->upcb) ? _ttcp->upcb->remote_ip.addr : 0;
					remoteData->port = (_ttcp->upcb) ? _ttcp->upcb->remote_port : 0;
				}
			}
		}
	}
}


cmd_spi_state_t get_reply_remote_data_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

    CHECK_ARD_NETIF(recv, reply, count);
    DUMP_SPI_CMD(recv);

    GET_DATA_BYTE(sock, recv+4);

    CREATE_HEADER_REPLY(reply, recv, 2);
	tRemoteClient remoteData = {0,0};
	//TODO pass the mode
	getRemoteData(sock, TTCP_MODE_RECEIVE, &remoteData);

    PUT_LONG_IN_BYTE_NO(remoteData.ipaddr, reply, 3);
    PUT_DATA_INT(remoteData.port, reply, 8);

    END_HEADER_REPLY(reply, 11, *count);

    return SPI_CMD_DONE;
}


void foundHostByName(const char *name, struct ip_addr *ipaddr, void *callback_arg)
{
	_hostIpAddr.addr = (ipaddr)?ipaddr->addr:0xffffffff;
	INFO_SPI("foundHostByName: Found Host: name=%s ip=0x%x\n", name, _hostIpAddr.addr);
	hostIpAddrFound = true;
}

int req_reply_host_by_name_cb(int numParam, char* buf, void* ctx) {

    char hostName[DNS_MAX_NAME_LENGTH];
	tParam* params = (tParam*) buf;

    // HostName
	if (params->paramLen < DNS_MAX_NAME_LENGTH) {
		memcpy(hostName, &params->param, params->paramLen);
		hostName[params->paramLen]='\0';
	} else {
		RETURN_ERR(WL_FAILURE)
	}

	INFO_SPI("Looking for Host: name=%s\n", hostName);
	_hostIpAddr.addr = 0;
	hostIpAddrFound = false;
    err_t err = dns_gethostbyname(hostName, &_hostIpAddr, foundHostByName, NULL);
    if (err == ERR_OK)
    {
    	INFO_SPI("Found Host: name=%s ip=0x%x\n", hostName, _hostIpAddr.addr);
    	hostIpAddrFound = true;
		RETURN_ERR(WL_SUCCESS)
    }
	RETURN_ERR(WL_FAILURE)
}

cmd_spi_state_t get_reply_host_by_name_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

	u32_t addr = (hostIpAddrFound)?_hostIpAddr.addr : 0xffffffff;
	INFO_SPI("Searching for Host: ip=0x%x found=%d\n", addr, hostIpAddrFound);

    CHECK_ARD_NETIF(recv, reply, count);

    CREATE_HEADER_REPLY(reply, recv, 1);

    PUT_LONG_IN_BYTE_NO(addr, reply, 3);

    END_HEADER_REPLY(reply, 8, *count);

    return SPI_CMD_DONE;
}

cmd_spi_state_t get_reply_mac_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

    CHECK_ARD_NETIF(recv, reply, count);

    CREATE_HEADER_REPLY(reply, recv, 1);

    reply[3] = WL_MAC_ADDR_LENGTH;
    uint8_t mac[WL_MAC_ADDR_LENGTH];
    if (wl_get_mac_addr(mac) != WL_SUCCESS) {
            RETURN_ERR_REPLY(recv, reply, count);
    }
    //rotate the byte order
    reply[4]=mac[5];
    reply[5]=mac[4];
    reply[6]=mac[3];
    reply[7]=mac[2];
    reply[8]=mac[1];
    reply[9]=mac[0];
    END_HEADER_REPLY(reply, 10, *count);

    return SPI_CMD_DONE;
}

cmd_spi_state_t get_reply_curr_net_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

	uint32_t type = (uint32_t)ctx;
    CHECK_ARD_NETIF(recv, reply, count);

    CREATE_HEADER_REPLY(reply, recv, 1);

    struct wl_network_t* net = wl_get_current_network();
    uint8_t len = 0;
    if (net != NULL)
    {
    	switch (type)
    	{
    	default:
    	case GET_CURR_SSID_CMD:
			{
			len = net->ssid.len;
			PUT_BUFDATA_BYTE(net->ssid.ssid, len, reply, 3);
			break;
			}
    	case GET_CURR_BSSID_CMD:
			{
			len = WL_MAC_ADDR_LENGTH; ;
			PUT_BUFDATA_BYTE_REV(net->bssid.octet, len, reply, 3);
			break;
			}
    	case GET_CURR_RSSI_CMD:
			{
				len=sizeof(net->rssi);
			PUT_LONG_IN_BYTE_HO(net->rssi, reply, 3);
			//printk("RSSI:%d", net->rssi);
			break;
			}
    	case GET_CURR_ENCT_CMD:
			{
				len = sizeof(net->enc_type);
				PUT_DATA_BYTE(net->enc_type, reply, 3);
				//printk("ENCT:%d", net->enc_type);
				break;
			}
    	}
    }else{
    	PUT_DATA_BYTE(0, reply, 3);
    }

    END_HEADER_REPLY(reply, 3+len+1, *count);

    //dump(reply, *count);

    return SPI_CMD_DONE;
}

cmd_spi_state_t get_reply_idx_net_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

	uint32_t type = (uint32_t)ctx;
    CHECK_ARD_NETIF(recv, reply, count);

    CREATE_HEADER_REPLY(reply, recv, 1);

    DUMP_SPI_CMD(recv);

    GET_DATA_BYTE(idx, recv+4);

    if (idx >= WL_NETWORKS_LIST_MAXNUM)
    {
    	WARN("Index out of range: %d\n", idx);
    	return SPI_CMD_DONE;
    }
    uint8_t len = 0;
    switch (type)
    {
    	default:
    	case GET_IDX_SSID_CMD:
			{
				len = network_list.net[idx]->ssid.len;
				PUT_BUFDATA_BYTE(network_list.net[idx]->ssid.ssid, len, reply, 3);
				INFO_UTIL("SSID:%s\n", network_list.net[idx]->ssid.ssid);
				break;
			}
    	case GET_IDX_RSSI_CMD:
			{
				len = 4;
				PUT_LONG_IN_BYTE_HO(network_list.net[idx]->rssi, reply, 3);
				INFO_UTIL("RSSI:%d\n", network_list.net[idx]->rssi);
				break;
			}
    	case GET_IDX_ENCT_CMD:
			{
				len = 1;
				PUT_DATA_BYTE(network_list.net[idx]->enc_type, reply, 3);
				INFO_UTIL("ENCT:%d\n", network_list.net[idx]->enc_type);
				break;
			}
   	}


    END_HEADER_REPLY(reply, 3+len+1, *count);

    DUMP(reply, *count);

    return SPI_CMD_DONE;
}

static void copy_network_list(struct wl_network_list_t *dst,
                              struct wl_network_list_t *src)
{
        int i;
        for (i = 0; i < dst->cnt; i++)
                free(dst->net[i]);
        free(dst->net);

        dst->cnt = 0;

        if (src->cnt == 0)
                return;
        dst->net = calloc(1, src->cnt * sizeof(struct wl_network_t *));
        if (dst->net == NULL) {
                printk("could not allocate all gui net array\n");
                return;
        }

        for (i = 0; i < src->cnt; i++) {
                struct wl_network_t *net = src->net[i];
                dst->net[i] = malloc(sizeof(*net));
                if (dst->net[i] == NULL) {
                        printk("could not allocate all gui nets\n");
                        return;
                }

                memcpy(dst->net[i], net, sizeof(*net));
                dst->cnt++;
        }
}

int start_scan_net_cmd_cb(int numParam, char* buf, void* ctx) {
	wl_err_t err = WL_FAILURE;

	INFO_SPI("Start Network Scan %d\n", numParam);
	if (scanNetCompleted){
		scanNetCompleted = false;
		err = wl_scan();
		if (err != WL_SUCCESS)
		{
			// May be busy scanning already, no fatal error
			WARN("err=%d\n", err);
			err = WL_SUCCESS;
		}
	}
	return err;
}

cmd_spi_state_t get_reply_scan_networks_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

	const int8_t SCAN_NOT_YET_COMPLETED = 0;

	if (!scanNetCompleted)
	{
		//return empty list with an error to retry
	     CREATE_HEADER_REPLY(reply, recv, SCAN_NOT_YET_COMPLETED);
	     END_HEADER_REPLY(reply, 3, *count);
	     INFO_SPI("Scan not completed!\n");
	     return SPI_CMD_DONE;
	}

    int network_cnt = 0;
    struct wl_network_list_t* wl_network_list;

    wl_get_network_list(&wl_network_list);
     if (wl_network_list->cnt == 0)
     {
    	 CREATE_HEADER_REPLY(reply, recv, 0);
    	 END_HEADER_REPLY(reply, 3, *count);
    	 INFO_SPI("Networks not found!\n");
    	 return SPI_CMD_DONE;
     }

     if (wl_network_list->cnt > WL_NETWORKS_LIST_MAXNUM)
     {
    	 network_cnt = WL_NETWORKS_LIST_MAXNUM ;
     }
     else{
    	 network_cnt = wl_network_list->cnt ;
     }

     copy_network_list(&network_list, wl_network_list);
     CREATE_HEADER_REPLY(reply, recv, network_cnt);

     uint8_t start = 3;
     int ii = 0;
     for (; ii < network_cnt; ii++)
     {
    	 uint8_t len = network_list.net[ii]->ssid.len+1;
    	 network_list.net[ii]->ssid.ssid[network_list.net[ii]->ssid.len]=0;
    	 PUT_BUFDATA_BYTE(network_list.net[ii]->ssid.ssid, len, reply, start);
    	 start += len+1;
    	 INFO_SPI("%d - %s [%d]- %d - %d - 0x%x\n",ii, network_list.net[ii]->ssid.ssid,
    			 len, network_list.net[ii]->enc_type,
    			 network_list.net[ii]->rssi, network_list.net[ii]->bssid);
     }

     END_HEADER_REPLY(reply, start, *count);
     //DUMP(reply, *count);

    return SPI_CMD_DONE;
}

cmd_spi_state_t get_state_tcp_cmd_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

    CHECK_ARD_NETIF(recv, reply, count);

    CREATE_HEADER_REPLY(reply, recv, PARAM_NUMS_1);

    uint8_t _state = CLOSED;
    if ((recv[3]==1)&&(recv[4]>=0)&&(recv[4]<MAX_SOCK_NUM))
    {
    	_state = getStateTcp(getTTCP((uint8_t)recv[4], TTCP_MODE_RECEIVE), 0);
    }
    PUT_DATA_BYTE(_state, reply, 3);
    END_HEADER_REPLY(reply, 5, *count);
    INFO_SPI_POLL("state:%d\n", _state);

    return SPI_CMD_DONE;
}

cmd_spi_state_t get_client_state_tcp_cmd_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

    CHECK_ARD_NETIF(recv, reply, count);

    CREATE_HEADER_REPLY(reply, recv, PARAM_NUMS_1);

    uint8_t _state = CLOSED;
    uint8_t _sock = recv[4];
    if ((recv[3]==1)&&(_sock>=0)&&(_sock<MAX_SOCK_NUM))
    {
    	void * p= getTTCP(_sock, TTCP_MODE_TRANSMIT);
    	if (p!=NULL)
    	{
			_state = getStateTcp(p, 1);	
    	}else{
    		WARN_VER("TTCP not found for sock:%d\n", _sock);
    	}
    }
    PUT_DATA_BYTE(_state, reply, 3);
    END_HEADER_REPLY(reply, 5, *count);
    INFO_SPI_POLL("sock:%d state:%d\n", _sock, _state);

    return SPI_CMD_DONE;
}

cmd_spi_state_t avail_data_tcp_cmd_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

	CHECK_ARD_NETIF(recv, reply, count);

	CREATE_HEADER_REPLY(reply, recv, PARAM_NUMS_1);
	uint16_t dataAvail = 0;
    if ((recv[3]==1)&&(recv[4]>=0)&&(recv[4]<MAX_SOCK_NUM))
    {
    	dataAvail = getAvailTcpDataByte((uint8_t)recv[4]);
    }
	PUT_DATA_INT_NO(dataAvail, reply, 3);
	END_HEADER_REPLY(reply, 6, *count);

	INFO_SPI_POLL("dataAvail:%d\n", dataAvail);

    return SPI_CMD_DONE;
}

cmd_spi_state_t test_cmd_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

	static int counter = 0;
	CHECK_ARD_NETIF(recv, reply, count);

	CREATE_HEADER_REPLY(reply, recv, PARAM_NUMS_1);
	PUT_DATA_BYTE(++counter, reply, 3);
	END_HEADER_REPLY(reply, 5, *count);
    return SPI_CMD_DONE;
}

cmd_spi_state_t data_sent_tcp_cmd_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

	CHECK_ARD_NETIF(recv, reply, count);
	SIGN2_DN();
	CREATE_HEADER_REPLY(reply, recv, PARAM_NUMS_1);
	uint8_t dataSent = 0;
    if ((recv[3]==1)&&(recv[4]>=0)&&(recv[4]<MAX_SOCK_NUM))
    {
    	dataSent = isDataSent(getTTCP((uint8_t)recv[4], TTCP_MODE_TRANSMIT));
    }
	PUT_DATA_BYTE(dataSent, reply, 3);
	END_HEADER_REPLY(reply, 5, *count);
	SIGN2_UP();
    return SPI_CMD_DONE;
}

cmd_spi_state_t get_data_tcp_cmd_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

	uint8_t data;

    CHECK_ARD_NETIF(recv, reply, count);

    tParam* params = (tParam*)&recv[3];

    GET_PARAM_NEXT(BYTE, params, _sock);
    GET_PARAM_NEXT(INT, params, _peek);

    if ((recv[3]==1)&&(recv[4]>=0)&&(recv[4]<MAX_SOCK_NUM))
    {
    	SIGN2_DN();

    	if (getTcpDataByte((uint8_t)recv[4], &data, _peek))
    	{
    		CREATE_HEADER_REPLY(reply, recv, PARAM_NUMS_1);
    		PUT_DATA_BYTE(data, reply, 3);
    		END_HEADER_REPLY(reply, 5, *count);
    	}else{
    		CREATE_HEADER_REPLY(reply, recv, PARAM_NUMS_0);
    		END_HEADER_REPLY(reply, 3, *count);
    	}
    	SIGN2_UP();
    }
    return SPI_CMD_DONE;
}

cmd_spi_state_t get_databuf_tcp_cmd_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

	uint8_t* data;
	uint16_t len;

    CHECK_ARD_NETIF(recv, reply, count);

    GET_DATA_BYTE(sock, buf+5);
    if ((sock>=0)&&(sock<MAX_SOCK_NUM))
    {
    	if (getTcpData((uint8_t)sock, (void**)&data, &len))
    	{
    		CREATE_HEADER_REPLY(reply, recv, PARAM_NUMS_1);
    		PUT_BUFDATA_INT(data, len, reply, 3);
    		END_HEADER_REPLY(reply, 3+len+2, *count);
    		freeTcpData((uint8_t)sock);
    	}else{
    		CREATE_HEADER_REPLY(reply, recv, PARAM_NUMS_0);
    		END_HEADER_REPLY(reply, 3, *count);
    	}
    }
    return SPI_CMD_DONE;
}

cmd_spi_state_t get_firmware_version_cmd_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

    CHECK_ARD_NETIF(recv, reply, count);

    CREATE_HEADER_REPLY(reply, recv, 1);

    uint8_t len = strlen(fwVersion);

    PUT_BUFDATA_BYTE(fwVersion, len, reply, 3);

    END_HEADER_REPLY(reply, 3+len+1, *count);

    return SPI_CMD_DONE;
}

cmd_spi_state_t get_test_cmd_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

	uint8_t buffer[255] = {0};

    CHECK_ARD_NETIF(recv, reply, count);

    CREATE_HEADER_REPLY(reply, recv, 1);
    uint8_t len = 0;
    if ((recv[3]==1)&&(recv[4]>=0)&&(recv[4]<0xFF))
    {
    	len = recv[4];
    	int i= 0;
    	for (; i<len; ++i) buffer[i]=i;
    	PUT_BUFDATA_BYTE(buffer, len, reply, 3);
    }else{
    	len = strlen(fwVersion);
    	PUT_BUFDATA_BYTE(fwVersion, len, reply, 3);
    }
    END_HEADER_REPLY(reply, 3+len+1, *count);

    return SPI_CMD_DONE;
}

int sendReply(int cmdIdx, char* recv, char* reply, void* resultCmd)
{
	uint16_t _count = 0;
    int _result = SPI_OK;

    cmd_spi_list[cmdIdx].reply_cb(recv, reply, resultCmd, &_count);
    state = SPI_CMD_REPLING;

    AVAIL_FOR_SPI();
    _result = write_stream(ARD_SPI, &reply[0], _count);
#ifdef _SPI_STATS_
    if ( _result != SPI_OK)
    {
    	statSpi.lastCmd = cmd_spi_list[cmdIdx].cmd_id;
    }
#endif
    BUSY_FOR_SPI();

    IF_SPI_DUMP(printk("==>"));
    DUMP_SPI(recv, count);
    IF_SPI_DUMP(printk("<=="));
	DUMP_SPI(reply, _count);
    replyCount = _count;
    return _result;
}

unsigned char* getStartCmdSeq(unsigned char* _recv, int len, int *offset)
{
	int i = 0;
	*offset = 0;
	//DEB_PIN_UP();
	for (; i<len; ++i)
	{
		if (_recv[i]==START_CMD)
		{
			if (i!=0)
			{
				DEB_PIN_TRIGGER();
				IF_WARN_VER(dump((char*)_recv, (uint16_t)len));
				WARN("%d] Disall. %d/%d cmd:%d\n", cmdCorr, i, len,_recv[i+1]);
			}
			*offset = i;
			return &_recv[i];
		}
	}
	//DEB_PIN_DN();
	WARN("%d] Disall. %d\n", cmdCorr, i);

	return NULL;
}

inline bool spiMsg8(uint8_t cmd)
{
	return ((cmd & DATA_FLAG)==0);
}

int call_reply_cb(char* recv, char* reply) {

//	// check the start of message
//	//TODO CHECK if also the ,en must be resize
//	//char* recv = (char*)getStartCmdSeq((unsigned char*)_recv, &count);
//	char* recv = (char*)getStartCmdSeq((unsigned char*)_recv, count);
//	if (recv == NULL)
//		return REPLY_ERR_MSG;

	unsigned char cmdId = (unsigned char) recv[1];
    uint8_t _result = REPLY_NO_ERR;
	U32 i;
	for (i = 0; i < ARRAY_SIZE(cmd_spi_list); i++) {
		if (cmd_spi_list[i].cmd_id == cmdId) {

			if (cmd_spi_list[i].flags == CMD_SET_FLAG) {
				//Send Reply for SET commands
                 if (sendReply(i, recv, reply, cmd_spi_list[i].ctx) != SPI_OK)
                     return REPLY_ERR_SET;
                 if (spiMsg8(cmdId))
                 {
                	 tSpiMsg* spiMsg = (tSpiMsg*) recv;
                	 _result = cmd_spi_list[i].cb(spiMsg->nParam,
     					(char*) &(spiMsg->params[0]), cmd_spi_list[i].ctx);
           		 }else
           		 {
           			tSpiMsgData* spiMsg = (tSpiMsgData*) recv;
                   	 _result = cmd_spi_list[i].cb(spiMsg->nParam,
         					(char*) &(spiMsg->params[0]), cmd_spi_list[i].ctx);
           		 }

                 if (_result != WIFI_SPI_ACK)
                 	return REPLY_ERR_CMD;
                 else
                 	return REPLY_NO_ERR;
			}else{
				if (spiMsg8(cmdId))
				{
					tSpiMsg* spiMsg = (tSpiMsg*) recv;
					_result = cmd_spi_list[i].cb(spiMsg->nParam,
							(char*) &(spiMsg->params[0]), NULL);
				}else{
					tSpiMsgData* spiMsg = (tSpiMsgData*) recv;
					_result = cmd_spi_list[i].cb(spiMsg->nParam,
							(char*) &(spiMsg->params[0]), NULL);
				}
				//Send Reply for GET commands or Immediate SET apply
				if (cmd_spi_list[i].flags == CMD_GET_FLAG) {
					if (sendReply(i, recv, reply, cmd_spi_list[i].ctx)  != SPI_OK)
						return REPLY_ERR_GET;
					else
						return REPLY_NO_ERR;
				}else if (cmd_spi_list[i].flags == CMD_IMM_SET_FLAG)
				{
					if (sendReply(i, recv, reply, &_result)  != SPI_OK)
						return REPLY_ERR_GET;
					else
						return REPLY_NO_ERR;

				}
			}
		}
	}
	// Command not found
	if (i==ARRAY_SIZE(cmd_spi_list))
	{
		WARN("Unknown cmd 0x%x\n", cmdId);
		DUMP(recv, count);
		return REPLY_ERR_CMD;
	}
	return REPLY_NO_ERR;
}

void init_spi_cmds(void* ctx) {
	spi_add_cmd(SET_NET_CMD, set_net_cmd_cb, ack_reply_cb, NULL, CMD_SET_FLAG);
	spi_add_cmd(SET_PASSPHRASE_CMD, set_passphrase_cmd_cb, ack_reply_cb, NULL, CMD_SET_FLAG);
	spi_add_cmd(SET_KEY_CMD, set_key_cmd_cb, ack_reply_cb, NULL, CMD_SET_FLAG);
	spi_add_cmd(SET_IP_CONFIG_CMD, set_ip_config_cmd_cb, ack_reply_cb, ctx, CMD_SET_FLAG);
	spi_add_cmd(SET_DNS_CONFIG_CMD, set_dns_config_cmd_cb, ack_reply_cb, ctx, CMD_SET_FLAG);
	spi_add_cmd(GET_CONN_STATUS_CMD, get_result_cmd_cb, get_reply_cb, NULL, CMD_GET_FLAG);
	spi_add_cmd(GET_IPADDR_CMD, ack_cmd_cb, get_reply_ipaddr_cb, NULL, CMD_GET_FLAG);
	spi_add_cmd(GET_MACADDR_CMD, ack_cmd_cb, get_reply_mac_cb, NULL, CMD_GET_FLAG);
	spi_add_cmd(GET_CURR_SSID_CMD, ack_cmd_cb, get_reply_curr_net_cb, (void*)GET_CURR_SSID_CMD, CMD_GET_FLAG);
	spi_add_cmd(GET_CURR_BSSID_CMD, ack_cmd_cb, get_reply_curr_net_cb, (void*)GET_CURR_BSSID_CMD, CMD_GET_FLAG);
	spi_add_cmd(GET_CURR_RSSI_CMD, ack_cmd_cb, get_reply_curr_net_cb, (void*)GET_CURR_RSSI_CMD, CMD_GET_FLAG);
	spi_add_cmd(GET_CURR_ENCT_CMD, ack_cmd_cb, get_reply_curr_net_cb, (void*)GET_CURR_ENCT_CMD, CMD_GET_FLAG);
	spi_add_cmd(START_SCAN_NETWORKS, start_scan_net_cmd_cb, ack_reply_cb, NULL, CMD_SET_FLAG);
	spi_add_cmd(SCAN_NETWORKS, ack_cmd_cb, get_reply_scan_networks_cb, NULL, CMD_GET_FLAG);
	spi_add_cmd(DISCONNECT_CMD, disconnect_cmd_cb, ack_reply_cb, NULL, CMD_SET_FLAG);
	spi_add_cmd(GET_IDX_ENCT_CMD, ack_cmd_cb, get_reply_idx_net_cb, (void*)GET_IDX_ENCT_CMD, CMD_GET_FLAG);
	spi_add_cmd(GET_IDX_SSID_CMD, ack_cmd_cb, get_reply_idx_net_cb, (void*)GET_IDX_SSID_CMD, CMD_GET_FLAG);
	spi_add_cmd(GET_IDX_RSSI_CMD, ack_cmd_cb, get_reply_idx_net_cb, (void*)GET_IDX_RSSI_CMD, CMD_GET_FLAG);
	spi_add_cmd(REQ_HOST_BY_NAME_CMD, req_reply_host_by_name_cb, ack_reply_cb, NULL, CMD_SET_FLAG);
	spi_add_cmd(GET_HOST_BY_NAME_CMD, ack_cmd_cb, get_reply_host_by_name_cb, NULL, CMD_GET_FLAG);
	spi_add_cmd(START_SERVER_TCP_CMD, start_server_tcp_cmd_cb, ack_reply_cb, NULL, CMD_SET_FLAG);
	spi_add_cmd(START_CLIENT_TCP_CMD, start_client_tcp_cmd_cb, ack_reply_cb, NULL, CMD_SET_FLAG);
	spi_add_cmd(STOP_CLIENT_TCP_CMD, stop_client_tcp_cmd_cb, ack_reply_cb, NULL, CMD_SET_FLAG);
	spi_add_cmd(GET_STATE_TCP_CMD, ack_cmd_cb, get_state_tcp_cmd_cb, NULL, CMD_GET_FLAG);
	spi_add_cmd(GET_DATA_TCP_CMD, ack_cmd_cb, get_data_tcp_cmd_cb, NULL, CMD_GET_FLAG);
	spi_add_cmd(AVAIL_DATA_TCP_CMD, ack_cmd_cb, avail_data_tcp_cmd_cb, NULL, CMD_GET_FLAG);
	spi_add_cmd(SEND_DATA_TCP_CMD, send_data_tcp_cmd_cb, ack_reply_cb, NULL, CMD_IMM_SET_FLAG);
	spi_add_cmd(DATA_SENT_TCP_CMD, ack_cmd_cb, data_sent_tcp_cmd_cb, NULL, CMD_GET_FLAG);
	spi_add_cmd(GET_DATABUF_TCP_CMD, ack_cmd_cb, get_databuf_tcp_cmd_cb, NULL, CMD_GET_FLAG);
	spi_add_cmd(GET_CLIENT_STATE_TCP_CMD, ack_cmd_cb, get_client_state_tcp_cmd_cb, NULL, CMD_GET_FLAG);
	spi_add_cmd(GET_FW_VERSION_CMD, ack_cmd_cb, get_firmware_version_cmd_cb, NULL, CMD_GET_FLAG);
	spi_add_cmd(GET_TEST_CMD, ack_cmd_cb, get_test_cmd_cb, NULL, CMD_GET_FLAG);
	spi_add_cmd(INSERT_DATABUF_CMD, insert_data_cmd_cb, ack_reply_cb, NULL, CMD_IMM_SET_FLAG);
	spi_add_cmd(SEND_DATA_UDP_CMD, send_data_udp_cmd_cb, ack_reply_cb, NULL, CMD_SET_FLAG);
	spi_add_cmd(GET_REMOTE_DATA_CMD, ack_cmd_cb, get_reply_remote_data_cb, NULL, CMD_GET_FLAG);
}


int checkMsgParam8(unsigned char* buf)
{
	int paramLenTot=0;
	tSpiMsg* spiMsg = (tSpiMsg*)buf;
	tParam	*param = spiMsg->params;
	int i=0;
	for (; i<spiMsg->nParam; ++i)
	{
		uint8_t _len = param->paramLen;
		paramLenTot+= _len+1;
		//printk("%d) len:0x%x\n", i, _len);
		param = (tParam*)((char*)(param)+_len+1);
	}
	return paramLenTot;
}

int checkMsgParam16(unsigned char* buf)
{
	int paramLenTot=0;
	tSpiMsgData* spiMsg = (tSpiMsgData*)buf;
	tDataParam* param = (tDataParam*)spiMsg->params;
	int i=0;
	for (; i<spiMsg->nParam; ++i)
	{
		uint16_t _len = param->dataLen;
		paramLenTot+= _len+sizeof(param->dataLen);
		//printk("%d) len:0x%x\n", i, _len);
		param = (tDataParam*)((char*)(param)+_len+sizeof(param->dataLen));
	}
	return paramLenTot;
}

bool checkMsgFormat(uint8_t* _recv, int len, int* offset)
{

	unsigned char* recv = getStartCmdSeq(_recv, len, offset);
	if ((recv == NULL)||(recv!=_recv))
	{
		DEB_PIN_TRIGGER();

		IF_WARN_VER(DUMP((char*)_recv, len));

		STATSPI_DISALIGN_ERROR();

		if (recv == NULL)
			return false;
	}
	tSpiMsg* spiMsg = (tSpiMsg*) recv;
	if ((spiMsg->cmd == START_CMD)&&((spiMsg->tcmd & REPLY_FLAG) == 0))
	{
		int paramLenTot = 0;
		if (spiMsg8(spiMsg->tcmd))
			paramLenTot = checkMsgParam8(recv);
		else
		{
			DUMP_SPI(_recv, len);
			paramLenTot = checkMsgParam16(recv);
		}

		//INFO_SPI("cmd:0x%x TotLen:%d\n", spiMsg->tcmd, paramLenTot);
		char* p = (char*)recv + paramLenTot + sizeof(tSpiHdr);
		if (*p == END_CMD)
		{
			return true;
		}else{
			WARN("%d] Not found end cmd: 0x%x\n", cmdCorr, *p);
		}
	}
	return false;
}

//#define AVR32_USART_CSR_ITERATION_MASK (UNDERRUN)            0x00000400
//#define AVR32_USART_CSR_OVRE_MASK                            0x00000020
//#define AVR32_USART_CSR_RXRDY_MASK                           0x00000001


void spi_poll(struct netif* netif) {

    ard_netif = netif;

    if (startReply)
	{
		startReply = false;
		int offset = 0;
		DISABLE_SPI_INT();
		if (checkMsgFormat(_receiveBuffer, receivedChars, &offset))
		{
			state = SPI_CMD_INPROGRESS;
			count = receivedChars-offset;
			if (count >= CMD_MAX_LEN)
				count = CMD_MAX_LEN;
			memcpy(buf, &_receiveBuffer[offset], count);

			//mark as buffer used
			_receiveBuffer[0] = 0;

			int err = call_reply_cb(buf, &reply[0]);
			if (err != REPLY_NO_ERR)
			{
				DUMP_SPI(buf, count);
				DUMP_SPI(reply, replyCount);
			}
			receivedChars = 0;
			count = 0;
			state = SPI_CMD_IDLE;
		}
		else
		{
			sendError();
			WARN("%d] Check format msg failed!\n", cmdCorr);
			IF_WARN_VER(dump((char*)_receiveBuffer, receivedChars));
			state = SPI_CMD_IDLE;
			count=0;
			//mark as buffer used
			_receiveBuffer[0] = 0;
		}
		CLEAR_SPI_INT();
		//Enable Spi int to receive a new command
		ENABLE_SPI_INT();
		//Available for receiving a new spi data
	    AVAIL_FOR_SPI();
	}

#ifdef _SPI_STATS_
    if (statSpi.lastError != 0)
    {
    	WARN("%d] Errot=0x%x spiStatus:0x%x\n", cmdCorr, statSpi.lastError, statSpi.status);
    	statSpi.lastError = 0;
    }
#endif
}

inline int spi_slaveReceiveInt(volatile avr32_spi_t *spi)
{
	receivedChars=0;
	int index = 0;
	int err = SPI_OK;
	state = SPI_CMD_INPUT;
	bool endOfFrame = false;

	do {
		unsigned int timeout = SPI_TIMEOUT;
		err = SPI_OK;

		while ((spi->sr & (AVR32_SPI_SR_RDRF_MASK | AVR32_SPI_SR_TXEMPTY_MASK)) !=
				(AVR32_SPI_SR_RDRF_MASK | AVR32_SPI_SR_TXEMPTY_MASK)) {
			if ((timeout--)==0) {
				err=SPI_ERROR_TIMEOUT;
				break;
			}
		}
		//DEB_PIN_TG();
		#if 0
#ifdef _SPI_STATS_		
		if (spi->sr & AVR32_SPI_SR_OVRES_MASK)
		{
			STATSPI_OVERRIDE_ERROR();
		}
#endif	
#endif					
		if (err == SPI_OK) {
			_receiveBuffer[index] = (spi->rdr >> AVR32_SPI_RDR_RD_OFFSET) & 0x00ff;
			DEB_PIN_UP(2);
			if ((index==0) && (_receiveBuffer[index] != START_CMD))
				DEB_PIN_TRIGGER();
			++index;
			++receivedChars;
		}else{
#ifdef _SPI_STATS_
			STATSPI_TIMEOUT_ERROR();
#endif
			break;
		}

		/* break on buffer overflow */
		if (receivedChars >= _BUFFERSIZE) {
			err = SPI_ERROR_OVERRUN_AND_MODE_FAULT;
			break;
		}

		if (_receiveBuffer[index - 1] == END_CMD)
		{
			int8_t numParams = 0;
			int idx = PARAM_LEN_POS+1;
			bool islen16bit = ((_receiveBuffer[CMD_POS] & DATA_FLAG) == DATA_FLAG);
			if (index >= idx)
			{
				numParams = _receiveBuffer[PARAM_LEN_POS];					
				while (((index-1) > idx)&&(numParams>0))
				{
					if (islen16bit)
						idx += (_receiveBuffer[idx]<<8) + _receiveBuffer[idx+1]+2;
					else
						idx += _receiveBuffer[idx]+1;
					--numParams;
				}
				if (((index-1) == idx) && (numParams == 0))
					endOfFrame = true;
			}
			if (!endOfFrame){
				WARN("Wrong termination index:%d nParam:%d idx:%d 16bit:%d\n", index, numParams, idx, islen16bit);
				#ifdef _DEBUG_
					dump((char*)_receiveBuffer, receivedChars); 
					while(0);
				#endif
			}		
		}
	} while (!endOfFrame);
	return err;
}

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
static void spi_int_handler(void)
{
	volatile avr32_spi_t *spi = ARD_SPI;
	DEB_PIN_DN(2);
	DISABLE_SPI_INT();

	if ((spi->sr & AVR32_SPI_SR_RDRF_MASK) != 0)
	{
		int err = spi_slaveReceiveInt(ARD_SPI);
        if (err == SPI_OK)
        {
        	BUSY_FOR_SPI();
        	startReply=true;
        	++cmdCorr;
        	//maintain disable interrupt to send the reply command
        	return;
        }
   	}
	ENABLE_SPI_INT();
}

inline spi_status_t spi_read8(volatile avr32_spi_t *spi, unsigned char *data)
{
  unsigned int timeout = SPI_TIMEOUT;

  while ((spi->sr & (AVR32_SPI_SR_RDRF_MASK | AVR32_SPI_SR_TXEMPTY_MASK)) !=
         (AVR32_SPI_SR_RDRF_MASK | AVR32_SPI_SR_TXEMPTY_MASK)) {
    if (!timeout--) {
      return SPI_ERROR_TIMEOUT;
    }
  }

  *data = (spi->rdr >> AVR32_SPI_RDR_RD_OFFSET) & 0x00ff;

  return SPI_OK;
}


/*!
 * \brief Interrupt handler of the External interrupt line "1".
 */
#if __GNUC__
__attribute__((__interrupt__))
#elif __ICCAVR32__
__interrupt
#endif
static void eic_int_handler1(void)
{
  eic_clear_interrupt_line(&AVR32_EIC, EXT_INT_LINE1);
  startRecvCmdSignal = TRUE;
}

//! Structure holding the configuration parameters of the EIC module.
eic_options_t eic_options[EXT_INT_NB_LINES];

void initExtInt()
{
	  // Enable edge-triggered interrupt.
	  eic_options[0].eic_mode   = EIC_MODE_EDGE_TRIGGERED;
	  // Interrupt will trigger on falling edge.
	  eic_options[0].eic_edge  = EIC_EDGE_FALLING_EDGE;
	  // Initialize in synchronous mode : interrupt is synchronized to the clock
	  eic_options[0].eic_async  = EIC_SYNCH_MODE;
	  // Set the interrupt line number.
	  eic_options[0].eic_line   = EXT_INT_LINE1;

	  // Disable all interrupts.
	  Disable_global_interrupt();

	  INTC_register_interrupt(&eic_int_handler1, EXT_INT_IRQ_LINE1, AVR32_INTC_INT0);

	  // Map the interrupt lines to the GPIO pins with the right peripheral functions.
	  gpio_enable_module_pin(EXT_INT_PIN_LINE1,EXT_INT_FUNCTION_LINE1);

	  // Init the EIC controller with the options
	  eic_init(&AVR32_EIC, eic_options, EXT_INT_NB_LINES);

	  // Enable the chosen lines and their corresponding interrupt feature.
	  eic_enable_line(&AVR32_EIC, eic_options[0].eic_line);
	  eic_enable_interrupt_line(&AVR32_EIC, eic_options[0].eic_line);

	  // Enable all interrupts.
	  Enable_global_interrupt();
}

int initSpi(void* ctx)
{
	volatile avr32_spi_t *spi = &AVR32_SPI0;
	gpio_map_t spi_piomap = {          \
	    {AVR32_SPI0_SCK_0_0_PIN, AVR32_SPI0_SCK_0_0_FUNCTION},  \
	    {AVR32_SPI0_MISO_0_0_PIN, AVR32_SPI0_MISO_0_0_FUNCTION},  \
	    {AVR32_SPI0_MOSI_0_0_PIN, AVR32_SPI0_MOSI_0_0_FUNCTION},  \
	    {AVR32_SPI0_NPCS_0_0_PIN, AVR32_SPI0_NPCS_0_0_FUNCTION},  \
	  };

	INFO_INIT("SPI init...\n");

	/* Init PIO */
	gpio_enable_module(spi_piomap, ARRAY_SIZE(spi_piomap));

	spi_options_t spiOptions;

	spiOptions.reg = 0;
	spiOptions.baudrate = SPI_SLAVE_SPEED;
	spiOptions.bits = SPI_BITS;
	spiOptions.spck_delay = 0;
	spiOptions.trans_delay = 4;
	spiOptions.stay_act = 0;
	spiOptions.spi_mode = 0;
	spiOptions.modfdis = 0;

	/* Initialize as slave; bits, spi_mode */
	if (spi_initSlave(spi, spiOptions.bits, spiOptions.spi_mode) != SPI_OK)
	{
		INFO_SPI("SPI initialization failed!");
		return 1;
	}

	spi_status_t status = spi_setupChipReg(spi, &spiOptions, FPBA_HZ);
	if (status == SPI_ERROR_ARGUMENT)
		WARN("Error configuring SPI\n");

	// Disable all interrupts.
	Disable_global_interrupt();

    // Register the SPI interrupt handler to the interrupt controller.
    INTC_register_interrupt((__int_handler)(&spi_int_handler), AVR32_SPI0_IRQ, AVR32_INTC_INT0);

    // Enable all interrupts.
	Enable_global_interrupt();

    ENABLE_SPI_INT();

	spi_enable(spi);
#ifdef _SPI_STATS_
	initStatSpi();
#endif
	init_spi_cmds(ctx);

	memset(_receiveBuffer, 0, sizeof(_receiveBuffer));
	memset(buf, 0, sizeof(buf));
	memset(reply, 0, sizeof(reply));

	initMapSockTcp();
	set_result(WL_IDLE_STATUS);

	init_pBuf();

	return 0;
}

