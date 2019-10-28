/*
  SPISlave library for esp8266

  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/
#include "SPISlave.h"
extern "C" {
#include "hspi_slave.h"
}

void SPISlaveClass::_data_rx(uint8_t * data, uint8_t len)
{
    if(_data_cb) {
        _data_cb(data, len);
    }
}
void SPISlaveClass::_status_rx(uint32_t data)
{
    if(_status_cb) {
        _status_cb(data);
    }
}
void SPISlaveClass::_data_tx(void)
{
    if(_data_sent_cb) {
        _data_sent_cb();
    }
}
void SPISlaveClass::_status_tx(void)
{
    if(_status_sent_cb) {
        _status_sent_cb();
    }
}
void SPISlaveClass::_s_data_rx(void *arg, uint8_t * data, uint8_t len)
{
    reinterpret_cast<SPISlaveClass*>(arg)->_data_rx(data,len);
}
void SPISlaveClass::_s_status_rx(void *arg, uint32_t data)
{
    reinterpret_cast<SPISlaveClass*>(arg)->_status_rx(data);
}
void SPISlaveClass::_s_data_tx(void *arg)
{
    reinterpret_cast<SPISlaveClass*>(arg)->_data_tx();
}
void SPISlaveClass::_s_status_tx(void *arg)
{
    reinterpret_cast<SPISlaveClass*>(arg)->_status_tx();
}

void SPISlaveClass::begin()
{
    hspi_slave_onData(&_s_data_rx);
    hspi_slave_onDataSent(&_s_data_tx);
    hspi_slave_onStatus(&_s_status_rx);
    hspi_slave_onStatusSent(&_s_status_tx);
    hspi_slave_begin(4, this);
}
  
void SPISlaveClass::begin(uint8_t statusLength)
{
    hspi_slave_onData(&_s_data_rx);
    hspi_slave_onDataSent(&_s_data_tx);
    hspi_slave_onStatus(&_s_status_rx);
    hspi_slave_onStatusSent(&_s_status_tx);
    hspi_slave_begin(statusLength, this);
}
void SPISlaveClass::end()
{
    hspi_slave_onData(nullptr);
    hspi_slave_onDataSent(nullptr);
    hspi_slave_onStatus(nullptr);
    hspi_slave_onStatusSent(nullptr);
    hspi_slave_end();
}
void SPISlaveClass::setData(uint8_t * data, size_t len)
{
    if(len > 32) {
        len = 32;
    }
    hspi_slave_setData(data, len);
}
void SPISlaveClass::setStatus(uint32_t status)
{
    hspi_slave_setStatus(status);
}
void SPISlaveClass::onData(SpiSlaveDataHandler cb)
{
    _data_cb = cb;
}
void SPISlaveClass::onDataSent(SpiSlaveSentHandler cb)
{
    _data_sent_cb = cb;
}
void SPISlaveClass::onStatus(SpiSlaveStatusHandler cb)
{
    _status_cb = cb;
}
void SPISlaveClass::onStatusSent(SpiSlaveSentHandler cb)
{
    _status_sent_cb = cb;
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SPISLAVE)
SPISlaveClass SPISlave;
#endif
