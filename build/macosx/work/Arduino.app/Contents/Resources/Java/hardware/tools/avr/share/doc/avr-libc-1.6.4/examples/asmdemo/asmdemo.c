/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Joerg Wunsch wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * Demo combining C and assembly source files.
 *
 * This demo implements an RC model type PWM decoder.  The incoming
 * PWM signal consists of a pulse sequence with a pulse width of 920
 * microseconds up to 2120 microseconds (1520 microseconds being the
 * neutral point).  Depending on the value of the decoded incoming
 * PWM, an outgoing PWM is controlled between 0 and 100 %.
 *
 * The project is intented to be run on an ATtiny13 that has only one
 * timer channel (timer 0), so both the incoming signal discrimination
 * as well as the outgoing PWM need to run on the same timer.
 *
 * For verification purposes, the same project can also be run on an
 * ATtiny25/45/85, where timer 1 can be used to evaluate the incoming
 * PWM signal, and timer 0 to generate the outgoing PWM.  In that
 * case, no additional assembly code is needed.
 *
 * $Id: asmdemo.c,v 1.1 2006/08/29 19:45:06 joerg_wunsch Exp $
 */

/*
 * This is the main C source file for the demo.
 */
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/sleep.h>

#include "project.h"

volatile uint16_t pwm_incoming;
volatile struct
{
  uint8_t pwm_received: 1;
}
intbits;

void
ioinit(void)
{
  counter_hi = 0;
  flags = 0;

  /*
   * Timer 0 runs as phase-correct PWM at full clock, OC0B connects to
   * the PWM engine.
   */
  TCCR0A = (1 << COM0B1) | (1 << WGM00);
  TCCR0B = (1 << CS00);
  OCR0A = 255;

#if defined(__AVR_ATtiny13__)
  TIMSK0 = (1 << TOIE0) | (1 << OCIE0A);

# define F_CPU 1200000ul
  /* Minimal PWM pulse width is 920 us. */
# define MIN_PWM_VAL ((920ul * F_CPU) / 1000000ul)
  /* Maximal PWM pulse width is 2120 us */
# define MAX_PWM_VAL ((2120ul * F_CPU) / 1000000ul)

#elif defined(__AVR_ATtiny25__) ||\
	defined(__AVR_ATtiny45__) ||\
	defined(__AVR_ATtiny85__)

# define F_CPU 1000000ul
  /*
   * We use a prescaler of 16 here to avoid the 32-bit calculations
   * below.
   */
  /* Minimal PWM pulse width is 920 us. */
# define MIN_PWM_VAL ((920ul * F_CPU) / 16 / 1000000ul)
  /* Maximal PWM pulse width is 2120 us */
# define MAX_PWM_VAL ((2120ul * F_CPU) / 16 / 1000000ul)

#else
# error "Don't know how to run on your MCU_TYPE."
#endif

  PCMSK = (1 << 4);
  GIFR = (1 << PCIF);
  GIMSK = (1 << PCIE);

  DDRB = (1 << PB1);
  PORTB = 0;

  sei();
}

#if defined(__AVR_ATtiny25__) ||\
	defined(__AVR_ATtiny45__) ||\
	defined(__AVR_ATtiny85__)
ISR(PCINT0_vect)
{
  uint8_t tcnt1;

  if (PINB & (1 << 4))
    {
      /* Start timer 1 with a prescaler of 16. */
      TCNT1 = 0;
      TCCR1 = (1 << CS12) | (1 << CS10);
      return;
    }

  /* Stop timer 1, current value is pulse width. */
  tcnt1 = TCNT1;
  TCCR1 = 0;
  GIMSK &= ~(1 << PCIE);

  pwm_incoming = tcnt1;
  intbits.pwm_received = 1;
}
#endif /* ATtinyX5 */

int
main(void)
{

  ioinit();

  for (;;)
    {
      if (intbits.pwm_received)
	{
	  intbits.pwm_received = 0;
#if defined(__AVR_ATtiny13__)
	  if (pwm_incoming < MIN_PWM_VAL)
	    pwm_incoming = MIN_PWM_VAL;
	  else if (pwm_incoming > MAX_PWM_VAL)
	    pwm_incoming = MAX_PWM_VAL;
	  OCR0B = (pwm_incoming - MIN_PWM_VAL) * 255ul / (MAX_PWM_VAL - MIN_PWM_VAL);
#else
	  OCR0B = (pwm_incoming - MIN_PWM_VAL) * 255u / (MAX_PWM_VAL - MIN_PWM_VAL);
#endif
	  GIFR = (1 << PCIF);
	  GIMSK |= (1 << PCIE);
	}
      sleep_mode();
    }
}
