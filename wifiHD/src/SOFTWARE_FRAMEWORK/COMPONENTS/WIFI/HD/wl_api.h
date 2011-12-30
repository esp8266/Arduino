/* This header file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*! \page License
 * Copyright (C) 2009, H&D Wireless AB All rights reserved.
 *
 * The license to use this software in whole and in part and to
 * redistribute it in any form follows with the WiFi HW module from H&D
 * Wireless and is granted under the following restrictions:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. The name of H&D Wireless AB may not be used to endorse or promote
 * products derived from this software without specific prior written
 * permission.
 *
 * 4. The software may only be used together with hardware from H&D
 * Wireless all other use is prohibited.
 *
 * 5. The license to use and redistribute the software is granted
 * together with the purchase of a hardware platform on a one to one
 * basis
 *
 * 6. The binary code may not be reversed engineered or by other means
 * copied to circumvent this license.
 *
 * THIS SOFTWARE IS PROVIDED BY H&D WIRELESS AB ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT
 * SHALL HD WIRELESS AB BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * For more information regarding this software license Contact H&D
 * Wireless AB (support@hd-wireless.se).
 */

/*! \file wl_api.h **************************************************************
 *
 * \brief Basic WiFi API
 *
 * This file provides the wl_api interface.
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
 * This is the documentation for the generic WiFi Driver API \a wl_api.
 *
 * \section files Main Files
 * - wl_api.h : WiFi driver interface.
 * - lib_ucr*_hd_wifi_standalone_v*.*.a - Driver library.
 * 
 */
/** \mainpage wl_api Reference Manual

\image html images/wl_api_block_diagram_small.png "wl_api Architecture"

(o)WL API &copy; is a programming interface for WiFi (802.11). It aims
to be a complete interface for embedded applications wanting to use
wireless as a communications interface. (o)WL API &copy; is shortened
"wl_api" in this document.

wl_api has been designed to meet the following goals :
 \li Simple : The API is as simple as is practicable
   to make it easy to use. 
 \li Minimal size : The API is usable on very resource constrained
   platforms. 
 \li Portable : The API is deployable on any platform with a standards
   compliant C compiler.
 \li OS independent : The API is deployable on systems using a real time
   operating system as well as with applications running on the
   "bare metal" of a hardware platform (that is without an operating system).

As a consequence of these design goals wl_api does not allow very fine
grained control of most parameters relating to 802.11 networks. That
would increase the flexibility of the API while also increasing
complexity and code size. When the underlying WiFi hardware can
support a richer feature set the extra features can be offered as a
add-on library if required.

The wl_api is implemented by two libraries. The core library is
compiled for a hardware platform and is independent of operating
system or IP stack.  The core library contains all WiFi
functionality. The core library is supported by a suite of transport
libraries. The transport libraries implements the hardware
communication layer and are specific to the type of hardware interface
used to connect the host platform to the WiFi hardware. For example,
there are transport libraries for SPI and for SDIO. Only the core
library has a public interface (wl_api.h) but applications will need
to link with both the core library and a transport library matching
the hardware configuration.

\section wl_api_princ Operation Principles

There are three important properties of wl_api to keep in mind when
programming with it.

The first is that wl_api is \b asynchronous. For instance, when the
\a wl_connect() function is called to attempt connection with an access
point it will trigger a sequence of packets being exchanged with the
access point after which, if everything is okay, a connection has been
established. The \a wl_connect() call is asynchronous (or non-blocking)
which means that you don't know if the connection attempt has
succeeded after the call returns. You only know if the sequence was
successfully started or not. To find out if, and when, the connection
attempt was successful you must register an event handler using the
function \a wl_register_event_cb(). This is true of a number of API calls
(which is indicated in their documentation).

The second important property is that wl_api is \b polled.  wl_api
never executes "by itself", since it would then have to support
interrupts, timers, locks and other operating system dependent
features.  Instead all asynchronous processes proceed when wl_api is
polled by calling the \a wl_poll() function. When \a wl_poll() is called
wl_api reacts to any received packets, expires any internal timers and
performs any other tasks necessary for forward progress. After
\a wl_poll() returns nothing will happen unless it or some other wl_api
function is called again.

The third important property is that wl_api is \b not \b thread \b safe. 
All wl_api calls must execute in the same context since the
library has no knowledge of the locking mechanisms available (if any).

The API is structured into these functional groups:

\li \ref wl_api
\li \ref wl_wifi
\li \ref wl_data


 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.hd-wireless.se/">H&D Wireless</A>.\n
 * Support and FAQ: http://www.atmel.com/
 */

#include <stdint.h>
#include <stdlib.h>
#include "spb.h"

#ifndef WL_API_H
#define WL_API_H

#define WL_API_RELEASE_NAME "v2.1.1"

/*! Size of the wl_api packet header */
#define WL_HEADER_SIZE 14 
/*! Maximum packet size (including wl_api headers and paddings */
#define WL_MAX_PKT_LEN 1600
/*! Maximum size of a SSID */
#define WL_SSID_MAX_LENGTH 32
/*! Size of a MAC-address or BSSID */
#define WL_MAC_ADDR_LENGTH 6

/*! \ingroup wl_api 
 * API Error codes */
typedef enum {
        WL_FAILURE = -1,
        WL_SUCCESS = 1,
        WL_OOM,
        WL_INVALID_LENGTH,
        WL_NOT_SUPPORTED,
        WL_ABSORBED,
        WL_RESOURCES,
        WL_BUSY,
        WL_RETRY, /*!< Retry the operation later. The driver is busy
                    resolving an operation that conflicts with the
                    request. */
        WL_INVALID_ARGS,
        WL_AVAIL,
        WL_CARD_FAILURE,        /*!< Could not detect SPB device */
        WL_FIRMWARE_INVALID,    /*!< Invalid firmware data */

} wl_err_t;

