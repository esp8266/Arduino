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
#include "util.h"
#include <string.h>
#include "debug.h"

/** Roaming configuration parameters **/

/*! The ROAMING_RSSI_THRESHOLD setting defines how bad the current
 *  signal strength should be before we'll consider roaming to an AP
 *  with better signal strength. The objective is to stay on the
 *  current AP as long as the RSSI is decent, even if there are other
 *  APs in the same BSS with better RSSI available.  
 *  If ROAMING_RSSI_THRESHOLD is too high we might roam unecessarily.  
 *  If ROAMING_RSSI_THRESHOLD is too low we might not roam in time to
 *  avoid packet loss. This also impacts power consumption, staying
 *  too long with an AP with poor RSSI will consume more power.
 *  Unit is dBm.
 */
#define ROAMING_RSSI_THRESHOLD -65

/*! The ROAMING_RSSI_DIFF setting defines how much better
 *  than the currently associated AP a new AP must be before 
 *  we'll attempt to roam over to the new AP.
 *  If ROAMING_RSSI_DIFF is too high it might be too hard
 *  to roam (important if the STA is expected to move
 *  quickly through different AP coverage areas).
 *  If ROAMING_RSSI_DIFF is too low we might bounce between
 *  two APs with similar signal strengths.
 *  Unit is dBm.
 */
#define ROAMING_RSSI_DIFF 10

# include "printf-stdarg.h"
#include "ard_utils.h"
#include "debug.h"

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
        void* ctx;
        uint8_t enabled;
        struct cm_candidate candidate;
};


/**
 * This function can be modified to pick a network based on
 * application specific criteria.
 *
 * If the SSID can not be found in the scan list it will be
 * assumed to be a hidden SSID and the wl_connect() command
 * will be called to attempt to probe for the network and
 * connect to it.
 */
static struct wl_network_t*
find_best_candidate(struct cm* cm)
{
        struct wl_network_list_t* netlist;
        struct wl_network_t *best_net = NULL;
        uint8_t i;
        
        if (wl_get_network_list(&netlist) != WL_SUCCESS)
                return NULL;
        
        if (netlist->cnt == 0)
                return NULL;

        for (i = 0; i < netlist->cnt; i++) {
                /* match on ssid */
                if (cm->candidate.ssid.len)
                        if (!equal_ssid(&cm->candidate.ssid, 
                                        &netlist->net[i]->ssid))
                                continue;

                /* match bssid */
                if (strncmp((char*) cm->candidate.bssid.octet, 
                            "\xff\xff\xff\xff\xff\xff", 6))
                        if (!equal_bssid(&cm->candidate.bssid, 
                                         &netlist->net[i]->bssid))
                                continue;
                /* check for best rssi. */
                if ( best_net && 
                     ( best_net->rssi > netlist->net[i]->rssi) ) {
                        continue;
                }
                best_net = netlist->net[i];
        }

        return best_net;
}


/**
 * 
 */
static void
select_net(struct cm* cm)
{
        struct wl_network_t *candidate_net;
        struct wl_network_t *current_net;
        struct wl_ssid_t *ssid_p;

        int ret;

        /* Nothing to do */
        if (0 == cm->candidate.ssid.len) {
                return;
        }
        
        current_net = wl_get_current_network();
        candidate_net = find_best_candidate(cm);

        /* Connected to the candidate? ... */
        if ( current_net == candidate_net ) {
                if ( current_net ) {
                        /* ...yes, dont change. */
                        
                        return;
                }
        }

        /* Roaming checks */
        if (current_net && candidate_net) {
                /* Are we changing BSSs? */
                if ( equal_ssid(&candidate_net->ssid, 
                                &current_net->ssid)) {

                        /* ...no. Does the currently connected
                         * net have a decent RSSI?...*/
                        if ( current_net->rssi > ROAMING_RSSI_THRESHOLD ) {
                                /* ...yes, stay with it. */
                                return;
                        }
                        /* ...no. Does the candidate have
                         * sufficiently better RSSI to
                         * motivate a switch to it? */
                        if ( candidate_net->rssi < current_net->rssi + 
                             ROAMING_RSSI_DIFF) {
                                return;
                        }
                        /* ...yes, try to roam to candidate_net */
                        CM_DPRINTF("CM: Roaming from rssi %d to %d\n",
                                   current_net->rssi,
                                   candidate_net->rssi);
                }
        }
        /* a candidate is found */
        if (candidate_net) {
                /* We connect to a specific bssid here because
                 * find_best_candidate() might have picked a
                 * particulare AP among many with the same SSID.
                 * wl_connect() would pick one of them at random.
                 */
                ret = wl_connect_bssid(candidate_net->bssid);
        }
        /* no candidate found */
        else {
                CM_DPRINTF("CM: No candidate found for ssid \"%s\"\n",
                           ssid2str(&cm->candidate.ssid));
                /* Might be a hidden SSID so we try to connect to it.
                 * wl_connect() will trigger a directed scan
                 * for the SSID in this case.
                 */
                ssid_p = &cm->candidate.ssid;
                ret = wl_connect(ssid_p->ssid, ssid_p->len);
        }
        switch (ret) {
        case WL_SUCCESS :
                return;
        case WL_BUSY:
                wl_disconnect();
                return;
        case WL_RETRY:
                break;
        default :
                CM_DPRINTF("CM: failed to connect\n");
                break;
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

        if ( 0 == cm->enabled ) {
                return;
        }
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
        LINK_LED_ON();
        ERROR_LED_OFF();
        if (cm->conn_cb)
                cm->conn_cb(net, cm->ctx);
}


/**
 *
 */
static void 
wl_conn_failure_cb(void* ctx)
{
        struct cm *cm = ctx;
        CM_DPRINTF("CM: connect failed, scanning\n");
        ERROR_LED_ON();
        LINK_LED_OFF();
        
        if ( 0 == cm->enabled ) {
                return;
        }
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
        LINK_LED_OFF();
        if (cm->disconn_cb)
                cm->disconn_cb(cm->ctx);

        if ( 0 == cm->enabled ) {
                return;
        }
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

        case WL_EVENT_SCAN_COMPLETE:
                wl_scan_complete_cb(cm);
                break;

        default:
                CM_DPRINTF("CM: unhandled event\n");
        };
}

static struct cm *cm = NULL;


/** 
 * Doesn't actually start the CM, just initializing. CM will run whenever
 * an valid ssid is set through wl_cm_set_network() and wl_cm_start()
 * has been called.
 */
wl_err_t
wl_cm_init(cm_scan_cb_t scan_cb, 
            cm_conn_cb_t conn_cb, 
            cm_disconn_cb_t disconn_cb,
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
        cm->enabled = 0;
        cm->ctx = ctx;
        
        CM_DPRINTF("CM: initialized\n");
        return WL_SUCCESS;
}

wl_err_t 
wl_cm_start(void) {
        if (NULL == cm)
                return WL_FAILURE;

        cm->enabled = 1;
        return WL_SUCCESS;
}

wl_err_t 
wl_cm_stop(void) {
        if (NULL == cm)
                return WL_FAILURE;

        cm->enabled = 0;
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

        if (cm->candidate.ssid.len)
                wl_scan();
        
        return WL_SUCCESS;
}
/*
 *  @}
 */
