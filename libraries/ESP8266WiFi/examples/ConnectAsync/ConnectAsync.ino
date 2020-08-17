/*
    This sketch tries to connect asynchronously to a remote server.
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

bool isDNSSearchInProgress = false;
unsigned long lastTry = 0;
WiFiClient client;

esp8266::polledTimeout::oneShotFastMs connectTimeout(5000); //use fully qualified type and avoid importing all ::esp8266 namespace to the global namespace

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

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  // Use WiFiClient class to create TCP connections
  if (client.connected()) {
    //do some stuff
  } else {
    // Try to connect
    if (connectTimeout) {
      int connected = client.connectAsync(host, port, &isDNSSearchInProgress);
      if (!connected) {
        if (isDNSSearchInProgress) {
          Serial.println("Connection in progress");
        } else {
          Serial.println("Connection failed");
        }
		connectTimeout.reset();
      } else {
        // isDNSSearchInProgress doesn't matter here
        Serial.println("Connected");
      }
      lastTry = millis();
    }
  }
  delay(1);
}
