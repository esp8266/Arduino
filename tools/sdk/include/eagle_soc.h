/*
 * ESPRESSIF MIT License
 *
 * Copyright (c) 2016 <ESPRESSIF SYSTEMS (SHANGHAI) PTE LTD>
 *
 * Permission is hereby granted for use on ESPRESSIF SYSTEMS ESP8266 only, in which case,
 * it is free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the Software is furnished
 * to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef _EAGLE_SOC_H_
#define _EAGLE_SOC_H_

//Register Bits{{
#define BIT31   0x80000000
#define BIT30   0x40000000
#define BIT29   0x20000000
#define BIT28   0x10000000
#define BIT27   0x08000000
#define BIT26   0x04000000
#define BIT25   0x02000000
#define BIT24   0x01000000
#define BIT23   0x00800000
#define BIT22   0x00400000
#define BIT21   0x00200000
#define BIT20   0x00100000
#define BIT19   0x00080000
#define BIT18   0x00040000
#define BIT17   0x00020000
#define BIT16   0x00010000
#define BIT15   0x00008000
#define BIT14   0x00004000
#define BIT13   0x00002000
#define BIT12   0x00001000
#define BIT11   0x00000800
#define BIT10   0x00000400
#define BIT9     0x00000200
#define BIT8     0x00000100
#define BIT7     0x00000080
#define BIT6     0x00000040
#define BIT5     0x00000020
#define BIT4     0x00000010
#define BIT3     0x00000008
#define BIT2     0x00000004
#define BIT1     0x00000002
#define BIT0     0x00000001
//}}

//Registers Operation {{
#define ETS_UNCACHED_ADDR(addr) (addr)
#define ETS_CACHED_ADDR(addr) (addr)


#define READ_PERI_REG(addr)                        (*((volatile uint32_t *)ETS_UNCACHED_ADDR(addr)))
#define WRITE_PERI_REG(addr, val)                  (*((volatile uint32_t *)ETS_UNCACHED_ADDR(addr))) = (uint32_t)(val)
#define CLEAR_PERI_REG_MASK(reg, mask)             WRITE_PERI_REG((reg), (READ_PERI_REG(reg)&(~(mask))))
#define SET_PERI_REG_MASK(reg, mask)               WRITE_PERI_REG((reg), (READ_PERI_REG(reg)|(mask)))
#define GET_PERI_REG_BITS(reg, hipos,lowpos)       ((READ_PERI_REG(reg)>>(lowpos))&((1<<((hipos)-(lowpos)+1))-1))
#define SET_PERI_REG_BITS(reg,bit_map,value,shift) (WRITE_PERI_REG((reg),(READ_PERI_REG(reg)&(~((bit_map)<<(shift))))|((value)<<(shift)) ))
//}}

//Periheral Clock {{
#define  CPU_CLK_FREQ                       80*1000000       //unit: Hz
#define  APB_CLK_FREQ                       CPU_CLK_FREQ
#define  UART_CLK_FREQ                      APB_CLK_FREQ
#define  TIMER_CLK_FREQ                     (APB_CLK_FREQ>>8) //divided by 256
//}}

//Peripheral device base address define{{
#define PERIPHS_DPORT_BASEADDR              0x3ff00000
#define PERIPHS_GPIO_BASEADDR               0x60000300
#define PERIPHS_TIMER_BASEDDR               0x60000600
#define PERIPHS_RTC_BASEADDR                0x60000700
#define PERIPHS_IO_MUX                      0x60000800
//}}

//Interrupt remap control registers define{{
#define EDGE_INT_ENABLE_REG                (PERIPHS_DPORT_BASEADDR+0x04)
#define TM1_EDGE_INT_ENABLE()              SET_PERI_REG_MASK(EDGE_INT_ENABLE_REG, BIT1)
#define TM1_EDGE_INT_DISABLE()             CLEAR_PERI_REG_MASK(EDGE_INT_ENABLE_REG, BIT1)
//}}

//GPIO reg {{
#define GPIO_REG_READ(reg)                 READ_PERI_REG(PERIPHS_GPIO_BASEADDR + (reg))
#define GPIO_REG_WRITE(reg, val)           WRITE_PERI_REG(PERIPHS_GPIO_BASEADDR + (reg), val)
#define GPIO_OUT_ADDRESS                   0x00
#define GPIO_OUT_W1TS_ADDRESS              0x04
#define GPIO_OUT_W1TC_ADDRESS              0x08

#define GPIO_ENABLE_ADDRESS                0x0c
#define GPIO_ENABLE_W1TS_ADDRESS           0x10
#define GPIO_ENABLE_W1TC_ADDRESS           0x14
#define GPIO_OUT_W1TC_DATA_MASK            0x0000ffff

#define GPIO_IN_ADDRESS                    0x18

#define GPIO_STATUS_ADDRESS                0x1c
#define GPIO_STATUS_W1TS_ADDRESS           0x20
#define GPIO_STATUS_W1TC_ADDRESS           0x24
#define GPIO_STATUS_INTERRUPT_MASK         0x0000ffff

#define GPIO_RTC_CALIB_SYNC                PERIPHS_GPIO_BASEADDR+0x6c
#define RTC_CALIB_START                    BIT31  //first write to zero, then to one to start
#define RTC_PERIOD_NUM_MASK                0x3ff   //max 8ms
#define GPIO_RTC_CALIB_VALUE               PERIPHS_GPIO_BASEADDR+0x70
#define RTC_CALIB_RDY_S                    31  //after measure, flag to one, when start from zero to one, turn to zero
#define RTC_CALIB_VALUE_MASK               0xfffff

#define GPIO_PIN0_ADDRESS                  0x28

#define GPIO_ID_PIN0                       0
#define GPIO_ID_PIN(n)                     (GPIO_ID_PIN0+(n))
#define GPIO_LAST_REGISTER_ID              GPIO_ID_PIN(15)
#define GPIO_ID_NONE                       0xffffffff

#define GPIO_PIN_COUNT                     16

#define GPIO_PIN_CONFIG_MSB                12
#define GPIO_PIN_CONFIG_LSB                11
#define GPIO_PIN_CONFIG_MASK               0x00001800
#define GPIO_PIN_CONFIG_GET(x)             (((x) & GPIO_PIN_CONFIG_MASK) >> GPIO_PIN_CONFIG_LSB)
#define GPIO_PIN_CONFIG_SET(x)             (((x) << GPIO_PIN_CONFIG_LSB) & GPIO_PIN_CONFIG_MASK)

#define GPIO_WAKEUP_ENABLE                 1
#define GPIO_WAKEUP_DISABLE                (~GPIO_WAKEUP_ENABLE)
#define GPIO_PIN_WAKEUP_ENABLE_MSB         10
#define GPIO_PIN_WAKEUP_ENABLE_LSB         10
#define GPIO_PIN_WAKEUP_ENABLE_MASK        0x00000400
#define GPIO_PIN_WAKEUP_ENABLE_GET(x)      (((x) & GPIO_PIN_WAKEUP_ENABLE_MASK) >> GPIO_PIN_WAKEUP_ENABLE_LSB)
#define GPIO_PIN_WAKEUP_ENABLE_SET(x)      (((x) << GPIO_PIN_WAKEUP_ENABLE_LSB) & GPIO_PIN_WAKEUP_ENABLE_MASK)

#define GPIO_PIN_INT_TYPE_MASK             0x380
#define GPIO_PIN_INT_TYPE_MSB              9
#define GPIO_PIN_INT_TYPE_LSB              7
#define GPIO_PIN_INT_TYPE_GET(x)           (((x) & GPIO_PIN_INT_TYPE_MASK) >> GPIO_PIN_INT_TYPE_LSB)
#define GPIO_PIN_INT_TYPE_SET(x)           (((x) << GPIO_PIN_INT_TYPE_LSB) & GPIO_PIN_INT_TYPE_MASK)

#define GPIO_PAD_DRIVER_ENABLE             1
#define GPIO_PAD_DRIVER_DISABLE            (~GPIO_PAD_DRIVER_ENABLE)
#define GPIO_PIN_PAD_DRIVER_MSB            2
#define GPIO_PIN_PAD_DRIVER_LSB            2
#define GPIO_PIN_PAD_DRIVER_MASK           0x00000004
#define GPIO_PIN_PAD_DRIVER_GET(x)         (((x) & GPIO_PIN_PAD_DRIVER_MASK) >> GPIO_PIN_PAD_DRIVER_LSB)
#define GPIO_PIN_PAD_DRIVER_SET(x)         (((x) << GPIO_PIN_PAD_DRIVER_LSB) & GPIO_PIN_PAD_DRIVER_MASK)

#define GPIO_AS_PIN_SOURCE                 0
#define SIGMA_AS_PIN_SOURCE                (~GPIO_AS_PIN_SOURCE)
#define GPIO_PIN_SOURCE_MSB                0
#define GPIO_PIN_SOURCE_LSB                0
#define GPIO_PIN_SOURCE_MASK               0x00000001
#define GPIO_PIN_SOURCE_GET(x)             (((x) & GPIO_PIN_SOURCE_MASK) >> GPIO_PIN_SOURCE_LSB)
#define GPIO_PIN_SOURCE_SET(x)             (((x) << GPIO_PIN_SOURCE_LSB) & GPIO_PIN_SOURCE_MASK)
// }}

// TIMER reg {{
#define TIMER_REG_READ(addr)               READ_PERI_REG(PERIPHS_TIMER_BASEDDR + addr)
#define TIMER_REG_WRITE(addr, val)         WRITE_PERI_REG(PERIPHS_TIMER_BASEDDR + addr, val)
#define TIMER_CLR_REG_MASK(reg, mask)      CLEAR_PERI_REG_MASK(PERIPHS_TIMER_BASEDDR +reg, mask)


//Previous definitions of the above, kept for a while for possible compatibility, but deprecated
#define RTC_REG_READ(addr)                 _Pragma("GCC warning \"'RTC_REG_READ' macro is deprecated\"")     TIMER_REG_READ(addr)
#define RTC_REG_WRITE(addr, val)           _Pragma("GCC warning \"'RTC_REG_WRITE' macro is deprecated\"")    TIMER_REG_WRITE(addr, val)
#define RTC_CLR_REG_MASK(reg, mask)        _Pragma("GCC warning \"'RTC_CLR_REG_MASK' macro is deprecated\"") TIMER_CLR_REG_MASK(reg, mask)



//load initial_value to timer1
#define FRC1_LOAD_ADDRESS                  0x00

//timer1's counter value(count from initial_value to 0)
#define FRC1_COUNT_ADDRESS                 0x04

#define FRC1_CTRL_ADDRESS                  0x08

//clear timer1's interrupt when write this address
#define FRC1_INT_ADDRESS                   0x0c
#define FRC1_INT_CLR_MASK                  0x00000001

//timer2's counter value(count from initial_value to 0)
#define FRC2_COUNT_ADDRESS                 0x24
// }}

/* Returns the current time according to the timer timer. */
#define NOW()                              TIMER_REG_READ(FRC2_COUNT_ADDRESS)

