/*
 * ard_utils.h
 *
 *  Created on: Jul 4, 2010
 *      Author: mlf by Metodo2 srl
 */

#ifndef ARD_UTILS_H_
#define ARD_UTILS_H_

#include "gpio.h"
#include "debug.h"
#include "ARDUINO/arduino.h"
#define INIT_SIGNAL_FOR_SPI() 	gpio_disable_pin_pull_up(ARDUINO_HANDSHAKE_PIN);
#define BUSY_FOR_SPI() 			gpio_set_gpio_pin(ARDUINO_HANDSHAKE_PIN)
#define AVAIL_FOR_SPI() 		gpio_clr_gpio_pin(ARDUINO_HANDSHAKE_PIN)

#define LED0_UP() 				gpio_set_gpio_pin(LED0_GPIO)
#define LED0_DN() 				gpio_clr_gpio_pin(LED0_GPIO)
#define LED0_TL() 				gpio_tgl_gpio_pin(LED0_GPIO)
#define LED1_UP() 				gpio_set_gpio_pin(LED1_GPIO)
#define LED1_DN() 				gpio_clr_gpio_pin(LED1_GPIO)
#define LED1_TL() 				gpio_tgl_gpio_pin(LED1_GPIO)
#define LED2_UP() 				gpio_set_gpio_pin(LED2_GPIO)
#define LED2_DN() 				gpio_clr_gpio_pin(LED2_GPIO)
#define LED2_TL() 				gpio_tgl_gpio_pin(LED2_GPIO)

#ifdef _DEBUG_
#define SIGN0_UP		LED0_UP
#define SIGN0_DN 		LED0_DN
#define SIGN0_TL 		LED0_TL
#define SIGN1_UP 		LED1_UP
#define SIGN1_DN 		LED1_DN
#define SIGN1_TL 		LED1_TL
#define SIGN2_UP 		LED2_UP
#define SIGN2_DN 		LED2_DN
#define SIGN2_TL 		LED2_TL

#define DEB_PIN_UP(X) 			gpio_set_gpio_pin(DEB##X##_PIN_GPIO)
#define DEB_PIN_DN(X) 			gpio_clr_gpio_pin(DEB##X##_PIN_GPIO)
#define DEB_PIN_ENA(X) 			gpio_enable_gpio_pin(DEB##X##_PIN_GPIO)
#define DEB_PIN_TOGGLE(X)		gpio_tgl_gpio_pin(DEB##X##_PIN_GPIO)
#define DEB_PIN_TRIGGER(X)		DEB_PIN_DN(X);  DEB_PIN_UP(X);


#else
#define SIGN0_UP()
#define SIGN0_DN()
#define SIGN0_TL()
#define SIGN1_UP()
#define SIGN1_DN()
#define SIGN1_TL()
#define SIGN2_UP()
#define SIGN2_DN()
#define SIGN2_TL()

#define DEB_PIN_UP(X)
#define DEB_PIN_DN(X)
#define DEB_PIN_ENA(X)
#define DEB_PIN_TOGGLE(X)
#define DEB_PIN_TRIGGER(X)

//#define TOGGLE_SIG0
#endif

#define DELAY_450NS asm volatile("nop")
#define DELAY_1uS  DELAY_450NS; DELAY_450NS;
#define TOGGLE_SIG0()	SIGN0_UP(); DELAY_450NS;SIGN0_DN();


#define LINK_LED_OFF	LED0_UP
#define ERROR_LED_OFF	LED1_UP
#define DATA_LED_OFF	LED2_UP

#define LINK_LED_ON		LED0_DN
#define ERROR_LED_ON	LED1_DN
#define DATA_LED_ON		LED2_DN

#define LINK_LED_BL		LED0_TL
#define ERROR_LED_BL	LED1_TL
#define DATA_LED_BL		LED2_TL


#define CREATE_HEADER_REPLY(REPLY, RECV, NUM_PARAMS)\
    REPLY[0] = RECV[0];                           \
	REPLY[1] = RECV[1] | REPLY_FLAG;            \
	REPLY[2] = NUM_PARAMS;

#define CREATE_HEADER_REPLY_WAIT(REPLY, RECV, NUM_PARAMS)\
    REPLY[0] = RECV[0];                           \
	REPLY[1] = RECV[1] | WAIT_FLAG;            \
	REPLY[2] = NUM_PARAMS;


#define END_HEADER_REPLY(REPLY, TOT_LEN, COUNT)\
    REPLY[TOT_LEN] = END_CMD;           \
    REPLY[TOT_LEN+1] = 0;               \
    COUNT=TOT_LEN+1;

