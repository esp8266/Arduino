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

#include "Arduino_LCD.h"
//#include <avr/pgmspace.h>
#include <limits.h>
//#include "pins_arduino.h"
#include "wiring_private.h"
#include <SPI.h>


// Constructor when using software SPI.  All output pins are configurable.
Arduino_LCD::Arduino_LCD(uint8_t cs, uint8_t rs, uint8_t sid, uint8_t sclk,
  uint8_t rst) : Adafruit_GFX(ILI9163C_TFTWIDTH, ILI9163C_TFTHEIGHT)
{
  _cs   = cs;
  _rs   = rs;
  _sid  = sid;
  _sclk = sclk;
  _rst  = rst;
  hwSPI = false;
}


// Constructor when using hardware SPI.  Faster, but must use SPI pins
// specific to each board type (e.g. 11,13 for Uno, 51,52 for Mega, etc.)
Arduino_LCD::Arduino_LCD(uint8_t cs, uint8_t rs, uint8_t rst) :
    Adafruit_GFX(ILI9163C_TFTWIDTH, ILI9163C_TFTHEIGHT) {
  _cs   = cs;
  _rs   = rs;
  _rst  = rst;
  hwSPI = true;
  _sid  = _sclk = 0;
}


inline void Arduino_LCD::spiwrite(uint8_t c) {

  //Serial.println(c, HEX);

/*  if (hwSPI) {
    SPDR = c;
    while(!(SPSR & _BV(SPIF)));
  } else {
    // Fast SPI bitbang swiped from LPD8806 library
    for(uint8_t bit = 0x80; bit; bit >>= 1) {
      if(c & bit) *dataport |=  datapinmask;
      else        *dataport &= ~datapinmask;
      *clkport |=  clkpinmask;
      *clkport &= ~clkpinmask;
    }
  }
*/
SPI.transfer(c);
}


void Arduino_LCD::writecommand(uint8_t c) {
//  *rsport &= ~rspinmask;
//  *csport &= ~cspinmask;
digitalWrite(_rs, LOW);
digitalWrite(_cs, LOW);

  //Serial.print("C ");
  spiwrite(c);
//SPI.transfer(c);
//  *csport |= cspinmask;
digitalWrite(_cs, HIGH);
}


void Arduino_LCD::writedata(uint8_t c) {
//  *rsport &= ~rspinmask;
//  *csport &= ~cspinmask;
digitalWrite(_rs, HIGH);
digitalWrite(_cs, LOW);

  //Serial.print("D ");
  spiwrite(c);
//SPI.transfer(c);
//  *csport |= cspinmask;
digitalWrite(_cs, HIGH);
} 


// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in these tables
// stored in PROGMEM.  The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.
#define DELAY 0x80
//PROGMEM static prog_uchar
/*uint8_t
  Bcmd[] = {                  // Initialization commands for 7735B screens
   18,                       // 18 commands in list:
    ILI9163C_SWRESET,   DELAY,  //  1: Software reset, no args, w/delay
      50,                     //     50 ms delay
    ILI9163C_SLPOUT ,   DELAY,  //  2: Out of sleep mode, no args, w/delay
      255,                    //     255 = 500 ms delay
    ILI9163C_COLMOD , 1+DELAY,  //  3: Set color mode, 1 arg + delay: // I THINK THERE WAS SOMETHING HERE BECAUSE THE COMMAND IS CALLED 3A on Adafruits
      0x05,                   //     16-bit color
      10,                     //     10 ms delay
    ILI9163C_FRMCTR1, 3+DELAY,  //  4: Frame rate control, 3 args + delay:
      0x00,                   //     fastest refresh
      0x06,                   //     6 lines front porch
      0x03,                   //     3 lines back porch
      10,                     //     10 ms delay
    ILI9163C_MADCTL , 1      ,  //  5: Memory access ctrl (directions), 1 arg:
      0x08,                   //     Row addr/col addr, bottom to top refresh
    ILI9163C_DISSET5, 2      ,  //  6: Display settings #5, 2 args, no delay:
      0x15,                   //     1 clk cycle nonoverlap, 2 cycle gate
                              //     rise, 3 cycle osc equalize
      0x02,                   //     Fix on VTL
    ILI9163C_INVCTR , 1      ,  //  7: Display inversion control, 1 arg:
      0x0,                    //     Line inversion
    ILI9163C_PWCTR1 , 2+DELAY,  //  8: Power control, 2 args + delay:
      0x02,                   //     GVDD = 4.7V
      0x70,                   //     1.0uA
      10,                     //     10 ms delay
    ILI9163C_PWCTR2 , 1      ,  //  9: Power control, 1 arg, no delay:
      0x05,                   //     VGH = 14.7V, VGL = -7.35V
    ILI9163C_PWCTR3 , 2      ,  // 10: Power control, 2 args, no delay:
      0x01,                   //     Opamp current small
      0x02,                   //     Boost frequency
    ILI9163C_VMCTR1 , 2+DELAY,  // 11: Power control, 2 args + delay:
      0x3C,                   //     VCOMH = 4V
      0x38,                   //     VCOML = -1.1V
      10,                     //     10 ms delay
    ILI9163C_PWCTR6 , 2      ,  // 12: Power control, 2 args, no delay:
      0x11, 0x15,
    ILI9163C_GMCTRP1,16      ,  // 13: Magical unicorn dust, 16 args, no delay:
      0x09, 0x16, 0x09, 0x20, //     (seriously though, not sure what
      0x21, 0x1B, 0x13, 0x19, //      these config values represent)
      0x17, 0x15, 0x1E, 0x2B,
      0x04, 0x05, 0x02, 0x0E,
    ILI9163C_GMCTRN1,16+DELAY,  // 14: Sparkles and rainbows, 16 args + delay:
      0x0B, 0x14, 0x08, 0x1E, //     (ditto)
      0x22, 0x1D, 0x18, 0x1E,
      0x1B, 0x1A, 0x24, 0x2B,
      0x06, 0x06, 0x02, 0x0F,
      10,                     //     10 ms delay
    ILI9163C_CASET  , 4      ,  // 15: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 2
      0x00, 0x81,             //     XEND = 129
    ILI9163C_RASET  , 4      ,  // 16: Row addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 1
      0x00, 0x81,             //     XEND = 160
    ILI9163C_NORON  ,   DELAY,  // 17: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ILI9163C_DISPON ,   DELAY,  // 18: Main screen turn on, no args, w/delay
      255 },                  //     255 = 500 ms delay
*/
uint8_t
  Bcmd[] = {                    // Initialization commands for 7735B screens
   19,                          // 19 commands in list:
    ILI9163C_SWRESET,   DELAY,  // 1: Software reset, no args, w/delay
      50,                       //     50 ms delay
    0x11 ,   DELAY,  		// 2: Out of sleep mode, no args, w/delay
      100,                      //     255 = 500 ms delay
    0x26 , 1,  			// 3: Set default gamma
      0x04,                     //     16-bit color
    0xb1, 2,              	// 4: Frame Rate
      0x0b,
      0x14,
    0xc0, 2,                    // 5: VRH1[4:0] & VC[2:0]
      0x08,
      0x00,
    0xc1, 1,                    // 6: BT[2:0]
      0x05,
    0xc5, 2,                    // 7: VMH[6:0] & VML[6:0]
      0x41,
      0x30,
    0xc7, 1,                    // 8: LCD Driving control
      0xc1,
    0xEC, 1,                    // 9: Set pumping color freq
      0x1b,
    0x3a , 1 + DELAY,  	        // 10: Set color format
      0x55,                     //     16-bit color
      100,
    0x2a, 4,                    // 11: Set Column Address
      0x00,
      0x00,
      0x00,
      0x7f,
    0x2b, 4,                    // 12: Set Page Address
      0x00,
      0x00,
      0x00,
      0x9f,
    0x36, 1,                    // 12+1: Set Scanning Direction
      0xc8,
    0xb7, 1,			// 14: Set Source Output Direciton
      0x00,
    0xf2, 1,			// 15: Enable Gamma bit
      0x01,
    0xe0, 15 + DELAY,		// 16: magic
      0x28, 0x24, 0x22, 0x31,
      0x2b, 0x0e, 0x53, 0xa5,
      0x42, 0x16, 0x18, 0x12,
      0x1a, 0x14, 0x03,
      50,
    0xe1, 15 + DELAY,		// 17: more magic
      0x17, 0x1b, 0x1d, 0x0e,
      0x14, 0x11, 0x2c, 0xa5,
      0x3d, 0x09, 0x27, 0x2d,
      0x25, 0x2b, 0x3c, 
      50, 
    ILI9163C_NORON  ,    DELAY, // 18: Normal display on, no args, w/delay
      10,                       //     10 ms delay
    ILI9163C_DISPON ,    DELAY, // 19: Main screen turn on, no args w/delay
      100 },                    //     100 ms delay
