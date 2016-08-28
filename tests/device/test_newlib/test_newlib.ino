#include <BSTest.h>
BS_ENV_DECLARE();


void setup()
{
    Serial.begin(115200);
    BS_RUN(Serial);
}

TEST_CASE("Floating point formatting works", "[newlib]")
{
    char buf[16];
    const float val = 0.02300;
    snprintf(buf, sizeof(buf), "%.05f", val);
    CHECK(String(buf) == "0.02300");
    float res;
    sscanf(buf, "%f", &res);
    CHECK(res == val);
}

TEST_CASE("#612 fmod and sqrt work", "[newlib]")
{
	CHECK(fabs(fmod(2.0, 1.5) - 0.5) < 1e-6);
	CHECK(fabs(fmod(-10, -3) - (-1.0)) < 1e-5);
}

void loop()
{
}
