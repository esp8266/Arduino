/*
  Pager Server

  The ESP8266WiFi library's WiFiServer and WiFiServerSecure
  work differently then WiFiServer and EthernetSever
  in Arduino networking libraries.
  This example demonstrates the ArduinoWiFiServer,
  which enhances the WiFiServer.
  ArduinoWiFiServer has available() behaving as documented
  and supports send-to-all-clients functionality, which
  is not implemented in ESP8266WiFi library's WiFiServer
  and WiFiServerSecure.

  The example is a simple server that echoes any incoming
  messages to all connected clients. Connect two or more
  telnet sessions to see how server.available() and
  server.print() work.

  created in September 2020 for ESP8266WiFi library
  by Juraj Andrassy https://github.com/jandrassy
*/

#include <ESP8266WiFi.h>
#include <ArduinoWiFiServer.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

ArduinoWiFiServer server(2323);

void setup() {

  Serial.begin(115200);

  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  server.begin();

  IPAddress ip = WiFi.localIP();
  Serial.println();
  Serial.println("Connected to WiFi network.");
  Serial.print("To access the server, connect with Telnet client to ");
  Serial.print(ip);
  Serial.println(" 2323");
}

void loop() {

  WiFiClient client = server.available(); // returns first client which has data to read or a 'false' client
  if (client) { // client is true only if it is connected and has data to read
    String s = client.readStringUntil('\n'); // read the message incoming from one of the clients
    s.trim(); // trim eventual \r
    Serial.println(s); // print the message to Serial Monitor
    client.print("echo: "); // this is only for the sending client
    server.println(s); // send the message to all connected clients
    server.flush(); // flush the buffers
  }
}
