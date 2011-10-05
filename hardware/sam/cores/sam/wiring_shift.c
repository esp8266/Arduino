/* %arduino_license% */

#include "Arduino.h"

#ifdef __cplusplus
extern "C"{
#endif

extern uint32_t shiftIn( uint32_t ulDataPin, uint32_t ulClockPin, uint32_t ulBitOrder )
{
	uint8_t value = 0 ;
	uint8_t i ;

	for ( i=0 ; i < 8 ; ++i )
    {
		digitalWrite( ulClockPin, HIGH ) ;

		if ( ulBitOrder == LSBFIRST )
        {
			value |= digitalRead( ulDataPin ) << i ;
        }
		else
        {
			value |= digitalRead( ulDataPin ) << (7 - i) ;
        }

		digitalWrite( ulClockPin, LOW ) ;
	}

	return value ;
}

extern void shiftOut( uint32_t ulDataPin, uint32_t ulClockPin, uint32_t ulBitOrder, uint32_t ulVal )
{
	uint8_t i ;

	for ( i=0 ; i < 8 ; i++ )
    {
		if ( ulBitOrder == LSBFIRST )
        {
			digitalWrite( ulDataPin, !!(ulVal & (1 << i)) ) ;
        }
		else	
        {
			digitalWrite( ulDataPin, !!(ulVal & (1 << (7 - i))) ) ;
        }

		digitalWrite( ulClockPin, HIGH ) ;
		digitalWrite( ulClockPin, LOW ) ;		
	}
}

#ifdef __cplusplus
} // extern "C"
#endif
