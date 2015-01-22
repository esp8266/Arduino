/*
 *  Programming interface for wl_api.
 *  Copyright (C) 2010 HD Wireless AB
 *
 *  You should have received a copy of the license along with this library.
 */

/*! \file wl_api.h *************************************************************
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
polled by calling the \a wl_tick() function. When \a wl_tick() is called
wl_api reacts to any received management frames, expires any internal timers and
performs any other tasks necessary for forward progress. After
\a wl_tick() returns nothing will happen unless it or some other wl_api
function is called again. Also, to send and receive data, the \a wl_process_rx()
and \a wl_process_tx() must be invoked upon reception and transmission of data.

The third important property is that wl_api is \b not \b thread \b safe. 
All wl_api calls must execute in the same context since the
library has no knowledge of the locking mechanisms available (if any).

\section wl_api_code_examples A note on the code examples

The code examples illustrate how to call the different wl_api functions.
They do not constitute a complete program. Functions with the prefix "app_" 
in the code examples are application specific calls that illustrate a
particular action. These functions are not part of the API and will have
to be implemented if needed. For a complete working code example see
one of the H&D Wireless software reference designs, such as the WiFi HTTP 
server demo code in the Atmel Software Framework.

The API is structured into these functional groups:

\li \ref wl_api
\li \ref wl_wifi
\li \ref wl_data
\li \ref wl_transport
\li \ref wl_custom

Also documented here is the transport layers for SPI and SDIO.
There interfaces are only necessary when porting the library to
a new hardware platform.

\li \ref wl_spi
\li \ref wl_sdio

 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.hd-wireless.se/">H&D Wireless</A>.\n
 * Support and FAQ: http://www.atmel.com/
 */
 
#ifndef WL_API_H
#define WL_API_H
 
#define WL_API_RELEASE_NAME "v2.7.0"

/*! Maximum size of a SSID */
#define WL_SSID_MAX_LENGTH 32
/*! Size of a MAC-address or BSSID */
#define WL_MAC_ADDR_LENGTH 6
/*! Maximum length of a passphrase */
#define WL_MAX_PASS_LEN 64
/*! Indicates that there is no SNR information */
#define WL_SNR_UNKNOWN -128

#define SPB104 104
#define SPB105 105
 
/*! \ingroup wl_api 
 * API Error codes */
typedef enum {
        WL_FAILURE = -1,
        WL_SUCCESS = 1,
        WL_NOEFFECT,
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
        WL_EVENT_SCAN_COMPLETE,
        WL_EVENT_FAILURE,
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

enum wl_host_attention_mode {
        WL_HOST_ATTENTION_SDIO = 0x1, /*!< For SDIO or polled SPI */
        WL_HOST_ATTENTION_SPI  = 0x5a /*!< For SPI with interrupt line */
};

/*! \ingroup wl_wifi 
 * Event descriptor 
*/
struct wl_event_t {
        enum wl_event_id_t id;  /**< Event identifier. */

};

/*! \ingroup wl_wifi
 * Infrastructure (ESS) or Ad-hoc (IBSS) connection modes.
 */
enum wl_conn_type_t {
        WL_CONN_TYPE_INFRA, /*!< For infrastructure mode (default) */
        WL_CONN_TYPE_ADHOC  /*!< For ad-hoc mode */
};

/* Note:
 * If your environment does not have stdint.h you will have to
 * define the fixed-width integer types specified in that file
 * yourself, make sure that those definitions are included 
 * before any inclusions of wl_api.h, and build with the macro
 * WITHOUT_STDINT defined. In this case the wl_api library
 * must have been built with the same integer type definitions.
 */

#ifndef WITHOUT_STDINT
#include <stdint.h>
#endif

/* Note:
 * If your environment does not have stdio.h you will have to define
 * the size_t type yourself, make sure that that definition is
 * included before any inclusions of wl_api.h, and build with the
 * macro WITHOUT_STDIO defined. In this case the wl_api library must
 * have been built with the same size_t type definition.
 */
#ifndef WITHOUT_STDIO
#include <stdio.h>
#endif

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
};

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
        uint8_t channel; /**< The wlan channel which the network uses */
        uint32_t beacon_period; /**< Beacon period for the network */
        uint16_t dtim_period; /**< DTIM period for the network */
        int32_t rssi; /**< Received Signal Strength in dBm (measured on beacons) */
        int32_t snr; /**< Received Signal to noise ratio in dBm (measured on beacons) */
        uint8_t enc_type; /**< The encryption type used in the network. */
    
        enum wl_conn_type_t net_type; /**< Type of network (Infrastructure or Ad-Hoc */
        size_t ie_len;    /**< Always 0 unless wl_api has been built with WL_CONFIG_WPA_SUPPLICANT */
                             
        uint8_t ie[0]; /**< Not used unless wl_api has been built with WL_CONFIG_WPA_SUPPLICANT */
};

