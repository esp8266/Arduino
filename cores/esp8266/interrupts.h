#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <Arduino.h>

// these auto classes wrap up xt_rsil so your code can be simplier, but can only be
// used in an ino or cpp files. 

// InterruptLock is used when you want to completely disable interrupts
//{
//    {
//      InterruptLock lock; 
//      // do work within interrupt lock here
//    }
//    do work outside of interrupt lock here outside its scope
//}
//

namespace esp8266
{

class InterruptLock {
public:
    InterruptLock() {
        _state = xt_rsil(15);
    }

    ~InterruptLock() {
        xt_wsr_ps(_state);
    }

    uint32_t savedInterruptLevel() const {
        return _state & 0x0f;
    }

protected:
    uint32_t _state;
};

// AutoInterruptLock is when you need to set a specific level, A normal use pattern is like
//
//{
//    {
//      AutoInterruptLock(1); // this routine will allow level 2 and above
//      // do work within interrupt lock here
//    }
//    do work outside of interrupt lock here outside its scope
//}
//
#define AutoInterruptLock(intrLevel) \
class _AutoDisableIntr { \
public: \
    _AutoDisableIntr() { _savedPS = xt_rsil(intrLevel);  } \
    ~_AutoDisableIntr() { xt_wsr_ps(_savedPS); } \
private: \
    uint32_t _savedPS; \
    }; \
_AutoDisableIntr _autoDisableIntr 

} // esp8266

#endif //INTERRUPTS_H
