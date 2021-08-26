/*
  ESP8266 Delegate class template test by Dirk O. Kaar
  This example code is in the public domain
*/

#include "Foo.h"
#ifndef NODELEGATE
#include <Delegate.h>
#else
#include <functional>
#endif
#include <memory>

extern void stopwatch();

std::shared_ptr<Foo> oPtr(new Foo());
bool inline cbCPtr(int result) {
  return oPtr->cb(result);
}


extern void set_f0(bool(*_f)(Foo*, int), Foo* _o);
extern void set_f1(bool(*_f)(Foo*, int), Foo* _o);
extern void set_f2(bool(*)(int result));
extern void set_f3(bool(*)(int result));

extern void set_f4(const std::function<bool(int)>& f);
extern void set_f5(const std::function<bool(int)>& f);

#ifndef NODELEGATE
extern void set_f6(const Delegate<bool(int)>&);
extern void set_f7(const Delegate<bool(int)>&);

extern void set_f8(const Delegate<bool(int)>&);

extern void set_f9(const Delegate<bool(int), Foo*>& f);
extern void set_f10(const Delegate<bool(int), Foo*>& f);

extern void set_f11(const Delegate<bool(int), Foo*>& f);
extern void set_f12(const Delegate<bool(int), Foo*>& f);
#else
extern void set_f6(const std::function<bool(int)>&);
extern void set_f7(const std::function<bool(int)>&);

extern void set_f8(const std::function<bool(int)>&);

extern void set_f9(const std::function<bool(int)>& f);
extern void set_f10(const std::function<bool(int)>& f);

extern void set_f11(const std::function<bool(int)>& f);
extern void set_f12(const std::function<bool(int)>& f);
#endif

void testPrep() {
  std::shared_ptr<Foo> o(oPtr);
  set_f0(Foo::cbwObj, o.get());
  set_f1([](Foo * o, int result) -> bool { return o->cb(result); }, o.get());
  set_f2(cbCPtr);
  set_f3(cbCPtr);

  set_f4([o](int result) -> bool { return o->cb(result); });
  set_f5(std::bind(Foo::cbwObj, o.get(), std::placeholders::_1));

  set_f6([o](int result) -> bool { return o->cb(result); });
  set_f7(std::bind(Foo::cbwObj, o.get(), std::placeholders::_1));

#ifndef NODELEGATE
  // hint to compiler to generate Delegate constructor for simple fp instead of functional
  using Fp2 = bool(*)(int);
  set_f8(static_cast<Fp2>([](int result) -> bool { return cbCPtr(result); }));
#else
  set_f8([](int result) -> bool { return cbCPtr(result); });
#endif

  set_f9([o](int result) -> bool { return o->cb(result); });
  set_f10(std::bind(Foo::cbwObj, o.get(), std::placeholders::_1));

#ifndef NODELEGATE
  set_f11({ [](Foo * o, int result) -> bool { return o->cb(result); }, o.get() }); // fast calling!
  set_f12({ Foo::cbwObj, o.get() }); // fast calling!
#else
  set_f11([o](int result) -> bool { return o->cb(result); });
  set_f12(std::bind(Foo::cbwObj, o.get(), std::placeholders::_1));
#endif
}