//RTC reg {{
#define REG_RTC_BASE  PERIPHS_RTC_BASEADDR

#define RTC_STORE0                              (REG_RTC_BASE + 0x030)
#define RTC_STORE1                              (REG_RTC_BASE + 0x034)
#define RTC_STORE2                              (REG_RTC_BASE + 0x038)
#define RTC_STORE3                              (REG_RTC_BASE + 0x03C)

#define RTC_GPIO_OUT                            (REG_RTC_BASE + 0x068)
#define RTC_GPIO_ENABLE                         (REG_RTC_BASE + 0x074)
#define RTC_GPIO_IN_DATA                        (REG_RTC_BASE + 0x08C)
#define RTC_GPIO_CONF                           (REG_RTC_BASE + 0x090)
#define PAD_XPD_DCDC_CONF                       (REG_RTC_BASE + 0x0A0)
//}}

//PIN Mux reg {{
#define PERIPHS_IO_MUX_FUNC             0x13
#define PERIPHS_IO_MUX_FUNC_S           4
#define PERIPHS_IO_MUX_PULLUP           BIT7
#define PERIPHS_IO_MUX_PULLUP2          BIT6
#define PERIPHS_IO_MUX_SLEEP_PULLUP     BIT3
#define PERIPHS_IO_MUX_SLEEP_PULLUP2    BIT2
#define PERIPHS_IO_MUX_SLEEP_OE         BIT1
#define PERIPHS_IO_MUX_OE               BIT0

