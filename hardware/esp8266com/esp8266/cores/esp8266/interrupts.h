#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stddef.h>
#include <stdint.h>
extern "C" {
#include "c_types.h"
#include "ets_sys.h"
}


#define xt_disable_interrupts(state, level) __asm__ __volatile__("rsil %0," __STRINGIFY(level) : "=a" (state))
#define xt_enable_interrupts(state)  __asm__ __volatile__("wsr %0,ps; isync" :: "a" (state) : "memory")

class InterruptLock {
public:
    InterruptLock() {
        xt_disable_interrupts(_state, 15);
    }

    ~InterruptLock() {
        xt_enable_interrupts(_state);
    }

protected:
    uint32_t _state;
};


#endif //INTERRUPTS_H
