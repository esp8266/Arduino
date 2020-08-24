/*
  Demonstrate the usage of client certificate validation
  for WiFiServerBearSSL.
  By Earle F. Philhower, III

  TLS servers can require that a client present it with an X.509
  certificate signed by a trusted authority.  Clients which try
  and connect without a x.509 key, or with an x.509 key not signed
  by the trusted authority (which could be a self-signing CA)
  can not connect.

  This example uses a predefined CA and any number of client
  certificates.  Clients will need both their X.509 cert and their
  private key, both of which are generated in the signing process.

  To run this example:
  1. Generate a private certificate-authority certificate and key:
       openssl genrsa -out ca_key.pem 2048
       openssl req -x509 -new -nodes -key ca_key.pem -days 4096 -config ca.conf -out ca_cer.pem

     KEEP ca_key.pem ABSOLUTELY SECURE, WITH IT ANYONE CAN MAKE CERTS
     SIGNED BY YOU!

     DO NOT UPLOAD ca_key.pem TO THE ESP8266, IT'S NOT NEEDED (SEE BELOW)!

     ca_cer.pem is the Public X.509 certificate for your signing authority
     and can(must) be shared and included in the server as the trust root.

  2. Generate a private server certificate and key pair (using the
     self-signed CA or any other CA you'd like)
       openssl genrsa -out server_key.pem 2048
       openssl req -out server_req.csr -key server_key.pem -new -config server.conf
       openssl x509 -req -in server_req.csr -out server_cer.pem -sha256 -CAcreateserial -days 4000 -CA ca_cer.pem -CAkey ca_key.pem

     KEEP server_key.pem SECURE, IT IS YOUR SERVER'S PRIVATE KEY.
     THIS WILL BE STORED IN THE SERVER ALONE. CLIENTS DO NOT NEED IT!

     server_cer.pem *CAN* BE SHARED WITH CLIENTS, OR THE CLIENTS CAN SIMPLY
     USE YOUR SELF-SIGNED CA_CER.PEM

  3. Generate any number of private client certificate/key pairs (using the
     private CA above)
       openssl genrsa -out client1_key.pem 2048
       openssl req -out client1_req.csr -key client1_key.pem -new -config client.conf
       openssl x509 -req -in client1_req.csr -out client1_cer.pem -sha256 -CAcreateserial -days 4000 -CA ca_cer.pem -CAkey ca_key.pem

     Every client should have its own unique certificate generated and
     a copy of that specific client's private key.

     DO NOT SHARE THE PRIVATE KEY GENERATED ABOVE!

  Included with this example are *SAMPLE* certs and keys.  They are NOT
  SECURE, since they're shared with all copies of the repo, so
  DO NOT USE THE SAMPLE CERTS, KEYS, OR CAS IN YOUR OWN PROJECT!!!

  Run this example and then try connecting to the server IP:4433.
  If you don't specify the client cert and key on the WGET command
  line, you will not get connected.

  ex: wget --quiet --O - --no-check-certificate --certificate=client1_cer.pem --private-key=client1_key.pem https://esp.ip.add.ress/

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

// The server which will require a client cert signed by the trusted CA
BearSSL::WiFiServerSecure server(443);

// The hardcoded certificate authority for this example.
// Don't use it on your own apps!!!!!
const char ca_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIC1TCCAb2gAwIBAgIJAMPt1Ms37+hLMA0GCSqGSIb3DQEBCwUAMCExCzAJBgNV
BAYTAlVTMRIwEAYDVQQDDAkxMjcuMC4wLjMwHhcNMTgwMzE0MDQyMTU0WhcNMjkw
NTMxMDQyMTU0WjAhMQswCQYDVQQGEwJVUzESMBAGA1UEAwwJMTI3LjAuMC4zMIIB
IjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKCAQEAxsa4qU/tlzN4YTcnn/I/ffsi
jOPc8QRcwClKzasIZNFEye4uThl+LGZWFIFb8X8Dc+xmmBaWlPJbqtphgFKStpar
DdduHSW1ud6Y1FVKxljo3UwCMrYm76Q/jNzXJvGs6Z1MDNsVZzGJaoqit2H2Hkvk
y+7kk3YbEDlcyVsLOw0zCKL4cd2DSNDyhIZxWo2a8Qn5IdjWAYtsTnW6MvLk/ya4
abNeRfSZwi+r37rqi9CIs++NpL5ynqkKKEMrbeLactWgHbWrZeaMyLpuUEL2GF+w
MRaAwaj7ERwT5gFJRqYwj6bbfIdx5PC7h7ucbyp272MbrDa6WNBCMwQO222t4wID
AQABoxAwDjAMBgNVHRMEBTADAQH/MA0GCSqGSIb3DQEBCwUAA4IBAQCmXfrC42nW
IpL3JDkB8YlB2QUvD9JdMp98xxo33+xE69Gov0e6984F1Gluao0p6sS7KF+q3YLS
4hjnzuGzF9GJMimIB7NMQ20yXKfKpmKJ7YugMaKTDWDhHn5679mKVbLSQxHCUMEe
tEnMT93/UaDbWBjV6zu876q5vjPMYgDHODqO295ySaA71UkijaCn6UwKUT49286T
V9ZtzgabNGHXfklHgUPWoShyze+G3g29I1BR0qABoJI63zaNu8ua42v5g1RldxsW
X8yKI14mFOGxuvcygG8L2xxysW7Zq+9g+O7gW0Pm6RDYnUQmIwY83h1KFCtYCJdS
2PgozwkkUNyP
-----END CERTIFICATE-----
)EOF";

// The server's private key which must be kept secret
const char server_private_key[] PROGMEM = R"EOF(
-----BEGIN RSA PRIVATE KEY-----
MIIEowIBAAKCAQEAsRNVTvqP++YUh8NrbXwE83xVsDqcB3F76xcXNKFDERfVd2P/
LvyDovCcoQtT0UCRgPcxRp894EuPH/Ru6Z2Lu85sV//i7ce27tc2WRFSfuhlRxHP
LJWHxTl1CEfXp/owkECQ4MB3pw6Ekc16iTEPiezTG+T+mQ/BkiIwcIK6CMlpR9DI
eYUTqv0f9NrUfAjdBrqlEO2gpgFvLFrkDEU2ntAIc4aPOP7yDOym/xzfy6TiG8Wo
7nlh6M97xTZGfbEPCH9rZDjo5istym1HzF5P+COq+OTSPscjFGXoi978o6hZwa7i
zxorg4h5a5lGnshRu2Gl+Ybfa14OwnIrv/yCswIDAQABAoIBAHxwgbsHCriTcEoY
Yx6F0VTrQ6ydA5mXfuYvS/eIfIE+pp1IgMScYEXZobjrJPQg1CA1l0NyFSHS97oV
JPy34sMQxcLx6KABgeVHCMJ/EeJtnv7a3SUP0GIhhsVS95Lsl8RIG4hWub+EzFVK
eZqAB9N9wr4Pp3wZPodbz37B38rb1QPyMFmQOLlHjKTOmoxsXhL2ot+R3+aLYSur
oPO1kQo7/d0UAZoy8h9OQN4a2EXvawh4O2EvFGbc5X/yXwAdEQ4NPp9VZhkNIRkV
+XZ3FcIqEVOploKtRF/tVBTz3g61/lFz21L9PMmV5y8tvSafr2SpJugGVmp2rrVQ
VNyGlIECgYEA10JSI5gmeCU3zK6kvOfBp54hY/5dDrSUpjKkMxpmm7WZQ6Il/k7A
hMcLeMzHiriT7WhRIXF8AOr2MoEkHkH3DhVNN4ccieVZx2SE5P5mVkItZGLrrpfU
dysR/ARAI1HYegGUiKacZtf9SrRavU0m7fOVOiYwbFRhjyX+MyuteYkCgYEA0pbz
4ZosetScP68uZx1sGlTfkcqLl7i15DHk3gnj6jKlfhvC2MjeLMhNDtKeUAuY7rLQ
guZ0CCghWAv0Glh5eYdfIiPhgqFfX4P5F3Om4zQHVPYj8xHfHG4ZP7dKQTndrO1Q
fLdGDTQLVXabAUSp2YGrijC8J9idSW1pYClvF1sCgYEAjkDn41nzYkbGP1/Swnwu
AEWCL4Czoro32jVxScxSrugt5wJLNWp508VukWBTJhugtq3Pn9hNaJXeKbYqVkyl
pgrxwpZph7+nuxt0r5hnrO2C7eppcjIoWLB/7BorAKxf8REGReBFT7nBTBMwPBW2
el4U6h6+tXh2GJG1Eb/1nnECgYAydVb0THOx7rWNkNUGggc/++why61M6kYy6j2T
cj05BW+f2tkCBoctpcTI83BZb53yO8g4RS2yMqNirGKN2XspwmTqEjzbhv0KLt4F
X4GyWOoU0nFksXiLIFpOaQWSwWG7KJWrfGJ9kWXR0Xxsfl5QLoDCuNCsn3t4d43T
K7phlwKBgHDzF+50+/Wez3YHCy2a/HgSbHCpLQjkknvgwkOh1z7YitYBUm72HP8Z
Ge6b4wEfNuBdlZll/y9BQQOZJLFvJTE5t51X9klrkGrOb+Ftwr7eI/H5xgcadI52
tPYglR5fjuRF/wnt3oX9JlQ2RtSbs+3naXH8JoherHaqNn8UpH0t
-----END RSA PRIVATE KEY-----
)EOF";

// The server's public certificate which must be shared
const char server_cert[] PROGMEM = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDTzCCAjcCCQDPXvMRYOpeuDANBgkqhkiG9w0BAQsFADCBpjESMBAGA1UEAwwJ
MTI3LjAuMC4xMQswCQYDVQQGEwJVUzElMCMGA1UECgwcTXkgT3duIENlcnRpZmlj
YXRlIEF1dGhvcml0eTEUMBIGA1UECAwLQXJkdWlub0xhbmQxFTATBgNVBAcMDEFy
ZHVpbm9WaWxsZTEVMBMGA1UECgwMRVNQODI2NlVzZXJzMRgwFgYDVQQLDA9FU1A4
MjY2LUFyZHVpbm8wHhcNMTgwMzE0MDQwMDAwWhcNMjkwMjI0MDQwMDAwWjAsMRYw
FAYDVQQKDA1NeSBTZXJ2ZXIgT3JnMRIwEAYDVQQDDAkxMjcuMC4wLjMwggEiMA0G
CSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQCxE1VO+o/75hSHw2ttfATzfFWwOpwH
cXvrFxc0oUMRF9V3Y/8u/IOi8JyhC1PRQJGA9zFGnz3gS48f9G7pnYu7zmxX/+Lt
x7bu1zZZEVJ+6GVHEc8slYfFOXUIR9en+jCQQJDgwHenDoSRzXqJMQ+J7NMb5P6Z
D8GSIjBwgroIyWlH0Mh5hROq/R/02tR8CN0GuqUQ7aCmAW8sWuQMRTae0Ahzho84
/vIM7Kb/HN/LpOIbxajueWHoz3vFNkZ9sQ8If2tkOOjmKy3KbUfMXk/4I6r45NI+
xyMUZeiL3vyjqFnBruLPGiuDiHlrmUaeyFG7YaX5ht9rXg7Cciu//IKzAgMBAAEw
DQYJKoZIhvcNAQELBQADggEBAEnG+FNyNCOkBvzHiUpHHpScxZqM2f+XDcewJgeS
L6HkYEDIZZDNnd5gduSvkHpdJtWgsvJ7dJZL40w7Ba5sxpZHPIgKJGl9hzMkG+aA
z5GMkjys9h2xpQZx9KL3q7G6A+C0bll7ODZlwBtY07CFMykT4Mp2oMRrQKRucMSV
AB1mKujLAnMRKJ3NM89RQJH4GYiRps9y/HvM5lh7EIK/J0/nEZeJxY5hJngskPKb
oPPdmkR97kaQnll4KNsC3owVlHVU2fMftgYkgQLzyeWgzcNa39AF3B6JlcOzNyQY
seoK24dHmt6tWmn/sbxX7Aa6TL/4mVlFoOgcaTJyVaY/BrY=
-----END CERTIFICATE-----
)EOF";

// Note there are no client certificates required here in the server.
// That is because all clients will send a certificate that can be
// proven to be signed by the public CA certificate included at the
// head of the app.

// Set time via NTP, as required for x.509 validation
void setClock()
{
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

  setClock(); // Required for X.509 validation

  // Attach the server private cert/key combo
  BearSSL::X509List *serverCertList = new BearSSL::X509List(server_cert);
  BearSSL::PrivateKey *serverPrivKey = new BearSSL::PrivateKey(server_private_key);
  server.setRSACert(serverCertList, serverPrivKey);

  // Require a certificate validated by the trusted CA
  BearSSL::X509List *serverTrustedCA = new BearSSL::X509List(ca_cert);
  server.setClientTrustAnchor(serverTrustedCA);

  // Actually start accepting connections
  server.begin();
}

static const char *HTTP_RES =
        "HTTP/1.0 200 OK\r\n"
        "Connection: close\r\n"
        "Content-Length: 59\r\n"
        "Content-Type: text/html; charset=iso-8859-1\r\n"
        "\r\n"
        "<html>\r\n"
        "<body>\r\n"
        "<p>Hello my friend!</p>\r\n"
        "</body>\r\n"
        "</html>\r\n";

void loop() {
  BearSSL::WiFiClientSecure incoming = server.available();
  if (!incoming) {
    return;
  }
  Serial.println("Incoming connection...\n");
  
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
