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

  Everywhere in the code where "ccy" or "ccys" is used, it means ESP.getCycleCount()
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

#include "core_esp8266_waveform.h"
#include <Arduino.h>
#include "ets_sys.h"
#include <atomic>

extern "C" {

// Maximum delay between IRQs, 1Hz
constexpr int32_t MAXIRQCCYS = microsecondsToClockCycles(1000000);
// The SDK and hardware take some time to actually get to our NMI code, so
// decrement the next IRQ's timer value by a bit so we can actually catch the
// real CPU cycle count we want for the waveforms.
constexpr int32_t DELTAIRQ = clockCyclesPerMicrosecond() == 160 ?
  (microsecondsToClockCycles(5) / 2) >> 1 : microsecondsToClockCycles(5) / 2;
// The generator has a time quantum for switching wave cycles during the same ISR invocation
constexpr uint32_t QUANTUM = clockCyclesPerMicrosecond() == 160 ?
  microsecondsToClockCycles(1) >> 1 : microsecondsToClockCycles(1);
// The latency between in-ISR rearming of the timer and the earliest firing
constexpr int32_t IRQLATENCY = clockCyclesPerMicrosecond() == 160 ?
  microsecondsToClockCycles(2) >> 1 : microsecondsToClockCycles(2);

// Set/clear GPIO 0-15 by bitmask
#define SetGPIO(a) do { GPOS = a; } while (0)
#define ClearGPIO(a) do { GPOC = a; } while (0)

// for INFINITE, the NMI proceeds on the waveform without expiry deadline.
// for EXPIRES, the NMI expires the waveform automatically on the expiry ccy.
// for UPDATEEXPIRY, the NMI recomputes the exact expiry ccy and transitions to EXPIRES.
// for INIT, the NMI initializes nextPhaseCcy, and if expiryCcy != 0 includes UPDATEEXPIRY.
enum class WaveformMode : uint8_t {INFINITE = 0, EXPIRES = 1, UPDATEEXPIRY = 2, INIT = 3};

// Waveform generator can create tones, PWM, and servos
typedef struct {
  uint32_t nextPhaseCcy; // ESP clock cycle when a period begins
  uint32_t nextOffCcy;   // ESP clock cycle when going from duty to off
  uint32_t dutyCcys;     // Set next off cycle at low->high to maintain phase
  uint32_t periodCcys;   // Set next phase cycle at low->high to maintain phase
  uint32_t expiryCcy;    // For time-limited waveform, the CPU clock cycle when this waveform must stop. If ExpiryState::UPDATE, temporarily holds relative ccy count
  WaveformMode mode;
  int8_t alignPhase;     // < 0 no phase alignment, otherwise starts waveform in phase with given pin
} Waveform;

static Waveform waveforms[17];        // State of all possible pins
static uint32_t waveformsState = 0;   // Is the pin high or low, updated in NMI so no access outside the NMI code
static volatile uint32_t waveformsEnabled = 0; // Is it actively running, updated in NMI so no access outside the NMI code

// Enable lock-free by only allowing updates to waveformsState and waveformsEnabled from IRQ service routine
static volatile uint32_t waveformToEnable = 0;  // Message to the NMI handler to start exactly one waveform on a inactive pin
static volatile uint32_t waveformToDisable = 0; // Message to the NMI handler to disable exactly one pin from waveform generation

static uint32_t (*timer1CB)() = nullptr;

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
  std::atomic_thread_fence(std::memory_order_release);
  if (!timerRunning && fn) {
    initTimer();
    timer1_write(microsecondsToClockCycles(1)); // Cause an interrupt post-haste
  } else if (timerRunning && !fn && !waveformsEnabled) {
    deinitTimer();
  }
}

int startWaveform(uint8_t pin, uint32_t highUS, uint32_t lowUS,
  uint32_t runTimeUS, int8_t alignPhase) {
  return startWaveformClockCycles(pin,
    microsecondsToClockCycles(highUS), microsecondsToClockCycles(lowUS),
    microsecondsToClockCycles(runTimeUS), alignPhase);
}

