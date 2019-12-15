#include <BSTest.h>
#include <Schedule.h>

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

TEST_CASE("scheduled functions are executed", "[schedule]")
{
    bool executed = false;
    CHECK(schedule_function([&](){
        executed = true;
    }));
    run_scheduled_functions();
    CHECK(executed);
}

TEST_CASE("scheduled functions are executed in correct order", "[schedule]")
{
    int counter = 0;
    auto fn = [&](int id) {
        CHECK(id == counter);
        ++counter;
    };
    for (int i = 0; i < 8; ++i) {
        schedule_function(std::bind(fn, i));
    }
    run_scheduled_functions();
}

TEST_CASE("functions are only executed once", "[schedule]")
{
    int counter = 0;
    auto fn = [&](){
        ++counter;
    };
    schedule_function(fn);
    schedule_function(fn);
    schedule_function(fn);
    run_scheduled_functions();
    CHECK(counter == 3);
    counter = 0;
    run_scheduled_functions();
    CHECK(counter == 0);
}

TEST_CASE("can schedule SCHEDULED_FN_MAX_COUNT functions", "[schedule]")
{
    int counter = 0;
    auto fn = [&](int id) {
        CHECK(id == counter);
        CHECK(id < SCHEDULED_FN_MAX_COUNT);
        ++counter;
    };
    int i;
    for (i = 0; i < SCHEDULED_FN_MAX_COUNT; ++i) {
        CHECK(schedule_function(std::bind(fn, i)));
    }
    CHECK(!schedule_function(std::bind(fn, i)));
    run_scheduled_functions();
    CHECK(counter == SCHEDULED_FN_MAX_COUNT);
    counter = 0;
    for (i = 0; i < SCHEDULED_FN_MAX_COUNT; ++i) {
        CHECK(schedule_function(std::bind(fn, i)));
    }
    CHECK(!schedule_function(std::bind(fn, i)));
    run_scheduled_functions();
    CHECK(counter == SCHEDULED_FN_MAX_COUNT);
}

void loop(){}
