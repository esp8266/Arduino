// Demonstrate the CertStore object with WiFiClientBearSSL
//
// Before running, you must download the set of certs using
// the script "certs-from-mozilla.py" (no parameters)
// and then uploading the generated .AR file to LittleFS or SD.
//
// You do not need to generate the ".IDX" file listed below,
// it is generated automatically when the CertStore object
// is created and written to SD or LittleFS by the ESP8266.
//
// Why would you need a CertStore?
//
// If you know the exact server being connected to, or you
// are generating your own self-signed certificates and aren't
// allowing connections to HTTPS/TLS servers out of your
// control, then you do NOT want a CertStore.  Hardcode the
// self-signing CA or the site's x.509 certificate directly.
//
// However, if you don't know what specific sites the system
// will be required to connect to and verify, a
// CertStore can allow you to select from among
// 10s or 100s of CAs against which you can check the
// target's X.509, without taking any more RAM than a single
// certificate.  This is the same way that standard browsers
// and operating systems verify SSL connections.
//
// About the chosen certs:
// The certificates are scraped from the Mozilla.org current
// list, but please don't take this as an endorsement or a
// requirement:  it is up to YOU, the USER, to specify the
// certificate authorities you will use as trust bases.
//
// Mar 2018 by Earle F. Philhower, III
// Released to the public domain

#include <ESP8266WiFi.h>
#include <CertStoreBearSSL.h>
#include <time.h>
#include <FS.h>
#include <LittleFS.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif

const char* ssid = STASSID;
const char* pass = STAPSK;

// A single, global CertStore which can be used by all
// connections.  Needs to stay live the entire time any of
// the WiFiClientBearSSLs are present.
BearSSL::CertStore certStore;

// Set time via NTP, as required for x.509 validation
void setClock() {
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

// Try and connect using a WiFiClientBearSSL to specified host:port and dump URL
void fetchURL(BearSSL::WiFiClientSecure* client, const char* host, const uint16_t port, const char* path) {
  if (!path) {
    path = "/";
  }

  Serial.printf("Trying: %s:443...", host);
  client->connect(host, port);
  if (!client->connected()) {
    Serial.printf("*** Can't connect. ***\n-------\n");
    return;
  }
  Serial.printf("Connected!\n-------\n");
  client->write("GET ");
  client->write(path);
  client->write(" HTTP/1.0\r\nHost: ");
  client->write(host);
  client->write("\r\nUser-Agent: ESP8266\r\n");
  client->write("\r\n");
  uint32_t to = millis() + 5000;
  if (client->connected()) {
    do {
      char tmp[32];
      memset(tmp, 0, 32);
      int rlen = client->read((uint8_t*)tmp, sizeof(tmp) - 1);
      yield();
      if (rlen < 0) {
        break;
      }
      // Only print out first line up to \r, then abort connection
      char* nl = strchr(tmp, '\r');
      if (nl) {
        *nl = 0;
        Serial.print(tmp);
        break;
      }
      Serial.print(tmp);
    } while (millis() < to);
  }
  client->stop();
  Serial.printf("\n-------\n");
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  LittleFS.begin();

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  setClock();  // Required for X.509 validation

  int numCerts = certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
  Serial.printf("Number of CA certs read: %d\n", numCerts);
  if (numCerts == 0) {
    Serial.printf("No certs found. Did you run certs-from-mozilla.py and upload the LittleFS directory before running?\n");
    return;  // Can't connect to anything w/o certs!
  }

  BearSSL::WiFiClientSecure* bear = new BearSSL::WiFiClientSecure();
  // Integrate the cert store with this connection
  bear->setCertStore(&certStore);
  Serial.printf("Attempting to fetch https://github.com/...\n");
  fetchURL(bear, "github.com", 443, "/");
  delete bear;
}

void loop() {
  Serial.printf("\nPlease enter a website address (www.blah.com) to connect to: ");
  String site;
  do {
    site = Serial.readString();
  } while (site == "");
  // Strip newline if present
  site.replace(String("\r"), emptyString);
  site.replace(String("\n"), emptyString);
  Serial.printf("https://%s/\n", site.c_str());

  BearSSL::WiFiClientSecure* bear = new BearSSL::WiFiClientSecure();
  // Integrate the cert store with this connection
  bear->setCertStore(&certStore);
  fetchURL(bear, site.c_str(), 443, "/");
  delete bear;
}
