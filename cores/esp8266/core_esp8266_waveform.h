/*
  esp8266_waveform - General purpose waveform generation and stepper motor
                     control, supporting outputs on all pins in parallel.

  Copyright (c) 2018 Earle F. Philhower, III.  All rights reserved.

  The code idea is to have a programmable waveform generator with a unique
  high and low period (defined in microseconds).  TIMER1 is set to 1-shot
  mode and is always loaded with the time until the next edge of any live
  waveforms or Stepper motors.

  Up to one waveform generator or stepper driver per pin supported.

  Each waveform generator is synchronized to the ESP cycle counter, not the
  timer.  This allows for removing interrupt jitter and delay as the counter
  always increments once per 80MHz clock.  Changes to a waveform are
  contiguous and only take effect on the next low->high waveform transition,
  allowing for smooth transitions.

  This replaces older tone(), analogWrite(), and the Servo classes.

  The stepper driver supports a constant jerk (da/dt, in ticks/sec^3) to
  produce a smooth acceleration curve and as well as a constant initial
  acceleration and velocity and number of pulses.

  The stepper driver can also force all steppers to wait for completion
  by the use of a SYNC option (i.e. when completing a move where the X and
  Y need to hit at one point before moving to the next X and Y).

  The user application is responsible for actually calculating the proper
  motion profiles (a general-purpose S-curve planner is left as an exercise
  for the reader).

  The steppers should be wired using an A4988 or DRV8825 controller and
  with a single, shared direction pin.

  Everywhere in the code where "cycles" is used, it means ESP.getCycleTime()
  cycles, not TIMER1 cycles (which may be 2 CPU clocks @ 160MHz).

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include <Arduino.h>

#ifndef __ESP8266_WAVEFORM_H
#define __ESP8266_WAVEFORM_H

#ifdef __cplusplus
extern "C" {
#endif

int startWaveform(uint8_t pin, uint32_t timeHighUS, uint32_t timeLowUS, uint32_t runTimeUS);
int stopWaveform(uint8_t pin);

int setStepperDirPin(uint8_t pin);
int pushStepperMove(uint8_t pin, int dir, int sync, uint16_t pulses, float j, float a0, float v0);
int removeStepper(uint8_t pin);

#ifdef __cplusplus
}
#endif


#endif

