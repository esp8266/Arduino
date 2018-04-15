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
  uint32_t nextServiceCycle;
  uint32_t timeHighCycles;
  uint32_t timeLowCycles;
  uint32_t timeLeftCycles;
  // To ensure stable change, only copy these over on low->high transition
  uint16_t gpioMask;
  uint16_t gpio16Mask;
//  unsigned gpioPin            : 4; // Check gpioPin16 first
  unsigned state              : 1;
  unsigned nextTimeHighCycles : 31;
//  unsigned gpioPin16          : 1; // Special case for weird IO16
  unsigned enabled            : 1;
  unsigned nextTimeLowCycles  : 31;
} Waveform;

// These can be accessed in interrupts, so ensure to bracket access with SEI/CLI
static Waveform waveform[] = {
  {0, 0, 0, 0, 1<<0, 0, 0, 0, 0, 0}, // GPIO0
  {0, 0, 0, 0, 1<<1, 0, 0, 0, 0, 0}, // GPIO1
  {0, 0, 0, 0, 1<<2, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 1<<3, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 1<<4, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 1<<5, 0, 0, 0, 0, 0},
  // GPIOS 6-11 not allowed, used for flash
  {0, 0, 0, 0, 1<<12, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 1<<13, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 1<<14, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 1<<15, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 0, 0, 0, 0}  // GPIO16
};


// Maximum umber of moves per stepper queue
#define STEPPERQUEUESIZE 16

// Stepper generator can send # of steps with given velocity and linear acceleration
// Can do any piecewise linear acceleration profile
typedef struct {
  float    j_2;         // Jerk value/2, pulses/sec/sec/sec
  float    a0;          // Initial acceleration, pulses/sec/sec
  float    v0;          // Initial velocity, pulses/sec
  unsigned pulses : 16; // Total # of pulses to emit
  unsigned sync : 1;    // Wait for all channels to have a sync before popping next move
  unsigned dir  : 1;    // CW=0 or CCW=1
} Motion;

// Minimum clock cycles per step.
#define MINSTEPCYCLES (4000)
// Maxmimum clock cycles per step.
#define MAXSTEPCYCLES (1000000000)

// Pre-allocated circular buffer
typedef struct {
  Motion * move;
  uint32_t nextEventCycles;

  // Copied from head for fast access
  uint16_t pulses;
  uint32_t cumCycles;
  float    j_2;
  float    a0;
  float    v0;
  unsigned sync : 1;
  unsigned dir  : 1;

  unsigned gpioPin    : 4;
  unsigned finished   : 1;

  uint8_t  readPtr;
  uint8_t  writePtr;
  uint8_t  validEntries;
} StepperQueue;

static volatile StepperQueue *stepQ = NULL;
static volatile uint8_t stepQCnt = 0;
static uint8_t stepDirPin = 16; // The weird one

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

// Called by the IRQ to move the next Motion to the head
static inline ICACHE_RAM_ATTR void PopStepper(int i) {
  StepperQueue *q = (StepperQueue *)&stepQ[i];
  if (q->validEntries == 0) {
    q->sync = false;
    q->finished = true;
    q->nextEventCycles = 0;
    return;
  }
  q->finished = false;

  Motion *head = &q->move[q->readPtr];
  q->pulses = head->pulses;
  q->cumCycles = 0;
  q->j_2 = head->j_2;
  q->a0 = head->a0;
  q->v0 = head->v0;
  q->sync = head->sync;
  q->dir = head->dir;
  q->nextEventCycles = 0; // (uint32_t)((clockCyclesPerMicrosecond()*1000000.0) / q->v0);
  q->readPtr = (q->readPtr + 1) & (STEPPERQUEUESIZE - 1);
  q->validEntries--;
}

// Called by the user to detach a stepper and free memory
int removeStepper(uint8_t pin) {
  sei();
  for (int i = 0; i < stepQCnt; i++) {
    if (stepQ[i].gpioPin == pin) {
      memmove((void*)&stepQ[i], (void*)&stepQ[i + 1], (stepQCnt - i - 1) * sizeof(stepQ[0]));
      stepQ = (StepperQueue*)realloc((void*)stepQ, (stepQCnt - 1) * sizeof(stepQ[0]));
      stepQCnt--;
      cli();
      return true;
    }
  }
  cli();
  return false;
}