Rcmd1[] = {                   // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ILI9163C_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ILI9163C_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ILI9163C_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ILI9163C_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ILI9163C_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ILI9163C_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ILI9163C_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ILI9163C_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ILI9163C_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ILI9163C_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,  
    ILI9163C_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ILI9163C_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ILI9163C_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ILI9163C_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      0xC8,                   //     row addr/col addr, bottom to top refresh
    ILI9163C_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 },                 //     16-bit color

  Rcmd2green[] = {            // Init for 7735R, part 2 (green tab only)
    2,                        //  2 commands in list:
    ILI9163C_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x02,             //     XSTART = 0
      0x00, 0x7F+0x02,        //     XEND = 127
    ILI9163C_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x01,             //     XSTART = 0
      0x00, 0x9F+0x01 },      //     XEND = 159
  Rcmd2red[] = {              // Init for 7735R, part 2 (red tab only)
    2,                        //  2 commands in list:
    ILI9163C_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ILI9163C_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F },           //     XEND = 159

  Rcmd3[] = {                 // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ILI9163C_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ILI9163C_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ILI9163C_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ILI9163C_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay


// Companion code to the above tables.  Reads and issues
// a series of LCD commands stored in PROGMEM byte array.
//void Arduino_LCD::commandList(prog_uchar *addr) {
void Arduino_LCD::commandList(uint8_t *addr) {

  uint8_t  numCommands, numArgs;
  uint16_t ms;

  numCommands = *addr++;   // Number of commands to follow
  while(numCommands--) {                 // For each command...
    writecommand(*addr++); //   Read, issue command
    numArgs  = *addr++;    //   Number of args to follow
    ms       = numArgs & DELAY;          //   If hibit set, delay follows args
    numArgs &= ~DELAY;                   //   Mask out delay bit
    while(numArgs--) {                   //   For each argument...
      writedata(*addr++);  //     Read, issue argument
    }

    if(ms) {
      ms = *addr++; // Read post-command delay time (ms)
      if(ms == 255) ms = 500;     // If 255, delay for 500 ms
      delay(ms);
    }
  }
}


// Initialization code common to both 'B' and 'R' type displays
//void Arduino_LCD::commonInit(prog_uchar *cmdList) {
void Arduino_LCD::commonInit(uint8_t *cmdList) {

  colstart = rowstart = 0; // May be overridden in init func

  pinMode(_rs, OUTPUT);
  pinMode(_cs, OUTPUT);
/*
  csport    = portOutputRegister(digitalPinToPort(_cs));
  cspinmask = digitalPinToBitMask(_cs);
  rsport    = portOutputRegister(digitalPinToPort(_rs));
  rspinmask = digitalPinToBitMask(_rs);
*/

//  if(hwSPI) { // Using hardware SPI
    SPI.begin();
    SPI.setClockDivider(21); // 4 MHz (half speed)
//    SPI.setClockDivider(SPI_CLOCK_DIV4); // 4 MHz (half speed)
//    SPI.setBitOrder(MSBFIRST);
// there is no setBitOrder on the SPI library for the Due
    SPI.setDataMode(SPI_MODE0);
/*
  } else {
    pinMode(_sclk, OUTPUT);
    pinMode(_sid , OUTPUT);
    clkport     = portOutputRegister(digitalPinToPort(_sclk));
    clkpinmask  = digitalPinToBitMask(_sclk);
    dataport    = portOutputRegister(digitalPinToPort(_sid));
    datapinmask = digitalPinToBitMask(_sid);
    *clkport   &= ~clkpinmask;
    *dataport  &= ~datapinmask;
  }
*/

  // toggle RST low to reset; CS low so it'll listen to us
//  *csport &= ~cspinmask;
  digitalWrite(_cs, LOW);
  if (_rst) {
    pinMode(_rst, OUTPUT);
    digitalWrite(_rst, HIGH);
    delay(500);
    digitalWrite(_rst, LOW);
    delay(500);
    digitalWrite(_rst, HIGH);
    delay(500);
  }

  if(cmdList) commandList(cmdList);
}


// Initialization for ST7735B screens
void Arduino_LCD::initB(void) {
  commonInit(Bcmd);
  commandList(Rcmd3);
}


// Initialization for ST7735R screens (green or red tabs)
void Arduino_LCD::initR(uint8_t options) {
  commonInit(Rcmd1);
  if(options == INITR_GREENTAB) {
    commandList(Rcmd2green);
    colstart = 2;
    rowstart = 1;
  } else {
    // colstart, rowstart left at default '0' values
    commandList(Rcmd2red);
  }
  commandList(Rcmd3);
}


void Arduino_LCD::setAddrWindow(uint8_t x0, uint8_t y0, uint8_t x1,
 uint8_t y1) {

  writecommand(ILI9163C_CASET); // Column addr set
  writedata(0x00);
  writedata(x0+colstart);     // XSTART 
  writedata(0x00);
  writedata(x1+colstart);     // XEND

  writecommand(ILI9163C_RASET); // Row addr set
  writedata(0x00);
  writedata(y0+rowstart);     // YSTART
  writedata(0x00);
  writedata(y1+rowstart);     // YEND

  writecommand(ILI9163C_RAMWR); // write to RAM
}


void Arduino_LCD::fillScreen(uint16_t color) {

  uint8_t x, y, hi = color >> 8, lo = color;

  setAddrWindow(0, 0, _width-1, _height-1);

//  *rsport |=  rspinmask;
//  *csport &= ~cspinmask;
digitalWrite(_rs, HIGH);
  digitalWrite(_cs, LOW);

  for(y=_height; y>0; y--) {
    for(x=_width; x>0; x--) {
//SPI.transfer(hi);
//SPI.transfer(lo);
      spiwrite(hi);
      spiwrite(lo);
    }
  }

//  *csport |= cspinmask;
  digitalWrite(_cs, HIGH);
}


void Arduino_LCD::pushColor(uint16_t color) {
//  *rsport |=  rspinmask;
//  *csport &= ~cspinmask;
digitalWrite(_rs, HIGH);
  digitalWrite(_cs, LOW);

  spiwrite(color >> 8);
  spiwrite(color);
//SPI.transfer(color>>8);
//SPI.transfer(color);

//  *csport |= cspinmask;
  digitalWrite(_cs, HIGH);
}


void Arduino_LCD::drawPixel(int16_t x, int16_t y, uint16_t color) {

  if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;

  setAddrWindow(x,y,x+1,y+1);

//  *rsport |=  rspinmask;
//  *csport &= ~cspinmask;
digitalWrite(_rs, HIGH);
  digitalWrite(_cs, LOW);

  spiwrite(color >> 8);
  spiwrite(color);
//SPI.transfer(color>>8);
//SPI.transfer(color);

//  *csport |= cspinmask;
  digitalWrite(_cs, HIGH);
}


void Arduino_LCD::drawFastVLine(int16_t x, int16_t y, int16_t h,
 uint16_t color) {

  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((y+h-1) >= _height) h = _height-y;
  setAddrWindow(x, y, x, y+h-1);

  uint8_t hi = color >> 8, lo = color;
//  *rsport |=  rspinmask;
//  *csport &= ~cspinmask;
digitalWrite(_rs, HIGH);
  digitalWrite(_cs, LOW);
  while (h--) {
    spiwrite(hi);
    spiwrite(lo);
//SPI.transfer(hi);
//SPI.transfer(lo);
  }
//  *csport |= cspinmask;
  digitalWrite(_cs, HIGH);
}


void Arduino_LCD::drawFastHLine(int16_t x, int16_t y, int16_t w,
  uint16_t color) {

  // Rudimentary clipping
  if((x >= _width) || (y >= _height)) return;
  if((x+w-1) >= _width)  w = _width-x;
  setAddrWindow(x, y, x+w-1, y);

  uint8_t hi = color >> 8, lo = color;
//  *rsport |=  rspinmask;
//  *csport &= ~cspinmask;
digitalWrite(_rs, HIGH);
  digitalWrite(_cs, LOW);
  while (w--) {
    spiwrite(hi);
    spiwrite(lo);
//SPI.transfer(hi);
//SPI.transfer(lo);
  }
//  *csport |= cspinmask;
  digitalWrite(_cs, HIGH);
}


// fill a rectangle
void Arduino_LCD::fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
  uint16_t color) {

  // rudimentary clipping (drawChar w/big text requires this)
  if((x >= _width) || (y >= _height)) return;
  if((x + w - 1) >= _width)  w = _width  - x;
  if((y + h - 1) >= _height) h = _height - y;

  setAddrWindow(x, y, x+w-1, y+h-1);

  uint8_t hi = color >> 8, lo = color;
//  *rsport |=  rspinmask;
//  *csport &= ~cspinmask;
digitalWrite(_rs, HIGH);
digitalWrite(_cs, LOW);
  for(y=h; y>0; y--) {
    for(x=w; x>0; x--) {
      spiwrite(hi);
      spiwrite(lo);
//SPI.transfer(hi);
//SPI.transfer(lo);
    }
  }

//  *csport |= cspinmask;
digitalWrite(_cs, HIGH);
}


