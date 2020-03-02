/*
 Arduino emulation - esp8266's core
 Copyright (c) 2018 david gauchard. All rights reserved.

 Permission is hereby granted, free of charge, to any person obtaining a
 copy of this software and associated documentation files (the "Software"),
 to deal with the Software without restriction, including without limitation
 the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the
 Software is furnished to do so, subject to the following conditions:

 - Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimers.

 - Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimers in the
   documentation and/or other materials provided with the distribution.

 - The names of its contributors may not be used to endorse or promote
   products derived from this Software without specific prior written
   permission.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 DEALINGS WITH THE SOFTWARE.
*/

#include <Esp.h>
#include <eboot_command.h>

#include <sys/time.h>

#include <stdlib.h>

unsigned long long operator"" _kHz(unsigned long long x) {
    return x * 1000;
}

unsigned long long operator"" _MHz(unsigned long long x) {
    return x * 1000 * 1000;
}

unsigned long long operator"" _GHz(unsigned long long x) {
    return x * 1000 * 1000 * 1000;
}

unsigned long long operator"" _kBit(unsigned long long x) {
    return x * 1024;
}

unsigned long long operator"" _MBit(unsigned long long x) {
    return x * 1024 * 1024;
}

unsigned long long operator"" _GBit(unsigned long long x) {
    return x * 1024 * 1024 * 1024;
}

unsigned long long operator"" _kB(unsigned long long x) {
    return x * 1024;
}

unsigned long long operator"" _MB(unsigned long long x) {
    return x * 1024 * 1024;
}

unsigned long long operator"" _GB(unsigned long long x) {
    return x * 1024 * 1024 * 1024;
}

uint32_t _SPIFFS_start;

void eboot_command_write (struct eboot_command* cmd)
{
	(void)cmd;
}

EspClass ESP;

void EspClass::restart ()
{
	mockverbose("Esp.restart(): exiting\n");
	exit(EXIT_SUCCESS);
}

uint32_t EspClass::getChipId()
{
	return 0xee1337;
}

bool EspClass::checkFlashConfig(bool needsEquals)
{
	(void) needsEquals;
	return true;
}

uint32_t EspClass::getSketchSize()
{
	return 400000;
}

uint32_t EspClass::getFreeHeap()
{
	return 30000;
}

String EspClass::getResetReason()
{
  return "Power on";
}

uint32_t EspClass::getFreeSketchSpace()
{
  return 4 * 1024 * 1024;
}

uint8_t EspClass::getCpuFreqMHz()
{
  return F_CPU / 1000000;
}

const char *EspClass::getSdkVersion()
{
  return "2.5.0";
}

uint32_t EspClass::getFlashChipSpeed()
{
  return 40;
}

void EspClass::getHeapStats(uint32_t* hfree, uint16_t* hmax, uint8_t* hfrag) {
  uint32_t hf = 10 * 1024;
  float hm = 1 * 1024;

  if (hfree) *hfree = hf;
  if (hmax) *hmax = hm;
  if (hfrag) *hfrag = 100 - (sqrt(hm) * 100) / hf;
}

bool EspClass::flashEraseSector(uint32_t sector)
{
	(void) sector;
	return true;
}

FlashMode_t EspClass::getFlashChipMode()
{
	return FM_DOUT;
}

FlashMode_t EspClass::magicFlashChipMode(uint8_t byte)
{
	(void) byte;
	return FM_DOUT;
}

bool EspClass::flashWrite(uint32_t offset, uint32_t *data, size_t size)
{
	(void)offset;
	(void)data;
	(void)size;
	return true;
}

bool EspClass::flashRead(uint32_t offset, uint32_t *data, size_t size)
{
	(void)offset;
	(void)data;
	(void)size;
	return true;
}

uint32_t EspClass::magicFlashChipSize(uint8_t byte) {
    switch(byte & 0x0F) {
        case 0x0: // 4 Mbit (512KB)
            return (512_kB);
        case 0x1: // 2 MBit (256KB)
            return (256_kB);
        case 0x2: // 8 MBit (1MB)
            return (1_MB);
        case 0x3: // 16 MBit (2MB)
            return (2_MB);
        case 0x4: // 32 MBit (4MB)
            return (4_MB);
        case 0x8: // 64 MBit (8MB)
            return (8_MB);
        case 0x9: // 128 MBit (16MB)
            return (16_MB);
        default: // fail?
            return 0;
    }
}

uint32_t EspClass::getFlashChipRealSize(void)
{
	return magicFlashChipSize(4);
}

uint32_t EspClass::getFlashChipSize(void)
{
	return magicFlashChipSize(4);
}

String EspClass::getFullVersion ()
{
	return "host-emulation";
}

uint32_t EspClass::getFreeContStack()
{
    return 4000;
}

void EspClass::resetFreeContStack()
{
}

uint32_t EspClass::getCycleCount()
{
    timeval t;
    gettimeofday(&t, NULL);
    return (((uint64_t)t.tv_sec) * 1000000 + t.tv_usec) * (F_CPU / 1000000);
}

void EspClass::setDramHeap()
{
}

void EspClass::setIramHeap()
{
}

void EspClass::setExternalHeap()
{
}

void EspClass::resetHeap()
{
}
