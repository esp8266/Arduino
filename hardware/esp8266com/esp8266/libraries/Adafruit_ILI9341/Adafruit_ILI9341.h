#ifndef _ADAFRUIT_ILI9341H_
#define _ADAFRUIT_ILI9341H_

#include "Adafruit_GFX.h"

extern "C"
{
#include <c_types.h>
#include <osapi.h>
#include <gpio.h>
#include "driver/hspi.h"
}

#define ILI9341_TFTWIDTH  240
#define ILI9341_TFTHEIGHT 320

#define ILI9341_NOP     0x00
#define ILI9341_SWRESET 0x01
#define ILI9341_RDDID   0x04
#define ILI9341_RDDST   0x09

#define ILI9341_SLPIN   0x10
#define ILI9341_SLPOUT  0x11
#define ILI9341_PTLON   0x12
#define ILI9341_NORON   0x13

#define ILI9341_RDMODE  0x0A
#define ILI9341_RDMADCTL  0x0B
#define ILI9341_RDPIXFMT  0x0C
#define ILI9341_RDIMGFMT  0x0A
#define ILI9341_RDSELFDIAG  0x0F

#define ILI9341_INVOFF  0x20
#define ILI9341_INVON   0x21
#define ILI9341_GAMMASET 0x26
#define ILI9341_DISPOFF 0x28
#define ILI9341_DISPON  0x29

#define ILI9341_CASET   0x2A
#define ILI9341_PASET   0x2B
#define ILI9341_RAMWR   0x2C
#define ILI9341_RAMRD   0x2E

#define ILI9341_PTLAR   0x30
#define ILI9341_MADCTL  0x36
#define ILI9341_PIXFMT  0x3A

#define ILI9341_FRMCTR1 0xB1
#define ILI9341_FRMCTR2 0xB2
#define ILI9341_FRMCTR3 0xB3
#define ILI9341_INVCTR  0xB4
#define ILI9341_DFUNCTR 0xB6

#define ILI9341_PWCTR1  0xC0
#define ILI9341_PWCTR2  0xC1
#define ILI9341_PWCTR3  0xC2
#define ILI9341_PWCTR4  0xC3
#define ILI9341_PWCTR5  0xC4
#define ILI9341_VMCTR1  0xC5
#define ILI9341_VMCTR2  0xC7

#define ILI9341_RDID1   0xDA
#define ILI9341_RDID2   0xDB
#define ILI9341_RDID3   0xDC
#define ILI9341_RDID4   0xDD

#define ILI9341_GMCTRP1 0xE0
#define ILI9341_GMCTRN1 0xE1
/*
#define ILI9341_PWCTR6  0xFC

*/

// Color definitions
#define	ILI9341_BLACK   0x0000
#define	ILI9341_BLUE    0x001F
#define	ILI9341_RED     0xF800
#define	ILI9341_GREEN   0x07E0
#define ILI9341_CYAN    0x07FF
#define ILI9341_MAGENTA 0xF81F
#define ILI9341_YELLOW  0xFFE0  
#define ILI9341_WHITE   0xFFFF

#define TFT_DC_DATA		GPIO_OUTPUT_SET(2, 1)
#define TFT_DC_COMMAND	GPIO_OUTPUT_SET(2, 0)
#define TFT_DC_INIT 	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U, FUNC_GPIO2); TFT_DC_DATA

#define MAKEWORD(b1, b2, b3, b4) (uint32_t(b1) | ((b2) << 8) | ((b3) << 16) | ((b4) << 24))
#define SWAPBYTES(i) ((i>>8) | (i<<8))

class Adafruit_ILI9341 : public Adafruit_GFX {

private:
 uint8_t  tabcolor;
 spi_config config;
 void transmitCmdData(uint8_t cmd, const uint8_t *data, uint8_t numDataByte);
 inline void transmitData(uint16_t data) {spi_wait_ready(); spi_send_uint16(data);}
 inline void transmitCmdData(uint8_t cmd, uint32_t data) {spi_wait_ready(); TFT_DC_COMMAND; spi_send_uint8(cmd); spi_wait_ready(); TFT_DC_DATA; spi_send_uint32(data);}
 inline void transmitData(uint16_t data, int32_t repeats){spi_wait_ready(); spi_send_uint16_r(data, repeats);}
 inline void transmitCmd(uint8_t cmd){spi_wait_ready(); TFT_DC_COMMAND; spi_send_uint8(cmd);spi_wait_ready(); TFT_DC_DATA;}
 inline void drawPixelInternal(int16_t x, int16_t y, uint16_t color) {	if((x < 0) ||(x >= _width) || (y < 0) || (y >= _height)) return;
	setAddrWindow(x,y,x+1,y+1);
	transmitData(SWAPBYTES(color));
 }

public:
  Adafruit_ILI9341() : Adafruit_GFX(ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT), tabcolor(0){}

  void     begin(),
           fillScreen(uint16_t color),
           drawPixel(int16_t x, int16_t y, uint16_t color),
           drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color),
           drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color),
           fillRect(int16_t x, int16_t y, int16_t w, int16_t h,
             uint16_t color),
           setRotation(uint8_t r),
           invertDisplay(boolean i);
  inline void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
  {	  transmitCmdData(ILI9341_CASET, MAKEWORD(x0 >> 8, x0 & 0xFF, x1 >> 8, x1 & 0xFF));
  	  transmitCmdData(ILI9341_PASET, MAKEWORD(y0 >> 8, y0 & 0xFF, y1 >> 8, y1 & 0xFF));
	  transmitCmd(ILI9341_RAMWR); // write to RAM
  }
  uint16_t color565(uint8_t r, uint8_t g, uint8_t b);
};

#endif
