/*
    HTTP over TLS (HTTPS) example sketch

    This example demonstrates how to use
    WiFiClientSecure class to access HTTPS API.
    We fetch and display the status of
    esp8266/Arduino project continuous integration
    build.

    Created by Ivan Grokhotkov, 2015.
    This example is in public domain.
*/

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

const char* host = "api.github.com";
const int httpsPort = 443;

// DigiCert High Assurance EV Root CA
const char trustRoot[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIE6zCCBHGgAwIBAgIQAtX25VXj+RoJlA3D2bWkgzAKBggqhkjOPQQDAzBWMQsw
CQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMTAwLgYDVQQDEydEaWdp
Q2VydCBUTFMgSHlicmlkIEVDQyBTSEEzODQgMjAyMCBDQTEwHhcNMjEwMzA0MDAw
MDAwWhcNMjIwMzA5MjM1OTU5WjBoMQswCQYDVQQGEwJVUzETMBEGA1UECBMKQ2Fs
aWZvcm5pYTEWMBQGA1UEBxMNU2FuIEZyYW5jaXNjbzEVMBMGA1UEChMMR2l0SHVi
LCBJbmMuMRUwEwYDVQQDDAwqLmdpdGh1Yi5jb20wWTATBgcqhkjOPQIBBggqhkjO
PQMBBwNCAAQf8SePhtD7JeGm0YuTQ4HihyeENuvsNFdYPPIxIx6Lj9iOu2ECkgy4
52UR+mhIF24OvPizDveyCFOqmG/MI7kwo4IDDTCCAwkwHwYDVR0jBBgwFoAUCrwI
KReMpTlteg7OM8cus+37w3owHQYDVR0OBBYEFP5TUYtiCp+N3FISu3CqxMlJhdG1
MCMGA1UdEQQcMBqCDCouZ2l0aHViLmNvbYIKZ2l0aHViLmNvbTAOBgNVHQ8BAf8E
BAMCB4AwHQYDVR0lBBYwFAYIKwYBBQUHAwEGCCsGAQUFBwMCMIGXBgNVHR8EgY8w
gYwwRKBCoECGPmh0dHA6Ly9jcmwzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydFRMU0h5
YnJpZEVDQ1NIQTM4NDIwMjBDQTEuY3JsMESgQqBAhj5odHRwOi8vY3JsNC5kaWdp
Y2VydC5jb20vRGlnaUNlcnRUTFNIeWJyaWRFQ0NTSEEzODQyMDIwQ0ExLmNybDA+
BgNVHSAENzA1MDMGBmeBDAECAjApMCcGCCsGAQUFBwIBFhtodHRwOi8vd3d3LmRp
Z2ljZXJ0LmNvbS9DUFMwgYMGCCsGAQUFBwEBBHcwdTAkBggrBgEFBQcwAYYYaHR0
cDovL29jc3AuZGlnaWNlcnQuY29tME0GCCsGAQUFBzAChkFodHRwOi8vY2FjZXJ0
cy5kaWdpY2VydC5jb20vRGlnaUNlcnRUTFNIeWJyaWRFQ0NTSEEzODQyMDIwQ0Ex
LmNydDAMBgNVHRMBAf8EAjAAMIIBAwYKKwYBBAHWeQIEAgSB9ASB8QDvAHUAKXm+
8J45OSHwVnOfY6V35b5XfZxgCvj5TV0mXCVdx4QAAAF3/bWc4AAABAMARjBEAiBm
IdofaKj+XfeISM/2tjap1nQY1afFSBAcdw/YtgjmSQIgMqWoDyfO66suyk2VFcld
1C+WHUNGvXsCRPof5HG5QQgAdgAiRUUHWVUkVpY/oS/x922G4CMmY63AS39dxoNc
buIPAgAAAXf9tZ0CAAAEAwBHMEUCIQCJzwZRfAvv0izotFx2KE0sgV8O+NfuHUpa
1866RqKEtwIgc65P+xToSqPbp/J1gSFBJgySI/a1YoB+3p8xXTYaDsAwCgYIKoZI
zj0EAwMDaAAwZQIxAL8fIlMNWdeKHalpm9z+ksCuYT4tSN1ubXeNvDywr56me+yT
+fr42MnEcBdUtLOVOAIwPNC9fAJjyHHTL2vaRW1JRnrovLKDQVbZpZNIZnlY3WFu
kmxiBWDOpyfJrG9vQ25K
-----END CERTIFICATE-----
)EOF";
X509List cert(trustRoot);

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.print("Connecting to ");
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

  // Set time via NTP, as required for x.509 validation
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

  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("Connecting to ");
  Serial.println(host);

  Serial.printf("Using certificate: %s\n", trustRoot);
  client.setTrustAnchors(&cert);

  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed");
    return;
  }

  String url = "/repos/esp8266/Arduino/commits/master/status";
  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("Request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("Headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  if (line.startsWith("{\"state\":\"success\"")) {
    Serial.println("esp8266/Arduino CI successfull!");
  } else {
    Serial.println("esp8266/Arduino CI has failed");
  }
  Serial.println("Reply was:");
  Serial.println("==========");
  Serial.println(line);
  Serial.println("==========");
  Serial.println("Closing connection");
}

void loop() {
}
