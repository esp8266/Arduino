/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * More advanced AVR demonstration.  Controls a LED attached to OCR1A.
 * The brightness of the LED is controlled with the PWM.  A number of
 * methods are implemented to control that PWM.
 *
 * $Id: largedemo.c,v 1.3 2007/01/19 22:17:10 joerg_wunsch Exp $
 */

#include <stdint.h>
#include <stdlib.h>

#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/sleep.h>
#include <avr/wdt.h>

/* Part 1: Macro definitions */

#define CONTROL_PORT PORTD
#define CONTROL_DDR  DDRD

#if defined(__AVR_ATtiny2313__)
/* no PD7 and no ADC available on ATtiny2313 */
#  define TRIGGER_DOWN PD2
#  define TRIGGER_UP   PD3
#  define FLASH	       PD4
#  define CLOCKOUT     PD6
#else
#  define TRIGGER_DOWN PD2
#  define TRIGGER_UP   PD3
#  define TRIGGER_ADC  PD4
#  define CLOCKOUT     PD6
#  define FLASH	       PD7
#endif

#if defined(__AVR_ATmega16__)
#  define PWMDDR     DDRD
#  define PWMOUT     PD5
#elif defined(__AVR_ATmega8__) || defined(__AVR_ATmega48__) ||\
      defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__)
#  define PWMDDR     DDRB
#  define PWMOUT     PB1
#elif defined(__AVR_ATtiny2313__)
#  define PWMDDR     DDRB
#  define PWMOUT     PB3
#  define HAVE_ADC   0
#  define USART_RXC_vect USART_RX_vect
#  define MCUCSR     MCUSR
#else
#  error "Unsupported MCU type"
#endif

#if defined(__AVR_ATmega48__) || defined(__AVR_ATmega88__) ||\
    defined(__AVR_ATmega168__)
/* map ATmega8/16 names to ATmegaX8 names */
#  define USART_RXC_vect USART_RX_vect
#  define UDR     UDR0
#  define UCSRA   UCSR0A
#  define UCSRB   UCSR0B
#  define FE      FE0
#  define TXEN    TXEN0
#  define RXEN    RXEN0
#  define RXCIE   RXCIE0
#  define UDRE    UDRE0
#  define U2X     U2X0
#  define UBRRL   UBRR0L

#  define TIMSK   TIMSK1
#  define MCUCSR  MCUSR
#endif

#if !defined(HAVE_ADC)
#  define HAVE_ADC 1
#endif

#define F_CPU 1000000UL	/* CPU clock in Hertz */

#define SOFTCLOCK_FREQ 100	/* internal software clock */

/*
 * Timeout to wait after last PWM change till updating the EEPROM.
 * Measured in internal clock ticks (approx. 100 Hz).
 */
#define EE_UPDATE_TIME (3 * SOFTCLOCK_FREQ) /* ca. 3 seconds */

/*
 * Timer1 overflow interrupt will be called with F_CPU / 2048
 * frequency.  This interrupt routine further divides that value,
 * resulting in an internal update interval of approx. 10 ms.
 * (The complicated looking scaling by 10 / addition of 9 is
 * poor man's fixed-point rounding algorithm...)
 */
#define TMR1_SCALE ((F_CPU * 10) / (2048UL * SOFTCLOCK_FREQ) + 9) / 10

/* Part 2: Variable definitions */

/*
 * Bits that are set inside interrupt routines, and watched outside in
 * the program's main loop.
 */
volatile struct
{
  uint8_t tmr_int: 1;
  uint8_t adc_int: 1;
  uint8_t rx_int: 1;
}
intflags;

/*
 * Last character read from the UART.
 */
volatile char rxbuff;

/*
 * Last value read from ADC.
 */
volatile uint16_t adcval;

/*
 * Where to store the PWM value in EEPROM.  This is used in order
 * to remember the value across a RESET or power cycle.
 */
uint16_t ee_pwm __attribute__((section(".eeprom"))) = 42;

/*
 * Current value of the PWM.
 */
int16_t pwm;

/*
 * EEPROM backup timer.  Bumped by the PWM update routine.  If it
 * expires, the current PWM value will be written to EEPROM.
 */
int16_t pwm_backup_tmr;

/*
 * Mirror of the MCUCSR register, taken early during startup.
 */
uint8_t mcucsr __attribute__((section(".noinit")));

/* Part 3: Interrupt service routines */

ISR(TIMER1_OVF_vect)
{
  static uint8_t scaler = TMR1_SCALE;

  if (--scaler == 0)
    {
      scaler = TMR1_SCALE;
      intflags.tmr_int = 1;
    }
}

#if HAVE_ADC
/*
 * ADC conversion complete.  Fetch the 10-bit value, and feed the
 * PWM with it.
 */
ISR(ADC_vect)
{
  adcval = ADCW;
  ADCSRA &= ~_BV(ADIE);		/* disable ADC interrupt */
  intflags.adc_int = 1;
}
#endif /* HAVE_ADC */

/*
 * UART receive interrupt.  Fetch the character received and buffer
 * it, unless there was a framing error.  Note that the main loop
 * checks the received character only once per 10 ms.
 */
