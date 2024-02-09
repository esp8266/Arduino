/*
  Demonstrate the usage of WiFiServerBearSSL.
  By Earle F. Philhower, III

  A simple HTTPS server is implemented with a self-signed
  certificate for the ESP8266.

  This is NOT the best way to implement a HTTPS website on the
  ESP8266.  Please see the ESP8266WebServerBearSSL example for
  a much better way of doing this!

  IMPORTANT NOTES ABOUT SSL CERTIFICATES

  1. USE/GENERATE YOUR OWN CERTIFICATES
    While a sample, self-signed certificate is included in this example,
    it is ABSOLUTELY VITAL that you use your own SSL certificate in any
    real-world deployment.  Anyone with the certificate and key may be
    able to decrypt your traffic, so your own keys should be kept in a
    safe manner, not accessible on any public network.

  2. HOW TO GENERATE YOUR OWN CERTIFICATE/KEY PAIR
    It is easy to use OpenSSL to generate a self-signed certificate
       openssl req -x509 -nodes -newkey rsa:2048 -keyout key.pem -out cert.pem -days 4096

    You may also, of course, use a commercial, trusted SSL provider to
    generate your certificate.

  Included with this example are *SAMPLE* certs and keys.  They are NOT
  SECURE, since they're shared with all copies of the repo, so
  DO NOT USE THE SAMPLE CERTS, KEYS, OR CAS IN YOUR OWN PROJECT!!!

  Run this example and then try connecting to the server https://IP.

  This example is released into the public domain.
*/

#include <ESP8266WiFi.h>
#include <time.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif

const char *ssid = STASSID;
const char *pass = STAPSK;

// The HTTPS server
BearSSL::WiFiServerSecure server(443);

// #define USE_EC // Enable Elliptic Curve signed cert

#define USING_INSECURE_CERTS_AND_KEYS_AND_CAS 1
#include <ssl-tls-ca-key-cert-example.h>

#define CACHE_SIZE 5  // Number of sessions to cache.
#define USE_CACHE     // Enable SSL session caching.
                      // Caching SSL sessions shortens the length of the SSL handshake.
                      // You can see the performance improvement by looking at the
                      // Network tab of the developer tools of your browser.
// #define DYNAMIC_CACHE // Whether to dynamically allocate the cache.

#if defined(USE_CACHE) && defined(DYNAMIC_CACHE)
// Dynamically allocated cache.
BearSSL::ServerSessions serverCache(CACHE_SIZE);
#elif defined(USE_CACHE)
// Statically allocated cache.
ServerSession store[CACHE_SIZE];
BearSSL::ServerSessions serverCache(store, CACHE_SIZE);
#endif

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

  // Attach the server private cert/key combo
  BearSSL::X509List *serverCertList = new BearSSL::X509List(server_cert);
  BearSSL::PrivateKey *serverPrivKey = new BearSSL::PrivateKey(server_private_key);
#ifndef USE_EC
  server.setRSACert(serverCertList, serverPrivKey);
#else
  server.setECCert(serverCertList, BR_KEYTYPE_KEYX | BR_KEYTYPE_SIGN, serverPrivKey);
#endif

  // Set the server's cache
#if defined(USE_CACHE)
  server.setCache(&serverCache);
#endif

  // Actually start accepting connections
  server.begin();
}

static const char *HTTP_RES = "HTTP/1.0 200 OK\r\n"
                              "Connection: close\r\n"
                              "Content-Length: 62\r\n"
                              "Content-Type: text/html; charset=iso-8859-1\r\n"
                              "\r\n"
                              "<html>\r\n"
                              "<body>\r\n"
                              "<p>Hello from ESP8266!</p>\r\n"
                              "</body>\r\n"
                              "</html>\r\n";

void loop() {
  static int cnt;
  BearSSL::WiFiClientSecure incoming = server.accept();
  if (!incoming) { return; }
  Serial.printf("Incoming connection...%d\n", cnt++);

  // Ugly way to wait for \r\n (i.e. end of HTTP request which we don't actually parse here)
  uint32_t timeout = millis() + 1000;
  int lcwn = 0;
  for (;;) {
    unsigned char x = 0;
    if ((millis() > timeout) || (incoming.available() && incoming.read(&x, 1) < 0)) {
      incoming.stop();
      Serial.printf("Connection error, closed\n");
      return;
    } else if (!x) {
      yield();
      continue;
    } else if (x == 0x0D) {
      continue;
    } else if (x == 0x0A) {
      if (lcwn) { break; }
      lcwn = 1;
    } else
      lcwn = 0;
  }
  incoming.write((uint8_t *)HTTP_RES, strlen(HTTP_RES));
  incoming.flush();
  incoming.stop();
  Serial.printf("Connection closed.\n");
}
