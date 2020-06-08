/* 
 SPI.cpp - SPI library for esp8266

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

#include "SPI.h"
#include "HardwareSerial.h"

#define SPI_PINS_HSPI			0 // Normal HSPI mode (MISO = GPIO12, MOSI = GPIO13, SCLK = GPIO14);
#define SPI_PINS_HSPI_OVERLAP	1 // HSPI Overllaped in spi0 pins (MISO = SD0, MOSI = SDD1, SCLK = CLK);

#define SPI_OVERLAP_SS 0


typedef union {
        uint32_t regValue;
        struct {
                unsigned regL :6;
                unsigned regH :6;
                unsigned regN :6;
                unsigned regPre :13;
                unsigned regEQU :1;
        };
} spiClk_t;

SPIClass::SPIClass() {
    useHwCs = false;
    pinSet = SPI_PINS_HSPI;
}

bool SPIClass::pins(int8_t sck, int8_t miso, int8_t mosi, int8_t ss)
{
    if (sck == 6 &&
        miso == 7 &&
        mosi == 8 &&
        ss == 0) {
        pinSet = SPI_PINS_HSPI_OVERLAP;
    } else if (sck == 14 &&
	           miso == 12 &&
               mosi == 13) {
        pinSet = SPI_PINS_HSPI;
    } else {
        return false;
    }

    return true;
}

void SPIClass::begin() {
    switch (pinSet) {
    case SPI_PINS_HSPI_OVERLAP:
        IOSWAP |= (1 << IOSWAP2CS);
        //SPI0E3 |= 0x1; This is in the MP3_DECODER example, but makes the WD kick in here.
        SPI1E3 |= 0x3;

        setHwCs(true);
        break;
    case SPI_PINS_HSPI:
    default:
        pinMode(SCK, SPECIAL);  ///< GPIO14
        pinMode(MISO, SPECIAL); ///< GPIO12
        pinMode(MOSI, SPECIAL); ///< GPIO13
        break;
    }

    SPI1C = 0;
    setFrequency(1000000); ///< 1MHz
    SPI1U = SPIUMOSI | SPIUDUPLEX | SPIUSSE;
    SPI1U1 = (7 << SPILMOSI) | (7 << SPILMISO);
    SPI1C1 = 0;
}

void SPIClass::end() {
    switch (pinSet) {
    case SPI_PINS_HSPI:
        pinMode(SCK, INPUT);
        pinMode(MISO, INPUT);
        pinMode(MOSI, INPUT);
        if (useHwCs) {
            pinMode(SS, INPUT);
        }
        break;
    case SPI_PINS_HSPI_OVERLAP:
        IOSWAP &= ~(1 << IOSWAP2CS);
        if (useHwCs) {
            SPI1P |= SPIPCS1DIS | SPIPCS0DIS | SPIPCS2DIS;
            pinMode(SPI_OVERLAP_SS, INPUT);
        }
        break;
    }
}

void SPIClass::setHwCs(bool use) {
    switch (pinSet) {
    case SPI_PINS_HSPI:
        if (use) {
            pinMode(SS, SPECIAL); ///< GPIO15
            SPI1U |= (SPIUCSSETUP | SPIUCSHOLD);
    } else {
            if (useHwCs) {
                pinMode(SS, INPUT);
            SPI1U &= ~(SPIUCSSETUP | SPIUCSHOLD);
            }
        }
        break;
    case SPI_PINS_HSPI_OVERLAP:
        if (use) {
            pinMode(SPI_OVERLAP_SS, FUNCTION_1); // GPI0 to SPICS2 mode
            SPI1P &= ~SPIPCS2DIS;
            SPI1P |= SPIPCS1DIS | SPIPCS0DIS;
            SPI1U |= (SPIUCSSETUP | SPIUCSHOLD);
        }
        else {
            if (useHwCs) {
                pinMode(SPI_OVERLAP_SS, INPUT);
                SPI1P |= SPIPCS1DIS | SPIPCS0DIS | SPIPCS2DIS;
                SPI1U &= ~(SPIUCSSETUP | SPIUCSHOLD);
            }
        }
        break;
    }

    useHwCs = use;
}

void SPIClass::beginTransaction(SPISettings settings) {
    while(SPI1CMD & SPIBUSY) {}
    setFrequency(settings._clock);
    setBitOrder(settings._bitOrder);
    setDataMode(settings._dataMode);
}

void SPIClass::endTransaction() {
}

void SPIClass::setDataMode(uint8_t dataMode) {

    /**
     SPI_MODE0 0x00 - CPOL: 0  CPHA: 0
     SPI_MODE1 0x01 - CPOL: 0  CPHA: 1
     SPI_MODE2 0x10 - CPOL: 1  CPHA: 0
     SPI_MODE3 0x11 - CPOL: 1  CPHA: 1
     */

    bool CPOL = (dataMode & 0x10); ///< CPOL (Clock Polarity)
    bool CPHA = (dataMode & 0x01); ///< CPHA (Clock Phase)

    // https://github.com/esp8266/Arduino/issues/2416
    // https://github.com/esp8266/Arduino/pull/2418
    if(CPOL)          // Ensure same behavior as
        CPHA ^= 1;    // SAM, AVR and Intel Boards

    if(CPHA) {
        SPI1U |= (SPIUSME);
    } else {
        SPI1U &= ~(SPIUSME);
    }

    if(CPOL) {
        SPI1P |= 1<<29;
    } else {
        SPI1P &= ~(1<<29);
        //todo test whether it is correct to set CPOL like this.
    }

}

