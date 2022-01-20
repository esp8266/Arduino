#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266NetBIOS.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif

const char*      ssid     = STASSID;
const char*      password = STAPSK;

ESP8266WebServer wwwserver(80);
String           content;

static void      handleRoot(void)
{
  content = F("<!DOCTYPE HTML>\n<html>Hello world from ESP8266");
  content += F("<p>");
  content += F("</html>");

  wwwserver.send(200, F("text/html"), content);
}

void setup()
{
  Serial.begin(115200);

  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  wwwserver.on("/", handleRoot);
  wwwserver.begin();

  NBNS.begin("ESP");
}

void loop()
{
  wwwserver.handleClient();
}
