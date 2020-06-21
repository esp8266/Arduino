/*
  ESP8266 mDNS responder clock

  This example demonstrates two features of the LEA clsLEAMDNSHost:
  1. The host and service domain negotiation process that ensures
     the uniqueness of the finally choosen host and service domain name.
  2. The dynamic MDNS service TXT feature

  A 'clock' service in announced via the MDNS responder and the current
  time is set as a TXT item (eg. 'curtime=Mon Oct 15 19:54:35 2018').
  The time value is updated every second!

  The ESP is initially announced to clients as 'esp8266.local', if this host domain
  is already used in the local network, another host domain is negociated. Keep an
  eye to the serial output to learn the final host domain for the clock service.
  The service itself is is announced as 'host domain'._espclk._tcp.local.
  As the service uses port 80, a very simple HTTP server is installed also to deliver
  a small web page containing a greeting and the current time (not updated).
  The web server code is taken nearly 1:1 from the 'mDNS_Web_Server.ino' example.
  Point your browser to 'host domain'.local to see this web page.

  Instructions:
  - Update WiFi SSID and password as necessary.
  - Flash the sketch to the ESP8266 board
  - Install host software:
    - For Linux, install Avahi (http://avahi.org/).
    - For Windows, install Bonjour (http://www.apple.com/support/bonjour/).
    - For Mac OSX and iOS support is built in through Bonjour already.
  - Use a MDNS/Bonjour browser like 'Discovery' to find the clock service in your local
    network and see the current time updates.

*/


#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <LwipIntf.h>
#include <time.h>

// uses API MDNSApiVersion::LEAv2

/*
   Include the clsLEAMDNSHost (the library needs to be included also)
   As LEA clsLEAMDNSHost is experimantal in the ESP8266 environment currently, the
   legacy clsLEAMDNSHost is defaulted in th include file.
   There are two ways to access LEA clsLEAMDNSHost:
   1. Prepend every declaration and call to global declarations or functions with the namespace, like:
      'LEAmDNS::clsLEAMDNSHost::hMDNSService  hMDNSService;'
      This way is used in the example. But be careful, if the namespace declaration is missing
      somewhere, the call might go to the legacy implementation...
   2. Open 'ESP8266mDNS.h' and set LEAmDNS to default.

*/
#define MDNS2_EXPERIMENTAL
#include <ESP8266mDNS.h>

#include <PolledTimeout.h>
/*
   Global defines and vars
*/


#define TIMEZONE_OFFSET     1                                   // CET
#define DST_OFFSET          1                                   // CEST
#define UPDATE_CYCLE        (1 * 1000)                          // every second

#define START_AP_AFTER_MS   10000 //60000                               // start AP after delay
#define SERVICE_PORT        80                                  // HTTP port

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

#ifndef APSSID
#define APSSID "ap4mdnsClock"
#define APPSK  "mdnsClock"
#endif

const char*                   ssid                    = STASSID;
const char*                   password                = STAPSK;

clsLEAMDNSHost                responder;                          // MDNS responder
bool                          bHostDomainConfirmed    = false;    // Flags the confirmation of the host domain
clsLEAMDNSHost::clsService*   hMDNSService            = 0;        // The handle of the clock service in the MDNS responder

// HTTP server at port 'SERVICE_PORT' will respond to HTTP requests
ESP8266WebServer              server(SERVICE_PORT);

/*
   getTimeString
*/
const char* getTimeString(void) {

  static char   acTimeString[32];
  time_t now = time(nullptr);
  ctime_r(&now, acTimeString);
  size_t    stLength;
  while (((stLength = strlen(acTimeString))) &&
         ('\n' == acTimeString[stLength - 1])) {
    acTimeString[stLength - 1] = 0; // Remove trailing line break...
  }
  return acTimeString;
}


/*
   setClock

   Set time via NTP
*/
void setClock(void) {
  configTime((TIMEZONE_OFFSET * 3600), (DST_OFFSET * 3600), "pool.ntp.org", "time.nist.gov", "time.windows.com");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);   // Secs since 01.01.1970 (when uninitalized starts with (8 * 3600 = 28800)
  while (now < 8 * 3600 * 2) {  // Wait for realistic value
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  Serial.printf("Current time: %s\n", getTimeString());
}