void SPIClass::setBitOrder(uint8_t bitOrder) {
    if(bitOrder == MSBFIRST) {
        SPI1C &= ~(SPICWBO | SPICRBO);
    } else {
        SPI1C |= (SPICWBO | SPICRBO);
    }
}

/**
 * calculate the Frequency based on the register value
 * @param reg
 * @return
 */
static uint32_t ClkRegToFreq(spiClk_t * reg) {
    return (ESP8266_CLOCK / ((reg->regPre + 1) * (reg->regN + 1)));
}

void SPIClass::setFrequency(uint32_t freq) {
    static uint32_t lastSetFrequency = 0;
    static uint32_t lastSetRegister = 0;

    if(freq >= ESP8266_CLOCK) {
        setClockDivider(0x80000000);
        return;
    }

    if(lastSetFrequency == freq && lastSetRegister == SPI1CLK) {
        // do nothing (speed optimization)
        return;
    }

    const spiClk_t minFreqReg = { 0x7FFFF020 };
    uint32_t minFreq = ClkRegToFreq((spiClk_t*) &minFreqReg);
    if(freq < minFreq) {
        // use minimum possible clock
        setClockDivider(minFreqReg.regValue);
        lastSetRegister = SPI1CLK;
        lastSetFrequency = freq;
        return;
    }

    uint8_t calN = 1;

    spiClk_t bestReg = { 0 };
    int32_t bestFreq = 0;

    // find the best match
    while(calN <= 0x3F) { // 0x3F max for N

        spiClk_t reg = { 0 };
        int32_t calFreq;
        int32_t calPre;
        int8_t calPreVari = -2;

        reg.regN = calN;

        while(calPreVari++ <= 1) { // test different variants for Pre (we calculate in int so we miss the decimals, testing is the easyest and fastest way)
            calPre = (((ESP8266_CLOCK / (reg.regN + 1)) / freq) - 1) + calPreVari;
            if(calPre > 0x1FFF) {
                reg.regPre = 0x1FFF; // 8191
            } else if(calPre <= 0) {
                reg.regPre = 0;
            } else {
                reg.regPre = calPre;
            }

            reg.regL = ((reg.regN + 1) / 2);
            // reg.regH = (reg.regN - reg.regL);

            // test calculation
            calFreq = ClkRegToFreq(&reg);
            //os_printf("-----[0x%08X][%d]\t EQU: %d\t Pre: %d\t N: %d\t H: %d\t L: %d = %d\n", reg.regValue, freq, reg.regEQU, reg.regPre, reg.regN, reg.regH, reg.regL, calFreq);

            if(calFreq == (int32_t) freq) {
                // accurate match use it!
                memcpy(&bestReg, &reg, sizeof(bestReg));
                break;
            } else if(calFreq < (int32_t) freq) {
                // never go over the requested frequency
                if((freq - calFreq) < (freq - bestFreq)) {
                    bestFreq = calFreq;
                    memcpy(&bestReg, &reg, sizeof(bestReg));
                }
            }
        }
        if(calFreq == (int32_t) freq) {
            // accurate match use it!
            break;
        }
        calN++;
    }

    // os_printf("[0x%08X][%d]\t EQU: %d\t Pre: %d\t N: %d\t H: %d\t L: %d\t - Real Frequency: %d\n", bestReg.regValue, freq, bestReg.regEQU, bestReg.regPre, bestReg.regN, bestReg.regH, bestReg.regL, ClkRegToFreq(&bestReg));

    setClockDivider(bestReg.regValue);
    lastSetRegister = SPI1CLK;
    lastSetFrequency = freq;

}

