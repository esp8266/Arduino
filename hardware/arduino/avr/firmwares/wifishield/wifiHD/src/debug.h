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

#define INFO_INIT_FLAG 	1
#define INFO_TCP_FLAG 	2
#define INFO_SPI_FLAG 	4
#define INFO_CM_FLAG 	8
#define INFO_UTIL_FLAG 16

#define INFO_D (1<<0xD)     // Debug
#define INFO_E (1<<0xE)     // Error
#define INFO_WARN_FLAG (1<<0xF)     // Warning
#define DEFAULT_INFO_FLAG 0 //INFO_WARN_FLAG

#ifdef _DEBUG_
#define DEFINE_DEBUG_VARIABLES()							\
uint16_t enableDebug = DEFAULT_INFO_FLAG | INFO_WARN_FLAG;	\
uint16_t verboseDebug = 0;									\
uint16_t dumpDebug = 0;										\
uint16_t pollDebug = 0;
#else
#define DEFINE_DEBUG_VARIABLES()							\
uint16_t enableDebug = DEFAULT_INFO_FLAG;					\
uint16_t verboseDebug = 0;									\
uint16_t dumpDebug = 0;										\
uint16_t pollDebug = 0;
#endif

#define INIT_DEBUG_VARIABLES()	\
		enableDebug = DEFAULT_INFO_FLAG | INFO_WARN_FLAG;	\
		verboseDebug = 0;									\
		dumpDebug = 0; pollDebug = 0;


#define PRINT_DEBUG_VARIABLES()	\
		printk("Debug 	enabled: 0x%x\n", enableDebug);	\
        printk("Verbose enabled: 0x%x\n", verboseDebug);	\
        printk("Dump 	enabled: 0x%x\n", dumpDebug);	\
        printk("POoll 	enabled: 0x%x\n", pollDebug);

#define TURNON_DEBUG_VARIABLES()	\
		enableDebug = 0xff;

extern uint16_t enableDebug;
extern uint16_t verboseDebug;
extern uint16_t dumpDebug;
extern uint16_t pollDebug;

#define ENABLE_DEBUG_LEVEL 	1
#define VERBOSE_DEBUG_LEVEL 2
#define DUMP_DEBUG_LEVEL 	3
#define POLL_DEBUG_LEVEL 	4

#define CHECK_DEBUG(VAR, LEVEL, LEVEL_LIMIT, FLAG)  		\
	do{										\
		if (LEVEL >= LEVEL_LIMIT) VAR |= FLAG;		\
		else VAR &= ~FLAG;			\
		}while(0);

#define CHECK_ENA_DEBUG(LEVEL, FLAG)	\
		CHECK_DEBUG(enableDebug, LEVEL, ENABLE_DEBUG_LEVEL, FLAG)
#define CHECK_VERB_DEBUG(LEVEL, FLAG) 	\
		CHECK_DEBUG(verboseDebug, LEVEL, VERBOSE_DEBUG_LEVEL, FLAG)
#define CHECK_DUMP_DEBUG(LEVEL, FLAG) 	\
		CHECK_DEBUG(dumpDebug, LEVEL, DUMP_DEBUG_LEVEL, FLAG)
#define CHECK_POLL_DEBUG(LEVEL, FLAG) 	\
		CHECK_DEBUG(pollDebug, LEVEL, POLL_DEBUG_LEVEL, FLAG)


#define CHECK_DEBUG_LEVEL(LEVEL, INFO_FLAG)		\
		CHECK_ENA_DEBUG(LEVEL, INFO_FLAG) 		\
		CHECK_VERB_DEBUG(LEVEL, INFO_FLAG) 		\
		CHECK_DUMP_DEBUG(LEVEL, INFO_FLAG)		\
		CHECK_POLL_DEBUG(LEVEL, INFO_FLAG)

#ifdef _INFO_DEBUG_
#define PRINT_DEBUG(msg, args...) do { 			\
	printk("[%s] " msg , __func__ , ##args );	\
} while (0)

#define INFO_DEBUG(msg, args...) do { 			\
	printk("I-[%s] " msg , __func__ , ##args );	\
} while (0)

#define WARN_DEBUG(msg, args...) do { 			\
	printk("W-[%s] " msg , __func__ , ##args );	\
} while (0)

#else
do { }while(0);
#endif

#define IF_DEBUG(X,Y) do { 			\
if (enableDebug & INFO_##X##_FLAG) \
Y;	\
} while (0)

#define IF_DEBUG_VER(X,Y) do { 			\
if (verboseDebug & INFO_##X##_FLAG) \
Y;	\
} while (0)

