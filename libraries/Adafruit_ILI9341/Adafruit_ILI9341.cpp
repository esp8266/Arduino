/***************************************************
  This is our library for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution

  Modified 09 May 2015 by Markus Sattler - rewrite the code add ESP8266 support and many optimizations now 220% fastet (320% total)
 ****************************************************/

#include "Adafruit_ILI9341.h"
#ifdef ESP8266
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif
#include <limits.h>
#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>

#ifdef ESP8266
#define hwSPI true
#endif

#define writeCmdDataTmp(cmd, ...)   {               \
        const uint8_t tmp##cmd##_[] = { __VA_ARGS__ };              \
        writeCmdData(cmd, (uint8_t *) &tmp##cmd##_[0], sizeof(tmp##cmd##_));  \
}


#ifndef ESP8266
// Constructor when using software SPI.  All output pins are configurable.
Adafruit_ILI9341::Adafruit_ILI9341(int8_t cs, int8_t dc, int8_t mosi,
				   int8_t sclk, int8_t rst, int8_t miso) : Adafruit_GFX(ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT) {
  _cs   = cs;
  _dc   = dc;
  _mosi  = mosi;
  _miso = miso;
  _sclk = sclk;
  _rst  = rst;
  hwSPI = false;
}
#endif

// Constructor when using hardware SPI.  Faster, but must use SPI pins
// specific to each board type (e.g. 11,13 for Uno, 51,52 for Mega, etc.)
#if defined(ILI9341_USE_HW_CS) || defined(ILI9341_USE_NO_CS)
Adafruit_ILI9341::Adafruit_ILI9341(int8_t dc, int8_t rst) : Adafruit_GFX(ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT) {
  _dc   = dc;
  _rst  = rst;
  hwSPI = true;
#ifdef ESP8266
  _dcMask = digitalPinToBitMask(_dc);
  _rstMask = digitalPinToBitMask(_rst);
#else
  _mosi  = _sclk = 0;
#endif
}
#else
  Adafruit_ILI9341::Adafruit_ILI9341(int8_t cs, int8_t dc, int8_t rst) : Adafruit_GFX(ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT) {
    _cs   = cs;
    _dc   = dc;
    _rst  = rst;
  #ifdef ESP8266
    _csMask = digitalPinToBitMask(_cs);
    _dcMask = digitalPinToBitMask(_dc);
    _rstMask = digitalPinToBitMask(_rst);
  #else
    hwSPI = true;
    _mosi  = _sclk = 0;
  #endif
  }
#endif


#ifdef ESP8266
void Adafruit_ILI9341::spiwrite16(uint16_t c) {
    SPI.write16(c, true);
}
#endif

void Adafruit_ILI9341::spiwrite(uint8_t c) {

  //Serial.print("0x"); Serial.print(c, HEX); Serial.print(", ");
#ifndef ESP8266
  if (hwSPI) {
#endif
#if defined (__AVR__)
      uint8_t backupSPCR = SPCR;
    SPCR = mySPCR;
    SPDR = c;
    while(!(SPSR & _BV(SPIF)));
    SPCR = backupSPCR;
#elif defined(TEENSYDUINO) || defined(ESP8266)
    SPI.write(c);
#elif defined (__arm__)
    SPI.setClockDivider(11); // 8-ish MHz (full! speed!)
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    SPI.transfer(c);
#endif
#ifndef ESP8266
  } else {
    // Fast SPI bitbang swiped from LPD8806 library
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      if(c & bit) {
	//digitalWrite(_mosi, HIGH);
	*mosiport |=  mosipinmask;
      } else {
	//digitalWrite(_mosi, LOW);
	*mosiport &= ~mosipinmask;
      }
      //digitalWrite(_sclk, HIGH);
      *clkport |=  clkpinmask;
      //digitalWrite(_sclk, LOW);
      *clkport &= ~clkpinmask;
    }
  }
#endif
}

void Adafruit_ILI9341::spiwriteBytes(uint8_t * data, uint32_t size) {
#ifdef ESP8266
    SPI.writeBytes(data, size);
#else
    while(size--) {
        spiwrite(*data);
        data++;
    }
#endif
}

void Adafruit_ILI9341::spiwritePattern(uint8_t * data, uint8_t size, uint32_t repeat) {
#ifdef ESP8266
    SPI.writePattern(data, size, repeat);
#else
    uint8_t * ptr;
    uint8_t i;
    while(repeat--) {
        ptr = data;
        i = size;
        while(i--) {
            spiwrite(*ptr);
            ptr++;
        }
    }
#endif
}


