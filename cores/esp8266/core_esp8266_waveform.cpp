/*
  esp8266_waveform - General purpose waveform generation and control,
                     supporting outputs on all pins in parallel.

  Copyright (c) 2018 Earle F. Philhower, III.  All rights reserved.
  Copyright (c) 2020 Dirk O. Kaar.

  The core idea is to have a programmable waveform generator with a unique
  high and low period (defined in microseconds or CPU clock cycles).  TIMER1 is
  set to 1-shot mode and is always loaded with the time until the next edge
  of any live waveforms.

  Up to one waveform generator per pin supported.

  Each waveform generator is synchronized to the ESP clock cycle counter, not the
  timer.  This allows for removing interrupt jitter and delay as the counter
  always increments once per 80MHz clock.  Changes to a waveform are
  contiguous and only take effect on the next waveform transition,
  allowing for smooth transitions.

  This replaces older tone(), analogWrite(), and the Servo classes.

  Everywhere in the code where "ccy" or "ccys" is used, it means ESP.getCycleTime()
  clock cycle time, or an interval measured in clock cycles, but not TIMER1
  cycles (which may be 2 CPU clock cycles @ 160MHz).

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

// Maximum delay between IRQs, 1Hz
constexpr uint32_t MAXIRQUS = 1000000;

// Set/clear GPIO 0-15 by bitmask
#define SetGPIO(a) do { GPOS = a; } while (0)
#define ClearGPIO(a) do { GPOC = a; } while (0)

// for INFINITE, the NMI proceeds on the waveform without expiry deadline.
// for EXPIRES, the NMI expires the waveform automatically on the expiry ccy.
// for UPDATEEXPIRY, the NMI recomputes the exact expiry ccy and transitions to EXPIRES.
// for INIT, the NMI initializes nextPhaseCcy, and if expiryCcy != 0 includes UPDATEEXPIRY.
enum class WaveformMode : uint32_t {INFINITE = 0, EXPIRES = 1, UPDATEEXPIRY = 2, INIT = 3};

// Waveform generator can create tones, PWM, and servos
typedef struct {
  uint32_t nextPhaseCcy;               // ESP clock cycle when a period begins
  volatile uint32_t nextTimeDutyCcys;   // Add at low->high to keep smooth waveform
  volatile uint32_t nextTimePeriodCcys; // Set next phase cycle at low->high to maintain phase
  volatile uint32_t expiryCcy;         // For time-limited waveform, the CPU clock cycle when this waveform must stop. If ExpiryState::UPDATE, temporarily holds relative ccy count
  volatile WaveformMode mode;
} Waveform;

static Waveform waveforms[17];        // State of all possible pins
static uint32_t waveformsState = 0;   // Is the pin high or low, updated in NMI so no access outside the NMI code
static volatile uint32_t waveformsEnabled = 0; // Is it actively running, updated in NMI so no access outside the NMI code

// Enable lock-free by only allowing updates to waveformsState and waveformsEnabled from IRQ service routine
static volatile uint32_t waveformToEnable = 0;  // Message to the NMI handler to start exactly one waveform on a inactive pin
static volatile uint32_t waveformToDisable = 0; // Message to the NMI handler to disable exactly one pin from waveform generation

static uint32_t (*timer1CB)() = NULL;

// Interrupt on/off control
static ICACHE_RAM_ATTR void timer1Interrupt();
static bool timerRunning = false;


// Non-speed critical bits
#pragma GCC optimize ("Os")

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
  } else if (timerRunning && !fn && !waveformsEnabled) {
    deinitTimer();
  }
}

int startWaveform(uint8_t pin, uint32_t timeHighUS, uint32_t timeLowUS, uint32_t runTimeUS) {
  return startWaveformClockCycles(pin,
    microsecondsToClockCycles(timeHighUS), microsecondsToClockCycles(timeLowUS), microsecondsToClockCycles(runTimeUS));
}

// Start up a waveform on a pin, or change the current one.  Will change to the new
// waveform smoothly on next low->high transition.  For immediate change, stopWaveform()
// first, then it will immediately begin.
int startWaveformClockCycles(uint8_t pin, uint32_t timeHighCcys, uint32_t timeLowCcys, uint32_t runTimeCcys) {
  const auto periodCcys = timeHighCcys + timeLowCcys;
  if ((pin > 16) || isFlashInterfacePin(pin) || !periodCcys) {
    return false;
  }
  Waveform* wave = &waveforms[pin];
  wave->nextTimeDutyCcys = timeHighCcys;
  wave->nextTimePeriodCcys = periodCcys;

  uint32_t mask = 1UL << pin;
  if (!(waveformsEnabled & mask)) {
    // wave->nextPhaseCcy is initialized by the ISR
    wave->expiryCcy = runTimeCcys; // in WaveformMode::INIT, temporarily hold relative cycle count
    wave->mode = WaveformMode::INIT;
    waveformToEnable = mask;
    if (!timerRunning) {
      initTimer();
      timer1_write(microsecondsToClockCycles(2));
    }
    else if (T1L > microsecondsToClockCycles(2)) {
      // Must not interfere if Timer is due shortly
      timer1_write(microsecondsToClockCycles(2));
    }
    while (waveformToEnable) {
      delay(0); // Wait for waveform to update
    }
  }
  else {
    wave->mode = WaveformMode::INFINITE; // turn off possible expiry to make update atomic from NMI
    wave->expiryCcy = runTimeCcys; // in WaveformMode::UPDATEEXPIRY, temporarily hold relative cycle count
    if (runTimeCcys)
      wave->mode = WaveformMode::UPDATEEXPIRY;
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
  if (waveformsEnabled & (1UL << pin)) {
    waveformToDisable = 1UL << pin;
  // Must not interfere if Timer is due shortly
  if (T1L > microsecondsToClockCycles(2)) {
    timer1_write(microsecondsToClockCycles(2));
  }
    while (waveformToDisable) {
      /* no-op */ // Can't delay() since stopWaveform may be called from an IRQ
    }
  }
  if (!waveformsEnabled && !timer1CB) {
    deinitTimer();
  }
  return true;
}

