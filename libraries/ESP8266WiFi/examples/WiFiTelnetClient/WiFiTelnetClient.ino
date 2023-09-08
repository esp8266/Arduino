#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid = "YOUR-SSID";
const char* password = "YOUR-SSID-PASSWORD";

IPAddress serverIP(54, 39, 129, 129); // Change this to the IP of your Telnet server
const int serverPort = 5000;

WiFiClient client;

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
  
  // Connect to the Telnet server
  Serial.println("Connecting to Telnet server...");
  if (client.connect(serverIP, serverPort)) {
    Serial.println("Connected to Telnet server");
  } else {
    Serial.println("Connection to Telnet server failed!");
  }
}

void loop() {
  // Check if there is data from the serial monitor
  if (Serial.available()) {
    char c = Serial.read();
    client.print(c); // Send the character to the Telnet server
  }

  // Check if there are incoming data from the Telnet server
  if (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
}