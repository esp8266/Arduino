// Example of the different modes of the X.509 validation options
// in the WiFiClientBearSSL object
//
// Mar 2018 by Earle F. Philhower, III
// Released to the public domain

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <StackThunk.h>
#include <time.h>
#include "certs.h"

#define FINGERPRINT fingerprint_www_example_org
#define PUBKEY pubkey_www_example_org
#define CERT cert_DigiCert_TLS_RSA_SHA256_2020_CA1

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif

const char *ssid = STASSID;
const char *pass = STAPSK;

const char *path = "/";

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

// Try and connect using a WiFiClientBearSSL to specified host:port and dump HTTP response
void fetchURL(BearSSL::WiFiClientSecure *client, const char *host, const uint16_t port, const char *path) {
  if (!path) { path = "/"; }

  ESP.resetFreeContStack();
  uint32_t freeStackStart = ESP.getFreeContStack();
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
  uint32_t freeStackEnd = ESP.getFreeContStack();
  Serial.printf("\nCONT stack used: %d\n", freeStackStart - freeStackEnd);
  Serial.printf("BSSL stack used: %d\n-------\n\n", stack_thunk_get_max_usage());
}

void fetchNoConfig() {
  Serial.printf(R"EOF(
If there are no CAs or insecure options specified, BearSSL will not connect.
Expect the following call to fail as none have been configured.
)EOF");
  BearSSL::WiFiClientSecure client;
  fetchURL(&client, SSL_host, SSL_port, path);
}

void fetchInsecure() {
  Serial.printf(R"EOF(
This is absolutely *insecure*, but you can tell BearSSL not to check the
certificate of the server.  In this mode it will accept ANY certificate,
which is subject to man-in-the-middle (MITM) attacks.
)EOF");
  BearSSL::WiFiClientSecure client;
  client.setInsecure();
  fetchURL(&client, SSL_host, SSL_port, path);
}

void fetchFingerprint() {
  Serial.printf(R"EOF(
The SHA-1 fingerprint of an X.509 certificate can be used to validate it
instead of the while certificate.  This is not nearly as secure as real
X.509 validation, but is better than nothing.  Also be aware that these
fingerprints will change if anything changes in the certificate chain
(i.e. re-generating the certificate for a new end date, any updates to
the root authorities, etc.).
)EOF");
  BearSSL::WiFiClientSecure client;
  client.setFingerprint(FINGERPRINT);
  fetchURL(&client, SSL_host, SSL_port, path);
}

void fetchSelfSigned() {
  Serial.printf(R"EOF(
It is also possible to accept *any* self-signed certificate.  This is
absolutely insecure as anyone can make a self-signed certificate.
)EOF");
  BearSSL::WiFiClientSecure client;
  Serial.printf("First, try and connect to a badssl.com self-signed website (will fail):\n");
  fetchURL(&client, "self-signed.badssl.com", 443, "/");
  Serial.printf("Now we'll enable self-signed certs (will pass)\n");
  client.allowSelfSignedCerts();
  fetchURL(&client, "self-signed.badssl.com", 443, "/");
}

void fetchKnownKey() {
  Serial.printf(R"EOF(
The server certificate can be completely ignored and its public key
hardcoded in your application. This should be secure as the public key
needs to be paired with the private key of the site, which is obviously
private and not shared.  A MITM without the private key would not be
able to establish communications.
)EOF");
  BearSSL::WiFiClientSecure client;
  BearSSL::PublicKey key(PUBKEY);
  client.setKnownKey(&key);
  fetchURL(&client, SSL_host, SSL_port, path);
}

void fetchCertAuthority() {
  Serial.printf(R"EOF(
A specific certification authority can be passed in and used to validate
a chain of certificates from a given server.  These will be validated
using BearSSL's rules, which do NOT include certificate revocation lists.
A specific server's certificate, or your own self-signed root certificate
can also be used.  ESP8266 time needs to be valid for checks to pass as
BearSSL does verify the notValidBefore/After fields.
)EOF");

  BearSSL::WiFiClientSecure client;
  BearSSL::X509List cert(CERT);
  client.setTrustAnchors(&cert);
  Serial.printf("Try validating without setting the time (should fail)\n");
  fetchURL(&client, SSL_host, SSL_port, path);

  Serial.printf("Try again after setting NTP time (should pass)\n");
  setClock();
  fetchURL(&client, SSL_host, SSL_port, path);
}

void fetchFaster() {
  Serial.printf(R"EOF(
The ciphers used to set up the SSL connection can be configured to
only support faster but less secure ciphers.  If you care about security
you won't want to do this.  If you need to maximize battery life, these
may make sense
)EOF");
  BearSSL::WiFiClientSecure client;
  Serial.printf("Insecure, all ciphers:\n");
  client.setInsecure();
  uint32_t now = millis();
  fetchURL(&client, SSL_host, SSL_port, path);
  uint32_t delta = millis() - now;
  Serial.printf("Insecure, less secure ciphers:\n");
  client.setInsecure();
  client.setCiphersLessSecure();
  now = millis();
  fetchURL(&client, SSL_host, SSL_port, path);
  uint32_t delta2 = millis() - now;
  Serial.printf("Insecure, few ciphers:\n");
  std::vector<uint16_t> myCustomList = { BR_TLS_RSA_WITH_AES_256_CBC_SHA256, BR_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA, BR_TLS_RSA_WITH_3DES_EDE_CBC_SHA };
  client.setInsecure();
  client.setCiphers(myCustomList);
  now = millis();
  fetchURL(&client, SSL_host, SSL_port, path);
  uint32_t delta3 = millis() - now;
  Serial.printf("Using more secure: %dms\nUsing less secure ciphers: %dms\nUsing custom cipher list: %dms\n", delta, delta2, delta3);
}

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println();

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

  fetchNoConfig();
  fetchInsecure();
  fetchFingerprint();
  fetchSelfSigned();
  fetchKnownKey();
  fetchCertAuthority();
  fetchFaster();
}


void loop() {
  // Nothing to do here
}
