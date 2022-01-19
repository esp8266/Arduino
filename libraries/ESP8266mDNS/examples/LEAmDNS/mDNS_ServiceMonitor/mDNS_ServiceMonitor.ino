/*
  ESP8266 mDNS Responder Service Monitor

  This example demonstrates two features of the LEA MDNSResponder:
  1. The host and service domain negotiation process that ensures
     the uniqueness of the finally chosen host and service domain name.
  2. The dynamic MDNS service lookup/query feature.

  A list of 'HTTP' services in the local network is created and kept up to date.
  In addition to this, a (very simple) HTTP server is set up on port 80
  and announced as a service.

  The ESP itself is initially announced to clients as 'esp8266.local', if this host domain
  is already used in the local network, another host domain is negotiated. Keep an
  eye to the serial output to learn the final host domain for the HTTP service.
  The service itself is is announced as 'host domain'._http._tcp.local.
  The HTTP server delivers a short greeting and the current  list of other 'HTTP' services (not updated).
  The web server code is taken nearly 1:1 from the 'mDNS_Web_Server.ino' example.
  Point your browser to 'host domain'.local to see this web page.

  Instructions:
  - Update WiFi SSID and password as necessary.
  - Flash the sketch to the ESP8266 board
  - Install host software:
    - For Linux, install Avahi (http://avahi.org/).
    - For Windows, install Bonjour (http://www.apple.com/support/bonjour/).
    - For Mac OSX and iOS support is built in through Bonjour already.
  - Use a browser like 'Safari' to see the page at http://'host domain'.local.

*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

/*
   Global defines and vars
*/

#define SERVICE_PORT 80 // HTTP port

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

char* pcHostDomain = 0; // Negotiated host domain
bool bHostDomainConfirmed = false; // Flags the confirmation of the host domain
MDNSResponder::hMDNSService hMDNSService = 0; // The handle of the http service in the MDNS responder
MDNSResponder::hMDNSServiceQuery hMDNSServiceQuery = 0; // The handle of the 'http.tcp' service query in the MDNS responder

const String cstrNoHTTPServices = "Currently no 'http.tcp' services in the local network!<br/>";
String strHTTPServices = cstrNoHTTPServices;

// HTTP server at port 'SERVICE_PORT' will respond to HTTP requests
ESP8266WebServer server(SERVICE_PORT);

/*
   setStationHostname
*/
bool setStationHostname(const char* p_pcHostname) {

  if (p_pcHostname) {
    WiFi.hostname(p_pcHostname);
    Serial.printf("setStationHostname: Station hostname is set to '%s'\n", p_pcHostname);
    return true;
  }
  return false;
}
/*
   MDNSServiceQueryCallback
*/

void MDNSServiceQueryCallback(MDNSResponder::MDNSServiceInfo serviceInfo, MDNSResponder::AnswerType answerType, bool p_bSetContent) {
  String answerInfo;
  switch (answerType) {
    case MDNSResponder::AnswerType::ServiceDomain:
      answerInfo = "ServiceDomain " + String(serviceInfo.serviceDomain());
      break;
    case MDNSResponder::AnswerType::HostDomainAndPort:
      answerInfo = "HostDomainAndPort " + String(serviceInfo.hostDomain()) + ":" + String(serviceInfo.hostPort());
      break;
    case MDNSResponder::AnswerType::IP4Address:
      answerInfo = "IP4Address ";
      for (IPAddress ip : serviceInfo.IP4Adresses()) {
        answerInfo += "- " + ip.toString();
      };
      break;
    case MDNSResponder::AnswerType::Txt:
      answerInfo = "TXT " + String(serviceInfo.strKeyValue());
      for (auto kv : serviceInfo.keyValues()) {
        answerInfo += "\nkv : " + String(kv.first) + " : " + String(kv.second);
      }
      break;
    default:
      answerInfo = "Unknown Answertype";
  }
  Serial.printf("Answer %s %s\n", answerInfo.c_str(), p_bSetContent ? "Modified" : "Deleted");
}

/*
   MDNSServiceProbeResultCallback
   Probe result callback for Services
*/

void serviceProbeResult(String p_pcServiceName,
    const MDNSResponder::hMDNSService p_hMDNSService,
    bool p_bProbeResult) {
  (void)p_hMDNSService;
  Serial.printf("MDNSServiceProbeResultCallback: Service %s probe %s\n", p_pcServiceName.c_str(), (p_bProbeResult ? "succeeded." : "failed!"));
}

