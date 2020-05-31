/*
  TFT Touch Shield 2.0 examples - tftbmp

  loovee
  2013-1-21

  this demo can show specified bmp file in root Directory of SD card
  please ensure that your image file is 320x240 size.

  change __Gnfile_num and __Gsbmp_files to display your image
*/

#include <SD.h>
#include <SPI.h>
#include <Streaming.h>

#include "TFTv2.h"

#define MAX_BMP         10                      // bmp file num
#define FILENAME_LEN    20                      // max file name length


const int PIN_SD_CS = 4;                        // pin of sd card

const int __Gnbmp_height = 320;                 // bmp hight
const int __Gnbmp_width  = 240;                 // bmp width

unsigned char __Gnbmp_image_offset  = 0;        // offset


int __Gnfile_num = 3;                           // num of file

char __Gsbmp_files[3][FILENAME_LEN] = {         // add file name here
  "flower.BMP",
  "hibiscus.bmp",
  "test.bmp",
};


File bmpFile;


void setup() {

  Serial.begin(115200);

  pinMode(PIN_SD_CS, OUTPUT);
  digitalWrite(PIN_SD_CS, HIGH);

  Tft.TFTinit();

  Sd2Card card;
  card.init(SPI_FULL_SPEED, PIN_SD_CS);

  if (!SD.begin(PIN_SD_CS)) {
    Serial.println("failed!");
    while (1);                              // init fail, die here
  }

  Serial.println("SD OK!");

  TFT_BL_ON;
}

void loop() {
  for (unsigned char i = 0; i < __Gnfile_num; i++) {
    bmpFile = SD.open(__Gsbmp_files[i]);
    if (! bmpFile) {
      Serial.println("didnt find image");
      while (1);
    }

    if (! bmpReadHeader(bmpFile)) {
      Serial.println("bad bmp");
      return;
    }

    bmpdraw(bmpFile, 0, 0);
    bmpFile.close();

    delay(1000);
  }

}

/*********************************************/
// This procedure reads a bitmap and draws it to the screen
// its sped up by reading many pixels worth of data at a time
// instead of just one pixel at a time. increading the buffer takes
// more RAM but makes the drawing a little faster. 20 pixels' worth
// is probably a good place

#define BUFFPIXEL       60                      // must be a divisor of 240 
#define BUFFPIXEL_X3    180                     // BUFFPIXELx3

void bmpdraw(File f, int x, int y) {
  bmpFile.seek(__Gnbmp_image_offset);

  uint32_t time = millis();

  uint8_t sdbuffer[BUFFPIXEL_X3];                 // 3 * pixels to buffer

  for (int i = 0; i < __Gnbmp_height; i++) {

    for (int j = 0; j < (240 / BUFFPIXEL); j++) {
      bmpFile.read(sdbuffer, BUFFPIXEL_X3);
      uint8_t buffidx = 0;
      int offset_x = j * BUFFPIXEL;

      unsigned int __color[BUFFPIXEL];

      for (int k = 0; k < BUFFPIXEL; k++) {
        __color[k] = sdbuffer[buffidx + 2] >> 3;                    // read
        __color[k] = __color[k] << 6 | (sdbuffer[buffidx + 1] >> 2); // green
        __color[k] = __color[k] << 5 | (sdbuffer[buffidx + 0] >> 3); // blue

        buffidx += 3;
      }

      Tft.setCol(offset_x, offset_x + BUFFPIXEL);
      Tft.setPage(i, i);
      Tft.sendCMD(0x2c);

      TFT_DC_HIGH;
      TFT_CS_LOW;

      for (int m = 0; m < BUFFPIXEL; m++) {
        SPI.transfer(__color[m] >> 8);
        SPI.transfer(__color[m]);
      }

      TFT_CS_HIGH;
    }

  }

  Serial.print(millis() - time, DEC);
  Serial.println(" ms");
}

boolean bmpReadHeader(File f) {
  // read header
  uint32_t tmp;
  uint8_t bmpDepth;

  if (read16(f) != 0x4D42) {
    // magic bytes missing
    return false;
  }

  // read file size
  tmp = read32(f);
  Serial.print("size 0x");
  Serial.println(tmp, HEX);

  // read and ignore creator bytes
  read32(f);

  __Gnbmp_image_offset = read32(f);
  Serial.print("offset ");
  Serial.println(__Gnbmp_image_offset, DEC);

  // read DIB header
  tmp = read32(f);
  Serial.print("header size ");
  Serial.println(tmp, DEC);


  int bmp_width = read32(f);
  int bmp_height = read32(f);

  if (bmp_width != __Gnbmp_width || bmp_height != __Gnbmp_height) {   // if image is not 320x240, return false
    return false;
  }

  if (read16(f) != 1) {
    return false;
  }

  bmpDepth = read16(f);
  Serial.print("bitdepth ");
  Serial.println(bmpDepth, DEC);

  if (read32(f) != 0) {
    // compression not supported!
    return false;
  }

  Serial.print("compression ");
  Serial.println(tmp, DEC);

  return true;
}

/*********************************************/
// These read data from the SD card file and convert them to big endian
// (the data is stored in little endian format!)

// LITTLE ENDIAN!
uint16_t read16(File f) {
  uint16_t d;
  uint8_t b;
  b = f.read();
  d = f.read();
  d <<= 8;
  d |= b;
  return d;
}

// LITTLE ENDIAN!
uint32_t read32(File f) {
  uint32_t d;
  uint16_t b;

  b = read16(f);
  d = read16(f);
  d <<= 16;
  d |= b;
  return d;
}
