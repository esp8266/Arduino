/*
   This example shows the use of the 'DNS forwarder' feature in the DNSServer.
   It does so by combining two examples CaptivePortalAdvanced and
   RangeExtender-NAPT. Additionally the CaptivePortalAdvanced part has a few
   upgrades to the HTML presentation to improve readability and ease of use on
   mobile devices.

   Also for an example of using HTML chunked response, see handleWifi() in
   handleHttp.ino.

   This example starts up in Captive Portal mode by default.
   It starts the SoftAP and NAPT w/o connecting the WLAN side.

   You connect your computer or mobile device to the WiFi Network 'MagicPortal'
   password 'ShowTime'. Your device should shortly notify you of a Captive
   Portal and the need to login. If it fails to do so in a timely maner,
   navigate to http://172.217.28.1/wifi and configure it there.

   Note, until a successful WLAN connection is made all DNS lookups will point
   back to the SoftAP at 172.217.28.1. This is the Captive Portal element of
   this example.

   Once the WLAN is connected, your device should notify you that you are
   connected. This, of course, assumes your WLAN connection has a path to the
   Internet.

   At this stage we are no longer running as a Captive Portal, but a regular
   NAPT. The DNSServer will be running with the DNS forwarder enabled. The
   DNSServer will resolve lookups for 'margicportal' to point to 172.217.28.1
   and all other lookup request will be forwarded to the 1st DNS server that was
   in the DHCP response for the WLAN interface.

   You should now be able to access things on the Internet. The ease of access
   to devices on your home Network may vary. By IP address it should work.
   Access by a hostname - maybe. Some home routers will use the hostname
   supplied during DHCP to support a local DNS table; some do not.

   There is an additional possible complication for using the local DNS, the DNS
   suffix list, this subject is seldom discussed. It is normally handled
   automaticly by the host computers DNS lookup code. For the DHCP case, the
   DHCP server will supply a suffix list, if there is one. Then when a name
   lookup fails and the name does not have a trailing (.)dot the host computer
   will append a suffix from the list and try again, until successful or the
   list is exhaused. This topic I fear can become a TL;DR. A quick wrapup by way
   of an example. On an Ubuntu system run `nmcli dev show eth0 | grep
   IP4\.DOMAIN` that may show you a suffix list. (replace eth0 with your wlan
   interface name) Try adding them to the local name you are failing to connect
   to. For example, assume 'myhost' fails. You see that 'lan' is in the suffix
   list. Try connecting to 'myhost.lan'.

   mDNS names also will not work. We do not have a way to pass those request
   back and forth through the NAPT.

   Note if hostnames are going to work for an ESP8266 device on your home
   Network, you have to have the call to WiFi.hostname(...) before you call
   WiFi.begin().

   In this example the SoftAP in 'Captive Portal' uses the same public address
   that was used in the CaptivePortalAdvanced example. Depending on your devices
   you may or may not be successful in using a private address. A previous
   PR-author discovered a fix that made the CaptivePortalAdvanced example work
   better with Android devices. That fix was to use that public address. At this
   time, this PR-author with a different Android device running the latest
   version of Android has seen no problems in using either. At least not yet :)
   FWIW: My device also works with the original CaptivePortalAdvanced example
   when using a private address. I would suggest keeping the public address
   for a while. At lest until you are confident everything is working well
   before experimenting with a private address.
*/


#if LWIP_FEATURES && !LWIP_IPV6

#include <ESP8266WiFi.h>
#include <lwip/napt.h>
#include <lwip/dns.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>

#define NAPT 1000
#define NAPT_PORT 10

/*
  Some defines for debugging
*/
#ifdef DEBUG_ESP_PORT
#define CONSOLE DEBUG_ESP_PORT
#else
#define CONSOLE Serial
#endif

#define _PRINTF(a, ...) printf_P(PSTR(a), ##__VA_ARGS__)
#define _PRINT(a) print(String(F(a)))
#define _PRINTLN(a) println(String(F(a)))
#define _PRINTLN2(a, b) println(String(F(a)) + b)

#define CONSOLE_PRINTF CONSOLE._PRINTF
#define CONSOLE_PRINT CONSOLE._PRINT
#define CONSOLE_PRINTLN CONSOLE._PRINTLN
#define CONSOLE_PRINTLN2 CONSOLE._PRINTLN2