// Pass 8-bit (each) R,G,B, get back 16-bit packed color
uint16_t Arduino_LCD::Color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


#define MADCTL_MY  0x80
#define MADCTL_MX  0x40
#define MADCTL_MV  0x20
#define MADCTL_ML  0x10
#define MADCTL_RGB 0x08
#define MADCTL_MH  0x04

void Arduino_LCD::setRotation(uint8_t m) {

  writecommand(ILI9163C_MADCTL);
  rotation = m % 4; // can't be higher than 3
  switch (rotation) {
   case 0:
    writedata(MADCTL_MX | MADCTL_MY | MADCTL_RGB);
    _width  = ILI9163C_TFTWIDTH;
    _height = ILI9163C_TFTHEIGHT;
    break;
   case 1:
    writedata(MADCTL_MY | MADCTL_MV | MADCTL_RGB);
    _width  = ILI9163C_TFTHEIGHT;
    _height = ILI9163C_TFTWIDTH;
    break;
   case 2:
    writedata(MADCTL_RGB);
    _width  = ILI9163C_TFTWIDTH;
    _height = ILI9163C_TFTHEIGHT;
    break;
   case 3:
    writedata(MADCTL_MX | MADCTL_MV | MADCTL_RGB);
    _width  = ILI9163C_TFTHEIGHT;
    _height = ILI9163C_TFTWIDTH;
    break;
  }
}


