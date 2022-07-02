// Example of using SSL sessions to speed up SSL connection initiation
//
// Note that sessions are a function of individual HTTPS servers, so if you
// are connecting to a service through a load abalncer (i.e. Azure, AWS, GitHub)
// two connections to the same IP address will generally connect to two
// different web servers, meaning that sessions won't work.  If you are
// connecting to a single server not behind a load balancer/etc., however,
// there should be a significant speedup.
//
// September 2018 by Earle F. Philhower, III
// Released to the public domain

#include <ESP8266WiFi.h>
#include <time.h>
#include "certs.h"

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif

const char *ssid = STASSID;
const char *pass = STAPSK;

const char *path = "/";

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

  Serial.printf("Connecting to %s\n", ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, pass);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());

  // Set up time to allow for certificate validation
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

// Try and connect using a WiFiClientBearSSL to specified host:port and dump HTTP response
void fetchURL(BearSSL::WiFiClientSecure *client, const char *host, const uint16_t port, const char *path) {
  if (!path) { path = "/"; }

  Serial.printf("Trying: %s:443...", host);
  if (!client->connect(host, port)) {
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
  while (client->available()) {
    do {
      char tmp[32];
      memset(tmp, 0, 32);
      int rlen = client->read((uint8_t *)tmp, sizeof(tmp) - 1);
      yield();
      if (rlen < 0) { break; }
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
  Serial.printf("\n-------\n\n");
}


void loop() {
  uint32_t start, finish;
  BearSSL::WiFiClientSecure client;
  BearSSL::X509List cert(certForum);
  const char *host = "esp8266.com";
  const int port = 443;

  Serial.printf("Connecting without sessions...");
  start = millis();
  client.setTrustAnchors(&cert);
  fetchURL(&client, host, port, path);
  finish = millis();
  Serial.printf("Total time: %dms\n", finish - start);

  BearSSL::Session session;
  client.setSession(&session);
  Serial.printf("Connecting with an uninitialized session...");
  start = millis();
  client.setTrustAnchors(&cert);
  fetchURL(&client, host, port, path);
  finish = millis();
  Serial.printf("Total time: %dms\n", finish - start);

  Serial.printf("Connecting with the just initialized session...");
  start = millis();
  client.setTrustAnchors(&cert);
  fetchURL(&client, host, port, path);
  finish = millis();
  Serial.printf("Total time: %dms\n", finish - start);

  Serial.printf("Connecting again with the initialized session...");
  start = millis();
  client.setTrustAnchors(&cert);
  fetchURL(&client, host, port, path);
  finish = millis();
  Serial.printf("Total time: %dms\n", finish - start);

  delay(10000);  // Avoid DDOSing github
}