#ifdef DEBUG_SKETCH
#define DEBUG_PRINTF CONSOLE_PRINTF
#define DEBUG_PRINT CONSOLE_PRINT
#define DEBUG_PRINTLN CONSOLE_PRINTLN
#define DEBUG_PRINTLN2 CONSOLE_PRINTLN2

#else
#define DEBUG_PRINTF(...) \
  do { \
  } while (false)
#define DEBUG_PRINT(...) \
  do { \
  } while (false)
#define DEBUG_PRINTLN(...) \
  do { \
  } while (false)
#define DEBUG_PRINTLN2(...) \
  do { \
  } while (false)
#endif



/* Set these to your desired softAP credentials. They are not configurable at runtime */
#ifndef APSSID
#define APSSID "MagicPortal"
#define APPSK "ShowTime"
#endif

const char *softAP_ssid = APSSID;
const char *softAP_password = APPSK;

/* hostname for mDNS. Should work at least on windows. Try http://esp8266.local */
const char *myHostname = "magicportal";

/* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
char ssid[33] = "";
char password[65] = "";
uint8_t bssid[6];
WiFiEventHandler staModeConnectedHandler;
WiFiEventHandler staModeDisconnectedHandler;

// DNS server
DNSServer dnsServer;

// Web server
ESP8266WebServer server(80);

/* Soft AP network parameters */
IPAddress apIP(172, 217, 28, 1);
IPAddress netMsk(255, 255, 255, 0);


/** Should I connect to WLAN asap? */
bool connect = false;

/** Set to true to start WiFi STA at setup time when credentials loaded successfuly from EEPROM */
/** Set to false to defer WiFi STA until configured through web interface. */
bool staReady = false;  // Don't connect right away

/** Last time I tried to connect to WLAN */
unsigned long lastConnectTry = 0;

/** Current WLAN status */
unsigned int status = WL_IDLE_STATUS;

