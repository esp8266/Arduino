/*
    2012 Copyright (c) Seeed Technology Inc.

    Authors: Albert.Miao & Loovee,
    Visweswara R (with initializtion code from TFT vendor)

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
    Foundation, Inc.,51 Franklin St,Fifth Floor, Boston, MA 02110-1301 USA

*/

#include <TFTv2.h>
#include <SPI.h>


void TFT::TFTinit(void)
{
    pinMode(2, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(15, OUTPUT);
    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV2);

    TFT_CS_HIGH;
    TFT_DC_HIGH;
    INT8U i = 0;
    for (i = 0; i < 3; i++)
    {
        readID();
    }
    delay(500);
    sendCMD(0x01);
    delay(200);

    sendCMD(0xCF);
    WRITE_DATA(0x00);
    WRITE_DATA(0x8B);
    WRITE_DATA(0X30);

    sendCMD(0xED);
    WRITE_DATA(0x67);
    WRITE_DATA(0x03);
    WRITE_DATA(0X12);
    WRITE_DATA(0X81);

    sendCMD(0xE8);
    WRITE_DATA(0x85);
    WRITE_DATA(0x10);
    WRITE_DATA(0x7A);

    sendCMD(0xCB);
    WRITE_DATA(0x39);
    WRITE_DATA(0x2C);
    WRITE_DATA(0x00);
    WRITE_DATA(0x34);
    WRITE_DATA(0x02);

    sendCMD(0xF7);
    WRITE_DATA(0x20);

    sendCMD(0xEA);
    WRITE_DATA(0x00);
    WRITE_DATA(0x00);

    sendCMD(0xC0);                                                      /* Power control                */
    WRITE_DATA(0x1B);                                                   /* VRH[5:0]                     */

    sendCMD(0xC1);                                                      /* Power control                */
    WRITE_DATA(0x10);                                                   /* SAP[2:0];BT[3:0]             */

    sendCMD(0xC5);                                                      /* VCM control                  */
    WRITE_DATA(0x3F);
    WRITE_DATA(0x3C);

    sendCMD(0xC7);                                                      /* VCM control2                 */
    WRITE_DATA(0XB7);

    sendCMD(0x36);                                                      /* Memory Access Control        */
    WRITE_DATA(0x08);

    sendCMD(0x3A);
    WRITE_DATA(0x55);

    sendCMD(0xB1);
    WRITE_DATA(0x00);
    WRITE_DATA(0x1B);

    sendCMD(0xB6);                                                      /* Display Function Control     */
    WRITE_DATA(0x0A);
    WRITE_DATA(0xA2);


    sendCMD(0xF2);                                                      /* 3Gamma Function Disable      */
    WRITE_DATA(0x00);

    sendCMD(0x26);                                                      /* Gamma curve selected         */
    WRITE_DATA(0x01);

    sendCMD(0xE0);                                                      /* Set Gamma                    */
    WRITE_DATA(0x0F);
    WRITE_DATA(0x2A);
    WRITE_DATA(0x28);
    WRITE_DATA(0x08);
    WRITE_DATA(0x0E);
    WRITE_DATA(0x08);
    WRITE_DATA(0x54);
    WRITE_DATA(0XA9);
    WRITE_DATA(0x43);
    WRITE_DATA(0x0A);
    WRITE_DATA(0x0F);
    WRITE_DATA(0x00);
    WRITE_DATA(0x00);
    WRITE_DATA(0x00);
    WRITE_DATA(0x00);

    sendCMD(0XE1);                                                      /* Set Gamma                    */
    WRITE_DATA(0x00);
    WRITE_DATA(0x15);
    WRITE_DATA(0x17);
    WRITE_DATA(0x07);
    WRITE_DATA(0x11);
    WRITE_DATA(0x06);
    WRITE_DATA(0x2B);
    WRITE_DATA(0x56);
    WRITE_DATA(0x3C);
    WRITE_DATA(0x05);
    WRITE_DATA(0x10);
    WRITE_DATA(0x0F);
    WRITE_DATA(0x3F);
    WRITE_DATA(0x3F);
    WRITE_DATA(0x0F);

    sendCMD(0x11);                                                      /* Exit Sleep                   */
    delay(120);
    sendCMD(0x29);                                                      /* Display on                   */
    fillScreen();
}

