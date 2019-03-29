// Shows how to use the Maximum Fragment Length option in
// BearSSL to reduce SSL memory needs.
//
// Mar 2018 by Earle F. Philhower, III
// Released to the public domain

#include <ESP8266WiFi.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

const char *ssid = STASSID;
const char *pass = STAPSK;

void fetch(BearSSL::WiFiClientSecure *client) {
  client->write("GET / HTTP/1.0\r\nHost: tls.mbed.org\r\nUser-Agent: ESP8266\r\n\r\n");
  client->flush();
  uint32_t startMillis = millis();
  do {
    char tmp[32];
//    memset(tmp, 0, 32);
    int rlen = client->read((uint8_t*)tmp, sizeof(tmp) - 1);
    yield();
    if (rlen < 0) {
      break;
    }
    if(rlen == 0) {
      delay(10); // Give background processes some time
      continue;
    }
    tmp[rlen] = '\0';
    Serial.print(tmp);
  } while (millis() - startMillis < 5000);
  client->stop();
  Serial.printf("\n-------\n");
}

int fetchNoMaxFragmentLength() {
  int ret = ESP.getFreeHeap();

  Serial.printf("\nConnecting to https://tls.mbed.org\n");
  Serial.printf("No MFLN attempted\n");

  BearSSL::WiFiClientSecure client;
  client.setInsecure();
  client.connect("tls.mbed.org", 443);
  if (client.connected()) {
    Serial.printf("Memory used: %d\n", ret - ESP.getFreeHeap());
    ret -= ESP.getFreeHeap();
    fetch(&client);
  } else {
    Serial.printf("Unable to connect\n");
  }
  return ret;
}

int fetchMaxFragmentLength() {
  int ret = ESP.getFreeHeap();

  // Servers which implement RFC6066's Maximum Fragment Length Negotiation
  // can be configured to limit the size of TLS fragments they transmit.
  // This lets small clients, like the ESP8266, use a smaller memory buffer
  // on the receive end (all the way down to under 1KB).  Unfortunately,
  // as of March 2018, there are not many public HTTPS servers which
  // implement this option.  You can deploy your own HTTPS or MQTT server
  // with MFLN enabled, of course.
  //
  // To determine if MFLN is supported by a server use the
  // ::probeMaxFragmentLength() method before connecting, and if it
  // returns true then you can use the ::setBufferSizes(rx, tx) to shrink
  // the needed BearSSL memory while staying within protocol limits.
  //
  // If MFLN is not supported, you may still be able to mimimize the buffer
  // sizes assuming you can ensure the server never transmits fragments larger
  // than the size (i.e. by using HTTP GET RANGE methods, etc.).

  BearSSL::WiFiClientSecure client;
  client.setInsecure();
  bool mfln = client.probeMaxFragmentLength("tls.mbed.org", 443, 512);
  Serial.printf("\nConnecting to https://tls.mbed.org\n");
  Serial.printf("MFLN supported: %s\n", mfln ? "yes" : "no");
  if (mfln) {
    client.setBufferSizes(512, 512);
  }
  client.connect("tls.mbed.org", 443);
  if (client.connected()) {
    Serial.printf("Memory used: %d\n", ret - ESP.getFreeHeap());
    ret -= ESP.getFreeHeap();
    fetch(&client);
  } else {
    Serial.printf("Unable to connect\n");
  }
  return ret;
}

void setup() {
  Serial.begin(115200);

  delay(1000);
  Serial.println();
  Serial.println();

  // We start by connecting to a WiFi network
  Serial.print("Connecting to ");
  Serial.print(ssid);

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
}

void loop() {
  Serial.printf("\n\n\n\n\n");

  yield();
  int a = fetchNoMaxFragmentLength();
  yield();
  int b = fetchMaxFragmentLength();
  yield();

  Serial.printf("\n\n");
  Serial.printf("Default SSL:       %d bytes used\n", a);
  Serial.printf("512 byte MFLN SSL: %d bytes used\n", b);
}
