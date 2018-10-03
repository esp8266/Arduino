/*
    HTTP over TLS (HTTPS) example sketch

    This example demonstrates how to use
    WiFiClientSecure class to access HTTPS API.
    We fetch and display the status of
    esp8266/Arduino project continuous integration
    build.

    Limitations:
      only RSA certificates
      no support of Perfect Forward Secrecy (PFS)
      TLSv1.2 is supported since version 2.4.0-rc1

    Created by Ivan Grokhotkov, 2015.
    This example is in public domain.
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#define DEPRECATED_SSL_WITH_AXTLS 0  // use 1 for axTLS, 0 for BearSSL

const char* ssid = "........";
const char* password = "........";

const char* host = "api.github.com";
const int httpsPort = 443;

// Use web browser to view and copy
// SHA1 fingerprint of the certificate

#if DEPRECATED_SSL_WITH_AXTLS

// no separator, or space, or ":", in iram:
const char* fingerprint = "5F F1 60 31 09 04 3E F2 90 D2 B0 8A 50 38 04 E8 37 9F BC 76";

#else // with BearSSL

// compatible with axTLS:
//const char* fingerprint = "35 85 74 EF 67 35 A7 CE 40 69 50 F3 C0 F6 80 CF 80 3B 2E 19";
// flash is allowed:
//#define fingerprint FPSTR("358574EF67:35:A7:CE:40:69:50:F3-C0-F6-80-CF-80-3B 2E 19")
// or an array in iram:
const uint8_t fingerprint [] = { 0x35, 0x85, 0x74, 0xEF, 0x67, 0x35, 0xA7, 0xCE, 0x40, 0x69, 0x50, 0xF3, 0xC0, 0xF6, 0x80, 0xCF, 0x80, 0x3B, 0x2E, 0x19 };

#endif

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Use WiFiClientSecure class to create TLS connection
  Serial.print("connecting to ");
  Serial.println(host);

  ////////////////////////// axTLS (soon deprecated)
  #if DEPRECATED_SSL_WITH_AXTLS

  axTLS::WiFiClientSecure client;
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  if (client.verify(fingerprint, host)) {
    Serial.println("certificate matches");
  } else {
    Serial.println("certificate doesn't match");
  }

  ////////////////////////// BearSSL
  #else // new SSL api with BearSSL

  BearSSL::WiFiClientSecure client;

  // WARNING:
  // - axTLS was insecure by default,
  //   and fingerprint could be checked after a successful connection
  // - BearSSL is not:
  //   "If there are no CAs or insecure options specified, BearSSL will not connect."
  //   Check the BearSSL_Validation.ino example for more details
  //   Insecure connection is available with `client.setInsecure()`
  //   Semi-secure checking using fingerprint:
  client.setFingerprint(fingerprint);

  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  #endif
  //////////////////////////

  String url = "/repos/esp8266/Arduino/commits/master/status";
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.println("reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("closing connection");
}

void loop() {
}