/*! \ingroup wl_wifi 
 * Event identifiers */
enum wl_event_id_t {
        WL_EVENT_MEDIA_CONNECTED = 0,
        WL_EVENT_CONN_FAILURE,
        WL_EVENT_MEDIA_DISCONNECTED,
        WL_EVENT_CONN_LOST,
        WL_EVENT_SCAN_COMPLETE,
        MAX_WL_EVENT
};

/*! \ingroup wl_wifi 
 * Authentication modes */
enum wl_auth_mode {
        AUTH_MODE_INVALID,
        AUTH_MODE_AUTO,
        AUTH_MODE_OPEN_SYSTEM,
        AUTH_MODE_SHARED_KEY,
        AUTH_MODE_WPA,
        AUTH_MODE_WPA2,
        AUTH_MODE_WPA_PSK,
        AUTH_MODE_WPA2_PSK
};

/*! \ingroup wl_wifi 
 * Encryption modes */
enum wl_enc_type {  /* Values map to 802.11 encryption suites... */
        ENC_TYPE_WEP  = 5,
        ENC_TYPE_TKIP = 2,
        ENC_TYPE_CCMP = 4,
        /* ... except these two, 7 and 8 are reserved in 802.11-2007 */
        ENC_TYPE_NONE = 7,
        ENC_TYPE_AUTO = 8
};


/*! \ingroup wl_wifi 
 * Event descriptor 
*/
struct wl_event_t {
        enum wl_event_id_t id;  /**< Event identifier. */

};

/*! Struct member alignment macro */
#define ALIGN __attribute__ ((aligned (4))) 

/*! \ingroup wl_wifi
 *
 * \brief SSID representation.
 *
 * The SSID is a binary string and cannot be treated as a
 * C-string safely. An empty SSID is represented by a
 * SSID struct with the len field set to 0.
 */
struct wl_ssid_t
{
        char ssid[WL_SSID_MAX_LENGTH]; /**< Octet array containing the SSID data. */
        uint8_t len; /**< Length of valid data in ssid member.
                      *   Cannot be longer than WL_SSID_MAX_LENGTH. */
} ALIGN;

/*! \ingroup wl_wifi
 *
 * MAC-address/BSSID representation 
 *
 * A broadcast BSSID is one with all octets set to 0xFF.
 */
struct wl_mac_addr_t
{
        uint8_t octet[WL_MAC_ADDR_LENGTH]; /**< Octet array containing the MAC address
                                            * data. This array is always WL_MAC_ADDR_LENGTH bytes.
                                            */
};

/*! \ingroup wl_wifi 
 *
 * Network representation 
 *
 */
struct wl_network_t
{
        struct wl_ssid_t ssid; /**< The SSID of the network. */
        struct wl_mac_addr_t bssid; /**<  The BSSID of the network. */
        int32_t rssi; /**< Received Signal Strength in dBm (measured on beacons) */
        uint8_t enc_type; /**< The encryption type used in the network. */
} ALIGN;
    
/** \defgroup wl_api Library support functions
 *
 * These functions manage the library in general. They concern initalizing
 * the library, downloading firmware to the WiFi chip and handling events
 * from the library.

For this example we assume that the application is running stand-alone
without an operating system.

Before the library can do anything it needs to start up the WiFi
hardware by downloading a firmware image. The firmware image is
relatively big (around 144kB) and is not included in the library since
it is only needed once. It is up to the application to decide where to
store the firmware image and how to read it into the wl_api library.

Step one is to write a function of the type \a ::wl_fw_download_cb_t
that wl_api will call to retrive the firmware image. Assuming that
you have some spare RAM on your platform you can simply include
the firmware image from the \a fw.h header file and write a
firmware read function like this

\code
static void fw_download_cb(void* ctx, uint8_t** buf, uint32_t* len)
{
        *buf = (uint8_t*) fw_buf;
        *len = fw_len;
}
\endcode

If the firmware image is stored in ROM this function may have to read
it back block by block instead.

The wl_api library is initialized like this 

\code
if ( wl_init(NULL, fw_download_cb, init_complete_cb) != WL_SUCCESS ) {
        app_error("Init failed");
        return 0;
}
\endcode

The library startup process will now require \a wl_poll() to be called
a number of times before it can complete. In addition, if the
application needs to know when the startup process has completed so
that it can, for example, start up an IP stack it will have to supply
a valid callback function of the type \a ::wl_init_complete_cb_t as a parameter
to the \a wl_init() call and start polling the wl_api library.

The init complete callback will only be executed during a call to \a wl_poll()
or another wl_api function. This simplifies the implementation since no
internal locking is required and the wl_api library becomes OS-independent.

\code
static void init_complete_cb(void* ctx) {
       init_ip_stack();
}
\endcode

Registering the event callback is straightforward :

\code
if (wl_register_event_cb(event_cb, NULL) != WL_SUCCESS) {
       app_error("Failed to register event handler");
       return 0;
}
\endcode

The \a wl_poll() function takes a free running "tick" counter with
millisecond resolution as an argument so that it can trigger internal
timers when necessary. Assuming that such a tick counter is provided
by the macro GET_MS_TICK() the wl_api execution loop becomes

\code
while (TRUE) {
       wl_poll(GET_MS_TICK());
}
\endcode

In a stand-alone application this loop would usually be the main application
loop and include application specific calls as well.

After some number of main loop iterations the WL_EVENT_INIT_COMPLETE
event is posted and the application can initialize its IP stack.

 *  @{
 */

