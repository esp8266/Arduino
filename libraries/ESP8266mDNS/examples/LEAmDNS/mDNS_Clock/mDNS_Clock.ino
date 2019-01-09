/*
  ESP8266 mDNS responder clock

  This example demonstrates two features of the LEA MDNSResponder:
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
#include <time.h>

/*
   Include the MDNSResponder (the library needs to be included also)
   As LEA MDNSResponder is experimantal in the ESP8266 environment currently, the
   legacy MDNSResponder is defaulted in th include file.
   There are two ways to access LEA MDNSResponder:
   1. Prepend every declaration and call to global declarations or functions with the namespace, like:
      'LEAmDNS::MDNSResponder::hMDNSService  hMDNSService;'
      This way is used in the example. But be careful, if the namespace declaration is missing
      somewhere, the call might go to the legacy implementation...
   2. Open 'ESP8266mDNS.h' and set LEAmDNS to default.

*/
#include <ESP8266mDNS.h>
#include <PolledTimeout.h>
/*
   Global defines and vars
*/

#define TIMEZONE_OFFSET     1                                   // CET
#define DST_OFFSET          1                                   // CEST
#define UPDATE_CYCLE        (1 * 1000)                          // every second

#define SERVICE_PORT        80                                  // HTTP port

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

const char*                   ssid                    = STASSID;
const char*                   password                = STAPSK;

char*                         pcHostDomain            = 0;        // Negociated host domain
bool                          bHostDomainConfirmed    = false;    // Flags the confirmation of the host domain
MDNSResponder::hMDNSService   hMDNSService            = 0;        // The handle of the clock service in the MDNS responder

// TCP server at port 'SERVICE_PORT' will respond to HTTP requests
WiFiServer                    server(SERVICE_PORT);


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
   This can be triggered by calling MDNS.announce().

*/
bool MDNSDynamicServiceTxtCallback(MDNSResponder* p_pMDNSResponder,
                                   const MDNSResponder::hMDNSService p_hService,
                                   void* p_pUserdata) {
  Serial.println("MDNSDynamicServiceTxtCallback");
  (void) p_pUserdata;

  if ((p_pMDNSResponder) &&
      (hMDNSService == p_hService)) {
    Serial.printf("Updating curtime TXT item to: %s\n", getTimeString());
    p_pMDNSResponder->addDynamicServiceTxt(p_hService, "curtime", getTimeString());
  }
  return true;
}


/*
   MDNSProbeResultCallback

   Probe result callback for the host domain.
   If the domain is free, the host domain is set and the clock service is
   added.
   If the domain is already used, a new name is created and the probing is
   restarted via p_pMDNSResponder->setHostname().

*/
bool MDNSProbeResultCallback(MDNSResponder* p_pMDNSResponder,
                             const char* p_pcDomainName,
                             const MDNSResponder::hMDNSService p_hService,
                             bool p_bProbeResult,
                             void* p_pUserdata) {
  Serial.println("MDNSProbeResultCallback");
  (void) p_pUserdata;

  if ((p_pMDNSResponder) &&
      (0 == p_hService)) {  // Called for host domain
    Serial.printf("MDNSProbeResultCallback: Host domain '%s.local' is %s\n", p_pcDomainName, (p_bProbeResult ? "free" : "already USED!"));
    if (true == p_bProbeResult) {
      // Set station hostname
      setStationHostname(pcHostDomain);

      if (!bHostDomainConfirmed) {
        // Hostname free -> setup clock service
        bHostDomainConfirmed = true;

        if (!hMDNSService) {
          // Add a 'clock.tcp' service to port 'SERVICE_PORT', using the host domain as instance domain
          hMDNSService = p_pMDNSResponder->addService(0, "espclk", "tcp", SERVICE_PORT);
          if (hMDNSService) {
            // Add a simple static MDNS service TXT item
            p_pMDNSResponder->addServiceTxt(hMDNSService, "port#", SERVICE_PORT);
            // Set the callback function for dynamic service TXTs
            p_pMDNSResponder->setDynamicServiceTxtCallback(hMDNSService, MDNSDynamicServiceTxtCallback, 0);
          }
        }
      }
    } else {
      // Change hostname, use '-' as divider between base name and index
      if (MDNSResponder::indexDomain(pcHostDomain, "-", 0)) {
        p_pMDNSResponder->setHostname(pcHostDomain);
      } else {
        Serial.println("MDNSProbeResultCallback: FAILED to update hostname!");
      }
    }
  }
  return true;
}


/*
   handleHTTPClient
*/
void handleHTTPClient(WiFiClient& client) {
  Serial.println("");
  Serial.println("New client");

  // Wait for data from client to become available
  while (client.connected() && !client.available()) {
    delay(1);
  }

  // Read the first line of HTTP request
  String req = client.readStringUntil('\r');

  // First line of HTTP request looks like "GET /path HTTP/1.1"
  // Retrieve the "/path" part by finding the spaces
  int addr_start = req.indexOf(' ');
  int addr_end = req.indexOf(' ', addr_start + 1);
  if (addr_start == -1 || addr_end == -1) {
    Serial.print("Invalid request: ");
    Serial.println(req);
    return;
  }
  req = req.substring(addr_start + 1, addr_end);
  Serial.print("Request: ");
  Serial.println(req);
  client.flush();

  // Get current time
  time_t now = time(nullptr);;
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);

  String s;
  if (req == "/") {
    IPAddress ip = WiFi.localIP();
    String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
    s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>Hello from ";
    s += WiFi.hostname() + " at " + ipStr;
    // Simple addition of the current time
    s += "\r\nCurrent time is: ";
    s += getTimeString();
    // done :-)
    s += "</html>\r\n\r\n";
    Serial.println("Sending 200");
  } else {
    s = "HTTP/1.1 404 Not Found\r\n\r\n";
    Serial.println("Sending 404");
  }
  client.print(s);

  Serial.println("Done with client");
}


/*
   setup
*/
void setup(void) {
  Serial.begin(115200);

  // Connect to WiFi network
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
  MDNS.setProbeResultCallback(MDNSProbeResultCallback, 0);
  // Init the (currently empty) host domain string with 'esp8266'
  if ((!MDNSResponder::indexDomain(pcHostDomain, 0, "esp8266")) ||
      (!MDNS.begin(pcHostDomain))) {
    Serial.println("Error setting up MDNS responder!");
    while (1) { // STOP
      delay(1000);
    }
  }
  Serial.println("MDNS responder started");

  // Start TCP (HTTP) server
  server.begin();
  Serial.println("TCP server started");
}


/*
   loop
*/
void loop(void) {
  // Check if a client has connected
  WiFiClient    client = server.available();
  if (client) {
    handleHTTPClient(client);
  }

  // Allow MDNS processing
  MDNS.update();

  // Update time (if needed)
  //static    unsigned long ulNextTimeUpdate = UPDATE_CYCLE;
  static clsMDNSTimeFlag timeFlag(UPDATE_CYCLE);
  if (timeFlag.flagged()/*ulNextTimeUpdate < millis()*/) {

    if (hMDNSService) {
      // Just trigger a new MDNS announcement, this will lead to a call to
      // 'MDNSDynamicServiceTxtCallback', which will update the time TXT item
      MDNS.announce();
    }
    //ulNextTimeUpdate = (millis() + UPDATE_CYCLE);   // Set update 'timer'
    timeFlag.restart();
  }
}


