#ifndef SPIDEF_H
#define SPIDEF_H


#ifndef LSBFIRST
#define LSBFIRST 0
#endif
#ifndef MSBFIRST
#define MSBFIRST 1
#endif


// AVR compatibility definitions
const uint8_t SPI_CLOCK_DIV4   = 4;
const uint8_t SPI_CLOCK_DIV16  = 16;
const uint8_t SPI_CLOCK_DIV64  = 64;
const uint8_t SPI_CLOCK_DIV128 = 128;
const uint8_t SPI_CLOCK_DIV2   = 2;
const uint8_t SPI_CLOCK_DIV8   = 8;
const uint8_t SPI_CLOCK_DIV32  = 32;

const uint8_t SPI_MODE0 = 0x00;
const uint8_t SPI_MODE1 = 0x04;
const uint8_t SPI_MODE2 = 0x08;
const uint8_t SPI_MODE3 = 0x0C;


#endif//SPIDEF_H
