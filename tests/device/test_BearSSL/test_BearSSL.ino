// Stress test the BearSSL connection options to determine
// maximum memory use for different SSL connections and
// SPIFFS certstore usage.  Before running you need to run
// certs-from-mozilla.py and upload the generated SPIFFS file.
//
// For more info on CertStores, see the BearSSL_CertStore example
//
// November 2018 by Earle F. Philhower, III
// Released to the public domain

#include <Arduino.h>
#include <BSTest.h>
#include <ESP8266WiFi.h>
#include <CertStoreBearSSL.h>
#include <FS.h>
#include <time.h>
#include <StackThunk.h>

extern "C" {
#include "user_interface.h"
}

BS_ENV_DECLARE();

void setClock();

// A single, global CertStore which can be used by all
// connections.  Needs to stay live the entire time any of
// the WiFiClientBearSSLs are present.
BearSSL::CertStore certStore;

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    BS_RUN(Serial);
}

bool pretest()
{
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.begin(getenv("STA_SSID"), getenv("STA_PASS"));
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    setClock();
    SPIFFS.begin();
    int numCerts = certStore.initCertStore(SPIFFS, "/certs.idx", "/certs.ar");
    Serial.printf("Number of CA certs read: %d\n", numCerts);
    if (numCerts == 0) {
        Serial.printf("No certs found. Did you run certs-from-mozill.py and upload the SPIFFS directory before running?\n");
        return false;
    }
    return true;
}

static void stopAll()
{
    WiFiClient::stopAll();
}

static void disconnectWiFI()
{
    wifi_station_disconnect();
}


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
void fetchURL(BearSSL::WiFiClientSecure *client, const char *host, const uint16_t port, const char *path) {
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
      char *nl = strchr(tmp, '\r');
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


int run(const char *str)
{
    BearSSL::WiFiClientSecure *bear = new BearSSL::WiFiClientSecure();
    // Integrate the cert store with this connection
    bear->setCertStore(&certStore);

    char buff[100];
    uint32_t maxUsage = 0;
    stack_thunk_repaint();
    sprintf(buff, "%s.badssl.com", str);
    Serial.printf("%s: ", buff);
    fetchURL(bear, buff, 443, "/");
    Serial.printf("Stack: %d\n", stack_thunk_get_max_usage());
    maxUsage = std::max(maxUsage, stack_thunk_get_max_usage());
    delete bear;

    printf("\n\n\nMAX THUNK STACK USAGE: %d\n", maxUsage);
    return maxUsage;
}

#define TC(x) TEST_CASE("BearSSL - Maximum stack usage < 5600 bytes @ "x".badssl.org", "[bearssl]") { REQUIRE(run(x) < 5600); }

TC("expired")
TC("wrong.host")
TC("self-signed")
TC("untrusted-root")
TC("revoked")
TC("pinning-test")
TC("no-common-name")
TC("no-subject")
TC("incomplete-chain")
TC("sha1-intermediate")
TC("sha256")
TC("sha384")
TC("sha512")
TC("1000-sans")
// TC("10000-sans") // Runs for >10 seconds, so causes false failure.  Covered by the 1000 SAN anyway
TC("ecc256")
TC("ecc384")
TC("rsa2048")
TC("rsa4096")
TC("extended-validation")
TC("dh480")
TC("dh512")
TC("dh1024")
TC("dh2048")
TC("dh-small-subgroup")
TC("dh-composite")
TC("static-rsa")
TC("tls-v1-0")
TC("tls-v1-1")
TC("tls-v1-2")
TC("invalid-expected-sct")

void loop() {
}

