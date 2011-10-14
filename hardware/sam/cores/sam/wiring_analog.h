/* %arduino_license% */

#ifndef _WIRING_ANALOG_
#define _WIRING_ANALOG_

#ifdef __cplusplus
 extern "C" {
#endif

/*
 * \brief SAM3 products have only one reference for ADC
 */
typedef enum _eAnalogReference
{
  AR_DEFAULT,
} eAnalogReference ;

/*
 * \brief Configures the reference voltage used for analog input (i.e. the value used as the top of the input range).
 * This function is kept only for compatibility with existing AVR based API.
 *
 * \param ulMmode Should be set to AR_DEFAULT.
 */
extern void analogReference( eAnalogReference ulMode ) ;

/*
 * \brief Writes an analog value (PWM wave) to a pin.
 *
 * \param ulPin
 * \param ulValue
 */
extern void analogWrite( uint32_t ulPin, uint32_t ulValue ) ;

/*
 * \brief Reads the value from the specified analog pin.
 *
 * \param ulValue
 */
extern uint32_t analogRead( uint32_t ulPin ) ;


#ifdef __cplusplus
}
#endif

#endif /* _WIRING_ANALOG_ */
