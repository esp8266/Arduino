#ifndef __ESPCONN_H__
#define __ESPCONN_H__

typedef sint8 err_t;

typedef void *espconn_handle;
typedef void (* espconn_connect_callback)(void *arg);
typedef void (* espconn_reconnect_callback)(void *arg, sint8 err);

/* Definitions for error constants. */

#define ESPCONN_OK          0    /* No error, everything OK. */
#define ESPCONN_MEM        -1    /* Out of memory error.     */
#define ESPCONN_TIMEOUT    -3    /* Timeout.                 */
#define ESPCONN_RTE        -4    /* Routing problem.         */
#define ESPCONN_INPROGRESS  -5    /* Operation in progress    */

#define ESPCONN_ABRT       -8    /* Connection aborted.      */
#define ESPCONN_RST        -9    /* Connection reset.        */
#define ESPCONN_CLSD       -10   /* Connection closed.       */
#define ESPCONN_CONN       -11   /* Not connected.           */

#define ESPCONN_ARG        -12   /* Illegal argument.        */
#define ESPCONN_ISCONN     -15   /* Already connected.       */

/** Protocol family and type of the espconn */
enum espconn_type {
    ESPCONN_INVALID    = 0,
    /* ESPCONN_TCP Group */
    ESPCONN_TCP        = 0x10,
    /* ESPCONN_UDP Group */
    ESPCONN_UDP        = 0x20,
};

/** Current state of the espconn. Non-TCP espconn are always in state ESPCONN_NONE! */
enum espconn_state {
    ESPCONN_NONE,
    ESPCONN_WAIT,
    ESPCONN_LISTEN,
    ESPCONN_CONNECT,
    ESPCONN_WRITE,
    ESPCONN_READ,
    ESPCONN_CLOSE
};

typedef struct _esp_tcp {
    int remote_port;
    int local_port;
    uint8 local_ip[4];
    uint8 remote_ip[4];
    espconn_connect_callback connect_callback;
    espconn_reconnect_callback reconnect_callback;
    espconn_connect_callback disconnect_callback;
	espconn_connect_callback write_finish_fn;
} esp_tcp;

typedef struct _esp_udp {
    int remote_port;
    int local_port;
    uint8 local_ip[4];
	uint8 remote_ip[4];
} esp_udp;

typedef struct _remot_info{
	enum espconn_state state;
	int remote_port;
	uint8 remote_ip[4];
}remot_info;

/** A callback prototype to inform about events for a espconn */
typedef void (* espconn_recv_callback)(void *arg, char *pdata, unsigned short len);
typedef void (* espconn_sent_callback)(void *arg);

/** A espconn descriptor */
struct espconn {
    /** type of the espconn (TCP, UDP) */
    enum espconn_type type;
    /** current state of the espconn */
    enum espconn_state state;
    union {
        esp_tcp *tcp;
        esp_udp *udp;
    } proto;
    /** A callback function that is informed about events for this espconn */
    espconn_recv_callback recv_callback;
    espconn_sent_callback sent_callback;
    uint8 link_cnt;
    void *reverse;
};

enum espconn_option{
	ESPCONN_START = 0x00,
	ESPCONN_REUSEADDR = 0x01,
	ESPCONN_NODELAY = 0x02,
	ESPCONN_COPY = 0x04,
	ESPCONN_END
};

/******************************************************************************
 * FunctionName : espconn_connect
 * Description  : The function given as the connect
 * Parameters   : espconn -- the espconn used to listen the connection
 * Returns      : none
*******************************************************************************/

sint8 espconn_connect(struct espconn *espconn);

/******************************************************************************
 * FunctionName : espconn_disconnect
 * Description  : disconnect with host
 * Parameters   : espconn -- the espconn used to disconnect the connection
 * Returns      : none
*******************************************************************************/

sint8 espconn_disconnect(struct espconn *espconn);

/******************************************************************************
 * FunctionName : espconn_delete
 * Description  : disconnect with host
 * Parameters   : espconn -- the espconn used to disconnect the connection
 * Returns      : none
*******************************************************************************/

sint8 espconn_delete(struct espconn *espconn);

/******************************************************************************
 * FunctionName : espconn_accept
 * Description  : The function given as the listen
 * Parameters   : espconn -- the espconn used to listen the connection
 * Returns      : none
*******************************************************************************/

sint8 espconn_accept(struct espconn *espconn);

/******************************************************************************
 * FunctionName : espconn_create
 * Description  : sent data for client or server
 * Parameters   : espconn -- espconn to the data transmission
 * Returns      : result
*******************************************************************************/

sint8 espconn_create(struct espconn *espconn);

/******************************************************************************
 * FunctionName : espconn_tcp_get_max_con
 * Description  : get the number of simulatenously active TCP connections
 * Parameters   : none
 * Returns      : none
*******************************************************************************/

