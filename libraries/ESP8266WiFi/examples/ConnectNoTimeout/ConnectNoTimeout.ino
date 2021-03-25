/*
    This sketch tries to connect to a remote server with 0 timeout.
    This causes, for the first call, to fail to connect, but a further call
    can return successfully if in the meantime the connection has been
    established.
*/

#include <ESP8266WiFi.h>
#include <PolledTimeout.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

const char* host = "djxmmx.net";
const uint16_t port = 17;

WiFiClient client;

void setup() {
  Serial.begin(115200);

  // We start by connecting to a WiFi network

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Set timeout to 0 to make it sort of async
  client.setTimeout(0);

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Use WiFiClient class to create TCP connections
  if (client.disconnected()) {
    client.connect(host, port);
  }
  /* This loop will exit if client has connected or
     if the connection has failed (e.g. due to lwip timeout)
  */
  while (!client.keepConnecting() && !client.disconnected()) {
    // do stuff while trying to connect
    delay(500);
    Serial.println("Trying to connect");
  }
  if (client.connected()) {
    //do some stuff when connected
    Serial.println("Finally connected");
  } else {
    Serial.println("Failed to connect");
  }
  delay(2000);
}
