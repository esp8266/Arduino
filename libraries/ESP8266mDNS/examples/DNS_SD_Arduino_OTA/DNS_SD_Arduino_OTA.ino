#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUDP.h>
 
const char* host = "esp8266-ota";
const char* ssid = "**********";
const char* pass = "**********";
const uint16_t aport = 8266;
 
WiFiUDP listener;

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("");
  Serial.println("Arduino OTA Test");
  
  Serial.printf("Sketch size: %u\n", ESP.getSketchSize());
  Serial.printf("Free size: %u\n", ESP.getFreeSketchSpace());
  
  WiFi.begin(ssid, pass);
  if(WiFi.waitForConnectResult() == WL_CONNECTED){
    MDNS.begin(host);
    MDNS.addService("arduino", "tcp", aport);
    listener.begin(aport);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }
}

void loop() {
  if (ota_cmd_listener.parsePacket()) {
    IPAddress remote = listener.remoteIP();
    int cmd  = listener.parseInt();
    int port = listener.parseInt();
    int sz   = listener.parseInt();
    Serial.printf("Starting Update: cmd:%d, port:%d, size:%d\r\n", cmd, port, sz);
    WiFiClient cl;
    if (!cl.connect(remote, port)) {
      Serial.println("Failed to connect");
      return;
    }
    listener.stop();
    if (!ESP.updateSketch(cl, sz)) {
      Serial.println("Update failed");
      listener.begin(aport);
    }
  }
  delay(100);
}
