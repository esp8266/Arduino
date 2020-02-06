/*
  esp8266_waveform - General purpose waveform generation and control,
                     supporting outputs on all pins in parallel.

  Copyright (c) 2018 Earle F. Philhower, III.  All rights reserved.

  The core idea is to have a programmable waveform generator with a unique
  high and low period (defined in microseconds or CPU cycles).  TIMER1 is
  set to 1-shot mode and is always loaded with the time until the next edge
  of any live waveforms.

  Up to one waveform generator per pin supported.

  Each waveform generator is synchronized to the ESP cycle counter, not the
  timer.  This allows for removing interrupt jitter and delay as the counter
  always increments once per 80MHz clock.  Changes to a waveform are
  contiguous and only take effect on the next waveform transition,
  allowing for smooth transitions.

  This replaces older tone(), analogWrite(), and the Servo classes.

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
#include "ets_sys.h"
#include "core_esp8266_waveform.h"

extern "C" {

// Maximum delay between IRQs
#define MAXIRQUS (10000)

// Set/clear GPIO 0-15 by bitmask
#define SetGPIO(a) do { GPOS = a; } while (0)
#define ClearGPIO(a) do { GPOC = a; } while (0)

enum class ExpiryState : uint32_t {OFF = 0, ON = 1, UPDATE = 2}; // for UPDATE, the NMI computes the exact expiry cycle and transitions to ON

// Waveform generator can create tones, PWM, and servos
typedef struct {
  uint32_t nextServiceCycle;           // ESP cycle timer when a transition required
  volatile int32_t nextTimeHighCycles; // Copy over low->high to keep smooth waveform
  volatile int32_t nextTimeLowCycles;  // Copy over high->low to keep smooth waveform
  volatile uint32_t expiryCycle;       // For time-limited waveform, the cycle when this waveform must stop. In ExpiryState::UPDATE, temporarily holds relative cycle count
  volatile ExpiryState hasExpiry;      // OFF: expiryCycle (temporarily) ignored. UPDATE: expiryCycle is zero-based, NMI will recompute 
} Waveform;

static Waveform waveform[17];        // State of all possible pins
static uint32_t waveformState = 0;   // Is the pin high or low, updated in NMI so no access outside the NMI code
static volatile uint32_t waveformEnabled = 0; // Is it actively running, updated in NMI so no access outside the NMI code

// Enable lock-free by only allowing updates to waveformState and waveformEnabled from IRQ service routine
static volatile uint32_t waveformToEnable = 0;  // Message to the NMI handler to start a waveform on a inactive pin
static volatile uint32_t waveformToDisable = 0; // Message to the NMI handler to disable a pin from waveform generation

static uint32_t (*timer1CB)() = NULL;

// Interrupt on/off control
static ICACHE_RAM_ATTR void timer1Interrupt();
static bool timerRunning = false;

static void initTimer() {
  timer1_disable();
  ETS_FRC_TIMER1_INTR_ATTACH(NULL, NULL);
  ETS_FRC_TIMER1_NMI_INTR_ATTACH(timer1Interrupt);
  timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
  timerRunning = true;
}

static void ICACHE_RAM_ATTR deinitTimer() {
  ETS_FRC_TIMER1_NMI_INTR_ATTACH(NULL);
  timer1_disable();
  timer1_isr_init();
  timerRunning = false;
}

// Set a callback.  Pass in NULL to stop it
void setTimer1Callback(uint32_t (*fn)()) {
  timer1CB = fn;
  if (!timerRunning && fn) {
    initTimer();
    timer1_write(microsecondsToClockCycles(2)); // Cause an interrupt post-haste
  } else if (timerRunning && !fn && !waveformEnabled) {
    deinitTimer();
  }
}

int startWaveform(uint8_t pin, uint32_t timeHighUS, uint32_t timeLowUS, uint32_t runTimeUS) {
  return startWaveformCycles(pin,
    microsecondsToClockCycles(timeHighUS), microsecondsToClockCycles(timeLowUS), microsecondsToClockCycles(runTimeUS));
}

// Start up a waveform on a pin, or change the current one.  Will change to the new
// waveform smoothly on next low->high transition.  For immediate change, stopWaveform()
// first, then it will immediately begin.
int startWaveformCycles(uint8_t pin, uint32_t timeHighCycles, uint32_t timeLowCycles, uint32_t runTimeCycles) {
  if ((pin > 16) || isFlashInterfacePin(pin) || !(timeHighCycles + timeLowCycles)) {
    return false;
  }
  Waveform* wave = &waveform[pin];
  // Adjust to shave off some of the IRQ time, approximately
  wave->nextTimeHighCycles = timeHighCycles;
  wave->nextTimeLowCycles = timeLowCycles;

  uint32_t mask = 1UL << pin;
  if (!(waveformEnabled & mask)) {
    // Actually set the pin high or low in the IRQ service to guarantee times
    uint32_t now = ESP.getCycleCount();
    wave->nextServiceCycle = now + microsecondsToClockCycles(2);
    wave->expiryCycle = now + microsecondsToClockCycles(2) + runTimeCycles;
    wave->hasExpiry = static_cast<bool>(runTimeCycles) ? ExpiryState::ON : ExpiryState::OFF;
    waveformToEnable |= mask;
    if (!timerRunning) {
      initTimer();
    }
    timer1_write(microsecondsToClockCycles(2));
    while (waveformToEnable) {
      delay(0); // Wait for waveform to update
    }
  }
  else {
    wave->hasExpiry = ExpiryState::OFF; // turn off to make update atomic from NMI
    wave->expiryCycle = runTimeCycles; // in ExpiryState::UPDATE, temporarily hold relative cycle count
    if (runTimeCycles)
      wave->hasExpiry = ExpiryState::UPDATE;
  }
  return true;
}

// Stops a waveform on a pin
int ICACHE_RAM_ATTR stopWaveform(uint8_t pin) {
  // Can't possibly need to stop anything if there is no timer active
  if (!timerRunning) {
    return false;
  }
  // If user sends in a pin >16 but <32, this will always point to a 0 bit
  // If they send >=32, then the shift will result in 0 and it will also return false
  if (waveformEnabled & (1UL << pin)) {
    waveformToDisable = 1UL << pin;
    timer1_write(microsecondsToClockCycles(2));
    while (waveformToDisable) {
      /* no-op */ // Can't delay() since stopWaveform may be called from an IRQ
    }
  }
  if (!waveformEnabled && !timer1CB) {
    deinitTimer();
  }
  return true;
}

