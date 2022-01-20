/**
    Header file for direct Ethernet frame access to the ENC28J60 controller
    @file enc28j60.h
*/

/*
    Copyright (c) 2012-2013, Thingsquare, http://www.thingsquare.com/.
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

// original sources: https://github.com/njh/EtherSia/tree/master/src/enc28j60.h

#ifndef ENC28J60_H
#define ENC28J60_H

#include <SPI.h>

/**
    Send and receive Ethernet frames directly using a ENC28J60 controller.
*/
class ENC28J60
{
public:
    /**
        Constructor that uses the default hardware SPI pins
        @param cs the Arduino Chip Select / Slave Select pin (default 10 on Uno)
    */
    ENC28J60(int8_t cs = SS, SPIClass& spi = SPI, int8_t intr = -1);

    /**
        Initialise the Ethernet controller
        Must be called before sending or receiving Ethernet frames

        @param address the local MAC address for the Ethernet interface
        @return Returns true if setting up the Ethernet interface was successful
    */
    boolean begin(const uint8_t* address);

    /**
        Send an Ethernet frame
        @param data a pointer to the data to send
        @param datalen the length of the data in the packet
        @return the number of bytes transmitted
    */
    virtual uint16_t sendFrame(const uint8_t* data, uint16_t datalen);

    /**
        Read an Ethernet frame
        @param buffer a pointer to a buffer to write the packet to
        @param bufsize the available space in the buffer
        @return the length of the received packet
               or 0 if no packet was received
    */
    virtual uint16_t readFrame(uint8_t* buffer, uint16_t bufsize);

protected:
    static constexpr bool interruptIsPossible() { return false; }

    /**
        Read an Ethernet frame size
        @return the length of data do receive
               or 0 if no frame was received
    */
    uint16_t readFrameSize();

    /**
        discard an Ethernet frame
        @param framesize readFrameSize()'s result
    */
    void discardFrame(uint16_t framesize);

    /**
        Read an Ethernet frame data
           readFrameSize() must be called first,
           its result must be passed into framesize parameter
        @param buffer a pointer to a buffer to write the frame to
        @param framesize readFrameSize()'s result
        @return the length of the received frame
               or 0 if a problem occurred
    */
    uint16_t readFrameData(uint8_t* frame, uint16_t framesize);

private:
    uint8_t is_mac_mii_reg(uint8_t reg);
    uint8_t readreg(uint8_t reg);
    void    writereg(uint8_t reg, uint8_t data);
    void    setregbitfield(uint8_t reg, uint8_t mask);
    void    clearregbitfield(uint8_t reg, uint8_t mask);
    void    setregbank(uint8_t new_bank);
    void    writedata(const uint8_t* data, int datalen);
    void    writedatabyte(uint8_t byte);
    int     readdata(uint8_t* buf, int len);
    uint8_t readdatabyte(void);
    void    softreset(void);
    uint8_t readrev(void);
    bool    reset(void);

    void    enc28j60_arch_spi_init(void);
    uint8_t enc28j60_arch_spi_write(uint8_t data);
    uint8_t enc28j60_arch_spi_read(void);
    void    enc28j60_arch_spi_select(void);
    void    enc28j60_arch_spi_deselect(void);

    // Previously defined in contiki/core/sys/clock.h
    void clock_delay_usec(uint16_t dt);

    uint8_t   _bank;
    int8_t    _cs;
    SPIClass& _spi;

    const uint8_t* _localMac;

    /* readFrame*() state */
    uint16_t _next, _len;
};

#endif /* ENC28J60_H */