void Arduino_LCD::invertDisplay(boolean i) {
  writecommand(i ? ILI9163C_INVON : ILI9163C_INVOFF);
}

/* 
  18, // there are 17 commands
    ILI9163C_SWRESET,   DELAY,  //  1: Software reset, no args, w/delay
      50,                     //     50 ms delay

	0x11, 			//Exit Sleep
	DELAY,50,

	0x26, 			//Set Default Gamma
	0x104,

	//0xF2, //E0h & E1h Enable/Disable
	//0x100,

	0xB1,
	0x10C,
	0x114,

	0xC0, //Set VRH1[4:0] & VC[2:0] for VCI1 & GVDD
	0x10C,
	0x105,

	0xC1, //Set BT[2:0] for AVDD & VCL & VGH & VGL
	0x102,

	0xC5, //Set VMH[6:0] & VML[6:0] for VOMH & VCOML
	0x129,
	0x143,

	0xC7,
	0x140,

	0x3a, //Set Color Format
	0x105,

	0x2A, //Set Column Address
	0x100,
	0x100,
	0x100,
	0x17F,

	0x2B, //Set Page Address
	0x100,
	0x100,
	0x100,
	0x19F,

	0x36, //Set Scanning Direction, RGB
	0x1C0,

	0xB7, //Set Source Output Direction
	0x100,

	0xf2, //Enable Gamma bit
	0x101,

	0xE0,
	0x136,//p1
	0x129,//p2
	0x112,//p3
	0x122,//p4
	0x11C,//p5
	0x115,//p6
	0x142,//p7
	0x1B7,//p8
	0x12F,//p9
	0x113,//p10
	0x112,//p11
	0x10A,//p12
	0x111,//p13
	0x10B,//p14
	0x106,//p15

	0xE1,
	0x109,//p1
	0x116,//p2
	0x12D,//p3
	0x10D,//p4
	0x113,//p5
	0x115,//p6
	0x140,//p7
	0x148,//p8
	0x153,//p9
	0x10C,//p10
	0x11D,//p11
	0x125,//p12
	0x12E,//p13
	0x134,//p14
	0x139,//p15

	0x33, // scroll setup
	0x100,
	0x100,
	0x100,
	0x1C1,
	0x100,
	0x100,

	0x29,	// Display On
	0x2C},	// write gram

*/

