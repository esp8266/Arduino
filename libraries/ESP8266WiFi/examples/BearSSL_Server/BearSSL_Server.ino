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
#define STAPSK  "your-password"
#endif

const char *ssid = STASSID;
const char *pass = STAPSK;

// The HTTPS server
BearSSL::WiFiServerSecure server(443);

//#define USE_EC // Enable Elliptic Curve signed cert

#ifndef USE_EC

// The server's private key which must be kept secret
const char server_private_key[] PROGMEM = R"EOF(
-----BEGIN PRIVATE KEY-----
MIIEvAIBADANBgkqhkiG9w0BAQEFAASCBKYwggSiAgEAAoIBAQDJblrg47vF3qlE
NMRM7uG8QwE6v/AKpxOL+CLb/32s+dW9Psgf+oZKJgzGkYUoJdWpLitTmTZeykAs
Sq7Iax5Rq/mGqyAc7oJAUUAupfNRU0KwkD1XqtpQWEFoiqoIqZbOZ4CRX5q8z/MN
BH1aPVBMKaL33uwknkgJBzxwZJ2+uGKxRJt8+koj1CXgUCk5lEAEEG5kqE326MjN
O/c4gBqulBV8AIoq6/trY3apTS7FEOiN47qh1PVzoBm/oGVwXvoZAZOj7+gGGo91
sBC5oHJy5Y2BOcNB3opTNXQTiK3Z80b5wc3iQS+h83qAfHwhs6tfAW22WkAf+jtt
x8KdRWFNAgMBAAECggEAPd+jFL9/d1lc/zGCNuuN9YlTgFti/bKyo2UWOCOz1AVu
LVJyoLgQtggYFoqur1Vn2y7uaiB+/gD8U16hb7jPuGCuJjq8g4aUBfOvVmTtZ8a+
joPQA/TcWJ+zf8xQTJbjVwWeDYmje2oZC5+cbbK1zp9fiuoz+U+RawyI+TE+700i
ESCmsKFIHy2Ifruva8HgcPYIPpZ9zLxJj0Dii+WDs7zM9h2dzO4HfImSG/DPmgoV
ydo9IcrUE7KoMLa8Uo7u1b2h6BnTn7GfYiMSUsYcYR3CnpDBknBWjZMwrV0uqv9q
TbVc4QXt+c1q89HDg7BIJaOAzbCvJfgAfXUqZyqwQQKBgQD5ENFjicUzCqPw7fOy
Q5Z8GeUbIJ5urT1MheAq7SPd2kK8TsO3hUjNC0LLNSyKPs6gsYaIiObO3wDGeZZk
xeHBhrUVaz2nIjI7TrnCUpMDOrdxcPr4bc+ifV5YT4W3OFBWQ9chQEx3Nm3DbiX4
fpno34AiFrJF791JkTPFj9OIUQKBgQDPCgcae1pQr77q+GL5Q2tku3RrE4cWtExf
m8DzAb4Vxe3EhPz8bVr+71rqr/KqNfG1uKE3sT0fhB6VMTkHTOQU13jDrvpPUS3W
Vg8cVr5/+iiyF0xb+W8LQ+GVdR5xnMPSZHUtXyURvtzT4nnTAlAtN7lEytX9BzbX
xhltOOwGPQKBgA/Y/BnDSGLpCGlqGpl7J3YaB7PkLXCJYV8fHZZdpGyXWKu2r0lc
F7fEQanAZmcde/RJl2/UlisPkXMPhXxAAw9XTOph+nhJ+rw/VB6DNot8DvQO5kks
Y4vJQlmIJc/0q1fx1RxuhO8I7Y8D0TKwi4Z/wh1pKEq+6mul649kiWchAoGAWn8B
l9uvIHGRO9eSO23ytTcSrfL9Kzln4KqN7iom0hGP2kRe6F9MVP5+ePKrWSb3Hf0z
ysoX83ymeYPob352e32rda04EA9lv7giJrrrzbikrSNt5w3iMcRcCB4HTpW9Kmtq
pIhgBZ+tmpf1s/vg28LtoloeqtjKagpW9tzYnekCgYAZFZ84EGqS9SHw5LELgGY4
mQLMwbYZ6wBMA2PlqYi/17hoAVWz37mLDjtWDB4ir78QMoGbesQVtK9W/4vzmez4
ZLKlffdL5tCtA08Gq9aond1z83Xdnh1UjtwHIJvJPc/AoCFW1r5skv/G6acAk6I2
Zs0aiirNGTEymRX4rw26Qg==
-----END PRIVATE KEY-----
)EOF";

