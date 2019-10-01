#include <map>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <BSTest.h>
#include <pgmspace.h>

BS_ENV_DECLARE();

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(false);
    BS_RUN(Serial);
}

bool pretest()
{
    WiFi.persistent(false);
    WiFi.mode(WIFI_OFF);
    return true;
}

static std::map<WiFiEvent_t, int> sEventsReceived;

static void onWiFiEvent(WiFiEvent_t event)
{
    sEventsReceived[event]++;
}

TEST_CASE("WiFi.onEvent is called for specific events", "[wifi][events]")
{
    sEventsReceived[WIFI_EVENT_STAMODE_CONNECTED] = 0;
    sEventsReceived[WIFI_EVENT_STAMODE_DISCONNECTED] = 0;
    sEventsReceived[WIFI_EVENT_STAMODE_GOT_IP] = 0;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    WiFi.onEvent(onWiFiEvent, WIFI_EVENT_STAMODE_CONNECTED);
    WiFi.onEvent(onWiFiEvent, WIFI_EVENT_STAMODE_DISCONNECTED);
    WiFi.onEvent(onWiFiEvent, WIFI_EVENT_STAMODE_GOT_IP);
    WiFi.onEvent(onWiFiEvent, WIFI_EVENT_ANY);
#pragma GCC diagnostic pop

    WiFi.mode(WIFI_STA);
    WiFi.begin(getenv("STA_SSID"), getenv("STA_PASS"));
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        REQUIRE(millis() - start < 5000);
    }
    WiFi.disconnect();
    delay(100);
    WiFi.mode(WIFI_OFF);
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    REQUIRE(sEventsReceived[WIFI_EVENT_STAMODE_CONNECTED] == 2);
    REQUIRE(sEventsReceived[WIFI_EVENT_STAMODE_DISCONNECTED] >= 2 && sEventsReceived[WIFI_EVENT_STAMODE_DISCONNECTED] % 2 == 0);
    REQUIRE(sEventsReceived[WIFI_EVENT_STAMODE_GOT_IP] == 2);
#pragma GCC diagnostic pop
}

TEST_CASE("STA mode events are called both when using DHCP and static config", "[wifi][events]")
{
    String events;
    
    auto handler1 = WiFi.onStationModeConnected([&](const WiFiEventStationModeConnected& evt){
        (void) evt;
        events += "connected,";
    });  
    auto handler2 = WiFi.onStationModeDisconnected([&](const WiFiEventStationModeDisconnected& evt){
        (void) evt;
        if (events.length()) {
            events += "disconnected,";
        }
    });
    auto handler3 = WiFi.onStationModeGotIP([&](const WiFiEventStationModeGotIP& evt){
        (void) evt;
        events += "got_ip,";
    });

    // run the test with DHCP
    WiFi.mode(WIFI_STA);
    WiFi.begin(getenv("STA_SSID"), getenv("STA_PASS"));
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        REQUIRE(millis() - start < 5000);
    }
    // save IP config
    IPAddress localIP = WiFi.localIP();
    IPAddress subnetMask = WiFi.subnetMask();
    IPAddress gatewayIP = WiFi.gatewayIP();
    WiFi.disconnect();
    delay(100);

    REQUIRE(events == "connected,got_ip,disconnected,");
    events = String();

    // now run the same with static IP config saved above

    WiFi.mode(WIFI_STA);
    WiFi.config(localIP, gatewayIP, subnetMask);
    WiFi.begin(getenv("STA_SSID"), getenv("STA_PASS"));
    start = millis();
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        REQUIRE(millis() - start < 5000);
    }
    WiFi.disconnect();
    delay(100);
    WiFi.mode(WIFI_OFF);
    REQUIRE(events == "connected,got_ip,disconnected,");
}

TEST_CASE("Events are not called if handler is deleted", "[wifi][events]")
{
    String events;
    
    WiFi.onStationModeConnected([&](const WiFiEventStationModeConnected& evt){
        (void) evt;
        events += "connected,";
    });  
    WiFi.onStationModeDisconnected([&](const WiFiEventStationModeDisconnected& evt){
        (void) evt;
        events += "disconnected,";
    });
    WiFi.onStationModeGotIP([&](const WiFiEventStationModeGotIP& evt){
        (void) evt;
        events += "got_ip,";
    });

    WiFi.mode(WIFI_STA);
    WiFi.begin(getenv("STA_SSID"), getenv("STA_PASS"));
    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        REQUIRE(millis() - start < 5000);
    }
    WiFi.disconnect();
    delay(100);

    REQUIRE(events == "");
}

void loop() {}
