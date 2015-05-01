#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
 
const char* ssid = "........";
const char* password = "........";
MDNSResponder mdns;

ESP8266WebServer server(80);

void handle_root() {
  server.send(200, "text/plain", "hello from esp8266!");
}

bool handle_not_found(){
  String message = "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  message += "\nNotFound!";
  server.send(404, "text/plain", message);
  return true;
}
 
void setup(void){
  Serial.begin(115200);
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
  
  if (mdns.begin("esp8266", WiFi.localIP())) Serial.println("MDNS responder started");
  
  server.on("/", handle_root);
  
  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handle_not_found);
  
  server.begin();
  Serial.println("HTTP server started");
}
 
void loop(void){
  mdns.update();
  server.handleClient();
} 
