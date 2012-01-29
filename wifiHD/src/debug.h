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


#ifdef _APP_DEBUG_
#define INFO(msg, args...) do { 			\
printk("I-[%s] " msg , __func__ , ##args );	\
} while (0)
#else /* !defined(_DEBUG_) */
#define INFO(msg, args...) do {} while (0)
#endif /* !defined(_DEBUG_) */

#if 1
#define WARN(msg, args...) do {	\
	printk("W-[%s] " msg , __func__ , ##args );		\
	} while (0)
#else
#define WARN(msg, args...) do {		} while (0)
#endif

extern void dump(char* _buf, uint16_t _count);

#ifdef _APP_DEBUG_
#define DUMP(BUF, COUNT) do {		\
	printk("[%s]\n", __func__);		\
	dump((char*)BUF, COUNT);				\
	} while (0)
#else
#define DUMP(BUF, COUNT) do {} while (0)
#endif
#endif
