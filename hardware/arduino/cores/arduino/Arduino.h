#ifndef Arduino_h
#define Arduino_h

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <avr/interrupt.h>

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include "binary.h"
#include "pins_arduino.h"

#ifdef __cplusplus
extern "C"{
#endif

#define HIGH 0x1
#define LOW  0x0

#define INPUT 0x0
#define OUTPUT 0x1

#define true 0x1
#define false 0x0

#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105

#define SERIAL  0x0
#define DISPLAY 0x1

#define LSBFIRST 0
#define MSBFIRST 1

#define CHANGE 1
#define FALLING 2
#define RISING 3

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
#define INTERNAL1V1 2
#define INTERNAL2V56 3
#else
#define INTERNAL 3
#endif
#define DEFAULT 1
#define EXTERNAL 0

// undefine stdlib's abs if encountered
#ifdef abs
#undef abs
#endif

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

#define interrupts() sei()
#define noInterrupts() cli()

#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( ((a) * 1000L) / (F_CPU / 1000L) )
#define microsecondsToClockCycles(a) ( ((a) * (F_CPU / 1000L)) / 1000L )

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))


typedef unsigned int word;

#define bit(b) (1UL << (b))

typedef uint8_t boolean;
typedef uint8_t byte;

void init(void);

void pinMode_lookup(uint8_t, uint8_t);
void digitalWrite_lookup(uint8_t, uint8_t);
int digitalRead_lookup(uint8_t);
int analogRead(uint8_t);
void analogReference(uint8_t mode);
void analogWrite(uint8_t, int);

unsigned long millis(void);
unsigned long micros(void);
void delay(unsigned long);
void delayMicroseconds(unsigned int us);
unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout);

void shiftOut(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder, uint8_t val);
uint8_t shiftIn(uint8_t dataPin, uint8_t clockPin, uint8_t bitOrder);

void attachInterrupt(uint8_t, void (*)(void), int mode);
void detachInterrupt(uint8_t);

void setup(void);
void loop(void);

/*
 * Check if a given pin requires locking.
 * When accessing lower 32 IO ports we can use SBI/CBI instructions, which are atomic. However
 * other IO ports require load+modify+store and we need to make them atomic by disabling
 * interrupts.
 */
INLINED int portWriteNeedsLocking(uint8_t pin)
{
	/* SBI/CBI instructions only work on lower 32 IO ports */
	if (inlined_portOutputRegister(inlined_digitalPinToPort(pin)) > (volatile uint8_t*)&_SFR_IO8(0x1F)) {
		return 1;
	}
	return 0;
}

/*
 * These functions will perform OR/AND on a given register, and are atomic.
 */
extern void __digitalWriteOR_locked(volatile uint8_t*out, uint8_t bit);
extern void __digitalWriteAND_locked(volatile uint8_t*out, uint8_t bit);

INLINED void digitalWrite(uint8_t pin, uint8_t value)
{
	if (__builtin_constant_p(pin)) {
		if (portWriteNeedsLocking(pin)) {
			if (value==LOW) {
				__digitalWriteAND_locked(inlined_portOutputRegister(inlined_digitalPinToPort(pin)),~inlined_digitalPinToBitMask(pin));
			} else {
				__digitalWriteOR_locked(inlined_portOutputRegister(inlined_digitalPinToPort(pin)),inlined_digitalPinToBitMask(pin));
			}
		} else {
			if (value==LOW) {
				*inlined_portOutputRegister(inlined_digitalPinToPort(pin)) &= ~(inlined_digitalPinToBitMask(pin));
			} else {
				*inlined_portOutputRegister(inlined_digitalPinToPort(pin)) |= inlined_digitalPinToBitMask(pin);
			}
		}
	} else {
		digitalWrite_lookup(pin,value);
	}
}

INLINED void pinMode(uint8_t pin, uint8_t mode)
{
	if (__builtin_constant_p(pin)) {
		if (mode==INPUT) {
			*inlined_portModeRegister(inlined_digitalPinToPort(pin)) &= ~(inlined_digitalPinToBitMask(pin));
		} else {
			*inlined_portModeRegister(inlined_digitalPinToPort(pin)) |= inlined_digitalPinToBitMask(pin);
		}
	} else {
		pinMode_lookup(pin,mode);
	}
}

INLINED int digitalRead(uint8_t pin)
{
	if (__builtin_constant_p(pin)) {
		return !! *inlined_portInputRegister(inlined_digitalPinToPort(pin));
	} else {
		return digitalRead_lookup(pin);
	}
}


#ifdef __cplusplus
} // extern "C"
#endif

#ifdef __cplusplus
#include "WCharacter.h"
#include "WString.h"
#include "HardwareSerial.h"

uint16_t makeWord(uint16_t w);
uint16_t makeWord(byte h, byte l);

#define word(...) makeWord(__VA_ARGS__)

unsigned long pulseIn(uint8_t pin, uint8_t state, unsigned long timeout = 1000000L);

void tone(uint8_t _pin, unsigned int frequency, unsigned long duration = 0);
void noTone(uint8_t _pin);

// WMath prototypes
long random(long);
long random(long, long);
void randomSeed(unsigned int);
long map(long, long, long, long, long);

#endif

#endif