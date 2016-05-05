#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>

const char* ssid = "********";
const char* password = "********";

ESP8266WebServer server(80);

const char* www_username = "admin";
const char* www_password = "esp8266";

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // If sketch as no default SSID and PSK (both start with *)
  // it should try to connect to SDK saved one (if any)
  if (*ssid!='*' && *password!='*')
    WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED){
    // If sketch as no default SSID and PSK (both start with *)
    // it should try to connect to SDK saved one (if any)
    if (*ssid!='*' && *password!='*')
      WiFi.begin(ssid, password);
    
    Serial.println("Retrying connection...");
  }

  ArduinoOTA.begin();

  server.on("/", [](){
    if(!server.authenticate(www_username, www_password))
      return server.requestAuthentication();
    server.send(200, "text/plain", "Login OK");
  });
  server.begin();

  Serial.print("Open http://");
  Serial.print(WiFi.localIP());
  Serial.println("/ in your browser to see it working");
}

void loop() {
  ArduinoOTA.handle();
  server.handleClient();
}
