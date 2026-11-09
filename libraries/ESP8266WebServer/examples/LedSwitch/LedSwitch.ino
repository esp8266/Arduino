#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>


/* Put your SSID & Password */
const char* ssid = "put your wifi name";  // Enter SSID here
const char* password = "input your wifi password";  //Enter Password here


ESP8266WebServer server(80);

uint8_t ledpin = 2;// connect your led to GPIO 2 of the esp
bool ledstatus = LOW;


void setup() {
  Serial.begin(115200);
  pinMode(ledpin, OUTPUT);
  
  
  WiFi.softAP(ssid, password); // remove password if your want it to be open
  IPAddress ip = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(ip);
  
  server.on("/", handle_OnConnect);
  server.on("/ledon", handle_ledon);
  server.on("/ledoff", handle_ledoff);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
}
void loop(){
  server.handleClient();
  if(ledstatus){
    digitalWrite(ledpin, HIGH);
  }
  else{
    digitalWrite(ledpin, LOW);
  }
}

void handle_OnConnect() {
  ledstatus = LOW;
  Serial.println("led status is off");
  server.send(200, "text/html", SendHTML(ledstatus));
}

void handle_ledon() {
  ledstatus = HIGH;
  Serial.println("led status is on");
  server.send(200, "text/html", SendHTML(true));
}

void handle_ledoff() {
  ledstatus = LOW;
  Serial.println("led status is off");
  server.send(200, "text/html", SendHTML(false));
}

void handle_NotFound() {
  server.send(404, "text/plain", "404: Not found");
}

String SendHTML(uint8_t ledstat){
  String ptr = "<!DOCTYPE html><html>\n";
  ptr += "<head><meta name=\"viewpoint\" content =\"width=device-width, intial-scale=1.0, user-scable=no\">\n";
  ptr += "<title>ESP8266 Web Server</title>\n";
  ptr += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr += "body{margin-top: 50px;} h1 {color: #E9D5CA; margin: 50px auto 30px;} h3{color: #809A6F; margin-bottom: 50px;}\n";
  ptr += ".buttom {display: block; width: 80px; background-color: #E9D5CA; border: none; color: white; padding: 13px 30px; text-decoration: none; font-size: 25px; margin: 0px auto 35px; cursor: pointer; border-radius:4px}\n";
  ptr += ".buttom-on {background-color: #809A6F;}\n";
  ptr += ".buttom-on:active {background-color: #c95414;}\n";
  ptr += ".buttom-off {background-color: #E9D5CA;}\n";
  ptr += ".buttom-off:active {background-color: #dfeb0c;}\n";
  ptr += "p {front-size: 14px; color: #888; margin-bottom: 10px;}\n";
  ptr += "</style></head>\n";
  ptr += "<body>\n";
  ptr += "<h1>ESP8266 Web Server</h1>\n";
  ptr += "<h3>Using access point</h3>\n";

  if(ledstat){
    ptr += "<p>LED is on</p><a class=\"buttom buttom-off\" href=\"/ledoff\">Turn LED on</a>\n";
  }
  else{
    ptr += "<p>LED is off</p><a class=\"buttom buttom-on\" href=\"/ledon\">Turn LED off</a>\n";
  }
  ptr += "</body></html>\n";
  return ptr;
}