// Add a stepper move, return TRUE on success, FALSE on out of space
// Calling application needs to ensure IRQS are disabled for the call!
static int PushStepper(int gpioPin, const Motion *nextMove) {
  StepperQueue *q = NULL;
  int i;
  sei();
  // Determine which queue it should be on, or maybe add one if needed
  for (i = 0; i < stepQCnt; i++) {
    if (stepQ[i].gpioPin == gpioPin) {
      q = (StepperQueue *)&stepQ[i];
      break;
    }
  }
  if (q == NULL) {
    // Make the stepper move array
    Motion *move = (Motion*)malloc(sizeof(stepQ[0].move) * STEPPERQUEUESIZE);
    if (!move) {
      cli();
      return false;
    }

    // Add a queue
    StepperQueue *newStepQ = (StepperQueue*)realloc((void*)stepQ, (stepQCnt + 1) * sizeof(stepQ[0]));
    if (!newStepQ) {
      cli();
      free(move);
      return false;
    }
    stepQ = newStepQ;
    q = (StepperQueue*) & (stepQ[stepQCnt]); // The one just added
    memset(q, 0, sizeof(*q));
    q->move = move;
    q->readPtr = 0;
    q->writePtr = 0;
    q->validEntries = 0;
    q->gpioPin = gpioPin;
    q->finished = true;
    i = stepQCnt;
    stepQCnt++;
  }
  // Have queue ready, can we fit this new one in?
  if (q->validEntries == STEPPERQUEUESIZE) {
    return false;
  }

  // Store and record
  q->move[q->writePtr] = *nextMove; // Copy actual values
  q->validEntries++;
  q->writePtr = (q->writePtr + 1) & (STEPPERQUEUESIZE - 1);
  if (!timerRunning) {
    initTimer();
    ReloadTimer(10); // Cause an interrupt post-haste
  }
  if (!q->sync) {
    PopStepper(i); // If there's only this in the queue and we're not waiting for sync, start it up
  }
  cli();
  return true;
}

// Called by user to add a PWL move to the queue, returns false if there is no space left
int pushStepperMove(uint8_t pin, int dir, int sync, uint16_t pulses, float j, float a0, float v0) {
  if (pin > 15) {
    // Only GPIO 0...15 allowed
    return false;
  }
  Motion m;
  m.pulses = pulses;
  m.j_2 = j / 2.0;
  m.a0 = a0;
  m.v0 = v0;
  m.sync = sync ? 1 : 0;
  m.dir = dir ? 1 : 0;
  return PushStepper(pin, &m);
}

// Assign a pin to stepper DIR
int setStepperDirPin(uint8_t pin) {
  if (pin > 16) {
    return false;
  }
  stepDirPin = pin;
  return true;
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
  sei();
  wave->nextTimeHighCycles = MicrosecondsToCycles(timeHighUS);
  wave->nextTimeLowCycles = MicrosecondsToCycles(timeLowUS);
  wave->timeLeftCycles = MicrosecondsToCycles(runTimeUS);
  if (!wave->enabled) {
    wave->state = 0;
    // Actually set the pin high or low in the IRQ service to guarantee times
    wave->timeHighCycles = MicrosecondsToCycles(timeHighUS) - 30; // Sub off some of the codepath time
    wave->timeLowCycles = MicrosecondsToCycles(timeLowUS) - 30; // Sub off some of the codepath time
    wave->nextServiceCycle = GetCycleCount() + MicrosecondsToCycles(1);
    wave->enabled = 1;
    if (!timerRunning) {
      initTimer();
    }
    ReloadTimer(MicrosecondsToCycles(1)); // Cause an interrupt post-haste
  }
  cli();
  return true;
}

// Stops a waveform on a pin
int stopWaveform(uint8_t pin) {
  for (size_t i = 0; i < countof(waveform); i++) {
    if (((pin == 16) && waveform[i].gpio16Mask) || ((pin != 16) && (waveform[i].gpioMask == 1<<pin))) {
      sei();
      waveform[i].enabled = 0;
      int cnt = stepQCnt;
      for (size_t i = 0; i < countof(waveform); i++) {
        cnt += waveform[i].enabled ? 1 : 0;
      }
      if (!cnt) {
        deinitTimer();
      }
      cli();
      return true;
    }
  }
  cli();
  return false;
}



