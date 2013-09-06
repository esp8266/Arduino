/***************************************************
  This is a library for the Adafruit 1.8" SPI display.
  This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
  as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618
 
  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#ifndef _ARDUINO_LCDH_
#define _ARDUINO_LCDH_

#if ARDUINO >= 100
 #include "Arduino.h"
 #include "Print.h"
#else
 #include "WProgram.h"
#endif
#include "utility/Adafruit_GFX.h"
//#include <avr/pgmspace.h>

// some flags for initR() :(
#define INITR_GREENTAB 0x0
#define INITR_REDTAB   0x1

#define ILI9163C_TFTWIDTH  128
#define ILI9163C_TFTHEIGHT 160

#define ILI9163C_NOP     0x00
#define ILI9163C_SWRESET 0x01
#define ILI9163C_RDDID   0x04
#define ILI9163C_RDDST   0x09

#define ILI9163C_SLPIN   0x10
#define ILI9163C_SLPOUT  0x11
#define ILI9163C_PTLON   0x12
#define ILI9163C_NORON   0x13

#define ILI9163C_INVOFF  0x20
#define ILI9163C_INVON   0x21
#define ILI9163C_DISPOFF 0x28
#define ILI9163C_DISPON  0x29
#define ILI9163C_CASET   0x2A
#define ILI9163C_RASET   0x2B
#define ILI9163C_RAMWR   0x2C
#define ILI9163C_RAMRD   0x2E

#define ILI9163C_PTLAR   0x30
#define ILI9163C_COLMOD  0x3A // this is interface pixel format, this might be the issue
#define ILI9163C_MADCTL  0x36

#define ILI9163C_FRMCTR1 0xB1
#define ILI9163C_FRMCTR2 0xB2
#define ILI9163C_FRMCTR3 0xB3
#define ILI9163C_INVCTR  0xB4
#define ILI9163C_DISSET5 0xB6

#define ILI9163C_PWCTR1  0xC0
#define ILI9163C_PWCTR2  0xC1
#define ILI9163C_PWCTR3  0xC2
#define ILI9163C_PWCTR4  0xC3
#define ILI9163C_PWCTR5  0xC4
#define ILI9163C_VMCTR1  0xC5

#define ILI9163C_RDID1   0xDA
#define ILI9163C_RDID2   0xDB
#define ILI9163C_RDID3   0xDC
#define ILI9163C_RDID4   0xDD

#define ILI9163C_PWCTR6  0xFC

#define ILI9163C_GMCTRP1 0xE0
#define ILI9163C_GMCTRN1 0xE1

// Color definitions
#define	ILI9163C_BLACK   0x0000
#define	ILI9163C_BLUE    0x001F
#define	ILI9163C_RED     0xF800
#define	ILI9163C_GREEN   0x07E0
#define ILI9163C_CYAN    0x07FF
#define ILI9163C_MAGENTA 0xF81F
#define ILI9163C_YELLOW  0xFFE0  
#define ILI9163C_WHITE   0xFFFF


class Arduino_LCD : public Adafruit_GFX {

 public:

  Arduino_LCD(uint8_t CS, uint8_t RS, uint8_t SID, uint8_t SCLK, uint8_t RST);
  Arduino_LCD(uint8_t CS, uint8_t RS, uint8_t RST);

  void     initB(void),                             // for ST7735B displays
           initR(uint8_t options = INITR_GREENTAB), // for ST7735R
           setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1),
           pushColor(uint16_t color),
           fillScreen(uint16_t color),
           drawPixel(int16_t x, int16_t y, uint16_t color),
           drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color),
           drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color),
           fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color),
           setRotation(uint8_t r),
           invertDisplay(boolean i);
  uint16_t Color565(uint8_t r, uint8_t g, uint8_t b);

  /* These are not for current use, 8-bit protocol only!
  uint8_t  readdata(void),
           readcommand8(uint8_t);
  uint16_t readcommand16(uint8_t);
  uint32_t readcommand32(uint8_t);
  void     dummyclock(void);
  */

 private:

  void     spiwrite(uint8_t),
           writecommand(uint8_t c),
           writedata(uint8_t d),
//           commandList(prog_uchar *addr),
//           commonInit(prog_uchar *cmdList);
           commandList(uint8_t *addr),
           commonInit(uint8_t *cmdList);
//uint8_t  spiread(void);

  boolean  hwSPI;
  volatile uint8_t *dataport, *clkport, *csport, *rsport;
  uint8_t  _cs, _rs, _rst, _sid, _sclk,
           datapinmask, clkpinmask, cspinmask, rspinmask,
           colstart, rowstart; // some displays need this changed
};

#endif
