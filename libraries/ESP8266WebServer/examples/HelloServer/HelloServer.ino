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

ESP8266WebServer server(80);

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, 0);
}

void handleNotFound(){
  digitalWrite(led, 1);
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
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  
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
  
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

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