/*! \ingroup wl_wifi 
 * Network list representation. Array of pointers to wl_network_t entries.
 *
 */
struct wl_network_list_t
{
        struct wl_network_t **net; /**< The list of pointers to networks */
        size_t cnt;                /**< Number of networks */
};

#define WL_RATE_1MBIT      2
#define WL_RATE_2MBIT      4
#define WL_RATE_5_5MBIT    11
#define WL_RATE_6MBIT      12
#define WL_RATE_9MBIT      18
#define WL_RATE_11MBIT     22
#define WL_RATE_12MBIT     24
#define WL_RATE_18MBIT     36
#define WL_RATE_22MBIT     44
#define WL_RATE_24MBIT     48
#define WL_RATE_33MBIT     66
#define WL_RATE_36MBIT     72
#define WL_RATE_48MBIT     96
#define WL_RATE_54MBIT     108
#define WL_RATE_NUM_RATES  14
#define WL_RATE_INVALID    WL_RATE_NUM_RATES

/*! \ingroup wl_wifi 
 *
 * Rate representation 
 *
 */
typedef uint8_t wl_rate_t;
    
/** \defgroup wl_api Library support functions
 *
 * These functions manage the library in general. They concern initalizing
 * the library, downloading firmware to the WiFi chip and handling events
 * from the library.

For this example we assume that the application is running stand-alone
without an operating system.

Before the library can do anything it needs to start up the WiFi
hardware by downloading a firmware image. The firmware image is
relatively big (around 144kB) and is therefore not included in the library 
it is only needed once. It is up to the application to decide where to
store the firmware image and how to read it into the wl_api library.

Step one is to write a function of the type \a ::wl_fw_read_cb_t
that wl_api will call to retrive the firmware image. Assuming that you
have some spare RAM (or whatever memory type is used for read only
data, such as FLASH memory) on your platform you can simply include
the firmware image from the \a wl_fw.h header file and write a
firmware read function like this

\code
static size_t fw_read_cb(void* ctx, 
                         uint8_t** buf, 
                         size_t offset, 
                         size_t len)
{
        if ( NULL == buf ) {
                return 0;
        }
        *buf = ((uint8_t*) fw_buf) + offset;
        if ( len > ( fw_len - offset ) ) {
                return fw_len - offset;
        }
        return len;
}

\endcode

If the firmware image is stored in ROM this function may have to read
it back block by block instead.

First, firmware must be downloaded to the device

\code
if ( wl_transport_init(fw_read_cb, NULL, &mode) != WL_SUCCESS ) {
        app_error("Firmware download failed");
        return 0;
}
\endcode

The wl_api library is then initialized like this 

\code
if ( wl_init(NULL, init_complete_cb, mode) != WL_SUCCESS ) {
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

Similar to \a wl_poll(), there is also a \a wl_tick() function that takes a
free running "tick" counter with millisecond resolution as an argument so
that it can trigger internal timers when necessary. Assuming that such a tick
counter is provided by the macro GET_MS_TICK() the wl_api execution loop becomes

\code
while (TRUE) {
       wl_tick(GET_MS_TICK());
       wl_poll();
}
\endcode

In a stand-alone application this loop would usually be the main application
loop and include application specific calls as well.

After some number of main loop iterations the init_complete_cb will be
invoked and the application can initialize its IP stack.

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
 * This function should be called after firmware has been downloaded to the
 * device.
 *
 * @param ctx Opaque context pointer that will be passed to the callback
 *            when invoked. This parameter is never accessed by the API.
 * @param init_complete_cb callback function to invoke when initialization is 
 *        complete.
 * @param mode Indicates the host attention mode used by the device. If
 *         \a wl_transport_init() was used to download the firmware image to the
 *         device, the proper mode can be obtained from the mode parameter of
 *         that function.
 *
 * @return 
 * - WL_SUCCESS 
 * - WL_FAILURE
 */
wl_err_t wl_init(void* ctx, wl_init_complete_cb_t init_complete_cb,
                 enum wl_host_attention_mode mode);


