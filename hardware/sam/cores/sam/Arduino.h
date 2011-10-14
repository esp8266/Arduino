/* %arduino_license% */

#ifndef Arduino_h
#define Arduino_h

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "libsam/chip.h"
#include "binary.h"

#ifdef __cplusplus
extern "C"{
#endif // __cplusplus

#include "wiring_constants.h"

#define clockCyclesPerMicrosecond() ( SystemCoreClock / 1000000L )
#define clockCyclesToMicroseconds(a) ( ((a) * 1000L) / (SystemCoreClock / 1000L) )
#define microsecondsToClockCycles(a) ( ((a) * (SystemCoreClock / 1000L)) / 1000L )


#include "wiring.h"
#include "wiring_digital.h"
#include "wiring_analog.h"
#include "wiring_shift.h"
#include "WInterrupts.h"

/* sketch */
extern void setup( void ) ;
extern void loop( void ) ;

// Get the bit location within the hardware port of the given virtual pin.
// This comes from the pins_*.c file for the active board configuration.
//
// These perform slightly better as macros compared to inline functions
//
#define digitalPinToPort( ulPin )    ( g_APinDescription[ulPin]->pPort )
#define digitalPinToBitMask( ulPin ) ( g_APinDescription[ulPin]->dwPin )
#define digitalPinToTimer( P )   (  )
#define analogInPinToBit( P )    ( P )
#define portOutputRegister( P )  (  )
#define portInputRegister( P )   (  )
#define portModeRegister( P )    (  )

//#define NOT_A_PIN 0  // defined in pio.h/EPioType
#define NOT_A_PORT           0

#define NOT_ON_TIMER         0
#define TIMER0               1

typedef enum _EExt_Interrupts
{
  EXTERNAL_INT_0=0,
  EXTERNAL_INT_1=1,
  EXTERNAL_INT_2=2,
  EXTERNAL_INT_3=3,
  EXTERNAL_INT_4=4,
  EXTERNAL_INT_5=5,
  EXTERNAL_INT_6=6,
  EXTERNAL_INT_7=7,
  EXTERNAL_NUM_INTERRUPTS
} EExt_Interrupts ;

typedef void (*voidFuncPtr)( void ) ;

/* Define attribute */
#if defined   ( __CC_ARM   ) /* Keil µVision 4 */
    #define WEAK (__attribute__ ((weak)))
#elif defined ( __ICCARM__ ) /* IAR Ewarm 5.41+ */
    #define WEAK __weak
#elif defined (  __GNUC__  ) /* GCC CS */
    #define WEAK __attribute__ ((weak))
#endif

/* Definitions and types for pins */
typedef enum _EAnalogChannel
{
  NONE=-1,
  ADC0=0,
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

/**
 * Pin Attributes to be OR-ed
 */
#define PIN_ATTR_COMBO         (1UL<<0)
#define PIN_ATTR_ANALOG        (1UL<<1)
#define PIN_ATTR_DIGITAL       (1UL<<2)
#define PIN_ATTR_PWM           (1UL<<3)
#define PIN_ATTR_TIMER         (1UL<<4)

/* Types used for the tables below */
typedef struct _PinDescription
{
  Pio* pPort ;
  uint32_t ulPin ;
  uint32_t ulPeripheralId ;
  EPioType ulPinType ;
  uint32_t ulPinConfiguration ;
  uint32_t ulPinAttribute ;
  EAnalogChannel ulAnalogChannel ;
} PinDescription ;

/* Pins table to be instanciated into variant.cpp */
extern const PinDescription g_APinDescription[] ;

#ifdef __cplusplus
} // extern "C"

#include "WCharacter.h"
#include "WString.h"
#include "Tone.h"
#include "WMath.h"
#include "HardwareSerial.h"
#endif // __cplusplus

#endif // Arduino_h
