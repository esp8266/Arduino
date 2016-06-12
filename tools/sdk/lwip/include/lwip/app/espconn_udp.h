#ifndef __ESPCONN_UDP_H__
#define __ESPCONN_UDP_H__

#ifndef ESPCONN_UDP_DEBUG
#define ESPCONN_UDP_DEBUG LWIP_DBG_OFF
#endif

#include "lwip/app/espconn.h"

/******************************************************************************
 * FunctionName : espconn_udp_client
 * Description  : Initialize the client: set up a PCB and bind it to the port
 * Parameters   : pespconn -- the espconn used to build client
 * Returns      : none
*******************************************************************************/

extern sint8 espconn_udp_client(struct espconn *pespconn);

/******************************************************************************
 * FunctionName : espconn_udp_disconnect
 * Description  : A new incoming connection has been disconnected.
 * Parameters   : espconn -- the espconn used to disconnect with host
 * Returns      : none
*******************************************************************************/

extern void espconn_udp_disconnect(espconn_msg *pdiscon);

/******************************************************************************
 * FunctionName : espconn_udp_server
 * Description  : Initialize the server: set up a PCB and bind it to the port
 * Parameters   : pespconn -- the espconn used to build server
 * Returns      : none
*******************************************************************************/

extern sint8 espconn_udp_server(struct espconn *espconn);

/******************************************************************************
 * FunctionName : espconn_udp_sent
 * Description  : sent data for client or server
 * Parameters   : void *arg -- client or server to send
 *                uint8* psent -- Data to send
 *                uint16 length -- Length of data to send
 * Returns      : none
*******************************************************************************/

extern err_t espconn_udp_sent(void *arg, uint8 *psent, uint16 length);

/******************************************************************************
 * FunctionName : espconn_udp_sendto
 * Description  : sent data for UDP
 * Parameters   : void *arg -- UDP to send
 * 				  uint8* psent -- Data to send
 *                uint16 length -- Length of data to send
 * Returns      : return espconn error code.
 * - ESPCONN_OK. Successful. No error occured.
 * - ESPCONN_MEM. Out of memory.
 * - ESPCONN_RTE. Could not find route to destination address.
 * - More errors could be returned by lower protocol layers.
*******************************************************************************/
extern err_t espconn_udp_sendto(void *arg, uint8 *psent, uint16 length);

#endif /* __ESPCONN_UDP_H__ */


