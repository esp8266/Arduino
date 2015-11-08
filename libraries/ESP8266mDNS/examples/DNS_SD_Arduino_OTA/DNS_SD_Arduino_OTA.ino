#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

const char* host = "esp8266-ota";
const char* ssid = "**********";
const char* pass = "**********";
const uint16_t ota_port = 8266;
const char* ota_pass = "1234";

WiFiUDP OTA;
WiFiServer MonitorServer(ota_port);
WiFiClient Monitor;

void setup() {
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Bare Minimum Arduino OTA");

  Serial.printf("Sketch size: %u\n", ESP.getSketchSize());
  Serial.printf("Free size: %u\n", ESP.getFreeSketchSpace());

  WiFi.begin(ssid, pass);
  if(WiFi.waitForConnectResult() == WL_CONNECTED){
    MDNS.begin(host);
    MDNS.enableArduino(ota_port, true);
    OTA.begin(ota_port);
    MonitorServer.begin();
    MonitorServer.setNoDelay(true);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi Connect Failed");
    delay(10000);
    ESP.reset();
  }
}

void loop() {
  if (OTA.parsePacket()) {
    IPAddress remote = OTA.remoteIP();
    String pass = OTA.readStringUntil(' ');
    int cmd  = OTA.parseInt();
    int port = OTA.parseInt();
    int size   = OTA.parseInt();
    
    if(!pass.equals(String(ota_pass))){
      Serial.println("ERROR: Wrong Password");
      return;
    }
    
    Serial.print("Update Start: ip:");
    Serial.print(remote);
    Serial.printf(", port:%d, size:%d\n", port, size);
    uint32_t startTime = millis();

    if(!Update.begin(size)){
      Update.printError(Serial);
      return;
    }

    WiFiUDP::stopAll();
    WiFiClient::stopAll();

    WiFiClient client;
    if (client.connect(remote, port)) {
      uint32_t written;
      while(!Update.isFinished()){
        written = Update.write(client);
        if(written > 0) client.print(written, DEC);
      }
      if(Update.end()){
        client.println("OK");
        Serial.printf("Update Success: %u\nRebooting...\n", millis() - startTime);
        ESP.restart();
      } else {
        Update.printError(client);
        Update.printError(Serial);
      }
    } else {
      Serial.printf("Connect Failed: %u\n", millis() - startTime);
    }
  }
  //IDE Monitor (connected to Serial)
  if (MonitorServer.hasClient()){
    if (!Monitor || !Monitor.connected()){
      if(Monitor) Monitor.stop();
      Monitor = MonitorServer.available();
    } else {
      MonitorServer.available().stop();
    }
  }
  if (Monitor && Monitor.connected() && Monitor.available()){
    while(Monitor.available())
      Serial.write(Monitor.read());
  }
  if(Serial.available()){
    size_t len = Serial.available();
    uint8_t * sbuf = (uint8_t *)malloc(len);
    Serial.readBytes(sbuf, len);
    if (Monitor && Monitor.connected()){
      Monitor.write((uint8_t *)sbuf, len);
      delay(0);
    }
    free(sbuf);
  }
  delay(0);
}
