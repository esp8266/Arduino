/* Multi-client Server Sent Event (aka EventSource) demo
  Run demo as follows:
  1. set SSID, password and ports, compile and run program
     you should see (random) updates of sensors A and B

  2. on the client(s), register it for the event bus using a REST API call: curl -sS "http://<your
  ESP IP>:<your port>/rest/events/subscribe" on both server and client, you should now see that your
  client is registered the server sends back the location of the event bus (channel) to the client:
      subscription for client IP <your client's IP address>: event bus location: http://<your ESP
  IP>:<your port>/rest/events/<channel>

     you will also see that the sensors are ready to broadcast state changes, but the client is not
  yet listening: SSEBroadcastState - client <your client IP>> registered but not listening

  3. on the client(s), start listening for events with: curl -sS "http://<your ESP IP>:<your
  port>/rest/events/<channel>" if all is well, the following is being displayed on the ESP console
      SSEHandler - registered client with IP <your client IP address> is listening...
      broadcast status change to client IP <your client IP>> for sensor[A|B] with new state <some
  number>> every minute you will see on the ESP: SSEKeepAlive - client is still connected

     on the client, you should see the SSE messages coming in:
      event: event
      data: { "TYPE":"KEEP-ALIVE" }
      event: event
      data: { "TYPE":"STATE", "sensorB": {"state" : 12408, "prevState": 13502} }
      event: event
      data: { "TYPE":"STATE", "sensorA": {"state" : 17664, "prevState": 49362} }

  4. on the client, stop listening by hitting control-C
    on the ESP, after maximum one minute, the following message is displayed: SSEKeepAlive - client
  no longer connected, remove subscription if you start listening again after the time expired, the
  "/rest/events" handle becomes stale and "Handle not found" is returned you can also try to start
  listening again before the KeepAliver timer expires or simply register your client again
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
#define STAPSK "your-password"
#endif

const char*        ssid     = STASSID;
const char*        password = STAPSK;
const unsigned int port     = 80;

ESP8266WebServer server(port);

#define SSE_MAX_CHANNELS                                                                           \
  8  // in this simplified example, only eight SSE clients subscription allowed
struct SSESubscription {
  IPAddress  clientIP;
  WiFiClient client;
  Ticker     keepAliveTimer;
} subscription[SSE_MAX_CHANNELS];
uint8_t subscriptionCount = 0;

typedef struct {
  const char*    name;
  unsigned short value;
  Ticker         update;
} sensorType;
sensorType sensor[2];

void handleNotFound() {
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

void SSEKeepAlive() {
  for (uint8_t i = 0; i < SSE_MAX_CHANNELS; i++) {
    if (!(subscription[i].clientIP)) {
      continue;
    }
    if (subscription[i].client.connected()) {
      Serial.printf_P(PSTR("SSEKeepAlive - client is still listening on channel %d\n"), i);
      subscription[i].client.println(
          F("event: event\ndata: { \"TYPE\":\"KEEP-ALIVE\" }\n"));  // Extra newline required by SSE
                                                                    // standard
    } else {
      Serial.printf_P(
          PSTR("SSEKeepAlive - client not listening on channel %d, remove subscription\n"), i);
      subscription[i].keepAliveTimer.detach();
      subscription[i].client.flush();
      subscription[i].client.stop();
      subscription[i].clientIP = INADDR_NONE;
      subscriptionCount--;
    }
  }
}

// SSEHandler handles the client connection to the event bus (client event listener)
// every 60 seconds it sends a keep alive event via Ticker
void SSEHandler(uint8_t channel) {
  WiFiClient       client = server.client();
  SSESubscription& s      = subscription[channel];
  if (s.clientIP != client.remoteIP()) {  // IP addresses don't match, reject this client
    Serial.printf_P(PSTR("SSEHandler - unregistered client with IP %s tries to listen\n"),
                    server.client().remoteIP().toString().c_str());
    return handleNotFound();
  }
  client.setNoDelay(true);
  client.setSync(true);
  Serial.printf_P(PSTR("SSEHandler - registered client with IP %s is listening\n"),
                  IPAddress(s.clientIP).toString().c_str());
  s.client = client;                                // capture SSE server client connection
  server.setContentLength(CONTENT_LENGTH_UNKNOWN);  // the payload can go on forever
  server.sendContent_P(
      PSTR("HTTP/1.1 200 OK\nContent-Type: text/event-stream;\nConnection: "
           "keep-alive\nCache-Control: no-cache\nAccess-Control-Allow-Origin: *\n\n"));
  s.keepAliveTimer.attach_scheduled(30.0, SSEKeepAlive);  // Refresh time every 30s for demo
}

void handleAll() {
  const char* uri        = server.uri().c_str();
  const char* restEvents = PSTR("/rest/events/");
  if (strncmp_P(uri, restEvents, strlen_P(restEvents))) {
    return handleNotFound();
  }
  uri += strlen_P(restEvents);  // Skip the "/rest/events/" and get to the channel number
  unsigned int channel = atoi(uri);
  if (channel < SSE_MAX_CHANNELS) {
    return SSEHandler(channel);
  }
  handleNotFound();
};

void SSEBroadcastState(const char* sensorName, unsigned short prevSensorValue,
                       unsigned short sensorValue) {
  for (uint8_t i = 0; i < SSE_MAX_CHANNELS; i++) {
    if (!(subscription[i].clientIP)) {
      continue;
    }
    String IPaddrstr = IPAddress(subscription[i].clientIP).toString();
    if (subscription[i].client.connected()) {
      Serial.printf_P(
          PSTR("broadcast status change to client IP %s on channel %d for %s with new state %d\n"),
          IPaddrstr.c_str(), i, sensorName, sensorValue);
      subscription[i].client.printf_P(PSTR("event: event\ndata: {\"TYPE\":\"STATE\", "
                                           "\"%s\":{\"state\":%d, \"prevState\":%d}}\n\n"),
                                      sensorName, sensorValue, prevSensorValue);
    } else {
      Serial.printf_P(
          PSTR("SSEBroadcastState - client %s registered on channel %d but not listening\n"),
          IPaddrstr.c_str(), i);
    }
  }
}

// Simulate sensors
void updateSensor(sensorType& sensor) {
  unsigned short newVal
      = (unsigned short)RANDOM_REG32;  // (not so good) random value for the sensor
  Serial.printf_P(PSTR("update sensor %s - previous state: %d, new state: %d\n"), sensor.name,
                  sensor.value, newVal);
  if (sensor.value != newVal) {
    SSEBroadcastState(sensor.name, sensor.value, newVal);  // only broadcast if state is different
  }
  sensor.value = newVal;
  sensor.update.once(rand() % 20 + 10, std::bind(updateSensor, sensor));  // randomly update sensor
}

void handleSubscribe() {
  if (subscriptionCount == SSE_MAX_CHANNELS - 1) {
    return handleNotFound();  // We ran out of channels
  }

  uint8_t   channel;
  IPAddress clientIP = server.client().remoteIP();  // get IP address of client
  String    SSEurl   = F("http://");
  SSEurl += WiFi.localIP().toString();
  SSEurl += F(":");
  SSEurl += port;
  size_t offset = SSEurl.length();
  SSEurl += F("/rest/events/");

  ++subscriptionCount;
  for (channel = 0; channel < SSE_MAX_CHANNELS; channel++)  // Find first free slot
    if (!subscription[channel].clientIP) {
      break;
    }
  subscription[channel] = { clientIP, server.client(), Ticker() };
  SSEurl += channel;
  Serial.printf_P(PSTR("Allocated channel %d, on uri %s\n"), channel,
                  SSEurl.substring(offset).c_str());
  // server.on(SSEurl.substring(offset), std::bind(SSEHandler, &(subscription[channel])));
  Serial.printf_P(PSTR("subscription for client IP %s: event bus location: %s\n"),
                  clientIP.toString().c_str(), SSEurl.c_str());
  server.send_P(200, "text/plain", SSEurl.c_str());
}

void startServers() {
  server.on(F("/rest/events/subscribe"), handleSubscribe);
  server.onNotFound(handleAll);
  server.begin();
  Serial.println("HTTP server and  SSE EventSource started");
}

void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {  // Wait for connection
    delay(500);
    Serial.print(".");
  }
  Serial.printf_P(PSTR("\nConnected to %s with IP address: %s\n"), ssid,
                  WiFi.localIP().toString().c_str());
  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  startServers();  // start web and SSE servers
  sensor[0].name = "sensorA";
  sensor[1].name = "sensorB";
  updateSensor(sensor[0]);
  updateSensor(sensor[1]);
}

void loop(void) {
  server.handleClient();
  MDNS.update();
  yield();
}
