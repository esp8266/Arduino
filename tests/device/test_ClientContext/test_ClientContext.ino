#include <Arduino.h>
#include <BSTest.h>
#include <test_config.h>
#include <ESP8266WiFi.h>
#include <Ticker.h>

extern "C" {
#include "user_interface.h"
}

BS_ENV_DECLARE();

// no need for #include
struct tcp_pcb;
extern struct tcp_pcb* tcp_tw_pcbs;
extern "C" void tcp_abort (struct tcp_pcb* pcb);

void tcpCleanup (void) {
  while (tcp_tw_pcbs)
    tcp_abort(tcp_tw_pcbs);
}

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.begin(STA_SSID, STA_PASS);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    BS_RUN(Serial);
}

TEST_CASE("WiFi release ClientContext", "[clientcontext]")
{
    #define LOOPS 20

    // will search for a tcp server these gateway's ports
    int ports [] = { 21, 23, 80, 443, 22,
                     0 };
    WiFiClient client;
    
    Serial.print(WiFi.gatewayIP());

    // look for reachable port on gateway
    int port;
    for (int i = 0; (port = ports[i]); i++)
    	if (client.connect(WiFi.gatewayIP(), ports[i])) {
    	    client.stop();
    	    break;
    	}

    Serial.printf(":%d\r\n", port);
    
    int loops = 0;
    int heapLost = 123456;

    if (port) {
    
        tcpCleanup();
        int heapStart = ESP.getFreeHeap();
        int minHeap = heapStart / 2;
        int heap = heapStart;
	Serial.printf("heap: %d\r\n", heap);

        while (++loops < LOOPS && (int)ESP.getFreeHeap() > minHeap)
            if (client.connect(WiFi.gatewayIP(), port)) {
                client.stop();
                tcpCleanup();
                int newHeap = (int)ESP.getFreeHeap();
		Serial.printf("%03d %5d %d\r\n", loops, newHeap, newHeap - heap);
		heap = newHeap;
            }

        heapLost = heapStart - ESP.getFreeHeap();

        Serial.printf("min heap: %d\r\nheap: %d\r\nloops: %d\r\nheapLost: %d\r\n",
            minHeap,
            ESP.getFreeHeap(),
            loops,
            (int)heapLost);
    }

    REQUIRE(loops == LOOPS && heapLost <= 0);
}

void loop()
{
}
