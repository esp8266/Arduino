
#ifndef __EEPROM_H
#define __EEPROM_H

#include <EEPROM.h>

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>

#define EEPROM_FILE_NAME "eeprom"

EEPROMClass::EEPROMClass ()
{
}

EEPROMClass::~EEPROMClass ()
{
	if (_fd >= 0)
		close(_fd);
}

void EEPROMClass::begin(size_t size)
{
	_size = size;
	if (   (_fd = open(EEPROM_FILE_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) == -1
	    || ftruncate(_fd, size) == -1)
	{
		fprintf(stderr, MOCK "EEPROM: cannot open/create '%s' for r/w: %s\n\r", EEPROM_FILE_NAME, strerror(errno));
		_fd = -1;
	}
}

void EEPROMClass::end()
{
	if (_fd != -1)
		close(_fd);
}

bool EEPROMClass::commit()
{
	return true;
}

uint8_t EEPROMClass::read (int x)
{
	char c = 0;
	if (pread(_fd, &c, 1, x) != 1)
		fprintf(stderr, MOCK "eeprom: %s\n\r", strerror(errno));
	return c;
}

void EEPROMClass::write (int x, uint8_t c)
{
	if (x > (int)_size)
		fprintf(stderr, MOCK "### eeprom beyond\r\n");
	else if (pwrite(_fd, &c, 1, x) != 1)
		fprintf(stderr, MOCK "eeprom: %s\n\r", strerror(errno));
}

#if 0
void EEPROMClass::update (int x, char c)
{
	put(x, c);
}
#endif
		
#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_EEPROM)
EEPROMClass EEPROM;
#endif

#endif