void SPIClass::setClockDivider(uint32_t clockDiv) {
    if(clockDiv == 0x80000000) {
        GPMUX |= (1 << 9); // Set bit 9 if sysclock required
    } else {
        GPMUX &= ~(1 << 9);
    }
    SPI1CLK = clockDiv;
}

inline void SPIClass::setDataBits(uint16_t bits) {
    const uint32_t mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
    bits--;
    SPI1U1 = ((SPI1U1 & mask) | ((bits << SPILMOSI) | (bits << SPILMISO)));
}

uint8_t SPIClass::transfer(uint8_t data) {
    while(SPI1CMD & SPIBUSY) {}
    // reset to 8Bit mode
    setDataBits(8);
    SPI1W0 = data;
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}
    return (uint8_t) (SPI1W0 & 0xff);
}

uint16_t SPIClass::transfer16(uint16_t data) {
    union {
            uint16_t val;
            struct {
                    uint8_t lsb;
                    uint8_t msb;
            };
    } in, out;
    in.val = data;

    if((SPI1C & (SPICWBO | SPICRBO))) {
        //LSBFIRST
        out.lsb = transfer(in.lsb);
        out.msb = transfer(in.msb);
    } else {
        //MSBFIRST
        out.msb = transfer(in.msb);
        out.lsb = transfer(in.lsb);
    }
    return out.val;
}

void SPIClass::transfer(void *buf, uint16_t count) {
    uint8_t *cbuf = reinterpret_cast<uint8_t*>(buf);

    // cbuf may not be 32bits-aligned
    for (; (((unsigned long)cbuf) & 3) && count; cbuf++, count--)
        *cbuf = transfer(*cbuf);

    // cbuf is now aligned
    // count may not be a multiple of 4
    uint16_t count4 = count & ~3;
    transferBytes(cbuf, cbuf, count4);

    // finish the last <4 bytes
    cbuf += count4;
    count -= count4;
    for (; count; cbuf++, count--)
        *cbuf = transfer(*cbuf);
}

void SPIClass::write(uint8_t data) {
    while(SPI1CMD & SPIBUSY) {}
    // reset to 8Bit mode
    setDataBits(8);
    SPI1W0 = data;
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}
}

void SPIClass::write16(uint16_t data) {
    write16(data, !(SPI1C & (SPICWBO | SPICRBO)));
}

void SPIClass::write16(uint16_t data, bool msb) {
    while(SPI1CMD & SPIBUSY) {}
    // Set to 16Bits transfer
    setDataBits(16);
    if(msb) {
        // MSBFIRST Byte first
        SPI1W0 = (data >> 8) | (data << 8);
    } else {
        // LSBFIRST Byte first
        SPI1W0 = data;
    }
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}
}

void SPIClass::write32(uint32_t data) {
    write32(data, !(SPI1C & (SPICWBO | SPICRBO)));
}

void SPIClass::write32(uint32_t data, bool msb) {
    while(SPI1CMD & SPIBUSY) {}
    // Set to 32Bits transfer
    setDataBits(32);
    if(msb) {
        union {
                uint32_t l;
                uint8_t b[4];
        } data_;
        data_.l = data;
        // MSBFIRST Byte first
        data = (data_.b[3] | (data_.b[2] << 8) | (data_.b[1] << 16) | (data_.b[0] << 24));
    }
    SPI1W0 = data;
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}
}

/**
 * Note:
 *  data need to be aligned to 32Bit
 *  or you get an Fatal exception (9)
 * @param data uint8_t *
 * @param size uint32_t
 */
void SPIClass::writeBytes(const uint8_t * data, uint32_t size) {
    while(size) {
        if(size > 64) {
            writeBytes_(data, 64);
            size -= 64;
            data += 64;
        } else {
            writeBytes_(data, size);
            size = 0;
        }
    }
}

void SPIClass::writeBytes_(const uint8_t * data, uint8_t size) {
    while(SPI1CMD & SPIBUSY) {}
    // Set Bits to transfer
    setDataBits(size * 8);

    uint32_t * fifoPtr = (uint32_t*)&SPI1W0;
    const uint32_t * dataPtr = (uint32_t*) data;
    uint32_t dataSize = ((size + 3) / 4);

    while(dataSize--) {
        *fifoPtr = *dataPtr;
        dataPtr++;
        fifoPtr++;
    }

    __sync_synchronize();
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}
}

/**
 * @param data uint8_t *
 * @param size uint8_t  max for size is 64Byte
 * @param repeat uint32_t
 */
