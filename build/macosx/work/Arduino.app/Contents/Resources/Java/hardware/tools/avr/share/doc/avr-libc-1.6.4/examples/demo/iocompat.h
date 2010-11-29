/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
 * ----------------------------------------------------------------------------
 *
 * IO feature compatibility definitions for various AVRs.
 *
 * $Id: iocompat.h,v 1.6.2.1 2008/03/17 22:08:46 joerg_wunsch Exp $
 */

#if !defined(IOCOMPAT_H)
#define IOCOMPAT_H 1

/*
 * Device-specific adjustments:
 *
 * Supply definitions for the location of the OCR1[A] port/pin, the
 * name of the OCR register controlling the PWM, and adjust interrupt
 * vector names that differ from the one used in demo.c
 * [TIMER1_OVF_vect].
 */
#if defined(__AVR_AT90S2313__)
#  define OC1 PB3
#  define OCR OCR1
#  define DDROC DDRB
#  define TIMER1_OVF_vect TIMER1_OVF1_vect
#elif defined(__AVR_AT90S2333__) || defined(__AVR_AT90S4433__)
#  define OC1 PB1
#  define DDROC DDRB
#  define OCR OCR1
#elif defined(__AVR_AT90S4414__) || defined(__AVR_AT90S8515__) || \
      defined(__AVR_AT90S4434__) || defined(__AVR_AT90S8535__) || \
      defined(__AVR_ATmega163__) || defined(__AVR_ATmega8515__) || \
      defined(__AVR_ATmega8535__) || \
      defined(__AVR_ATmega164P__) || defined(__AVR_ATmega324P__) || \
      defined(__AVR_ATmega644__) || defined(__AVR_ATmega644P__) || \
      defined(__AVR_ATmega1284P__)
#  define OC1 PD5
#  define DDROC DDRD
#  define OCR OCR1A
#  if !defined(TIMSK)		/* new ATmegas */
#    define TIMSK TIMSK1
#  endif
#elif defined(__AVR_ATmega8__) || defined(__AVR_ATmega48__) || \
      defined(__AVR_ATmega88__) || defined(__AVR_ATmega168__)
#  define OC1 PB1
#  define DDROC DDRB
#  define OCR OCR1A
#  if !defined(TIMSK)		/* ATmega48/88/168 */
#    define TIMSK TIMSK1
#  endif /* !defined(TIMSK) */
#elif defined(__AVR_ATtiny2313__)
#  define OC1 PB3
#  define OCR OCR1A
#  define DDROC DDRB
#elif defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || \
      defined(__AVR_ATtiny84__)
#  define OC1 PA6
#  define DDROC DDRA
#  if !defined(OCR1A)
#    /* work around misspelled name in avr-libc 1.4.[0..1] */
#    define OCR OCRA1
#  else
#    define OCR OCR1A
#  endif
#  define TIMSK TIMSK1
#  define TIMER1_OVF_vect TIM1_OVF_vect /* XML and datasheet mismatch */
#elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || \
      defined(__AVR_ATtiny85__)
/* Timer 1 is only an 8-bit timer on these devices. */
#  define OC1 PB1
#  define DDROC DDRB
#  define OCR OCR1A
#  define TCCR1A TCCR1
#  define TCCR1B TCCR1
#  define TIMER1_OVF_vect TIM1_OVF_vect
#  define TIMER1_TOP 255	/* only 8-bit PWM possible */
#  define TIMER1_PWM_INIT _BV(PWM1A) | _BV(COM1A1)
#  define TIMER1_CLOCKSOURCE _BV(CS12) /* use 1/8 prescaler */
#elif defined(__AVR_ATtiny26__)
/* Rather close to ATtinyX5 but different enough for its own section. */
#  define OC1 PB1
#  define DDROC DDRB
#  define OCR OCR1A
#  define TIMER1_OVF_vect TIMER1_OVF1_vect
#  define TIMER1_TOP 255	/* only 8-bit PWM possible */
#  define TIMER1_PWM_INIT _BV(PWM1A) | _BV(COM1A1)
#  define TIMER1_CLOCKSOURCE _BV(CS12) /* use 1/8 prescaler */
/*
 * Without setting OCR1C to TOP, the ATtiny26 does not trigger an
 * overflow interrupt in PWM mode.
 */
#  define TIMER1_SETUP_HOOK() OCR1C = 255
#elif defined(__AVR_ATtiny261__) || defined(__AVR_ATtiny461__) || \
      defined(__AVR_ATtiny861__)
#  define OC1 PB1
#  define DDROC DDRB
#  define OCR OCR1A
#  define TIMER1_PWM_INIT _BV(WGM10) | _BV(PWM1A) | _BV(COM1A1)
/*
 * While timer 1 could be operated in 10-bit mode on these devices,
 * the handling of the 10-bit IO registers is more complicated than
 * that of the 16-bit registers of other AVR devices (no combined
 * 16-bit IO operations possible), so we restrict this demo to 8-bit
 * mode which is pretty standard.
 */
#  define TIMER1_TOP 255
#  define TIMER1_CLOCKSOURCE _BV(CS12) /* use 1/8 prescaler */
#elif defined(__AVR_ATmega32__) || defined(__AVR_ATmega16__)
#  define OC1 PD5
#  define DDROC DDRD
#  define OCR OCR1A
#elif defined(__AVR_ATmega64__) || defined(__AVR_ATmega128__) || \
      defined(__AVR_ATmega165__) || defined(__AVR_ATmega169__) || \
      defined(__AVR_ATmega325__) || defined(__AVR_ATmega3250__) || \
      defined(__AVR_ATmega645__) || defined(__AVR_ATmega6450__) || \
      defined(__AVR_ATmega329__) || defined(__AVR_ATmega3290__) || \
      defined(__AVR_ATmega649__) || defined(__AVR_ATmega6490__) || \
      defined(__AVR_ATmega640__) || \
      defined(__AVR_ATmega1280__) || defined(__AVR_ATmega1281__) || \
      defined(__AVR_ATmega2560__) || defined(__AVR_ATmega2561__)
#  define OC1 PB5
#  define DDROC DDRB
#  define OCR OCR1A
#  if !defined(PB5) 		/* work around missing bit definition */
#    define PB5 5
#  endif
#  if !defined(TIMSK)		/* new ATmegas */
#    define TIMSK TIMSK1
#  endif
#else
#  error "Don't know what kind of MCU you are compiling for"
#endif

/*
 * Map register names for older AVRs here.
 */
#if !defined(COM1A1)
#  define COM1A1 COM11
#endif

#if !defined(WGM10)
#  define WGM10 PWM10
#  define WGM11 PWM11
#endif

/*
 * Provide defaults for device-specific macros unless overridden
 * above.
 */
#if !defined(TIMER1_TOP)
#  define TIMER1_TOP 1023	/* 10-bit PWM */
#endif

#if !defined(TIMER1_PWM_INIT)
#  define TIMER1_PWM_INIT _BV(WGM10) | _BV(WGM11) | _BV(COM1A1)
#endif

#if !defined(TIMER1_CLOCKSOURCE)
#  define TIMER1_CLOCKSOURCE _BV(CS10) /* full clock */
#endif

#endif /* !defined(IOCOMPAT_H) */
