#ifndef FUNCTIONALINTERRUPT_H
#define FUNCTIONALINTERRUPT_H

#include <stddef.h>
#include <stdint.h>
#include <functional>

extern "C" {
#include "c_types.h"
#include "ets_sys.h"
}

void attachInterrupt(uint8_t pin, std::function<void(void)> intRoutine, int mode);

#endif //INTERRUPTS_H
