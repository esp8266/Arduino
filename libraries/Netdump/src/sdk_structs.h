#ifndef _SDK_STRUCTS_H_
#define _SDK_STRUCTS_H_

#include <ESP8266WiFi.h>

// SDK structures -----------------------------------

typedef struct
{
    signed rssi:8;            /**< signal intensity of packet */
    unsigned rate:4;          /**< data rate */
    unsigned is_group:1;
    unsigned :1;              /**< reserve */
    unsigned sig_mode:2;      /**< 0:is not 11n packet; 1:is 11n packet */
    unsigned legacy_length:12;
    unsigned damatch0:1;
    unsigned damatch1:1;
    unsigned bssidmatch0:1;
    unsigned bssidmatch1:1;
    unsigned mcs:7;           /**< if is 11n packet, shows the modulation(range from 0 to 76) */
    unsigned cwb:1;           /**< if is 11n packet, shows if is HT40 packet or not */
    unsigned HT_length:16;             /**< reserve */
    unsigned smoothing:1;     /**< reserve */
    unsigned not_sounding:1;  /**< reserve */
    unsigned :1;              /**< reserve */
    unsigned aggregation:1;   /**< Aggregation */
    unsigned stbc:2;          /**< STBC */
    unsigned fec_coding:1;    /**< Flag is set for 11n packets which are LDPC */
    unsigned sgi:1;           /**< SGI */
    unsigned rxend_state:8;
    unsigned ampdu_cnt:8;     /**< ampdu cnt */
    unsigned channel:4;       /**< which channel this packet in */
    unsigned :4;              /**< reserve */
    signed noise_floor:8;
} wifi_pkt_rx_ctrl_t;

typedef struct {
    wifi_pkt_rx_ctrl_t rx_ctrl;
    uint8 buf[112];
    uint16 cnt;
    uint16 len; //length of packet
} wifi_pkt_mgmt_t;

typedef struct {
  uint16 length;
  uint16 seq;
  uint8  address3[6];
} wifi_pkt_lenseq_t;

typedef struct {
    wifi_pkt_rx_ctrl_t rx_ctrl;
    uint8_t  buf[36];
    uint16_t cnt;
    wifi_pkt_lenseq_t lenseq[1];
} wifi_pkt_data_t;

typedef struct {
    wifi_pkt_rx_ctrl_t rx_ctrl; /**< metadata header */
    uint8_t payload[0];       /**< Data or management payload. Length of payload is described by rx_ctrl.sig_len. Type of content determined by packet type argument of callback. */
} wifi_promiscuous_pkt_t;

#endif
