//*********************************************/
// 
//  File:   debug.h
// 
//  Author: dlf (Metodo2 srl)
// 
//********************************************/


#ifndef Debug_H
#define Debug_H

#include <stdio.h>
#include <string.h>

#define PRINT_FILE_LINE() do { 						\
		Serial.print("[");Serial.print(__FILE__);		\
		Serial.print("::");Serial.print(__LINE__);Serial.print("]");\
}while (0);

#ifdef _DEBUG_

#define INFO(format, args...) do { \
	char buf[250];	\
	sprintf(buf, format, args); \
	Serial.println(buf); \
} while(0);

#define INFO1(x) do { PRINT_FILE_LINE() Serial.print("-I-");\
		Serial.println(x);    			\
}while (0);


#define INFO2(x,y) do { PRINT_FILE_LINE() Serial.print("-I-");\
		Serial.print(x,16);Serial.print(",");Serial.println(y,16); \
}while (0);


#else
#define INFO1(x) do {} while(0);
#define INFO2(x,y) do {} while(0);
#define INFO(format, args...) do {} while(0);
#endif

#if 0
#define WARN(args) do { PRINT_FILE_LINE()			\
		Serial.print("-W-"); Serial.println(args);	\
}while (0);
#else
#define WARN(args) do {} while (0);
#endif

#if _DEBUG_SPI_
#define DBG_PIN2 5
#define DBG_PIN 4

#define START()         digitalWrite(DBG_PIN2, HIGH);
#define END()           digitalWrite(DBG_PIN2, LOW);
#define SET_TRIGGER()   digitalWrite(DBG_PIN, HIGH);
#define RST_TRIGGER()   digitalWrite(DBG_PIN, LOW);

#define INIT_TRIGGER()  pinMode(DBG_PIN, OUTPUT); \
                        pinMode(DBG_PIN2, OUTPUT); \
                        RST_TRIGGER()
#define TOGGLE_TRIGGER() SET_TRIGGER() \
                           delayMicroseconds(2);    \
                               RST_TRIGGER()
#else
#define START()
#define END()
#define SET_TRIGGER()
#define RST_TRIGGER()
#define INIT_TRIGGER()
#define TOGGLE_TRIGGER()
#endif

#endif
