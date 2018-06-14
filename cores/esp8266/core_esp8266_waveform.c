/*
  esp8266_waveform - General purpose waveform generation and control,
                     supporting outputs on all pins in parallel.

  Copyright (c) 2018 Earle F. Philhower, III.  All rights reserved.

  The core idea is to have a programmable waveform generator with a unique
  high and low period (defined in microseconds).  TIMER1 is set to 1-shot
  mode and is always loaded with the time until the next edge of any live
  waveforms.

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
#include "core_esp8266_waveform.h"

// Need speed, not size, here
#pragma GCC optimize ("O3")

// Map the IRQ stuff to standard terminology
#define cli() ets_intr_lock()
#define sei() ets_intr_unlock()

// Maximum delay between IRQs
#define MAXIRQUS (10000)

// If the cycles from now to an event are below this value, perform it anyway since IRQs take longer than this
#define CYCLES_FLUFF (100)

// Macro to get count of predefined array elements
#define countof(a) ((size_t)(sizeof(a)/sizeof(a[0])))

// Set/clear *any* GPIO
#define SetGPIOPin(a) do { if (a < 16) { GPOS |= (1<<a); } else { GP16O |= 1; } } while (0)
#define ClearGPIOPin(a) do { if (a < 16) { GPOC |= (1<<a); } else { GP16O &= ~1; } } while (0)
// Set/clear GPIO 0-15
#define SetGPIO(a) do { GPOS = a; } while (0)
#define ClearGPIO(a) do { GPOC = a; } while (0)

// Waveform generator can create tones, PWM, and servos
typedef struct {
  uint32_t nextServiceCycle;        // ESP cycle timer when a transition required
  uint32_t timeLeftCycles;          // For time-limited waveform, how many ESP cycles left
  uint16_t gpioMask;                // Mask instead of value to speed IRQ loop
  uint16_t gpio16Mask;              // Mask instead of value to speed IRQ loop
  unsigned state              : 1;  // Current state of this pin
  unsigned nextTimeHighCycles : 31; // Copy over low->high to keep smooth waveform
  unsigned enabled            : 1;  // Is this GPIO generating a waveform?
  unsigned nextTimeLowCycles  : 31; // Copy over high->low to keep smooth waveform
} Waveform;

// These can be accessed in interrupts, so ensure to bracket access with SEI/CLI
static Waveform waveform[] = {
  {0, 0, 1<<0, 0, 0, 0, 0, 0}, // GPIO0
  {0, 0, 1<<1, 0, 0, 0, 0, 0}, // GPIO1
  {0, 0, 1<<2, 0, 0, 0, 0, 0},
  {0, 0, 1<<3, 0, 0, 0, 0, 0},
  {0, 0, 1<<4, 0, 0, 0, 0, 0},
  {0, 0, 1<<5, 0, 0, 0, 0, 0},
  // GPIOS 6-11 not allowed, used for flash
  {0, 0, 1<<12, 0, 0, 0, 0, 0},
  {0, 0, 1<<13, 0, 0, 0, 0, 0},
  {0, 0, 1<<14, 0, 0, 0, 0, 0},
  {0, 0, 1<<15, 0, 0, 0, 0, 0},
  {0, 0, 0, 1, 0, 0, 0, 0}  // GPIO16
};

static uint32_t (*timer1CB)() = NULL;;


// Helper functions
static inline ICACHE_RAM_ATTR uint32_t MicrosecondsToCycles(uint32_t microseconds) {
  return clockCyclesPerMicrosecond() * microseconds;
}

static inline ICACHE_RAM_ATTR uint32_t min_u32(uint32_t a, uint32_t b) {
  if (a < b) {
    return a;
  }
  return b;
}

static inline ICACHE_RAM_ATTR uint32_t min_s32(int32_t a, int32_t b) {
  if (a < b) {
    return a;
  }
  return b;
}

static inline ICACHE_RAM_ATTR void ReloadTimer(uint32_t a) {
  // Below a threshold you actually miss the edge IRQ, so ensure enough time
  if (a > 32) {
    timer1_write(a);
  } else {
    timer1_write(32);
  }
}

static inline ICACHE_RAM_ATTR uint32_t GetCycleCount() {
  uint32_t ccount;
  __asm__ __volatile__("esync; rsr %0,ccount":"=a"(ccount));
  return ccount;
}

// Interrupt on/off control
static ICACHE_RAM_ATTR void timer1Interrupt();
static uint8_t timerRunning = false;
static uint32_t lastCycleCount = 0; // Last ESP cycle counter on running the interrupt routine

static void initTimer() {
  timer1_disable();
  timer1_isr_init();
  timer1_attachInterrupt(timer1Interrupt);
  lastCycleCount = GetCycleCount();
  timer1_enable(TIM_DIV1, TIM_EDGE, TIM_SINGLE);
  timerRunning = true;
}

static void deinitTimer() {
  timer1_attachInterrupt(NULL);
  timer1_disable();
  timer1_isr_init();
  timerRunning = false;
}

// Set a callback.  Pass in NULL to stop it
void setTimer1Callback(uint32_t (*fn)()) {
  timer1CB = fn;
  if (!timerRunning && fn) {
    initTimer();
  } else if (timerRunning && !fn) {
    int cnt = 0;
    for (size_t i = 0; i < countof(waveform); i++) {
      cnt += waveform[i].enabled ? 1 : 0;
    }
    if (!cnt) {
      deinitTimer();
    }
  }
  ReloadTimer(MicrosecondsToCycles(1)); // Cause an interrupt post-haste
}

// Start up a waveform on a pin, or change the current one.  Will change to the new
// waveform smoothly on next low->high transition.  For immediate change, stopWaveform()
// first, then it will immediately begin.
int startWaveform(uint8_t pin, uint32_t timeHighUS, uint32_t timeLowUS, uint32_t runTimeUS) {
  Waveform *wave = NULL;
  for (size_t i = 0; i < countof(waveform); i++) {
    if (((pin == 16) && waveform[i].gpio16Mask==1) || ((pin != 16) && (waveform[i].gpioMask == 1<<pin))) {
      wave = (Waveform*) & (waveform[i]);
      break;
    }
  }
  if (!wave) {
    return false;
  }
  wave->nextTimeHighCycles = MicrosecondsToCycles(timeHighUS) - 70;  // Take out some time for IRQ codepath
  wave->nextTimeLowCycles = MicrosecondsToCycles(timeLowUS) - 70;  // Take out some time for IRQ codepath
  wave->timeLeftCycles = MicrosecondsToCycles(runTimeUS);
  if (!wave->enabled) {
    wave->state = 0;
    // Actually set the pin high or low in the IRQ service to guarantee times
    wave->nextServiceCycle = GetCycleCount() + MicrosecondsToCycles(1);
    wave->enabled = 1;
    if (!timerRunning) {
      initTimer();
    }
    ReloadTimer(MicrosecondsToCycles(1)); // Cause an interrupt post-haste
  }
  return true;
}

// Stops a waveform on a pin
int stopWaveform(uint8_t pin) {
  for (size_t i = 0; i < countof(waveform); i++) {
    if (((pin == 16) && waveform[i].gpio16Mask) || ((pin != 16) && (waveform[i].gpioMask == 1<<pin))) {
      waveform[i].enabled = 0;
      int cnt = timer1CB?1:0;
      for (size_t i = 0; i < countof(waveform); i++) {
        cnt += waveform[i].enabled ? 1 : 0;
      }
      if (!cnt) {
        deinitTimer();
      }
      return true;
    }
  }
  cli();
  return false;
}

static ICACHE_RAM_ATTR void timer1Interrupt() {
  uint32_t nextEventCycles;
  #if F_CPU == 160000000
  uint8_t cnt = 20;
  #else
  uint8_t cnt = 10;
  #endif

  do {
    nextEventCycles = MicrosecondsToCycles(MAXIRQUS);
    for (size_t i = 0; i < countof(waveform); i++) {
      Waveform *wave = &waveform[i];
      uint32_t now;

      // If it's not on, ignore!
      if (!wave->enabled) {
        continue;
      }

      // Check for toggles
      now = GetCycleCount();
      if (now >= wave->nextServiceCycle) {
        wave->state = !wave->state;
        if (wave->state) {
          SetGPIO(wave->gpioMask);
          if (wave->gpio16Mask) {
            GP16O |= wave->gpio16Mask; // GPIO16 write slow as it's RMW
          }
          wave->nextServiceCycle = now + wave->nextTimeHighCycles;
          nextEventCycles = min_u32(nextEventCycles, wave->nextTimeHighCycles);
        } else {
          ClearGPIO(wave->gpioMask);
          if (wave->gpio16Mask) {
            GP16O &= ~wave->gpio16Mask;
          }
          wave->nextServiceCycle = now + wave->nextTimeLowCycles;
          nextEventCycles = min_u32(nextEventCycles, wave->nextTimeLowCycles);
        }
      } else {
        uint32_t deltaCycles = wave->nextServiceCycle - now;
        nextEventCycles = min_u32(nextEventCycles, deltaCycles);
      }
    }
  } while (--cnt && (nextEventCycles < MicrosecondsToCycles(4)));

  uint32_t curCycleCount = GetCycleCount();
  uint32_t deltaCycles = curCycleCount - lastCycleCount;
  lastCycleCount = curCycleCount;

  // Check for timed-out waveforms out of the high-frequency toggle loop
  for (size_t i = 0; i < countof(waveform); i++) {
    Waveform *wave = &waveform[i];
    if (wave->timeLeftCycles) {
      // Check for unsigned underflow with new > old
      if (deltaCycles >= wave->timeLeftCycles) {
        // Done, remove!
        wave->enabled = false;
        ClearGPIO(wave->gpioMask);
        GP16O &= ~wave->gpio16Mask;
      } else {
        uint32_t newTimeLeftCycles = wave->timeLeftCycles - deltaCycles;
        wave->timeLeftCycles = newTimeLeftCycles;
      }
    }
  }

  if (timer1CB) {
    nextEventCycles = min_u32(nextEventCycles, timer1CB());
  }

  #if F_CPU == 160000000
  if (nextEventCycles <= 5 * MicrosecondsToCycles(1)) {
    nextEventCycles = MicrosecondsToCycles(1) / 2;
  } else {
    nextEventCycles -= 5 * MicrosecondsToCycles(1);
  }
  nextEventCycles = nextEventCycles >> 1;
  #else
  if (nextEventCycles <= 6 * MicrosecondsToCycles(1)) {
    nextEventCycles = MicrosecondsToCycles(1) / 2;
  } else {
    nextEventCycles -= 6 * MicrosecondsToCycles(1);
  }
  #endif

  ReloadTimer(nextEventCycles);
}