/*! \brief WiFi event callback.
 * 
 * This function receives WiFi events that the application
 * wants notification of. This function is supplied by the user
 * of the API.
 *
 * @param event Struct describing the type of event and, for some
 *              events, additional information regarding the
 *              status of the event. See wl_event_t for additional
 *              information.
 * @param ctx   A context handle. This handle is passed
 *              untouched to the callback and has the same value
 *              as the context registered with the callback in
 *              wl_register_event_cb().
 */
typedef void (*wl_event_cb_t) (struct wl_event_t event, void* ctx);


/*! \brief WiFi event callback.
 * 
 * This function is invoked in interrupt context when there is new data
 * available from the mac. This function is supplied by the user
 * of the API.
 *
 * This function is typically used only by the TCP/IP stack driver.
 *
 * @param ctx   A context handle. This handle is passed
 *              untouched to the callback and has the same value
 *              as the context registered with the callback in
 *              wl_register_event_cb().
 */
typedef void (*wl_rx_isr_t) (void* ctx);


/*! \brief Firmware access function.
 *
 * Reads the WiFi firmware image. This function is supplied by
 * the user of this API since storage for the firmware image is
 * managed by the application.
 *
 * This function should read a number of bytes of the firmware of the firmware
 * image. The number of remaining bytes is given as the input value of
 * \a len. Upon return, \a buf should point to a buffer which holds the read
 * data and \a len should hold the number of bytes read. Hence, \a len is both
 * an input and an output parameter.
 *
 * The offset where reading starts should be incremented sequentially by the
 * ouput value of \a len bytes upon completion of each call.
 *
 * This function will be called repeatedly until the complete firmware image
 * has been read. The last call will have the input value of \a len set to 0
 * to indicate that no more data is needed and that any dynamically allocated
 * buffer which holds firmware data are safe to free.
 *
 * @param ctx Opaque context pointer as provided to \a wl_init() that will be
 *            passed back to the callback.
 * @param buf  Should be assigned the address of the buffer holding the read
 *             data upon return.
 * @param len  Should hold the value of the number of bytes read upon return.
 */
typedef void (wl_fw_download_cb_t)(void *ctx, uint8_t** buf, uint32_t* len);


/*! \brief Initialization complete callback function.
 *
 * Invoked when WiFi initialization is complete.
 *
 * @param ctx Opaque context pointer as provided to \a wl_init() that will be
 *            passed back to the callback.
 */
typedef void (wl_init_complete_cb_t)(void* ctx);


/*! \brief Register an event handler.
 *
 * Register an event handler with the driver. This
 * event handler will be called whenever a event
 * listed in #wl_event_id_t occurs.
 * See #wl_event_cb_t and #wl_event_id_t for more details.
 * 
 * @param cb Event callback function to register.
 * @param ctx Opaque context pointer that will be
 *            passed to the callback when it is
 *            invoked. This parameter is never
 *            accessed by the API.
 * @return WL_SUCCESS
 */
wl_err_t wl_register_event_cb(wl_event_cb_t cb, void* ctx);

/*! \brief Initialize the wl_api library.
 *
 * Note that \a wl_poll() must be called for this function to progress
 * towards complete init
 *
 * The startup process will proceed asynchronously and will inkove
 * init_complete_cb when completed. The callback will not be invoked if any
 * error occurs during initialization.
 *
 * @param ctx Opaque context pointer that will be passed to the callbacks
 *            when they are invoked. This parameter is never
 *            accessed by the API.
 * @param fw_download_cb callback function to invoke during firmware download.
 * @param init_complete_cb callback function to invoke when initialization is 
 *        complete.
 * @return 
 * - WL_SUCCESS 
 * - WL_FAILURE
 * - WL_CARD_FAILURE if the wl hardware device is not available
 * - WL_FIRMWARE_INVALID if the firmware obtained through fw_download_cb is
 *                       invalid.
 */
wl_err_t wl_init(void* ctx, 
                 wl_fw_download_cb_t fw_download_cb,
                 wl_init_complete_cb_t init_complete_cb);

/*! \brief WiFi driver forward progress function
 *
 * This function must be called in stand-alone environments to
 * ensure forward progress. Periodic timers are triggered from this function
 * so it should be called as often as possible if precision
 * timing is required (traffic timeouts, authentication timeouts
 * etc).
 *
 * @param tick A tick count in us. This is used to expire timers
 *  in the driver.
 */
void wl_poll(uint32_t tick);

/*!  @} */


