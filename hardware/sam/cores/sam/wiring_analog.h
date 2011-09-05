#ifndef _WIRING_ANALOG_
#define _WIRING_ANALOG_

#ifdef __cplusplus
 extern "C" {
#endif

/*
 * \brief
 */
extern void analogReference( uint8_t mode ) ;

/*
 * \brief
 */
extern void analogWrite( uint8_t, int ) ;

/*
 * \brief
 */
extern int analogRead( uint8_t ) ;


#ifdef __cplusplus
}
#endif

#endif /* _WIRING_ANALOG_ */
