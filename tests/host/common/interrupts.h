#ifndef INTERRUPTS_H
#define INTERRUPTS_H

class InterruptLock {
public:
    uint32_t savedInterruptLevel() const {
        return 0x0f;
    }
};

#define AutoInterruptLock(intrLevel) \
class _AutoDisableIntr { \
public: \
    _AutoDisableIntr() {} \
    ~_AutoDisableIntr() {} \
    }; \
_AutoDisableIntr _autoDisableIntr 

#endif //INTERRUPTS_H