/*
   setStationHostname
*/
bool setStationHostname(const char* p_pcHostname) {

  if (p_pcHostname) {
    WiFi.hostname(p_pcHostname);
    Serial.printf("setDeviceHostname: Station hostname is set to '%s'\n", p_pcHostname);
  }
  return true;
}


/*
   MDNSDynamicServiceTxtCallback

   Add a dynamic MDNS TXT item 'ct' to the clock service.
   The callback function is called every time, the TXT items for the clock service
   are needed.
   This can be triggered by calling responder.announce().

*/
void MDNSDynamicServiceTxtCallback(const clsLEAMDNSHost::hMDNSService& p_hService) {
  Serial.println("MDNSDynamicServiceTxtCallback");

  if (hMDNSService == &p_hService) {
    Serial.printf("Updating curtime TXT item to: %s\n", getTimeString());
    hMDNSService->addDynamicServiceTxt("curtime", getTimeString());
  }
}


/*
   handleHTTPClient
*/

void handleHTTPRequest() {
  Serial.println("");
  Serial.println("HTTP Request");

  // Get current time
  time_t now = time(nullptr);;
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);

  String s;

  s = "<!DOCTYPE HTML>\r\n<html>Hello from ";
  s += WiFi.hostname() + " at " + WiFi.localIP().toString();
  // Simple addition of the current time
  s += "\r\nCurrent time is: ";
  s += getTimeString();
  // done :-)
  s += "</html>\r\n\r\n";
  Serial.println("Sending 200");
  server.send(200, "text/html", s);
}

/*
   setup
*/
void setup(void) {
  Serial.begin(115200);

  // Connect to WiFi network

  WiFi.persistent(false);

  // useless informative callback
  if (!LwipIntf::stateUpCB([](netif * nif) {
  Serial.printf("New interface %c%c(%d) is up(%d)\n",
                nif->name[0],
                nif->name[1],
                netif_get_index(nif),
                netif_is_up(nif));
  })) {
    Serial.println("Error: could not add useless informative callback\n");
  }

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Sync clock
  setClock();

  // Setup MDNS responder
  // Init the (currently empty) host domain string with 'esp8266'
  if (responder.begin("leamdnsv2", [](clsLEAMDNSHost & p_rMDNSHost,
                                      const char* p_pcDomainName,
  bool p_bProbeResult)->void {
  Serial.printf("mDNSHost_AP::ProbeResultCallback: '%s' is %s\n", p_pcDomainName, (p_bProbeResult ? "FREE" : "USED!"));
    // Unattended added service
    hMDNSService = p_rMDNSHost.addService(0, "espclk", "tcp", 80);
    hMDNSService->addDynamicServiceTxt("curtime", getTimeString());
    hMDNSService->setDynamicServiceTxtCallback(MDNSDynamicServiceTxtCallback);
  })) {
    Serial.println("mDNS-AP started");
  } else {
    Serial.println("FAILED to start mDNS-AP");
  }

  // Setup HTTP server
  server.on("/", handleHTTPRequest);
  server.begin();
  Serial.println("HTTP server started");
}

/*
   loop
*/
void loop(void) {

  // Check if a request has come in
  server.handleClient();
  // Allow MDNS processing
  responder.update();

  static esp8266::polledTimeout::periodicMs timeout(UPDATE_CYCLE);
  if (timeout.expired()) {

    if (hMDNSService) {
      // Just trigger a new MDNS announcement, this will lead to a call to
      // 'MDNSDynamicServiceTxtCallback', which will update the time TXT item
      responder.announce();
    }
  }

  static bool AP_started = false;
  if (!AP_started && millis() > START_AP_AFTER_MS) {
    AP_started = true;
    Serial.printf("Starting AP...\n");
    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP(APSSID, APPSK);
    Serial.printf("AP started...(%s:%s, %s)\n",
                  WiFi.softAPSSID().c_str(),
                  WiFi.softAPPSK().c_str(),
                  WiFi.softAPIP().toString().c_str());
  }
}