/** \defgroup wl_wifi Connection Management
 *
 * These functions access WiFi-specific functionality such as
 * scanning, connect/disconnect, authentication and encryption,
 * and power save modes.
 *

\section scanning Scanning 

To scan all channels that are available in the current regulatory
domain 

\code
       if ( wl_scan() != WL_SUCCESS ) {
              // May be busy scanning already, no fatal error
              return 0;
       }
\endcode

Since wl_scan() only starts the scanning process the application
should add code to the event handler to catch the "scan complete" event
and retrieve the list of seen networks from the library

\code
static void event_cb(struct wl_event_t event, void* ctx) {
       switch(event.id) {
              case WL_EVENT_INIT_COMPLETE:
                     app_init_ip_stack();
                     break;
              case WL_EVENT_SCAN_COMPLETE:
                     struct wl_network_t* netlist;
                     uint8_t netcnt;
                     
                     wl_get_network_list(&netlist, &netcnt);
                     while (--netcnt) {
                             print_network(netlist + netcnt);
                     }
                     break;
       }
}
\endcode

The function print_network() could display the network name, the SSID, in
a user interface. It is important to keep in mind is that despite the fact 
that the SSID is usually presented as a ASCII string, it is
in fact just a byte string and can legally contain all kinds of
non-printable characters, including a 0-byte. This means that it is
easy to end up with buffer overrun bugs if the SSID is ever treated
as a normal string without precautions.

\code
void print_network(struct wl_network_t* wl_network)
{
        char ssid[WL_SSID_MAX_LENGTH + 1];
        memset(ssid, 0, sizeof(ssid));
        memcpy(ssid, wl_network->ssid.ssid, wl_network->ssid.len);
        if (app_is_printable(ssid)) {
                app_print("\"%s\" ", ssid);
        }
        else {
                app_print("<binary SSID> ");
        }
        switch (wl_network->enc_type) {
        case ENC_TYPE_WEP :
                app_print("(WEP encryption)");
                break;
        case ENC_TYPE_TKIP :
                app_print("(TKIP encryption)");
                break;
        case ENC_TYPE_CCMP :
                app_print("(CCMP encryption)");
                break;
        }
        app_print("\n");
}
\endcode

\section connecting Connecting

To connect to an access point (beware binary SSIDs) the connection process
must be started

\code
         if ( wl_connect("My AP", strlen("My AP"))
              != WL_SUCCESS ) {
              app_error("Connection failed.\n");
              return 0;
         }
\endcode

and the \a WL_EVENT_MEDIA_CONNECTED and \a WL_EVENT_CONN_FAILURE events should be
caught. To detect that a connection is terminated after it has been successfully established
(such as when the AP goes out of range) the \a WL_EVENT_MEDIA_DISCONNECTED event
must be also be caught


\code
static void event_cb(struct wl_event_t event, void* ctx) {
       switch(event.id) {
              case WL_EVENT_INIT_COMPLETE:
                     app_init_ip_stack();
                     break;
              case WL_EVENT_SCAN_COMPLETE:
                     struct wl_network_t* netlist;
                     uint8_t netcnt;
                     
                     wl_get_network_list(&netlist, &netcnt);
                     while (--netcnt) {
                             print_network(netlist + netcnt);
                     }
                     break;
              case WL_EVENT_CONN_FAILURE:
                     app_error("Connection failed\n");
                     break;
              case WL_EVENT_MEDIA_CONNECTED:
                     app_print("Connected to Access Point\n");
                     app_ip_interface_up();
                     break;
              case WL_EVENT_MEDIA_DISCONNECTED:
                     app_print("Disconnected from Access Point\n");
                     app_ip_interface_down();
                     break;
       }
}
\endcode

\section security Security

To use WEP a WEP key must be added before the connection is initiated.
To set the 40-bit WEP key 0xDEADBEEF00 as default key for key index 0 do

\code
         char key[5] = { 0xDE, 0xAD, 0xBE, 0xEF, 0x00 };
         struct wl_mac_addr_t bssid;

         // This means that the bssid is a broadcast bssid and the WEP key will be a default key instead of a key-mapping key.
         memset(&bssid.octet, 0xff, sizeof bssid.octet);

         if ( wl_add_wep_key(0, sizeof key, key, &bssid) 
              != WL_SUCCESS ) {
              app_error("Failed to add WEP key.");
              return 0;
         }
\endcode

If Shared Key authentication is required then this is configured through
a call to \a wl_set_auth_mode(). 

\code
        if ( wl_set_auth_mode(AUTH_MODE_SHARED_KEY) != WL_SUCCESS) {
              app_error("Failed to set authentication mode.");
              return 0;
        }
\endcode

It is important to know that \a wl_set_auth_mode() sets a \b forcing
authentication mode. If the mode is set to AUTH_MODE_SHARED_KEY then
all connection attempts made will fail if the target of the connection
attempt does not support Shared Key authentication. This is true of all
non-WEP Access Points, including Access Points with WPA or WPA2. To 
reset the authentication mode call \a wl_set_auth_mode() with the
mode set to AUTH_MODE_OPEN_SYSTEM (this is the default mode if
\a wl_set_auth_mode() is never called).

To use WPA/WPA2 with a Pre-shared key a passphrase must be associated
with the network before the connection is initiated. 

\code
        struct wl_network_t net;
        char passphrase[] = "MySecretKey";

        memset(&net, 0, sizeof net);
        memset(net.bssid.octet, 0xFF, sizeof net.bssid.octet);
        strncpy(net.ssid.ssid, "My AP", strlen("My AP"));
        net.ssid.len = strlen("My AP");
        net.enc_type = ENC_TYPE_AUTO;
        if (wl_set_passphrase(&net, 
                              passphrase, 
                              strlen(passphrase), 
                              ENC_TYPE_AUTO,
                              AUTH_MODE_AUTO) 
            != WL_SUCCESS) {
                app_error("Failed to add passphrase");
        }
\endcode

The library supports several passphrase-network associations to be
configured simultaneously. Be aware that the \a wl_connect() call
can take up to 15 seconds longer than normal when using a pre-shared
WPA/WPA2 key since the platform must calculate a temporal encryption
key from the passphrase before the connection attempt can start.

 *  @{
 */