#define IF_DEBUG_DUMP(X,Y) do { 			\
if (dumpDebug & INFO_##X##_FLAG) \
Y;	\
} while (0)

#define IF_DEBUG_POLL(X,Y) do { 			\
if (pollDebug & INFO_##X##_FLAG) {\
Y;	\
}} while (0)



#define IF_WARN(Y) IF_DEBUG(WARN,Y)
#define IF_WARN_VER(Y) IF_DEBUG_VER(WARN,Y)
#define IF_TCP(Y) IF_DEBUG(TCP,Y)
#define IF_TCP_VER(Y) IF_DEBUG_VER(TCP,Y)
#define IF_TCP_POLL(Y) IF_DEBUG_POLL(TCP,Y)
#define IF_TCP_DUMP(Y) IF_DEBUG_DUMP(TCP,Y)
#define IF_SPI(Y) IF_DEBUG(SPI,Y)
#define IF_SPI_VER(Y) IF_DEBUG_VER(SPI,Y)
#define IF_SPI_DUMP(Y) IF_DEBUG_DUMP(SPI,Y)
#define IF_SPI_POLL(Y) IF_DEBUG_POLL(SPI,Y)
#define IF_UTIL(Y) IF_DEBUG(UTIL,Y)
#define IF_UTIL_VER(Y) IF_DEBUG_VER(UTIL,Y)

#define WARN(msg, args...)			IF_DEBUG(WARN,WARN_DEBUG(msg, ##args))
#define WARN_VER(msg, args...)		IF_DEBUG_VER(WARN,WARN_DEBUG(msg, ##args))
#define WARN_POLL(msg, args...)		IF_DEBUG_POLL(WARN,WARN_DEBUG(msg, ##args))
#if 0	// disable to reduce the size of binary
#define INFO_INIT(msg, args...) 	IF_DEBUG(INIT,PRINT_DEBUG(msg, ##args))
#define INFO_INIT_VER(msg, args...) IF_DEBUG_VER(INIT,PRINT_DEBUG(msg, ##args))
#else
#define INFO_INIT(msg, args...) 	
#define INFO_INIT_VER(msg, args...) 
#endif
#define INFO_TCP(msg, args...) 		IF_DEBUG(TCP,PRINT_DEBUG(msg, ##args))
#define INFO_TCP_VER(msg, args...) 	IF_DEBUG_VER(TCP,PRINT_DEBUG(msg, ##args))
#define INFO_TCP_DUMP(msg, args...) IF_DEBUG_DUMP(TCP,PRINT_DEBUG(msg, ##args))
#define INFO_TCP_POLL(msg, args...) IF_DEBUG_POLL(TCP,PRINT_DEBUG(msg, ##args))
#define INFO_SPI(msg, args...) 		IF_DEBUG(SPI,PRINT_DEBUG(msg, ##args))
#define INFO_SPI_VER(msg, args...) 	IF_DEBUG_VER(SPI,PRINT_DEBUG(msg, ##args))
#define INFO_SPI_DUMP(msg, args...) IF_DEBUG_DUMP(SPI,PRINT_DEBUG(msg, ##args))
#define INFO_SPI_POLL(msg, args...) IF_DEBUG_POLL(SPI,PRINT_DEBUG(msg, ##args))
#define INFO_UTIL(msg, args...) 	IF_DEBUG(UTIL,PRINT_DEBUG(msg, ##args))
#define INFO_UTIL_VER(msg, args...) IF_DEBUG_VER(UTIL,PRINT_DEBUG(msg, ##args))
#define CM_DPRINTF(msg, args...) 	IF_DEBUG(CM,PRINT_DEBUG(msg, ##args))

extern void dump(char* _buf, uint16_t _count);

#define _DUMP(BUF, COUNT) do {		\
	printk("[%s]: ", __func__);		\
	dump((char*)BUF, COUNT);				\
	} while (0)

#ifdef _APP_DEBUG_
#define DUMP(BUF, COUNT) _DUMP(BUF, COUNT)
#else
#define DUMP(BUF, COUNT) do {} while (0)
#endif
#endif

#define DUMP_TCP(BUF, COUNT) IF_TCP_DUMP(_DUMP(BUF, COUNT))
#define DUMP_SPI(BUF, COUNT) IF_SPI_DUMP(_DUMP(BUF, COUNT))

#define DUMP_SPI_CMD(BUF) do {				\
	if (dumpDebug & INFO_SPI_FLAG) {		\
	int i = 0;								\
	for (; i < CMD_MAX_LEN; ++i) 			\
	{										\
		printk("0x%x ", BUF[i]);			\
		if (BUF[i] == END_CMD)				\
			break;							\
	}										\
	printk("\n");							\
	}										\
}while(0);

