#include <Arduino.h>
#include <BSTest.h>
#include <ESP8266WiFi.h>

extern "C" {
#include "user_interface.h"
}

BS_ENV_DECLARE();

// no need for #include
struct tcp_pcb;
extern struct tcp_pcb* tcp_tw_pcbs;
extern "C" void tcp_abort (struct tcp_pcb* pcb);

void tcpCleanup (void)
{
  while (tcp_tw_pcbs)
    tcp_abort(tcp_tw_pcbs);
}

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    WiFi.persistent(false);
    WiFi.mode(WIFI_STA);
    WiFi.begin(getenv("STA_SSID"), getenv("STA_PASS"));
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    BS_RUN(Serial);
}

TEST_CASE("WiFi release ClientContext", "[clientcontext]")
{
    #define MAXLOOPS     50
    #define SUCCESS_GOAL 10
    #define srv          getenv("SERVER_IP")

    WiFiClient client;
    
    Serial.print(srv);

    // look for reachable port on gateway
    int port;
    for (port = 8266; port <= 8285; port++)
        if (client.connect(srv, port))
        {
            client.stop();
            break;
        }
    if (port > 8285)
        port = 0;

    Serial.printf(":%d\r\n", port);
    
    int loops = 0;
    int success = 0;

    if (port)
    {
        tcpCleanup();
        int heapStart = ESP.getFreeHeap();
        int minHeap = heapStart / 2;
        int heap = heapStart;
        Serial.printf("heap: %d\r\n", heap);

        while (success < SUCCESS_GOAL && ++loops <= MAXLOOPS && (int)ESP.getFreeHeap() > minHeap)
            if (client.connect(srv, port))
            {
                client.stop();
                tcpCleanup();
                int newHeap = (int)ESP.getFreeHeap();
                Serial.printf("%03d %5d %d\r\n", loops, newHeap, newHeap - heap);
                if (newHeap - heap == 0)
                    success++; 
                heap = newHeap;
            }

        Serial.printf("heap: %d\r\n"
                      "loops: %d\r\nstable-loops: %d\r\n",
            ESP.getFreeHeap(),
            loops,
            success);
    }

    REQUIRE(success >= SUCCESS_GOAL);
}

void loop()
{
}