// Start up a waveform on a pin, or change the current one.  Will change to the new
// waveform smoothly on next low->high transition.  For immediate change, stopWaveform()
// first, then it will immediately begin.
int startWaveformClockCycles(uint8_t pin, uint32_t highCcys, uint32_t lowCcys,
  uint32_t runTimeCcys, int8_t alignPhase) {
  const auto periodCcys = highCcys + lowCcys;
  // correct the upward bias for duty cycles shorter than generator quantum
  // effectively rounds to nearest quantum
  if (highCcys < QUANTUM / 2)
  {
    highCcys = 0;
  }
  else if (lowCcys < QUANTUM / 2)
  {
    highCcys = periodCcys;  
  }
  // sanity checks, including mixed signed/unsigned arithmetic safety
  if ((pin > 16) || isFlashInterfacePin(pin) || (alignPhase > 16) ||
    static_cast<int32_t>(periodCcys) <= 0 || highCcys > periodCcys) {
    return false;
  }
  Waveform& wave = waveforms[pin];
  wave.dutyCcys = highCcys;
  wave.periodCcys = periodCcys;

  if (!(waveformsEnabled & (1UL << pin))) {
    // wave.nextPhaseCcy and wave.nextOffCcy are initialized by the ISR
    wave.expiryCcy = runTimeCcys; // in WaveformMode::INIT, temporarily hold relative cycle count
    wave.mode = WaveformMode::INIT;
    wave.alignPhase = (alignPhase < 0) ? -1 : alignPhase;
    if (!wave.dutyCcys) {
      // If initially at zero duty cycle, force GPIO off
      if (pin == 16) {
        GP16O &= ~1; // GPIO16 write slow as it's RMW
      }
      else {
        ClearGPIO(1UL << pin);
      }
    }
    std::atomic_thread_fence(std::memory_order_release);
    waveformToEnable = 1UL << pin;
    if (!timerRunning) {
      initTimer();
      timer1_write(microsecondsToClockCycles(1));
    }
    else if (T1L > DELTAIRQ + IRQLATENCY) {
      // Must not interfere if Timer is due shortly, cluster phases to reduce interrupt load
      timer1_write(microsecondsToClockCycles(1));
    }
    while (waveformToEnable) {
      delay(0); // Wait for waveform to update
    }
  }
  else {
    wave.mode = WaveformMode::INFINITE; // turn off possible expiry to make update atomic from NMI
    std::atomic_thread_fence(std::memory_order_release);
    wave.expiryCcy = runTimeCcys; // in WaveformMode::UPDATEEXPIRY, temporarily hold relative cycle count
    std::atomic_thread_fence(std::memory_order_release);
    if (runTimeCcys) {
      wave.mode = WaveformMode::UPDATEEXPIRY;
      std::atomic_thread_fence(std::memory_order_release);
    }
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
  if (T1L > DELTAIRQ + IRQLATENCY) {
    timer1_write(microsecondsToClockCycles(1));
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

  const uint32_t isrStartCcy = ESP.getCycleCount();

  if (waveformToEnable || waveformToDisable) {
    // Handle enable/disable requests from main app.
    waveformsEnabled = (waveformsEnabled & ~waveformToDisable) | waveformToEnable; // Set the requested waveforms on/off
    waveformToEnable = 0;
    waveformToDisable = 0;
    // Find the first GPIO being generated by checking GCC's find-first-set (returns 1 + the bit of the first 1 in an int32_t)
    startPin = __builtin_ffs(waveformsEnabled) - 1;
    // Find the last bit by subtracting off GCC's count-leading-zeros (no offset in this one)
    endPin = 32 - __builtin_clz(waveformsEnabled);
  }

  // Exit the loop if the next event, if any, is sufficiently distant.
  const uint32_t isrTimeoutCcy = isrStartCcy + microsecondsToClockCycles(12);
  uint32_t now = ESP.getCycleCount();
  uint32_t nextEventCcy = now + MAXIRQCCYS;
  bool busy = waveformsEnabled;
  while (busy) {
    for (int pin = startPin; pin <= endPin; ++pin) {
      // If it's not on, ignore!
      if (!(waveformsEnabled & (1UL << pin)))
        continue;

      Waveform& wave = waveforms[pin];

      switch (wave.mode) {
      case WaveformMode::INIT:
        waveformsState &= ~(1UL << pin); // Clear the state of any just started
        wave.nextPhaseCcy = (waveformsEnabled & (1UL << wave.alignPhase)) ?
          waveforms[wave.alignPhase].nextPhaseCcy : now;
        if (!wave.expiryCcy) {
          wave.mode = WaveformMode::INFINITE;
          break;
        }
      case WaveformMode::UPDATEEXPIRY:
        wave.expiryCcy += wave.nextPhaseCcy; // in WaveformMode::UPDATEEXPIRY, expiryCcy temporarily holds relative CPU cycle count
        wave.mode = WaveformMode::EXPIRES;
        break;
      default:
        break;
      }

      const bool duty = waveformsState & (1UL << pin);
      nextEventCcy = duty ? wave.nextOffCcy : wave.nextPhaseCcy;

      if (WaveformMode::EXPIRES == wave.mode && static_cast<int32_t>(nextEventCcy - wave.expiryCcy) >= 0) {
        if (static_cast<int32_t>(now - wave.expiryCcy) >= 0) {
          // Disable any waveforms that are done
          waveformsEnabled ^= 1UL << pin;
        }
        else {
          nextEventCcy = wave.expiryCcy;
        }
      }
      else {
        const int32_t overshootCcys = now - nextEventCcy;
        if (overshootCcys >= 0) {
          if (duty) {
            const bool endOfPeriod = wave.nextPhaseCcy == wave.nextOffCcy;
            const uint32_t skipPeriodCcys = ((overshootCcys + wave.dutyCcys) > wave.periodCcys) ? ((overshootCcys + wave.dutyCcys) / wave.periodCcys) * wave.periodCcys : 0;
            if (wave.dutyCcys == wave.periodCcys) {
              wave.nextPhaseCcy += skipPeriodCcys;
              wave.nextOffCcy = wave.nextPhaseCcy;
              nextEventCcy = wave.nextPhaseCcy;
            }
            else if (endOfPeriod) {
              // preceeding period had zero off cycle, continue direct into new duty cycle
              wave.nextPhaseCcy += skipPeriodCcys;
              wave.nextOffCcy = wave.nextPhaseCcy + wave.dutyCcys;
              wave.nextPhaseCcy += wave.periodCcys;
              nextEventCcy = wave.nextOffCcy;
            }
            else {
              waveformsState ^= 1UL << pin;
              nextEventCcy = wave.nextPhaseCcy;
              if (pin == 16) {
                GP16O &= ~1; // GPIO16 write slow as it's RMW
              }
              else {
                ClearGPIO(1UL << pin);
              }
            }
          }
          else {
            const uint32_t skipPeriodCcys = (static_cast<uint32_t>(overshootCcys) > wave.periodCcys) ? (overshootCcys / wave.periodCcys) * wave.periodCcys : 0;
            wave.nextPhaseCcy += skipPeriodCcys;
            if (!wave.dutyCcys) {
              wave.nextPhaseCcy += wave.periodCcys;
              wave.nextOffCcy = wave.nextPhaseCcy;
            }
            else {
              waveformsState ^= 1UL << pin;
              wave.nextOffCcy = wave.nextPhaseCcy + wave.dutyCcys;
              wave.nextPhaseCcy += wave.periodCcys;
              if (pin == 16) {
                GP16O |= 1; // GPIO16 write slow as it's RMW
              }
              else {
                SetGPIO(1UL << pin);
              }
            }
            nextEventCcy = wave.nextOffCcy;
          }
        }
      }

      now = ESP.getCycleCount();
    }
    const int32_t timerMarginCcys = isrTimeoutCcy - nextEventCcy;
    busy = waveformsEnabled && timerMarginCcys > 0;
  }

  int32_t nextTimerCcys;
  if (timer1CB) {
    int32_t callbackCcys = microsecondsToClockCycles(timer1CB());
    // Account for unknown duration of timer1CB().
    nextTimerCcys = nextEventCcy - ESP.getCycleCount();
    if (nextTimerCcys > callbackCcys)
      nextTimerCcys = callbackCcys;
  }
  else {
    nextTimerCcys = nextEventCcy - now;
  }

  // Firing timer too soon, the NMI occurs before ISR has returned.
  if (nextTimerCcys <= IRQLATENCY + DELTAIRQ) {
    nextTimerCcys = IRQLATENCY;
  }
  else if (nextTimerCcys >= MAXIRQCCYS) {
    nextTimerCcys = MAXIRQCCYS - DELTAIRQ;
  }
  else {
    nextTimerCcys -= DELTAIRQ;
  }

  // Do it here instead of global function to save time and because we know it's edge-IRQ
  if (clockCyclesPerMicrosecond() == 160) {
    T1L = nextTimerCcys >> 1;
  }
  else {
    T1L = nextTimerCcys;
  }
  TEIE |= TEIE1; // Edge int enable
}

};