/*! \brief Scan all channels.
 * 
 * Starts a scan of all WiFi channels allowed in this regulatory
 * domain. The list of allowed channels (the domain) is adapted to the 
 * channels announced as allowed by the first AP heard.
 *
 * The scan will proceed asynchronously and will raise a
 * WL_EVENT_SCAN_COMPLETE event when completed.
 *
 * @return
 * - WL_SUCCESS
 * - WL_FAILURE.
 */
wl_err_t wl_scan(void);

/*! \brief Get the list of currently known networks.
 *
 * Retrieves the list of currently known networks from 
 * the driver. To ensure that this list is up-to-date
 * a wl_scan() call should be issued and this function
 * should be called upon reception of the WL_EVENT_SCAN_COMPLETE
 * event. This function can be called at other times
 * but the list of networks retrieved then might not
 * correspond to the networks actually in range.
 * 
 * Note that a successful scan does not necessarily
 * find any networks.
 *
 * This function is not thread safe. It must be called in the
 * same execution context as wl_poll().
 *
 * @param network_list Output buffer. The API call returns
 *        a pointer to allocated memory containing \a network_cnt
 *        network entries.
 * @param network_cnt The number of networks
 *        allocated in network_list. 
 * @return
 * - WL_SUCCESS
 * - WL_FAILURE.
 */
wl_err_t wl_get_network_list(struct wl_network_t** network_list, uint8_t* network_cnt);

/*! \brief Connect to a SSID.
 *
 * Attempt to connect to a given SSID. If the driver is already
 * connected to an AP with a different SSID then this call will
 * return WL_BUSY and wl_disconnect() should be called before
 * trying again.
 *
 * The connection process will proceed asynchronously and will raise a
 * WL_EVENT_MEDIA_CONNECTED event when completed, or a WL_EVENT_CONN_FAILURE
 * when failed. After a WL_EVENT_MEDIA_CONNECTED event has been raised
 * a WL_EVENT_MEDIA_DISCONNECT event will be raised if the connection is
 * terminated. Note that this can be caused by external factors and can
 * happen at any time.
 *
 * @param ssid Pointer to the SSID string.
 *             Freed by caller.
 * @param ssid_len Length of the SSID string in octets. Max value is 32.
 * @return 
 *  - WL_SUCCESS
 *  - WL_FAILURE if the network could not be found
 *  - WL_BUSY if the driver is already connected
 *  - WL_RETRY if the driver is busy resolving a conflicting operation.
 *    The operation should be retried after a wait (at least one call to wl_poll()
 *    for polled implementations).
 */
wl_err_t wl_connect(char* ssid, size_t ssid_len);

/*! \brief Connect to a BSSID
 *
 * Attempt to connect to a given BSSID. If the driver is already
 * connected to an AP with a different BSSID then this call will
 * return WL_BUSY and wl_disconnect() should be called before
 * trying again.
 *
 * The connection process will proceed asynchronously and will raise a
 * WL_EVENT_MEDIA_CONNECTED event when completed, or a WL_EVENT_CONN_FAILURE
 * when failed. After a WL_EVENT_MEDIA_CONNECTED event has been raised
 * a WL_EVENT_MEDIA_DISCONNECT event will be raised if the connection is
 * terminated. Note that this can be caused by external factors and can
 * happen at any time.
 *
 * @param bssid Pointer to the BSSID. Freed by caller.
 * @return 
 *  - WL_SUCCESS
 *  - WL_FAILURE if the network could not be found
 *  - WL_BUSY if the driver is already connected
 *  - WL_RETRY if the driver is busy resolving a conflicting operation.
 *    The operation should be retried after a wait (at least one call to wl_poll()
 *    for polled implementations).
 */
wl_err_t wl_connect_bssid(struct wl_mac_addr_t bssid);

/*! \brief Disconnect from the network
 *
 * Disconnect from any currently associated network.
 *
 * The disconnection process will proceed asynchronously and will raise a
 * WL_EVENT_MEDIA_DISCONNECTED event when completed.
 * @return
 *  - WL_SUCCESS if the disconnect process was started
 *  - WL_FAILURE if the driver was not connected
 *  - WL_RETRY if the driver is in the process of connecting.
 *     In this case the disconnect must be retried after
 *     the connection attempt has completed (resulted in a
 *     WL_EVENT_MEDIA_CONNECTED or a WL_EVENT_CONN_FAILURE event).
 */
wl_err_t wl_disconnect(void);

/*! \brief Set the WEP authentication mode of the driver.
 *
 * Set the authentication mode to use. This is only for
 * enforcing Shared Key authentication in WEP networks.
 * The default mode is AUTH_MODE_OPEN_SYSTEM which works
 * with all types of encryption.
 *
 * Note that as long as the mode is set to AUTH_MODE_SHARED_KEY
 * all connections to APs using no encryption or WPA/WPA2 will
 * fail since those require Open System authenticaton.
 *
 * @param mode AUTH_MODE_OPEN_SYSTEM for Open System authentication
 *             (that is no authentication). This should be used for WPA/WPA2
 *             connections.
 *             AUTH_MODE_SHARED_KEY for WEP shared key authentication.
 *             Other values of \a mode are invalid with this call
 *             and will cause WL_FAILURE to be returned.
 * @return
 *      - WL_SUCCESS on success. 
 *      - WL_FAILURE on failure.
 */
wl_err_t wl_set_auth_mode(enum wl_auth_mode mode);

