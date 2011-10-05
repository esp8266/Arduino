/* %arduino_license% */

#ifndef _WIRING_PULSE_
#define _WIRING_PULSE_

#ifdef __cplusplus
 extern "C" {
#endif

/*
 * \brief Measures the length (in microseconds) of a pulse on the pin; state is HIGH
 * or LOW, the type of pulse to measure.  Works on pulses from 2-3 microseconds
 * to 3 minutes in length, but must be called at least a few dozen microseconds
 * before the start of the pulse.
 */
extern uint32_t pulseIn( uint32_t ulPin, uint32_t ulState, uint32_t ulTimeout = 1000000L ) ;


#ifdef __cplusplus
}
#endif

#endif /* _WIRING_PULSE_ */
