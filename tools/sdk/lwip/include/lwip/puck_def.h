/*
 * puck_def.h
 *
 *  Created on: Jul 22, 2010
 *      Author: dtoma
 */

#ifndef PUCK_DEF_H_
#define PUCK_DEF_H_



#define INSTRUMENT_PORT      8760

#define INSTRUMENT_LENGTH    80

#define MDNS_NAME_LENGTH     68 //68

char* PUCK_SERVICE    = NULL;
//#define PUCK_SERVICE	"_Escpressif._tcp.local"
#define DNS_SD_SERVICE       "_services._dns-sd._udp.local"
#define SERVICE_DESCRIPTION  "PUCK PROTOCOL"
#define PUCK_SERVICE_LENGTH  30

#define UUID_LEN      		 	16
#define DS_VERS_LEN    			2
#define DS_SIZE_LEN    			2
#define MAN_ID_LEN     			4
#define MAN_MODEL_LEN  			2
#define MAN_VERS_LEN   			2
#define SER_NUM_LEN    			4
#define NAME_LEN       			64
#define PUCK_DATASHEET_SIZE   	96

#define UUID_OFFSET       0
#define DS_VERS_OFFSET    UUID_LEN + UUID_OFFSET
#define DS_SIZE_OFFSET    DS_VERS_LEN + DS_VERS_OFFSET
#define MAN_ID_OFFSET     DS_SIZE_LEN + DS_SIZE_OFFSET
#define MAN_MODEL_OFFSET  MAN_ID_LEN + MAN_ID_OFFSET
#define MAN_VERS_OFFSET   MAN_MODEL_LEN + MAN_MODEL_OFFSET
#define SER_NUM_OFFSET    MAN_VERS_LEN + MAN_VERS_OFFSET
#define NAME_OFFSET       SER_NUM_LEN + SER_NUM_OFFSET

#endif /* __PUCK_DEF_H__ */
