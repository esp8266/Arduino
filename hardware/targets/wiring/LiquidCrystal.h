/*
  LiquidCrystal.cpp - Liquid Crystal Display library for Wiring & Arduino
  Copyright (c) 2006 Hernando Barragan and Nicholas Zambetti.
  All right reserved.

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


#ifndef LiquidCrystal_h
#define LiquidCrystal_h


#include <inttypes.h>

class LiquidCrystal
{
  private:
    uint8_t _control_rs;
    uint8_t _control_rw;
    uint8_t _control_e;
    uint8_t _port;
    void display_init(void);
    void display_start(void);
    void display_wait(void);
    void display_control_write(uint8_t);
    uint8_t display_control_read(void);
    void display_data_write(uint8_t);
    uint8_t display_data_read(void);
    void display_write(char *, uint8_t);
    void printNumber(unsigned long, uint8_t);
  public:
    //LiquidCrystal();
    LiquidCrystal(uint8_t, uint8_t, uint8_t, uint8_t);
//    uint8_t read(void);
    void clear(void);
    void home(void);
    void setCursor(uint8_t, uint8_t);
    void print(char);
    void print(char[]);
    void print(uint8_t);
    void print(int);
    void print(long);
    void print(long, int);
    void println(void);
    void println(char);
    void println(char[]);
    void println(uint8_t);
    void println(int);
    void println(long);
    void println(long, int);
};


#endif
