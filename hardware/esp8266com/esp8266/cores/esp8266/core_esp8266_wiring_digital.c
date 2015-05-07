/* 
  digital.c - wiring digital implementation for esp8266

  Copyright (c) 2015 Hristo Gochkov. All rights reserved.
  This file is part of the esp8266 core for Arduino environment.
 
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
#define ARDUINO_MAIN
#include "wiring_private.h"
#include "pins_arduino.h"
#include "c_types.h"
#include "eagle_soc.h"
#include "ets_sys.h"

extern void __pinMode(uint8_t pin, uint8_t mode) {
  if(pin < 16){
    if(mode == SPECIAL){
      GPC(pin) = (GPC(pin) & (0xF << GPCI)); //SOURCE(GPIO) | DRIVER(NORMAL) | INT_TYPE(UNCHANGED) | WAKEUP_ENABLE(DISABLED)
      GPEC = (1 << pin); //Disable
      GPF(pin) = GPFFS(GPFFS_BUS(pin));//Set mode to BUS (RX0, TX0, TX1, SPI, HSPI or CLK depending in the pin)
      if(pin == 3) GPF(pin) |= (1 << GPFPU);//enable pullup on RX
    } else if(mode & FUNCTION_0){
      GPC(pin) = (GPC(pin) & (0xF << GPCI)); //SOURCE(GPIO) | DRIVER(NORMAL) | INT_TYPE(UNCHANGED) | WAKEUP_ENABLE(DISABLED)
      GPEC = (1 << pin); //Disable
      GPF(pin) = GPFFS((mode >> 4) & 0x07);
      if(pin == 13 && mode == FUNCTION_4) GPF(pin) |= (1 << GPFPU);//enable pullup on RX
    }  else if(mode == OUTPUT || mode == OUTPUT_OPEN_DRAIN){
      GPF(pin) = GPFFS(GPFFS_GPIO(pin));//Set mode to GPIO
      GPC(pin) = (GPC(pin) & (0xF << GPCI)); //SOURCE(GPIO) | DRIVER(NORMAL) | INT_TYPE(UNCHANGED) | WAKEUP_ENABLE(DISABLED)
      if(mode == OUTPUT_OPEN_DRAIN) GPC(pin) |= (1 << GPCD);
      GPES = (1 << pin); //Enable
    } else if(mode == INPUT || mode == INPUT_PULLUP || mode == INPUT_PULLDOWN){
      GPF(pin) = GPFFS(GPFFS_GPIO(pin));//Set mode to GPIO
      GPEC = (1 << pin); //Disable
      GPC(pin) = (GPC(pin) & (0xF << GPCI)) | (1 << GPCD); //SOURCE(GPIO) | DRIVER(OPEN_DRAIN) | INT_TYPE(UNCHANGED) | WAKEUP_ENABLE(DISABLED)
      if(mode == INPUT_PULLUP) {
          GPF(pin) |= (1 << GPFPU);  // Enable  Pullup
      } else if(mode == INPUT_PULLDOWN) {
          GPF(pin) |= (1 << GPFPD);  // Enable  Pulldown
      }
    }
  } else if(pin == 16){
    GPF16 = GP16FFS(GPFFS_GPIO(pin));//Set mode to GPIO
    GPC16 = 0;
    if(mode == INPUT || mode == INPUT_PULLDOWN){
      if(mode == INPUT_PULLDOWN){
        GPF16 |= (1 << GP16FPD);//Enable Pulldown
      }
      GP16E &= ~1;
    } else if(mode == OUTPUT){
      GP16E |= 1;
    }
  }
}

extern void ICACHE_RAM_ATTR __digitalWrite(uint8_t pin, uint8_t val) {
  val &= 0x01;
  if(pin < 16){
    if(val) GPOS = (1 << pin);
    else GPOC = (1 << pin);
  } else if(pin == 16){
    if(val) GP16O |= 1;
    else GP16O &= ~1;
  }
}

extern int ICACHE_RAM_ATTR __digitalRead(uint8_t pin) {
  if(pin < 16){
    return GPIP(pin);
  } else if(pin == 16){
    return GP16I & 0x01;
  }
  return 0;
}

/*
  GPIO INTERRUPTS
*/

typedef void (*voidFuncPtr)(void);

typedef struct {
  uint8_t mode;
  void (*fn)(void);
} interrupt_handler_t;

static interrupt_handler_t interrupt_handlers[16];
static uint32_t interrupt_reg = 0;

void interrupt_handler(void *arg) {
  uint32_t status = GPIE;
  GPIEC = status;//clear them interrupts
  if(status == 0 || interrupt_reg == 0) return;
  ETS_GPIO_INTR_DISABLE();
  int i = 0;
  uint32_t changedbits = status & interrupt_reg;
  while(changedbits){
    while(!(changedbits & (1 << i))) i++;
    changedbits &= ~(1 << i);
    interrupt_handler_t *handler = &interrupt_handlers[i];
    if(((handler->mode & 1) == digitalRead(i)) && handler->fn) handler->fn();
  }
  ETS_GPIO_INTR_ENABLE();
}

extern void __attachInterrupt(uint8_t pin, voidFuncPtr userFunc, int mode) {
  if(pin < 16) {
    interrupt_handler_t *handler = &interrupt_handlers[pin];
    handler->mode = mode;
    handler->fn = userFunc;
    interrupt_reg |= (1 << pin);
    GPC(pin) &= ~(0xF << GPCI);//INT mode disabled
    GPIEC = (1 << pin); //Clear Interrupt for this pin
    GPC(pin) |= ((mode & 0xF) << GPCI);//INT mode "mode"
  }
}

extern void __detachInterrupt(uint8_t pin) {
  if(pin < 16) {
    GPC(pin) &= ~(0xF << GPCI);//INT mode disabled
    GPIEC = (1 << pin); //Clear Interrupt for this pin
    interrupt_reg &= ~(1 << pin);
    interrupt_handler_t *handler = &interrupt_handlers[pin];
    handler->mode = 0;
    handler->fn = 0;
  }
}

// stored state for the noInterrupts/interrupts methods
uint32_t interruptsState = 0;

void initPins() {
  //Disable UART interrupts
  system_set_os_print(0);
  U0IE = 0;
  U1IE = 0;

  for (int i = 0; i <= 5; ++i) {
    pinMode(i, INPUT);
  }
  // pins 6-11 are used for the SPI flash interface
  for (int i = 12; i <= 16; ++i) {
    pinMode(i, INPUT);
  }
  
  ETS_GPIO_INTR_ATTACH(interrupt_handler, &interrupt_reg);
  ETS_GPIO_INTR_ENABLE();
}

extern void pinMode(uint8_t pin, uint8_t mode) __attribute__ ((weak, alias("__pinMode")));
extern void digitalWrite(uint8_t pin, uint8_t val) __attribute__ ((weak, alias("__digitalWrite")));
extern int digitalRead(uint8_t pin) __attribute__ ((weak, alias("__digitalRead")));
extern void attachInterrupt(uint8_t pin, voidFuncPtr handler, int mode) __attribute__ ((weak, alias("__attachInterrupt")));
extern void detachInterrupt(uint8_t pin) __attribute__ ((weak, alias("__detachInterrupt")));