void setup() {
  WiFi.persistent(false);  // w/o this a flash write occurs at every boot
  WiFi.mode(WIFI_OFF);     // Prevent use of SDK stored credentials
  CONSOLE.begin(115200);
  CONSOLE_PRINTLN("\r\n\r\nNAPT with Configuration Portal ...");

  staModeConnectedHandler = WiFi.onStationModeConnected(
    [](const WiFiEventStationModeConnected &data) {
      // Keep a copy of the BSSID for the AP that WLAN connects to.
      // This is used in the WLAN report on WiFi Details page.
      memcpy(bssid, data.bssid, sizeof(bssid));
    });

  staModeDisconnectedHandler = WiFi.onStationModeDisconnected(
    [](const WiFiEventStationModeDisconnected &) {
      if (dnsServer.isForwarding()) {
        dnsServer.disableForwarder("*");
        dnsServer.setTTL(0);
        // Reminder, Serial.println() will not work from these callbacks.
        // For debug printf use ets_uart_printf().
      }
    });

  /*
    While you can remove the password parameter to make the AP open.
    You will be operating with less security and allowing snoopers to see
    the credentials you use for your WiFi.
  */
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(softAP_ssid, softAP_password);
  // The following comment for delay(500) was committed Aug 19, 2015; is it
  // still true? Commented out for verification. - APR 2020
  // delay(500); // Without delay I've seen the IP address blank
  CONSOLE_PRINTF("SoftAP '%s' started\r\n", softAP_ssid);
  CONSOLE_PRINTLN2("  IP address: ", WiFi.softAPIP().toString());

  /* Captive portals will usually use a TTL of 0 to avoid DNS cache poisoning. */
  dnsServer.setTTL(0);

  /* Setup the DNS server redirecting all the domains to the apIP */
  dnsServer.start(IANA_DNS_PORT, "*", apIP);
  CONSOLE_PRINTLN("DNSServer started:");
  CONSOLE_PRINTF("  DNS Forwarding is %s\r\n", dnsServer.isForwarding() ? "on" : "off");
  CONSOLE_PRINTF("  Resolve all domain lookups, '%s', to this AP's IP address, '%s' %s.\r\n",
                 dnsServer.getDomainName().c_str(),
                 softAP_ssid,
                 WiFi.softAPIP().toString().c_str());
  CONSOLE_PRINTF("  TTL set to %u\r\n", dnsServer.getTTL());

  /*
    Do some NAPT startup stuff
  */
  CONSOLE_PRINTLN("Begin NAPT initialization:");
  CONSOLE_PRINTF("  Heap before NAPT init: %d\r\n", ESP.getFreeHeap());

  err_t ret = ip_napt_init(NAPT, NAPT_PORT);
  CONSOLE_PRINTF("  ip_napt_init(%d,%d): ret=%d (OK=%d)\r\n", NAPT, NAPT_PORT, (int)ret, (int)ERR_OK);
  if (ret == ERR_OK) {
    ret = ip_napt_enable_no(SOFTAP_IF, 1);
    CONSOLE_PRINTF("  ip_napt_enable_no(SOFTAP_IF): ret=%d (OK=%d)\r\n", (int)ret, (int)ERR_OK);
    if (ret == ERR_OK) {
      CONSOLE_PRINTF("  NAPT AP '%s' started.\r\n", softAP_ssid);
      if (WiFi.localIP().isSet()) {
        CONSOLE_PRINTF("    It is an extension of '%s' made through WLAN interface.\r\n", ssid);
        CONSOLE_PRINTF("    Remote WLAN IP Address: %s.\r\n", WiFi.localIP().toString().c_str());
      }
    }
  }
  CONSOLE_PRINTF("  Heap after NAPT init: %d\r\n", ESP.getFreeHeap());
  if (ret != ERR_OK) {
    CONSOLE_PRINTF("  NAPT initialization failed!!!\r\n");
  }

  /* Setup web pages: root, wifi config pages, SO captive portal detectors and not found. */
  server.on("/", handleRoot);
  server.on("/wifi", handleWifi);
  server.on("/wifisave", handleWifiSave);
  server.on("/generate_204", handleRoot);  // Android captive portal. Maybe not needed. Might be handled by notFound handler.
  server.on("/fwlink", handleRoot);        // Microsoft captive portal. Maybe not needed. Might be handled by notFound handler.
  server.onNotFound(handleNotFound);
  server.begin();  // Web server start
  CONSOLE_PRINTLN("HTTP server started");
  loadCredentials();                         // Load WLAN credentials from network
  connect = (strlen(ssid) > 0 && staReady);  // Request WLAN connect if there is a SSID and we want to connect at startup
}

void connectWifi() {
  CONSOLE_PRINTF("Connecting as wifi client, WLAN, to '%s' ...\r\n", ssid);
  WiFi.disconnect();
  /*
    A call to set hostname, must be set before the call to WiFi.begin, otherwise
    the name may be missing from the routers DNS lookup results. Note, not all
    routers will import registered DHCP host names from clients into the active
    local DNS resolver. For those that do, it is best to set hostname before
    calling WiFi.begin().
  */
  WiFi.hostname(myHostname);
  WiFi.begin(ssid, password);
  int connRes = WiFi.waitForConnectResult();
  if (-1 == connRes) {
    CONSOLE_PRINTLN("  WiFi.waitForConnectResult() timed out.");
  } else {
    CONSOLE_PRINTF("  Connection status: %s, %d\r\n", getWiFiStatusString(connRes).c_str(), connRes);
  }
}

