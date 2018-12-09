#include <catch.hpp>
#include "PolledTimeout.h"

//This won't work for
template<typename argT>
inline bool
fuzzycomp(argT a, argT b)
{
  const argT epsilon = 10;
  return (std::max(a,b) - std::min(a,b) <= epsilon);
}

TEST_CASE("OneShot Timeout 3000ms", "[polledTimeout]")
{
  using esp8266::polledTimeout::oneShot;
  using timeType = oneShot::timeType;
  timeType before, after, delta;

  Serial.println("OneShot Timeout 3000ms");

  oneShot timeout(3000);
  before = millis();
  while(!timeout.expired())
    yield();
  after = millis();

  delta = after - before;
  Serial.printf("delta = %lu\n", delta);

  REQUIRE(fuzzycomp(delta, (timeType)3000));


  Serial.print("reset\n");

  timeout.reset();
  before = millis();
  while(!timeout)
    yield();
  after = millis();

  delta = after - before;
  Serial.printf("delta = %lu\n", delta);

  REQUIRE(fuzzycomp(delta, (timeType)3000));
}

TEST_CASE("OneShot Timeout 3000ms reset to 1000ms", "[polledTimeout]")
{
  using esp8266::polledTimeout::oneShot;
  using timeType = oneShot::timeType;
  timeType before, after, delta;

  Serial.println("OneShot Timeout 3000ms");

  oneShot timeout(3000);
  before = millis();
  while(!timeout.expired())
    yield();
  after = millis();

  delta = after - before;
  Serial.printf("delta = %lu\n", delta);

  REQUIRE(fuzzycomp(delta, (timeType)3000));


  Serial.print("reset\n");

  timeout.reset(1000);
  before = millis();
  while(!timeout)
    yield();
  after = millis();

  delta = after - before;
  Serial.printf("delta = %lu\n", delta);

  REQUIRE(fuzzycomp(delta, (timeType)1000));
}

TEST_CASE("Periodic Timeout 1T 3000ms", "[polledTimeout]")
{
  using esp8266::polledTimeout::periodic;
  using timeType = periodic::timeType;
  timeType before, after, delta;

  Serial.println("Periodic Timeout 1T 3000ms");

  periodic timeout(3000);
  before = millis();
  while(!timeout)
    yield();
  after = millis();

  delta = after - before;
  Serial.printf("delta = %lu\n", delta);

  REQUIRE(fuzzycomp(delta, (timeType)3000));

  Serial.print("no reset needed\n");

  before = millis();
  while(!timeout)
    yield();
  after = millis();
  
  delta = after - before;
  Serial.printf("delta = %lu\n", delta);

  REQUIRE(fuzzycomp(delta, (timeType)3000));
}

TEST_CASE("Periodic Timeout 10T 1000ms", "[polledTimeout]")
{
  using esp8266::polledTimeout::periodic;
  using timeType = periodic::timeType;
  timeType before, after, delta;

  Serial.println("Periodic 10T Timeout 1000ms");

  int counter = 10;

  periodic timeout(1000); 
  before = millis();
  while(1)
  {
    if(timeout)
    {
      Serial.print("*");
      if(!--counter)
        break;
      yield();
    }
  }
  after = millis();
  
  delta = after - before;
  Serial.printf("\ndelta = %lu\n", delta);
  REQUIRE(fuzzycomp(delta, (timeType)10000));
}

TEST_CASE("OneShot Timeout 3000ms reset to 1000ms custom yield", "[polledTimeout]")
{
  using YieldOrSkipPolicy = esp8266::polledTimeout::YieldPolicy::YieldOrSkip;
  using oneShotYield = esp8266::polledTimeout::timeoutTemplate<false, YieldOrSkipPolicy>;
  using timeType = oneShotYield::timeType;
  timeType before, after, delta;

  Serial.println("OneShot Timeout 3000ms");


  oneShotYield timeout(3000);
  before = millis();
  while(!timeout.expired());
  after = millis();
  
  delta = after - before;
  Serial.printf("delta = %lu\n", delta);

  REQUIRE(fuzzycomp(delta, (timeType)3000));


  Serial.print("reset\n");

  timeout.reset(1000);
  before = millis();
  while(!timeout);
  after = millis();
  
  delta = after - before;
  Serial.printf("delta = %lu\n", delta);

  REQUIRE(fuzzycomp(delta, (timeType)1000));
}

