/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * Copyright (c) 2014 by Paul Stoffregen <paul@pjrc.com> (Transaction API)
 * Copyright (c) 2014 by Matthijs Kooijman <matthijs@stdin.nl> (SPISettings AVR)
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "SPI.h"
#include "HSPI.h"

SPIClass SPI;


SPIClass::SPIClass()
: _impl(0)
{
}

void SPIClass::begin()
{
  if (_impl)
    end();
  _impl = new HSPI;
  _impl->begin();
}

void SPIClass::end() 
{
  if (!_impl)
    return;
  _impl->end();
  delete _impl;
  _impl = 0;
}

uint8_t SPIClass::transfer(uint8_t data) 
{
  if (!_impl)
    return;
  return _impl->transfer(data);
}

void SPIClass::transfer(void *buf, size_t count) 
{
  if (!_impl)
    return;
  _impl->transfer(buf, count);
}

void SPIClass::setBitOrder(uint8_t bitOrder) 
{
  if (!_impl)
    return;
  _impl->setBitOrder(bitOrder);
}

void SPIClass::setDataMode(uint8_t dataMode) 
{
  if (!_impl)
    return;
  _impl->setDataMode(dataMode);
}

void SPIClass::setClockDivider(uint8_t clockDiv) 
{
  if (!_impl)
    return;
  _impl->setClockDivider(clockDiv);
}

