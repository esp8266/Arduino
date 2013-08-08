// Squawk Soft-Synthesizer Library for Arduino
//
// Davey Taylor 2013
// d.taylor@arduino.cc

#ifndef _SQUAWK_H_
#define _SQUAWK_H_
#include <stddef.h>
#include <inttypes.h>
#include "Arduino.h"

#define Melody const uint8_t PROGMEM

class SquawkStream {
	public:
	  virtual ~SquawkStream() = 0;
    virtual uint8_t read() = 0;
    virtual void seek(size_t offset) = 0;
};
inline SquawkStream::~SquawkStream() { }

class SquawkSynth {

protected:
  // Load and play specified melody
  void play(SquawkStream *melody);

public:
  SquawkSynth() {};

  // Initialize Squawk to generate samples at sample_rate Hz
  void begin(uint16_t sample_rate);

  // Load and play specified melody
  // melody needs to point to PROGMEM data
  void play(const uint8_t *melody);
  
  // Resume currently loaded melody (or enable direct osc manipulation by sketch)
  void play();
    
  // Pause playback
  void pause();
  
  // Stop playback (unloads song)
  void stop();
  
  // Tune Squawk to a different frequency - default is 1.0
  void tune(float tuning);

  // Change the tempo - default is 50
	void tempo(uint16_t tempo);
};

extern SquawkSynth Squawk;

// oscillator structure
typedef struct {
  uint8_t  vol;
  uint16_t freq;
  uint16_t phase;
} osc_t;

typedef osc_t Oscillator;

// oscillator memory
extern osc_t osc[4];
extern uint8_t pcm;
// channel 0 is pulse wave @ 25% duty
// channel 1 is square wave
// channel 2 is triangle wave
// channel 3 is noise

// For channel 3, freq is used as part of its LFSR and should not be changed.
// LFSR: Linear feedback shift register, a method of producing a
// pseudo-random bit sequence, used to generate nasty noise.

#ifdef __AVR_ATmega32U4__
// Supported configurations for ATmega32U4
#define SQUAWK_PWM_PIN5  OCR3AL
#define SQUAWK_PWM_PIN11 OCR0A
#define SQUAWK_PWM_PIN3  OCR0B
/*
// NOT SUPPORTED YET
#define SQUAWK_PWM_PIN6  OCR4D
#define SQUAWK_PWM_PIN9  OCR4B
#define SQUAWK_PWM_PIN10 OCR4B
*/
#endif

#ifdef __AVR_ATmega168__
// Supported configurations for ATmega168
#define SQUAWK_PWM_PIN6  OCR0A
#define SQUAWK_PWM_PIN5  OCR0B
#define SQUAWK_PWM_PIN11 OCR2A
#define SQUAWK_PWM_PIN3  OCR2B
#endif

#ifdef __AVR_ATmega328P__
// Supported configurations for ATmega328P
#define SQUAWK_PWM_PIN6  OCR0A
#define SQUAWK_PWM_PIN5  OCR0B
#define SQUAWK_PWM_PIN11 OCR2A
#define SQUAWK_PWM_PIN3  OCR2B
#endif

/*
// NOT SUPPORTED YET
#define SQUAWK_SPI SPDR
#define SQUAWK_RLD_PORTB PORTB
#define SQUAWK_RLD_PORTC PORTC
*/

extern void squawk_playroutine() asm("squawk_playroutine");

