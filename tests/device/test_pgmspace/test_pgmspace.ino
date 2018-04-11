#include <BSTest.h>
BS_ENV_DECLARE();


void setup()
{
    Serial.begin(115200);
    BS_RUN(Serial);
}

TEST_CASE("printf_P expands varargs correctly (#2819)", "[pgmspace]")
{
    char buf[16];
    printf_P(PSTR("String %s\n"), "Test");
    
    volatile float val = 0.023f;
    snprintf(buf, sizeof(buf), "%.05f", val);
    CHECK(String(buf) == "0.02300");
    float res;
    sscanf(buf, "%f", &res);
    CHECK(res == val);
}

TEST_CASE("strncpy_P terminates string with zeroes (#2633)", "[pgmspace]")
{
	CHECK(fabs(fmod(2.0, 1.5) - 0.5) < 1e-6);
	CHECK(fabs(fmod(-10, -3) - (-1.0)) < 1e-5);
}

void loop()
{
}