static ICACHE_RAM_ATTR void timer1Interrupt() {
  // Optimize the NMI inner loop by keeping track of the min and max GPIO that we
  // are generating.  In the common case (1 PWM) these may be the same pin and
  // we can avoid looking at the other pins.
  static int startPin = 0;
  static int endPin = 0;

  constexpr uint32_t isrTimeoutCycles = microsecondsToClockCycles(14);
  const uint32_t isrStart = ESP.getCycleCount();
  uint32_t nextEventCycle = isrStart + microsecondsToClockCycles(MAXIRQUS);

  if (waveformToEnable || waveformToDisable) {
    // Handle enable/disable requests from main app.
    waveformEnabled = (waveformEnabled & ~waveformToDisable) | waveformToEnable; // Set the requested waveforms on/off
    waveformState &= ~waveformToEnable;  // And clear the state of any just started
    waveformToEnable = 0;
    waveformToDisable = 0;
    // Find the first GPIO being generated by checking GCC's find-first-set (returns 1 + the bit of the first 1 in an int32_t)
    startPin = __builtin_ffs(waveformEnabled) - 1;
    // Find the last bit by subtracting off GCC's count-leading-zeros (no offset in this one)
    endPin = 32 - __builtin_clz(waveformEnabled);
  }

  bool done = false;
  if (waveformEnabled) {
    uint32_t now = ESP.getCycleCount();
    do {
      nextEventCycle = now + microsecondsToClockCycles(MAXIRQUS);
      for (int i = startPin; i <= endPin; i++) {
        uint32_t mask = 1UL<<i;

        // If it's not on, ignore!
        if (!(waveformEnabled & mask))
          continue;

        Waveform *wave = &waveform[i];

        // Check for toggles etc.
        if (ExpiryState::UPDATE == wave->hasExpiry) {
          wave->expiryCycle += wave->nextServiceCycle; // in ExpiryState::UPDATE, expiryCycle temporarily holds relative cycle count
          if (waveformState & mask)
            wave->expiryCycle += wave->nextTimeLowCycles; // update expiry time to next full period
          wave->hasExpiry = ExpiryState::ON;
        }
        const int32_t cyclesToGo = wave->nextServiceCycle - now;
        int32_t expiryToGo = (ExpiryState:: ON == wave->hasExpiry) ? wave->expiryCycle - now : (cyclesToGo + 1);
        const int32_t nextEventCycles = nextEventCycle - now;
        if (cyclesToGo >= expiryToGo) {
          // don't update waveform on or after expiring
          expiryToGo = 0;
        }
        else if (cyclesToGo > 0) {
          if (nextEventCycles > cyclesToGo)
            nextEventCycle = wave->nextServiceCycle;
        }
        else {
          waveformState ^= mask;
          if (waveformState & mask) {
            if (wave->nextTimeHighCycles) {
              if (i == 16) {
                GP16O |= 1; // GPIO16 write slow as it's RMW
              }
              else {
                SetGPIO(mask);
              }
              wave->nextServiceCycle += wave->nextTimeHighCycles;
            }
            if (nextEventCycles > wave->nextTimeHighCycles)
              nextEventCycle = wave->nextServiceCycle;
          }
          else {
            if (wave->nextTimeLowCycles) {
              if (i == 16) {
                GP16O &= ~1; // GPIO16 write slow as it's RMW
              }
              else {
                ClearGPIO(mask);
              }
              wave->nextServiceCycle += wave->nextTimeLowCycles;
            }
            if (nextEventCycles > wave->nextTimeLowCycles)
              nextEventCycle = wave->nextServiceCycle;
          }
        }

        // Disable any waveforms that are done
        if ((ExpiryState::ON == wave->hasExpiry) && expiryToGo <= 0) {
          // Done, remove!
          waveformEnabled &= ~mask;
        }
        now = ESP.getCycleCount();
      }

      // Exit the loop if we've hit the fixed runtime limit or the next event is known to be after that timeout would occur
      done = (now - isrStart >= isrTimeoutCycles) || (nextEventCycle - isrStart  >= isrTimeoutCycles);
    } while (!done);
  } // if (waveformEnabled)

  int32_t nextEventCycles;
  if (!timer1CB) {
    nextEventCycles = nextEventCycle - ESP.getCycleCount();
  }
  else {
    int32_t callbackCycles = microsecondsToClockCycles(timer1CB());
    nextEventCycles = nextEventCycle - ESP.getCycleCount();
    if (nextEventCycles > callbackCycles)
      nextEventCycles = callbackCycles;
  }

  if (nextEventCycles < microsecondsToClockCycles(2))
    nextEventCycles = microsecondsToClockCycles(2);

  if (clockCyclesPerMicrosecond() == 160)
    timer1_write(nextEventCycles / 2);
  else
    timer1_write(nextEventCycles);
}

};
