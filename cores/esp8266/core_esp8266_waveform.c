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
#include "ets_sys.h"
#include "core_esp8266_waveform.h"

// Maximum delay between IRQs
#define MAXIRQUS (10000)

// If the cycles from now to an event are below this value, perform it anyway since IRQs take longer than this
#define CYCLES_FLUFF (100)

// Set/clear GPIO 0-15 by bitmask
#define SetGPIO(a) do { GPOS = a; } while (0)
#define ClearGPIO(a) do { GPOC = a; } while (0)

// Convert from us to ESP8266 clocks
#define MicrosecondsToCycles(m) (clockCyclesPerMicrosecond() * (m))

// Waveform generator can create tones, PWM, and servos
typedef struct {
  uint32_t nextServiceCycle;   // ESP cycle timer when a transition required
  uint32_t expiryCycle;        // For time-limited waveform, the cycle when this waveform must stop
  uint32_t nextTimeHighCycles; // Copy over low->high to keep smooth waveform
  uint32_t nextTimeLowCycles;  // Copy over high->low to keep smooth waveform
} Waveform;

// These can be accessed in interrupts, so ensure to bracket access with SEI/CLI
static Waveform waveform[17];        // State of all possible pins
static volatile uint32_t waveformState = 0;   // Is the pin high or low
static volatile uint32_t waveformEnabled = 0; // Is it actively running

// Enable lock-free by only allowing updates to waveformState and waveformEnabled from IRQ service routine
static volatile uint32_t waveformToEnable = 0; // Message from startWaveform to IRQ to actuall begin a wvf
static volatile uint32_t waveformToDisable = 0; // Message from startWaveform to IRQ to actuall begin a wvf

static uint32_t (*timer1CB)() = NULL;


// Non-speed critical bits
#pragma GCC optimize ("Os")

static inline ICACHE_RAM_ATTR uint32_t GetCycleCount() {
  uint32_t ccount;
  __asm__ __volatile__("esync; rsr %0,ccount":"=a"(ccount));
  return ccount;
}

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
    timer1_write(MicrosecondsToCycles(1)); // Cause an interrupt post-haste
  } else if (timerRunning && !fn && !waveformEnabled) {
    deinitTimer();
  }
}

// Start up a waveform on a pin, or change the current one.  Will change to the new
// waveform smoothly on next low->high transition.  For immediate change, stopWaveform()
// first, then it will immediately begin.
int startWaveform(uint8_t pin, uint32_t timeHighUS, uint32_t timeLowUS, uint32_t runTimeUS) {
  if ((pin > 16) || isFlashInterfacePin(pin)) {
    return false;
  }
  Waveform *wave = &waveform[pin];
#if F_CPU == 160000000
  wave->nextTimeHighCycles = MicrosecondsToCycles(timeHighUS) - 140;  // Take out some time for IRQ codepath
  wave->nextTimeLowCycles = MicrosecondsToCycles(timeLowUS) - 140;  // Take out some time for IRQ codepath
#else
  wave->nextTimeHighCycles = MicrosecondsToCycles(timeHighUS) > 280 ? MicrosecondsToCycles(timeHighUS) - 280 : MicrosecondsToCycles(timeHighUS);  // Take out some time for IRQ codepath
  wave->nextTimeLowCycles = MicrosecondsToCycles(timeLowUS) > 280 ? MicrosecondsToCycles(timeLowUS)- 280 : MicrosecondsToCycles(timeLowUS);  // Take out some time for IRQ codepath
#endif
  wave->expiryCycle = runTimeUS ? GetCycleCount() + MicrosecondsToCycles(runTimeUS) : 0;
  if (runTimeUS && !wave->expiryCycle) {
    wave->expiryCycle = 1; // expiryCycle==0 means no timeout, so avoid setting it
  }

  uint32_t mask = 1<<pin;
  if (!waveformEnabled && mask) {
    // Actually set the pin high or low in the IRQ service to guarantee times
    wave->nextServiceCycle = GetCycleCount() + MicrosecondsToCycles(1);
    waveformToEnable |= mask;
    if (!timerRunning) {
      initTimer();
    }
    timer1_write(MicrosecondsToCycles(1)); // Cause an interrupt post-haste
    while (waveformToEnable) {
      delay(1); // Wait for waveform to update
    }
  }

  return true;
}

// Speed critical bits
#pragma GCC optimize ("O2")
// Normally would not want two copies like this, but due to different
// optimization levels the inline attribute gets lost if we try the
// other version.