/*!
 * @brief Add a WEP encryption key to the device.
 *
 * Configure a key into the device. The key type (WEP-40, WEP-104)
 * is determined by the size of the key (5 bytes for WEP-40, 13 bytes for WEP-104).
 *
 * @param key_idx The key index to set. Valid values are 0-3.
 * @param key_len Length of key in bytes. Valid values are 5 and 13.
 * @param key Key input buffer.
 * @param bssid BSSID that the key applies to. If this is
 *        the broadcast BSSID then the key is configured
 *        as one of the default keys (not _the_ default key,
 *        this must be set by calling set_default_wep_key()
 *        after adding it). If the BSSID is a valid unicast
 *        bssid then the key is configured as a key-mapping
 *        key ( See 802.11-2007 8.2.1.3 ).
 * @return 
 * - WL_SUCCESS on success.
 * - WL_INVALID_LENGTH if the key length is bad.
 * - WL_FAILURE on failure
 */
wl_err_t wl_add_wep_key(uint8_t key_idx, 
                        size_t key_len, 
                        const void *key, 
                        struct wl_mac_addr_t *bssid);

/*! @brief Set the default WEP key index.
 *
 * Select which WEP key to use for transmitted packets.
 * For this to work correctly you must have added a WEP
 * key with \a wl_add_wep_key() as a default key, using the
 * same index as the one set in this call.
 * @param key_idx Index of the key to make the default key.
 *        Valid values are 0-3.
 * @return WL_SUCCESS or WL_FAILURE.
 */
wl_err_t wl_set_default_wep_key(uint8_t key_idx);

/*! \brief Delete a WEP key.
 *
 * Deletes a WEP key from the driver.
 *
 * @param key_idx The index of the key to delete. Valid values are 0-3.
 * @param bssid BSSID that the key applies to. If this is
 *        the broadcast BSSID then the key deleted is a default key.
 *        If the BSSID is a valid unicast bssid then the deleted
 *        key is a key-mapping key.
 * @return WL_SUCCESS or WL_FAILURE
 */
wl_err_t wl_delete_wep_key(uint8_t key_idx, struct wl_mac_addr_t *bssid);

/*! @brief Set a WPA/WPA2 passphase
 *
 * Associate a WPA/WPA2/RSN passphrase with a network.
 * The number of passphrases that can be stored can
 * vary but is always at least one. Passphrases can
 * be added until \a wl_add_wpa_passphrase() returns
 * WL_RESOURCES.
 *
 * @param net Network with which to associate the passphrase.
 * @param passphrase Passphrase. Valid characters in a passphrase
 *        must lie between ASCII 32-126 (decimal).
 * @param len Length of passphrase. Valid lengths are 8-63.
 * @param enc_type Encryption type. If this is set to ENC_TYPE_AUTO
 *        then the most secure supported mode will be automatically
 *        selected. Normally you only need to pass something else here
 *        if you need to enforce picking a certain encryption mode when
 *        the network supports several modes and you don't want to use
 *        the best one.
 * @param auth_mode Authentication mode. If this is set to AUTH_MODE_AUTO
 *        then the most secure mode will be automatically selected.
 *        Normally you only need to pass something else here if the network
 *        announces support for both WPA and WPA2/RSN and the passphrases are
 *        different.
 * @return 
 *        - WL_SUCCESS
 *        - WL_INVALID_ARGS if the passphrase length is invalid.
 *        - WL_RESOURCES if no more passphrases can be added.
 */
wl_err_t wl_set_passphrase(const struct wl_network_t *net,
                           const char *passphrase, 
                           const size_t len,
                           const enum wl_enc_type  enc_type,
                           const enum wl_auth_mode auth_mode);

/*! @brief Remove a WPA/WPA2 passphase
 *
 * Remove a WPA/WPA2/RSN passphrase associated with a network.
 *
 * @param net Network with which to associate the passphrase.
 *        If net is NULL then all stored passphrases will be
 *        cleared.
 * @return 
 *        - WL_SUCCESS
 *        - WL_FAILURE if no passphrase was associated with the net.
 */
wl_err_t wl_clear_passphrase(struct wl_network_t *net);

/*! \brief Enable legacy power save mode
 *
 * Enable legacy power save mode. In legacy power save mode, the device
 * will power down when idle. When connected, the device will wake up to
 * receive beacon frames and any buffered data from the AP. The response
 * time when legacy power save is enabled might therefore be as long as the
 * AP beacon interval (mostly 100 ms). However, the throughput should not
 * be affected.
 *
 * @return WL_SUCCESS or WL_FAILURE.
 */
wl_err_t wl_enable_ps(void);

/*! \brief Disable legacy power save mode
 *
 * @return WL_SUCCESS or WL_FAILURE.
 */
wl_err_t wl_disable_ps(void);

/*! \brief Configure power save parameters.
 *
 * @param use_ps_poll Use PS-Poll frames to retrieve buffered data. Any changes
 *        to this parameter will take effect upon next connect
 *        or when power save is enabled through wl_enable_ps().
 *        Note: To retrieve one buffered packet, the ps poll scheme
 *        needs one ps poll packet to the AP instead of two null
 *        packets in the power management bit scheme. Ps poll avoids
 *        the overhead of traffic monitoring time in active mode as
 *        well. But since each ps poll request can make the AP
 *        release only one buffered packet, it is not the optimal
 *        scheme for applications with heavy downlink traffic.
 * @param ps_traffic_timeout Timeout in [ms] to wait for more buffered data
 *        from AP. This setting has no effect if
 *        use_ps_poll is 1. Any changes to this parameter
 *        will take effect immediately.
 * @param ps_delay Power save will de delayed ps_delay [ms] after connecting to
 *        an AP.
 * @param rx_all_dtim If set to 1, then STA will wake up to listen to every
 *        beacon containing DTIM (delivery traffic indication messages) when
 *        connected. The actual DTIM interval is configured in the AP.
 *        If the DTIM interval, as configured in the AP, is larger than
 *        \a listen_interval, the STA will wakeup according to the
 *        \a listen_interval parameter.
 * @param listen_interval The Listen Interval field is used to indicate to the
 *        AP how often a STA in power save mode wakes to listen
 *        to beacon frames. The value of this parameter is expressed in units
 *        of Beacon Interval. An AP may use the Listen Interval information in
 *        determining the lifetime of frames that it buffers for a STA.
 *        Any changes to this parameter will take effect upon next association.
 *
 * @return WL_SUCCESS or WL_FAILURE.
 */