ISR(USART_RXC_vect)
{
  uint8_t c;

  c = UDR;
  if (bit_is_clear(UCSRA, FE))
    {
      rxbuff = c;
      intflags.rx_int = 1;
    }
}

/* Part 4: Auxiliary functions */

/*
 * Read out and reset MCUCSR early during startup.
 */
void handle_mcucsr(void)
  __attribute__((section(".init3")))
  __attribute__((naked));
void handle_mcucsr(void)
{
  mcucsr = MCUCSR;
  MCUCSR = 0;
}

/*
 * Do all the startup-time peripheral initializations.
 */
static void
ioinit(void)
{
  uint16_t pwm_from_eeprom;

  /*
   * Set up the 16-bit timer 1.
   *
   * Timer 1 will be set up as a 10-bit phase-correct PWM (WGM10 and
   * WGM11 bits), with OC1A used as PWM output.  OC1A will be set when
   * up-counting, and cleared when down-counting (COM1A1|COM1A0), this
   * matches the behaviour needed by the STK500's low-active LEDs.
   * The timer will runn on full MCU clock (1 MHz, CS10 in TCCR1B).
   */
  TCCR1A = _BV(WGM10) | _BV(WGM11) | _BV(COM1A1) | _BV(COM1A0);
  TCCR1B = _BV(CS10);

  OCR1A = 0;			/* set PWM value to 0 */

  /* enable pull-ups for pushbuttons */
#if HAVE_ADC
  CONTROL_PORT = _BV(TRIGGER_DOWN) | _BV(TRIGGER_UP) | _BV(TRIGGER_ADC);
#else
  CONTROL_PORT = _BV(TRIGGER_DOWN) | _BV(TRIGGER_UP);
#endif

  /*
   * Enable Port D outputs: PD6 for the clock output, PD7 for the LED
   * flasher.  PD1 is UART TxD but not DDRD setting is provided for
   * that, as enabling the UART transmitter will automatically turn
   * this pin into an output.
   */
  CONTROL_DDR = _BV(CLOCKOUT) | _BV(FLASH);

  /*
   * As the location of OC1A differs between supported MCU types, we
   * enable that output separately here.  Note that the DDRx register
   * *might* be the same as CONTROL_DDR above, so make sure to not
   * clobber it.
   */
  PWMDDR |= _BV(PWMOUT);

  UCSRA = _BV(U2X);		/* improves baud rate error @ F_CPU = 1 MHz */
  UCSRB = _BV(TXEN)|_BV(RXEN)|_BV(RXCIE); /* tx/rx enable, rx complete intr */
  UBRRL = (F_CPU / (8 * 9600UL)) - 1;  /* 9600 Bd */

#if HAVE_ADC
  /*
   * enable ADC, select ADC clock = F_CPU / 8 (i.e. 125 kHz)
   */
  ADCSRA = _BV(ADEN) | _BV(ADPS1) | _BV(ADPS0);
#endif

  TIMSK = _BV(TOIE1);
  sei();			/* enable interrupts */

  /*
   * Enable the watchdog with the largest prescaler.  Will cause a
   * watchdog reset after approximately 2 s @ Vcc = 5 V
   */
  wdt_enable(WDTO_2S);

  /*
   * Read the value from EEPROM.  If it is not 0xffff (erased cells),
   * use it as the starting value for the PWM.
   */
  if ((pwm_from_eeprom = eeprom_read_word(&ee_pwm)) != 0xffff)
    OCR1A = (pwm = pwm_from_eeprom);
}

/*
 * Some simple UART IO functions.
 */

/*
 * Send character c down the UART Tx, wait until tx holding register
 * is empty.
 */
static void
putchr(char c)
{

  loop_until_bit_is_set(UCSRA, UDRE);
  UDR = c;
}

/*
 * Send a C (NUL-terminated) string down the UART Tx.
 */
static void
printstr(const char *s)
{

  while (*s)
    {
      if (*s == '\n')
	putchr('\r');
      putchr(*s++);
    }
}

/*
 * Same as above, but the string is located in program memory,
 * so "lpm" instructions are needed to fetch it.
 */
static void
printstr_p(const char *s)
{
  char c;

  for (c = pgm_read_byte(s); c; ++s, c = pgm_read_byte(s))
    {
      if (c == '\n')
	putchr('\r');
      putchr(c);
    }
}

/*
 * Update the PWM value.  If it has changed, send the new value down
 * the serial line.
 */
static void
set_pwm(int16_t new)
{
  char s[8];

  if (new < 0)
    new = 0;
  else if (new > 1000)
    new = 1000;

  if (new != pwm)
    {
      OCR1A = (pwm = new);

      /*
       * Calculate a "percentage".  We just divide by 10, as we
       * limited the max value of the PWM to 1000 above.
       */
      new /= 10;
      itoa(new, s, 10);
      printstr(s);
      putchr(' ');

      pwm_backup_tmr = EE_UPDATE_TIME;
    }
}

/* Part 5: main() */

