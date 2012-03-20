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
#include "clocks.h"
#include "debug.h"
#include "delay.h"
#include "eic.h"
#include "timer.h"
#include "lwip/dns.h"



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

uint8_t counter = 0;

extern void tcp_debug_print_pcbs(void);

static char buf[CMD_MAX_LEN];
static char reply[REPLY_MAX_LEN];
static uint16_t count = 0;
static uint16_t replyCount = 0;
static cmd_spi_state_t state = SPI_CMD_IDLE;
int receivedChars = 0;
#define  _BUFFERSIZE 100
static uint8_t _receiveBuffer[_BUFFERSIZE];
bool startReply = false;
bool end_write = false;	//TODO only for debug

// Signal indicating a new command is coming from SPI interface
static volatile Bool startRecvCmdSignal = FALSE;

#define MAX_CMD_NUM 30
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
}

void printStatSpi()
{
	printk("lastCmd  \t: 0x%x\n", statSpi.lastCmd);
	printk("lastErr  \t: 0x%x\n", statSpi.lastError);
	printk("spiStatus\t: 0x%X\n", statSpi.status);
	printk("spiTxErr \t: 0x%x\n", statSpi.txErr);
	printk("spiRxErr \t: 0x%x\n", statSpi.rxErr);
	printk("spiTmoErr\t: 0x%x\n", statSpi.timeoutErr);
	printk("spiTmoIntErr\t: 0x%x\n", statSpi.timeoutIntErr);
	printk("wrongFrame\t: 0x%x\n", statSpi.wrongFrame);
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

tSpiMsg spiMsgBuf[MAX_CMD_PIPE_SIZE];
unsigned char indexCmdPipe = 0;
int result = WL_CONNECT_FAILED; //Store the result of the last operation

void* mapSockTCP[MAX_SOCK_NUM];

struct netif* ard_netif = NULL;

// Network list retrived in the last scanNetwork
static struct wl_network_t network_list[WL_NETWORKS_LIST_MAXNUM];

struct ip_addr _hostIpAddr;

void* getTTCP(uint8_t sock)
{
	if (sock < MAX_SOCK_NUM)
		return mapSockTCP[sock];
	return NULL;
}

int getSock(void * _ttcp)
{
	int i = 0;
	for (; i<MAX_SOCK_NUM; i++)
	{
		if (_ttcp == mapSockTCP[i])
			return i;
	}
	return -1;
}

void clearMapSockTcp(uint8_t sock)
{
	if (sock < MAX_SOCK_NUM)
		mapSockTCP[sock] = NULL;
}

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
			bytes / ms, bytes % ms, ttcp->udp ? "udp" : "tcp", ttcp->mode
					== TTCP_MODE_TRANSMIT ? "tx" : "rx");
}


void showTTCPstatus()
{
	int i = 0;
	for (; i<MAX_SOCK_NUM; i++)
	{
		void* p = mapSockTCP[i];
		if (p)
		{
			ttcp_t* _ttcp = (ttcp_t* )p;
			printk("Socket n.:%d addr:0x%x port:%d\n", i, _ttcp->addr, _ttcp->port);
			if (_ttcp->tpcb){
				printk("[tpcp]-Status:%d\n", _ttcp->tpcb->state);
			}
			if (_ttcp->lpcb){
				printk("[tlcp]-Status:%d\n", _ttcp->lpcb->state);
			}
			ard_tcp_print_stats(_ttcp);
		}
	}

	tcp_debug_print_pcbs();
}


inline spi_status_t myspi_read(volatile avr32_spi_t *spi, unsigned short *data)
{
	if (spi->sr & AVR32_SPI_SR_RDRF_MASK)
		*data = spi->rdr >> AVR32_SPI_RDR_RD_OFFSET;
  return SPI_OK;
}

inline spi_status_t myspi_write(volatile avr32_spi_t *spi, unsigned short data)
{
  unsigned int timeout = SPI_TIMEOUT;

  while (!(spi->sr & AVR32_SPI_SR_TDRE_MASK)) {
    if (!timeout--) {
      return SPI_ERROR_TIMEOUT;
    }
  }

  spi->tdr = data << AVR32_SPI_TDR_TD_OFFSET;

  return SPI_OK;
}


