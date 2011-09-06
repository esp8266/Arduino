#define ARDUINO_MAIN
#include "Arduino.h"

/*
 * \brief Main entry point of Arduino application
 */
int main( void )
{
	init() ;

	setup() ;

	for ( ; ; )
    {
		loop() ;
    }

//	return 0 ;
}
