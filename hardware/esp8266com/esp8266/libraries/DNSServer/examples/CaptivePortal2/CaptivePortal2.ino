#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
 
const char* ssid     = "esp8266";
boolean LEDstate[] = {LOW, false, LOW};

const char* html = "<html><head><title>Success</title><style>.bt{display:block;width:250px;height:100px;padding:10px;margin:10px;"
                    "text-align:center;border-radius:5px;color:white;font-weight:bold;font-size:70px;text-decoration:none;} "
                    "body{background:#000;} .r{background:#933;} .g{background:#363;} .y{background:#EE0;height:100px;"
                    "width:100px;border-radius:50px;} .b{background:#000;height:100px;width:100px;border-radius:50px;} "
                    ".a{font-size:35px;} td{vertical-align:middle;}</style>"
                    "</head><body><table><tr><td><div class='TGT0'></div></td><td><a class='bt g' href='/L0?v=1'>ON</a></td>"
                    "<td><a class='bt r' href='/L0?v=0'>OFF</a></td></tr><tr><td><div class='TGT2'></div></td><td>"
                    "<a class='bt g' href='/L2?v=1'>ON</a></td><td><a class='bt r' href='/L2?v=0'>OFF</a></td></tr>"
                    "<tr><td>&nbsp;</td><td><a class='bt g a' href='/ALL?v=1'><br/>ALL ON</a></td><td>"
                    "<a class='bt r a' href='/ALL?v=0'><br/>ALL OFF</a></td></tr></body></html>";

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 1, 1);
IPAddress netMsk(255, 255, 255, 0);
DNSServer dnsServer;
ESP8266WebServer server(80);

void setup() {
  pinMode(0, OUTPUT);
  pinMode(2, OUTPUT);
  digitalWrite(2, LEDstate[0]);
  digitalWrite(2, LEDstate[2]);
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, netMsk);
  WiFi.softAP(ssid);
  Serial.print("SSID: ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", apIP);
  Serial.println("USP Server started");
  server.on("/", handle_root);
  server.on("/generate_204", handle_root);  //Android captive portal
  server.on("/L0", handle_L0);
  server.on("/L2", handle_L2);
  server.on("/ALL", handle_ALL);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

}

void loop() {
  dnsServer.processNextRequest();
  server.handleClient();
}

void handleNotFound() {
  Serial.print("\t\t\t\t URI Not Found: ");
  Serial.println(server.uri());
  server.send ( 200, "text/plain", "URI Not Found" );
}

void handle_root() {
  Serial.println("Page served");
  String toSend = html;
  toSend.replace("TGT0", LEDstate[0] ? "y" : "b");
  toSend.replace("TGT2", LEDstate[2] ? "y" : "b");
  server.send(200, "text/html", toSend);
  delay(100);
}

void handle_L0() {
  change_states(0);
  handle_root();
}

void handle_L2() {
  change_states(2);
  handle_root();
}

void handle_ALL() {
  change_states(0);
  change_states(2);
  handle_root();
}

void change_states(int tgt) {
  if (server.hasArg("v")) {
    int state = server.arg("v").toInt() == 1;
    Serial.print("LED");
    Serial.print(tgt);
    Serial.print("=");
    Serial.println(state);
    LEDstate[tgt] = state ? HIGH : LOW;
    digitalWrite(tgt, LEDstate[tgt]);
  }
}