void loop() {
  if (connect) {
    connect = false;
    connectWifi();
    lastConnectTry = millis();
  }
  {
    unsigned int s = WiFi.status();
    if (s == 0 && millis() > (lastConnectTry + 60000) && ssid[0] && staReady) {
      /* When all of the following conditions are true, try to connect */
      /*  1) If WLAN disconnected */
      /*  2) Required idle time between connect attempts has passed. */
      /*  3) We have an ssid configured */
      /*  4) We are ready for the STA to come up */
      /* Don't set retry time too low as retry interfere the softAP operation */
      connect = true;
    }
    if (status != s) {  // WLAN status change
      CONSOLE_PRINTF("WLAN Status changed:\r\n");
      CONSOLE_PRINTF("  new status:      %s, %d\r\n", getWiFiStatusString(s).c_str(), s);
      CONSOLE_PRINTF("  previous status: %s, %d\r\n", getWiFiStatusString(status).c_str(), status);
      status = s;
      if (s == WL_CONNECTED) {
        /* Just connected to WLAN */
        CONSOLE.println();
        if (WiFi.localIP().isSet() && WiFi.softAPIP().isSet()) {
          CONSOLE_PRINTF("NAPT AP '%s' status:\r\n", softAP_ssid);
          if (WiFi.localIP().isSet()) {
            CONSOLE_PRINTF("  It is an extension of '%s' made through WLAN interface.\r\n", ssid);
            CONSOLE_PRINTF("  WLAN connected with IP Address: %s.\r\n", WiFi.localIP().toString().c_str());
          }
        } else {
          CONSOLE_PRINT("WLAN connected to ");
          CONSOLE.println(ssid);
          CONSOLE_PRINT("  IP address: ");
          CONSOLE.println(WiFi.localIP());
        }
        // Setup MDNS responder
        if (!MDNS.begin(myHostname, WiFi.localIP())) {
          CONSOLE_PRINTLN("  Error setting up MDNS responder!");
        } else {
          CONSOLE_PRINTLN("  mDNS responder started");
          // Add service to MDNS-SD
          MDNS.addService("http", "tcp", 80);
        }
        /*
          Setup the DNSServer to respond only to request for our hostname and
          forward other name request to the DNS configured to the WLAN.
        */
        dnsServer.setTTL(600);  // 10 minutes
        dnsServer.enableForwarder(myHostname, WiFi.dnsIP(0));
        CONSOLE_PRINTF("DNSServer changes/status:\r\n");
        CONSOLE_PRINTF("  DNS Forwarding is %s\r\n", dnsServer.isForwarding() ? "on" : "off");
        CONSOLE_PRINTF("  Resolve '%s' to this AP's IP address, '%s' %s.\r\n",
                       dnsServer.getDomainName().c_str(),
                       softAP_ssid,
                       WiFi.softAPIP().toString().c_str());
        if (dnsServer.isDNSSet()) {
          CONSOLE_PRINTF("  Forward other lookups to DNS: %s\r\n", dnsServer.getDNS().toString().c_str());
        }
        CONSOLE_PRINTF("  TTL set to %u\r\n", dnsServer.getTTL());

      } else {
        /* Captive portals will usually use a TTL of 0 to avoid DNS cache poisoning. */
        dnsServer.setTTL(0);
        /* Setup the DNSServer to redirect all the domain lookups to the apIP */
        dnsServer.disableForwarder("*");
        CONSOLE_PRINTF("DNSServer changes/status:\r\n");
        CONSOLE_PRINTF("  DNS Forwarding is %s\r\n", dnsServer.isForwarding() ? "on" : "off");
        CONSOLE_PRINTF("  Resolve all domain lookups, '%s', to this AP's IP address, '%s' %s.\r\n",
                       dnsServer.getDomainName().c_str(),
                       softAP_ssid,
                       WiFi.softAPIP().toString().c_str());
        CONSOLE_PRINTF("  TTL set to %u\r\n", dnsServer.getTTL());

        // Note, it is not necessary to clear the DNS forwarder address. This
        // is being done here, to test that methods isDNSSet() and setDNS() work.
        dnsServer.setDNS(0U);
        if (dnsServer.isDNSSet()) {
          CONSOLE_PRINTF("  DNS forwarder address: %s\r\n", dnsServer.getDNS().toString().c_str());
        } else {
          CONSOLE_PRINTF("  DNS forwarder address not set.\r\n");
        }

        if (s == WL_NO_SSID_AVAIL) {
          WiFi.disconnect();
        }
      }
    }
    if (s == WL_CONNECTED) {
      MDNS.update();
    }
  }
  // Do work:
  // DNS
  dnsServer.processNextRequest();
  // HTTP
  server.handleClient();
}

#else  // LWIP_FEATURES && !LWIP_IPV6

#include <ESP8266WiFi.h>
void setup() {
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  Serial.begin(115200);
  Serial.printf("\n\nNAPT not supported in this configuration\n");
}

void loop() {
}

#endif  // LWIP_FEATURES && !LWIP_IPV6
