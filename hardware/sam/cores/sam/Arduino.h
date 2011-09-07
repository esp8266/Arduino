#ifndef Arduino_h
#define Arduino_h

#include <stdint.h>
//#include <sys/types.h>

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "binary.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

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

#define DEFAULT 1
#define EXTERNAL 0

// undefine stdlib's abs if encountered
#ifdef abs
#undef abs
#endif // abs

#define min(a,b) ((a)<(b)?(a):(b))
#define max(a,b) ((a)>(b)?(a):(b))
#define abs(x) ((x)>0?(x):-(x))
#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define round(x)     ((x)>=0?(long)((x)+0.5):(long)((x)-0.5))
#define radians(deg) ((deg)*DEG_TO_RAD)
#define degrees(rad) ((rad)*RAD_TO_DEG)
#define sq(x) ((x)*(x))

#define interrupts() __enable_irq()
#define noInterrupts() __disable_irq()

#define clockCyclesPerMicrosecond() ( VARIANT_MCK / 1000000L )
#define clockCyclesToMicroseconds(a) ( ((a) * 1000L) / (VARIANT_MCK / 1000L) )
#define microsecondsToClockCycles(a) ( ((a) * (VARIANT_MCK / 1000L)) / 1000L )

#define lowByte(w) ((uint8_t) ((w) & 0xff))
#define highByte(w) ((uint8_t) ((w) >> 8))

#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

typedef unsigned int word;

#define bit(b) (1UL << (b))

// TODO: to be checked
typedef uint8_t boolean ;
typedef uint8_t byte ;

#include "wiring.h"
#include "wiring_digital.h"
#include "wiring_analog.h"
#include "wiring_shift.h"

/* sketch */
extern void setup( void ) ;
extern void loop( void ) ;

// Get the bit location within the hardware port of the given virtual pin.
// This comes from the pins_*.c file for the active board configuration.
//
// These perform slightly better as macros compared to inline functions
//
#define digitalPinToPort( Pin )    ( APinDescription[Pin]->pPort )
#define digitalPinToBitMask( Pin ) ( APinDescription[Pin]->dwPin )
#define digitalPinToTimer( P )   (  )
#define analogInPinToBit( P )    ( P )
#define portOutputRegister( P )  (  )
#define portInputRegister( P )   (  )
#define portModeRegister( P )    (  )

//#define NOT_A_PIN 0  // defined in pio.h/EPioType
#define NOT_A_PORT           0

#define NOT_ON_TIMER         0
#define TIMER0               1

#ifdef __cplusplus
} // extern "C"
#endif // __cplusplus



#ifdef __cplusplus
#include "WCharacter.h"
#include "WString.h"
#include "HardwareSerial.h"

uint16_t makeWord( uint16_t w ) ;
uint16_t makeWord( byte h, byte l ) ;

#define word(...) makeWord(__VA_ARGS__)

#include "Tone.h"
#include "WMath.h"

#endif // __cplusplus

//! Include variant header
#include "variant.h"

//! Definitions and types for pins
typedef enum _EAnalogChannel
{
  ADC0,
  ADC1,
  ADC2,
  ADC3,
  ADC4,
  ADC5,
  ADC6,
  ADC7,
  ADC8,
  ADC9,
  ADC10,
  ADC11,
  ADC12,
  ADC13,
  ADC14,
  ADC15,
  DAC0,
  DAC1
} EAnalogChannel ;

/* Types used for the tables below */
typedef struct _PinDescription
{
  Pio* pPort ;
  uint32_t dwPin ;
  uint32_t dwPeripheralId ;
  EPioType dwPinType ;
  uint32_t dwPinAttribute ;
  EAnalogChannel dwAnalogChannel ;
} PinDescription ;

extern const PinDescription APinDescription[] ;

#include "pins_arduino.h"

#endif // Arduino_h
