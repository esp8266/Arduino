/*
 * debug.c
 *
 *  Created on: 13.05.2015
 *      Author: Markus Sattler
 */

#include "Arduino.h"

void ICACHE_RAM_ATTR hexdump(uint8_t *mem, uint32_t len, uint8_t cols) {
    os_printf("\n[HEXDUMP] Address: 0x%08X len: 0x%X (%d)", mem, len, len);
    for(uint32_t i = 0; i < len; i++) {
        if(i % cols == 0) {
            os_printf("\n[0x%08X] 0x%08X: ", mem, i);
        }
        os_printf("%02X ", *mem);
        mem++;
    }
    os_printf("\n");
}

