// Example of the different modes of the X.509 validation options
// in the WiFiClientBearSSL object
//
// Mar 2018 by Earle F. Philhower, III
// Released to the public domain

#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <StackThunk.h>
#include <sntp.h>
#include <time.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

const char *ssid = STASSID;
const char *pass = STAPSK;

const char *   host = "api.github.com";
const uint16_t port = 443;
const char *   path = "/";

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
bool fetchURL(BearSSL::WiFiClientSecure *client, const char *host, const uint16_t port, const char *path) {
  if (!path) {
    path = "/";
  }

  ESP.resetFreeContStack();
  uint32_t freeStackStart = ESP.getFreeContStack();
  Serial.printf("Trying: %s:443...\n", host);
  client->connect(host, port);
  if (!client->connected()) {
    Serial.printf("*** Can't connect. ***\n-------\n");
    return false;
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
  uint32_t freeStackEnd = ESP.getFreeContStack();
  Serial.printf("\nCONT stack used: %d\n", freeStackStart - freeStackEnd);
  Serial.printf("BSSL stack used: %d\n-------\n\n", stack_thunk_get_max_usage());
  return true;
}

bool fetchNoConfig() {
  Serial.printf(R"EOF(
If there are no CAs or insecure options specified, BearSSL will not connect.
Expect the following call to fail as none have been configured.
)EOF");
  BearSSL::WiFiClientSecure client;
  return !fetchURL(&client, host, port, path);
}

bool fetchInsecure() {
  Serial.printf(R"EOF(
This is absolutely *insecure*, but you can tell BearSSL not to check the
certificate of the server.  In this mode it will accept ANY certificate,
which is subject to man-in-the-middle (MITM) attacks.
)EOF");
  BearSSL::WiFiClientSecure client;
  client.setInsecure();
  return fetchURL(&client, host, port, path);
}

bool fetchFingerprint() {
  Serial.printf(R"EOF(
The SHA-1 fingerprint of an X.509 certificate can be used to validate it
instead of the while certificate.  This is not nearly as secure as real
X.509 validation, but is better than nothing.  Also be aware that these
fingerprints will change if anything changes in the certificate chain
(i.e. re-generating the certificate for a new end date, any updates to
the root authorities, etc.).
)EOF");
  bool ret = true; // Expect to be all ok
  BearSSL::WiFiClientSecure client;
  // Old fingerprint, no longer valid.
  static const char wrong_fp[] PROGMEM = "59:74:61:88:13:CA:12:34:15:4D:11:0A:C1:7F:E6:67:07:69:42:F5";
  Serial.printf("\nFirst, try and connect with a wrong fingerprint (will fail):\n");
  client.setFingerprint(wrong_fp);
  if (fetchURL(&client, host, port, path) != false)
    ret = false;
  static const char right_fp[] PROGMEM = "df:b2:29:c6:a6:38:1a:59:9d:c9:ad:92:2d:26:f5:3c:83:8f:a5:87";
  Serial.printf("\nNow we'll try and connect with a valid fingerprint (will pass):\n");
  client.setFingerprint(right_fp);
  if (fetchURL(&client, host, port, path) != true)
    ret = false;
  return ret;
}

bool fetchSelfSigned() {
  Serial.printf(R"EOF(
It is also possible to accept *any* self-signed certificate.  This is
absolutely insecure as anyone can make a self-signed certificate.
)EOF");
  bool ret = true; // Expect to be all ok
  BearSSL::WiFiClientSecure client;
  Serial.printf("\nFirst, try and connect to a badssl.com self-signed website (will fail):\n");
  if (fetchURL(&client, "self-signed.badssl.com", 443, "/") != false)
    ret = false;
  Serial.printf("\nNow we'll enable self-signed certs (will pass):\n");
  client.allowSelfSignedCerts();
  if (fetchURL(&client, "self-signed.badssl.com", 443, "/") != true)
    ret = false;
  return ret;
}

bool fetchKnownKey() {
  Serial.printf(R"EOF(
The server certificate can be completely ignored and its public key
hardcoded in your application. This should be secure as the public key
needs to be paired with the private key of the site, which is obviously
private and not shared.  A MITM without the private key would not be
able to establish communications.
)EOF");
  // Extracted by: openssl x509 -pubkey -noout -in servercert.pem
  static const char wrong_pubkey[] PROGMEM = R"KEY(
-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAoObTh6xvTjspdWBMSh77
3a+BCjfEia21jp0dDFXapLHLq9MUvNJtuNF8Nh9FoQYlMmN/lEz01pcGPyTyhfWD
jCeKf2rIRugEb0xfTEiapoDBCNucboGLVFnwxm1YKj1C6tpdqmuQe68SNDAbIlyv
ze7yPAiQmZG+QRbG4JVZqdZSOd7powLiaOP5tVbOrmInXv+jlB+Jgg9d6oJNr94P
O6oESm+khUOAETXxO9ZmgGiXbgrpeVdjRJHB4kXb3Sy5LT0Wdq8LpASAwBA1JvKf
OEOoHaUZefCwYJhA+ExUU18yDt6GZedPXCF6xzV/3qPgjrLTAkJkQBQhIJYUCVQf
UwIDAQAB
-----END PUBLIC KEY-----
)KEY";
  // Extracted by: openssl x509 -pubkey -noout -in servercert.pem
  static const char right_pubkey[] PROGMEM = R"KEY(
-----BEGIN PUBLIC KEY-----
MIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAoObTh6xvTjspdWBMSh76
3a+BCjfEia21jp0dDFXapLHLq9MUvNJtuNF8Nh9FoQYlMmN/lEz01pcGPyTyhfWD
jCeKf2rIRugEb0xfTEiapoDBCNucboGLVFnwxm1YKj1C6tpdqmuQe68SNDAbIlyv
ze7yPAiQmZG+QRbG4JVZqdZSOd7powLiaOP5tVbOrmInXv+jlB+Jgg9d6oJNr94P
O6oESm+khUOAETXxO9ZmgGiXbgrpeVdjRJHB4kXb3Sy5LT0Wdq8LpASAwBA1JvKf
OEOoHaUZefCwYJhA+ExUU18yDt6GZedPXCF6xzV/3qPgjrLTAkJkQBQhIJYUCVQf
UwIDAQAB
-----END PUBLIC KEY-----
)KEY";

  bool ret = true; // Expect to be all ok
  BearSSL::WiFiClientSecure client;
  BearSSL::PublicKey wrong_key(wrong_pubkey);
  BearSSL::PublicKey right_key(right_pubkey);
  client.setKnownKey(&wrong_key);
  Serial.printf("\nFirst, try and connect with a wrong PubKey (will fail):\n");
  if (fetchURL(&client, host, port, path) != false)
    ret = false;
  client.setKnownKey(&right_key);
  Serial.printf("\nNow we'll try and connect with a correct PubKey (will pass):\n");
  if (fetchURL(&client, host, port, path) != true)
    ret = false;
  return ret;
}