#define PERIPHS_IO_MUX_CONF_U           (PERIPHS_IO_MUX + 0x00)
#define SPI0_CLK_EQU_SYS_CLK            BIT8
#define SPI1_CLK_EQU_SYS_CLK            BIT9

#define PERIPHS_IO_MUX_MTDI_U           (PERIPHS_IO_MUX + 0x04)
#define FUNC_MTDI                           0
#define FUNC_I2SI_DATA                      1
#define FUNC_HSPIQ_MISO                     2
#define FUNC_GPIO12                         3
#define FUNC_UART0_DTR                      4

#define PERIPHS_IO_MUX_MTCK_U           (PERIPHS_IO_MUX + 0x08)
#define FUNC_MTCK                           0
#define FUNC_I2SI_BCK                       1
#define FUNC_HSPID_MOSI                     2
#define FUNC_GPIO13                         3
#define FUNC_UART0_CTS                      4

#define PERIPHS_IO_MUX_MTMS_U           (PERIPHS_IO_MUX + 0x0C)
#define FUNC_MTMS                           0
#define FUNC_I2SI_WS                        1
#define FUNC_HSPI_CLK                       2
#define FUNC_GPIO14                         3
#define FUNC_UART0_DSR                      4

#define PERIPHS_IO_MUX_MTDO_U           (PERIPHS_IO_MUX + 0x10)
#define FUNC_MTDO                           0
#define FUNC_I2SO_BCK                       1
#define FUNC_HSPI_CS0                       2
#define FUNC_GPIO15                         3
#define FUNC_U0RTS                          4
#define FUNC_UART0_RTS                      4

