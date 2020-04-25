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
constexpr int32_t MAXIRQCCYS = microsecondsToClockCycles(10000);
// Maximum servicing time for any single IRQ
constexpr uint32_t ISRTIMEOUTCCYS = microsecondsToClockCycles(14);
// The SDK and hardware take some time to actually get to our NMI code, so
// decrement the next IRQ's timer value by a bit so we can actually catch the
// real CPU cycle count we want for the waveforms.
constexpr int32_t DELTAIRQ = clockCyclesPerMicrosecond() == 160 ?
  microsecondsToClockCycles(3) >> 1 : microsecondsToClockCycles(3);
// The latency between in-ISR rearming of the timer and the earliest firing
constexpr int32_t IRQLATENCY = clockCyclesPerMicrosecond() == 160 ?
  microsecondsToClockCycles(3) >> 1 : microsecondsToClockCycles(3);

// Set/clear GPIO 0-15 by bitmask
#define SetGPIO(a) do { GPOS = a; } while (0)
#define ClearGPIO(a) do { GPOC = a; } while (0)

// for INFINITE, the NMI proceeds on the waveform without expiry deadline.
// for EXPIRES, the NMI expires the waveform automatically on the expiry ccy.
// for UPDATEEXPIRY, the NMI recomputes the exact expiry ccy and transitions to EXPIRES.
// for INIT, the NMI initializes nextPeriodCcy, and if expiryCcy != 0 includes UPDATEEXPIRY.
enum class WaveformMode : uint8_t {INFINITE = 0, EXPIRES = 1, UPDATEEXPIRY = 2, INIT = 3};

// Waveform generator can create tones, PWM, and servos
typedef struct {
  uint32_t nextEventCcy; // ESP clock cycle when switching wave cycle, or expiring wave.
  uint32_t nextPeriodCcy; // ESP clock cycle when a period begins. If WaveformMode::INIT, temporarily holds positive phase offset ccy count
  uint32_t endDutyCcy;   // ESP clock cycle when going from duty to off
  uint32_t dutyCcys;     // Set next off cycle at low->high to maintain phase
  uint32_t periodCcys;   // Set next phase cycle at low->high to maintain phase
  uint32_t expiryCcy;    // For time-limited waveform, the CPU clock cycle when this waveform must stop. If WaveformMode::UPDATE, temporarily holds relative ccy count
  WaveformMode mode;
  int8_t alignPhase;     // < 0 no phase alignment, otherwise starts waveform in relative phase offset to given pin
  bool autoPwm;          // perform PWM duty to idle cycle ratio correction under high load at the expense of precise timings
} Waveform;

namespace {

  static struct {
    Waveform pins[17];             // State of all possible pins
    uint32_t states = 0;           // Is the pin high or low, updated in NMI so no access outside the NMI code
    volatile uint32_t enabled = 0; // Is it actively running, updated in NMI so no access outside the NMI code

    // Enable lock-free by only allowing updates to waveform.states and waveform.enabled from IRQ service routine
    volatile uint32_t toEnable = 0;  // Message to the NMI handler to start exactly one waveform on a inactive pin
    volatile uint32_t toDisable = 0; // Message to the NMI handler to disable exactly one pin from waveform generation

    uint32_t(*timer1CB)() = nullptr;

    bool timer1Running = false;
  } waveform;

}

// Interrupt on/off control
static ICACHE_RAM_ATTR void timer1Interrupt();

// Non-speed critical bits
#pragma GCC optimize ("Os")

static void initTimer() {
  timer1_disable();
  ETS_FRC_TIMER1_INTR_ATTACH(NULL, NULL);
  ETS_FRC_TIMER1_NMI_INTR_ATTACH(timer1Interrupt);
  timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
  waveform.timer1Running = true;
}

static void ICACHE_RAM_ATTR deinitTimer() {
  ETS_FRC_TIMER1_NMI_INTR_ATTACH(NULL);
  timer1_disable();
  timer1_isr_init();
  waveform.timer1Running = false;
}

// Set a callback.  Pass in NULL to stop it
void setTimer1Callback(uint32_t (*fn)()) {
  waveform.timer1CB = fn;
  std::atomic_thread_fence(std::memory_order_release);
  if (!waveform.timer1Running && fn) {
    initTimer();
    timer1_write(microsecondsToClockCycles(1)); // Cause an interrupt post-haste
  } else if (waveform.timer1Running && !fn && !waveform.enabled) {
    deinitTimer();
  }
}

int startWaveform(uint8_t pin, uint32_t highUS, uint32_t lowUS,
  uint32_t runTimeUS, int8_t alignPhase, uint32_t phaseOffsetUS, bool autoPwm) {
  return startWaveformClockCycles(pin,
    microsecondsToClockCycles(highUS), microsecondsToClockCycles(lowUS),
    microsecondsToClockCycles(runTimeUS), alignPhase, microsecondsToClockCycles(phaseOffsetUS), autoPwm);
}

