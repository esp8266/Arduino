/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*! \page License
 * Copyright (C) 2009, H&D Wireless AB All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of H&D Wireless AB may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY H&D WIRELESS AB ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE EXPRESSLY AND
 * SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "wl_cm.h"
#include "wl_util.h"
#include <string.h>



#if 1
# include "printf-stdarg.h"
# define CM_DPRINTF(fmt...) printk(fmt)
#else
# define CM_DPRINTF(fmt...)
#endif


/** \defgroup wl_cm Connection Manager
 *
 * These functions are used to configure and control the WiFi connetion 
 * manager. 
 *
 *
 *  @{
 */

struct cm_candidate {
        struct wl_ssid_t ssid;
        struct wl_mac_addr_t bssid;
};

struct cm {
        cm_scan_cb_t *scan_cb;
        cm_conn_cb_t *conn_cb;
        cm_disconn_cb_t *disconn_cb;
        cm_err_cb_t *err_cb;
        void* ctx;
        
        struct cm_candidate candidate;
};


/**
 *
 */
static struct wl_network_t*
find_best_candidate(struct cm* cm)
{
        struct wl_network_t* wl_network;
        uint8_t cnt, i;
        
        wl_get_network_list(&wl_network, &cnt);
        if (cnt == 0)
                return NULL;

        for (i = 0; i < cnt; i++) {
                /* match on ssid */
                if (cm->candidate.ssid.len)
                        if (!equal_ssid(&cm->candidate.ssid, 
                                        &wl_network[i].ssid))
                                continue;

                /* match bssid */
                if (strncmp((char*) cm->candidate.bssid.octet, 
                            "\xff\xff\xff\xff\xff\xff", 6))
                        if (!equal_bssid(&cm->candidate.bssid, 
                                         &wl_network[i].bssid))
                                continue;

                /* anything will do */
                return &wl_network[i];
        }

        return NULL;
}


/**
 * 
 */
static void
select_net(struct cm* cm)
{
        struct wl_network_t *candidate_net;
        struct wl_network_t *current_net;
        int ret;
        
        current_net = wl_get_current_network();
        candidate_net = find_best_candidate(cm);

        /* disconnected, and no candidate found? */
        if (cm->candidate.ssid.len != 0 && current_net == NULL && candidate_net == NULL) {
		; /* to scan */
        
        /* already connected to the candidate? */
        } else if (current_net == candidate_net) {
                return;

        /* disconnected, and a candidate is found */
        } else if (current_net == NULL && candidate_net) {
                ret = wl_connect(candidate_net->ssid.ssid,
                                 candidate_net->ssid.len);
                switch (ret) {
                case WL_SUCCESS :
                        return;
                case WL_BUSY:
                        wl_disconnect();
                        return;
                default :
                        cm->err_cb(&ret);
                        break;
                } 
                
                CM_DPRINTF("CM: failed to connect\n");

        /* connected, but another (or no valid) candidate was found */
        } else if (current_net) {
                if (wl_disconnect() == WL_SUCCESS)
                        return;
                
                CM_DPRINTF("CM: failed to disconnect\n");
        }
                        
        /* some operation failed or no candidate found */
        if (wl_scan() != WL_SUCCESS)
                CM_DPRINTF("CM: failed to scan\n");                
}


/**
 *
 */
static void 
wl_scan_complete_cb(void* ctx)
{
        struct cm *cm = ctx;

        CM_DPRINTF("CM: scan completed\n");

        if (cm->scan_cb)
                cm->scan_cb(cm->ctx);

        select_net(cm);
}

/**
 *
 */
static void 
wl_media_connected_cb(void* ctx)
{
        struct cm *cm = ctx;
        struct wl_network_t *net = wl_get_current_network();
        CM_DPRINTF("CM: connected to %s\n", ssid2str(&net->ssid));
        if (cm->conn_cb)
                cm->conn_cb(net, cm->ctx);
}


