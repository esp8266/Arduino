#ifndef _WIRING_
#define _WIRING_

#ifdef __cplusplus
 extern "C" {
#endif


extern void init( void ) ;

extern uint32_t millis( void ) ;

extern uint32_t micros( void ) ;

extern void delay( uint32_t dwMs ) ;

extern void delayMicroseconds( uint32_t dwUs ) ;


#ifdef __cplusplus
}
#endif

#endif /* _WIRING_ */
