#ifndef Pins_Arduino_h
#define Pins_Arduino_h

//#ifdef ARDUINO_MAIN

#define PIN_LED_BLUE         (0u)
#define PIN_LED_GREEN        (1u)
#define PIN_LED_RED          (2u)
#define PIN_LED              PIN_LED_BLUE

#define PIN_DAC0             (u)

static const uint8_t SS   = 34 ;
static const uint8_t MOSI = 32 ;
static const uint8_t MISO = 31 ;
static const uint8_t SCK  = 33 ;

#define PINS_UART            (16u)

#if defined VARIANT_REV_A
#  define PIN_TSC_IRQ_WUP_ID (1UL << 3)
#elif defined VARIANT_REV_B
#  define PIN_TSC_IRQ_WUP_ID (1UL << 15)
#else
#error "No board revision defined"
#endif

#define BOARD_LCD_PINS              PIN_EBI_DATA_BUS, PIN_EBI_NRD, PIN_EBI_NWE, PIN_EBI_NCS1, PIN_EBI_LCD_RS
#define BOARD_LCD_BASE              0x61000000 /** Define ILI9325 base address. */
#define BOARD_LCD_RS                (1 << 1) /** Define ILI9325 register select signal. */

//#endif // ARDUINO_MAIN

#endif // Pins_Arduino_h
