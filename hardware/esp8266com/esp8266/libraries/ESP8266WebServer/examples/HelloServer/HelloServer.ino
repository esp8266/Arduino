#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
 
const char* ssid = "...............";
const char* password = "...............";


ESP8266WebServer server(80);
 
const int led = 13;
 
void handle_root() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  delay(100);
  digitalWrite(led, 0);
}
 
void setup(void)
{
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);

  // Connect to WiFi network
  WiFi.begin(ssid, password);
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
   
  server.on("/", handle_root);
  
  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });
  
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void)
{
  server.handleClient();
} 
