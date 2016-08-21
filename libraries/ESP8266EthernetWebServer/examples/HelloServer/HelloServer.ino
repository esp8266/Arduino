/*
Simple ESP8266 web server sample for wired connection. 
Connect Wiznet W5100 to GPIO 12 (MSIO), 13 (MOSI), 14 (CLK) and 15 (CS)
Some times W5100 can take long time to start serve TCP. If you have problems with connecting to your ESP8266 check ping first. 
If module does not reply to ping check your wirings. If module reply to ping get cup of coffee and it probably answer also to http after that. 

This sample is taken from ESP8266 repository without Licence header information and modified by Tapio Haapala 2016 for ESP8266EthernetWebServer.
*/

#include <SPI.h>
#include <Ethernet.h>
#include <ESP8266EthernetWebServer.h>

// Enter a MAC address and IP address for your controller below.

byte mac[] = {
  0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
// The IP address will be dependent on your local network:
IPAddress ip(192, 168, 1, 69);

ESP8266EthernetWebServer server(80);

const int led = 13;

void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266 ethernet port!");
}

void handleNotFound(){
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void){
  // Open serial communications and wait for port to open:
  Serial.begin(9600);  
  Serial.println();

  // start the ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  server.on("/", handleRoot);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}