// Start up a waveform on a pin, or change the current one.  Will change to the new
// waveform smoothly on next low->high transition.  For immediate change, stopWaveform()
// first, then it will immediately begin.
int startWaveformClockCycles(uint8_t pin, uint32_t highCcys, uint32_t lowCcys,
  uint32_t runTimeCcys, int8_t alignPhase, uint32_t phaseOffsetCcys, bool autoPwm) {
  uint32_t periodCcys = highCcys + lowCcys;
  if (periodCcys < MAXIRQCCYS) {
    if (!highCcys) {
      periodCcys = (MAXIRQCCYS / periodCcys) * periodCcys;
    }
    else if (!lowCcys) {
      highCcys = periodCcys = (MAXIRQCCYS / periodCcys) * periodCcys;
    }
  }
  // sanity checks, including mixed signed/unsigned arithmetic safety
  if ((pin > 16) || isFlashInterfacePin(pin) || (alignPhase > 16) ||
    static_cast<int32_t>(periodCcys) <= 0 || highCcys > periodCcys) {
    return false;
  }
  Waveform& wave = waveform.pins[pin];
  wave.dutyCcys = highCcys;
  wave.periodCcys = periodCcys;
  wave.autoPwm = autoPwm;

  if (!(waveform.enabled & (1UL << pin))) {
    // wave.nextPeriodCcy and wave.endDutyCcy are initialized by the ISR
    wave.nextPeriodCcy = phaseOffsetCcys;
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
    waveform.toEnable = 1UL << pin;
    if (!waveform.timer1Running) {
      initTimer();
      timer1_write(microsecondsToClockCycles(1));
    }
    else if (T1L > IRQLATENCY + DELTAIRQ) {
      // Must not interfere if Timer is due shortly, cluster phases to reduce interrupt load
      timer1_write(microsecondsToClockCycles(1));
    }
    while (waveform.toEnable) {
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
  if (!waveform.timer1Running) {
    return false;
  }
  // If user sends in a pin >16 but <32, this will always point to a 0 bit
  // If they send >=32, then the shift will result in 0 and it will also return false
  if (waveform.enabled & (1UL << pin)) {
    waveform.toDisable = 1UL << pin;
    // Must not interfere if Timer is due shortly
    if (T1L > IRQLATENCY + DELTAIRQ) {
      timer1_write(microsecondsToClockCycles(1));
    }
    while (waveform.toDisable) {
      /* no-op */ // Can't delay() since stopWaveform may be called from an IRQ
    }
  }
  if (!waveform.enabled && !waveform.timer1CB) {
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
  static int nextPin = 0;

  const uint32_t isrStartCcy = ESP.getCycleCount();

  if (waveform.toEnable || waveform.toDisable) {
    // Handle enable/disable requests from main app.
    waveform.enabled = (waveform.enabled & ~waveform.toDisable) | waveform.toEnable; // Set the requested waveforms on/off
    waveform.toEnable = 0;
    waveform.toDisable = 0;
    // Find the first GPIO being generated by checking GCC's find-first-set (returns 1 + the bit of the first 1 in an int32_t)
    startPin = __builtin_ffs(waveform.enabled) - 1;
    // Find the last bit by subtracting off GCC's count-leading-zeros (no offset in this one)
    endPin = 32 - __builtin_clz(waveform.enabled);
  }

  // Exit the loop if the next event, if any, is sufficiently distant.
  const uint32_t isrTimeoutCcy = isrStartCcy + ISRTIMEOUTCCYS;
  uint32_t now = ESP.getCycleCount();
  uint32_t nextTimerCcy;
  bool busy = waveform.enabled;
  if (!busy) {
    nextTimerCcy = now + MAXIRQCCYS;
  }
  else if (!(waveform.enabled & (1UL << nextPin))) {
    nextPin = startPin;
  }
  bool initPins = true;
  while (busy) {
    nextTimerCcy = now + MAXIRQCCYS;
    int stopPin = nextPin;
    int pin = nextPin;
    do {
      // If it's not on, ignore
      if (!(waveform.enabled & (1UL << pin)))
        continue;

      Waveform& wave = waveform.pins[pin];

      if (initPins) {
        switch (wave.mode) {
        case WaveformMode::INIT:
          waveform.states &= ~(1UL << pin); // Clear the state of any just started
          wave.nextPeriodCcy = (waveform.enabled & (1UL << wave.alignPhase)) ?
            waveform.pins[wave.alignPhase].nextPeriodCcy + wave.nextPeriodCcy : now;
          wave.nextEventCcy = wave.nextPeriodCcy;
          if (!wave.expiryCcy) {
            wave.mode = WaveformMode::INFINITE;
            break;
          }
          // fall through
        case WaveformMode::UPDATEEXPIRY:
          wave.expiryCcy += wave.nextPeriodCcy; // in WaveformMode::UPDATEEXPIRY, expiryCcy temporarily holds relative CPU cycle count
          wave.mode = WaveformMode::EXPIRES;
          initPins = false; // only one pin per IRQ
          break;
        default:
          break;
        }
      }

      int32_t overshootCcys = now - wave.nextEventCcy;
      if (overshootCcys >= 0) {
        if (!wave.autoPwm) {
          // for best effort hard timings
          overshootCcys = 0;
        }
        if (WaveformMode::EXPIRES == wave.mode && wave.nextEventCcy == wave.expiryCcy) {
          // Disable any waveforms that are done
          waveform.enabled ^= 1UL << pin;
        }
        else {
          // get true accumulated overshoot
          overshootCcys = now - ((waveform.states & (1UL << pin)) ? wave.endDutyCcy : wave.nextPeriodCcy);
          const uint32_t idleCcys = wave.periodCcys - wave.dutyCcys;
          uint32_t fwdPeriods = static_cast<uint32_t>(overshootCcys) >= idleCcys ?
            ((overshootCcys + wave.dutyCcys) / wave.periodCcys) : 0;
          uint32_t nextEdgeCcy;
          if (waveform.states & (1UL << pin)) {
          	// up to and including this period 100% duty
            const bool endOfPeriod = wave.nextPeriodCcy == wave.endDutyCcy;
          	// active configuration and forward 100% duty
            if (!idleCcys) {
              wave.nextPeriodCcy += (fwdPeriods + 1) * wave.periodCcys;
              wave.endDutyCcy = wave.nextPeriodCcy;
              nextEdgeCcy = wave.nextPeriodCcy;
            }
            else if (endOfPeriod) {
              // preceeding period had zero idle cycle, continue direct into new duty cycle
              wave.nextPeriodCcy += fwdPeriods * wave.periodCcys;
              wave.endDutyCcy = wave.nextPeriodCcy + wave.dutyCcys;
              wave.nextPeriodCcy += wave.periodCcys;
              nextEdgeCcy = wave.endDutyCcy;
            }
            else if (fwdPeriods) {
              // maintain phase, maintain duty/idle ratio, temporarily reduce frequency by skipPeriods
              fwdPeriods = (overshootCcys + wave.periodCcys) / wave.dutyCcys;
              wave.endDutyCcy += fwdPeriods * wave.dutyCcys;
              wave.nextPeriodCcy += fwdPeriods * wave.periodCcys;
              nextEdgeCcy = wave.endDutyCcy;
            }
            else {
              waveform.states ^= 1UL << pin;
              nextEdgeCcy = wave.nextPeriodCcy + overshootCcys;
              if (pin == 16) {
                GP16O &= ~1; // GPIO16 write slow as it's RMW
              }
              else {
                ClearGPIO(1UL << pin);
              }
            }
          }
          else {
            if (!wave.dutyCcys) {
              wave.nextPeriodCcy += (fwdPeriods + 1) * wave.periodCcys;
              wave.endDutyCcy = wave.nextPeriodCcy;
            }
            else {
              waveform.states ^= 1UL << pin;
              if (fwdPeriods)
              {
                // maintain phase, maintain duty/idle ratio, temporarily reduce frequency by skipPeriods
                wave.endDutyCcy =
                  wave.nextPeriodCcy + (fwdPeriods + 1) * wave.dutyCcys +
                  (overshootCcys + wave.dutyCcys - fwdPeriods * wave.periodCcys);
                wave.nextPeriodCcy += (fwdPeriods + 1) * wave.periodCcys;
              }
              else
              {
                wave.endDutyCcy = wave.nextPeriodCcy + wave.dutyCcys + overshootCcys;
                wave.nextPeriodCcy += wave.periodCcys;
              }
              if (pin == 16) {
                GP16O |= 1; // GPIO16 write slow as it's RMW
              }
              else {
                SetGPIO(1UL << pin);
              }
            }
            nextEdgeCcy = wave.endDutyCcy;
          }

          wave.nextEventCcy =
            (WaveformMode::EXPIRES == wave.mode && static_cast<int32_t>(nextEdgeCcy - wave.expiryCcy) > 0) ?
            wave.expiryCcy : nextEdgeCcy;
        }
      }

      if (static_cast<int32_t>(nextTimerCcy - wave.nextEventCcy) > 0) {
        nextTimerCcy = wave.nextEventCcy;
        nextPin = pin;
      }

    } while ((pin = (pin < endPin) ? pin + 1 : startPin, pin != stopPin));

    initPins = false;
    now = ESP.getCycleCount();
    const int32_t timerMarginCcys = isrTimeoutCcy - nextTimerCcy;
    busy = timerMarginCcys > 0;
    if (busy) {
      while (static_cast<int32_t>(nextTimerCcy - now) > 0) {
        now = ESP.getCycleCount();
      }
    }
  }

  int32_t nextTimerCcys;
  if (waveform.timer1CB) {
    int32_t callbackCcys = microsecondsToClockCycles(waveform.timer1CB());
    // Account for unknown duration of timer1CB().
    nextTimerCcys = nextTimerCcy - ESP.getCycleCount();
    if (nextTimerCcys > callbackCcys) {
      nextTimerCcys = callbackCcys;
    }
  }
  else {
    nextTimerCcys = nextTimerCcy - now;
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