bool fetchCertAuthority() {
  static const char digicert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDxTCCAq2gAwIBAgIQAqxcJmoLQJuPC3nyrkYldzANBgkqhkiG9w0BAQUFADBs
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSswKQYDVQQDEyJEaWdpQ2VydCBIaWdoIEFzc3VyYW5j
ZSBFViBSb290IENBMB4XDTA2MTExMDAwMDAwMFoXDTMxMTExMDAwMDAwMFowbDEL
MAkGA1UEBhMCVVMxFTATBgNVBAoTDERpZ2lDZXJ0IEluYzEZMBcGA1UECxMQd3d3
LmRpZ2ljZXJ0LmNvbTErMCkGA1UEAxMiRGlnaUNlcnQgSGlnaCBBc3N1cmFuY2Ug
RVYgUm9vdCBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAMbM5XPm
+9S75S0tMqbf5YE/yc0lSbZxKsPVlDRnogocsF9ppkCxxLeyj9CYpKlBWTrT3JTW
PNt0OKRKzE0lgvdKpVMSOO7zSW1xkX5jtqumX8OkhPhPYlG++MXs2ziS4wblCJEM
xChBVfvLWokVfnHoNb9Ncgk9vjo4UFt3MRuNs8ckRZqnrG0AFFoEt7oT61EKmEFB
Ik5lYYeBQVCmeVyJ3hlKV9Uu5l0cUyx+mM0aBhakaHPQNAQTXKFx01p8VdteZOE3
hzBWBOURtCmAEvF5OYiiAhF8J2a3iLd48soKqDirCmTCv2ZdlYTBoSUeh10aUAsg
EsxBu24LUTi4S8sCAwEAAaNjMGEwDgYDVR0PAQH/BAQDAgGGMA8GA1UdEwEB/wQF
MAMBAf8wHQYDVR0OBBYEFLE+w2kD+L9HAdSYJhoIAu9jZCvDMB8GA1UdIwQYMBaA
FLE+w2kD+L9HAdSYJhoIAu9jZCvDMA0GCSqGSIb3DQEBBQUAA4IBAQAcGgaX3Nec
nzyIZgYIVyHbIUf4KmeqvxgydkAQV8GK83rZEWWONfqe/EW1ntlMMUu4kehDLI6z
eM7b41N5cdblIZQB2lWHmiRk9opmzN6cN82oNLFpmyPInngiK3BD41VHMWEZ71jF
hS9OMPagMRYjyOfiZRYzy78aG6A9+MpeizGLYAiJLQwGXFK3xPkKmNEVX58Svnw2
Yzi9RKR/5CYrCsSXaQ3pjOLAEFe4yHYSkVXySGnYvCoCWw9E1CAx2/S6cCZdkGCe
vEsXCS+0yx5DaMkHJ8HSXPfqIbloEpw8nL+e/IBcm2PN7EeqJSdnoDfzAIJ9VNep
+OkuE6N36B9K
-----END CERTIFICATE-----
)EOF";

  Serial.printf(R"EOF(
A specific certification authority can be passed in and used to validate
a chain of certificates from a given server.  These will be validated
using BearSSL's rules, which do NOT include certificate revocation lists.
A specific server's certificate, or your own self-signed root certificate
can also be used.  ESP8266 time needs to be valid for checks to pass as
BearSSL does verify the notValidBefore/After fields.
)EOF");

  bool ret = true; // Expect to be all ok
  BearSSL::WiFiClientSecure client;
  BearSSL::X509List cert(digicert);
  client.setTrustAnchors(&cert);
