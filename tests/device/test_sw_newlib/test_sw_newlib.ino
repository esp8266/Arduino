#include <BSTest.h>
#include <cmath>
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


TEST_CASE("#7845 std::remainder works", "[newlib]")
{
        CHECK(fabs(std::remainder((double)10.123456, (double)5.0) - (double)0.123456) < 1e-5);
        CHECK(fabs(std::remainder((float)15.123456, (float)5.0) - (float)0.123456) < 1e-5);
}

void loop()
{
}
