/*
 Arduino.h - Main include file for the Arduino SDK
 Copyright (c) 2005-2013 Arduino Team.  All right reserved.

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
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef Arduino_h
#define Arduino_h

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "umm_malloc/umm_malloc_cfgport.h"
#include "stdlib_noniso.h"
#include "binary.h"
#include "esp8266_peri.h"
#include "twi.h"

#include "core_esp8266_features.h"
#include "core_esp8266_version.h"

#define HIGH 0x1
#define LOW  0x0

//GPIO FUNCTIONS
#define INPUT             0x00
#define INPUT_PULLUP      0x02
#define INPUT_PULLDOWN_16 0x04 // PULLDOWN only possible for pin16
#define OUTPUT            0x01
#define OUTPUT_OPEN_DRAIN 0x03
#define WAKEUP_PULLUP     0x05
#define WAKEUP_PULLDOWN   0x07
#define SPECIAL           0xF8 //defaults to the usable BUSes uart0rx/tx uart1tx and hspi
#define FUNCTION_0        0x08
#define FUNCTION_1        0x18
#define FUNCTION_2        0x28
#define FUNCTION_3        0x38
#define FUNCTION_4        0x48

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105
#define EULER 2.718281828459045235360287471352

#define SERIAL  0x0
#define DISPLAY 0x1

#define LSBFIRST 0
#define MSBFIRST 1

//Interrupt Modes
#define RISING    0x01
#define FALLING   0x02
#define CHANGE    0x03
#define ONLOW     0x04
#define ONHIGH    0x05
#define ONLOW_WE  0x0C
#define ONHIGH_WE 0x0D

#define DEFAULT 1
#define EXTERNAL 0

//timer dividers
enum TIM_DIV_ENUM {
  TIM_DIV1 = 0,   //80MHz (80 ticks/us - 104857.588 us max)
  TIM_DIV16 = 1,  //5MHz (5 ticks/us - 1677721.4 us max)
  TIM_DIV256 = 3 //312.5Khz (1 tick = 3.2us - 26843542.4 us max)
};


//timer int_types
#define TIM_EDGE	0
#define TIM_LEVEL	1
//timer reload values
#define TIM_SINGLE	0 //on interrupt routine you need to write a new value to start the timer again
#define TIM_LOOP	1 //on interrupt the counter will start with the same value again

#define timer1_read()           (T1V)
#define timer1_enabled()        ((T1C & (1 << TCTE)) != 0)
#define timer1_interrupted()    ((T1C & (1 << TCIS)) != 0)

typedef void(*timercallback)(void);

void timer1_isr_init(void);
void timer1_enable(uint8_t divider, uint8_t int_type, uint8_t reload);
void timer1_disable(void);
void timer1_attachInterrupt(timercallback userFunc);
void timer1_detachInterrupt(void);
void timer1_write(uint32_t ticks); //maximum ticks 8388607

// timer0 is a special CPU timer that has very high resolution but with
// limited control.
// it uses CCOUNT (ESP.GetCycleCount()) as the non-resetable timer counter
// it does not support divide, type, or reload flags
// it is auto-disabled when the compare value matches CCOUNT
// it is auto-enabled when the compare value changes
#define timer0_interrupted()    (ETS_INTR_PENDING() & (_BV(ETS_COMPARE0_INUM)))
#define timer0_read() ((__extension__({uint32_t count;__asm__ __volatile__("esync; rsr %0,ccompare0":"=a" (count));count;})))
#define timer0_write(count) __asm__ __volatile__("wsr %0,ccompare0; esync"::"a" (count) : "memory")

void timer0_isr_init(void);
void timer0_attachInterrupt(timercallback userFunc);
void timer0_detachInterrupt(void);

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

#define interrupts() xt_rsil(0)
#define noInterrupts() xt_rsil(15)

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

// avr-libc defines _NOP() since 1.6.2
#ifndef _NOP
#define _NOP() do { __asm__ volatile ("nop"); } while (0)
#endif

typedef uint16_t word;

#define bit(b) (1UL << (b))
#define _BV(b) (1UL << (b))

typedef bool boolean;
typedef uint8_t byte;

void ets_intr_lock();
void ets_intr_unlock();

void init(void);
void initVariant(void);

void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);
int digitalRead(uint8_t pin);
int analogRead(uint8_t pin);
void analogReference(uint8_t mode);
void analogWrite(uint8_t pin, int val);
void analogWriteMode(uint8_t pin, int val, bool openDrain);
void analogWriteFreq(uint32_t freq);
void analogWriteResolution(int res);
void analogWriteRange(uint32_t range);

unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout);
unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout);

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

void attachInterrupt(uint8_t pin, void (*)(void), int mode);
void detachInterrupt(uint8_t pin);
void attachInterruptArg(uint8_t pin, void (*)(void*), void* arg, int mode);

#if FLASH_MAP_SUPPORT
#include "flash_hal.h"
#endif
void preinit(void);
void setup(void);
void loop(void);

void yield(void);

void optimistic_yield(uint32_t interval_us);

#define _PORT_GPIO16    1
#define digitalPinToPort(pin)       (((pin)==16)?(_PORT_GPIO16):(0))
#define digitalPinToBitMask(pin)    (((pin)==16)?(1):(1UL << (pin)))
#define digitalPinToTimer(pin)      (0)
#define portOutputRegister(port)    (((port)==_PORT_GPIO16)?((volatile uint32_t*) &GP16O):((volatile uint32_t*) &GPO))
#define portInputRegister(port)     (((port)==_PORT_GPIO16)?((volatile uint32_t*) &GP16I):((volatile uint32_t*) &GPI))
#define portModeRegister(port)      (((port)==_PORT_GPIO16)?((volatile uint32_t*) &GP16E):((volatile uint32_t*) &GPE))

#define NOT_A_PIN -1
#define NOT_A_PORT -1
#define NOT_AN_INTERRUPT -1
#define NOT_ON_TIMER 0

#ifdef __cplusplus
} // extern "C"
#endif

// undefine stdlib's definitions when encountered, provide abs that supports floating point for C code
#ifndef __cplusplus
#undef abs
#define abs(x) ({ __typeof__(x) _x = (x); _x > 0 ? _x : -_x; })
#undef round
#define round(x) ({ __typeof__(x) _x = (x); _x >= 0 ? (long)(_x + 0.5) : (long)(_x - 0.5); })
#endif // ifndef __cplusplus

// from this point onward, we need to configure the c++ environment
#ifdef __cplusplus

#include <algorithm>
#include <cstdlib>
#include <cmath>


#include "mmu_iram.h"


using std::min;
using std::max;
using std::round;
using std::isinf;
using std::isnan;

// Use float-compatible stl abs() and round(), we don't use Arduino macros to avoid issues with the C++ libraries
using std::abs;
using std::round;

#define _min(a,b) ({ decltype(a) _a = (a); decltype(b) _b = (b); _a < _b? _a : _b; })
#define _max(a,b) ({ decltype(a) _a = (a); decltype(b) _b = (b); _a > _b? _a : _b; })

uint16_t makeWord(uint16_t w);
uint16_t makeWord(byte h, byte l);

#define word(...) makeWord(__VA_ARGS__)

unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout = 1000000L);
unsigned long pulseInLong(uint8_t pin, uint8_t state, unsigned long timeout = 1000000L);

void tone(uint8_t _pin, unsigned int frequency, unsigned long duration = 0);
void tone(uint8_t _pin, int frequency, unsigned long duration = 0);
void tone(uint8_t _pin, double frequency, unsigned long duration = 0);
void noTone(uint8_t _pin);

// WMath prototypes
long random(long);
long random(long, long);
void randomSeed(unsigned long);
long secureRandom(long);
long secureRandom(long, long);
long map(long, long, long, long, long);

void setTZ(const char* tz);

// configure time using POSIX TZ string
// server pointers *must remain valid* for the duration of the program
void configTime(const char* tz, const char* server1,
    const char* server2 = nullptr, const char* server3 = nullptr);

// configures with approximated TZ value. part of the old api, prefer configTime with TZ variable
void configTime(int timezone, int daylightOffset_sec, const char* server1,
    const char* server2 = nullptr, const char* server3 = nullptr);

// esp32 api compatibility
inline void configTzTime(const char* tz, const char* server1,
    const char* server2 = nullptr, const char* server3 = nullptr)
{
    configTime(tz, server1, server2, server3);
}

bool getLocalTime(struct tm * info, uint32_t ms = 5000);

// Everything we expect to be implicitly loaded for the sketch
#include <pgmspace.h>

#include "WCharacter.h"
#include "WString.h"

// configTime wrappers for temporary server{1,2,3} strings
void configTime(int timezone, int daylightOffset_sec, String server1,
    String server2 = String(), String server3 = String());
void configTime(const char* tz, String server1,
    String server2 = String(), String server3 = String());

#include "HardwareSerial.h"
#include "Esp.h"
#include "Updater.h"

#endif // __cplusplus

#include "debug.h"
#include "pins_arduino.h"

#endif

#ifdef DEBUG_ESP_OOM
// Position *alloc redefinition at the end of Arduino.h because <cstdlib> would
// have undefined them. Mandatory for supporting OOM and other debug alloc
// definitions in .ino files
#include "heap_api_debug.h"
#endif
