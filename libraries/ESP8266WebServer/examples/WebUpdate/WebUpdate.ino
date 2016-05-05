/*
  To upload through terminal you can use: curl -F "image=@firmware.bin" esp8266-webupdate.local/update
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

// Put your own Wifi Router SSID/KEY here
// remember that password len should be >7 to get it working
// If you leave this wrong default values, ESP will try to connect
// to last SSID/PASS that worked on this device (if any of course)
const char* ssid = "******";
const char* password = "******";
const char* host = "esp8266-webupdate";

ESP8266WebServer server(80);
const char* serverIndex = "<form method='POST' action='/update' enctype='multipart/form-data'><input type='file' name='update'><input type='submit' value='Update'></form>";

void setup(void){
  Serial.begin(115200);
  Serial.println("\r\nBooting Sketch...");
  WiFi.mode(WIFI_AP_STA);

  // If sketch as no default SSID and bad PSK (len <8)
  // it should try to connect to SDK saved one (if any)
  Serial.print(F("Connecting with SSID of "));
  if ( strlen(password)<8 ) {
    Serial.printf( "SDK '%s'\r\n", WiFi.SSID().c_str() );
    // If autoconnect is disabled force connection
    if (!WiFi.getAutoConnect()) 
      WiFi.begin();
  } else {
    Serial.printf( "Sketch '%s'\r\n", ssid ); 
    WiFi.begin(ssid, password);
  }
  // Will be blocked in this while loop until connected
  while (WiFi.waitForConnectResult() != WL_CONNECTED){
    delay(1000);
    Serial.printf("%4ld sec : ...\n", millis()/1000);
  }

  if(WiFi.waitForConnectResult() == WL_CONNECTED){
    MDNS.begin(host);
    server.on("/", HTTP_GET, [](){
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/html", serverIndex);
    });
    server.on("/update", HTTP_POST, [](){
      server.sendHeader("Connection", "close");
      server.sendHeader("Access-Control-Allow-Origin", "*");
      server.send(200, "text/plain", (Update.hasError())?"FAIL":"OK");
      ESP.restart();
    },[](){
      HTTPUpload& upload = server.upload();
      if(upload.status == UPLOAD_FILE_START){
        Serial.setDebugOutput(true);
        WiFiUDP::stopAll();
        Serial.printf("Update: %s\n", upload.filename.c_str());
        uint32_t maxSketchSpace = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
        if(!Update.begin(maxSketchSpace)){//start with max available size
          Update.printError(Serial);
        }
      } else if(upload.status == UPLOAD_FILE_WRITE){
        if(Update.write(upload.buf, upload.currentSize) != upload.currentSize){
          Update.printError(Serial);
        }
      } else if(upload.status == UPLOAD_FILE_END){
        if(Update.end(true)){ //true to set the size to the current progress
          Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
          Update.printError(Serial);
        }
        Serial.setDebugOutput(false);
      }
      yield();
    });
    server.begin();
    MDNS.addService("http", "tcp", 80);
  
    Serial.printf("Ready! Open http://%s.local in your browser\n", host);
  } else {
    Serial.println("WiFi Failed");
  }
}
 
void loop(void){
  server.handleClient();
  delay(1);
} 
