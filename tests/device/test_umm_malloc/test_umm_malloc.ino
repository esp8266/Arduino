// test that we can include umm_malloc.h from sketch (#1652)
#include <umm_malloc/umm_malloc.h>

#include <BSTest.h>

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

TEST_CASE("umm_info can be called", "[umm_malloc]")
{
    umm_info(NULL, 1);
}

void loop()
{
}
