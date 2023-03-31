/*
  HelloServerBearSSL - Simple HTTPS server example

  This example demonstrates a basic ESP8266WebServerSecure HTTPS server
  that can serve "/" and "/inline" and generate detailed 404 (not found)
  HTTP respoinses.  Be sure to update the SSID and PASSWORD before running
  to allow connection to your WiFi network.

  Adapted by Earle F. Philhower, III, from the HelloServer.ino example.
  This example is released into the public domain.
*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServerSecure.h>
#include <ESP8266mDNS.h>
#include <umm_malloc/umm_malloc.h>
#include <umm_malloc/umm_heap_select.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK "your-password"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

BearSSL::ESP8266WebServerSecure server(443);
BearSSL::ServerSessions serverCache(5);

#define USING_INSECURE_CERTS_AND_KEYS_AND_CAS 1
#include <ssl-tls-ca-key-cert-example.h>

String bigChunk;

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "Hello from esp8266 over HTTPS!");
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) { message += " " + server.argName(i) + ": " + server.arg(i) + "\n"; }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void handleChunked() {
  server.chunkedResponseModeStart(200, F("text/html"));

  server.sendContent(bigChunk);
  server.sendContent(F("chunk 2"));
  server.sendContent(bigChunk);

  server.chunkedResponseFinalize();
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) { Serial.println("MDNS responder started"); }

  server.getServer().setRSACert(new BearSSL::X509List(server_cert), new BearSSL::PrivateKey(server_private_key));

  // Cache SSL sessions to accelerate the TLS handshake.
  server.getServer().setCache(&serverCache);

  server.on("/", handleRoot);

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.on("/chunks", handleChunked);

  server.onNotFound(handleNotFound);

  // prepare chunk in ram for sending
  constexpr int chunkLen = 4000;  // ~4KB chunk
  bigChunk.reserve(chunkLen);
  bigChunk = F("chunk of len ");
  bigChunk += chunkLen;
  String piece = F("-blah");
  while (bigChunk.length() < chunkLen - piece.length())
    bigChunk += piece;

  server.begin();
  Serial.println("HTTPS server started");
}

extern "C" void stack_thunk_dump_stack();

void processKey(Print& out, int hotKey) {
  switch (hotKey) {
    case 'd':
      {
        HeapSelectDram ephemeral;
        umm_info(NULL, true);
        break;
      }
    case 'i':
      {
        HeapSelectIram ephemeral;
        umm_info(NULL, true);
        break;
      }
    case 'h':
      {
        {
          HeapSelectIram ephemeral;
          Serial.printf(PSTR("IRAM ESP.getFreeHeap:  %u\n"), ESP.getFreeHeap());
        }
        {
          HeapSelectDram ephemeral;
          Serial.printf(PSTR("DRAM ESP.getFreeHeap:  %u\n"), ESP.getFreeHeap());
        }
        break;
      }
#ifdef DEBUG_ESP_PORT
    // From this context stack_thunk_dump_stack() will only work when Serial
    // debug is enabled.
    case 'p':
      out.println(F("Calling stack_thunk_dump_stack();"));
      stack_thunk_dump_stack();
      break;
#endif
    case 'R':
      out.printf_P(PSTR("Restart, ESP.restart(); ...\r\n"));
      ESP.restart();
      break;
    case '\r': out.println();
    case '\n': break;
    case '?':
      out.println();
      out.println(F("Press a key + <enter>"));
      out.println(F("  h    - Free Heap Report;"));
      out.println(F("  i    - iRAM umm_info(null, true);"));
      out.println(F("  d    - dRAM umm_info(null, true);"));
#ifdef DEBUG_ESP_PORT
      out.println(F("  p    - call stack_thunk_dump_stack();"));
#endif
      out.println(F("  R    - Restart, ESP.restart();"));
      out.println(F("  ?    - Print Help"));
      out.println();
      break;
    default:
      out.printf_P(PSTR("\"%c\" - Not an option?  / ? - help"), hotKey);
      out.println();
      processKey(out, '?');
      break;
  }
}


void loop(void) {
  server.handleClient();
  MDNS.update();
  if (Serial.available() > 0) {
    int hotKey = Serial.read();
    processKey(Serial, hotKey);
  }
}