// Send pulses for specific direction.
// Stepper direction pin needs to be set before calling (helps ensure setup time)
static ICACHE_RAM_ATTR void AdvanceSteppers(uint32_t deltaCycles, int dir) {
  static uint16_t toClear = 0; // Store last call's pins to allow us to meet hold time by clearing on the processing of the other dir
  uint16_t pulseGPIO = 0;
  for (size_t i = 0; i < stepQCnt; i++) {
    StepperQueue *q = (StepperQueue*)&stepQ[i];
    if (q->dir != dir || q->finished) {
      continue;
    }
    q->cumCycles += deltaCycles;
    uint32_t newNextEventCycles = q->nextEventCycles - deltaCycles;
    if ((deltaCycles >= q->nextEventCycles) || (newNextEventCycles <= CYCLES_FLUFF)) {
      // If there are no more pulses in the current motion, try to pop next one here
      if (!q->pulses) {
        if (!q->sync) {
          PopStepper(i);
          if (!q->pulses) {
            // We tried to pop, but there's nothing left, done!
            continue;
          }
          // We will generate the first pulse of the next motion later on in this loop
        } else {
          // Sync won't allow us to advance here.  The main loop will have to
          // call this loop *again* after all are processed the first time
          // if we are sync'd.
          q->nextEventCycles = 0; // Don't look at this for timing
          continue;
        }
      }
      pulseGPIO |= 1 << q->gpioPin;
      q->pulses--;

      // Forgive me for going w/FP.  The dynamic range for fixed math would require many 64 bit multiplies
      static const float cycPerSec = 1000000.0 * clockCyclesPerMicrosecond();
      static const float secPerCyc = 1.0 / (1000000.0 * clockCyclesPerMicrosecond());
      float t = q->cumCycles * secPerCyc;
      float newVel = ((q->j_2 * t) + q->a0) * t + q->v0;
      uint32_t newPeriodCycles = (uint32_t)(cycPerSec / newVel);
      if (newPeriodCycles < MINSTEPCYCLES) {
        newPeriodCycles = MINSTEPCYCLES;
      } else if (newPeriodCycles > MAXSTEPCYCLES) {
        newPeriodCycles = MAXSTEPCYCLES;
      }
      q->nextEventCycles = newPeriodCycles;
    } else {
      q->nextEventCycles = newNextEventCycles;
    }
  }
  ClearGPIO(toClear);
  SetGPIO(pulseGPIO);
  toClear = pulseGPIO;
}


static ICACHE_RAM_ATTR uint32_t ProcessSteppers(uint32_t deltaCycles) {
  ClearGPIOPin(stepDirPin);
  AdvanceSteppers(deltaCycles, 0);
  SetGPIOPin(stepDirPin);
  AdvanceSteppers(deltaCycles, 1);

  // Check for sync, and if all set and 0 steps clear it
  bool haveSync = true;
  bool wantSync = false;
  for (int i = 0; i < stepQCnt; i++) {
    haveSync &= stepQ[i].sync && stepQ[i].finished;
    wantSync |= stepQ[i].sync;
  }

  if (wantSync && haveSync) { // Sync requested, and hit
    for (int i = 0; i < stepQCnt; i++) {
      PopStepper(i);
      stepQ[i].nextEventCycles = 1; // Cause the pulse to fire immediately
    }
    // Hokey, but here only now could we know it was time to fire everyone again
    ClearGPIOPin(stepDirPin);
    AdvanceSteppers(deltaCycles, 0);
    SetGPIOPin(stepDirPin);
    AdvanceSteppers(deltaCycles, 1);
  }
  // When's the next event?
  uint32_t nextEventCycles = MicrosecondsToCycles(MAXIRQUS);
  for (size_t i = 0; i < stepQCnt; i++) {
    if (stepQ[i].nextEventCycles) {
      nextEventCycles = min_u32(nextEventCycles, stepQ[i].nextEventCycles);
    }
  }

  return nextEventCycles;
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
          wave->nextServiceCycle = now + wave->timeHighCycles;
          wave->timeHighCycles = wave->nextTimeHighCycles;
          nextEventCycles = min_u32(nextEventCycles, wave->timeHighCycles);
        } else {
          ClearGPIO(wave->gpioMask);
          if (wave->gpio16Mask) {
            GP16O &= ~wave->gpio16Mask;
          }
          wave->nextServiceCycle = now + wave->timeLowCycles;
          wave->timeLowCycles = wave->nextTimeLowCycles;
          nextEventCycles = min_u32(nextEventCycles, wave->timeLowCycles);
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

  if (stepQCnt) {
    nextEventCycles = min_u32(nextEventCycles, ProcessSteppers(deltaCycles));
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