void SPIClass::writePattern(const uint8_t * data, uint8_t size, uint32_t repeat) {
    if(size > 64) return; //max Hardware FIFO

    while(SPI1CMD & SPIBUSY) {}

    uint32_t buffer[16];
    uint8_t *bufferPtr=(uint8_t *)&buffer;
    const uint8_t *dataPtr = data;
    volatile uint32_t * fifoPtr = &SPI1W0;
    uint8_t r;
    uint32_t repeatRem;
    uint8_t i;

    if((repeat * size) <= 64){
        repeatRem = repeat * size;
        r = repeat;
        while(r--){
            dataPtr = data;
            for(i=0; i<size; i++){
                *bufferPtr = *dataPtr;
                bufferPtr++;
                dataPtr++;
            }
        }

        r = repeatRem;
        if(r & 3) r = r / 4 + 1;
        else r = r / 4;
        for(i=0; i<r; i++){
            *fifoPtr = buffer[i];
            fifoPtr++;
        }
        SPI1U = SPIUMOSI | SPIUSSE;
    } else {
        //Orig
        r = 64 / size;
        repeatRem = repeat % r * size;
        repeat = repeat / r;

        while(r--){
            dataPtr = data;
            for(i=0; i<size; i++){
                *bufferPtr = *dataPtr;
                bufferPtr++;
                dataPtr++;
            }
        }

        //Fill fifo with data
        for(i=0; i<16; i++){
            *fifoPtr = buffer[i];
            fifoPtr++;
        }

        r = 64 / size;

        SPI1U = SPIUMOSI | SPIUSSE;
        setDataBits(r * size * 8);
        while(repeat--){
            SPI1CMD |= SPIBUSY;
            while(SPI1CMD & SPIBUSY) {}
        }
    }
    //End orig
    setDataBits(repeatRem * 8);
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}

    SPI1U = SPIUMOSI | SPIUDUPLEX | SPIUSSE;
}

/**
 * @param out uint8_t *
 * @param in  uint8_t *
 * @param size uint32_t
 */
void SPIClass::transferBytes(const uint8_t * out, uint8_t * in, uint32_t size) {
    while(size) {
        if(size > 64) {
            transferBytes_(out, in, 64);
            size -= 64;
            if(out) out += 64;
            if(in) in += 64;
        } else {
            transferBytes_(out, in, size);
            size = 0;
        }
    }
}

/**
 * Note:
 *  in and out need to be aligned to 32Bit
 *  or you get an Fatal exception (9)
 * @param out uint8_t *
 * @param in  uint8_t *
 * @param size uint8_t (max 64)
 */

void SPIClass::transferBytesAligned_(const uint8_t * out, uint8_t * in, uint8_t size) {
    if (!size)
        return;

    while(SPI1CMD & SPIBUSY) {}
    // Set in/out Bits to transfer

    setDataBits(size * 8);

    volatile uint32_t *fifoPtr = &SPI1W0;

    if (out) {
        uint8_t outSize = ((size + 3) / 4);
        uint32_t *dataPtr = (uint32_t*) out;
        while (outSize--) {
            *(fifoPtr++) = *(dataPtr++);
        }
    } else {
        uint8_t outSize = ((size + 3) / 4);
        // no out data only read fill with dummy data!
        while (outSize--) {
            *(fifoPtr++) = 0xFFFFFFFF;
        }
    }

    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}

    if (in) {
        uint32_t *dataPtr = (uint32_t*) in;
        fifoPtr = &SPI1W0;
        int inSize = size;
        // Unlike outSize above, inSize tracks *bytes* since we must transfer only the requested bytes to the app to avoid overwriting other vars.
        while (inSize >= 4) {
            *(dataPtr++) = *(fifoPtr++);
            inSize -= 4;
            in += 4;
        }
        volatile uint8_t *fifoPtrB = (volatile uint8_t *)fifoPtr;
        while (inSize--) {
            *(in++) = *(fifoPtrB++);
        }
    }
}


void SPIClass::transferBytes_(const uint8_t * out, uint8_t * in, uint8_t size) {
    if (!((uint32_t)out & 3) && !((uint32_t)in & 3)) {
        // Input and output are both 32b aligned or NULL
        transferBytesAligned_(out, in, size);
    } else {
        // HW FIFO has 64b limit and ::transferBytes breaks up large xfers into 64byte chunks before calling this function
        // We know at this point at least one direction is misaligned, so use temporary buffer to align everything
        // No need for separate out and in aligned copies, we can overwrite our out copy with the input data safely
        uint8_t aligned[64]; // Stack vars will be 32b aligned
        if (out) {
            memcpy(aligned, out, size);
        }
        transferBytesAligned_(out ? aligned : nullptr, in ? aligned : nullptr, size);
        if (in) {
            memcpy(in, aligned, size);
        }
    }
}


#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SPI)
SPIClass SPI;
#endif