uint8 espconn_tcp_get_max_con(void);

/******************************************************************************
 * FunctionName : espconn_tcp_set_max_con
 * Description  : set the number of simulatenously active TCP connections
 * Parameters   : num -- total number
 * Returns      : none
*******************************************************************************/

sint8 espconn_tcp_set_max_con(uint8 num);

/******************************************************************************
 * FunctionName : espconn_tcp_get_max_con_allow
 * Description  : get the count of simulatenously active connections on the server
 * Parameters   : espconn -- espconn to get the count
 * Returns      : result
*******************************************************************************/

sint8 espconn_tcp_get_max_con_allow(struct espconn *espconn);

/******************************************************************************
 * FunctionName : espconn_tcp_set_max_con_allow
 * Description  : set the count of simulatenously active connections on the server
 * Parameters   : espconn -- espconn to set the count
 * 				  num -- support the connection number
 * Returns      : result
*******************************************************************************/

sint8 espconn_tcp_set_max_con_allow(struct espconn *espconn, uint8 num);

/******************************************************************************
 * FunctionName : espconn_regist_time
 * Description  : used to specify the time that should be called when don't recv data
 * Parameters   : espconn -- the espconn used to the connection
 * 				  interval -- the timer when don't recv data
 * Returns      : none
*******************************************************************************/

sint8 espconn_regist_time(struct espconn *espconn, uint32 interval, uint8 type_flag);

/******************************************************************************
 * FunctionName : espconn_get_connection_info
 * Description  : used to specify the function that should be called when disconnect
 * Parameters   : espconn -- espconn to set the err callback
 *                discon_cb -- err callback function to call when err
 * Returns      : none
*******************************************************************************/

sint8 espconn_get_connection_info(struct espconn *pespconn, remot_info **pcon_info, uint8 typeflags);

/******************************************************************************
 * FunctionName : espconn_regist_sentcb
 * Description  : Used to specify the function that should be called when data
 *                has been successfully delivered to the remote host.
 * Parameters   : struct espconn *espconn -- espconn to set the sent callback
 *                espconn_sent_callback sent_cb -- sent callback function to
 *                call for this espconn when data is successfully sent
 * Returns      : none
*******************************************************************************/

sint8 espconn_regist_sentcb(struct espconn *espconn, espconn_sent_callback sent_cb);

/******************************************************************************
 * FunctionName : espconn_regist_sentcb
 * Description  : Used to specify the function that should be called when data
 *                has been successfully delivered to the remote host.
 * Parameters   : espconn -- espconn to set the sent callback
 *                sent_cb -- sent callback function to call for this espconn
 *                when data is successfully sent
 * Returns      : none
*******************************************************************************/

sint8 espconn_regist_write_finish(struct espconn *espconn, espconn_connect_callback write_finish_fn);

/******************************************************************************
 * FunctionName : espconn_sent
 * Description  : sent data for client or server
 * Parameters   : espconn -- espconn to set for client or server
 *                psent -- data to send
 *                length -- length of data to send
 * Returns      : none
*******************************************************************************/

sint8 espconn_sent(struct espconn *espconn, uint8 *psent, uint16 length);

/******************************************************************************
 * FunctionName : espconn_regist_connectcb
 * Description  : used to specify the function that should be called when
 *                connects to host.
 * Parameters   : espconn -- espconn to set the connect callback
 *                connect_cb -- connected callback function to call when connected
 * Returns      : none
*******************************************************************************/

sint8 espconn_regist_connectcb(struct espconn *espconn, espconn_connect_callback connect_cb);

/******************************************************************************
 * FunctionName : espconn_regist_recvcb
 * Description  : used to specify the function that should be called when recv
 *                data from host.
 * Parameters   : espconn -- espconn to set the recv callback
 *                recv_cb -- recv callback function to call when recv data
 * Returns      : none
*******************************************************************************/

sint8 espconn_regist_recvcb(struct espconn *espconn, espconn_recv_callback recv_cb);

/******************************************************************************
 * FunctionName : espconn_regist_reconcb
 * Description  : used to specify the function that should be called when connection
 *                because of err disconnect.
 * Parameters   : espconn -- espconn to set the err callback
 *                recon_cb -- err callback function to call when err
 * Returns      : none
*******************************************************************************/

sint8 espconn_regist_reconcb(struct espconn *espconn, espconn_reconnect_callback recon_cb);

/******************************************************************************
 * FunctionName : espconn_regist_disconcb
 * Description  : used to specify the function that should be called when disconnect
 * Parameters   : espconn -- espconn to set the err callback
 *                discon_cb -- err callback function to call when err
 * Returns      : none
*******************************************************************************/