/*! \brief Shutdown the wl_api library and free resources.
 *
 * \a wl_init() must be invoked to startup the library
 * again.
 *
 * @return 
 * - WL_SUCCESS on success
 * - WL_FAILURE
 *
 */
wl_err_t wl_shutdown(void);


/*! \brief WiFi driver timer tick function
 *
 * Periodic timers are triggered from this function so it should be called as
 * often as possible if precision timing is required (traffic timeouts,
 * authentication timeouts etc).
 *
 * @param tick A tick count in us. This is used to expire timers
 *  in the driver.
 */
void wl_tick(uint32_t tick);

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
              case WL_EVENT_SCAN_COMPLETE:
                     struct wl_network_list_t *netlist;
                     uint8_t netcnt;
                     
                     wl_get_network_list(&netlist);
                     netcnt = netlist->cnt;
                     while (--netcnt) {
                             print_network(netlist->net[netcnt]);
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
              case WL_EVENT_SCAN_COMPLETE:
                     struct wl_network_list_t *netlist;
                     uint8_t netcnt;
                     
                     wl_get_network_list(&netlist);
                     netcnt = netlist->cnt;
                     while (--netcnt) {
                             print_network(netlist->net[netcnt]);
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
 * Currently, there's a limit on the scan list size that depends on the
 * architecture (6 networks for the AVR32 UCR1 architecture 16 networks for
 * other architectures. If more network exist, the strongest networks are
 * chosen. Note that the limitation on the scan list size does not limit the
 * networks which the device can connect to. See wl_connect() for more
 * details.
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
 * @param network_list Output buffer. The API call returns
 *        a pointer to allocated memory containing the network list.
 * @return
 * - WL_SUCCESS
 * - WL_FAILURE.
 */
wl_err_t wl_get_network_list(struct wl_network_list_t **network_list);

#ifdef WFE_6_12
/*! \brief Start a Ad-hoc network.
 *
 * Attempt to start a Ad-hoc (IBSS) network. If a Ad-hoc network
 * is successfully started then a WL_EVENT_MEDIA_CONNECTED event
 * will be raised once the first peer station connects to the Ad-hoc
 * network (and not when the network is announced on the air).
 *
 * If a Ad-hoc network should be started with encryption 
 * enabled then \a wl_set_passphrase() should be called before
 * \a wl_start_adhoc_net() to configure the security parameters.
 * The Ad-hoc network is started with the security parameters
 * (if any) that was configured for the specified \a ssid.
 *
 * @param ssid The SSID of the new network. If there's a network
 *             already present with this SSID this call will fail.
 * @param channel The channel to use. Valid channels are 1-14
 * @param auth_mode The authentication mode to use. Supported
 *        authentication modes for Ad-hoc networks are 
 *        AUTH_MODE_OPEN_SYSTEM and AUTH_MODE_SHARED_KEY.
 *        Passing other modes will cause a WL_INVALID_ARGS return.
 *        If AUTH_MODE_SHARED_KEY is used then a valid WEP
 *        key must be set with a call to \a wl_add_wep_key()
 *        and the default WEP key index must be set with a
 *        call to \a wl_set_default_wep_key().
 * @return 
 *        - WL_SUCCESS on success.
 *        - WL_INVALID_ARGS if the ssid is malformed, if
 *          the channel not valid or if the authentication mode
 *          is invalid.
 *        - WL_RETRY if the driver is busy resolving a conflicting
 *          operation.  The operation should be retried after a wait
 *          (at least one call to wl_poll() for polled implementations).  
 *        - WL_BUSY if the driver is already connected or if a network
 *          with the same SSID is already known.
 *         
 */
wl_err_t wl_start_adhoc_net(struct wl_ssid_t ssid, 
                            uint8_t channel,
                            enum wl_auth_mode auth_mode);
#endif
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
 * If wl_connect() is invoked with a network that is not shown in the
 * scan list, the device will probe for that specific network and connect
 * to it, if found. This is also the method to use in order to connect to
 * "hidden" networks (AP's that doesn't broadcast its SSID).
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
 * If wl_connect_bssid() is invoked with a network that is not shown in the
 * scan list, the device will probe for that specific network and connect
 * to it, if found.
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
 * \brief Packet processing interface.
 *
 * Note that the examples in this group assumes that the transport library
 * functions in the \a wl_transport group are being used. For more information,
 * See the documentation for those functions in the \a wl_transport group. 

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
                                       &vlan);
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
pointing to the beginning of a 802.3 Ethernet frame of length
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
        char wlan_hdr[WL_HEADER_SIZE];
        // The packet must have an Ethernet header
        if (len < ETHERNET_HEADER_SIZE) {
                app_error("Invalid packet length");
                return 0;
        }
        hdr_len = sizeof wlan_hdr;
        status = wl_process_tx(pkt,
                               ETHERNET_HEADER_SIZE,
                               len,
                               wlan_hdr,
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

/*! Size of the wl_api packet header */
#ifdef WFE_6_12
#define WL_HEADER_SIZE 16
#else
#define WL_HEADER_SIZE 14
#endif
    
/*! Maximum packet size (including wl_api headers and paddings)
 *
 * Maximum packet size is obtained with the following data:
 *
 * 1500 bytes of Ethernet payload (MTU) + 14 bytes of Ethernet header +
 * WL_HEADER_SIZE of wl header. This data is then size-aligned to 16.
 * 
 */
#define WL_MAX_PKT_LEN 1536


/*!
 * \brief Process rx packet.
 *
 * Processes a raw rx packet by unencrypting it (if necessary)
 * and stripping headers so as to output a 802.3 frame.
 *
 * wl_process_rx() will strip bytes both from the head and from the tail.
 *
 * Upon return from wl_process_rx(), the pointer at stripped_pkt will
 * point to the start of the Ethernet header, hence adjusting the offset
 * by WL_HEADER_LEN bytes. Any padding (added by the wifi device) will
 * be removed from the tail of the packet, hence making len smaller.
 *
 * The wl_api library of the device will not perform any Ethernet padding
 * removal. The padding removal performed by wl_process_rx() is only for
 * the padding used in the protocol shared by the host and the device.
 * This padding is mainly there to ensure that the host does not have to
 * deal with rx of odd-sized data buffers (which some DMA's have problems
 * to handle).
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
 * Takes a Ethernet II frame header and generates a message passing header
 * for it.
 *
 * The caller should ensure that any frames injected into wl_process_tx()
 * are proper Ethernet frames. The wl_api library or the device will not
 * perform any Ethernet padding if the frames are too short.
 *
 * The Ethernet header is assumed to have the following layout :
 * <dst addr:6><src addr:6><type:2>...
 * The rest of the Ethernet header buffer (if any) is ignored.
 * 
 * A note on the TX packet representation :
 * If your TX packets are simple contiguous buffers you can ignore
 * the rest of this note and pass NULL in parameter \a pkt_handle.
 * A TX packet may have a more complex structure than a RX packet
 * (which must be a contiguous, flat buffer). The IP stack may
 * for example represent a packet as a linked list of buffers where
 * the Ethernet header, the IP header and other headers, are represented
 * by separate buffers. In some cases, such as when the driver is
 * running in SoftAP mode, a TX packet has to be copied and queued 
 * internally for later processing and to support this when packets
 * have a complicated structure a special data access function can
 * be registered. See \a wl_register_pkt_read_cb() for details.
 * If you use \a wl_process_tx() with non-simple packets you
 * should pass a handle to the packet in parameter \a pkt_handle
 * and register an access function with \a wl_register_pkt_read_cb().
 *
 * @param eth_hdr Input buffer (Ethernet header)
 * @param eth_hdr_len Input buffer length (must be >= 14)
 *  This is usually the same as pkt_len unless e.g linked list or buffers
 *  chained in other ways are being used.
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
 * @param pkt_handle A handle to the complete packet. If this parameter
 *  is NULL then \a eth_hdr is expected to point to the whole packet
 *  in a single contiguous buffer (the default). If a different packet
 *  representation is used this parameter should be a handle to the 
 *  complete packet and will be passed unmodified to the data
 *  access function that was registered with \a wl_register_pkt_read_cb().
 *
 * @returns 
 *          - WL_FAILURE
 *          - WL_RESOURCES if packet can not be processed at the moment.
 *                       The caller must either drop the packet or try
 *                       retransmit it later.
 *          - WL_AVAIL     if network not available
 *          - WL_SUCCESS   if packet is ready for transmission through wl_tx().
 */
wl_err_t wl_process_tx(char *eth_hdr, 
                       size_t eth_hdr_len, 
                       size_t pkt_len, 
                       char *hdr,
                       uint16_t vlanid_prio,
                       void *pkt_handle);

 
/*! \brief Get current TX and RX rate used for data transfer
 *
 * During transmission and reception of data, the actual rate used will depend
 * on the signal quality. This function can be used to get the actual rate used
 * for the last tx and rx data.
 *
 * @param tx will hold the tx rate upon successful return.
 * @param rx will hold the rx rate upon successful return.
 *
 * @return
 * - WL_SUCCESS on success
 * - WL_FAILURE on failure.
 */ 
wl_err_t wl_get_rate(wl_rate_t *tx, wl_rate_t *rx);


/*! @} */ /* End wl_data group */


/** \defgroup wl_transport Transport interface
 *
 * \brief Low level transport interface.
 *
 * These functions access the low level transport driver which makes
 * the application independent of the actual physical transport
 * layer (usually SDIO or SPI).
 *
 
For applications running on an real time kernel or without an
operating system, the provided transport library will fit right into the
application design. However, when running on a more complex operating system
(such as windows or linux) which has its own transport primitivies and
components (and probably its own IP stack) it might be preferred to design a
custom transport library for that specific environment. Therefore, these
transport interface functions are fully optional.


 *  @{
 */

#define WL_RX_MIN_PKT_LEN    32


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
 * This function should read the specified number of bytes of the
 * firmware image starting at the specified \a offset. The number of
 * bytes to read is given in \a len. Upon return, \a buf should point
 * to a buffer which holds the read data and the number of valid bytes
 * in \a buf is returned from the call.
 *
 * This function will be called repeatedly until the complete firmware
 * image has been read.
 * 
 * This function may be called again at any time while the driver is
 * running to download further pieces of the WiFi firmware image as
 * needed by the runtime requirements. This will normally only happen
 * when the driver switches between networks of different kinds such
 * as from WEP to WPA, or from ESS to IBSS for example.
 *
 * For convenience, any time a firmware chunk has been completely
 * downloaded this function will be called once with the \a buf
 * parameter set to NULL to indicate that no more data is needed right
 * now and that any dynamically allocated buffers which holds firmware
 * data can be freed without much performance impact. 
 *
 * @param ctx Opaque context pointer as provided to \a wl_init() that will be
 *            passed back to the callback.
 * @param buf Should be assigned the address of the buffer holding the read
 *            data upon return. This parameter can be NULL which indicates
 *            that there are no further immediately pending accesses.
 * @param offset Offset in bytes from the start of the firmware image.
 *            Data should be copied into buf starting at \a offset.
 * @param len The number of bytes to copy into \a buf. 
 * @return The number of bytes copied into buf. This may be smaller than
 *         \len if the implementation of the function so requires.
 */
typedef size_t (wl_fw_read_cb_t)(void *ctx, 
                                 const uint8_t **buf, 
                                 size_t offset, 
                                 size_t len);


/*! \brief Initialize the transport interface and download the WiFi firmware
 * image to the device.
 *
 * This operation will proceed synchronously until the firmware is completely
 * downloaded. wl_init() should be called after this function has returned to
 * perform device initialization.
 *
 * @param fw_read_cb callback function to invoke during firmware download.
 * @param ctx Opaque context pointer that will be passed to the callbacks
 *            when they are invoked. This parameter is never
 *            accessed by the API.
 * @param mode will hold the host attention mode used by the transport layer.
 *         This parameter can be passed directly to \a wl_init().
 *
 * @return 
 *
 * - WL_CARD_FAILURE if the wl hardware device is not available
 * - WL_FIRMWARE_INVALID if the firmware obtained through fw_read_cb is
 *                       invalid.
 * - WL_OOM if the necessary memory could not be allocated.
 */
wl_err_t wl_transport_init(wl_fw_read_cb_t *fw_read_cb,
                           void *ctx,
                           enum wl_host_attention_mode *mode);
 
/*! \brief WiFi driver forward progress function
 *
 * This function must be called in polled environments to
 * ensure forward progress. The call can be made as often as possible from
 * the main application loop. However, the call will not have any effect unless
 * there is an interrupt pending from the hardware.
 *
 * In interrupt mode, wl_poll() must be called if no interrupt
 * handler is registered through wl_register_rx_isr(). When an interrupt
 * handler is registered, it is no longer necessary to invoke wl_poll().
 *
 * Note that this function should not be invoked from interrupt context.
 *
 */
void wl_poll(void);

 
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

 
/*! \brief Configure data alignment
 *
 * This function can be used if the host SDIO/SPI controller has certain
 * requirements on the data transfer sizes that can be used on the SDIO/SPI bus.
 *
 * If the txsize parameter is non-zero, additional padding data should be added
 * when performing the low level transfer of data buffer of sizes that are not
 * a multiple of the size_align parameter. See \ref wl_sdio and \ref wl_spi for
 * more information.
 *
 * @param txsize will configure the size alignment for tx data.
 *
 */
void wl_conf_alignment(uint8_t txsize);


/*! @} */ /* End wl_transport group */


/** \defgroup wl_custom Custom environment support
 *
 * \brief Support for custom environments
 *
 * These functions should only be used in cases where the transport library is
 * not used at all. This usually applies to operating systems and environments
 * where there already exists a transport layer framework, e.g. linux or
 * windows.
 *
 *

Note that the \a wl_poll() function is part of the transport library. Therefore,
it should not be used in custom environments. Therefore, it is necessary to
implement a custom polling or interrupt based scheme to ensure that any
incoming packets are processed by the core.

 *  @{
 */
 
 /*! \brief Wakeup callback function.
 *
 * Invoked when the WiFi device should wake up from power save mode.
 * This function should send the proper commands to the device.
 *
 * Note that this type should only be used in custom environments, where
 * the transport library is not used.
 *
 * @param ctx Opaque context pointer as provided to \a wl_register_wakeup_cb()
 *            that will be passed back to the callback.
 * @param wakeup indicates whether wakeup should be set or cleared in the
 *               device.
 */
typedef void (wl_wakeup_cb_t)(void* ctx, uint8_t wakeup);

/*! \brief Register wakeup callback function.
 *
 * Register a function that will be invoked when the WiFi device should wake
 * up from power save mode.
 *
 * Note that this function should only be used in custom environments, where
 * the transport library is not used.
 *
 * @param wakeup_cb Will be invoked when the device should wakeup from sleep
 *                 mode.
 * @param ctx Opaque context pointer that will be passed back to the callback.
 */
void wl_register_wakeup_cb(wl_wakeup_cb_t *wakeup_cb, void *ctx);
 

/*! \brief Management tx callback function.
 *
 * Invoked when the a management message should be transmitted to the
 * WiFi device. This function should ensure that the message is passed through
 * to the device and should never fail.
 *
 * Note that this type should only be used in custom environments, where
 * the transport library is not used.
 *
 * @param ctx Opaque context pointer as provided to \a wl_register_mgmt_tx_cb()
 *            that will be passed back to the callback.
 * @param buf Points to the buffer which holds the management data,
 * @param len Size of the buffer.
 */
typedef void (wl_mgmt_tx_cb_t)(void *ctx, const uint8_t *buf, uint16_t len);


/*! \brief Register management tx callback function
 *
 * Register a function that will be invoked when a management message should
 * be transmitted to the device.
 *
 * Note that this function should only be used in custom environments, where
 * the transport library is not used.
 *
 * IMPORTANT : In a custom environment without a transport library \a
 *             wl_register_mgmt_tx_cb() \b must have been called
 *             before \a wl_fw_download() is called since \a
 *             wl_fw_download() depends on the \a mgmt_tx_cb() to send
 *             the firmware data to the WiFi chip.
 *
 * @param mgmt_tx_cb The callback function to invoke.
 * @param ctx Opaque context pointer that  will be passed back to the callback.
 */
void wl_register_mgmt_tx_cb(wl_mgmt_tx_cb_t *mgmt_tx_cb, void *ctx);



/*! \brief Download the WiFi firmware image to the device.
 *
 * This operation will proceed synchronously until the firmware is completely
 * downloaded. wl_init() should be called after this function has returned to
 * perform device initialization. This function depends on \a
 * wl_register_mgmt_tx_cb(). See that function for details.
 *
 * @param ctx Opaque context pointer that will be passed to the callbacks
 *            when they are invoked. This parameter is never
 *            accessed by the API.
 * @param fw_read_cb callback function to invoke during firmware download.
 *
 * @return 
 *
 * - WL_CARD_FAILURE if the wl hardware device is not available
 * - WL_FIRMWARE_INVALID if the firmware obtained through fw_read_cb is
 *                       invalid.
 * - WL_OOM if the necessary memory could not be allocated.
 */
 wl_err_t wl_fw_download(wl_fw_read_cb_t *fw_read_cb, void *ctx);

 
 
/*! @} */ /* End wl_custom group */



#endif
