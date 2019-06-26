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


TEST_CASE("this test runs successfully", "[bs]")
{
    CHECK(1 + 1 == 2);
    REQUIRE(2 * 2 == 4);
}

TEST_CASE("another test which fails", "[bs][fail]")
{
    CHECK(true);
    CHECK(false);
    CHECK(true);
    CHECK(false);
}

TEST_CASE("another test which fails and crashes", "[bs][fail]")
{
    CHECK(true);
    REQUIRE(false);
}


TEST_CASE("third test which should be skipped", "[.]")
{
    FAIL();
}


TEST_CASE("this test also runs successfully", "[bs]")
{

}

void loop()
{
}
