/* Multi-client Server Sent Event (aka EventSource) demo
  Run demo as follows:
  1. set SSID, password and ports, compile and run program
     you should see (random) updates of sensors A and B

  2. on the client(s), register it for the event bus using a REST API call: curl -sS "http://<your ESP IP>:<your port>/rest/events/subscribe"
     on both server and client, you should now see that your client is registered
     the server sends back the location of the event bus (channel) to the client:
      subscription for client IP <your client's IP address>: event bus location: http://<your ESP IP>:<your port + 1>/rest/events

     you will also see that the sensors are ready to broadcast state changes, but the client is not yet listening:
      SSEBroadcastState - client <your client IP>> registered but not listening

  3. on the client(s), start listening for events with: curl -sS "http://<your ESP IP>:<your port + 1>/rest/events"
     if all is well, the following is being displayed on the ESP console
      SSEHandler - registered client with IP <your client IP address> is listening...
      broadcast status change to client IP <your client IP>> for sensor[A|B] with new state <some number>>
     every minute you will see on the ESP: SSEKeepAlive - client is still connected  
      
     on the client, you should see the SSE messages coming in:
      event: event
      data: { "TYPE":"KEEP-ALIVE" }
      event: event
      data: { "TYPE":"STATE", "sensorB": {"state" : 12408, "prevState": 13502} }
      event: event
      data: { "TYPE":"STATE", "sensorA": {"state" : 17664, "prevState": 49362} }

  4. on the client, stop listening by hitting control-C
    on the ESP, after maximum one minute, the following message is displayed: SSEKeepAlive - client no longer connected, remove subscription
    if you start listening again after the time expired, the "/rest/events" handle becomes stale and "Handle not found" is returned
    you can also try to start listening again before the KeepAliver timer expires or simply register your client again
*/

extern "C" {
#include "c_types.h"
} 
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Ticker.h>

#ifndef STASSID
#define STASSID "your-ssid"
#define STAPSK  "your-password"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;
const unsigned int port = 80;

ESP8266WebServer server(port);
ESP8266WebServer SSEserver(port + 1);

#define MAX_CHANNELS 2  // in this simplified example, only two SSE clients subscription allowed
struct SSESubscription {
  uint32_t clientIP;
  WiFiClient client;
  Ticker keepAliveTimer;
} subscription[MAX_CHANNELS]; 

unsigned short sensorA = 0, sensorB = 0;   //Simulate two sensors
Ticker update, updateA, updateB;

