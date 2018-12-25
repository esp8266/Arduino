#include <Arduino.h>
#include <BSTest.h>
#include <ESP8266WiFi.h>

#include <ping.h>

BS_ENV_DECLARE();

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

static struct ping_option po;
static const uint32_t PING_COUNT = 5;
static volatile uint32_t recv_count;
static volatile uint32_t done_count;

static void ping_recv(void* options, void* resp)
{
    (void)options;
    (void)resp;
    ++recv_count;
}

static void ping_done(void* options, void* resp)
{
    (void)options;
    (void)resp;
    done_count = ((struct ping_resp*)resp)->total_count;
}

TEST_CASE("pings sent/answered", "[lwip]")
{
    IPAddress address;
    if (WiFi.hostByName(getenv("SERVER_IP"), address))
    {
        po.ip = address;
        po.count = PING_COUNT;
        po.coarse_time = 1;
        po.sent_function = &ping_done;
        po.recv_function = &ping_recv;
        ping_start(&po);
        delay((PING_COUNT+2)*1000);
    }
    REQUIRE(recv_count == PING_COUNT);
    REQUIRE(done_count == PING_COUNT);
}

void loop()
{
}
