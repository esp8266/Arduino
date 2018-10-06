#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

const char *ssid = "........";
const char *password = "........";

ESP8266WebServer server(80);

const int led = 2;

void setup(void) {
    pinMode(led, OUTPUT);
    digitalWrite(led, 0);
    Serial.begin(9600);
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    Serial.println("");

    // Wait for connection
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin("esp8266")) {
        Serial.println("MDNS responder started");
    }

    server.on("/", []() {
        server.send(200, "text/plain", "hello from esp8266!");
    });

    server.on("/led/2/actions/{}", []() {
        String action = server.pathArg(0);
        if (action == "on") {
            // /led/2/actions/on
            digitalWrite(led, 1);
            server.send(200, "text/plain", "Led 2 on");
        } else if (action == "off") {
            // /led/2/actions/off
            digitalWrite(led, 0);
            server.send(200, "text/plain", "Led 2 off");
        } else {
            server.send(404, "text/plain", "Action '" + action + "' was not found");
        }
    });

    server.begin();
    Serial.println("HTTP server started");
}

void loop(void) {
    server.handleClient();
}