#define PERIPHS_IO_MUX_U0RXD_U          (PERIPHS_IO_MUX + 0x14)
#define FUNC_U0RXD                          0
#define FUNC_I2SO_DATA                      1
#define FUNC_GPIO3                          3
#define FUNC_CLK_XTAL_BK                    4

#define PERIPHS_IO_MUX_U0TXD_U          (PERIPHS_IO_MUX + 0x18)
#define FUNC_U0TXD                          0
#define FUNC_SPICS1                         1
#define FUNC_GPIO1                          3
#define FUNC_CLK_RTC_BK                     4

#define PERIPHS_IO_MUX_SD_CLK_U         (PERIPHS_IO_MUX + 0x1c)
#define FUNC_SDCLK                          0
#define FUNC_SPICLK                         1
#define FUNC_GPIO6                          3
#define UART1_CTS                           4

#define PERIPHS_IO_MUX_SD_DATA0_U       (PERIPHS_IO_MUX + 0x20)
#define FUNC_SDDATA0                        0
#define FUNC_SPIQ_MISO                      1
#define FUNC_SPIQ                           1
#define FUNC_GPIO7                          3
#define FUNC_U1TXD                          4
#define FUNC_UART1_TXD                      4

#define PERIPHS_IO_MUX_SD_DATA1_U       (PERIPHS_IO_MUX + 0x24)
#define FUNC_SDDATA1                        0
#define FUNC_SPID_MOSI                      1
#define FUNC_SPID                           1
#define FUNC_GPIO8                          3
#define FUNC_U1RXD                          4
#define FUNC_UART1_RXD                      4
#define FUNC_SDDATA1_U1RXD                  7

#define PERIPHS_IO_MUX_SD_DATA2_U       (PERIPHS_IO_MUX + 0x28)
#define FUNC_SDDATA2                        0
#define FUNC_SPIHD                          1
#define FUNC_GPIO9                          3
#define UFNC_HSPIHD                         4

#define PERIPHS_IO_MUX_SD_DATA3_U       (PERIPHS_IO_MUX + 0x2c)
#define FUNC_SDDATA3                        0
#define FUNC_SPIWP                          1
#define FUNC_GPIO10                         3
#define FUNC_HSPIWP                         4

#define PERIPHS_IO_MUX_SD_CMD_U         (PERIPHS_IO_MUX + 0x30)
#define FUNC_SDCMD                          0
#define FUNC_SPICS0                         1
#define FUNC_GPIO11                         3
#define U1RTS                               4
#define UART1_RTS                           4

#define PERIPHS_IO_MUX_GPIO0_U          (PERIPHS_IO_MUX + 0x34)
#define FUNC_GPIO0                          0
#define FUNC_SPICS2                         1
#define FUNC_CLK_OUT                        4

#define PERIPHS_IO_MUX_GPIO2_U          (PERIPHS_IO_MUX + 0x38)
#define FUNC_GPIO2                          0
#define FUNC_I2SO_WS                        1
#define FUNC_U1TXD_BK                       2
#define FUNC_UART1_TXD_BK                   2
#define FUNC_U0TXD_BK                       4
#define FUNC_UART0_TXD_BK                   4

#define PERIPHS_IO_MUX_GPIO4_U          (PERIPHS_IO_MUX + 0x3C)
#define FUNC_GPIO4                          0
#define FUNC_CLK_XTAL                       1

#define PERIPHS_IO_MUX_GPIO5_U          (PERIPHS_IO_MUX + 0x40)
#define FUNC_GPIO5                          0
#define FUNC_CLK_RTC                        1

#define PIN_PULLUP_DIS(PIN_NAME)                 CLEAR_PERI_REG_MASK(PIN_NAME, PERIPHS_IO_MUX_PULLUP)
#define PIN_PULLUP_EN(PIN_NAME)                  SET_PERI_REG_MASK(PIN_NAME, PERIPHS_IO_MUX_PULLUP)

#define PIN_FUNC_SELECT(PIN_NAME, FUNC)  do { \
    WRITE_PERI_REG(PIN_NAME,   \
                                (READ_PERI_REG(PIN_NAME) \
                                     &  (~(PERIPHS_IO_MUX_FUNC<<PERIPHS_IO_MUX_FUNC_S)))  \
                                     |( (((FUNC&BIT2)<<2)|(FUNC&0x3))<<PERIPHS_IO_MUX_FUNC_S) );  \
    } while (0)

//}}

#endif //_EAGLE_SOC_H_