#ifndef CORE_MOCK
  Serial.printf("\nTry validating without setting the time (should fail):\n");
  if (fetchURL(&client, host, port, path) != true) // Error here, time already set
    ret = false;
#endif
  Serial.printf("\nTry again after setting NTP time (should pass):\n");
  setClock();
  if (fetchURL(&client, host, port, path) != true)
    ret = false;
  return ret;
}

void fetchFaster() {
  Serial.printf(R"EOF(
The ciphers used to set up the SSL connection can be configured to
only support faster but less secure ciphers.  If you care about security
you won't want to do this.  If you need to maximize battery life, these
may make sense
)EOF");
  BearSSL::WiFiClientSecure client;
  client.setInsecure();
  uint32_t now = millis();
  fetchURL(&client, host, port, path);
  uint32_t delta = millis() - now;
  client.setInsecure();
  client.setCiphersLessSecure();
  now = millis();
  fetchURL(&client, host, port, path);
  uint32_t delta2 = millis() - now;
  std::vector<uint16_t> myCustomList = { BR_TLS_RSA_WITH_AES_256_CBC_SHA256, BR_TLS_ECDHE_ECDSA_WITH_AES_128_CBC_SHA, BR_TLS_RSA_WITH_3DES_EDE_CBC_SHA };
  client.setInsecure();
  client.setCiphers(myCustomList);
  now = millis();
  fetchURL(&client, host, port, path);
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

  bool ret = true;
  ret &= fetchNoConfig();
  ret &= fetchInsecure();
  ret &= fetchFingerprint();
  ret &= fetchSelfSigned();
  ret &= fetchKnownKey();
  ret &= fetchCertAuthority();
  fetchFaster();
#ifdef CORE_MOCK
  if (!ret)
    exit(1);
#endif
}


void loop() {
  // Nothing to do here
}