#define RETURN_ERR_REPLY(RECV,REPLY,COUNT)  \
    {uint8_t err = 0; return ack_reply_cb(RECV,REPLY,&err,COUNT);}

#define CHECK_ARD_NETIF(RECV,REPLY,COUNT) \
    if (ard_netif == NULL)  \
    { uint8_t err = 0; return ack_reply_cb(RECV,REPLY,&err,COUNT); }

#define PUT_LONG_IN_BYTE_HO(LONG, BYTE, IDX)   {           \
    uint32_t _long = LONG;                              \
    BYTE[IDX] = 4;                                      \
    BYTE[IDX+1] = (uint8_t)(_long & 0xff);              \
    BYTE[IDX+2] = (uint8_t)((_long & 0xff00)>>8);       \
    BYTE[IDX+3] = (uint8_t)((_long & 0xff0000)>>16);    \
    BYTE[IDX+4] = (uint8_t)((_long & 0xff000000)>>24);  \
}

#define PUT_LONG_IN_BYTE_NO(LONG, BYTE, IDX)   {           \
    uint32_t _long = LONG;                              \
    BYTE[IDX] = 4;                                      \
    BYTE[IDX+4] = (uint8_t)(_long & 0xff);              \
    BYTE[IDX+3] = (uint8_t)((_long & 0xff00)>>8);       \
    BYTE[IDX+2] = (uint8_t)((_long & 0xff0000)>>16);    \
    BYTE[IDX+1] = (uint8_t)((_long & 0xff000000)>>24);  \
}


#define PUT_DATA_INT(INT, BYTE, IDX)   {           		\
    uint16_t _int = INT;								\
	BYTE[IDX] = 2;                            			\
    BYTE[IDX+1] = (uint8_t)((_int & 0xff00)>>8);       	\
    BYTE[IDX+2] = (uint8_t)(_int & 0xff);              	\
}

#define PUT_DATA_INT_NO(INT, BYTE, IDX)   {           		\
    uint16_t _int = INT;								\
	BYTE[IDX] = 2;                            			\
    BYTE[IDX+2] = (uint8_t)((_int & 0xff00)>>8);       	\
    BYTE[IDX+1] = (uint8_t)(_int & 0xff);              	\
}

#define PUT_DATA_BYTE(DATA, BYTE, IDX)   {           	\
    BYTE[IDX] = 1;                                      \
    BYTE[IDX+1] = (uint8_t)DATA;						\
}

#define PUT_BUFDATA_BYTE(BUF, BUFLEN, BYTE, IDX)	{	\
    BYTE[IDX] = (uint8_t)(BUFLEN & 0xff); 			\
	uint16_t i = 0;										\
	for (; i<BUFLEN; ++i)								\
		BYTE[IDX+1+i]=BUF[i];							\
}

#define PUT_BUFDATA_INT(BUF, BUFLEN, BYTE, IDX)	{		\
    BYTE[IDX] = (uint8_t)((BUFLEN & 0xff00)>>8); 		\
    BYTE[IDX+1] = (uint8_t)(BUFLEN & 0xff); 			\
	uint16_t i = 0;										\
	for (; i<BUFLEN; ++i)								\
		BYTE[IDX+2+i]=BUF[i];							\
}


#define PUT_BUFDATA_BYTE_REV(BUF, BUFLEN, BYTE, IDX) {	\
	BYTE[IDX] = (uint8_t)(BUFLEN & 0xff); 				\
	uint16_t i = 0;										\
	for (; i<BUFLEN; ++i)								\
		BYTE[IDX+1+i]=BUF[BUFLEN-i-1];					\
}

#define GET_DATA_LONG(INT32, BUF)	\
		uint32_t INT32 = ((*(BUF))<<24) + ((*(BUF+1))<<16) + ((*(BUF+2))<<8) + (*(BUF+3));

#define GET_DATA_INT(INT16, BUF)	\
		uint16_t INT16 = ((*(BUF))<<8) + (*(BUF+1));

#define GET_DATA_BYTE(BYTE, BUF)	\
		uint8_t BYTE = (*(BUF));

#define CHECK_PARAM_LEN(PARAM, LEN)	((PARAM!=NULL)&&(PARAM->paramLen == LEN))

#define NEXT_PARAM(PARAM)	\
		do {				\
		if (PARAM!=NULL){	\
			PARAM=(tParam*)((uint8_t*)PARAM+PARAM->paramLen+1);	\
			GET_PARAM_BYTE(PARAM, end)							\
			if (end == END_CMD)	WARN("End of cmd params", PARAM); \
		}														\
		}while(0);