void notFound(ESP8266WebServer &server) {
  Serial.println(F("Handle not found"));
  String message = "Handle Not Found\n\n";
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
void handleNotFound() { notFound(server); }

void SSEKeepAlive(SSESubscription *s) {
  SSESubscription &subscription = *s;
  if (!subscription.clientIP) return;
  WiFiClient client = subscription.client;  
  if (client.connected()) {
    Serial.println(F("SSEKeepAlive - client is still connected"));
    client.println(F("event: event\ndata: { \"TYPE\":\"KEEP-ALIVE\" }"));
  } else { 
    Serial.println(F("SSEKeepAlive - client no longer connected, remove subscription"));
    subscription.keepAliveTimer.detach();
    client.flush();
    client.stop();
    subscription.clientIP = 0;
  }
}

// SSEHandler handles the client connection to the event bus (client event listener)
// every 60 seconds it sends a keep alive event via Ticker
void SSEHandler(SSESubscription *s) {
  WiFiClient client = SSEserver.client();
  SSESubscription &subscription = *s;
  if (subscription.clientIP != uint32_t(client.remoteIP())) { // IP addresses don't match, reject this client
    Serial.printf_P(PSTR("SSEHandler - unregistered client with IP %s tries to listen\n"), SSEserver.client().remoteIP().toString().c_str());
    return notFound(SSEserver);
  }  
  //client.setNoDelay(true);  // Any of these will crash the ESP
  //client.setSync(true);
  Serial.printf_P(PSTR("SSEHandler - registered client with IP %s is listening...\n"), IPAddress(subscription.clientIP).toString().c_str());
  subscription.client = client; // capture SSE server client connection
  SSEserver.setContentLength(CONTENT_LENGTH_UNKNOWN); // the payload can go on forever
  subscription.keepAliveTimer.attach(30.0, std::bind(SSEKeepAlive, s));  // Refresh time every 30s for demo
}

void handleSSENotFound() { 
  const char *uri = SSEserver.uri().c_str();
  if (strncmp_P(uri, PSTR("/rest/events/"), sizeof("/rest/events"))) return notFound(SSEserver);
  uri += sizeof("/rest/events");
  Serial.printf_P(PSTR("WebServer missed .on registration for channel %d\n"), atoi(uri));
  SSEHandler(&subscription[atoi(uri)]);
};

void SSEBroadcastState(const char *sensorName, unsigned short prevSensorValue, unsigned short sensorValue) {
  for (uint8_t i = 0; i < MAX_CHANNELS; i++) { 
    if (!(subscription[i].clientIP)) continue;
    WiFiClient client = subscription[i].client;
    String IPaddrstr = IPAddress(subscription[i].clientIP).toString();
    if (client.connected()) {
      Serial.printf_P(PSTR("broadcast status change to client IP %s on channel %d for %s with new state %d\n"), 
        IPaddrstr.c_str(), i, sensorName, sensorValue); 
      client.printf_P(PSTR("event: event\ndata: {\"TYPE\":\"STATE\", \"%s\":{\"state\":%d, \"prevState\":%d}}\n"),
        sensorName, sensorValue, prevSensorValue);
    } else
      Serial.printf_P(PSTR("SSEBroadcastState - client %s registered on channel %d but not listening\n"), IPaddrstr.c_str(), i);
  }
}

// Simulate sensors
void updateSensor(const char* name, unsigned short *value) {
  unsigned short newVal = (unsigned short)RANDOM_REG32; // (not so good) random value for the sensor
  unsigned short val = *value;
  Serial.printf_P(PSTR("update sensor %s - previous state: %d, new state: %d\n"), name, val, newVal);
  if (val != newVal) SSEBroadcastState(name, newVal, val); // only broadcast if state is different
  *value = newVal;
  update.once(rand() % 20 + 10, std::bind(updateSensor, name, value));  // randomly update sensor A 
}

void handleSubscribe() {
  IPAddress clientIP = server.client().remoteIP();   // get IP address of client
  String SSEurl = F("http://");
  SSEurl += WiFi.localIP().toString();
  SSEurl += F(":");
  SSEurl += port + 1;
  size_t offset = SSEurl.length();
  SSEurl += F("/rest/events/");
 
  uint8_t channel = -1;
  for (uint8_t i = 0; i < MAX_CHANNELS; i++) {
      if (subscription[i].clientIP == (uint32_t) clientIP) {
      Serial.println(F("ERROR: only one channel per IP address"));  
      if (channel >= 0)
        subscription[channel].clientIP = 0;  // invalide previously allocated channel
      return notFound(SSEserver);
    } else if (!subscription[i].clientIP) {  // Free slot, allocate new subscription
      channel = i;
      subscription[i] = {(uint32_t) clientIP, SSEserver.client(), Ticker()};
      SSEurl += channel;
      SSEurl += F("\n");
      Serial.printf_P(PSTR("Allocated channel %d, on uri %s\n"), channel, SSEurl.substring(offset).c_str());  
      SSEserver.on(SSEurl.substring(offset), std::bind(SSEHandler, &(subscription[i])));
      break;
    }
  } 
  Serial.printf_P(PSTR("subscription for client IP %s: event bus location: %s\n"), clientIP.toString().c_str(), SSEurl.c_str());  
  server.send_P(200, "text/plain", SSEurl.c_str());
}

void startServers() {
  server.on(F("/rest/events/subscribe"), handleSubscribe);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  SSEserver.onNotFound(handleSSENotFound);
  //SSEserver.keepCurrentClient(true);  // Looks like it is not needed
  SSEserver.begin();
  Serial.println("HTTP SSE EventSource server started");
}

void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {   // Wait for connection
    delay(500);
    Serial.print(".");
  }
  Serial.printf_P(PSTR("\nConnected to %s with IP address: %s\n"), ssid, WiFi.localIP().toString().c_str());
  if (MDNS.begin("esp8266"))
    Serial.println("MDNS responder started");

  //for (uint8_t i = 0; i <MAX_CHANNELS; i++) subscription[i].clientIP = 0; // ESP gcc++ doesn not seem to initialize to zero
  startServers();   // start web and SSE servers
  updateSensor("sensorA", &sensorA);
  updateSensor("sensorB", &sensorB);
}

void loop(void) {
  server.handleClient();
  SSEserver.handleClient();
  MDNS.update();
  yield();
}