/*
   MDNSHostProbeResultCallback

   Probe result callback for the host domain.
   If the domain is free, the host domain is set and the http service is
   added.
   If the domain is already used, a new name is created and the probing is
   restarted via p_pMDNSResponder->setHostname().

*/

void hostProbeResult(String p_pcDomainName, bool p_bProbeResult) {

  Serial.printf("MDNSHostProbeResultCallback: Host domain '%s.local' is %s\n", p_pcDomainName.c_str(), (p_bProbeResult ? "free" : "already USED!"));

  if (true == p_bProbeResult) {
    // Set station hostname
    setStationHostname(pcHostDomain);

    if (!bHostDomainConfirmed) {
      // Hostname free -> setup clock service
      bHostDomainConfirmed = true;

      if (!hMDNSService) {
        // Add a 'http.tcp' service to port 'SERVICE_PORT', using the host domain as instance domain
        hMDNSService = MDNS.addService(0, "http", "tcp", SERVICE_PORT);
        if (hMDNSService) {
          MDNS.setServiceProbeResultCallback(hMDNSService, serviceProbeResult);

          // Add some '_http._tcp' protocol specific MDNS service TXT items
          // See: http://www.dns-sd.org/txtrecords.html#http
          MDNS.addServiceTxt(hMDNSService, "user", "");
          MDNS.addServiceTxt(hMDNSService, "password", "");
          MDNS.addServiceTxt(hMDNSService, "path", "/");
        }

        // Install dynamic 'http.tcp' service query
        if (!hMDNSServiceQuery) {
          hMDNSServiceQuery = MDNS.installServiceQuery("http", "tcp", MDNSServiceQueryCallback);
          if (hMDNSServiceQuery) {
            Serial.printf("MDNSProbeResultCallback: Service query for 'http.tcp' services installed.\n");
          } else {
            Serial.printf("MDNSProbeResultCallback: FAILED to install service query for 'http.tcp' services!\n");
          }
        }
      }
    }
  } else {
    // Change hostname, use '-' as divider between base name and index
    if (MDNSResponder::indexDomain(pcHostDomain, "-", 0)) {
      MDNS.setHostname(pcHostDomain);
    } else {
      Serial.println("MDNSProbeResultCallback: FAILED to update hostname!");
    }
  }
}

/*
   HTTP request function (not found is handled by server)
*/
void handleHTTPRequest() {
  Serial.println("");
  Serial.println("HTTP Request");

  IPAddress ip = WiFi.localIP();
  String ipStr = ip.toString();
  String s = "<!DOCTYPE HTML>\r\n<html><h3><head>Hello from ";
  s += WiFi.hostname() + ".local at " + WiFi.localIP().toString() + "</h3></head>";
  s += "<br/><h4>Local HTTP services are :</h4>";
  s += "<ol>";
  for (auto info : MDNS.answerInfo(hMDNSServiceQuery)) {
    s += "<li>";
    s += info.serviceDomain();
    if (info.hostDomainAvailable()) {
      s += "<br/>Hostname: ";
      s += String(info.hostDomain());
      s += (info.hostPortAvailable()) ? (":" + String(info.hostPort())) : "";
    }
    if (info.IP4AddressAvailable()) {
      s += "<br/>IP4:";
      for (auto ip : info.IP4Adresses()) {
        s += " " + ip.toString();
      }
    }
    if (info.txtAvailable()) {
      s += "<br/>TXT:<br/>";
      for (auto kv : info.keyValues()) {
        s += "\t" + String(kv.first) + " : " + String(kv.second) + "<br/>";
      }
    }
    s += "</li>";
  }
  s += "</ol><br/>";

  Serial.println("Sending 200");
  server.send(200, "text/html", s);
  Serial.println("Done with request");
}

/*
   setup
*/
void setup(void) {
  Serial.begin(115200);
  Serial.setDebugOutput(false);

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

  // Setup HTTP server
  server.on("/", handleHTTPRequest);

  // Setup MDNS responders
  MDNS.setHostProbeResultCallback(hostProbeResult);

  // Init the (currently empty) host domain string with 'esp8266'
  if ((!MDNSResponder::indexDomain(pcHostDomain, 0, "esp8266")) || (!MDNS.begin(pcHostDomain))) {
    Serial.println(" Error setting up MDNS responder!");
    while (1) { // STOP
      delay(1000);
    }
  }
  Serial.println("MDNS responder started");

  // Start HTTP server
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  // Check if a request has come in
  server.handleClient();
  // Allow MDNS processing
  MDNS.update();
}
