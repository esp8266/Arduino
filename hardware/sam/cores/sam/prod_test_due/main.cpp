/* ----------------------------------------------------------------------------
 *         SAM Software Package License 
 * ----------------------------------------------------------------------------
 * Copyright (c) 2011, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#include "variant.h"

/* Ensure we are compiling this for Arduino Due */
#if !defined( arduino_due )
#error "Error: wrong variant chosen"
#endif

typedef enum _ePins
{
  /* LEDs */
  PIN_13=0,
  PIN_RXL=1,
  PIN_TXL=2,
  
  /* UART */
  PIN_URXD=3,
  PIN_UTXD=4,
  PIN_UART=5,
  
  /* USART0 */
  PIN_RXD0=6,
  PIN_TXD0=7,
  PIN_USART0=8,
  
  /* USART1 */
  PIN_RXD1=9,
  PIN_TXD1=10,
  PIN_USART1=11,
  
  /* USART2 */
  PIN_RXD2=12,
  PIN_TXD2=13,
  PIN_USART2=14,
  
  /* SPI */
  PIN_MISO=15,
  PIN_MOSI=16,
  PIN_SPCK=17,
  PIN_NPCS0=18,
  
  /* TWI0 */
  PIN_SDA1=19,
  PIN_SCL1=20,
  PIN_TWI0=21,
  
  /* TWI1 */
  PIN_SDA0=22,
  PIN_SCL0=23,
  PIN_TWI1=24,
  
  /* Analog */
  PIN_AD0=25,
  PIN_AD1=26,
  PIN_AD2=27,
  PIN_AD3=28,
  PIN_AD4=29,
  PIN_AD5=30,
  PIN_AD6=31,
  PIN_AD7=32,
  PIN_AD8=33,
  PIN_AD9=34,
  PIN_AD10=35,
  PIN_AD11=36,
  PIN_AD12=37,
  PIN_AD13=38,

  /* External DAC */
  PIN_DAC_CS=39,
  PIN_DAC_SCK=40,
  PIN_DAC_DIN=41,
  
  /* PWM */
  PIN_PWM=42,
  PIN_PWMH0=43,
  PIN_PWMH1=44,
  PIN_PWMH2=45,
  PIN_PWMH3=46,
  PIN_PWML0=47,
  PIN_PWML1=48,
  PIN_PWML2=49,
  PIN_PWML3=50,
  
  PIN_10=51,
  
  PIN_22=52,
  PIN_23=53,
  PIN_24=54,
  PIN_25=55,
  PIN_26=56,
  PIN_27=57,
  PIN_28=58,
  PIN_29=59,
  PIN_30=60,
  PIN_31=61,
  PIN_32=62,
  PIN_33=63,
  PIN_34=64,
  PIN_35=65,
  PIN_36=66,
  PIN_37=67,
  PIN_38=68,
  PIN_39=69,
  PIN_40=70,
  PIN_41=71,
  PIN_42=72,
  PIN_43=73,
  PIN_44=74,
  PIN_45=75,
  PIN_46=76,
  PIN_47=77,
  PIN_48=78,
  PIN_49=79,
  PIN_50=80,
  PIN_51=81,
  PIN_52=82,
  PIN_53=83  
} ePins ;

#define PIN_FIRST      PIN_13
#define PIN_LAST       PIN_53

void setup( void )
{
  // Initialize all digital pins in output mode.
  for ( dwIndex=PIN_FIRST ; dwIndex <= PIN_LAST ; dwIndex++ )
  {
    if ( ((g_APinDescription[dwIndex].ulPinAttribute & PIN_ATTR_COMBO) == 0) && ((g_APinDescription[dwIndex].ulPinAttribute & PIN_ATTR_ANALOG) == 0) )
    {
      pinMode( dwIndex, OUTPUT ) ;
    }
  }
}

void loop( void )
{
  uint32_t dwIndex ;
  uint32_t adwAnalogValues[PIN_AD13-PIN_AD0+1] ;
  uint32_t dwChaserIndex=PIN_FIRST ;
  uint32_t dwChaserIndexLast=PIN_FIRST ;

  // production test loop
  for ( ;; )
  {
    // Read analog values
    for ( dwIndex=PIN_AD0 ; dwIndex <= PIN_AD13 ; dwIndex++ )
    {
//      adwAnalogValues[dwIndex]= ;
    }

    // Transmit analog values on USB CDC
    for ( dwIndex=PIN_AD0 ; dwIndex <= PIN_AD13 ; dwIndex++ )
    {
//      CDC_Print( adwAnalogValues[dwIndex] ) ;
    }

    // Do chaser
    // set the current pin output to VCC
    digitalWrite( dwChaserIndex, HIGH ) ;  
    
    // set the previous pin output to GND
    if ( dwChaserIndexLast != dwChaserIndex )
    {
      digitalWrite( dwChaserIndex, LOW ) ;   
    }

    // backup current chaser pin index
    dwChaserIndexLast=dwChaserIndex ;
    dwChaserIndex++ ;

    // find next chaser pin
    for ( ; ; )
    {
      // Go circular if we reached the end of pins
      if ( dwChaserIndex > PIN_LAST )
      {
        dwChaserIndex=PIN_FIRST ;
      }

      // bypass combo and analog pins
      if ( ((g_APinDescription[dwChaserIndex].ulPinAttribute & PIN_ATTR_COMBO) != 0) || ((g_APinDescription[dwChaserIndex].ulPinAttribute & PIN_ATTR_ANALOG) != 0) )
      {
        dwChaserIndex++ ;
      }
      else
      {
        break ;
      }
    }

    // Go circular if we reached the end of pins
    if ( dwChaserIndex > PIN_LAST )
    {
      dwChaserIndex=PIN_FIRST ;
    }

    delay( 500 ) ;              // wait for a second
  }
}