inline void Adafruit_ILI9341::spiCsLow(void) {
#ifdef ILI9341_USE_DIGITAL_WRITE
    digitalWrite(_cs, LOW);
#else
#ifdef ESP8266
#if !defined(ILI9341_USE_HW_CS) && !defined(ILI9341_USE_NO_CS)
    GPOC = _csMask;
#endif
#else
    *csport &= ~cspinmask;
#endif
#endif
}

inline void Adafruit_ILI9341::spiCsHigh(void) {
#ifdef ILI9341_USE_DIGITAL_WRITE
    digitalWrite(_cs, HIGH);
#else
#ifdef ESP8266
#if !defined(ILI9341_USE_HW_CS) && !defined(ILI9341_USE_NO_CS)
    GPOS = _csMask;
#endif
#else
    *csport |= cspinmask;
#endif
#endif
}

inline void Adafruit_ILI9341::spiDcLow(void){
#ifdef ILI9341_USE_DIGITAL_WRITE
    digitalWrite(_dc, LOW);
#else
#ifdef ESP8266
#ifndef USE_HW_CS
    GPOC = _dcMask;
#endif
#else
    *dcport &= ~dcpinmask;
#endif
#endif
}

inline void Adafruit_ILI9341::spiDcHigh(void) {
#ifdef ILI9341_USE_DIGITAL_WRITE
    digitalWrite(_dc, HIGH);
#else
#ifdef ESP8266
    GPOS = _dcMask;
#else
    *dcport |= dcpinmask;
#endif
#endif
}

void Adafruit_ILI9341::writecommand(uint8_t c) {
    spiDcLow();
    spiCsLow();

    spiwrite(c);

    spiCsHigh();
}

void Adafruit_ILI9341::writedata(uint8_t c) {
    spiDcHigh();
    spiCsLow();

    spiwrite(c);

    spiCsHigh();
} 

void Adafruit_ILI9341::writedata(uint8_t * data, uint8_t size) {
    spiDcHigh();
    spiCsLow();

    spiwriteBytes(data, size);

    spiCsHigh();
}

void Adafruit_ILI9341::writeCmdData(uint8_t cmd, uint8_t * data, uint8_t size) {
    spiDcLow();
    spiCsLow();

    spiwrite(cmd);

    spiDcHigh();

    spiwriteBytes(data, size);

    spiCsHigh();
}

uint16_t Adafruit_ILI9341::getHeight(void) {
    return _height;
}

uint16_t Adafruit_ILI9341::getWidth(void){
    return _width;
}

// If the SPI library has transaction support, these functions
// establish settings and protect from interference from other
// libraries.  Otherwise, they simply do nothing.
#ifdef SPI_HAS_TRANSACTION

#ifdef ESP8266
SPISettings spiSettings = SPISettings(ESP8266_CLOCK, MSBFIRST, SPI_MODE0);
#else
SPISettings spiSettings =  SPISettings(8000000, MSBFIRST, SPI_MODE0);
#endif

static inline void spi_begin(void) __attribute__((always_inline));
static inline void spi_begin(void) {
  SPI.beginTransaction(spiSettings);
}
static inline void spi_end(void) __attribute__((always_inline));
static inline void spi_end(void) {
  SPI.endTransaction();
}
#else
#define spi_begin()
#define spi_end()
#endif

// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in these tables
// stored in PROGMEM.  The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.
#define DELAY 0x80


// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
void Adafruit_ILI9341::commandList(uint8_t *addr) {

  uint8_t  numCommands, numArgs;
  uint16_t ms;

  numCommands = pgm_read_byte(addr++);   // Number of commands to follow
  while(numCommands--) {                 // For each command...
    writecommand(pgm_read_byte(addr++)); //   Read, issue command
    numArgs  = pgm_read_byte(addr++);    //   Number of args to follow
    ms       = numArgs & DELAY;          //   If hibit set, delay follows args
    numArgs &= ~DELAY;                   //   Mask out delay bit
    while(numArgs--) {                   //   For each argument...
      writedata(pgm_read_byte(addr++));  //     Read, issue argument
    }

    if(ms) {
      ms = pgm_read_byte(addr++); // Read post-command delay time (ms)
      if(ms == 255) ms = 500;     // If 255, delay for 500 ms
      delay(ms);
    }
  }
}


void Adafruit_ILI9341::begin(void) {
  if (_rst > NOT_A_PIN) {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, LOW);
  }

  pinMode(_dc, OUTPUT);
#ifndef USE_HW_CS
  pinMode(_cs, OUTPUT);