#define GET_PARAM_LONG(PARAM, LONG)			\
		uint32_t LONG = 0;					\
		if CHECK_PARAM_LEN(PARAM, 4) { 		\
		tLongParam* s = (tLongParam*)PARAM;	\
		LONG = s->param; 					\
		}

#define GET_PARAM_INT(PARAM, INT)			\
		uint16_t INT = 0;					\
		if CHECK_PARAM_LEN(PARAM, 2) { 		\
		tIntParam* s = (tIntParam*)PARAM;	\
		INT = s->param; 					\
		}

#define GET_PARAM_BYTE(PARAM, BYTE)			\
		uint8_t BYTE = 0;					\
		if CHECK_PARAM_LEN(PARAM, 1) { 		\
		tByteParam* s = (tByteParam*)PARAM;	\
		BYTE = s->param;					\
		}

#define GET_PARAM_NEXT(TYPE, PARAM, DATA)	\
		GET_PARAM_##TYPE(PARAM, DATA)		\
		NEXT_PARAM(PARAM)

#ifdef _SPI_STATS_
#define STATSPI_TIMEOUT_ERROR()		\
		statSpi.timeoutIntErr++;	\
		statSpi.rxErr++;			\
		statSpi.lastError = SPI_TIMEOUT_ERROR;	\
		statSpi.status = spi_getStatus(ARD_SPI);

#define STATSPI_DISALIGN_ERROR()		\
		statSpi.frameDisalign++;	\
		statSpi.rxErr++;			\
		statSpi.lastError = SPI_ALIGN_ERROR;	\
		statSpi.status = spi_getStatus(ARD_SPI);

#define STATSPI_OVERRIDE_ERROR()		\
		statSpi.overrideFrame++;	\
		statSpi.rxErr++;			\
		statSpi.lastError = SPI_OVERRIDE_ERROR;	\
		statSpi.status = spi_getStatus(ARD_SPI);

#define STATSPI_TX_TIMEOUT_ERROR()	\
		statSpi.timeoutErr++;		\
		statSpi.txErr++;			\
		statSpi.lastError = SPI_ERROR_TIMEOUT;	\
		statSpi.status = spi_getStatus(ARD_SPI);
#else
#define STATSPI_TIMEOUT_ERROR()
#define STATSPI_TX_TIMEOUT_ERROR()
#define STATSPI_DISALIGN_ERROR()
#define STATSPI_OVERRIDE_ERROR()
#endif

#define DUMP_TCP_STATE(TTCP) do {\
		int i = getCurrClientConnId(); \
		INFO_TCP("%d] ttcp:%p tpcb:%p state:%d lpcb:%p state:%d left:%d sent:%d\n", \
			i, TTCP, TTCP->tpcb[i], (TTCP->tpcb[i])?TTCP->tpcb[i]->state:0, \
			TTCP->lpcb, (TTCP->lpcb)?TTCP->lpcb->state:0, \
			(TTCP->tpcb[i])?TTCP->left[i]:0, (TTCP->tpcb[i])?TTCP->buff_sent[i]:0); \
			} while(0);
			
#define Mode2Str(_Mode) ((_Mode==0)?"TRANSMIT":"RECEIVE")			
#define ProtMode2Str(_protMode) ((_protMode==0)?"TCP":"UDP")

typedef struct sData
{
	uint8_t*	data;
	uint16_t	len;
	uint16_t	idx;
	void* 	pcb;
}tData;

struct pbuf;

void init_pBuf();

uint8_t* insert_pBuf(struct pbuf* q, uint8_t sock, void* _pcb);

uint8_t* insertBuf(uint8_t sock, uint8_t* buf, uint16_t len);

uint8_t* mergeBuf(uint8_t sock, uint8_t** buf, uint16_t* _len);

uint16_t clearBuf(uint8_t sock);

tData* get_pBuf(uint8_t sock);

void freetData(void * buf, uint8_t sock);

void freetDataIdx(uint8_t idxBuf, uint8_t sock);

bool isBufAvail();

bool getTcpData(uint8_t sock, void** payload, uint16_t* len);

bool getTcpDataByte(uint8_t sock, uint8_t* payload, uint8_t peek);

uint16_t getAvailTcpDataByte(uint8_t sock);

bool isAvailTcpDataByte(uint8_t sock);

uint8_t freeTcpData(uint8_t sock);

void freeAllTcpData(uint8_t sock);

#endif /* ARD_UTILS_H_ */
