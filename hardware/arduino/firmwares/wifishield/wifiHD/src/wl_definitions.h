/*
 * wl_definitions.h
 *
 *  Created on: Mar 6, 2011
 *      Author: mlf by Metodo2 srl
 */

#ifndef WL_DEFINITIONS_H_
#define WL_DEFINITIONS_H_

// Maximum size of a SSID
#define WL_SSID_MAX_LENGTH 32
// Length of passphrase. Valid lengths are 8-63.
#define WL_WPA_KEY_MAX_LENGTH 63
// Length of key in bytes. Valid values are 5 and 13.
#define WL_WEP_KEY_MAX_LENGTH 13
// Size of a MAC-address or BSSID
#define WL_MAC_ADDR_LENGTH 6
// Size of a MAC-address or BSSID
#define WL_IPV4_LENGTH 4
// Maximum size of a SSID list
#define WL_NETWORKS_LIST_MAXNUM	10
// Maxmium number of socket
#define	MAX_SOCK_NUM		4
//Maximum number of attempts to establish wifi connection
#define WL_MAX_ATTEMPT_CONNECTION	10

typedef enum {
        WL_IDLE_STATUS,
        WL_NO_SSID_AVAIL,
        WL_SCAN_COMPLETED,
        WL_CONNECTED,
        WL_CONNECT_FAILED,
        WL_CONNECTION_LOST,
        WL_DISCONNECTED
} wl_status_t;


#endif /* WL_DEFINITIONS_H_ */
