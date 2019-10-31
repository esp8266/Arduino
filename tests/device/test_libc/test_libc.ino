#include <BSTest.h>
#include <StreamString.h>

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

extern "C" {
  extern void memmove_main(void);
  extern void memcpy_main(void);
  extern void strcmp_main(void);
  extern void tstring_main(void);
  extern void libm_test_string(void);
  int rand_seed = 1539;
}

void loop() {
}

// These tests crash the system if they fail.
TEST_CASE("libc memmove1 test", "[libc]")
{
  Serial.printf("memmove1: ");
  memmove_main();
  REQUIRE (1==1);
}

TEST_CASE("libc memcpy-1 test", "[libc]")
{
  Serial.printf("memcpy-1: ");
  memcpy_main();
  REQUIRE(1==1);
}

TEST_CASE("libc strcmp-1 test", "[libc]")
{
  Serial.printf("strcmp-1: ");
  strcmp_main();
  REQUIRE(1==1);
}

TEST_CASE("libc tstring test", "[libc]")
{
  Serial.printf("tstring: ");
  tstring_main();
  REQUIRE(1==1);
}

TEST_CASE("libc libm string test", "[libc]")
{
  Serial.printf("libm_string: ");
  libm_test_string();
  REQUIRE(1==1);
}

