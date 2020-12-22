/*
    Copyright (c) 2013, WIZnet Co., Ltd.
    Copyright (c) 2016, Nicholas Humfrey
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
    COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
    (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
    HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
    STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
    OF THE POSSIBILITY OF SUCH DAMAGE.

*/

// original sources: https://github.com/njh/W5100MacRaw

#include <SPI.h>
#include "w5100.h"


uint8_t Wiznet5100::wizchip_read(uint16_t address)
{
    uint8_t ret;

    wizchip_cs_select();
    _spi.transfer(0x0F);
    _spi.transfer((address & 0xFF00) >>  8);
    _spi.transfer((address & 0x00FF) >>  0);
    ret = _spi.transfer(0);
    wizchip_cs_deselect();

    return ret;
}

uint16_t Wiznet5100::wizchip_read_word(uint16_t address)
{
    return ((uint16_t)wizchip_read(address) << 8) + wizchip_read(address + 1);
}


void Wiznet5100::wizchip_read_buf(uint16_t address, uint8_t* pBuf, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        pBuf[i] = wizchip_read(address + i);
    }
}

void Wiznet5100::wizchip_write(uint16_t address, uint8_t wb)
{
    wizchip_cs_select();
    _spi.transfer(0xF0);
    _spi.transfer((address & 0xFF00) >>  8);
    _spi.transfer((address & 0x00FF) >>  0);
    _spi.transfer(wb);    // Data write (write 1byte data)
    wizchip_cs_deselect();
}

void Wiznet5100::wizchip_write_word(uint16_t address, uint16_t word)
{
    wizchip_write(address, (uint8_t)(word >> 8));
    wizchip_write(address + 1, (uint8_t) word);
}

void Wiznet5100::wizchip_write_buf(uint16_t address, const uint8_t* pBuf, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        wizchip_write(address + i, pBuf[i]);
    }
}

void Wiznet5100::setSn_CR(uint8_t cr)
{
    // Write the command to the Command Register
    wizchip_write(Sn_CR, cr);

    // Now wait for the command to complete
    while (wizchip_read(Sn_CR));
}

uint16_t Wiznet5100::getSn_TX_FSR()
{
    uint16_t val = 0, val1 = 0;
    do
    {
        val1 = wizchip_read_word(Sn_TX_FSR);
        if (val1 != 0)
        {
            val = wizchip_read_word(Sn_TX_FSR);
        }
    } while (val != val1);
    return val;
}


uint16_t Wiznet5100::getSn_RX_RSR()
{
    uint16_t val = 0, val1 = 0;
    do
    {
        val1 = wizchip_read_word(Sn_RX_RSR);
        if (val1 != 0)
        {
            val = wizchip_read_word(Sn_RX_RSR);
        }
    } while (val != val1);
    return val;
}

void Wiznet5100::wizchip_send_data(const uint8_t *wizdata, uint16_t len)
{
    uint16_t ptr;
    uint16_t size;
    uint16_t dst_mask;
    uint16_t dst_ptr;

    ptr = getSn_TX_WR();

    dst_mask = ptr & TxBufferMask;
    dst_ptr = TxBufferAddress + dst_mask;

    if (dst_mask + len > TxBufferLength)
    {
        size = TxBufferLength - dst_mask;
        wizchip_write_buf(dst_ptr, wizdata, size);
        wizdata += size;
        size = len - size;
        dst_ptr = TxBufferAddress;
        wizchip_write_buf(dst_ptr, wizdata, size);
    }
    else
    {
        wizchip_write_buf(dst_ptr, wizdata, len);
    }

    ptr += len;

    setSn_TX_WR(ptr);
}

void Wiznet5100::wizchip_recv_data(uint8_t *wizdata, uint16_t len)
{
    uint16_t ptr;
    uint16_t size;
    uint16_t src_mask;
    uint16_t src_ptr;

    ptr = getSn_RX_RD();

    src_mask = ptr & RxBufferMask;
    src_ptr = RxBufferAddress + src_mask;


    if ((src_mask + len) > RxBufferLength)
    {
        size = RxBufferLength - src_mask;
        wizchip_read_buf(src_ptr, wizdata, size);
        wizdata += size;
        size = len - size;
        src_ptr = RxBufferAddress;
        wizchip_read_buf(src_ptr, wizdata, size);
    }
    else
    {
        wizchip_read_buf(src_ptr, wizdata, len);
    }

    ptr += len;

    setSn_RX_RD(ptr);
}