wl_err_t wl_conf_ps(uint8_t use_ps_poll,  
                    uint32_t ps_traffic_timeout,
                    uint32_t ps_delay,
                    uint8_t rx_all_dtim,
                    uint16_t listen_interval);

    
/*! \brief Get the interface MAC address.
 *
 * Return the 802.3 MAC address of the network interface.
 * 
 * @param buf Output buffer. It must be at least WL_MAC_ADDR_LENGTH
 *            bytes long and only the first WL_MAC_ADDR_LENGTH bytes 
 *            will contain valid data.
 * @return 
 *         - WL_FAILURE if the interface is not up.
 *         - WL_SUCCESS 
 */
wl_err_t wl_get_mac_addr(uint8_t* buf);

/*! \brief Return the associated network.
 * 
 * Return the description of the currently associated
 * network, if any.
 *
 * @return The network description, or NULL of the driver
 *  is unconnected.
 */
struct wl_network_t* wl_get_current_network(void);
/*!  @} */

/** \defgroup wl_data Data Transfer
 *
 * \brief Packet transport interface.
 *
 * These functions access the low level transport driver which makes
 * the application independent of the actual physical transport
 * layer (usually SDIO or SPI).
 *

For the IP stack integration you need to intercept received packets so
they can be sent up the stack and to transmit packets coming down the
stack.

By default the wl_api library discards all data packets.  To receive
them the application must register a rx interrupt service routine (isr) 
using the \a wl_register_rx_isr() function.

\code
static void rx_isr(void* ctx) {
       rx_pending = TRUE;
}
\endcode

Since the rx_isr() function is only called in interrupt context, it is not
safe to perform the actual read directly from rx_isr(). If an OS is used, 
the normal case is to signal a receiver thread to invoke the ip stack 
read function to read the pending data. In a system that runs without an OS 
(as in the example), a flag is set to indicate that wl_rx() can be invoked 
from the ip stack read function next time the ip stack is polled. 
The beginning of a ip stack read function can look like this

\code
static void ip_stack_rx_pkt() {
       char *pkt = malloc(MAX_PKT_SIZE);
       uint16_t len = MAX_PKT_SIZE;

       if (p == NULL) {
              app_error("Out of memory.");
              return;
       }
       wl_rx(pkt, &len);
       if (0 == len) {
              app_error("Packet reception failed.");
              free(pkt);
              return
       }      
}
\endcode

Since the ip_stack_rx_pkt() function should only be called when there is 
actually a packet ready to read you do not have to check the return value 
from \a wl_rx() since it only returns failure if there is no packet ready to
read.

A packet arriving from the WiFi interface can be either a data
packet or a message from the WiFi hardware to the WiFi driver
(which is implemented by the wl_api library). This means that
wl_api must process every packet to decide if it is an internal
message or a data frame that
should be passed up to the application. Data packets are
prefixed with an extra header containing some administrative
information, and may be followed by padding bytes and so
wl_api also needs to strip the extra header and any padding
before the packet can be safely ingested by the IP stack.
All this happens in the function \a wl_process_rx() which \b must
be called on every packet received by a call to \a wl_rx().

Continuing the ip_stack_rx_pkt() example 

\code
       {
                char*  stripped_pkt;
                size_t stripped_pkt_len;
                uint16_t vlan;
                int status;
                
                status = wl_process_rx(pkt,
                                       len,
                                       &stripped_pkt,
                                       &stripped_pkt_len,
                                       &NULL,
                                       &vlan,
                                       NULL);
                if (WL_ABSORBED == status) {
                       // This is normal. The packet was a
                       // wl_api-internal message.
                       free(pkt);
                       return;    
                }
                app_ip_stack_input(stripped_pkt,
                                   stripped_pkt_len,
                                   vlan);
                free(pkt);
        }
}
\endcode
                               
If \a wl_process_rx() decides that the packet was a command it processes
it and returns \a WL_ABSORBED to signal that the packet should
not be used by anyone else. Otherwise stripped_pkt is
pointing to the beginning of a 802.3 ethernet frame of length
stripped_pkt_len. If the IP stack supports VLAN and QoS
the extra VLAN tag should be passed to the IP stack
together with the packet. For IP stacks without this support the VLAN tag 
contents can safely be ignored, but it must still be filled in by \a wl_process_tx().

To register the receive isr 

\code
        wl_register_rx_isr(rx_isr, NULL);
\endcode

Transmitting data packets happens in a similar way but does not
require a callback/isr since the application/IP stack knows when it has
packets to send.

\code
int ip_stack_tx_pkt(char *pkt, size_t len, uint16_t vlan_tag) {
        int status;
        size_t hdr_len;
        char wlan_hdr[WL_HEADER_SIZE];
        // The packet must have an ethernet header
        if (len < ETHERNET_HEADER_SIZE) {
                app_error("Invalid packet length");
                return 0;
        }
        hdr_len = sizeof wlan_hdr;
        status = wl_process_tx(pkt,
                               ETHERNET_HEADER_SIZE,
                               len,
                               wlan_hdr,
                               &hdr_len,
                               vlan_tag,
                               NULL);
        if ( WL_SUCCESS != status ) {
                app_error("Packet processing failed");
                return 0;
        }
        // Transmit the header first
        if (wl_tx(wlan_hdr, hdr_len) != WL_SUCCESS) {
                app_error("Header transmission failed");
                return 0;
        }
        // Then transmit the data packet
        if (wl_tx(pkt, len) != WL_SUCCESS) {
                app_error("Packet transmission failed");
                return 0;
        }
}
\endcode

The final piece of the puzzle in the IP stack integration is
the MAC address of the WiFi interface

\code
        char mac_addr[WL_MAC_ADDR_LENGTH];

        wl_get_mac_addr(mac_addr);
        ip_stack_set_mac_address(mac_addr);
\endcode

 *  @{
 */