static inline ICACHE_RAM_ATTR uint32_t GetCycleCountIRQ() {
  uint32_t ccount;
  __asm__ __volatile__("esync; rsr %0,ccount":"=a"(ccount));
  return ccount;
}

static inline ICACHE_RAM_ATTR uint32_t min_u32(uint32_t a, uint32_t b) {
  if (a < b) {
    return a;
  }
  return b;
}

// Stops a waveform on a pin
int ICACHE_RAM_ATTR stopWaveform(uint8_t pin) {
  // Can't possibly need to stop anything if there is no timer active
  if (!timerRunning) {
    return false;
  }
  // If user sends in a pin >16 but <32, this will always point to a 0 bit
  // If they send >=32, then the shift will result in 0 and it will also return false
  uint32_t mask = 1<<pin;
  if (!(waveformEnabled & mask)) {
    return false; //It's not running, nothing to do here
  }
  waveformToDisable |= mask;
  timer1_write(MicrosecondsToCycles(1));
  while (waveformToDisable) {
    delay(1); // Wait for IRQ to update
  }
  if (!waveformEnabled && !timer1CB) {
    deinitTimer();
  }
  return true;
}

static ICACHE_RAM_ATTR void timer1Interrupt() {
  uint32_t nextEventCycles;
#if F_CPU == 160000000
  int cnt = 20;
#else
  int cnt = 10;
#endif

  // Handle enable/disable requests from main app.
  waveformEnabled = (waveformEnabled & ~waveformToDisable) | waveformToEnable; // Set the requested waveforms on/off
  waveformState &= ~waveformToEnable;  // And clear the state of any just started
  waveformToEnable = 0;
  waveformToDisable = 0;

  do {
    nextEventCycles = MicrosecondsToCycles(MAXIRQUS);
    for (size_t i = 0; i <= 16; i++) {
      uint32_t mask = 1<<i;

      // If it's not on, ignore!
      if (!(waveformEnabled & mask)) {
        continue;
      }

      Waveform *wave = &waveform[i];
      uint32_t now = GetCycleCountIRQ();

      // Disable any waveforms that are done
      if (wave->expiryCycle) {
        int32_t expiryToGo = wave->expiryCycle - now;
        if (expiryToGo < 0) {
            // Done, remove!
            waveformEnabled &= ~mask;
            if (i==16) GP16O &= ~1;
            else ClearGPIO(mask);
            continue;
          }
      }

      // Check for toggles
      int32_t cyclesToGo = wave->nextServiceCycle - now;
      if (cyclesToGo < 0) {
        waveformState ^= mask;
        if (waveformState & mask) {
          if (i==16) GP16O |= 1; // GPIO16 write slow as it's RMW
          else SetGPIO(mask);

          wave->nextServiceCycle = now + wave->nextTimeHighCycles;
          nextEventCycles = min_u32(nextEventCycles, wave->nextTimeHighCycles);
        } else {
          if (i==16) GP16O &= ~1; // GPIO16 write slow as it's RMW
          else ClearGPIO(mask);

          wave->nextServiceCycle = now + wave->nextTimeLowCycles;
          nextEventCycles = min_u32(nextEventCycles, wave->nextTimeLowCycles);
        }
      } else {
        uint32_t deltaCycles = wave->nextServiceCycle - now;
        nextEventCycles = min_u32(nextEventCycles, deltaCycles);
      }
    }
  } while (--cnt && (nextEventCycles < MicrosecondsToCycles(4)));

  if (timer1CB) {
    nextEventCycles = min_u32(nextEventCycles, timer1CB());
  }

#if F_CPU == 160000000
  if (nextEventCycles < MicrosecondsToCycles(5)) {
    nextEventCycles = MicrosecondsToCycles(1);
  } else {
    nextEventCycles -= MicrosecondsToCycles(4) + MicrosecondsToCycles(1)/2;
  }
  nextEventCycles = nextEventCycles >> 1;
#else
  if (nextEventCycles < MicrosecondsToCycles(8)) {
    nextEventCycles = MicrosecondsToCycles(2);
  } else {
    nextEventCycles -= MicrosecondsToCycles(6);
  }
#endif

  // Do it here instead of global function to save time and because we know it's edge-IRQ
  TEIE |= TEIE1; //edge int enable
  T1L = nextEventCycles; // Already know we're in range by MAXIRQUS
}
