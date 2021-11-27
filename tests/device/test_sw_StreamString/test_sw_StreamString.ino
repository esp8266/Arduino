
#include <Arduino.h>
#include <BSTest.h>

#define check(what, res1, res2) CHECK(strcmp(res1, res2) == 0)

#include "../../../libraries/esp8266/examples/StreamString/StreamString.ino"

BS_ENV_DECLARE();

bool pretest ()
{
    return true;
}

void setup ()
{
    Serial.begin(115200);
    BS_RUN(Serial);
}

TEST_CASE("StreamString tests", "[StreamString]")
{
    testStream();
}