int
main(void)
{
  /*
   * Our modus of operation.  MODE_UPDOWN means we watch out for
   * either PD2 or PD3 being low, and increase or decrease the
   * PWM value accordingly.  This is the default.
   * MODE_ADC means the PWM value follows the value of ADC0 (PA0).
   * This is enabled by applying low level to PC1.
   * MODE_SERIAL means we get commands via the UART.  This is
   * enabled by sending a valid V.24 character at 9600 Bd to the
   * UART.
   */
  enum
  {
    MODE_UPDOWN,
    MODE_ADC,
    MODE_SERIAL
  } __attribute__((packed)) mode = MODE_UPDOWN;
  uint8_t flash = 0;

  ioinit();

  if ((mcucsr & _BV(WDRF)) == _BV(WDRF))
    printstr_p(PSTR("\nOoops, the watchdog bit me!"));

  printstr_p(PSTR("\nHello, this is the avr-gcc/libc "
		  "demo running on an "
#if defined(__AVR_ATmega16__)
		  "ATmega16"
#elif defined(__AVR_ATmega8__)
		  "ATmega8"
#elif defined(__AVR_ATmega48__)
		  "ATmega48"
#elif defined(__AVR_ATmega88__)
		  "ATmega88"
#elif defined(__AVR_ATmega168__)
		  "ATmega168"
#elif defined(__AVR_ATtiny2313__)
		  "ATtiny2313"
#else
		  "unknown AVR"
#endif
		  "\n"));

  for (;;)
    {
      wdt_reset();

      if (intflags.tmr_int)
	{
	  /*
	   * Our periodic 10 ms interrupt happened.  See what we can
	   * do about it.
	   */
	  intflags.tmr_int = 0;
	  /*
	   * toggle PD6, just to show the internal clock; should
	   * yield ~ 48 Hz on PD6
	   */
	  CONTROL_PORT ^= _BV(CLOCKOUT);
	  /*
	   * flash LED on PD7, approximately once per second
	   */
	  flash++;
	  if (flash == 5)
	    CONTROL_PORT |= _BV(FLASH);
	  else if (flash == 100)
	    {
	      flash = 0;
	      CONTROL_PORT &= ~_BV(FLASH);
	    }

	  switch (mode)
	    {
	    case MODE_SERIAL:
	      /*
	       * In serial mode, there's nothing to do anymore here.
	       */
	      break;

	    case MODE_UPDOWN:
	      /*
	       * Query the pushbuttons.
	       *
	       * NB: watch out to use PINx for reading, as opposed
	       * to using PORTx which would be the mirror of the
	       * _output_ latch register (resp. pullup configuration
	       * bit for input pins)!
	       */
	      if (bit_is_clear(PIND, TRIGGER_DOWN))
		set_pwm(pwm - 10);
	      else if (bit_is_clear(PIND, TRIGGER_UP))
		set_pwm(pwm + 10);
#if HAVE_ADC
	      else if (bit_is_clear(PIND, TRIGGER_ADC))
		mode = MODE_ADC;
#endif
	      break;

	    case MODE_ADC:
#if HAVE_ADC
	      if (bit_is_set(PIND, TRIGGER_ADC))
		mode = MODE_UPDOWN;
	      else
		{
		  /*
		   * Start one conversion.
		   */
		  ADCSRA |= _BV(ADIE);
		  ADCSRA |= _BV(ADSC);
		}
#endif /* HAVE_ADC */
	      break;
	    }

	  if (pwm_backup_tmr && --pwm_backup_tmr == 0)
	    {
	      /*
	       * The EEPROM backup timer expired.  Save the current
	       * PWM value in EEPROM.  Note that this function might
	       * block for a few milliseconds (after writing the
	       * first byte).
	       */
	      eeprom_write_word(&ee_pwm, pwm);
	      printstr_p(PSTR("[EEPROM updated] "));
	    }
	}

#if HAVE_ADC
      if (intflags.adc_int)
	{
	  intflags.adc_int = 0;
	  set_pwm(adcval);
	}
#endif /* HAVE_ADC */

      if (intflags.rx_int)
	{
	  intflags.rx_int = 0;

	  if (rxbuff == 'q')
	    {
	      printstr_p(PSTR("\nThank you for using serial mode."
			      "  Good-bye!\n"));
	      mode = MODE_UPDOWN;
	    }
	  else
	    {
	      if (mode != MODE_SERIAL)
		{
		  printstr_p(PSTR("\nWelcome at serial control, "
				  "type +/- to adjust, or 0/1 to turn on/off\n"
				  "the LED, q to quit serial mode, "
				  "r to demonstrate a watchdog reset\n"));
		  mode = MODE_SERIAL;
		}
	      switch (rxbuff)
		{
		case '+':
		  set_pwm(pwm + 10);
		  break;

		case '-':
		  set_pwm(pwm - 10);
		  break;

		case '0':
		  set_pwm(0);
		  break;

		case '1':
		  set_pwm(1000);
		  break;

		case 'r':
		  printstr_p(PSTR("\nzzzz... zzz..."));
		  for (;;)
		    ;
		}
	    }
	}
      sleep_mode();
    }
}