INT8U TFT::readID(void)
{
    INT8U i = 0;
    INT8U data[3] ;
    INT8U ID[3] = {0x00, 0x93, 0x41};
    INT8U ToF = 1;
    for (i = 0; i < 3; i++)
    {
        data[i] = Read_Register(0xd3, i + 1);
        if (data[i] != ID[i])
        {
            ToF = 0;
        }
    }
    if (!ToF)                                                           /* data!=ID                     */
    {
        Serial.print("Read TFT ID failed, ID should be 0x09341, but read ID = 0x");
        for (i = 0; i < 3; i++)
        {
            Serial.print(data[i], HEX);
        }
        Serial.println();
    }
    return ToF;
}

void TFT::setCol(INT16U StartCol, INT16U EndCol)
{
    sendCMD(0x2A);                                                      /* Column Command address       */
    sendData(StartCol);
    sendData(EndCol);
}

void TFT::setPage(INT16U StartPage, INT16U EndPage)
{
    sendCMD(0x2B);                                                      /* Column Command address       */
    sendData(StartPage);
    sendData(EndPage);
}

void TFT::fillScreen(INT16U XL, INT16U XR, INT16U YU, INT16U YD, INT16U color)
{
    unsigned long  XY = 0;
    unsigned long i = 0;

    if (XL > XR)
    {
        XL = XL ^ XR;
        XR = XL ^ XR;
        XL = XL ^ XR;
    }
    if (YU > YD)
    {
        YU = YU ^ YD;
        YD = YU ^ YD;
        YU = YU ^ YD;
    }
    XL = constrain((int)XL, (int)MIN_X, (int)MAX_X);
    XR = constrain((int)XR, (int)MIN_X, (int)MAX_X);
    YU = constrain((int)YU, (int)MIN_Y, (int)MAX_Y);
    YD = constrain((int)YD, (int)MIN_Y, (int)MAX_Y);

    XY = (XR - XL + 1);
    XY = XY * (YD - YU + 1);

    Tft.setCol(XL, XR);
    Tft.setPage(YU, YD);
    Tft.sendCMD(0x2c);

    TFT_DC_HIGH;
    TFT_CS_LOW;

    INT8U Hcolor = color >> 8;
    INT8U Lcolor = color & 0xff;
    for (i = 0; i < XY; i++)
    {
        SPI.transfer(Hcolor);
        SPI.transfer(Lcolor);
    }

    TFT_CS_HIGH;
}

void TFT::fillScreen(void)
{
    Tft.setCol(0, 239);
    Tft.setPage(0, 319);
    Tft.sendCMD(0x2c);                                                  /* start to write to display ram */

    TFT_DC_HIGH;
    TFT_CS_LOW;
    for (INT16U i = 0; i < 38400; i++)
    {
        SPI.transfer(0);
        SPI.transfer(0);
        SPI.transfer(0);
        SPI.transfer(0);
    }
    TFT_CS_HIGH;
}


void TFT::setXY(INT16U poX, INT16U poY)
{
    setCol(poX, poX);
    setPage(poY, poY);
    sendCMD(0x2c);
}

void TFT::setPixel(INT16U poX, INT16U poY, INT16U color)
{

    sendCMD(0x2A);                                                      /* Column Command address       */
    sendData(poX);
    sendData(poX);

    sendCMD(0x2B);                                                      /* Column Command address       */
    sendData(poY);
    sendData(poY);

    sendCMD(0x2c);

    sendData(color);
}

void TFT::drawChar(INT8U ascii, INT16U poX, INT16U poY, INT16U size, INT16U fgcolor)
{
    if ((ascii >= 32) && (ascii <= 127))
    {
        ;
    }
    else
    {
        ascii = '?' - 32;
    }
    for (int i = 0; i < FONT_X; i++)
    {
        INT8U temp = pgm_read_byte(&simpleFont[ascii - 0x20][i]);
        for (INT8U f = 0; f < 8; f++)
        {
            if ((temp >> f) & 0x01)
            {
                fillRectangle(poX + i * size, poY + f * size, size, size, fgcolor);
            }
        }

    }
}

void TFT::drawString(const char *string, INT16U poX, INT16U poY, INT16U size, INT16U fgcolor)
{
    while (*string)
    {
        drawChar(*string, poX, poY, size, fgcolor);
        string++;

        if (poX < MAX_X)
        {
            poX += FONT_SPACE * size;                                   /* Move cursor right            */
        }
    }
}

