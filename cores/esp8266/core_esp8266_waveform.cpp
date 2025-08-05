/*
  esp8266_waveform - General purpose waveform generation and control,
                     supporting outputs on all pins in parallel.

  Copyright (c) 2018 Earle F. Philhower, III.  All rights reserved.
  Copyright (c) 2020 Dirk O. Kaar.

  The core idea is to have a programmable waveform generator with a unique
  high and low period (defined in microseconds or CPU clock cycles).  TIMER1
  is set to 1-shot mode and is always loaded with the time until the next
  edge of any live waveforms.

  Up to one waveform generator per pin supported.

  Each waveform generator is synchronized to the ESP clock cycle counter, not
  the timer.  This allows for removing interrupt jitter and delay as the
  counter always increments once per 80MHz clock.  Changes to a waveform are
  contiguous and only take effect on the next waveform transition,
  allowing for smooth transitions.

  This replaces older tone(), analogWrite(), and the Servo classes.

  Everywhere in the code where "ccy" or "ccys" is used, it means ESP.getCycleCount()
  clock cycle count, or an interval measured in CPU clock cycles, but not
  TIMER1 cycles (which may be 2 CPU clock cycles @ 160MHz).

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

// Timer is 80MHz fixed. 160MHz CPU frequency need scaling.
constexpr bool ISCPUFREQ160MHZ = clockCyclesPerMicrosecond() == 160;
// Maximum delay between IRQs, Timer1, <= 2^23 / 80MHz
constexpr int32_t MAXIRQTICKSCCYS = microsecondsToClockCycles(10000);
// Maximum servicing time for any single IRQ
constexpr uint32_t ISRTIMEOUTCCYS = microsecondsToClockCycles(18);
// The latency between in-ISR rearming of the timer and the earliest firing
constexpr int32_t IRQLATENCYCCYS = microsecondsToClockCycles(2);
// The SDK and hardware take some time to actually get to our NMI code
constexpr int32_t DELTAIRQCCYS = ISCPUFREQ160MHZ ?
  microsecondsToClockCycles(2) >> 1 : microsecondsToClockCycles(2);

// for INFINITE, the NMI proceeds on the waveform without expiry deadline.
// for EXPIRES, the NMI expires the waveform automatically on the expiry ccy.
// for UPDATEEXPIRY, the NMI recomputes the exact expiry ccy and transitions to EXPIRES.
// for INIT, the NMI initializes nextPeriodCcy, and if expiryCcy != 0 includes UPDATEEXPIRY.
enum class WaveformMode : uint8_t {INFINITE = 0, EXPIRES = 1, UPDATEEXPIRY = 2, INIT = 3};

// Waveform generator can create tones, PWM, and servos
struct Waveform {
  uint32_t nextPeriodCcy; // ESP clock cycle when a period begins. If WaveformMode::INIT, temporarily holds positive phase offset ccy count
  uint32_t endDutyCcy;    // ESP clock cycle when going from duty to off
  int32_t dutyCcys;       // Set next off cycle at low->high to maintain phase
  int32_t adjDutyCcys;    // Temporary correction for next period
  int32_t periodCcys;     // Set next phase cycle at low->high to maintain phase
  uint32_t expiryCcy;     // For time-limited waveform, the CPU clock cycle when this waveform must stop. If WaveformMode::UPDATE, temporarily holds relative ccy count
  WaveformMode mode;
  int8_t alignPhase;      // < 0 no phase alignment, otherwise starts waveform in relative phase offset to given pin
  bool autoPwm;           // perform PWM duty to idle cycle ratio correction under high load at the expense of precise timings
};

namespace {

  static struct {
    Waveform pins[17];         // State of all possible pins
    uint32_t states = 0;       // Is the pin high or low, updated in NMI so no access outside the NMI code
    uint32_t enabled = 0;      // Is it actively running, updated in NMI so no access outside the NMI code

    // Enable lock-free by only allowing updates to waveform.states and waveform.enabled from IRQ service routine
    int32_t toSetBits = 0;     // Message to the NMI handler to start/modify exactly one waveform
    int32_t toDisableBits = 0; // Message to the NMI handler to disable exactly one pin from waveform generation

    uint32_t(*timer1CB)() = nullptr;

    bool timer1Running = false;

    uint32_t nextEventCcy;
  } waveform;

}

// Interrupt on/off control
static IRAM_ATTR void timer1Interrupt();

// Non-speed critical bits
#pragma GCC optimize ("Os")

// PWM implementation using special purpose state machine
//
// Keep an ordered list of pins with the delta in cycles between each
// element, with a terminal entry making up the remainder of the PWM
// period.  With this method sum(all deltas) == PWM period clock cycles.
//
// At t=0 set all pins high and set the timeout for the 1st edge.
// On interrupt, if we're at the last element reset to t=0 state
// Otherwise, clear that pin down and set delay for next element
// and so forth.

// The SDK sometimes is running at a different speed the the Arduino core
// so the ESP cycle counter is actually running at a variable speed.
// adjust(x) takes care of adjusting a delta clock cycle amount accordingly.
#if F_CPU == 80000000
// shl if the core is at 160 MHz
#define adjust(x) ((x) << ( (CPU2X & 1) ? 1 : 0))
#else
#define adjust(x) ((x) >> 0)
#endif

constexpr int maxPWMs = 8;

// PWM machine state
struct PWMState {
    uint32_t mask; // Bitmask of active pins
    uint32_t cnt;  // How many entries
    uint32_t idx;  // Where the state machine is along the list
    uint8_t  pin[maxPWMs + 1];
    uint32_t delta[maxPWMs + 1];
    uint32_t nextServiceCycle;  // Clock cycle for next step
    struct PWMState* pwmUpdate; // Set by main code, cleared by ISR
};

static PWMState pwmState;
static uint32_t _pwmFreq = 1000;
static uint32_t _pwmPeriod = microsecondsToClockCycles(1000000UL) / _pwmFreq;

static __attribute__((noinline)) void initTimer() {
  timer1_disable();
  ETS_FRC_TIMER1_INTR_ATTACH(NULL, NULL);
  ETS_FRC_TIMER1_NMI_INTR_ATTACH(timer1Interrupt);
  timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
  waveform.timer1Running = true;
  waveform.nextEventCcy = ESP.getCycleCount() + IRQLATENCYCCYS;
  timer1_write(IRQLATENCYCCYS); // Cause an interrupt post-haste
}

static IRAM_ATTR void deinitIdleTimer() {
  if (!waveform.timer1Running || waveform.enabled || waveform.timer1CB || pwmState.cnt) {
    return;
  }
  ETS_FRC_TIMER1_NMI_INTR_ATTACH(NULL);
  timer1_disable();
  timer1_isr_init();
  waveform.timer1Running = false;
}

static IRAM_ATTR void forceTimerTrigger() {
  // Must not interfere if Timer is due shortly
  if (T1V > IRQLATENCYCCYS) {
    waveform.nextEventCcy = ESP.getCycleCount() + IRQLATENCYCCYS;
    timer1_write(IRQLATENCYCCYS);
  }
}

extern "C" {

// Notify the NMI that a new PWM state is available through the mailbox.
// Wait for mailbox to be emptied (either busy or delay() as needed)
static IRAM_ATTR void _notifyPWM(PWMState *p, bool idle) {
  p->pwmUpdate = nullptr;
  std::atomic_thread_fence(std::memory_order_release);
  pwmState.pwmUpdate = p;
  std::atomic_thread_fence(std::memory_order_acq_rel);
  if (idle) {
    forceTimerTrigger();
  }
  while (pwmState.pwmUpdate) {
    if (idle) {
      esp_yield();
    }
    std::atomic_thread_fence(std::memory_order_acquire);
  }
}

static void _addPWMtoList(PWMState& p, uint8_t pin, uint32_t val, uint32_t range) {
    // Stash the val and range so we can re-evaluate the fraction
    // should the user change PWM frequency.  This allows us to
    // give as great a precision as possible.  We know by construction
    // that the waveform for this pin will be inactive so we can borrow
    // memory from that structure.
    waveform.pins[pin].dutyCcys = val; // Numerator == high
    waveform.pins[pin].periodCcys = range; // Denominator == low

    uint32_t cc = (_pwmPeriod * val) / range;

    // Clip to sane values in the case we go from OK to not-OK when adjusting frequencies
    if (cc == 0) {
        cc = 1;
    }
    else if (cc >= _pwmPeriod) {
        cc = _pwmPeriod - 1;
    }

    if (p.cnt == 0) {
        // Starting up from scratch, special case 1st element and PWM period
        p.pin[0] = pin;
        p.delta[0] = cc;
        // Final pin is never used: p.pin[1] = 0xff;
        p.delta[1] = _pwmPeriod - cc;
    }
    else {
        uint32_t ttl = 0;
        uint32_t i;
        // Skip along until we're at the spot to insert
        for (i = 0; (i <= p.cnt) && (ttl + p.delta[i] < cc); i++) {
            ttl += p.delta[i];
        }
        // Shift everything out by one to make space for new edge
        for (int32_t j = p.cnt; j >= (int)i; j--) {
            p.pin[j + 1] = p.pin[j];
            p.delta[j + 1] = p.delta[j];
        }
        int off = cc - ttl; // The delta from the last edge to the one we're inserting
        p.pin[i] = pin;
        p.delta[i] = off; // Add the delta to this new pin
        p.delta[i + 1] -= off; // And subtract it from the follower to keep sum(deltas) constant
    }
    p.cnt++;
    p.mask |= 1 << pin;
}

// Called when analogWriteFreq() changed to update the PWM total period
void _setPWMFreq(uint32_t freq) {
  _pwmFreq = freq;

  // Convert frequency into clock cycles
  uint32_t cc = microsecondsToClockCycles(1000000UL) / freq;

  // Simple static adjustment to bring period closer to requested due to overhead
  // Empirically determined as a constant PWM delay and a function of the number of PWMs
#if F_CPU == 80000000
  cc -= ((microsecondsToClockCycles(pwmState.cnt) * 13) >> 4) + 110;
#else
  cc -= ((microsecondsToClockCycles(pwmState.cnt) * 10) >> 4) + 75;
#endif

  if (cc == _pwmPeriod) {
    return; // No change
  }

  _pwmPeriod = cc;

  if (pwmState.cnt) {
    PWMState p;  // The working copy since we can't edit the one in use
    p.mask = 0;
    p.cnt = 0;
    for (uint32_t i = 0; i < pwmState.cnt; i++) {
      auto pin = pwmState.pin[i];
      _addPWMtoList(p, pin, waveform.pins[pin].dutyCcys, waveform.pins[pin].periodCcys);
    }
    // Update and wait for mailbox to be emptied
    if (!waveform.timer1Running) {
      initTimer();
    }
    _notifyPWM(&p, true);
    deinitIdleTimer();
  }
}

// Helper routine to remove an entry from the state machine
// and clean up any marked-off entries
static void _cleanAndRemovePWM(PWMState *p, uint8_t pin) {
  uint32_t leftover = 0;
  uint32_t in, out;
  for (in = 0, out = 0; in < p->cnt; in++) {
    if ((p->pin[in] != pin) && (p->mask & (1 << p->pin[in]))) {
        p->pin[out] = p->pin[in];
        p->delta[out] = p->delta[in] + leftover;
        leftover = 0;
        out++;
    } else {
        leftover += p->delta[in];
        p->mask &= ~(1 << p->pin[in]);
    }
  }
  p->cnt = out;
  // Final pin is never used: p->pin[out] = 0xff;
  p->delta[out] = p->delta[in] + leftover;
}


// Disable PWM on a specific pin (i.e. when a digitalWrite or analogWrite(0%/100%))
IRAM_ATTR bool _stopPWM(uint8_t pin) {
  if (!((1 << pin) & pwmState.mask)) {
    return false; // Pin not actually active
  }

  PWMState p = pwmState; // The working copy since we can't edit the one in use

  // In _stopPWM we just clear the mask but keep everything else
  // untouched to save IRAM.  The main startPWM will handle cleanup.
  p.mask &= ~(1 << pin);
  if (!p.mask) {
    // If all have been stopped, then turn PWM off completely
    p.cnt = 0;
  }

  // Update and wait for mailbox to be emptied, no delay (could be in ISR)
  _notifyPWM(&p, false);
  // Possibly shut down the timer completely if we're done
  deinitIdleTimer();
  return true;
}

// Called by analogWrite(1...99%) to set the PWM duty in clock cycles
bool _setPWM(uint8_t pin, uint32_t val, uint32_t range) {
  const uint32_t pinBit = 1UL << pin;
  if (waveform.enabled & pinBit) {
    stopWaveform(pin);
  }
  PWMState p;  // Working copy
  p = pwmState;
  // Get rid of any entries for this pin
  _cleanAndRemovePWM(&p, pin);
  // And add it to the list, in order
  if (p.cnt >= maxPWMs) {
    return false; // No space left
  }

  // Sanity check for all-on/off
  uint32_t cc = (_pwmPeriod * val) / range;
  if ((cc == 0) || (cc >= _pwmPeriod)) {
    digitalWrite(pin, cc ? HIGH : LOW);
    return true;
  }

  _addPWMtoList(p, pin, val, range);

  // Set mailbox and wait for ISR to copy it over
  if (!waveform.timer1Running) {
    initTimer();
  }
  _notifyPWM(&p, true);
  deinitIdleTimer();

  // Potentially recalculate the PWM period if we've added another pin
  _setPWMFreq(_pwmFreq);

  return true;
}

// Set a callback. Pass in NULL to stop it
void setTimer1Callback(uint32_t (*fn)()) {
  waveform.timer1CB = fn;
  std::atomic_thread_fence(std::memory_order_acq_rel);
  if (!waveform.timer1Running && waveform.timer1CB) {
    initTimer();
  } else {
    deinitIdleTimer();
  }
}

// Start up a waveform on a pin, or change the current one.  Will change to the new
// waveform smoothly on next low->high transition.  For immediate change, stopWaveform()
// first, then it will immediately begin.
int startWaveformClockCycles(uint8_t pin, uint32_t highCcys, uint32_t lowCcys,
  uint32_t runTimeCcys, int8_t alignPhase, uint32_t phaseOffsetCcys, bool autoPwm) {
  uint32_t periodCcys = highCcys + lowCcys;
  if (periodCcys < MAXIRQTICKSCCYS) {
    if (!highCcys) {
      periodCcys = (MAXIRQTICKSCCYS / periodCcys) * periodCcys;
    }
    else if (!lowCcys) {
      highCcys = periodCcys = (MAXIRQTICKSCCYS / periodCcys) * periodCcys;
    }
  }
  // sanity checks, including mixed signed/unsigned arithmetic safety
  if ((pin > 16) || isFlashInterfacePin(pin) || (alignPhase > 16) ||
    static_cast<int32_t>(periodCcys) <= 0 ||
    static_cast<int32_t>(highCcys) < 0 || static_cast<int32_t>(lowCcys) < 0) {
    return false;
  }
  Waveform& wave = waveform.pins[pin];
  wave.dutyCcys = highCcys;
  wave.adjDutyCcys = 0;
  wave.periodCcys = periodCcys;
  wave.autoPwm = autoPwm;

  _stopPWM(pin); // Make sure there's no PWM live here

  std::atomic_thread_fence(std::memory_order_acquire);
  const uint32_t pinBit = 1UL << pin;
  if (!(waveform.enabled & pinBit)) {
    // wave.nextPeriodCcy and wave.endDutyCcy are initialized by the ISR
    wave.nextPeriodCcy = phaseOffsetCcys;
    wave.expiryCcy = runTimeCcys; // in WaveformMode::INIT, temporarily hold relative cycle count
    wave.mode = WaveformMode::INIT;
    wave.alignPhase = (alignPhase < 0) ? -1 : alignPhase;
    if (!wave.dutyCcys) {
      // If initially at zero duty cycle, force GPIO off
      if (pin == 16) {
        GP16O = 0;
      }
      else {
        GPOC = pinBit;
      }
    }
    std::atomic_thread_fence(std::memory_order_release);
    waveform.toSetBits = 1UL << pin;
    std::atomic_thread_fence(std::memory_order_release);
    if (!waveform.timer1Running) {
      initTimer();
    }
    // The ISR pulls updates on the next waveform interval
  }
  else {
    wave.mode = WaveformMode::INFINITE; // turn off possible expiry to make update atomic from NMI
    std::atomic_thread_fence(std::memory_order_release);
    wave.expiryCcy = runTimeCcys; // in WaveformMode::UPDATEEXPIRY, temporarily hold relative cycle count
    if (runTimeCcys) {
      wave.mode = WaveformMode::UPDATEEXPIRY;
      std::atomic_thread_fence(std::memory_order_release);
      waveform.toSetBits = 1UL << pin;
    }
  }
  std::atomic_thread_fence(std::memory_order_acq_rel);
  while (waveform.toSetBits) {
    esp_yield(); // Wait for waveform to update
    std::atomic_thread_fence(std::memory_order_acquire);
  }
  return true;
} 

int startWaveform(uint8_t pin, uint32_t timeHighUS, uint32_t timeLowUS,
  uint32_t runTimeUS, int8_t alignPhase, uint32_t phaseOffsetUS, bool autoPwm) {
  return startWaveformClockCycles(pin,
    microsecondsToClockCycles(timeHighUS), microsecondsToClockCycles(timeLowUS),
    microsecondsToClockCycles(runTimeUS), alignPhase, microsecondsToClockCycles(phaseOffsetUS), autoPwm);
}

// Stops a waveform on a pin
IRAM_ATTR int stopWaveform(uint8_t pin) {
  // Can't possibly need to stop anything if there is no timer active
  if (!waveform.timer1Running) {
    return false;
  }
  
  _stopPWM(pin); // Make sure there's no PWM live here

  // If user sends in a pin >16 but <32, this will always point to a 0 bit
  // If they send >=32, then the shift will result in 0 and it will also return false
  std::atomic_thread_fence(std::memory_order_acquire);
  const uint32_t pinBit = 1UL << pin;
  if (waveform.enabled & pinBit) {
    waveform.toDisableBits = 1UL << pin;
    std::atomic_thread_fence(std::memory_order_release);
    forceTimerTrigger();
    while (waveform.toDisableBits) {
      /* no-op */ // Can't delay() since stopWaveform may be called from an IRQ
      std::atomic_thread_fence(std::memory_order_acquire);
    }
  }
  deinitIdleTimer();
  return true;
}

};

