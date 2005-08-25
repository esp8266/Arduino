//  osc.h
//
//  Open Sound Control message sending fn's for avrmini
//  
//  Scott Wilson
//  July 21, 2002
//

#ifndef _OSC_H
#define _OSC_H

#include "global.h"
#include <progmem.h>

#define oscSendMessageOneArg oscSendMessageInt

void oscInit(void);
void oscSendMessage(const char PROGMEM *address);
void oscSendMessageInt(const char PROGMEM *address, s32 arg);
void oscSendMessageIntInt(const char PROGMEM *address, s32 arg, s32 arg2);
void oscSendMessageString(const char PROGMEM *address, const char PROGMEM *arg);


#endif
