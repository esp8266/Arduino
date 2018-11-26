#include <catch.hpp>
#include "PolledTimeout.h"


TEST_CASE("OneShot Timeout 3000ms", "[polledTimeout]")
{
  using esp8266::polledTimeoutOneShot;
  using timeType = polledTimeoutOneShot::timeType;
  timeType before, after, delta;

  Serial.println("OneShot Timeout 3000ms");

  Serial.print("before while 1\n");
  polledTimeoutOneShot timeout(3000);
  before = millis();
  while(!timeout.expired())
    yield();
  after = millis();
  Serial.print("after while 1\n");

  delta = after - before;
  Serial.printf("delta = %lu\n", delta);

  REQUIRE(delta == 3000);


  Serial.print("reset\n");

  Serial.print("before while 2\n");  
  timeout.reset();
  before = millis();
  while(!timeout)
    yield();
  after = millis();
  Serial.print("after while 2\n");

  delta = after - before;
  Serial.printf("delta = %lu\n", delta);

  REQUIRE(delta == 3000);
}

TEST_CASE("OneShot Timeout 3000ms reset to 1000ms", "[polledTimeout]")
{
  using esp8266::polledTimeoutOneShot;
  using timeType = polledTimeoutOneShot::timeType;
  timeType before, after, delta;

  Serial.println("OneShot Timeout 3000ms");

  Serial.print("before while 1\n");
  polledTimeoutOneShot timeout(3000);
  before = millis();
  while(!timeout.expired())
    yield();
  after = millis();
  Serial.print("after while 1\n");

  delta = after - before;
  Serial.printf("delta = %lu\n", delta);

  REQUIRE(delta == 3000);


  Serial.print("reset\n");

  Serial.print("before while 2\n");  
  timeout.reset(1000);
  before = millis();
  while(!timeout)
    yield();
  after = millis();
  Serial.print("after while 2\n");

  delta = after - before;
  Serial.printf("delta = %lu\n", delta);

  REQUIRE(delta == 1000);
}

TEST_CASE("Periodic Timeout 1T 3000ms", "[polledTimeout]")
{
  using esp8266::polledTimeoutPeriodic;
  using timeType = polledTimeoutPeriodic::timeType;
  timeType before, after, delta;

  Serial.println("Periodic Timeout 1T 3000ms");

  Serial.print("before while 1\n");
  polledTimeoutPeriodic timeout(3000);
  before = millis();
  while(!timeout)
    yield();
  after = millis();
  Serial.print("after while 1\n");

  delta = after - before;
  Serial.printf("delta = %lu\n", delta);

  REQUIRE(delta == 3000);

  Serial.print("no reset needed\n");

  Serial.print("before while 2\n");  
  before = millis();
  while(!timeout)
    yield();
  after = millis();
  Serial.print("after while 2\n");
  
  delta = after - before;
  Serial.printf("delta = %lu\n", delta);

  REQUIRE(delta == 3000);
}

TEST_CASE("Periodic Timeout 10T 1000ms", "[polledTimeout]")
{
  using esp8266::polledTimeoutPeriodic;
  using timeType = polledTimeoutPeriodic::timeType;
  timeType before, after, delta;

  Serial.println("Periodic 10T Timeout 1000ms");

  int counter = 10;

  polledTimeoutPeriodic timeout(1000); 
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
  REQUIRE(delta == 10000);
}

TEST_CASE("OneShot Timeout 3000ms reset to 1000ms custom yield", "[polledTimeout]")
{
  using YieldOrSkipPolicy = esp8266::polledTimeoutPolicy::YieldOrSkip;
  using polledTimeoutOneShotYield = esp8266::polledTimeout<false, YieldOrSkipPolicy>;
  using timeType = polledTimeoutOneShotYield::timeType;
  timeType before, after, delta;

  Serial.println("OneShot Timeout 3000ms");


  Serial.print("before while 1\n");
  polledTimeoutOneShotYield timeout(3000);
  before = millis();
  while(!timeout.expired());
  after = millis();
  Serial.print("after while 1\n");
  
  delta = after - before;
  Serial.printf("delta = %lu\n", delta);

  REQUIRE(delta == 3000);


  Serial.print("reset\n");

  Serial.print("before while 2\n");  
  timeout.reset(1000);
  before = millis();
  while(!timeout);
  after = millis();
  Serial.print("after while 2\n");
  
  delta = after - before;
  Serial.printf("delta = %lu\n", delta);

  REQUIRE(delta == 1000);
}