void Wiznet5100::wizchip_recv_ignore(uint16_t len)
{
    uint16_t ptr;

    ptr = getSn_RX_RD();
    ptr += len;
    setSn_RX_RD(ptr);
}

void Wiznet5100::wizchip_sw_reset()
{
    setMR(MR_RST);
    getMR(); // for delay

    setSHAR(_mac_address);
}


Wiznet5100::Wiznet5100(int8_t cs, SPIClass& spi, int8_t intr):
    _spi(spi), _cs(cs)
{
    (void)intr;
}

boolean Wiznet5100::begin(const uint8_t *mac_address)
{
    memcpy(_mac_address, mac_address, 6);

    pinMode(_cs, OUTPUT);
    wizchip_cs_deselect();

#if 0
    _spi.begin();
    _spi.setClockDivider(SPI_CLOCK_DIV4); // 4 MHz?
    _spi.setBitOrder(MSBFIRST);
    _spi.setDataMode(SPI_MODE0);
#endif

    wizchip_sw_reset();

    // Set the size of the Rx and Tx buffers
    wizchip_write(RMSR, RxBufferSize);
    wizchip_write(TMSR, TxBufferSize);

    // Set our local MAC address
    setSHAR(_mac_address);

    // Open Socket 0 in MACRaw mode
    setSn_MR(Sn_MR_MACRAW);
    setSn_CR(Sn_CR_OPEN);
    if (getSn_SR() != SOCK_MACRAW)
    {
        // Failed to put socket 0 into MACRaw mode
        return false;
    }

    // Success
    return true;
}

void Wiznet5100::end()
{
    setSn_CR(Sn_CR_CLOSE);

    // clear all interrupt of the socket
    setSn_IR(0xFF);

    // Wait for socket to change to closed
    while (getSn_SR() != SOCK_CLOSED);
}

uint16_t Wiznet5100::readFrame(uint8_t *buffer, uint16_t bufsize)
{
    uint16_t data_len = readFrameSize();

    if (data_len == 0)
    {
        return 0;
    }

    if (data_len > bufsize)
    {
        // Packet is bigger than buffer - drop the packet
        discardFrame(data_len);
        return 0;
    }

    return readFrameData(buffer, data_len);
}

uint16_t Wiznet5100::readFrameSize()
{
    uint16_t len = getSn_RX_RSR();

    if (len == 0)
    {
        return 0;
    }

    uint8_t head[2];
    uint16_t data_len = 0;

    wizchip_recv_data(head, 2);
    setSn_CR(Sn_CR_RECV);

    data_len = head[0];
    data_len = (data_len << 8) + head[1];
    data_len -= 2;

    return data_len;
}

void Wiznet5100::discardFrame(uint16_t framesize)
{
    wizchip_recv_ignore(framesize);
    setSn_CR(Sn_CR_RECV);
}

uint16_t Wiznet5100::readFrameData(uint8_t *buffer, uint16_t framesize)
{
    wizchip_recv_data(buffer, framesize);
    setSn_CR(Sn_CR_RECV);

#if 1
    // let lwIP deal with mac address filtering
    return framesize;
#else
    // W5100 doesn't have any built-in MAC address filtering
    if ((buffer[0] & 0x01) || memcmp(&buffer[0], _mac_address, 6) == 0)
    {
        // Addressed to an Ethernet multicast address or our unicast address
        return framesize;
    }
    else
    {
        return 0;
    }
#endif
}

uint16_t Wiznet5100::sendFrame(const uint8_t *buf, uint16_t len)
{
    // Wait for space in the transmit buffer
    while (1)
    {
        uint16_t freesize = getSn_TX_FSR();
        if (getSn_SR() == SOCK_CLOSED)
        {
            return -1;
        }
        if (len <= freesize)
        {
            break;
        }
    };

    wizchip_send_data(buf, len);
    setSn_CR(Sn_CR_SEND);

    while (1)
    {
        uint8_t tmp = getSn_IR();
        if (tmp & Sn_IR_SENDOK)
        {
            setSn_IR(Sn_IR_SENDOK);
            // Packet sent ok
            break;
        }
        else if (tmp & Sn_IR_TIMEOUT)
        {
            setSn_IR(Sn_IR_TIMEOUT);
            // There was a timeout
            return -1;
        }
    }

    return len;
}
