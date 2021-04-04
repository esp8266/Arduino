#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <BSTest.h>
#include <pgmspace.h>

BS_ENV_DECLARE();

static ESP8266WebServer server(80);
static uint32_t siteHits = 0;
static String siteData = "";


void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    BS_RUN(Serial);
}

bool pretest()
{
    WiFi.persistent(false);
    WiFi.begin(getenv("STA_SSID"), getenv("STA_PASS"));
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
    }
    MDNS.begin("etd");
    server.onNotFound([](){ server.send(404); });
    server.begin();
    return true;
}

void handle_request()
{
    for (uint8_t i=0; i<server.args(); i++){
        // skip "plain" which is automatically added during arg parsing for post's
        if (server.argName(i) == "plain")
            continue;
        if(i > 0)
            siteData += "\n";
        siteData += server.argName(i) + " = " + server.arg(i);
    }
    siteHits++;
    server.send(200, "text/plain", siteData);
}


TEST_CASE("HTTP GET Parameters", "[HTTPServer]")
{
    {
        siteHits = 0;
        siteData = "";
        server.on("/get", HTTP_GET, &handle_request);
        uint32_t startTime = millis();
        while(siteHits == 0 && (millis() - startTime) < 10000)
        {
            MDNS.update();
            server.handleClient();
        }
        REQUIRE(siteHits > 0 && siteData.equals("var1 = val with spaces\nva=r+ = so&me%"));
    }
}

TEST_CASE("HTTP POST Parameters", "[HTTPServer]")
{
    {
        siteHits = 0;
        siteData = "";
        server.on("/post", HTTP_POST, &handle_request);
        uint32_t startTime = millis();
        while(siteHits == 0 && (millis() - startTime) < 10000)
        {
            MDNS.update();
            server.handleClient();
        }
        REQUIRE(siteHits > 0 && siteData.equals("var2 = val with spaces"));
    }
}

TEST_CASE("HTTP GET+POST Parameters", "[HTTPServer]")
{
    {
        siteHits = 0;
        siteData = "";
        server.on("/get_and_post", HTTP_POST, &handle_request);
        uint32_t startTime = millis();
        while(siteHits == 0 && (millis() - startTime) < 10000)
        {
            MDNS.update();
            server.handleClient();
        }
        REQUIRE(siteHits > 0 && siteData.equals("var3 = val with spaces\nva&r+ = so=me%"));
    }
}

#if 0
TEST_CASE("HTTP Upload", "[HTTPServer]")
{
    {
        siteHits = 0;
        siteData = "";
        server.on("/upload", HTTP_POST, &handle_request, [](){
            HTTPUpload& upload = server.upload();
            if(upload.status == UPLOAD_FILE_START){
              siteData = upload.filename;
            } else if(upload.status == UPLOAD_FILE_END){
              siteData.concat(":");
              siteData.concat(String(upload.totalSize));
              siteData.concat("\n");
            }
        });
        uint32_t startTime = millis();
        while(siteHits == 0 && (millis() - startTime) < 10000)
        {
            MDNS.update();
            server.handleClient();
        }
        REQUIRE(siteHits > 0 && siteData.equals("test.txt:16\nvar4 = val with spaces"));
    }
}
#endif

void loop()
{
}
