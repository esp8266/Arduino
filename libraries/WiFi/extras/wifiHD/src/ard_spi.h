/*
 * ard_spi.h
 *
 *  Created on: May 27, 2010
 *      Author: mlf by Metodo2 srl
 */

#ifndef ARD_SPI_H_
#define ARD_SPI_H_

#include "lwip/netif.h"
#include "console.h"
#include "wl_definitions.h"


typedef enum {
		SPI_CMD_IDLE,
		SPI_CMD_INPUT,
        SPI_CMD_DONE,
        SPI_CMD_INPROGRESS,
        SPI_CMD_REPLING,
        SPI_CMD_FAIL,
} cmd_spi_state_t;

typedef enum {
		REPLY_ERR_GET,
		REPLY_ERR_SET,
		REPLY_ERR_CMD,
		REPLY_ERR_MSG,
        REPLY_NO_ERR,
} reply_err_t;


typedef enum {
	CMD_GET_FLAG = 0x01,
	CMD_SET_FLAG = 0x02,
	CMD_IMM_SET_FLAG = 0x04,
}cmd_flags;

typedef enum eProtMode {TCP_MODE, UDP_MODE}tProtMode;

#define TIMEOUT_SPI  		200
#define SPI_ALIGN_ERROR		0xF0
#define SPI_OVERRIDE_ERROR	0xF1
#define SPI_TIMEOUT_ERROR	0xF2
#define DUMMY_DATA			0xFF

typedef int (*cmd_spi_cb_t)(int numParam, char* buf, void* ctx);
typedef cmd_spi_state_t (*cmd_spi_rcb_t)(char* recv, char* reply, void* ctx, uint16_t* _count);

typedef struct eRemoteClient{
	uint32_t ipaddr;
	uint16_t port;
}tRemoteClient;

void set_result_cmd(int err) ;

void set_result(wl_status_t _status);

int initSpi(void* ctx);

void initExtInt();

void spi_poll(struct netif* netif);

int spi_slaveReceive(volatile avr32_spi_t *spi);

void showTTCPstatus();

int getSock(void * _ttcp);

void* getTTCP(uint8_t sock, uint8_t mode);

void setMapSockMode(uint8_t sock, void* _ttcp, uint8_t _tcp_mode);

void clearMapSockTcp(uint8_t sock, uint8_t mode);

int start_server_tcp(uint16_t port, uint8_t sock, uint8_t protMode);

int start_client_tcp(uint32_t _addr, uint16_t port, uint8_t sock, uint8_t protMode);

void setRemoteClient(uint16_t sock, uint32_t _ipaddr, uint16_t _port);

tRemoteClient* getRemoteClient(uint16_t sock);

void getRemoteData(uint8_t sock, uint8_t mode, tRemoteClient* remoteData);

#endif /* ARD_SPI_H_ */
