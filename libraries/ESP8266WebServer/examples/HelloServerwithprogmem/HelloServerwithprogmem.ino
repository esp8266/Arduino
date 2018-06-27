#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "hello.h"

ESP8266WebServer server(80);      // Server Port  hier einstellen



void handleRoot() {
  server.send(200, "text/plain", (F("hello from esp8266!")));
}

void setup()
{
  Serial.begin(115200);
  Serial.println();

  Serial.print("Setting soft-AP ... ");
  boolean result = WiFi.softAP("ESPsoftAP_01", "pass-to-soft-AP");
  if(result == true)
  {
    Serial.println("Ready");
  }
  else
  {
    Serial.println("Failed!");
  }

    server.on("/", handleRoot);

    server.on("/inline", [](){
      server.send(200, "text/plain", (F("this works as well")));
    });

    server.on("/hello.png", [](){
      server.send_P(200, "image/png", hello_png, hello_png_len);      // sending a picture
    });

    server.begin();
    Serial.println(F("HTTP server started"));
}

void loop()
}
{
  server.handleClient();
