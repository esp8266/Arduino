#ifndef WiFi_Spi_h
#define WiFi_Spi_h

#include "wl_definitions.h"

#define CMD_FLAG        0
#define REPLY_FLAG      1<<7

#define WIFI_SPI_ACK        1
#define WIFI_SPI_ERR        0xFF

#define TIMEOUT_CHAR    1000

//#define	MAX_SOCK_NUM		4	/**< Maxmium number of socket  */
#define NO_SOCKET_AVAIL     255

#define START_CMD   0xE0
#define WAIT_CMD   	0xE1
#define END_CMD     0xEE
  
enum {
	SET_NET_CMD 		= 0x10,
	SET_PASSPHRASE_CMD	= 0x11,
	SET_KEY_CMD	        = 0x12,

	GET_CONN_STATUS_CMD	= 0x20,
	GET_IPADDR_CMD		= 0x21,
	GET_MACADDR_CMD		= 0x22,
	GET_CURR_SSID_CMD	= 0x23,
	GET_CURR_BSSID_CMD	= 0x24,
	GET_CURR_RSSI_CMD	= 0x25,
	GET_CURR_ENCT_CMD	= 0x26,
	SCAN_NETWORKS		= 0x27,


    DISCONNECT_CMD		= 0x30,

    START_SERVER_TCP_CMD    = 0x40,
    GET_STATE_TCP_CMD       = 0x41,
    GET_DATA_TCP_CMD		= 0x42,
    AVAIL_DATA_TCP_CMD		= 0x43,
	SEND_DATA_TCP_CMD		= 0x44,
    DATA_SENT_TCP_CMD		= 0x45,
    GET_DATABUF_TCP_CMD		= 0x46,
};


enum wl_tcp_state {
  CLOSED      = 0,
  LISTEN      = 1,
  SYN_SENT    = 2,
  SYN_RCVD    = 3,
  ESTABLISHED = 4,
  FIN_WAIT_1  = 5,
  FIN_WAIT_2  = 6,
  CLOSE_WAIT  = 7,
  CLOSING     = 8,
  LAST_ACK    = 9,
  TIME_WAIT   = 10
};


enum numParams{
    PARAM_NUMS_0,
    PARAM_NUMS_1,
    PARAM_NUMS_2,
    PARAM_NUMS_3,
    PARAM_NUMS_4,
    PARAM_NUMS_5,
    MAX_PARAM_NUMS
};

#define MAX_PARAMS MAX_PARAM_NUMS-1
#define PARAM_LEN_SIZE 1

typedef struct  __attribute__((__packed__))
{
	uint8_t     paramLen;
	char*	    param;
}tParam;

typedef struct  __attribute__((__packed__))
{
	unsigned char	cmd;
	unsigned char	tcmd;
	unsigned char	nParam;
	tParam	params[MAX_PARAMS];
}tSpiMsg;


typedef struct  __attribute__((__packed__))
{
	unsigned char	cmd;
	unsigned char	tcmd;
	//unsigned char	totLen;
	unsigned char	nParam;
}tSpiHdr;

#endif
