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
 * $Id: isrs.S,v 1.1 2006/08/29 19:45:06 joerg_wunsch Exp $
 */
/*
 * This file contains the interrupt service routine implementations
 * when compiling the project for the ATtiny13 target.
 */

#include <avr/io.h>

#include "project.h"

#if defined(__AVR_ATtiny13__)

/*
 * Timer 0 hit TOP (0xff), i.e. it turns from up-counting
 * into down-counting direction.
 */
.global TIM0_COMPA_vect
TIM0_COMPA_vect:
	in	sreg_save, _SFR_IO_ADDR(SREG)
	inc	counter_hi
	clr	flags
	out	_SFR_IO_ADDR(SREG), sreg_save
	reti

/*
 * Timer 0 hit BOTTOM (0x00), i.e. it turns from down-counting
 * into up-counting direction.
 */
.global	TIM0_OVF_vect
TIM0_OVF_vect:
	in	sreg_save, _SFR_IO_ADDR(SREG)
	inc	counter_hi
	ser	flags
	out	_SFR_IO_ADDR(SREG), sreg_save
	reti

;;; one 16-bit word to store our rising edge's timestamp
.lcomm	starttime.0, 2

.extern	pwm_incoming
.extern	intbits

.global PCINT0_vect
PCINT0_vect:
	in	sreg_save, _SFR_IO_ADDR(SREG)

	;; save our working registers
	push	r18
	push	r19
	push	r20
	push	r21

	;; Now that we are ready to fetch the current
	;; value of TCNT0, allow interrupts for a
	;; moment.  As the effect of the SEI will be
	;; deferred by one instruction, any possible
	;; rollover of TCNT0 (hitting BOTTOM when
	;; counting down, or MAX when counting up) will
	;; allow the above ISRs to trigger right here,
	;; and update their status, so our combined
	;; 16-bit time from [counter_hi, TCNT0] will
	;; be correct.
	sei
	in	r20, _SFR_IO_ADDR(TCNT0)
	cli
	;; Now, make our working copy of the status,
	;; so we can re-enable interrupts again.
	mov	r21, counter_hi
	mov	r19, flags
	sei

	;; what direction were we counting?
	sbrs	r19, 0
	;; we are down-counting, invert TCNT0
	com	r20
	;; at this point, r21:20 has our current
	;; 16-bit time

	;; now, look which of the edges triggered
	;; our pin-change interrupt
	sbis	_SFR_IO_ADDR(PINB), 4
	rjmp	10f
	;; rising edge detected, just record starttime
	sts	(starttime.0) + 1, r21
	sts	starttime.0, r20
	rjmp	99f		; we are done here

	;; Falling edge: compute pulse width, store it
	;; into pwm_incoming, disable pin-change
	;; interrupt until the upper layers had a chance
	;; to fetch the result.

10:	in	r18, _SFR_IO_ADDR(GIMSK)
	andi	r18, ~(1 << PCIE)
	out	_SFR_IO_ADDR(GIMSK), r18

	;; pwm_incoming = current_time - starttime
	lds	r19, (starttime.0) + 1
	lds	r18, starttime.0
	sub	r20, r18
	sbc	r21, r19
	sts	(pwm_incoming) + 1, r21
	sts	pwm_incoming, r20

	;; signal upper layer
	lds	r18, intbits
	ori	r18, 1
	sts	intbits, r18
99:
	pop	r21
	pop	r20
	pop	r19
	pop	r18

	out	_SFR_IO_ADDR(SREG), sreg_save
	reti
#endif  /* ATtiny13 */
