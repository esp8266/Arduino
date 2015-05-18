/*
 *  Copyright (C) 2015 -2018  Espressif System
 *
 */

#ifndef __SMARTCONFIG_H__
#define __SMARTCONFIG_H__

typedef void (*sc_callback_t)(void *data);

typedef enum {
    SC_STATUS_WAIT = 0,
    SC_STATUS_FIND_CHANNEL,
    SC_STATUS_GETTING_SSID_PSWD,
    SC_STATUS_GOT_SSID_PSWD,
    SC_STATUS_LINK,
    SC_STATUS_LINK_OVER,
} sc_status;

typedef enum {
    SC_TYPE_ESPTOUCH = 0,
    SC_TYPE_AIRKISS,
} sc_type;

sc_status smartconfig_get_status(void);
const char *smartconfig_get_version(void);
bool smartconfig_start(sc_type type, sc_callback_t cb, ...);
bool smartconfig_stop(void);

#endif
