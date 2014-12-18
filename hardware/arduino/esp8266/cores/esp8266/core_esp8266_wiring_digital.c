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


enum PinFunction { GPIO, PWM };
static uint32_t g_gpio_function[PINCOUNT] = {
    GPIO
};


void pinMode(uint8_t pin, uint8_t mode)
{
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

void digitalWrite(uint8_t pin, uint8_t val)
{
    uint32_t mask = 1 << pin;
    if (val)
      GPIO_REG_WRITE(GPIO_OUT_W1TS_ADDRESS, mask);
    else
      GPIO_REG_WRITE(GPIO_OUT_W1TC_ADDRESS, mask);
}

int digitalRead(uint8_t pin)
{
    return ((gpio_input_get() >> pin) & 1);
}

void analogWrite(uint8_t pin, int val)
{
}

typedef void (*inthandler_t)(void);
static inthandler_t g_handlers[PINCOUNT] = { 0 };


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

void attachInterrupt(uint8_t pin, inthandler_t handler, int mode) 
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
        gpio_pin_intr_state_set(pin, GPIO_PIN_INTR_ANYEGDE);
    }
    else
    {
        gpio_pin_intr_state_set(pin, GPIO_PIN_INTR_DISABLE);
    }
}

void detachInterrupt(uint8_t pin) 
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