int write_stream(volatile avr32_spi_t *spi, const char *stream, uint16_t len)
{
	uint16_t _len = 0;
	unsigned short dummy=0;
    bool streamExit = false;

     	do {
             if (*stream == END_CMD)
                 streamExit = true;

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
    			unsigned long data = ARD_SPI->rdr;
    			data = data;
    		}
    		//SIGN1_UP();

//    		if (myspi_read(spi, &dummy) == SPI_ERROR_TIMEOUT)
//    		{
//    			statSpi.timeoutErr++;
//    			statSpi.rxErr++;
//    			statSpi.lastError = SPI_ERROR_TIMEOUT;
//    			statSpi.status = spi_getStatus(spi);
//    			return SPI_ERROR_TIMEOUT;
//    		}
	}while ((!streamExit)&&(_len++ <= len));
 		end_write=true;

     	//while (spi_readRegisterFullCheck(spi));
 		spi_read(spi,&dummy);
     	if ((ARD_SPI->sr & AVR32_SPI_SR_RDRF_MASK) != 0)
     	{
     		int a = 0;
     		if ((ARD_SPI->sr & AVR32_SPI_SR_RDRF_MASK) != 0)
     			a = rand();
     		a=a+5;
     	}

	if (!streamExit)
	{
#ifdef _SPI_STATS_
		statSpi.wrongFrame++;
		statSpi.lastError = SPI_ERROR_ARGUMENT;
#endif
		return SPI_ERROR_ARGUMENT;
	}
	if ((ARD_SPI->sr & AVR32_SPI_SR_RDRF_MASK) != 0)
	     	{
		//SIGN1_DN();
	     	}
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

}

#define ENABLE_SPI_INT() do {											\
    Bool global_interrupt_enabled = Is_global_interrupt_enabled();	\
    if (global_interrupt_enabled) Disable_global_interrupt();		\
    ARD_SPI->IER.rdrf = 1;											\
    ARD_SPI->IER.rxbuff = 1;	ARD_SPI->IER.endrx = 1;										\
    if (global_interrupt_enabled) Enable_global_interrupt();		\
}while(0);

#define DISABLE_SPI_INT() do {										\
    Bool global_interrupt_enabled = Is_global_interrupt_enabled();	\
    if (global_interrupt_enabled) Disable_global_interrupt();		\
    ARD_SPI->IDR.rdrf = 1; ARD_SPI->IDR.rxbuff = 1;	ARD_SPI->IDR.endrx = 1;										\
    if (global_interrupt_enabled) Enable_global_interrupt();		\
}while(0);

#define CLEAR_SPI_INT() do {	\
		eic_clear_interrupt_line(&AVR32_EIC, AVR32_SPI0_IRQ);	\
	}while(0);

void dump(char* _buf, uint16_t _count) {
#ifdef _APP_DEBUG_
	int i;
	for (i = 0; i < _count; ++i)
		printk("0x%x ", _buf[i]);
	printk("\n");
#endif
}

#ifdef _APP_DEBUG_
#define DUMP_SPI_DATA(BUF, COUNT) do {		\
	if (verboseDebug & INFO_SPI_FLAG) {		\
	int i = 0;								\
	for (; i < COUNT; ++i) 					\
	{										\
		printk("0x%x ", BUF[i]);			\
		if (i % 20 == 0)					\
			printk("\n");					\
	}										\
	printk("\n");							\
	}										\
}while(0);
#else
#define DUMP_SPI_DATA(BUF, COUNT) do {}while(0);
#endif


#ifdef _APP_DEBUG_
#define DUMP_SPI_CMD(BUF) do {				\
	if (verboseDebug & INFO_SPI_FLAG) {		\
	int i = 0;								\
	for (; i < CMD_MAX_LEN; ++i) 			\
	{										\
		printk("0x%x ", BUF[i]);			\
		if (BUF[i] == END_CMD)				\
			break;							\
	}										\
	printk("\n");							\
	}										\
}while(0);
#else
#define DUMP_SPI_CMD(BUF) do {}while(0);
#endif



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
#ifdef _APP_DEBUG_
    printk("KEY IDX = %d\n", idx);
    dump(key, len);
    printk("KEY len %d\n", len);
