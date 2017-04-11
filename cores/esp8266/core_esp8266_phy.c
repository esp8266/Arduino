/*
 phy.c - ESP8266 PHY initialization data

 Copyright (c) 2015 Ivan Grokhotkov. All rights reserved.
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


 #include <stdint.h>
 #include <stddef.h>
 #include <stdbool.h>
 #include <string.h>
 #include "c_types.h"
 #include "ets_sys.h"
 #include "spi_flash.h"

static const uint8_t ICACHE_FLASH_ATTR phy_init_data[128] =
{
    [0] = 5,  // Reserved, do not change
    [1] = 0,  // Reserved, do not change
    [2] = 4,  // Reserved, do not change
    [3] = 2,  // Reserved, do not change
    [4] = 5,  // Reserved, do not change
    [5] = 5,  // Reserved, do not change
    [6] = 5,  // Reserved, do not change
    [7] = 2,  // Reserved, do not change
    [8] = 5,  // Reserved, do not change
    [9] = 0,  // Reserved, do not change
    [10] = 4,  // Reserved, do not change
    [11] = 5,  // Reserved, do not change
    [12] = 5,  // Reserved, do not change
    [13] = 4,  // Reserved, do not change
    [14] = 5,  // Reserved, do not change
    [15] = 5,  // Reserved, do not change
    [16] = 4,  // Reserved, do not change
    [17] = -2, // Reserved, do not change
    [18] = -3, // Reserved, do not change
    [19] = -1, // Reserved, do not change
    [20] = -16, // Reserved, do not change
    [21] = -16, // Reserved, do not change
    [22] = -16, // Reserved, do not change
    [23] = -32, // Reserved, do not change
    [24] = -32, // Reserved, do not change
    [25] = -32, // Reserved, do not change

    [26] = 225, // spur_freq_cfg, spur_freq=spur_freq_cfg/spur_freq_cfg_div
    [27] = 10,  // spur_freq_cfg_div
    // each bit for 1 channel, 1 to select the spur_freq if in band, else 40
    [28] = 0xff,   // spur_freq_en_h
    [29] = 0xff,   // spur_freq_en_l

    [30] = 0xf8, // Reserved, do not change
    [31] = 0,    // Reserved, do not change
    [32] = 0xf8, // Reserved, do not change
    [33] = 0xf8, // Reserved, do not change

    [34] = 82, // target_power_qdb_0, 82 means target power is 82/4=20.5dbm
    [35] = 78, // target_power_qdb_1, 78 means target power is 78/4=19.5dbm
    [36] = 74, // target_power_qdb_2, 74 means target power is 74/4=18.5dbm
    [37] = 68, // target_power_qdb_3, 68 means target power is 68/4=17dbm
    [38] = 64, // target_power_qdb_4, 64 means target power is 64/4=16dbm
    [39] = 56, // target_power_qdb_5, 56 means target power is 56/4=14dbm

    [40] = 0, // target_power_index_mcs0
    [41] = 0, // target_power_index_mcs1
    [42] = 1, // target_power_index_mcs2
    [43] = 1, // target_power_index_mcs3
    [44] = 2, // target_power_index_mcs4
    [45] = 3, // target_power_index_mcs5
    [46] = 4, // target_power_index_mcs6
    [47] = 5, // target_power_index_mcs7

    // crystal_26m_en
    // 0: 40MHz
    // 1: 26MHz
    // 2: 24MHz
    #if F_CRYSTAL == 40000000
      [48] = 0,
    #else
      [48] = 1,
    #endif



    // sdio_configure
    // 0: Auto by pin strapping
    // 1: SDIO dataoutput is at negative edges (SDIO V1.1)
    // 2: SDIO dataoutput is at positive edges (SDIO V2.0)
    [50] = 0,

    // bt_configure
    // 0: None,no bluetooth
    // 1: GPIO0 -> WLAN_ACTIVE/ANT_SEL_WIFI
    //    MTMS -> BT_ACTIVE
    //    MTCK  -> BT_PRIORITY
    //    U0RXD -> ANT_SEL_BT
    // 2: None, have bluetooth
    // 3: GPIO0 -> WLAN_ACTIVE/ANT_SEL_WIFI
    //    MTMS -> BT_PRIORITY
    //    MTCK  -> BT_ACTIVE
    //    U0RXD -> ANT_SEL_BT
    [51] = 0,

    // bt_protocol
    // 0: WiFi-BT are not enabled. Antenna is for WiFi
    // 1: WiFi-BT are not enabled. Antenna is for BT
    // 2: WiFi-BT 2-wire are enabled, (only use BT_ACTIVE), independent ant
    // 3: WiFi-BT 3-wire are enabled, (when BT_ACTIVE = 0, BT_PRIORITY must be 0), independent ant
    // 4: WiFi-BT 2-wire are enabled, (only use BT_ACTIVE), share ant
    // 5: WiFi-BT 3-wire are enabled, (when BT_ACTIVE = 0, BT_PRIORITY must be 0), share ant
    [52] = 0,

    // dual_ant_configure
    // 0: None
    // 1: dual_ant (antenna diversity for WiFi-only): GPIO0 + U0RXD
    // 2: T/R switch for External PA/LNA:  GPIO0 is high and U0RXD is low during Tx
    // 3: T/R switch for External PA/LNA:  GPIO0 is low and U0RXD is high during Tx
    [53] = 0,

    [54] = 2, // Reserved, do not change

    // share_xtal
    // This option is to share crystal clock for BT
    // The state of Crystal during sleeping
    // 0: Off
    // 1: Forcely On
    // 2: Automatically On according to XPD_DCDC
    // 3: Automatically On according to GPIO2
    [55] = 0,

    [64] = 225, // spur_freq_cfg_2, spur_freq_2=spur_freq_cfg_2/spur_freq_cfg_div_2
    [65] = 10,  // spur_freq_cfg_div_2
    [66] = 0,   // spur_freq_en_h_2
    [67] = 0,   // spur_freq_en_l_2
    [68] = 0,   // spur_freq_cfg_msb
    [69] = 0,   // spur_freq_cfg_2_msb
    [70] = 0,   // spur_freq_cfg_3_low
    [71] = 0,   // spur_freq_cfg_3_high
    [72] = 0,   // spur_freq_cfg_4_low
    [73] = 0,   // spur_freq_cfg_4_high

    [74] = 1,   // Reserved, do not change
    [75] = 0x93,   // Reserved, do not change
    [76] = 0x43,   // Reserved, do not change
    [77] = 0x00,   // Reserved, do not change

    // low_power_en
    // 0: disable low power mode
    // 1: enable low power mode
    [93] = 0,

    // lp_rf_stg10
    // the attenuation of RF gain stage 0 and 1,
    // 0xf: 0db,
    // 0xe: -2.5db,
    // 0xd: -6db,
    // 0x9: -8.5db,
    // 0xc: -11.5db,
    // 0x8: -14db,
    // 0x4: -17.5,
    // 0x0: -23
    [94] = 0x00,


    // lp_bb_att_ext
    // the attenuation of BB gain,
    // 0: 0db,
    // 1: -0.25db,
    // 2: -0.5db,
    // 3: -0.75db,
    // 4: -1db,
    // 5: -1.25db,
    // 6: -1.5db,
    // 7: -1.75db,
    // 8: -2db
    // max valve is 24(-6db)
    [95] = 0,

    // pwr_ind_11b_en
    // 0: 11b power is same as mcs0 and 6m
    // 1: enable 11b power different with ofdm
    [96] = 0,

    // pwr_ind_11b_0
    // 1m, 2m power index [0~5]
    [97] = 0,

    // pwr_ind_11b_1
    // 5.5m, 11m power index [0~5]
    [98] = 0,

    // vdd33_const
    // the voltage of PA_VDD
    // x=0xff: it can measure VDD33,
    // 18<=x<=36: use input voltage,
    // the value is voltage*10, 33 is 3.3V, 30 is 3.0V,
    // x<18 or x>36: default voltage is 3.3V
    //
    // the value of this byte depend from the TOUT pin usage (1 or 2):
    // 1)
    // analogRead function (system_adc_read()):
    // is only available when wire TOUT pin17 to external circuitry, Input Voltage Range restricted to 0 ~ 1.0V.
    // For this function the vdd33_const must be set as real power voltage of VDD3P3 pin 3 and 4
    // The range of operating voltage of ESP8266 is 1.8V~3.6V，the unit of vdd33_const is 0.1V，so effective value range of vdd33_const is [18,36]
    // 2)
    // getVcc function (system_get_vdd33):
    // is only available when TOUT pin17 is suspended (floating), this function measure the power voltage of VDD3P3 pin 3 and 4
    // For this function the vdd33_const must be set to 255 (0xFF).
    [107] = 33,

    // disable RF calibration for certain number of times
    [108] = 0,

    // freq_correct_en
    // bit[0]:0->do not correct frequency offset, 1->correct frequency offset.
    // bit[1]:0->bbpll is 168M, it can correct + and - frequency offset,  1->bbpll is 160M, it only can correct + frequency offset
    // bit[2]:0->auto measure frequency offset and correct it, 1->use 113 byte force_freq_offset to correct frequency offset.
    // 0: do not correct frequency offset.
    // 1: auto measure frequency offset and correct it,  bbpll is 168M, it can correct + and - frequency offset.
    // 3: auto measure frequency offset and correct it,  bbpll is 160M, it only can correct + frequency offset.
    // 5: use 113 byte force_freq_offset to correct frequency offset, bbpll is 168M, it can correct + and - frequency offset.
    // 7: use 113 byte force_freq_offset to correct frequency offset, bbpll is 160M , it only can correct + frequency offset.
    [112] = 0,

    // force_freq_offset
    // signed, unit is 8kHz
    [113] = 0,

    // rf_cal_use_flash
    // 0: RF init no RF CAL, using all RF CAL data in flash, it takes about 2ms for RF init
    // 1: RF init only do TX power control CAL, others using RF CAL data in flash, it takes about 20ms for RF init
    // 2: RF init no RF CAL, using all RF CAL data in flash, it takes about 2ms for RF init  (same as 0?!)
    // 3: RF init do all RF CAL, it takes about 200ms for RF init
    [114] = 1
};

// These functions will be overriden from C++ code.
// Unfortunately, we can't use extern "C" because Arduino preprocessor
// doesn't generate forward declarations for extern "C" functions correctly,
// so we use mangled names here.
#define __get_adc_mode _Z14__get_adc_modev
#define __get_rf_mode _Z13__get_rf_modev
#define __run_user_rf_pre_init _Z22__run_user_rf_pre_initv

static bool spoof_init_data = false;

extern int __real_spi_flash_read(uint32_t addr, uint32_t* dst, size_t size);
extern int ICACHE_RAM_ATTR __wrap_spi_flash_read(uint32_t addr, uint32_t* dst, size_t size);

extern int ICACHE_RAM_ATTR __wrap_spi_flash_read(uint32_t addr, uint32_t* dst, size_t size)
{
    if (!spoof_init_data || size != 128) {
        return __real_spi_flash_read(addr, dst, size);
    }

    memcpy(dst, phy_init_data, sizeof(phy_init_data));
    ((uint8_t*)dst)[107] = __get_adc_mode();
    return 0;
}

extern int __get_rf_mode(void)  __attribute__((weak));
extern int __get_rf_mode(void)
{
    return -1;  // mode not set
}

extern int __get_adc_mode(void) __attribute__((weak));
extern int __get_adc_mode(void)
{
    return 33; // default ADC mode
}

extern void __run_user_rf_pre_init(void) __attribute__((weak));
extern void __run_user_rf_pre_init(void)
{
    return; // default do noting
}

uint32_t user_rf_cal_sector_set(void)
{
    spoof_init_data = true;
    return flashchip->chip_size/SPI_FLASH_SEC_SIZE - 4;
}

void user_rf_pre_init()
{
    // *((volatile uint32_t*) 0x60000710) = 0;
    spoof_init_data = false;
    volatile uint32_t* rtc_reg = (volatile uint32_t*) 0x60001000;
    if((rtc_reg[24] >> 16) > 4) {
        rtc_reg[24] &= 0xFFFF;
        rtc_reg[30] = 0;
    }

    system_set_os_print(0);
    int rf_mode = __get_rf_mode();
    if (rf_mode >= 0) {
        system_phy_set_rfoption(rf_mode);
    }
    __run_user_rf_pre_init();
}


void ICACHE_RAM_ATTR user_spi_flash_dio_to_qio_pre_init() {}
