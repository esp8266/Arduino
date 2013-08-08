/* Arduino FAT16 Library
 * Copyright (C) 2008 by William Greiman
 *
 * This file is part of the Arduino FAT16 Library
 *
 * This Library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This Library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with the Arduino Fat16 Library.  If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef SdCard_h
#define SdCard_h
 /**
  * \file
  * SdCard class
  */
#include <SdInfo.h>
//------------------------------------------------------------------------------
// Warning only SD_CHIP_SELECT_PIN, the SD card select pin, may be redefined.
// define hardware SPI pins
#if defined(__AVR_ATmega168__)\
||defined(__AVR_ATmega168P__)\
||defined(__AVR_ATmega328P__)
// 168 and 328 Arduinos
/** Slave Select pin */
uint8_t const SPI_SS_PIN   = 10;
/** Master Out Slave In pin */
uint8_t const SPI_MOSI_PIN = 11;
/** Master In Slave Out pin */
uint8_t const SPI_MISO_PIN = 12;
/** Serial Clock */
uint8_t const SPI_SCK_PIN  = 13;
//------------------------------------------------------------------------------
#elif defined(__AVR_ATmega1280__)\
|| defined(__AVR_ATmega2560__)
// pins for Arduino Mega
uint8_t const SPI_SS_PIN   = 53;
uint8_t const SPI_MOSI_PIN = 51;
uint8_t const SPI_MISO_PIN = 50;
uint8_t const SPI_SCK_PIN  = 52;
//------------------------------------------------------------------------------
#elif defined(__AVR_ATmega644P__)\
|| defined(__AVR_ATmega644__)\
|| defined(__AVR_ATmega1284P__)
// pins for Sanguino
uint8_t const SPI_SS_PIN   = 4;
uint8_t const SPI_MOSI_PIN = 5;
uint8_t const SPI_MISO_PIN = 6;
uint8_t const SPI_SCK_PIN  = 7;
//------------------------------------------------------------------------------
#elif defined(__AVR_ATmega32U4__)
// pins for Teensy 2.0
uint8_t const SPI_SS_PIN   = 8;
uint8_t const SPI_MOSI_PIN = 16;
uint8_t const SPI_MISO_PIN = 14;
uint8_t const SPI_SCK_PIN  = 15;
//------------------------------------------------------------------------------
#elif defined(__AVR_AT90USB646__)\
|| defined(__AVR_AT90USB1286__)
// pins for Teensy++ 1.0 & 2.0
uint8_t const SPI_SS_PIN   = 20;
uint8_t const SPI_MOSI_PIN = 22;
uint8_t const SPI_MISO_PIN = 23;
uint8_t const SPI_SCK_PIN  = 21;
//------------------------------------------------------------------------------
#else  // SPI pins
#error unknown CPU
#endif  // SPI pins
//------------------------------------------------------------------------------
/**
 * SD Chip Select pin
 *
 * Warning if this pin is redefined the hardware SS pin will be enabled
 * as an output by init().  An avr processor will not function as an SPI
 * master unless SS is set to output mode.
 *
 * For example to set SD_CHIP_SELECT_PIN to 8 for the SparkFun microSD shield:
 * uint8_t const SD_CHIP_SELECT_PIN = 8;
 *
 * The default chip select pin for the SD card is SS.
 */
uint8_t const SD_CHIP_SELECT_PIN = SPI_SS_PIN;
//------------------------------------------------------------------------------
/** command timeout ms */
uint16_t const SD_COMMAND_TIMEOUT = 300;
/** init timeout ms */
uint16_t const SD_INIT_TIMEOUT    = 2000;
/** read timeout ms */
uint16_t const SD_READ_TIMEOUT    = 300;
/** write timeout ms */
uint16_t const SD_WRITE_TIMEOUT   = 600;
//------------------------------------------------------------------------------
// error codes
/** Card did not go into SPI mode */
uint8_t const SD_ERROR_CMD0              = 1;
/** Card did not go ready */
uint8_t const SD_ERROR_ACMD41            = 2;
/** Write command not accepted */
uint8_t const SD_ERROR_CMD24             = 3;
/** Read command not accepted */
uint8_t const SD_ERROR_CMD17             = 4;
/** timeout waiting for read data */
uint8_t const SD_ERROR_READ_TIMEOUT      = 5;
/** write error occurred */
uint8_t const SD_ERROR_WRITE_RESPONSE    = 6;
/** timeout waiting for write status */
uint8_t const SD_ERROR_WRITE_TIMEOUT     = 7;
/** attempt to write block zero */
uint8_t const SD_ERROR_BLOCK_ZERO_WRITE  = 8;
/** card returned an error to a CMD13 status check after a write */
uint8_t const SD_ERROR_WRITE_PROGRAMMING = 9;
/** invalid SPI speed in init() call */
uint8_t const SD_ERROR_SPI_SPEED         = 10;
//------------------------------------------------------------------------------
// SD command codes
/** SEND OPERATING CONDITIONS */
uint8_t const ACMD41   = 0X29;
/** GO_IDLE_STATE - init card in spi mode if CS low */
uint8_t const CMD0     = 0X00;
/** SEND_CSD - Card Specific Data */
uint8_t const CMD9     = 0X09;
/** SEND_CID - Card IDentification */
uint8_t const CMD10    = 0X0A;
/** SEND_STATUS - read the card status register */
uint8_t const CMD13    = 0X0D;
/** READ_BLOCK */
uint8_t const CMD17    = 0X11;
/** WRITE_BLOCK */
uint8_t const CMD24    = 0X18;
/** APP_CMD - escape for application specific command */
uint8_t const CMD55    = 0X37;
//------------------------------------------------------------------------------
/**
 * \class SdCard
 * \brief Hardware access class for SD flash cards
 *
 * Supports raw access to a standard SD flash memory card.
 *
 */
class SdCard  {
 public:
  /** Code for a SD error. See SdCard.h for definitions. */
  uint8_t errorCode;
  /** Data that may be helpful in determining the cause of an error */
  uint8_t errorData;
  uint32_t cardSize(void);
  /**
   * Initialize an SD flash memory card with default clock rate and chip
   * select pin.  See SdCard::init(uint8_t sckRateID, uint8_t chipSelectPin).
   */
  uint8_t init(void) {
    return init(0, SD_CHIP_SELECT_PIN);
  }
  /**
   * Initialize an SD flash memory card with the selected SPI clock rate
   * and the default SD chip select pin.
   * See SdCard::init(uint8_t slow, uint8_t chipSelectPin).
   */
  uint8_t init(uint8_t speed) {
    return init(speed, SD_CHIP_SELECT_PIN);
  }
  uint8_t init(uint8_t speed, uint8_t chipselectPin);
  uint8_t readBlock(uint32_t block, uint8_t* dst);
  /** Read the CID register which contains info about the card.
   *  This includes Manufacturer ID, OEM ID, product name, version,
   *  serial number, and manufacturing date. */
  uint8_t readCID(cid_t* cid) {
    return readReg(CMD10, cid);
  }
  uint8_t writeBlock(uint32_t block, const uint8_t* src);
 private:
  uint8_t cardAcmd(uint8_t cmd, uint32_t arg);
  uint8_t cardCommand(uint8_t cmd, uint32_t arg);
  uint8_t chipSelectPin_;
  uint8_t speed_;
  void chipSelectHigh(void);
  void chipSelectLow(void);
  void error(uint8_t code, uint8_t data);
  void error(uint8_t code);
  uint8_t readReg(uint8_t cmd, void* buf);
  uint8_t readTransfer(uint8_t* dst, uint16_t count);
};
#endif  // SdCard_h