#endif
    memset(&bssid.octet, 0xff, sizeof bssid.octet);

    wl_add_wep_key(idx, len, key, &bssid);
    wl_set_auth_mode(AUTH_MODE_SHARED_KEY);
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

int start_server_tcp_cmd_cb(int numParam, char* buf, void* ctx) {
	wl_err_t err = WL_FAILURE;
	tParam* params = (tParam*) buf;
    if (numParam == 2)
    {
    	GET_PARAM_NEXT(INT, params, port);
    	GET_PARAM_NEXT(BYTE, params, sock);

    	struct ip_addr addr = { 0 };
        uint16_t buflen = 1024;
        uint16_t nbuf = 1024;
#ifdef _APP_DEBUG_
        int verbose = 1;
#else
        int verbose = 0;
#endif
        int udp = 0;
        int mode = 1;   //RECEIVE
        void* _ttcp = NULL;

        if (sock >= MAX_SOCK_NUM)
        	return WIFI_SPI_ERR;

        INFO_SPI("Start Server [%d, %d]\n", port, sock);
        if (ard_tcp_start(addr, port, NULL, NULL, mode, nbuf, buflen, udp, verbose, sock, &_ttcp) == 0)
        {
        	if (sock < MAX_SOCK_NUM)
        		mapSockTCP[sock]=_ttcp;
        	INFO_SPI("Map [%d, %p]\n", sock, _ttcp);
            err = WL_SUCCESS;
        }
    }
    return (err==WL_SUCCESS) ? WIFI_SPI_ACK : WIFI_SPI_ERR;
}

int start_client_tcp_cmd_cb(int numParam, char* buf, void* ctx) {
	wl_err_t err = WL_FAILURE;
	tParam* params = (tParam*) buf;
    if (numParam == 3)
    {
    	GET_PARAM_NEXT(LONG, params, _addr);
    	GET_PARAM_NEXT(INT, params, port);
     	GET_PARAM_NEXT(BYTE, params, sock);

        INFO_SPI("Addr:0x%x, port:%d, sock:%d\n", _addr, port, sock);

        uint16_t buflen = 1024;
        uint16_t nbuf = 1024;
    	struct ip_addr addr = { .addr = _addr};
#ifdef _APP_DEBUG_
        int verbose = 1;
#else
        int verbose = 0;
#endif
        int udp = 0;
        int mode = 0;   //TRANSMIT
        void* _ttcp = NULL;

        if (sock >= MAX_SOCK_NUM)
        	return WIFI_SPI_ERR;

        INFO_SPI("Start Client [0x%x, %d, %d]\n", addr, port, sock);
        if (ard_tcp_start((struct ip_addr)addr, port, NULL, NULL, mode, nbuf, buflen, udp, verbose, sock, &_ttcp) == 0)
        {
        	if (sock < MAX_SOCK_NUM)
        		mapSockTCP[sock]=_ttcp;
        	INFO_SPI("Map [%d, %p]\n", sock, _ttcp);
            err = WL_SUCCESS;
        }else{
        	mapSockTCP[sock]=0;
        }
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
        	_ttcp = mapSockTCP[sock];
        	ard_tcp_stop(_ttcp);
        	mapSockTCP[sock]=0;
            err = WL_SUCCESS;
        }
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
        err = sendTcpData(getTTCP(sock), (uint8_t*)(buf+5), len);
    }
    DATA_LED_OFF();
    return (err==WL_SUCCESS) ? WIFI_SPI_ACK : WIFI_SPI_ERR;
}

int ack_cmd_cb(int numParam, char* buf, void* ctx) {
	return WIFI_SPI_ACK;
}

