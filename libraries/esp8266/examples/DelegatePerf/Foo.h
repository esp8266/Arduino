/*
  ESP8266 Delegate class template test by Dirk O. Kaar
  This example code is in the public domain
*/

#include <ctype.h>

extern uint32_t cnt;

struct Foo {
    int val;
    bool cb(int result) { val = result; ++cnt; return true; }
    static bool cbwObj(Foo* obj, int result) { return ((Foo*)obj)->cb(result); }
};