#endif
#ifndef ESP8266
#ifndef ILI9341_USE_DIGITAL_WRITE
  csport    = portOutputRegister(digitalPinToPort(_cs));
  cspinmask = digitalPinToBitMask(_cs);
  dcport    = portOutputRegister(digitalPinToPort(_dc));
  dcpinmask = digitalPinToBitMask(_dc);
#endif
  if(hwSPI) { // Using hardware SPI
#endif
#if defined (__AVR__)
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV2); // 8 MHz (full! speed!)
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    mySPCR = SPCR;
#elif defined(TEENSYDUINO)
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV2); // 8 MHz (full! speed!)
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
#elif defined (__arm__)
      SPI.begin();
      SPI.setClockDivider(11); // 8-ish MHz (full! speed!)
      SPI.setBitOrder(MSBFIRST);
      SPI.setDataMode(SPI_MODE0);
#elif defined (ESP8266)
      SPI.begin();
#ifdef USE_HW_CS
      SPI.setHwCs(true);
#endif
#endif
#ifndef ESP8266
  } else {
    pinMode(_sclk, OUTPUT);
    pinMode(_mosi, OUTPUT);
    pinMode(_miso, INPUT);
    clkport     = portOutputRegister(digitalPinToPort(_sclk));
    clkpinmask  = digitalPinToBitMask(_sclk);
    mosiport    = portOutputRegister(digitalPinToPort(_mosi));
    mosipinmask = digitalPinToBitMask(_mosi);
    *clkport   &= ~clkpinmask;
    *mosiport  &= ~mosipinmask;
  }
#endif
  // toggle RST low to reset
  if (_rst > NOT_A_PIN) {
    digitalWrite(_rst, HIGH);
    delay(5);
    digitalWrite(_rst, LOW);
    delay(20);
    digitalWrite(_rst, HIGH);
    delay(150);
  }

  /*
  uint8_t x = readcommand8(ILI9341_RDMODE);
  Serial.print("\nDisplay Power Mode: 0x"); Serial.println(x, HEX);
  x = readcommand8(ILI9341_RDMADCTL);
  Serial.print("\nMADCTL Mode: 0x"); Serial.println(x, HEX);
  x = readcommand8(ILI9341_RDPIXFMT);
  Serial.print("\nPixel Format: 0x"); Serial.println(x, HEX);
  x = readcommand8(ILI9341_RDIMGFMT);
  Serial.print("\nImage Format: 0x"); Serial.println(x, HEX);
  x = readcommand8(ILI9341_RDSELFDIAG);
  Serial.print("\nSelf Diagnostic: 0x"); Serial.println(x, HEX);
*/
  //if(cmdList) commandList(cmdList);

  if (hwSPI) spi_begin();

  writeCmdDataTmp(0xEF, 0x03, 0x80, 0x02);
  writeCmdDataTmp(0xCF, 0x00, 0XC1, 0X30);
  writeCmdDataTmp(0xED, 0x64, 0x03, 0X12, 0X81);
  writeCmdDataTmp(0xE8, 0x85, 0x00, 0x78);
  writeCmdDataTmp(0xCB, 0x39, 0x2C, 0x00, 0x34, 0x02);
  writeCmdDataTmp(0xF7, 0x20);
  writeCmdDataTmp(0xEA, 0x00, 0x00);

  //Powercontrol
  //VRH[5:0]
  writeCmdDataTmp(ILI9341_PWCTR1, 0x23);

  //Powercontrol
  //SAP[2:0];BT[3:0]
  writeCmdDataTmp(ILI9341_PWCTR2, 0x10);

  //VCMcontrol
  writeCmdDataTmp(ILI9341_VMCTR1, 0x3e, 0x28);

  //VCMcontrol2
  writeCmdDataTmp(ILI9341_VMCTR2, 0x86);

  //MemoryAccessControl
  writeCmdDataTmp(ILI9341_MADCTL, 0x48);

  writeCmdDataTmp(ILI9341_PIXFMT, 0x55);
  writeCmdDataTmp(ILI9341_FRMCTR1, 0x00, 0x18);

  //DisplayFunctionControl
  writeCmdDataTmp(ILI9341_DFUNCTR, 0x08, 0x82, 0x27);

  //3GammaFunctionDisable
  writeCmdDataTmp(0xF2, 0x00);
  
  //Gammacurveselected
  writeCmdDataTmp(ILI9341_GAMMASET, 0x01);
  
  //SetGamma
  writeCmdDataTmp(ILI9341_GMCTRP1, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00);
  writeCmdDataTmp(ILI9341_GMCTRN1, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F);

  writecommand(ILI9341_SLPOUT);    //Exit Sleep 
  if (hwSPI) spi_end();
  delay(120); 		
  if (hwSPI) spi_begin();
  writecommand(ILI9341_DISPON);    //Display on 
  if (hwSPI) spi_end();

}


