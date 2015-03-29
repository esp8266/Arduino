/* 
  core_esp8266_wiring_digital.c - implementation of Wiring API for esp8266

  Copyright (c) 2014 Ivan Grokhotkov. All rights reserved.
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
#include "gpio.h"
#include "ets_sys.h"

#define MODIFY_PERI_REG(reg, mask, val) WRITE_PERI_REG(reg, (READ_PERI_REG(reg) & (~mask)) | (uint32_t) val)

#define PINCOUNT 16

static const uint32_t g_pin_muxes[PINCOUNT] = {
    [0] = PERIPHS_IO_MUX_GPIO0_U,
    [1] = PERIPHS_IO_MUX_U0TXD_U,
    [2] = PERIPHS_IO_MUX_GPIO2_U,
    [3] = PERIPHS_IO_MUX_U0RXD_U,
    [4] = PERIPHS_IO_MUX_GPIO4_U,
    [5] = PERIPHS_IO_MUX_GPIO5_U,

    // These 6 pins are used for SPI flash interface
    [6] = 0,
    [7] = 0,
    [8] = 0,
    [9] = 0,
    [10] = 0,
    [11] = 0,

    [12] = PERIPHS_IO_MUX_MTDI_U,
    [13] = PERIPHS_IO_MUX_MTCK_U,
    [14] = PERIPHS_IO_MUX_MTMS_U,
    [15] = PERIPHS_IO_MUX_MTDO_U,
};

static const uint32_t g_pin_funcs[PINCOUNT] = {
    [0] = FUNC_GPIO0,
    [1] = FUNC_GPIO1,
    [2] = FUNC_GPIO2,
    [3] = FUNC_GPIO3,
    [4] = FUNC_GPIO4,
    [5] = FUNC_GPIO5,
    [12] = FUNC_GPIO12,
    [13] = FUNC_GPIO13,
    [14] = FUNC_GPIO14,
    [15] = FUNC_GPIO15,
};


uint32_t digitalPinToPort(uint32_t pin)
{
    return 0;
}

uint32_t digitalPinToBitMask(uint32_t pin)
{
    return 1 << pin;
}

volatile uint32_t* portOutputRegister(uint32_t port)
{
    return (volatile uint32_t*) (PERIPHS_GPIO_BASEADDR + GPIO_OUT_ADDRESS);
}

volatile uint32_t* portInputRegister(uint32_t port)
{
    return (volatile uint32_t*) (PERIPHS_GPIO_BASEADDR + GPIO_IN_ADDRESS);
}

volatile uint32_t* portModeRegister(uint32_t port)
{
    return (volatile uint32_t*) (PERIPHS_GPIO_BASEADDR + GPIO_ENABLE_ADDRESS);
}


enum PinFunction { GPIO, PWM };
static uint32_t g_gpio_function[PINCOUNT] = {
    GPIO
};

extern void __pinMode(uint8_t pin, uint8_t mode)
{
    if (pin == 16)
    {
        uint32_t val = (mode == OUTPUT) ? 1 : 0;
        
        MODIFY_PERI_REG(PAD_XPD_DCDC_CONF, 0x43, 1);
        MODIFY_PERI_REG(RTC_GPIO_CONF, 1, 0);
        MODIFY_PERI_REG(RTC_GPIO_ENABLE, 1, val);
        return;
    }

    uint32_t mux = g_pin_muxes[pin];
    if (mode == INPUT)
    {
        gpio_output_set(0, 0, 0, 1 << pin);
        PIN_PULLUP_DIS(mux);
    }
    else if (mode == INPUT_PULLUP)
    {
        gpio_output_set(0, 0, 0, 1 << pin);
        PIN_PULLUP_EN(mux);
    }
    else if (mode == OUTPUT)
    {
        gpio_output_set(0, 0, 1 << pin, 0);
    }
    else if (mode == OUTPUT_OPEN_DRAIN)
    {
        GPIO_REG_WRITE(
            GPIO_PIN_ADDR(GPIO_ID_PIN(pin)), 
            GPIO_REG_READ(GPIO_PIN_ADDR(GPIO_ID_PIN(pin))) | 
            GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_ENABLE)
        );

        GPIO_REG_WRITE(GPIO_ENABLE_ADDRESS, GPIO_REG_READ(GPIO_ENABLE_ADDRESS) | (1 << pin));
    }
}

extern void __digitalWrite(uint8_t pin, uint8_t val)
{
    if (pin == 16) 
    {
        MODIFY_PERI_REG(RTC_GPIO_OUT, 1, (val & 1));
        return;
    }

    uint32_t mask = 1 << pin;
    if (val)
      GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, mask);
    else
      GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, mask);
}

extern int __digitalRead(uint8_t pin)
{
    if (pin == 16)
        return (READ_PERI_REG(RTC_GPIO_IN_DATA) & 1);
    else
        return ((gpio_input_get() >> pin) & 1);
}

extern void __analogWrite(uint8_t pin, int val)
{
}

typedef void (*voidFuncPtr)(void);
static voidFuncPtr g_handlers[PINCOUNT] = { 0 };


void interrupt_handler(void *arg)
{
    uint32_t intr_mask = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
    for (int pin = 0; intr_mask; intr_mask >>= 1, ++pin)
    {
        if ((intr_mask & 1) && g_handlers[pin])
        {
            GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, 1 << pin);
            (*g_handlers[pin])();
        }
    }
}

extern void __attachInterrupt(uint8_t pin, voidFuncPtr handler, int mode) 
{
    if (pin < 0 || pin > PINCOUNT)
        return;

    g_handlers[pin] = handler;
    
    if (mode == RISING)
    {
        gpio_pin_intr_state_set(pin, GPIO_PIN_INTR_POSEDGE);
    }
    else if (mode == FALLING)
    {
        gpio_pin_intr_state_set(pin, GPIO_PIN_INTR_NEGEDGE);
    }
    else if (mode == CHANGE)
    {
        gpio_pin_intr_state_set(pin, GPIO_PIN_INTR_ANYEDGE);
    }
    else
    {
        gpio_pin_intr_state_set(pin, GPIO_PIN_INTR_DISABLE);
    }
}

extern void __detachInterrupt(uint8_t pin) 
{
    g_handlers[pin] = 0;
    gpio_pin_intr_state_set(pin, GPIO_PIN_INTR_DISABLE);
}

void initPins()
{
    gpio_init();
    for (int i = 0; i < PINCOUNT; ++i)
    {
        uint32_t mux = g_pin_muxes[i];
        if (mux)
        {
            uint32_t func = g_pin_funcs[i];
            PIN_FUNC_SELECT(mux, func);
        }
    }
    ETS_GPIO_INTR_ATTACH(&interrupt_handler, NULL);
}

extern void pinMode(uint8_t pin, uint8_t mode) __attribute__ ((weak, alias("__pinMode")));
extern void digitalWrite(uint8_t pin, uint8_t val) __attribute__ ((weak, alias("__digitalWrite")));
extern int  digitalRead(uint8_t pin) __attribute__ ((weak, alias("__digitalRead")));
extern void analogWrite(uint8_t pin, int val) __attribute__ ((weak, alias("__analogWrite")));
extern void attachInterrupt(uint8_t pin, voidFuncPtr handler, int mode) __attribute__ ((weak, alias("__attachInterrupt")));
extern void detachInterrupt(uint8_t pin) __attribute__ ((weak, alias("__detachInterrupt")));

