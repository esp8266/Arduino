/*
  Small math example, checking whether we properly integrate with c++ math

  ref:
  - https://github.com/esp8266/Arduino/issues/5530
  - https://github.com/espressif/arduino-esp32/pull/2738

  Released to public domain
*/

#include <BSTest.h>
#include <type_traits>

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

#define TEST_MATH_IS_SAME(OP1, OP2) \
    std::is_same<decltype(OP1), decltype(OP2)>::value

TEST_CASE("std::abs and abs result is the same", "[arduino-math]")
{
    CHECK(TEST_MATH_IS_SAME(abs(-5), std::abs(-5)));
    CHECK(TEST_MATH_IS_SAME(abs(-25.0), std::abs(-25.0)));
    CHECK(TEST_MATH_IS_SAME(abs(10.0), std::abs(10.0)));
    CHECK(! TEST_MATH_IS_SAME(abs(10.0), std::abs(10)));
    CHECK(! TEST_MATH_IS_SAME(abs(-5), std::abs(10.0)));
}

TEST_CASE("abs works with ints", "[arduino-math]")
{
    int a = -3;
    int b = 3;
    CHECK(TEST_MATH_IS_SAME(abs(a), a));
    CHECK(TEST_MATH_IS_SAME(abs(b), b));
    CHECK(abs(a) == b);
    CHECK(abs(b) == b);
}

bool compare_floats(float a, float b) {
    return std::fabs(a - b) < std::numeric_limits<float>::epsilon();
}

TEST_CASE("abs works with floats", "[arduino-math]")
{
    float a = -3.5;
    float b = 3.5;
    CHECK(TEST_MATH_IS_SAME(abs(a), a));
    CHECK(TEST_MATH_IS_SAME(abs(b), b));
    CHECK(compare_floats(abs(a), b));
    CHECK(compare_floats(abs(b), b));
}

TEST_CASE("round works with floats", "[arduino-math]")
{
    float a = 2.9;
    float b = 3.0;
    CHECK(TEST_MATH_IS_SAME(round(a), a));
    CHECK(TEST_MATH_IS_SAME(round(b), b));
    CHECK(compare_floats(round(a), b));
    CHECK(compare_floats(round(b), b));
}

TEST_CASE("round result is float", "[arduino-math]")
{
    float a = 2.9;
    float b = 3.0;
    CHECK(TEST_MATH_IS_SAME(round(a), a));
    CHECK(TEST_MATH_IS_SAME(round(b), b));
    CHECK(compare_floats(round(a), b));
    CHECK(compare_floats(round(b), b));
}

void loop(){}