int get_result_cmd_cb(int numParam, char* buf, void* ctx) {
	*buf=result;
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
		reply[4] = result; //param
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

void foundHostByName(const char *name, struct ip_addr *ipaddr, void *callback_arg)
{
	INFO_SPI("foundHostByName: Found Host: name=%s ip=0x%x\n", name, ipaddr->addr);
	_hostIpAddr.addr = ipaddr->addr;
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

	_hostIpAddr.addr = 0;
    err_t err = dns_gethostbyname(hostName, &_hostIpAddr, foundHostByName, NULL);
    if (err == ERR_OK)
    {
    	INFO_SPI("Found Host: name=%s ip=0x%x\n", hostName, _hostIpAddr.addr);
		RETURN_ERR(WL_SUCCESS)
    }
	RETURN_ERR(WL_FAILURE)
}

cmd_spi_state_t get_reply_host_by_name_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

    CHECK_ARD_NETIF(recv, reply, count);

    CREATE_HEADER_REPLY(reply, recv, 1);

    PUT_LONG_IN_BYTE_NO(_hostIpAddr.addr, reply, 3);

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
				len = network_list[idx].ssid.len;
				PUT_BUFDATA_BYTE(network_list[idx].ssid.ssid, len, reply, 3);
				INFO_UTIL("SSID:%s\n", network_list[idx].ssid.ssid);
				break;
			}
    	case GET_IDX_RSSI_CMD:
			{
				len = 4;
				PUT_LONG_IN_BYTE_HO(network_list[idx].rssi, reply, 3);
				INFO_UTIL("RSSI:%d\n", network_list[idx].rssi);
				break;
			}
    	case GET_IDX_ENCT_CMD:
			{
				len = 1;
				PUT_DATA_BYTE(network_list[idx].enc_type, reply, 3);
				INFO_UTIL("ENCT:%d\n", network_list[idx].enc_type);
				break;
			}
   	}


    END_HEADER_REPLY(reply, 3+len+1, *count);

    dump(reply, *count);

    return SPI_CMD_DONE;
}


cmd_spi_state_t get_reply_scan_networks_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

	INFO_SPI("netif:0x%x\n", ard_netif);
	CHECK_ARD_NETIF(recv, reply, count);

    int network_cnt = 0;

    struct wl_network_t* wl_network_list;
    uint8_t wl_network_cnt;

     wl_get_network_list(&wl_network_list, &wl_network_cnt);
     if (wl_network_cnt == 0)
     {
    	 CREATE_HEADER_REPLY(reply, recv, 0);
    	 END_HEADER_REPLY(reply, 3, *count);
    	 return SPI_CMD_DONE;
     }

     if (wl_network_cnt > WL_NETWORKS_LIST_MAXNUM)
     {
    	 network_cnt = WL_NETWORKS_LIST_MAXNUM ;
     }
     else{
    	 network_cnt = wl_network_cnt ;
     }

     memcpy(network_list, wl_network_list,
            sizeof(struct wl_network_t) * network_cnt);

     CREATE_HEADER_REPLY(reply, recv, network_cnt);

     uint8_t start = 3;
     int ii = 0;
     for (; ii < network_cnt; ii++)
     {
    	 uint8_t len = network_list[ii].ssid.len;
    	 PUT_BUFDATA_BYTE(network_list[ii].ssid.ssid, len, reply, start);
    	 start += len+1;
    	 INFO_SPI("%d - %s - %d - %d - 0x%x\n",ii, network_list[ii].ssid.ssid, network_list[ii].enc_type,
    			 network_list[ii].rssi, network_list[ii].bssid);
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
    	_state = getStateTcp(mapSockTCP[(uint8_t)recv[4]], 0);
    }
    PUT_DATA_BYTE(_state, reply, 3);
    END_HEADER_REPLY(reply, 5, *count);

    return SPI_CMD_DONE;
}

cmd_spi_state_t get_client_state_tcp_cmd_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

    CHECK_ARD_NETIF(recv, reply, count);

    CREATE_HEADER_REPLY(reply, recv, PARAM_NUMS_1);

    uint8_t _state = CLOSED;
    if ((recv[3]==1)&&(recv[4]>=0)&&(recv[4]<MAX_SOCK_NUM))
    {
    	void * p= mapSockTCP[(uint8_t)recv[4]];
    	// get if we are in server or Transmit mode (0)
    	if (getModeTcp(p) == TTCP_MODE_TRANSMIT)
    	{

    		_state = getStateTcp(p, 1);
    		INFO_TCP("p=%p _ttcp=%p state:%d\n",
    				p, ((struct ttcp*) p)->tpcb, _state);
    	}else {
    		_state = getStateTcp(p, 1);
    	}
    }
    PUT_DATA_BYTE(_state, reply, 3);
    END_HEADER_REPLY(reply, 5, *count);

    return SPI_CMD_DONE;
}


