#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <PolledTimeout.h>
#include <Schedule.h>

#ifndef D5
#define D5 (14)
#endif

// enter your WiFi configuration below
const char* AP_SSID = "SSID";  // your router's SSID here
const char* AP_PASS = "PSK";  // your router's password here

uint32_t timeout = 30E3;  // 30 second timeout on the WiFi connection
esp8266::polledTimeout::oneShotMs wifiTimeout(timeout);  // 30 second timeout on WiFi connection

ESP8266WebServer server(80);

void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!\r\n");
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

void setup() {
  Serial.begin(74800);
  while (!Serial);
  delay(100);
  Serial.println();
  WiFi.mode(WIFI_STA);
  WiFi.begin(AP_SSID, AP_PASS);
  Serial.print(F("connecting to WiFi "));
  Serial.println(WiFi.SSID());

  wifiTimeout.reset(timeout);
  while (((!WiFi.localIP()) || (WiFi.status() != WL_CONNECTED)) && (!wifiTimeout)) {
    yield();
  }
  if ((WiFi.status() != WL_CONNECTED) || !WiFi.localIP()) {
    Serial.println(F("WiFi timed out and didn't connect"));
  } else {
    Serial.println(F("IP address: "));
    Serial.println(WiFi.localIP());
  }
  WiFi.setAutoReconnect(true);

  //if (MDNS.begin("esp8266")) {
  //    Serial.println("MDNS responder started");
  //}

  server.on("/", handleRoot);

  server.on("/on", []() {
    tone(D5, 440);
    server.send(200, "text/plain", "tone on");
  });

  server.on("/off", []() {
    noTone(D5);
    server.send(200, "text/plain", "tone off");
  });

  server.on("/sleep", []() {
    ESP.autoLightSleep();
    wifi_fpm_set_wakeup_cb([]() {
      schedule_function([]() {
        Serial.println("auto light sleep wakeup CB");
      });
    });
    server.send(200, "text/plain", "auto light sleep on");
  });

  server.on("/nosleep", []() {
    ESP.autoSleepOff();
    server.send(200, "text/plain", "sleep off");
  });

  server.onNotFound(handleNotFound);

  server.begin();

  Serial.println("HTTP server started");
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  digitalWrite(LED_BUILTIN, LOW);
  server.handleClient();
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1);
}
