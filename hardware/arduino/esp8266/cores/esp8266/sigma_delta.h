#ifndef SIGMA_DELTA_H
#define SIGMA_DELTA_H

#include <stdint.h>

void sigma_delta_close(uint32_t gpio);
void set_sigma_target(uint8_t target);
void set_sigma_prescale(uint8_t prescale);
void set_sigma_duty_312KHz(uint8_t duty);

#endif//SIGMA_DELTA_H