cmd_spi_state_t avail_data_tcp_cmd_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

	CHECK_ARD_NETIF(recv, reply, count);

	CREATE_HEADER_REPLY(reply, recv, PARAM_NUMS_1);
	uint8_t dataAvail = 0;
    if ((recv[3]==1)&&(recv[4]>=0)&&(recv[4]<MAX_SOCK_NUM))
    {
    	dataAvail = isAvailTcpDataByte((uint8_t)recv[4]) ? 1 : 0;
    }
	PUT_DATA_BYTE(dataAvail, reply, 3);
	END_HEADER_REPLY(reply, 5, *count);
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



/*
cmd_spi_state_t ack_data_sent_reply_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

	CHECK_ARD_NETIF(recv, reply, count);

	CREATE_HEADER_REPLY(reply, recv, PARAM_NUMS_1);
	uint8_t dataSent = 0;
    if ((recv[3]==1)&&(recv[4]>=0)&&(recv[4]<MAX_SOCK_NUM))
    {
    	int timeout = 0;
    	do {
    		dataSent = isDataSent(mapSockTCP[(uint8_t)recv[4]]);
    	}while ((dataSent == 0)&&(timeout++ < TIMEOUT_SENT_REPLY));
    }
	PUT_DATA_BYTE(dataSent, reply, 3);
	END_HEADER_REPLY(reply, 5, *count);
    return SPI_CMD_DONE;
}
*/

cmd_spi_state_t data_sent_tcp_cmd_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

	CHECK_ARD_NETIF(recv, reply, count);
	SIGN2_DN();
	CREATE_HEADER_REPLY(reply, recv, PARAM_NUMS_1);
	uint8_t dataSent = 0;
    if ((recv[3]==1)&&(recv[4]>=0)&&(recv[4]<MAX_SOCK_NUM))
    {
    	dataSent = isDataSent(mapSockTCP[(uint8_t)recv[4]]);
    }
	PUT_DATA_BYTE(dataSent, reply, 3);
	END_HEADER_REPLY(reply, 5, *count);
	SIGN2_UP();
    return SPI_CMD_DONE;
}


cmd_spi_state_t get_data_tcp_cmd_cb(char* recv, char* reply, void* ctx, uint16_t* count) {

	uint8_t data;

    CHECK_ARD_NETIF(recv, reply, count);

    if ((recv[3]==1)&&(recv[4]>=0)&&(recv[4]<MAX_SOCK_NUM))
    {
    	SIGN2_DN();
    	if (getTcpDataByte((uint8_t)recv[4], &data))
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

    if ((recv[3]==1)&&(recv[4]>=0)&&(recv[4]<MAX_SOCK_NUM))
    {
    	if (getTcpData((uint8_t)recv[4], (void**)&data, &len))
    	{
    		CREATE_HEADER_REPLY(reply, recv, PARAM_NUMS_1);
    		PUT_BUFDATA_INT(data, len, reply, 3);
    		END_HEADER_REPLY(reply, 3+len+2+1, *count);
    		freeTcpData((uint8_t)recv[4]);
    	}else{
    		CREATE_HEADER_REPLY(reply, recv, PARAM_NUMS_0);
    		END_HEADER_REPLY(reply, 3, *count);
    	}
    }
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
    if ( result != SPI_OK)
    {
    	statSpi.lastCmd = cmd_spi_list[cmdIdx].cmd_id;
    }
#endif
    BUSY_FOR_SPI();

    //unsigned char status = spi_getStatus(ARD_SPI);
    //INFO_SPI("Status after write: 0x%x\n",status);

    replyCount = _count;
    return _result;
}

unsigned char* getStartCmdSeq(unsigned char* _recv, int len, int *offset)
{
	int i = 0;
	*offset = 0;
	for (; i<len; ++i)
	{
		if (_recv[i]==START_CMD)
		{
			if (i!=0)
			{
				//WARN("Disall. %d/%d cmd:%d\n", i, len,_recv[i+1]);
				//SIGN2_DN();
			}
			*offset = i;
			return &_recv[i];
		}
	}
	WARN("Disall. %d\n", i);
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
     					(char*) &(spiMsg->params[0]), NULL);
           		 }else
           		 {
           			tSpiMsgData* spiMsg = (tSpiMsgData*) recv;
                   	 _result = cmd_spi_list[i].cb(spiMsg->nParam,
         					(char*) &(spiMsg->params[0]), NULL);
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
		dump(recv, count);
		return REPLY_ERR_CMD;
	}
	return REPLY_NO_ERR;
}