sint8 espconn_regist_disconcb(struct espconn *espconn, espconn_connect_callback discon_cb);

/******************************************************************************
 * FunctionName : espconn_port
 * Description  : access port value for client so that we don't end up bouncing
 *                all connections at the same time .
 * Parameters   : none
 * Returns      : access port value
*******************************************************************************/

uint32 espconn_port(void);

/******************************************************************************
 * FunctionName : espconn_set_opt
 * Description  : access port value for client so that we don't end up bouncing
 *                all connections at the same time .
 * Parameters   : none
 * Returns      : access port value
*******************************************************************************/

sint8 espconn_set_opt(struct espconn *espconn, uint8 opt);

/******************************************************************************
 * TypedefName : dns_found_callback
 * Description : Callback which is invoked when a hostname is found.
 * Parameters  : name -- pointer to the name that was looked up.
 *               ipaddr -- pointer to an ip_addr_t containing the IP address of
 *               the hostname, or NULL if the name could not be found (or on any
 *               other error).
 *               callback_arg -- a user-specified callback argument passed to
 *               dns_gethostbyname
*******************************************************************************/

typedef void (*dns_found_callback)(const char *name, ip_addr_t *ipaddr, void *callback_arg);

/******************************************************************************
 * FunctionName : espconn_gethostbyname
 * Description  : Resolve a hostname (string) into an IP address.
 * Parameters   : pespconn -- espconn to resolve a hostname
 *                hostname -- the hostname that is to be queried
 *                addr -- pointer to a ip_addr_t where to store the address if 
 *                it is already cached in the dns_table (only valid if ESPCONN_OK
 *                is returned!)
 *                found -- a callback function to be called on success, failure
 *                or timeout (only if ERR_INPROGRESS is returned!)
 * Returns      : err_t return code
 *                - ESPCONN_OK if hostname is a valid IP address string or the host
 *                  name is already in the local names table.
 *                - ESPCONN_INPROGRESS enqueue a request to be sent to the DNS server
 *                  for resolution if no errors are present.
 *                - ESPCONN_ARG: dns client not initialized or invalid hostname
*******************************************************************************/

err_t espconn_gethostbyname(struct espconn *pespconn, const char *hostname, ip_addr_t *addr, dns_found_callback found);

/******************************************************************************
 * FunctionName : espconn_encry_connect
 * Description  : The function given as connection
 * Parameters   : espconn -- the espconn used to connect with the host
 * Returns      : none
*******************************************************************************/

sint8 espconn_secure_connect(struct espconn *espconn);

/******************************************************************************
 * FunctionName : espconn_encry_disconnect
 * Description  : The function given as the disconnection
 * Parameters   : espconn -- the espconn used to disconnect with the host
 * Returns      : none
*******************************************************************************/

sint8 espconn_secure_disconnect(struct espconn *espconn);

/******************************************************************************
 * FunctionName : espconn_encry_sent
 * Description  : sent data for client or server
 * Parameters   : espconn -- espconn to set for client or server
 * 				  psent -- data to send
 *                length -- length of data to send
 * Returns      : none
*******************************************************************************/

sint8 espconn_secure_sent(struct espconn *espconn, uint8 *psent, uint16 length);

/******************************************************************************
 * FunctionName : espconn_secure_accept
 * Description  : The function given as the listen
 * Parameters   : espconn -- the espconn used to listen the connection
 * Returns      : none
*******************************************************************************/

sint8 espconn_secure_accept(struct espconn *espconn);

/******************************************************************************
 * FunctionName : espconn_igmp_join
 * Description  : join a multicast group
 * Parameters   : host_ip -- the ip address of udp server
 * 				  multicast_ip -- multicast ip given by user
 * Returns      : none
*******************************************************************************/
sint8 espconn_igmp_join(ip_addr_t *host_ip, ip_addr_t *multicast_ip);

/******************************************************************************
 * FunctionName : espconn_igmp_leave
 * Description  : leave a multicast group
 * Parameters   : host_ip -- the ip address of udp server
 * 				  multicast_ip -- multicast ip given by user
 * Returns      : none
*******************************************************************************/
sint8 espconn_igmp_leave(ip_addr_t *host_ip, ip_addr_t *multicast_ip);

/******************************************************************************
 * FunctionName : espconn_recv_hold
 * Description  : hold tcp receive
 * Parameters   : espconn -- espconn to hold
 * Returns      : none
*******************************************************************************/
sint8 espconn_recv_hold(struct espconn *pespconn);

/******************************************************************************
 * FunctionName : espconn_recv_unhold
 * Description  : unhold tcp receive
 * Parameters   : espconn -- espconn to unhold
 * Returns      : none
*******************************************************************************/
sint8 espconn_recv_unhold(struct espconn *pespconn);

#endif

