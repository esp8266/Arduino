#ifndef ESP_SCHEDULE_H
#define ESP_SCHEDULE_H

#include <functional>

#define SCHEDULED_FN_MAX_COUNT 32
#define SCHEDULED_FN_INITIAL_COUNT 4

// Run given function next time `loop` function returns, 
// or `run_scheduled_functions` is called.
// Use std::bind to pass arguments to a function, or call a class member function.
bool schedule_function(std::function<void(void)> fn);

// Run all scheduled functions. 
// Use this function if your are not using `loop`, or `loop` does not return
// on a regular basis.
void run_scheduled_functions();

#endif //ESP_SCHEDULE_H
