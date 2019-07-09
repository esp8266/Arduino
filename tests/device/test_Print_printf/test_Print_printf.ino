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

TEST_CASE("Print::printf works for any reasonable output length", "[Print]")
{

    auto test_printf = [](size_t size) {
        StreamString str;
        auto buf = new char[size + 1];
        for (size_t i = 0; i < size; ++i) {
            buf[i] = 'a';
        }
        buf[size] = 0;
        str.printf("%s%8d", buf, 56789102);
        delete[] buf;
        CHECK(str.length() == size + 8);
        CHECK(str.substring(size) == "56789102");
    };

    auto before = ESP.getFreeHeap();
    test_printf(1);
    test_printf(10);
    test_printf(100);
    test_printf(1000);
    test_printf(10000);
    auto after = ESP.getFreeHeap();
    CHECK(before == after);
}

void loop() {}
