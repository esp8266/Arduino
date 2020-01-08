#include <ESP8266WiFi.h>
#include <BSTest.h>
#include <sstream>
#include <iostream>

BS_ENV_DECLARE();

void setup()
{
    Serial.begin(115200);
    BS_RUN(Serial);
}

bool pretest()
{
    return true;
}

TEST_CASE("can print to std::cout", "[iostream]")
{
    std::stringstream test_stream("");
    test_stream << "hello stream";

    // empty the RX buffer, just in case
    Serial.readString();

    USC0(0) |= (1 << UCLBE);  // enable loopback
    std::cout << test_stream.str().c_str() << std::endl;
    delay(100);
    USC0(0) &= ~(1 << UCLBE);  // disable loopback
    
    String result = Serial.readStringUntil('\n');

    Serial.printf("result: '%s'\n", result.c_str());

    CHECK(result == test_stream.str().c_str());
}

void loop() { }
