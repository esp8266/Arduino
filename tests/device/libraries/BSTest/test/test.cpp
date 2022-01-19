#include <stdio.h>
#include "BSTest.h"

BS_ENV_DECLARE();

int main()
{
    while (true)
    {
        try
        {
            BS_RUN();
            return 0;
        }
        catch (...)
        {
            printf("Exception\n\n");
        }
    }
    return 1;
}

TEST_CASE("this test runs successfully", "[bluesmoke]")
{
    CHECK(1 + 1 == 2);
    REQUIRE(2 * 2 == 4);
}

TEST_CASE("another test which fails", "[bluesmoke][fail]")
{
    CHECK(true);
    CHECK(false);
    CHECK(true);
    CHECK(false);
}

TEST_CASE("another test which fails and crashes", "[bluesmoke][fail]")
{
    CHECK(true);
    REQUIRE(false);
}

TEST_CASE("third test which should be skipped", "[.]")
{
    FAIL();
}

TEST_CASE("this test also runs successfully", "[bluesmoke]")
{
}

TEST_CASE("environment variables can be set and read from python", "[bluesmoke]")
{
    const char* res = getenv("VAR_FROM_PYTHON");
    REQUIRE(res != NULL);
    CHECK(strcmp(res, "42") == 0);
    setenv("VAR_FROM_TEST", "24", 1);
}
