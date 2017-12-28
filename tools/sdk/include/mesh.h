/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2015 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */
#ifndef __LWIP_API_MESH_H__
#define __LWIP_API_MESH_H__

#include "ip_addr.h"
#include "user_interface.h"
#include "espconn.h"
#ifdef __cplusplus
extern "C" {
#endif

#define ESP_MESH_GROUP_ID_LEN (6)

typedef void (* espconn_mesh_callback)();
typedef void (* espconn_mesh_scan_callback)(void *arg, int8_t status);

enum mesh_type {
	MESH_CLOSE = 0,
    MESH_LOCAL,
    MESH_ONLINE,
    MESH_NONE = 0xFF
};
/** \defgroup Mesh_APIs Mesh APIs
  * @brief Mesh APIs
  *
  * 
  *
  */

/** @addtogroup Mesh_APIs
  * @{
  */

enum mesh_status {
    MESH_DISABLE = 0,
    MESH_WIFI_CONN,
    MESH_NET_CONN,
    MESH_LOCAL_AVAIL,
    MESH_ONLINE_AVAIL
};

enum mesh_node_type {
	MESH_NODE_PARENT = 0,
	MESH_NODE_CHILD,
	MESH_NODE_ALL
};

struct mesh_scan_para_type {
	espconn_mesh_scan_callback usr_scan_cb; // scan done callback
	uint8_t grp_id[ESP_MESH_GROUP_ID_LEN];  // group id
	bool grp_set;                           // group set
};


/**
  * @brief     Check whether the IP address is mesh local IP address or not.
  *
  * @attention 1. The range of mesh local IP address is 2.255.255.* ~ max_hop.255.255.*.
  * @attention 2. IP pointer should not be NULL. If the IP pointer is NULL, it will return false.
  *
  * @param     struct ip_addr *ip : IP address
  *
  * @return    true  : the IP address is mesh local IP address
  * @return    false : the IP address is not mesh local IP address
  */
bool espconn_mesh_local_addr(struct ip_addr *ip);

/**
  * @brief     Get the information of router used by mesh network.
  *
  * @attention 1. The function should be called after mesh_enable_done
  *
  * @param     struct station_config *router: router inforamtion
  *
  * @return  true  : succeed
  * @return  false : fail
  */
bool espconn_mesh_get_router(struct station_config *router);

/**
  * @brief   Set the information of router used by mesh network.
  *
  * @attention The function must be called before espconn_mesh_enable.
  *
  * @param   struct station_config *router: router information.
  *               user should initialize the ssid and password.
  *
  * @return  true  : succeed
  * @return  false : fail
  */
bool espconn_mesh_set_router(struct station_config *router);

/**
  * @brief   Set server setup by user.
  *
  * @attention If users wants to use themself server, they use the function.
  *                 but the function must be called before espconn_mesh_enable.
  *                 at the same time, users need to implement the server.
  *
  * @param     struct ip_addr *ip : ip address of server.
  * @param     uint16_t port : port used by server.
  *
  * @return  true  : succeed
  * @return  false : fail
  */
bool espconn_mesh_server_init(struct ip_addr *ip, uint16_t port);

/**
  * @brief   Get the information of mesh node.
  *
  * @param   enum mesh_node_type typ : mesh node type.
  * @param   uint8_t **info : the information will be saved in *info.
  * @param   uint8_t *count : the node count in *info.
  *
  * @return  true  : succeed
  * @return  false : fail
  */
bool espconn_mesh_get_node_info(enum mesh_node_type type,
		                        uint8_t **info, uint8_t *count);

/**
  * @brief     Set WiFi cryption algrithm and password for mesh node.
  *
  * @attention The function must be called before espconn_mesh_enable.
  *
  * @param     AUTH_MODE mode : cryption algrithm (WPA/WAP2/WPA_WPA2).
  * @param     uint8_t *passwd : password of WiFi.
  * @param     uint8_t passwd_len : length of password (8 <= passwd_len <= 64).
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool espconn_mesh_encrypt_init(AUTH_MODE mode, uint8_t *passwd, uint8_t passwd_len);
/**
  * @brief     Set prefix of SSID for mesh node.
  *
  * @attention The function must be called before espconn_mesh_enable.
  *
  * @param     uint8_t *prefix : prefix of SSID.
  * @param     uint8_t prefix_len : length of prefix (0 < passwd_len <= 22).
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool espconn_mesh_set_ssid_prefix(uint8_t *prefix, uint8_t prefix_len);

/**
  * @brief     Set max hop for mesh network.
  *
  * @attention The function must be called before espconn_mesh_enable.
  *
  * @param     uint8_t max_hops : max hop of mesh network (1 <= max_hops < 10, 4 is recommended).
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool espconn_mesh_set_max_hops(uint8_t max_hops);

/**
  * @brief     Set group ID of mesh node.
  *
  * @attention The function must be called before espconn_mesh_enable.
  *
  * @param     uint8_t *grp_id : group ID.
  * @param     uint16_t gid_len: length of group ID, now gid_len = 6.
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool espconn_mesh_group_id_init(uint8_t *grp_id, uint16_t gid_len);

/**
  * @brief     Set the curent device type.
  *
  * @param     uint8_t dev_type : device type of mesh node
  *
  * @return    true  : succeed
  * @return    false : fail
  */
bool espconn_mesh_set_dev_type(uint8_t dev_type);
/**
  * @brief    Get the curent device type.
  *
  * @param    none
  *
  * @return    device type
  */
uint8_t espconn_mesh_get_dev_type();

/**
  * @brief     Try to establish mesh connection to server.
  *
  * @attention If espconn_mesh_connect fail, returns non-0 value, there is no connection, so it
  *            won't enter any espconn callback.
  *
  * @param     struct espconn *usr_esp : the network connection structure,  the usr_esp to
  *            listen to the connection
  *
  * @return    0     : succeed
  * @return    Non-0 : error code
  *    -  ESPCONN_RTE    - Routing Problem
  *    -  ESPCONN_MEM    - Out of memory
  *    -  ESPCONN_ISCONN - Already connected
  *    -  ESPCONN_ARG    - Illegal argument, can't find the corresponding connection
  *                        according to structure espconn
  */
int8_t espconn_mesh_connect(struct espconn *usr_esp);

/**
  * @brief     Disconnect a mesh connection.
  *
  * @attention Do not call this API in any espconn callback. If needed, please use system
  *            task to trigger espconn_mesh_disconnect.
  *
  * @param     struct espconn *usr_esp : the network connection structure
  *
  * @return    0     : succeed
  * @return    Non-0 : error code
  *    -  ESPCONN_ARG - illegal argument, can't find the corresponding TCP connection
  *                     according to structure espconn
  */
  
int8_t espconn_mesh_disconnect(struct espconn *usr_esp);

/**
  * @brief     Get current mesh status.
  *
  * @param     null
  *
  * @return    the current mesh status, please refer to enum mesh_status.
  */
int8_t espconn_mesh_get_status();

/**
  * @brief     Send data through mesh network.
  *
  * @attention Please call espconn_mesh_sent after espconn_sent_callback of the pre-packet.
  *
  * @param     struct espconn *usr_esp : the network connection structure
  * @param     uint8 *pdata : pointer of data
  * @param     uint16 len : data length
  *
  * @return    0     : succeed
  * @return    Non-0 : error code
  *    -  ESPCONN_MEM    - out of memory
  *    -  ESPCONN_ARG    - illegal argument, can't find the corresponding network transmission
  *                        according to structure espconn
  *    -  ESPCONN_MAXNUM - buffer of sending data is full
  *    -  ESPCONN_IF     - send UDP data fail
  */
int8_t espconn_mesh_sent(struct espconn *usr_esp, uint8 *pdata, uint16 len);

/**
  * @brief     Get max hop of mesh network.
  *
  * @param     null.
  *
  * @return    the current max hop of mesh
  */
uint8_t espconn_mesh_get_max_hops();

/**
  * @brief    To enable mesh network.
  *
  * @attention 1. the function should be called in user_init. 
  * @attention 2. if mesh node can not scan the mesh AP, it will be isolate node without trigger enable_cb.
  *                     user can use espconn_mesh_get_status to get current status of node.
  * @attention 3. if user try to enable online mesh, but node fails to establish mesh connection
  *                     the node will work with local mesh.
  *
  * @param     espconn_mesh_callback enable_cb : callback function of mesh-enable
  * @param     enum mesh_type type : type of mesh, local or online.
  *
  * @return    null
  */
void espconn_mesh_enable(espconn_mesh_callback enable_cb, enum mesh_type type);

/**
  * @brief     To disable mesh network.
  *
  * @attention When mesh network is disabed, the system will trigger disable_cb. 
  *
  * @param     espconn_mesh_callback disable_cb : callback function of mesh-disable
  * @param     enum mesh_type type : type of mesh, local or online.
  *
  * @return    null
  */
void espconn_mesh_disable(espconn_mesh_callback disable_cb);

/**
  * @brief     To print version of mesh.
  *
  * @param     null
  *
  * @return    null
  */
void espconn_mesh_print_ver();

/**
  * @brief     To get AP around node.
  *
  * @attention User can get normal AP or mesh AP using the function.
  *                  If user plans to get normal AP, he/she needs to clear grp_set flag in para.
  *                  If user plans to get mesh AP, he/she needs to set grp_set and grp_id;
  *
  * @param     struct mesh_scan_para_type *para : callback function of mesh-disable
  *
  * @return    null
  */
void espconn_mesh_scan(struct mesh_scan_para_type *para);
/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif
