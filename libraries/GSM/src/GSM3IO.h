#ifdef TTOPEN_V1
	#define __POWERPIN__ 5
	#define __RESETPIN__ 6
#else
	#define __RESETPIN__ 7
#endif

#if defined(__AVR_ATmega328P__) 
	#ifdef TTOPEN_V1
		#define __TXPIN__ 3
		#define __RXPIN__ 4
		#define __RXINT__ 3
	#else
		#define __TXPIN__ 3
		#define __RXPIN__ 2
		#define __RXINT__ 3
	#endif
#elif defined(__AVR_ATmega2560__) || defined(__AVR_ATmega1280__)
	#define __TXPIN__ 3
	#define __RXPIN__ 10
	#define __RXINT__ 4
#elif defined(__AVR_ATmega32U4__)
	#define __TXPIN__ 3
	#define __RXPIN__ 8
	#define __RXINT__ 3
#endif