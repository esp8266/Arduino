//*********************************************/
// 
//  File:   debug.h
// 
//  Author:  Domenico La Fauci
// 
//********************************************/


#ifndef Debug_H
#define Debug_H

#include <stdio.h>
#include <string.h>

#define INFO_0 1
#define INFO_1 2
#define INFO_2 4
#define INFO_3 8
#define INFO_4 16
#define INFO_5 32
#define INFO_D (1<<0xD)     // Debug
#define INFO_E (1<<0xE)     // Error
#define INFO_F (1<<0xF)     // Warning

#define PRINT_FILE_LINE() do { 						\
		Serial.print("[");Serial.print(__FILE__);		\
		Serial.print("::");Serial.print(__LINE__);Serial.print("]");\
}while (0);

#ifdef _DEBUG_

#define INFO1(x) do { PRINT_FILE_LINE() Serial.print("-W-");\
		Serial.println(x);    			\
}while (0);


#define INFO2(x,y) do { PRINT_FILE_LINE() Serial.print("-I-");\
		Serial.print(x);Serial.print(",");Serial.println(y); \
}while (0);


#else
#define INFO1(x) do {} while(0);
#define INFO(format, args...) do {} while(0);
#endif

#define WARN(args) do { PRINT_FILE_LINE()			\
		Serial.print("-W-"); Serial.println(args);	\
}while (0);

#define DBG_PIN2 3
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

#endif
