/*
 *  Programming interface for wlap_api.
 *  Copyright (C) 2011 HD Wireless AB
 *
 *  You should have received a copy of the license along with this library.
 */

/*! \file wlap_api.h *************************************************************
 *
 * \brief WiFi AP API
 *
 * This file provides the wlap_api interface.
 *
 * - Compiler:           GNU GCC for AVR32
 * - Supported devices:  
 *  \li SPB104 + EVK1100
 *  \li SPB104 + EVK1101
 *  \li SPB104 + EVK1104
 *  \li SPB104 + EVK1105 (SPI)
 *  \li SPB104 + EVK1105 (SPI + irq)
 *  \li SPB105 + EVK1105 (SPI)
 * - AppNote:
 *
 * \author               H&D Wireless AB: \n
 *
 *****************************************************************************
 *
 * \section intro Introduction
 * This is the documentation for the WiFi AP Driver API \a wlap_api.
 *
 * \section files Main Files
 * - wlap_api.h : WiFi driver interface.
 * - libwlap_api_*.*.a - Driver library.
 * 
 */

#ifndef WLAP_API_H
#define WLAP_API_H
 
#define WLAP_API_RELEASE_NAME "unknown"

#include <wl_api.h>

/** \defgroup wl_softap Access Point Mode
 *
 * \brief Support the WiFi Access Point mode.
 *
 *  @{
 */
/*
 * Station representation 
 *
 */
struct wl_sta_t
{
        struct wl_mac_addr_t bssid; /**<  The BSSID of the network. */
        uint8_t queued_pkt_cnt;     /**< Number of queueud packets for 
                                       this STA. */
        uint8_t in_ps;              /**< Is the STA in power save mode. */
        uint8_t aid;                /**< STA AID */
};

/* Station list representation. Array of pointers to wl_sta_t entries. */
struct wl_sta_list_t
{
        struct wl_sta_t **sta; /**< The list of pointers to stations */
        size_t cnt;            /**< Number of stations               */
};

/*! \brief Get the list of currently associated stations (SoftAP).
 *
 * Retrieves the list of current stations from 
 * the driver. 
 *
 * This function is not thread safe. It must be called in the
 * same execution context as wl_poll().
 *
 * @param network_list Output buffer. The API call returns
 *        a pointer to allocated memory containing the network list.
 * @return
 * - WL_SUCCESS
 * - WL_FAILURE.
 */
wl_err_t wlap_get_sta_list(struct wl_sta_list_t **network_list);


/*! Callback used to read data from a TX packet.
 * This function is supplied by the user of the API.
 *
 * @param dst Destination buffer. The data should be copied
 *  to this buffer.
 * @param src_handle Handle to the source packet from where
 *  the data should be copied. This handle is the same one that
 *  is passed in parameter \a pkt_handle to \a wl_process_tx().
 * @param read_len Number of bytes to copy from \a src_handle
 *  to \a dst.
 * @param offset The offset in bytes, counting from the
 *  beginning of the Ethernet header, from where to copy data.
 * @return
 *  - The number of bytes copied. This number may be smaller
 *    than the length requested in \a read_len but it may not
 *    be shorter than the length of the packet counting from
 *    \a offset. In other words, if the caller of this function
 *    receives a return count that is shorter than \a read_len
 *    he will assume that all packet data has been read.
 * - < 0 on error.
 */
typedef ssize_t (*wl_pkt_read_cb_t)(char *dst,
                                    void *src_handle,
                                    size_t read_len,
                                    int offset);

/*! \brief Register a data access function for TX packets (SoftAP).
 *
 * When a TX data packet has a different representation than a single
 * contiguous buffer in memory then a packet read function must be
 * implemented and registered with this call. Whenever the library
 * needs to read packet data it will call this function to do it.
 *
 * This function can be ignored if the TX packet representation is
 * a single contiguous buffer. This function is only needed in SoftAP
 * mode.
 *
 * @param pkt_read_cb Read callback. 
 * @param ctx Context 
 */
void wl_register_pkt_read_cb(wl_pkt_read_cb_t pkt_read_cb);

/*! \brief Start a network using the SoftAP mode.
 *
 * This call will cause the WiFi chip to start sending beacons
 * and accept associations from WiFi stations.
 *
 */
wl_err_t wlap_start_ap(const char *ssid,
                     const size_t ssid_len,
                     const uint8_t channel,
                     const enum wl_auth_mode auth_mode,
                     const enum wl_enc_type enc_type);

/*! \brief Disconnect a STA (SoftAP)
 * 
 * @param bssid The BSSID of the station to disconnect.
 * @return 
 *  - WL_SUCCESS
 *  - WL_FAILURE.
 */
wl_err_t wlap_disconnect_sta(const struct wl_mac_addr_t bssid);


/*! @} */ /* End wl_softap group */


#endif
