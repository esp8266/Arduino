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
  ex: curl --insecure --cert client1_cer.pem --key client1_key.pem https://esp.ip.add.ress/

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

constexpr int port = 443;

// The server which will require a client cert signed by the trusted CA
BearSSL::WiFiServerSecure server(port);

// The hardcoded certificate authority for this example.
// The server's private key which must be kept secret
// The server's public certificate which must be shared
// Don't use them on your own apps!!!!!
#define USING_INSECURE_CERTS_AND_KEYS_AND_CAS 1
#include <ssl-tls-ca-key-cert-example.h>

// Note there are no client certificates required here in the server.
// That is because all clients will send a certificate that can be
// proven to be signed by the public CA certificate included at the
// head of the app.

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

  setClock();  // Required for X.509 validation

  // Attach the server private cert/key combo
  BearSSL::X509List *serverCertList = new BearSSL::X509List(server_cert);
  BearSSL::PrivateKey *serverPrivKey = new BearSSL::PrivateKey(server_private_key);
  server.setRSACert(serverCertList, serverPrivKey);

  // Require a certificate validated by the trusted CA
  BearSSL::X509List *serverTrustedCA = new BearSSL::X509List(ca_cert);
  server.setClientTrustAnchor(serverTrustedCA);

  // Actually start accepting connections
  server.begin();

  Serial.println("This example requires a client certificate.");
  Serial.printf("ex: wget --quiet --O - --no-check-certificate --certificate=client1_cer.pem --private-key=client1_key.pem https://%s:%d/\n", WiFi.localIP().toString().c_str(), port);
  Serial.printf("ex: curl --insecure --cert client1_cer.pem --key client1_key.pem https://%s:%d/\n", WiFi.localIP().toString().c_str(), port);
}

static const char *HTTP_RES = "HTTP/1.0 200 OK\r\n"
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
  BearSSL::WiFiClientSecure incoming = server.accept();
  if (!incoming) { return; }
  Serial.println("Incoming connection...\n");

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