// SAMPLE GRINDER
// generates samples and updates oscillators
// uses 132 cycles (not counting playroutine)
//     ~1/3 CPU @ 44kHz on 16MHz
#define SQUAWK_CONSTRUCT_ISR(TARGET_REGISTER) \
uint16_t cia, cia_count; \
intptr_t squawk_register = (intptr_t)&TARGET_REGISTER; \
ISR(TIMER1_COMPA_vect, ISR_NAKED) { \
  asm volatile( \
    "push r2                                          " "\n\t" \
    "in   r2,                    __SREG__             " "\n\t" \
    "push r18                                         " "\n\t" \
    "push r27                                         " "\n\t" \
    "push r26                                         " "\n\t" \
    "push r0                                          " "\n\t" \
    "push r1                                          " "\n\t" \
\
    "lds  r18,                   osc+2*%[mul]+%[fre]  " "\n\t" \
    "lds  r0,                    osc+2*%[mul]+%[pha]  " "\n\t" \
    "add  r0,                    r18                  " "\n\t" \
    "sts  osc+2*%[mul]+%[pha],   r0                   " "\n\t" \
    "lds  r18,                   osc+2*%[mul]+%[fre]+1" "\n\t" \
    "lds  r1,                    osc+2*%[mul]+%[pha]+1" "\n\t" \
    "adc  r1,                    r18                  " "\n\t" \
    "sts  osc+2*%[mul]+%[pha]+1, r1                   " "\n\t" \
\
    "mov  r27,                   r1                   " "\n\t" \
    "sbrc r27,                   7                    " "\n\t" \
    "com  r27                                         " "\n\t" \
    "lsl  r27                                         " "\n\t" \
    "lds  r26,                   osc+2*%[mul]+%[vol]  " "\n\t" \
    "subi r27,                   128                  " "\n\t" \
    "muls r27,                   r26                  " "\n\t" \
    "lsl  r1                                          " "\n\t" \
    "mov  r26,                   r1                   " "\n\t" \
\
    "lds  r18,                   osc+0*%[mul]+%[fre]  " "\n\t" \
    "lds  r0,                    osc+0*%[mul]+%[pha]  " "\n\t" \
    "add  r0,                    r18                  " "\n\t" \
    "sts  osc+0*%[mul]+%[pha],   r0                   " "\n\t" \
    "lds  r18,                   osc+0*%[mul]+%[fre]+1" "\n\t" \
    "lds  r1,                    osc+0*%[mul]+%[pha]+1" "\n\t" \
    "adc  r1,                    r18                  " "\n\t" \
    "sts  osc+0*%[mul]+%[pha]+1, r1                   " "\n\t" \
\
    "mov  r18,                   r1                   " "\n\t" \
    "lsl  r18                                         " "\n\t" \
    "and  r18,                   r1                   " "\n\t" \
    "lds  r27,                   osc+0*%[mul]+%[vol]  " "\n\t" \
    "sbrc r18,                   7                    " "\n\t" \
    "neg  r27                                         " "\n\t" \
    "add  r26,                   r27                  " "\n\t" \
\
    "lds  r18,                   osc+1*%[mul]+%[fre]  " "\n\t" \
    "lds  r0,                    osc+1*%[mul]+%[pha]  " "\n\t" \
    "add  r0,                    r18                  " "\n\t" \
    "sts  osc+1*%[mul]+%[pha],   r0                   " "\n\t" \
    "lds  r18,                   osc+1*%[mul]+%[fre]+1" "\n\t" \
    "lds  r1,                    osc+1*%[mul]+%[pha]+1" "\n\t" \
    "adc  r1,                    r18                  " "\n\t" \
    "sts  osc+1*%[mul]+%[pha]+1, r1                   " "\n\t" \
\
    "lds  r27,                   osc+1*%[mul]+%[vol]  " "\n\t" \
    "sbrc r1,                    7                    " "\n\t" \
    "neg  r27                                         " "\n\t" \
    "add  r26,                   r27                  " "\n\t" \
\
    "ldi  r27,                   1                    " "\n\t" \
    "lds  r0,                    osc+3*%[mul]+%[fre]  " "\n\t" \
    "lds  r1,                    osc+3*%[mul]+%[fre]+1" "\n\t" \
    "add  r0,                    r0                   " "\n\t" \
    "adc  r1,                    r1                   " "\n\t" \
    "sbrc r1,                    7                    " "\n\t" \
    "eor  r0,                    r27                  " "\n\t" \
    "sbrc r1,                    6                    " "\n\t" \
    "eor  r0,                    r27                  " "\n\t" \
    "sts  osc+3*%[mul]+%[fre],   r0                   " "\n\t" \
    "sts  osc+3*%[mul]+%[fre]+1, r1                   " "\n\t" \
\
    "lds  r27,                   osc+3*%[mul]+%[vol]  " "\n\t" \
    "sbrc r1,                    7                    " "\n\t" \
    "neg  r27                                         " "\n\t" \
    "add  r26,                   r27                  " "\n\t" \
\
    "lds  r27,                   pcm                  " "\n\t" \
    "add  r26,                   r27                  " "\n\t" \
    "sts  %[reg],                r26                  " "\n\t" \
\
	  "lds  r27,                   cia_count+1          " "\n\t" \
	  "lds  r26,                   cia_count            " "\n\t" \
	  "sbiw r26,                   1                    " "\n\t" \
	  "breq call_playroutine                            " "\n\t" \
	  "sts  cia_count+1,           r27                  " "\n\t" \
	  "sts  cia_count,             r26                  " "\n\t" \
    "pop  r1                                          " "\n\t" \
    "pop  r0                                          " "\n\t" \
    "pop  r26                                         " "\n\t" \
    "pop  r27                                         " "\n\t" \
    "pop  r18                                         " "\n\t" \
    "out  __SREG__,              r2                   " "\n\t" \
    "pop  r2                                          " "\n\t" \
	  "reti                                             " "\n\t" \
    "call_playroutine:                                " "\n\t" \
\
	  "lds  r27, cia+1                                  " "\n\t" \
	  "lds  r26, cia                                    " "\n\t" \
	  "sts  cia_count+1,           r27                  " "\n\t" \
	  "sts  cia_count,             r26                  " "\n\t" \
\
    "sei                                              " "\n\t" \
	  "push r19                                         " "\n\t" \
	  "push r20                                         " "\n\t" \
	  "push r21                                         " "\n\t" \
	  "push r22                                         " "\n\t" \
	  "push r23                                         " "\n\t" \
	  "push r24                                         " "\n\t" \
	  "push r25                                         " "\n\t" \
	  "push r30                                         " "\n\t" \
	  "push r31                                         " "\n\t" \
\
    "clr  r1                                          " "\n\t" \
    "call squawk_playroutine                          " "\n\t" \
\
	  "pop  r31                                         " "\n\t" \
	  "pop  r30                                         " "\n\t" \
	  "pop  r25                                         " "\n\t" \
	  "pop  r24                                         " "\n\t" \
	  "pop  r23                                         " "\n\t" \
	  "pop  r22                                         " "\n\t" \
	  "pop  r21                                         " "\n\t" \
	  "pop  r20                                         " "\n\t" \
	  "pop  r19                                         " "\n\t" \
\
    "pop  r1                                          " "\n\t" \
    "pop  r0                                          " "\n\t" \
    "pop  r26                                         " "\n\t" \
    "pop  r27                                         " "\n\t" \
    "pop  r18                                         " "\n\t" \
    "out  __SREG__,              r2                   " "\n\t" \
    "pop  r2                                          " "\n\t" \
	  "reti                                             " "\n\t" \
    : \
    : [reg] "M" _SFR_MEM_ADDR(TARGET_REGISTER), \
      [mul] "M" (sizeof(Oscillator)), \
      [pha] "M" (offsetof(Oscillator, phase)), \
      [fre] "M" (offsetof(Oscillator, freq)), \
      [vol] "M" (offsetof(Oscillator, vol)) \
  ); \
}

#endif