/**
 *
 */
static void 
wl_conn_failure_cb(void* ctx)
{
        CM_DPRINTF("CM: connect failed, scanning\n");
        
        if (wl_scan() != WL_SUCCESS)
                /* should never happen */
                CM_DPRINTF("CM: could not start scan after connect fail!\n");
}


/**
 *
 */
static void 
wl_conn_lost_cb(void* ctx)
{
        struct cm *cm = ctx;
        CM_DPRINTF("CM: connection lost, scanning\n");

        if (cm->disconn_cb)
                cm->disconn_cb(cm->ctx);

        if (wl_scan() != WL_SUCCESS)
                /* should never happen */
                CM_DPRINTF("CM: could not start scan after connect lost!\n");
}


/**
 *
 */
static void
wl_event_cb(struct wl_event_t event, void* ctx)
{
        struct cm *cm = ctx;

        switch (event.id) {
        case WL_EVENT_MEDIA_CONNECTED:
                wl_media_connected_cb(cm);
                break;
                
        case WL_EVENT_CONN_FAILURE:
                wl_conn_failure_cb(cm);
                break;
                
        case WL_EVENT_MEDIA_DISCONNECTED:
                CM_DPRINTF("CM: disconnected\n");
                wl_conn_lost_cb(cm);
                break;

        case WL_EVENT_CONN_LOST:
                wl_conn_lost_cb(cm);
                break;

        case WL_EVENT_SCAN_COMPLETE:
                wl_scan_complete_cb(cm);
                break;

        default:
                CM_DPRINTF("CM: unhandled event\n");
        };
}

static struct cm *cm = NULL;


/**
 *
 */
wl_err_t
wl_cm_start(cm_scan_cb_t scan_cb, 
            cm_conn_cb_t conn_cb, 
            cm_disconn_cb_t disconn_cb,
            cm_err_cb_t    err_cb,
            void* ctx)
{
        if (cm != NULL)
                return WL_FAILURE;

        cm = calloc(1, sizeof(struct cm));
        if (cm == NULL) {
                CM_DPRINTF("CM: out of memory\n");
                return WL_FAILURE;
        }
        
        if (wl_register_event_cb(wl_event_cb, cm) != WL_SUCCESS) {
                CM_DPRINTF("CM: could not register event cb\n");
                return WL_FAILURE;
        }

        cm->scan_cb = scan_cb;
        cm->conn_cb = conn_cb;
        cm->disconn_cb = disconn_cb;
        cm->err_cb = err_cb;
        cm->ctx = ctx;
        if (wl_scan() != WL_SUCCESS)
                CM_DPRINTF("CM: could not scan\n");
        
        CM_DPRINTF("CM: initialized\n");
        return WL_SUCCESS;
}


/**
 * Set the desired network which the connection manager should try to 
 * connect to.
 * 
 * The ssid and bssid of the desired network should be specified. The ssid and
 * bssid will be matched against the networks found during scan. If any 
 * parameter is null, it will always match. If both parameters are null, 
 * the first found network will be chosen.
 *
 * @param ssid The ssid of the desired network. If null, any ssid will match.
 * @param bssid The bssid of the desired network. If null, any bssid will match.
 *          
 */
wl_err_t 
wl_cm_set_network(struct wl_ssid_t *ssid, struct wl_mac_addr_t *bssid)
{
        if (cm == NULL)
                return WL_FAILURE;
           
        if (ssid)
                memcpy(&cm->candidate.ssid, ssid, sizeof(cm->candidate.ssid));
        else 
                cm->candidate.ssid.len = 0;
        
        if (bssid)
                memcpy(&cm->candidate.bssid, bssid, 
                       sizeof(cm->candidate.bssid));
        else
                memset(&cm->candidate.bssid, 0xff, sizeof(cm->candidate.bssid));
        (void) wl_scan();
        return WL_SUCCESS;
}
/*
 *  @}
 */