// The server's public certificate which must be shared
const char server_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDUTCCAjmgAwIBAgIJAOcfK7c3JQtnMA0GCSqGSIb3DQEBCwUAMD8xCzAJBgNV
BAYTAkFVMQ0wCwYDVQQIDAROb25lMQ0wCwYDVQQKDAROb25lMRIwEAYDVQQDDAlF
U1BTZXJ2ZXIwHhcNMTgwMzE0MTg1NTQ1WhcNMjkwNTMxMTg1NTQ1WjA/MQswCQYD
VQQGEwJBVTENMAsGA1UECAwETm9uZTENMAsGA1UECgwETm9uZTESMBAGA1UEAwwJ
RVNQU2VydmVyMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAyW5a4OO7
xd6pRDTETO7hvEMBOr/wCqcTi/gi2/99rPnVvT7IH/qGSiYMxpGFKCXVqS4rU5k2
XspALEquyGseUav5hqsgHO6CQFFALqXzUVNCsJA9V6raUFhBaIqqCKmWzmeAkV+a
vM/zDQR9Wj1QTCmi997sJJ5ICQc8cGSdvrhisUSbfPpKI9Ql4FApOZRABBBuZKhN
9ujIzTv3OIAarpQVfACKKuv7a2N2qU0uxRDojeO6odT1c6AZv6BlcF76GQGTo+/o
BhqPdbAQuaBycuWNgTnDQd6KUzV0E4it2fNG+cHN4kEvofN6gHx8IbOrXwFttlpA
H/o7bcfCnUVhTQIDAQABo1AwTjAdBgNVHQ4EFgQUBEk8LqgV+sMjdl/gpP1OlcNW
14EwHwYDVR0jBBgwFoAUBEk8LqgV+sMjdl/gpP1OlcNW14EwDAYDVR0TBAUwAwEB
/zANBgkqhkiG9w0BAQsFAAOCAQEAO1IrqW21KfzrxKmtuDSHdH5YrC3iOhiF/kaK
xXbigdtw6KHW/pIhGiA3BY5u+d5eVuHTR5YSwIbbRvOjuoNBATAw/8f5mt5Wa+C3
PDpLNxDys561VbCW45RMQ0x5kybvDYi0D1R/grqZ18veuFSfE6QMJ/mzvr575fje
8r5Ou0IZOYYF8cyqG5rA4U7BYXEnH44VgwlpkF8pitPsnyUWaAYqE0KnZ0qw0Py4
HCkfGJNlNOOamnr6KakVlocwKY0SdxcLoXSs5ogTQvTSrAOjwcm1RA0hOCXr8f/f
UsQIIGpPVh1plR1vYNndDeBpRJSFkoJTkgAIrlFzSMwNebU0pg==
-----END CERTIFICATE-----
)EOF";

#else
const char server_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIB0zCCAXqgAwIBAgIJALANi2eTiGD/MAoGCCqGSM49BAMCMEUxCzAJBgNVBAYT
AkFVMRMwEQYDVQQIDApTb21lLVN0YXRlMSEwHwYDVQQKDBhJbnRlcm5ldCBXaWRn
aXRzIFB0eSBMdGQwHhcNMTkwNjExMjIyOTU2WhcNMjAwNjEwMjIyOTU2WjBFMQsw
CQYDVQQGEwJBVTETMBEGA1UECAwKU29tZS1TdGF0ZTEhMB8GA1UECgwYSW50ZXJu
ZXQgV2lkZ2l0cyBQdHkgTHRkMFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAExIkZ
w7zjk6TGcScff1PAehuEGmKZTf8VfnkjyJH0IbBgZibZ+qwYGBEnkz4KpKv7TkHo
W+j7F5EMcLcSrUIpy6NTMFEwHQYDVR0OBBYEFI6A0f+g0HyxUT6xrbVmRU79urbj
MB8GA1UdIwQYMBaAFI6A0f+g0HyxUT6xrbVmRU79urbjMA8GA1UdEwEB/wQFMAMB
Af8wCgYIKoZIzj0EAwIDRwAwRAIgWvy7ofQTGZMNqxUfe4gjtkU+C9AkQtaOMW2U
5xFFSvcCICvcGrQpoi7tRTq8xsXFmr8MYWgQTpVAtj6opXMQct/l
-----END CERTIFICATE-----
)EOF";

// The server's private key which must be kept secret
const char server_private_key[] PROGMEM = R"EOF(
-----BEGIN EC PARAMETERS-----
BggqhkjOPQMBBw==
-----END EC PARAMETERS-----
-----BEGIN EC PRIVATE KEY-----
MHcCAQEEIKyLR9/NT7ZdWM+2rklehveuk+jyIHJ+P8ZUQ392HOYvoAoGCCqGSM49
AwEHoUQDQgAExIkZw7zjk6TGcScff1PAehuEGmKZTf8VfnkjyJH0IbBgZibZ+qwY
GBEnkz4KpKv7TkHoW+j7F5EMcLcSrUIpyw==
-----END EC PRIVATE KEY-----
)EOF";

#endif

#define CACHE_SIZE 5 // Number of sessions to cache.
// Caching SSL sessions shortens the length of the SSL handshake.
// You can see the performance improvement by looking at the
// Network tab of the developer tools of your browser.
#define USE_CACHE // Enable SSL session caching.
//#define DYNAMIC_CACHE // Whether to dynamically allocate the cache.

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
  server.setECCert(serverCertList, BR_KEYTYPE_KEYX|BR_KEYTYPE_SIGN, serverPrivKey);
#endif

  // Set the server's cache
#if defined(USE_CACHE)
  server.setCache(&serverCache);
#endif

  // Actually start accepting connections
  server.begin();
}

static const char *HTTP_RES =
        "HTTP/1.0 200 OK\r\n"
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
  if (!incoming) {
    return;
  }
  Serial.printf("Incoming connection...%d\n",cnt++);
  
  // Ugly way to wait for \r\n (i.e. end of HTTP request which we don't actually parse here)
  uint32_t timeout=millis() + 1000;
  int lcwn = 0;
  for (;;) {
    unsigned char x=0;
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
      if (lcwn) {
        break;
      }
      lcwn = 1;
    } else
      lcwn = 0;
  }
  incoming.write((uint8_t*)HTTP_RES, strlen(HTTP_RES));
  incoming.flush();
  incoming.stop();
  Serial.printf("Connection closed.\n");
}
