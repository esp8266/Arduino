#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>

// Put your own Wifi Router SSID/KEY here
// remember that password len should be >7 to get it working
// If you leave this wrong default values, ESP will try to connect
// to last SSID/PASS that worked on this device (if any of course)
const char* ssid = "******";
const char* password = "******";

ESP8266WebServer server(80);

const char* www_username = "admin";
const char* www_password = "esp8266";

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

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
