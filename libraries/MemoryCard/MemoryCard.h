/*

 MemoryCard - a slightly more friendly wrapper for sdfatlib

 This library aims to expose a subset of SD card functionality
 in the form of a higher level "wrapper" object.

 License: GNU General Public License V3
          (Because sdfatlib is licensed with this.)

 (C) Copyright 2010 SparkFun Electronics

 */

#ifndef __MEMORY_CARD_H__
#define __MEMORY_CARD_H__

#include "MemoryCardDevice.h"

extern MemoryCardDevice MemoryCard;

// Preinstantiate required object
MemoryCardDevice MemoryCard;

#endif