void init_spi_cmds() {
	spi_add_cmd(SET_NET_CMD, set_net_cmd_cb, ack_reply_cb, NULL, CMD_SET_FLAG);
	spi_add_cmd(SET_PASSPHRASE_CMD, set_passphrase_cmd_cb, ack_reply_cb, NULL, CMD_SET_FLAG);
	spi_add_cmd(SET_KEY_CMD, set_key_cmd_cb, ack_reply_cb, NULL, CMD_SET_FLAG);
	spi_add_cmd(GET_CONN_STATUS_CMD, get_result_cmd_cb, get_reply_cb, NULL, CMD_GET_FLAG);
	spi_add_cmd(GET_IPADDR_CMD, ack_cmd_cb, get_reply_ipaddr_cb, NULL, CMD_GET_FLAG);
	spi_add_cmd(GET_MACADDR_CMD, ack_cmd_cb, get_reply_mac_cb, NULL, CMD_GET_FLAG);
	spi_add_cmd(GET_CURR_SSID_CMD, ack_cmd_cb, get_reply_curr_net_cb, (void*)GET_CURR_SSID_CMD, CMD_GET_FLAG);
	spi_add_cmd(GET_CURR_BSSID_CMD, ack_cmd_cb, get_reply_curr_net_cb, (void*)GET_CURR_BSSID_CMD, CMD_GET_FLAG);
	spi_add_cmd(GET_CURR_RSSI_CMD, ack_cmd_cb, get_reply_curr_net_cb, (void*)GET_CURR_RSSI_CMD, CMD_GET_FLAG);
	spi_add_cmd(GET_CURR_ENCT_CMD, ack_cmd_cb, get_reply_curr_net_cb, (void*)GET_CURR_ENCT_CMD, CMD_GET_FLAG);
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
		//LED_On(LED2);
		if (len < 20)	//TODO stamp only short messages wrong
			dump((char*)_recv, len);
		//LED_Off(LED2);
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
			DUMP_SPI_DATA(_recv, len);
			paramLenTot = checkMsgParam16(recv);
		}

		//INFO_SPI("cmd:0x%x TotLen:%d\n", spiMsg->tcmd, paramLenTot);
		char* p = (char*)recv + paramLenTot + sizeof(tSpiHdr);
		if (*p == END_CMD)
		{
			return true;
		}else{
			WARN("Not found end cmd: 0x%x\n", *p);
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
		if (checkMsgFormat(_receiveBuffer, receivedChars, &offset))
		{
			state = SPI_CMD_INPROGRESS;
			count = receivedChars-offset;
			memcpy(buf, &_receiveBuffer[offset], count);

			DISABLE_SPI_INT();
			int err = call_reply_cb(buf, &reply[0]);
			if (err != REPLY_NO_ERR)
			{
				//LED_On(LED1);
				//INFO_SPI("[E(0x%x):%d spiStatus:%d]\n", statSpi.lastCmd, err, statSpi.status);
				DUMP_SPI_DATA(buf, count);
				DUMP_SPI_DATA(reply, replyCount);
				//LED_Off(LED1);
			}
			receivedChars = 0;
			count = 0;
			state = SPI_CMD_IDLE;
	     	if ((ARD_SPI->sr & AVR32_SPI_SR_RDRF_MASK) != 0)
	     	{
	     		unsigned long data = ARD_SPI->rdr;
	     		data = data;
	     		if ((ARD_SPI->sr & AVR32_SPI_SR_RDRF_MASK) != 0)
	     		{
	     			//SIGN1_DN();
	     		}
	     	}
		}
		else
		{
			sendError();
			WARN("Check format msg failed!\n");
			dump((char*)_receiveBuffer, receivedChars);
			state = SPI_CMD_IDLE;
			count=0;
		}
		CLEAR_SPI_INT();
		//Enable Spi int to receive a new command
		ENABLE_SPI_INT();
		//Available for receiving a new spi data
	    AVAIL_FOR_SPI();
	}
}