//fillRectangle(poX+i*size, poY+f*size, size, size, fgcolor);
void TFT::fillRectangle(INT16U poX, INT16U poY, INT16U length, INT16U width, INT16U color)
{
    fillScreen(poX, poX + length, poY, poY + width, color);
}

void  TFT::drawHorizontalLine(INT16U poX, INT16U poY,
                              INT16U length, INT16U color)
{
    setCol(poX, poX + length);
    setPage(poY, poY);
    sendCMD(0x2c);
    for (INT16U i = 0; i < length; i++)
    {
        sendData(color);
    }
}

void TFT::drawLine(INT16U x0, INT16U y0, INT16U x1, INT16U y1, INT16U color)
{

    int x = x1 - x0;
    int y = y1 - y0;
    int dx = abs(x), sx = x0 < x1 ? 1 : -1;
    int dy = -abs(y), sy = y0 < y1 ? 1 : -1;
    int err = dx + dy, e2;                                              /* error value e_xy             */
    for (;;)
    {
        /* loop                         */
        setPixel(x0, y0, color);
        e2 = 2 * err;
        if (e2 >= dy)                                                   /* e_xy+e_x > 0                 */
        {
            if (x0 == x1)
            {
                break;
            }
            err += dy; x0 += sx;
        }
        if (e2 <= dx)                                                   /* e_xy+e_y < 0                 */
        {
            if (y0 == y1)
            {
                break;
            }
            err += dx; y0 += sy;
        }
    }
}

void TFT::drawVerticalLine(INT16U poX, INT16U poY, INT16U length, INT16U color)
{
    setCol(poX, poX);
    setPage(poY, poY + length);
    sendCMD(0x2c);
    for (INT16U i = 0; i < length; i++)
    {
        sendData(color);
    }
}

void TFT::drawRectangle(INT16U poX, INT16U poY, INT16U length, INT16U width, INT16U color)
{
    drawHorizontalLine(poX, poY, length, color);
    drawHorizontalLine(poX, poY + width, length, color);
    drawVerticalLine(poX, poY, width, color);
    drawVerticalLine(poX + length, poY, width, color);
}

void TFT::drawCircle(int poX, int poY, int r, INT16U color)
{
    int x = -r, y = 0, err = 2 - 2 * r, e2;
    do
    {
        setPixel(poX - x, poY + y, color);
        setPixel(poX + x, poY + y, color);
        setPixel(poX + x, poY - y, color);
        setPixel(poX - x, poY - y, color);
        e2 = err;
        if (e2 <= y)
        {
            err += ++y * 2 + 1;
            if (-x == y && e2 <= x)
            {
                e2 = 0;
            }
        }
        if (e2 > x)
        {
            err += ++x * 2 + 1;
        }
    } while (x <= 0);
}

void TFT::fillCircle(int poX, int poY, int r, INT16U color)
{
    int x = -r, y = 0, err = 2 - 2 * r, e2;
    do
    {

        drawVerticalLine(poX - x, poY - y, 2 * y, color);
        drawVerticalLine(poX + x, poY - y, 2 * y, color);

        e2 = err;
        if (e2 <= y)
        {
            err += ++y * 2 + 1;
            if (-x == y && e2 <= x)
            {
                e2 = 0;
            }
        }
        if (e2 > x)
        {
            err += ++x * 2 + 1;
        }
    } while (x <= 0);

}

void TFT::drawTraingle(int poX1, int poY1, int poX2, int poY2, int poX3, int poY3, INT16U color)
{
    drawLine(poX1, poY1, poX2, poY2, color);
    drawLine(poX1, poY1, poX3, poY3, color);
    drawLine(poX2, poY2, poX3, poY3, color);
}

