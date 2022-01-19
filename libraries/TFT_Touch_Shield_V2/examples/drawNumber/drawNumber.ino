/*  draw number's APP
    drawNumber(long long_num,INT16U poX, INT16U poY,INT16U size,INT16U fgcolor);
    drawFloat(float floatNumber,INT8U decimal,INT16U poX, INT16U poY,INT16U size,INT16U fgcolor);
    drawFloat(float floatNumber,INT16U poX, INT16U poY,INT16U size,INT16U fgcolor);
*/

#include <stdint.h>
#include <TFTv2.h>
#include <SPI.h>

void setup() {
  TFT_BL_ON; // turn on the background light

  Tft.TFTinit(); // init TFT library

  Tft.drawNumber(1024, 0, 0, 1, RED); // draw a integer: 1024, Location: (0, 0),  size: 1, color: RED

  Tft.drawNumber(1024, 0, 20, 2, BLUE); // draw a integer: 1024, Location: (0, 20), size: 2, color: BLUE

  Tft.drawNumber(1024, 0, 50, 3, GREEN); // draw a integer: 1024, Location: (0, 50), size: 3, color: GREEN

  Tft.drawNumber(1024, 0, 90, 4, BLUE); // draw a integer: 1024, Location: (0, 90), size:4, color: BLUE

  Tft.drawFloat(1.2345, 0, 150, 4, YELLOW); // draw a float number: 1.2345, Location: (0, 150), size: 4, color: YELLOW

  Tft.drawFloat(1.2345, 2, 0, 200, 4, BLUE); // draw a float number: 1.2345: Location: (0, 200), size: 4, decimal: 2, color: BLUE

  Tft.drawFloat(1.2345, 4, 0, 250, 4, RED); // draw a float number: 1.2345 Location: (0, 250), size: 4, decimal: 4, color: RED
}

void loop() {
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