// Speed critical bits
#pragma GCC optimize ("O2")

static ICACHE_RAM_ATTR void timer1Interrupt() {
  // Optimize the NMI inner loop by keeping track of the min and max GPIO that we
  // are generating.  In the common case (1 PWM) these may be the same pin and
  // we can avoid looking at the other pins.
  static int startPin = 0;
  static int endPin = 0;

  constexpr int32_t isrTimeoutCcys = microsecondsToClockCycles(14);
  const uint32_t isrStartCcy = ESP.getCycleCount();

  if (waveformToEnable || waveformToDisable) {
    // Handle enable/disable requests from main app.
    waveformsEnabled = (waveformsEnabled & ~waveformToDisable) | waveformToEnable; // Set the requested waveforms on/off
    waveformsState &= ~waveformToEnable;  // And clear the state of any just started
    waveformToEnable = 0;
    waveformToDisable = 0;
    // Find the first GPIO being generated by checking GCC's find-first-set (returns 1 + the bit of the first 1 in an int32_t)
    startPin = __builtin_ffs(waveformsEnabled) - 1;
    // Find the last bit by subtracting off GCC's count-leading-zeros (no offset in this one)
    endPin = 32 - __builtin_clz(waveformsEnabled);
  }

  uint32_t now = ESP.getCycleCount();
  int32_t nextTimerCcys = microsecondsToClockCycles(MAXIRQUS);
  uint32_t nextTimerCcy = now + nextTimerCcys;
  if (waveformsEnabled) {
      bool done = false;
      do {
      nextTimerCcys = microsecondsToClockCycles(MAXIRQUS);
      for (int i = startPin; i <= endPin; i++) {
        uint32_t mask = 1UL<<i;

        // If it's not on, ignore!
        if (!(waveformsEnabled & mask))
          continue;

        Waveform *wave = &waveforms[i];

        switch (wave->mode) {
        case WaveformMode::INIT:
          wave->nextPhaseCcy = now;
          if (!wave->expiryCcy) {
            wave->mode = WaveformMode::INFINITE;
            break;
          }
        case WaveformMode::UPDATEEXPIRY:
          wave->expiryCcy += wave->nextPhaseCcy; // in WaveformMode::UPDATEEXPIRY, expiryCcy temporarily holds relative CPU cycle count
          wave->mode = WaveformMode::EXPIRES;
          break;
        default:
          break;
        }
        const uint32_t nextEventCcy = wave->nextPhaseCcy + ((waveformsState & mask) ? wave->nextTimeDutyCcys : 0);
        int32_t nextEventCcys =  nextEventCcy - now;
        const int32_t expiryCcys = (WaveformMode::EXPIRES == wave->mode) ? wave->expiryCcy - now : (nextEventCcys + 1);
        if (nextEventCcys <= 0 && expiryCcys > nextEventCcys) {
          waveformsState ^= mask;
          if (waveformsState & mask) {
            if (wave->nextTimeDutyCcys) {
              if (i == 16) {
                GP16O |= 1; // GPIO16 write slow as it's RMW
              }
              else {
                SetGPIO(mask);
              }
            }
            nextEventCcys += wave->nextTimeDutyCcys;
          }
          else {
            if (wave->nextTimeDutyCcys != wave->nextTimePeriodCcys) {
              if (i == 16) {
                GP16O &= ~1; // GPIO16 write slow as it's RMW
              }
              else {
                ClearGPIO(mask);
              }
            }
            // handles overshoot where an updated period is shorter than the previous duty cycle
            // maintains phase if the previous period is an integer multiple of the new period
            do {
              wave->nextPhaseCcy += wave->nextTimePeriodCcys;
              nextEventCcys = wave->nextPhaseCcy - now;
            } while (nextEventCcys <= -static_cast<int32_t>(wave->nextTimeDutyCcys));
          }
        }
        if (nextTimerCcys > nextEventCcys)
          nextTimerCcys = nextEventCcys;

        // Disable any waveforms that are done
        if ((WaveformMode::EXPIRES == wave->mode)) {
          if (expiryCcys <= 0) {
            // Done, remove!
            waveformsEnabled &= ~mask;
          }
          else if (nextTimerCcys > expiryCcys)
          {
            nextTimerCcys = expiryCcys;
          }
        }
      }

      nextTimerCcy = now + nextTimerCcys;
      now = ESP.getCycleCount();
      nextTimerCcys = nextTimerCcy - now;
      // Exit the loop if the next event, if any, is sufficiently distant,
      // and guard against insanely short waveform periods that the ISR cannot maintain
      const int32_t isrRemainingCcys = isrTimeoutCcys - (now - isrStartCcy);
      done = !waveformsEnabled || (nextTimerCcys >= isrRemainingCcys) || (isrRemainingCcys <= 0);
    } while (!done);
  } // if (waveformsEnabled)

  if (timer1CB) {
    int32_t callbackCcys = microsecondsToClockCycles(timer1CB());
    // Account for unknown duration of timer1CB().
    nextTimerCcys = nextTimerCcy - ESP.getCycleCount();
    if (nextTimerCcys > callbackCcys)
      nextTimerCcys = callbackCcys;
  }

  // Firing timer too soon, the NMI occurs before ISR has returned.
  // But, must fire timer early to reach deadlines for waveforms.
  if (nextTimerCcys <= microsecondsToClockCycles(6))
    nextTimerCcys = microsecondsToClockCycles(2);
  else
    nextTimerCcys -= microsecondsToClockCycles(4);

  if (clockCyclesPerMicrosecond() == 160)
    timer1_write(nextTimerCcys / 2);
  else
    timer1_write(nextTimerCcys);
}

};