void Adafruit_ILI9341::area_update_start(uint32_t x, uint32_t y, uint32_t w, uint32_t h) {
    spiCsLow();
    setAddrWindow_(x, y, x + w - 1, y + h - 1);
}

void Adafruit_ILI9341::area_update_data(uint8_t *data, uint32_t pixel){
    spiwriteBytes(&data[0], (pixel*2));
}

void Adafruit_ILI9341::area_update_end(void){
    spiCsHigh();
}

void Adafruit_ILI9341::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {
    spiCsLow();
    setAddrWindow_(x0, y0, x1, y1);
    spiCsHigh();
}

void Adafruit_ILI9341::setAddrWindow_(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1) {

    uint8_t buffC[] = { (uint8_t) (x0 >> 8), (uint8_t) x0, (uint8_t) (x1 >> 8), (uint8_t) x1 };
    uint8_t buffP[] = { (uint8_t) (y0 >> 8), (uint8_t) y0, (uint8_t) (y1 >> 8), (uint8_t) y1 };

    spiDcLow();
    spiwrite(ILI9341_CASET);
    spiDcHigh();
    spiwriteBytes(&buffC[0], sizeof(buffC));

    spiDcLow();
    spiwrite(ILI9341_PASET);
    spiDcHigh();
    spiwriteBytes(&buffP[0], sizeof(buffP));

    spiDcLow();
    spiwrite(ILI9341_RAMWR);
    spiDcHigh();

}


void Adafruit_ILI9341::pushColor(uint16_t color) {
  if (hwSPI) spi_begin();

  spiDcHigh();
  spiCsLow();

#ifdef ESP8266
  spiwrite16(color);
#else
  spiwrite(color >> 8);
  spiwrite(color);
#endif

  spiCsHigh();

  if (hwSPI) spi_end();
}

void Adafruit_ILI9341::drawPixel(int16_t x, int16_t y, uint16_t color) {

    if((x < 0) || (x >= _width) || (y < 0) || (y >= _height)) {
        return;
    }

    if(hwSPI) {
        spi_begin();
    }

    spiCsLow();

    setAddrWindow_(x, y, x + 1, y + 1);

#ifdef ESP8266
    spiwrite16(color);
#else
    spiwrite(color >> 8);
    spiwrite(color);
#endif

    spiCsHigh();

    if(hwSPI) {
        spi_end();
    }
}


void Adafruit_ILI9341::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) {

    // Rudimentary clipping
    if((x >= _width) || (y >= _height)) return;
    if((y + h - 1) >= _height)   h = _height - y;

    if(hwSPI) {
        spi_begin();
    }

    spiCsLow();

    setAddrWindow_(x, y, x, (y + h - 1));

    uint8_t colorBin[] = { (uint8_t) (color >> 8), (uint8_t) color };
    spiwritePattern(&colorBin[0], 2, h);

    spiCsHigh();

    if(hwSPI) {
        spi_end();
    }
}

void Adafruit_ILI9341::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) {

    // Rudimentary clipping
    if((x >= _width) || (y >= _height)) return;
    if((x+w-1) >= _width)  w = _width-x;

    if(hwSPI) {
        spi_begin();
    }

    spiDcHigh();
    spiCsLow();

    setAddrWindow_(x, y, (x + w - 1), y);

    uint8_t colorBin[] = { (uint8_t) (color >> 8), (uint8_t) color };
    spiwritePattern(&colorBin[0], 2, w);

    spiCsHigh();

    if(hwSPI) {
        spi_end();
    }
}

void Adafruit_ILI9341::fillScreen(uint16_t color) {
    fillRect(0, 0, _width, _height, color);
}

// fill a rectangle
void Adafruit_ILI9341::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {

    // rudimentary clipping (drawChar w/big text requires this)
    if((x >= _width) || (y >= _height))
        return;
    if((x + w - 1) >= _width)
        w = _width - x;
    if((y + h - 1) >= _height)
        h = _height - y;

    if(hwSPI) {
        spi_begin();
    }

    spiCsLow();

    setAddrWindow_(x, y, (x + w - 1), (y + h - 1));

    uint8_t colorBin[] = { (uint8_t) (color >> 8), (uint8_t) color };
    spiwritePattern(&colorBin[0], 2, (w * h));

    spiCsHigh();

    if(hwSPI) {
        spi_end();
    }
}


// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t Adafruit_ILI9341::color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH  0x04

void Adafruit_ILI9341::setRotation(uint8_t m) {

  if (hwSPI) spi_begin();
  writecommand(ILI9341_MADCTL);
  rotation = m % 4; // can't be higher than 3
  switch (rotation) {
   case 0:
     writedata(MADCTL_MX | MADCTL_BGR);
     _width  = ILI9341_TFTWIDTH;
     _height = ILI9341_TFTHEIGHT;
     break;
   case 1:
     writedata(MADCTL_MV | MADCTL_BGR);
     _width  = ILI9341_TFTHEIGHT;
     _height = ILI9341_TFTWIDTH;
     break;
  case 2:
    writedata(MADCTL_MY | MADCTL_BGR);
     _width  = ILI9341_TFTWIDTH;
     _height = ILI9341_TFTHEIGHT;
    break;
   case 3:
     writedata(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
     _width  = ILI9341_TFTHEIGHT;
     _height = ILI9341_TFTWIDTH;
     break;
  }
  if (hwSPI) spi_end();
}


void Adafruit_ILI9341::invertDisplay(boolean i) {
  if (hwSPI) spi_begin();
  writecommand(i ? ILI9341_INVON : ILI9341_INVOFF);
  if (hwSPI) spi_end();
}


////////// stuff not actively being used, but kept for posterity


uint8_t Adafruit_ILI9341::spiread(void) {
  uint8_t r = 0;

  if (hwSPI) {
#if defined (__AVR__)
    uint8_t backupSPCR = SPCR;
    SPCR = mySPCR;
    SPDR = 0x00;
    while(!(SPSR & _BV(SPIF)));
    r = SPDR;
    SPCR = backupSPCR;
#elif defined(TEENSYDUINO)
    r = SPI.transfer(0x00);
#elif defined (__arm__)
    SPI.setClockDivider(11); // 8-ish MHz (full! speed!)
    SPI.setBitOrder(MSBFIRST);
    SPI.setDataMode(SPI_MODE0);
    r = SPI.transfer(0x00);
#else
    r = SPI.transfer(0x00);
#endif
  } else {
#ifndef ESP8266
    for (uint8_t i=0; i<8; i++) {
      digitalWrite(_sclk, LOW);
      digitalWrite(_sclk, HIGH);
      r <<= 1;
      if (digitalRead(_miso))
	r |= 0x1;
    }
#endif
  }
  //Serial.print("read: 0x"); Serial.print(r, HEX);
  
  return r;
}

uint8_t Adafruit_ILI9341::readdata(void) {
    if(hwSPI) spi_begin();
    spiCsLow();
    spiDcLow();
    uint8_t r = spiread();
    spiCsHigh();
    if(hwSPI) spi_end();
    return r;
}
 
uint8_t Adafruit_ILI9341::readcommand8(uint8_t c, uint8_t index) {
    if(hwSPI) spi_begin();

    spiCsLow();
    spiDcLow();

    spiwrite(0xD9);  // woo sekret command?
    spiDcHigh();
    spiwrite(0x10 + index);

#ifndef ESP8266
    digitalWrite(_sclk, LOW);
#endif

 	spiDcLow();
 	spiwrite(c);

    spiDcHigh();
    uint8_t r = spiread();
    spiCsHigh();

    if(hwSPI) spi_end();
    return r;
}
 
/*

 uint16_t Adafruit_ILI9341::readcommand16(uint8_t c) {
 digitalWrite(_dc, LOW);
 if (_cs)
 digitalWrite(_cs, LOW);
 
 spiwrite(c);
 pinMode(_sid, INPUT); // input!
 uint16_t r = spiread();
 r <<= 8;
 r |= spiread();
 if (_cs)
 digitalWrite(_cs, HIGH);
 
 pinMode(_sid, OUTPUT); // back to output
 return r;
 }
 
 uint32_t Adafruit_ILI9341::readcommand32(uint8_t c) {
 digitalWrite(_dc, LOW);
 if (_cs)
 digitalWrite(_cs, LOW);
 spiwrite(c);
 pinMode(_sid, INPUT); // input!
 
 dummyclock();
 dummyclock();
 
 uint32_t r = spiread();
 r <<= 8;
 r |= spiread();
 r <<= 8;
 r |= spiread();
 r <<= 8;
 r |= spiread();
 if (_cs)
 digitalWrite(_cs, HIGH);
 
 pinMode(_sid, OUTPUT); // back to output
 return r;
 }
 
 */
