#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid = "YOUR-SSID";
const char* password = "YOUR-SSID-PASSWORD";

// Create an instance of the WiFiServer class
WiFiServer telnetServer(23);

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to Wi-Fi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start the Telnet server
  telnetServer.begin();
  Serial.println("Telnet server started");
}

void loop() {
  WiFiClient client = telnetServer.available();

  if (client) {
    Serial.println("New client connected");
    client.println("Welcome to the ESP8266 Telnet Server");

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        client.write(c);
      }
    }

    Serial.println("Client disconnected");
    client.stop();
  }
}