/*!
 * \brief Process rx packet.
 *
 * Processes a raw rx packet by unencrypting it (if necessary)
 * and stripping headers so as to output a 802.3 frame.
 *
 * @param pkt Input buffer (raw packet)
 * @param pkt_len Length of the input buffer (in bytes)
 * @param stripped_pkt Pointer to the packet with the
 *  transport header stripped.
 * @param stripped_pkt_len Length of the stripped packet.
 * @param vlanid_prio VLAN ID and 802.1p priority value
 * using following format:
 * <PRE>
 *        1
 *  5|432109876543|210
 *  -+------------+---
 *  0|   VLANID   |PRI
 * </PRE>
 *
 * @returns 
 *          - WL_FAILURE
 *          - WL_ABSORBED if the packet was an internal driver command
 *                      and not a proper data packet. The packet should
 *                      be freed and the stripped_pkt will not point
 *                      to a valid packet.
 *          - WL_SUCCESS
 */
wl_err_t wl_process_rx(char *pkt, size_t pkt_len, char **stripped_pkt, 
                       size_t *stripped_pkt_len, uint16_t *vlanid_prio);

/*! \brief Process tx packet.
 *
 * Prepare tx packet for transmission.
 * 
 * This function is typically used only by the TCP/IP stack driver.
 *
 * Takes a Ethernet II frame header and generates a message passing header for it.
 * The ethernet header is assumed to have the following layout :
 * <dst addr:6><src addr:6><type:2>...
 * The rest of the ethernet header buffer (if any) is ignored.
 * 
 * @param eth_hdr Input buffer (ethernet header)
 * @param eth_hdr_len Input buffer length (must be >= 14)
 * @param pkt_len Length of the complete data packet (in bytes)
 * @param hdr Pointer to the header buffer (must be
 * allocated by the caller). The length of the buffer
 * must be at least WL_HEADER_SIZE bytes.
 * @param vlanid_prio VLAN ID and 802.1p priority value
 * using following format:
 * <PRE>
 *        1
 *  5|432109876543|210
 *  -+------------+---
 *  0|   VLANID   |PRI
 * </PRE>
 * Ignored for legacy association (no WMM)
 *
 * @returns 
 *          - WL_FAILURE
 *          - WL_RESOURCES if packet can not be processed at the moment.
 *                       The caller must either drop the packet or try
 *                       retransmit it later.
 *          - WL_AVAIL     if network not available
 *          - WL_SUCCESS   if packet is ready for transmission through wl_tx().
 */
wl_err_t wl_process_tx(char *eth_hdr, size_t eth_hdr_len, size_t pkt_len, 
                       char *hdr, uint16_t vlanid_prio);



/*! \brief Register RX callback
 *
 * Register function to be called by the low level transport driver
 * when a new packet is available or when there is a state change in the 
 * data path. When invoked, any pending data can be fetched by calling wl_rx().
 *
 * This function is typically used only by the TCP/IP stack driver.
 * Note, the registered function is called in interrupt context.
 *
 * @param isr rx interrup handler. 
 * @param ctx Opaque context pointer that is passed unmodified to the
 * rx_cb callback when it is invoked.
 * 
 * @return WL_SUCCESS
 */
wl_err_t wl_register_rx_isr(wl_rx_isr_t isr, void* ctx);


/*! \brief Read pending packet
 *
 * Read a pending packet from the low level transport driver.
 * The read packet must be passed to the wl_process_rx() function
 * for proper driver operation.
 *
 * @param buf Buffer to read the packet into. This buffer must be
 *            at least WL_MAX_PKT_LEN bytes long.
 * @param len Length of buf in bytes. Contains the length of the
 *            read packet in bytes on output.
 * @return 
 *         - WL_FAILURE if no RX packet is pending.
 *         - WL_SUCCESS
 */
wl_err_t wl_rx(uint8_t* buf, uint16_t* len);

/*! \brief Send processed tx packet 
 *
 * Send a packet to the low level transport driver.
 * This packet has to have been successfully processed by the
 * wl_process_tx() function.
 *
 * @param buf Buffer to send.
 * @param len Length of buf in bytes.
 * 
 * @return 
 *         - WL_FAILURE if the interface is not ready to send.
 *         - WL_SUCCESS if the packet was successfully transmitted.
 */
wl_err_t wl_tx(const uint8_t* buf, uint16_t len);

/*! @} */ /* End wl_data group */



#endif
