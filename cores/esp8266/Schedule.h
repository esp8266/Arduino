#ifndef ESP_SCHEDULE_H
#define ESP_SCHEDULE_H

#include <functional>

#define SCHEDULED_FN_MAX_COUNT 32
#define SCHEDULED_FN_INITIAL_COUNT 4

// This API was not considered stable but is now stabilizing.
// Function signatures may change, queue must stay FIFO.
// You have been warned.

// Run given function ONCE next time `loop` function returns,
// or `yield` is called,
// or `run_scheduled_functions` is called.
// Use std::bind to pass arguments to a function, or call a class member function.
// Note: there is no mechanism for cancelling scheduled functions.
// Keep that in mind when binding functions to objects which may have short lifetime.
// Returns false if the number of scheduled functions exceeds SCHEDULED_FN_MAX_COUNT.
bool schedule_function(const std::function<void(void)>& fn);

// Run given function periodically about every <repeat_us> microseconds until it returns false.
// Note that it may be more than <repeat_us> microseconds between calls if `yield` is not called
// frequently, and therefore should not be used for timing critical operations.
bool schedule_function_us(const std::function<bool(void)>& fn, uint32_t repeat_us);

// Run all scheduled functions.
// Use this function if your are not using `loop`, or `loop` does not return
// on a regular basis.
void run_scheduled_functions();

#endif //ESP_SCHEDULE_H