inline int spi_slaveReceiveInt(volatile avr32_spi_t *spi, bool startRecvd)
{
  receivedChars=0;
  int index = 0;
  int err = SPI_OK;
  state = SPI_CMD_INPUT;
  unsigned int timeout = SPI_TIMEOUT;
  if (startRecvd)
  {
	  TOGGLE_SIG0();
	  _receiveBuffer[index]=START_CMD;
	  ++receivedChars;
	  ++index;
  }
  do {
	err = SPI_OK;
	//SIGN1_DN();
	//spi->tdr = DUMMY_DATA << AVR32_SPI_TDR_TD_OFFSET;
    while ((spi->sr & (AVR32_SPI_SR_RDRF_MASK | AVR32_SPI_SR_TXEMPTY_MASK)) !=
           (AVR32_SPI_SR_RDRF_MASK | AVR32_SPI_SR_TXEMPTY_MASK)) {
      if (!timeout--) {
    	  err=SPI_ERROR_TIMEOUT;
    	  break;
      }
    }
    _receiveBuffer[index] = (spi->rdr >> AVR32_SPI_RDR_RD_OFFSET) & 0x00ff;
    if (_receiveBuffer[index] == START_CMD){
    	TOGGLE_SIG0();
    //SIGN1_UP();
    }
    if (err == SPI_OK) {
        ++index;
        ++receivedChars;
    }else{
#ifdef _SPI_STATS_
    	STATSPI_TIMEOUT_ERROR();
#endif
    	break;
    }

    /* break on buffer overflow */
    if (receivedChars > _BUFFERSIZE) {
		err = SPI_ERROR_OVERRUN_AND_MODE_FAULT;
      break;
    }

  } while (_receiveBuffer[index - 1] != END_CMD);
  return err;
}


uint32_t data = 0;
uint32_t status_register = 0;

#if defined (__GNUC__)
__attribute__((__interrupt__))
#elif defined (__ICCAVR32__)
__interrupt
#endif
static void spi_int_handler(void)
{
	//SIGN2_DN();
	//eic_clear_interrupt_line(&AVR32_EIC, AVR32_SPI0_IRQ);
	AVAIL_FOR_SPI();
	DISABLE_SPI_INT();

	//TODO verify why after the reply write the RDRF is set
	unsigned short dummy = 0;
	if ((end_write)&&((ARD_SPI->sr & AVR32_SPI_SR_RDRF_MASK) != 0)) {
		end_write=false;
		//SIGN1_UP();
		spi_read(ARD_SPI, &dummy);
	}

	if (((ARD_SPI->sr & AVR32_SPI_SR_RDRF_MASK) != 0)||(dummy==START_CMD))
	{
		int err = spi_slaveReceiveInt(ARD_SPI, dummy==START_CMD);
        if (err != SPI_OK)
        {
#ifdef _SPI_STATS_
        	//TODO verify why at the end of cmd cycle RDF bit is high without any data recv.
        	if (statSpi.lastError != SPI_ERROR_TIMEOUT)
        		INFO_SPI("[E(0x%x):%d spiStatus:%d]\n", statSpi.lastError, err, statSpi.status);
#endif
        }else{
        	BUSY_FOR_SPI();
        	startReply=true;
        	//maintain disable interrupt to send the reply command
        	//SIGN2_UP();
        	return;
        }

   	}
	ENABLE_SPI_INT();
	//SIGN2_UP();
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

int initSpi()
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
	init_spi_cmds();

	memset(_receiveBuffer, 0, _BUFFERSIZE);

	init_pBuf();

	return 0;
}

