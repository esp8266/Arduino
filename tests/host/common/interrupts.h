#ifndef INTERRUPTS_H
#define INTERRUPTS_H

class InterruptLock {
public:
    InterruptLock() {
        _state = 0x00;
    }

    ~InterruptLock() {
        _state = 0x0f;
    }

    uint32_t savedInterruptLevel() const {
        return _state & 0x0f;
    }

protected:
    uint32_t _state;
};

#define AutoInterruptLock(intrLevel) \
class _AutoDisableIntr { \
public: \
    _AutoDisableIntr() {} \
    ~_AutoDisableIntr() {} \
    }; \
_AutoDisableIntr _autoDisableIntr 

#endif //INTERRUPTS_H
