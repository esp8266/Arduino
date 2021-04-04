#include <catch.hpp>
#include "PolledTimeout.h"

#define mockverbose printf
#include "common/MockEsp.cpp" // getCycleCount

//This won't work for
template<typename argT>
inline bool
fuzzycomp(argT a, argT b)
{
  const argT epsilon = 10;
  return (std::max(a,b) - std::min(a,b) <= epsilon);
}

TEST_CASE("OneShot Timeout 500000000ns (0.5s)", "[polledTimeout]")
{
  using PolledTimeout::oneShotFastNs;
  using timeType = oneShotFastNs::timeType;
  timeType before, after, delta;

  Serial.println("OneShot Timeout 500000000ns (0.5s)");

  oneShotFastNs timeout(500000000);
  before = micros();
  while(!timeout.expired())
    yield();
  after = micros();

  delta = after - before;
  Serial.printf("delta = %u\n", delta);

  REQUIRE(fuzzycomp(delta/1000, (timeType)500));


  Serial.print("reset\n");

  timeout.reset();
  before = micros();
  while(!timeout)
    yield();
  after = micros();

  delta = after - before;
  Serial.printf("delta = %u\n", delta);

  REQUIRE(fuzzycomp(delta/1000, (timeType)500));
}

TEST_CASE("OneShot Timeout 3000000us", "[polledTimeout]")
{
  using PolledTimeout::oneShotFastUs;
  using timeType = oneShotFastUs::timeType;
  timeType before, after, delta;

  Serial.println("OneShot Timeout 3000000us");

  oneShotFastUs timeout(3000000);
  before = micros();
  while(!timeout.expired())
    yield();
  after = micros();

  delta = after - before;
  Serial.printf("delta = %u\n", delta);

  REQUIRE(fuzzycomp(delta/1000, (timeType)3000));


  Serial.print("reset\n");

  timeout.reset();
  before = micros();
  while(!timeout)
    yield();
  after = micros();

  delta = after - before;
  Serial.printf("delta = %u\n", delta);

  REQUIRE(fuzzycomp(delta/1000, (timeType)3000));
}

TEST_CASE("OneShot Timeout 3000ms", "[polledTimeout]")
{
  using PolledTimeout::oneShotMs;
  using timeType = oneShotMs::timeType;
  timeType before, after, delta;

  Serial.println("OneShot Timeout 3000ms");

  oneShotMs timeout(3000);
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
  using PolledTimeout::oneShotMs;
  using timeType = oneShotMs::timeType;
  timeType before, after, delta;

  Serial.println("OneShot Timeout 3000ms");

  oneShotMs timeout(3000);
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
  using PolledTimeout::periodicMs;
  using timeType = periodicMs::timeType;
  timeType before, after, delta;

  Serial.println("Periodic Timeout 1T 3000ms");

  periodicMs timeout(3000);
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
  using PolledTimeout::periodicMs;
  using timeType = periodicMs::timeType;
  timeType before, after, delta;

  Serial.println("Periodic 10T Timeout 1000ms");

  int counter = 10;

  periodicMs timeout(1000);
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
  using YieldOrSkipPolicy = PolledTimeout::YieldPolicy::YieldOrSkip;
  using oneShotMsYield = PolledTimeout::TimeoutTemplate<false, YieldOrSkipPolicy>;
  using timeType = oneShotMsYield::timeType;
  timeType before, after, delta;

  Serial.println("OneShot Timeout 3000ms");


  oneShotMsYield timeout(3000);
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

