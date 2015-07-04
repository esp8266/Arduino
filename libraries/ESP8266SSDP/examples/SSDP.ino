#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <ESP8266WebServer.h>
#include <ESP8266SSDP.h>

const char* ssid = "************";
const char* password = "***********";

ESP8266WebServer HTTP(80);

void setup() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting WiFi...");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if(WiFi.waitForConnectResult() == WL_CONNECTED){
  
    Serial.printf("Starting HTTP...\n");
    HTTP.on("/", HTTP_GET, [](){
      HTTP.send(200, "text/plain", "Hello World!");
    });
    HTTP.on("/ssdp/schema.xml", HTTP_GET, [](){
      SSDP.schema(HTTP.client());
    });
    HTTP.begin();
  
    Serial.printf("Starting SSDP...\n");
    SSDP.begin();
    SSDP.setName((char*)"ESP8266");
    SSDP.setSerialNumber((char*)"A0123456789");
    SSDP.setURL((char*)"/");
    SSDP.setModelName((char*)"ESP-12e");
    SSDP.setModelNumber((char*)"1.0");
    SSDP.setModelURL((char*)"http://12e.espressif.com");
    SSDP.setManufacturer((char*)"Espressif");
    SSDP.setManufacturerURL((char*)"http://espressif.com");
    
    Serial.printf("Ready!\n");
  } else {
    Serial.printf("WiFi Failed\n");
    while(1) delay(100);
  }
}

void loop() {
  HTTP.handleClient();
  SSDP.update();
  delay(1);
}