INT8U TFT::drawNumber(long long_num, INT16U poX, INT16U poY, INT16U size, INT16U fgcolor)
{
    INT8U char_buffer[10] = "";
    INT8U i = 0;
    INT8U f = 0;

    if (long_num < 0)
    {
        f = 1;
        drawChar('-', poX, poY, size, fgcolor);
        long_num = -long_num;
        if (poX < MAX_X)
        {
            poX += FONT_SPACE * size;                                   /* Move cursor right            */
        }
    }
    else if (long_num == 0)
    {
        f = 1;
        drawChar('0', poX, poY, size, fgcolor);
        return f;
        if (poX < MAX_X)
        {
            poX += FONT_SPACE * size;                                   /* Move cursor right            */
        }
    }


    while (long_num > 0)
    {
        char_buffer[i++] = long_num % 10;
        long_num /= 10;
    }

    f = f + i;
    for (; i > 0; i--)
    {
        drawChar('0' + char_buffer[i - 1], poX, poY, size, fgcolor);
        if (poX < MAX_X)
        {
            poX += FONT_SPACE * size;                                   /* Move cursor right            */
        }
    }
    return f;
}

INT8U TFT::drawFloat(float floatNumber, INT8U decimal, INT16U poX, INT16U poY, INT16U size, INT16U fgcolor)
{
    INT16U temp = 0;
    float decy = 0.0;
    float rounding = 0.5;
    INT8U f = 0;
    if (floatNumber < 0.0)
    {
        drawChar('-', poX, poY, size, fgcolor);
        floatNumber = -floatNumber;
        if (poX < MAX_X)
        {
            poX += FONT_SPACE * size;                                   /* Move cursor right            */
        }
        f = 1;
    }
    for (INT8U i = 0; i < decimal; ++i)
    {
        rounding /= 10.0;
    }
    floatNumber += rounding;

    temp = (INT16U)floatNumber;
    INT8U howlong = drawNumber(temp, poX, poY, size, fgcolor);
    f += howlong;
    if ((poX + 8 * size * howlong) < MAX_X)
    {
        poX += FONT_SPACE * size * howlong;                             /* Move cursor right            */
    }

    if (decimal > 0)
    {
        drawChar('.', poX, poY, size, fgcolor);
        if (poX < MAX_X)
        {
            poX += FONT_SPACE * size;                                   /* Move cursor right            */
        }
        f += 1;
    }
    decy = floatNumber - temp;                                          /* decimal part,  4             */
    for (INT8U i = 0; i < decimal; i++)
    {
        decy *= 10;                                                     /* for the next decimal         */
        temp = decy;                                                    /* get the decimal              */
        drawNumber(temp, poX, poY, size, fgcolor);
        floatNumber = -floatNumber;
        if (poX < MAX_X)
        {
            poX += FONT_SPACE * size;                                   /* Move cursor right            */
        }
        decy -= temp;
    }
    f += decimal;
    return f;
}

INT8U TFT::drawFloat(float floatNumber, INT16U poX, INT16U poY, INT16U size, INT16U fgcolor)
{
    INT8U decimal = 2;
    INT16U temp = 0;
    float decy = 0.0;
    float rounding = 0.5;
    INT8U f = 0;
    if (floatNumber < 0.0)                                              /* floatNumber < 0              */
    {
        drawChar('-', poX, poY, size, fgcolor);                         /* add a '-'                    */
        floatNumber = -floatNumber;
        if (poX < MAX_X)
        {
            poX += FONT_SPACE * size;                                   /* Move cursor right            */
        }
        f = 1;
    }
    for (INT8U i = 0; i < decimal; ++i)
    {
        rounding /= 10.0;
    }
    floatNumber += rounding;

    temp = (INT16U)floatNumber;
    INT8U howlong = drawNumber(temp, poX, poY, size, fgcolor);
    f += howlong;
    if ((poX + 8 * size * howlong) < MAX_X)
    {
        poX += FONT_SPACE * size * howlong;                             /* Move cursor right            */
    }


    if (decimal > 0)
    {
        drawChar('.', poX, poY, size, fgcolor);
        if (poX < MAX_X)
        {
            poX += FONT_SPACE * size;                                   /* Move cursor right            */
        }
        f += 1;
    }
    decy = floatNumber - temp;                                          /* decimal part,                */
    for (INT8U i = 0; i < decimal; i++)
    {
        decy *= 10;                                                     /* for the next decimal         */
        temp = decy;                                                    /* get the decimal              */
        drawNumber(temp, poX, poY, size, fgcolor);
        floatNumber = -floatNumber;
        if (poX < MAX_X)
        {
            poX += FONT_SPACE * size;                                   /* Move cursor right            */
        }
        decy -= temp;
    }
    f += decimal;
    return f;
}

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_TFT)
TFT Tft;
#endif

/*********************************************************************************************************
    END FILE
*********************************************************************************************************/
