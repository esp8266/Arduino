/*
  ESP8266 Delegate class template test by Dirk O. Kaar
  This example code is in the public domain
*/

#include "Foo.h"
#include <Delegate.h>
#include <MultiDelegate.h>

constexpr long unsigned MAXCNT = 100000UL;
const String TESTCASE = "F";
const String LATENCY = "Latency/cycles = ";
uint32_t cycles;
uint32_t cnt;

enum TestCase { F0 = 0, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12 };
TestCase testCases[] = { F0, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12 };

using Fp0 = bool (*)(Foo*, int);
using Fp2 = bool(*)(int);

Foo* o0;
Fp0 f0;
Foo* o1;
Fp0 f1;
Delegate<bool(int)> f2;
Delegate<bool(int), Foo*> f3;

std::function<bool(int)> f4;
std::function<bool(int)> f5;

Delegate<bool(int)> f6;
Delegate<bool(int)> f7;
Delegate<bool(int)> f8;

Delegate<bool(int), Foo*> f9;
Delegate<bool(int), Foo*> f10;
Delegate<bool(int), Foo*> f11;
Delegate<bool(int), Foo*> f12;

void set_f0(Fp0 _f, Foo* _o) { f0 = _f; o0 = _o; }
void set_f1(Fp0 _f, Foo* _o) { f1 = _f; o1 = _o; }
void set_f2(Fp2 _f) { f2 = { _f }; }
void set_f3(Fp2 _f) { f3 = { _f }; }

void set_f4(const std::function<bool(int)>& _f) { f4 = _f; }
void set_f5(const std::function<bool(int)>& _f) { f5 = _f; }

void set_f6(const Delegate<bool(int)>& _f) { f6 = _f; }
void set_f7(const Delegate<bool(int)>& _f) { f7 = _f; }
void set_f8(const Delegate<bool(int)>& _f) { f8 = _f; }

void set_f9(const Delegate<bool(int), Foo*>& _f) { f9 = _f; }
void set_f10(const Delegate<bool(int), Foo*>& _f) { f10 = _f; }

void set_f11(const Delegate<bool(int), Foo*>& _f) { f11 = _f; }
void set_f12(const Delegate<bool(int), Foo*>& _f) { f12 = _f; }

extern void testPrep();

void stopWatch()
{
    if (MAXCNT == cnt)
    {
        Serial.print(LATENCY);
        Serial.println(cycles / MAXCNT);
        cycles = 0;
        cnt = 0;
    }
}

void setup()
{
    Serial.begin(115200);
    testPrep();

    cycles = 0;
    cnt = 0;
}

// Add the main program code into the continuous loop() function
void loop()
{
    for (auto tc : testCases) {
        Serial.print(TESTCASE);
        Serial.print(tc);
        Serial.print(": ");
        for (unsigned i = 0; i < MAXCNT; ++i)
        {
            auto start = ESP.getCycleCount();
            switch (tc) {
            case F0: f0(o0, 42); break;
            case F1: f1(o1, 42); break;
            case F2: f2(42); break; // { cbCPtr }
            case F3: f3(42); break; // { cbCPtr }

            case F4: f4(42); break; // [o](int result) -> bool { return o->cb(result); }
            case F5: f5(42); break; // std::bind(Foo::cbwObj, o, std::placeholders::_1)

            case F6: f6(42); break; // [o](int result) -> bool { return o->cb(result); }
            case F7: f7(42); break; // std::bind(Foo::cbwObj, o, std::placeholders::_1)
            case F8: f8(42); break; // [](int result) -> bool { return cbCPtr(result); }

            case F9: f9(42); break; // [o](int result) -> bool { return o->cb(result); } <==== antipattern for Delegate, use f11 instead
            case F10: f10(42); break; // std::bind(Foo::cbwObj, o, std::placeholders::_1) <==== antipattern for Delegate, use f11 instead

            case F11: f11(42); break; // [](Foo* o, int result) -> bool { return o->cb(result); }, o.get() })
            case F12: f12(42); break; // { Foo::cbwObj, o.get() }
            }
            cycles += (ESP.getCycleCount() - start);
            stopWatch();
        }
        yield();
    }
    delay(16000);
}