// Speed critical bits
#pragma GCC optimize ("O2")

// For dynamic CPU clock frequency switch in loop the scaling logic would have to be adapted.
// Using constexpr makes sure that the CPU clock frequency is compile-time fixed.
static inline IRAM_ATTR int32_t scaleCcys(const int32_t ccys, const bool isCPU2X) {
  if (ISCPUFREQ160MHZ) {
    return isCPU2X ? ccys : (ccys >> 1);
  }
  else {
    return isCPU2X ? (ccys << 1) : ccys;
  }
}

static IRAM_ATTR void timer1Interrupt() {
  const uint32_t isrStartCcy = ESP.getCycleCount();
  const bool isCPU2X = CPU2X & 1;

  int32_t clockDrift = isrStartCcy - waveform.nextEventCcy;
  if ((waveform.toSetBits && !(waveform.enabled & waveform.toSetBits)) || waveform.toDisableBits) {
    // Handle enable/disable requests from main app.
    waveform.enabled = (waveform.enabled | waveform.toSetBits) & ~waveform.toDisableBits; // Set the requested waveforms on/off
    // Find the first GPIO being generated by checking GCC's find-first-set (returns 1 + the bit of the first 1 in an int32_t)
    waveform.toDisableBits = 0;
  }

  if (waveform.toSetBits) {
    const int toSetPin = __builtin_ffs(waveform.toSetBits) - 1;
    Waveform& wave = waveform.pins[toSetPin];
    switch (wave.mode) {
    case WaveformMode::INIT:
      waveform.states &= ~waveform.toSetBits; // Clear the state of any just started
      if (wave.alignPhase >= 0 && waveform.enabled & (1UL << wave.alignPhase)) {
        wave.nextPeriodCcy = waveform.pins[wave.alignPhase].nextPeriodCcy + wave.nextPeriodCcy;
      }
      else {
        wave.nextPeriodCcy = waveform.nextEventCcy;
      }
      if (!wave.expiryCcy) {
        wave.mode = WaveformMode::INFINITE;
        break;
      }
      // fall through
    case WaveformMode::UPDATEEXPIRY:
      // in WaveformMode::UPDATEEXPIRY, expiryCcy temporarily holds relative CPU cycle count
      wave.expiryCcy = wave.nextPeriodCcy + scaleCcys(wave.expiryCcy, isCPU2X);
      wave.mode = WaveformMode::EXPIRES;
      break;
    default:
      break;
    }
    waveform.toSetBits = 0;
  }

  // Exit the loop if the next event, if any, is sufficiently distant.
  const uint32_t isrTimeoutCcy = isrStartCcy + ISRTIMEOUTCCYS;
  uint32_t busyPins = waveform.enabled;
  waveform.nextEventCcy = isrStartCcy + MAXIRQTICKSCCYS;

  uint32_t now = ESP.getCycleCount();
  uint32_t isrNextEventCcy = now;

  // PWM state machine implementation
  if (!pwmState.cnt && pwmState.pwmUpdate) {
    // Start up the PWM generator by copying from the mailbox
    pwmState.cnt = 1;
    pwmState.idx = 1; // Ensure copy this cycle, cause it to start at t=0
    pwmState.nextServiceCycle = now; // Do it this loop!
    // No need for mem barrier here.  Global must be written by IRQ exit
  }
  bool busyPWM = pwmState.cnt;

  do {
    if (busyPWM) {
      int32_t pwmCyclesToGo;
      do {
        pwmCyclesToGo = pwmState.nextServiceCycle - now;
        if (pwmCyclesToGo <= 0) {
          if (pwmState.idx == pwmState.cnt) { // Start of pulses, possibly copy new
            if (pwmState.pwmUpdate) {
              // Do the memory copy from temp to global and clear mailbox
              pwmState = *pwmState.pwmUpdate;
            }
            GPOS = pwmState.mask; // Set all active pins high
            if (pwmState.mask & (1 << 16)) {
              GP16O = 1;
            }
            pwmState.idx = 0;
          }
          else {
            do {
              // Drop the pin at this edge
              GPOC = 1 << pwmState.pin[pwmState.idx];
              if (pwmState.pin[pwmState.idx] == 16) {
                GP16O = 0;
              }
              pwmState.idx++;
              // Any other pins at this same PWM value will have delta==0, drop them too.
            } while (pwmState.delta[pwmState.idx] == 0);
          }
          // Preserve duty cycle over PWM period by using now+xxx instead of += delta
          pwmCyclesToGo = adjust(pwmState.delta[pwmState.idx]);
          pwmState.nextServiceCycle = now + pwmCyclesToGo;
        }
        now = ESP.getCycleCount();
        busyPWM = static_cast<int32_t>(isrTimeoutCcy - pwmState.nextServiceCycle) > 0;
      // PWM can starve the generic waveform generator if pwmCyclesToGo remains below 100
      } while (busyPWM && pwmCyclesToGo < 100);
    }
  
    if (busyPins) {
      // on first loop iteration per ISR call, isrNextEventCcy == now, this is skipped
      if (static_cast<int32_t>(isrNextEventCcy - now) > IRQLATENCYCCYS + DELTAIRQCCYS) {
        waveform.nextEventCcy = isrNextEventCcy;
        busyPins = 0;
      }
      else {
        isrNextEventCcy = waveform.nextEventCcy;
        uint32_t loopPins = busyPins;
        while (loopPins) {
          const int pin = __builtin_ffsl(loopPins) - 1;
          const uint32_t pinBit = 1UL << pin;
          loopPins ^= pinBit;
    
          Waveform& wave = waveform.pins[pin];
    
          if (clockDrift) {
            wave.endDutyCcy += clockDrift;
            wave.nextPeriodCcy += clockDrift;
            wave.expiryCcy += clockDrift;
          }
    
          uint32_t waveNextEventCcy = (waveform.states & pinBit) ? wave.endDutyCcy : wave.nextPeriodCcy;
          if (WaveformMode::EXPIRES == wave.mode &&
            static_cast<int32_t>(waveNextEventCcy - wave.expiryCcy) >= 0 &&
            static_cast<int32_t>(now - wave.expiryCcy) >= 0) {
            // Disable any waveforms that are done
            waveform.enabled ^= pinBit;
            busyPins ^= pinBit;
          }
          else {
            const int32_t overshootCcys = now - waveNextEventCcy;
            if (overshootCcys >= 0) {
              const int32_t periodCcys = scaleCcys(wave.periodCcys, isCPU2X);
              if (waveform.states & pinBit) {
                // active configuration and forward are 100% duty
                if (wave.periodCcys == wave.dutyCcys) {
                  wave.nextPeriodCcy += periodCcys;
                  wave.endDutyCcy = wave.nextPeriodCcy;
                }
                else {
                  if (wave.autoPwm) {
                    wave.adjDutyCcys += overshootCcys;
                  }
                  waveform.states ^= pinBit;
                  if (16 == pin) {
                    GP16O = 0;
                  }
                  else {
                    GPOC = pinBit;
                  }
                }
                waveNextEventCcy = wave.nextPeriodCcy;
              }
              else {
                wave.nextPeriodCcy += periodCcys;
                if (!wave.dutyCcys) {
                  wave.endDutyCcy = wave.nextPeriodCcy;
                }
                else {
                  int32_t dutyCcys = scaleCcys(wave.dutyCcys, isCPU2X);
                  if (dutyCcys <= wave.adjDutyCcys) {
                    dutyCcys >>= 1;
                    wave.adjDutyCcys -= dutyCcys;
                  }
                  else if (wave.adjDutyCcys) {
                    dutyCcys -= wave.adjDutyCcys;
                    wave.adjDutyCcys = 0;
                  }
                  wave.endDutyCcy = now + dutyCcys;
                  if (static_cast<int32_t>(wave.endDutyCcy - wave.nextPeriodCcy) > 0) {
                    wave.endDutyCcy = wave.nextPeriodCcy;
                  }
                  waveform.states |= pinBit;
                  if (16 == pin) {
                    GP16O = 1;
                  }
                  else {
                    GPOS = pinBit;
                  }
                }
                waveNextEventCcy = wave.endDutyCcy;
              }
    
              if (WaveformMode::EXPIRES == wave.mode && static_cast<int32_t>(waveNextEventCcy - wave.expiryCcy) > 0) {
                waveNextEventCcy = wave.expiryCcy;
              }
            }
    
            if (static_cast<int32_t>(waveNextEventCcy - isrTimeoutCcy) >= 0) {
              busyPins ^= pinBit;
              if (static_cast<int32_t>(waveform.nextEventCcy - waveNextEventCcy) > 0) {
                waveform.nextEventCcy = waveNextEventCcy;
              }
            }
            else if (static_cast<int32_t>(isrNextEventCcy - waveNextEventCcy) > 0) {
              isrNextEventCcy = waveNextEventCcy;
            }
          }
          now = ESP.getCycleCount();
        }
        clockDrift = 0;
      }
    }
  } while (busyPWM || busyPins);

  int32_t callbackCcys = 0;
  if (waveform.timer1CB) {
    callbackCcys = scaleCcys(waveform.timer1CB(), isCPU2X);
  }
  now = ESP.getCycleCount();
  int32_t nextEventCcys = waveform.nextEventCcy - now;
  // Account for unknown duration of timer1CB().
  if (waveform.timer1CB && nextEventCcys > callbackCcys) {
    waveform.nextEventCcy = now + callbackCcys;
    nextEventCcys = callbackCcys;
  }
  if (pwmState.cnt) {
    const int32_t nextPwmServiceCcys = pwmState.nextServiceCycle - now;
    if (nextEventCcys > nextPwmServiceCcys)
    {
      waveform.nextEventCcy = pwmState.nextServiceCycle;
      nextEventCcys = nextPwmServiceCcys;
    }
  }

  // Timer is 80MHz fixed. 160MHz CPU frequency need scaling.
  int32_t deltaIrqCcys = DELTAIRQCCYS;
  int32_t irqLatencyCcys = IRQLATENCYCCYS;
  if (isCPU2X) {
    nextEventCcys >>= 1;
    deltaIrqCcys >>= 1;
    irqLatencyCcys >>= 1;
  }

  // Firing timer too soon, the NMI occurs before ISR has returned.
  if (nextEventCcys < irqLatencyCcys + deltaIrqCcys) {
    waveform.nextEventCcy = now + IRQLATENCYCCYS + DELTAIRQCCYS;
    nextEventCcys = irqLatencyCcys;
  }
  else {
    nextEventCcys -= deltaIrqCcys;
  }

  // Register access is fast and edge IRQ was configured before.
  T1L = nextEventCcys;
}
