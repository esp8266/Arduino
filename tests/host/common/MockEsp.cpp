
#include <Esp.h>
#include <eboot_command.h>

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
	fprintf(stderr, MOCK "Esp.restart(): exiting\n");
	exit(EXIT_SUCCESS);
}

uint32_t EspClass::getChipId()
{
	return 0xee1337;
}

bool EspClass::checkFlashConfig(bool needsEquals)
{
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

bool EspClass::flashEraseSector(uint32_t sector)
{
	return true;
}

FlashMode_t EspClass::getFlashChipMode()
{
	return FM_DOUT;
}

FlashMode_t EspClass::magicFlashChipMode(uint8_t byte)
{
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