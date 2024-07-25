#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// Your STA WiFi Credentials
// ( This is the AP your ESP will connect to )
const char *ssid = "...";
const char *password = "...";

// Your AP WiFi Credentials
// ( This is the AP your ESP will broadcast )
const char *ap_ssid = "ESP8266_Demo";
const char *ap_password = "";

ESP8266WebServer server(80);

const int led = 13;

// ON_STA_FILTER - Only accept requests coming from STA interface
bool ON_STA_FILTER(ESP8266WebServer &server) {
  return WiFi.localIP() == server.client().localIP();
}

// ON_AP_FILTER - Only accept requests coming from AP interface
bool ON_AP_FILTER(ESP8266WebServer &server) {
  return WiFi.softAPIP() == server.client().localIP();
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_AP_STA);
  // Connect to STA
  WiFi.begin(ssid, password);
  // Start AP
  WiFi.softAP(ap_ssid, ap_password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  // This route will be accessible by STA clients only
  server.on("/", [&]() {
          digitalWrite(led, 1);
          server.send(200, "text/plain", "Hi!, This route is accessible for STA clients only");
          digitalWrite(led, 0);
        })
    .setFilter(ON_STA_FILTER);

  // This route will be accessible by AP clients only
  server.on("/", [&]() {
          digitalWrite(led, 1);
          server.send(200, "text/plain", "Hi!, This route is accessible for AP clients only");
          digitalWrite(led, 0);
        })
    .setFilter(ON_AP_FILTER);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
