/*
 ESP8266-specific implementation of the UART download mode
 Copyright (c) 2021 Timo Wischer <twischer@freenet.de>
 All rights reserved.
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

 This implementation is based on the original implementation of the ROM.
 It was shortend to reduce the memory usage. The complete version and the
 development history can be found in:
 https://github.com/twischer/Arduino/tree/reboot_uart_download_full
 This might be usefull in case of issues.
 */
#include "reboot_uart_dwnld.h"
#include <stdnoreturn.h>
#include <user_interface.h>
#include <esp8266_undocumented.h>


static inline uint32_t __rsil_1() {
	uint32_t program_state;
	asm volatile("rsil %0, 1" : "=r" (program_state));
	return program_state;
}

static inline void __wsr_intenable(uint32_t interupt_enable) {
	asm volatile("wsr.intenable %0" :: "r" (interupt_enable));
}

static inline void __wsr_litbase(uint32_t literal_base) {
	asm volatile("wsr.litbase %0" :: "r" (literal_base));
}

static inline void __wsr_ps(uint32_t program_state) {
	asm volatile("wsr.ps %0" :: "r" (program_state));
}

static inline void __wsr_vecbase(uint32_t vector_base) {
	asm volatile("wsr.vecbase %0" :: "r" (vector_base));
}

[[noreturn]] void IRAM_ATTR esp8266UartDownloadMode()
{
	/* reverse engineered from system_restart_core() */
	/* Before disabling instruction cache and restoring instruction RAM to a
	 * power-on like state, SPI bus must be idle.
	 */
	Wait_SPI_Idle(flashchip);

	Cache_Read_Disable();
	/* This will disable the 32kB instruction cache and extend the IRAM by 32kB.
	 * Therefore the full 64kB of IRAM will be available for boot.
	 * Cache_Read_Enable() sets those bits but Cache_Read_Disable() does not clear
	 * them. On hardware reset those bits are cleared. Therefore clear them also
	 * for this reboot.
	 */
	CLEAR_PERI_REG_MASK(PERIPHS_DPORT_ICACHE_ENABLE,
		ICACHE_ENABLE_FIRST_16K | ICACHE_ENABLE_SECOND_16K);

	/* reverse engineered from _ResetHandler() */
	/* disable all level 1 interrupts */
	__wsr_intenable(0);
	/* Clear the literal base to use an offset of 0 for
	 * Load 32-bit PC-Relative(L32R) instructions
	 */
	__wsr_litbase(0);
	asm volatile("rsync");

	/* Set interrupt vector base address to system ROM */
	__wsr_vecbase(0x40000000);
	/* Set interrupt level to 1. Therefore disable interrupts of level 1.
	 * Above levels like level 2,... might still be active if available
	 * on ESP8266.
	 */
	__rsil_1();

	/* reverse engineered from _start() */
	/* Set stack pointer to upper end of data RAM */
	const uint32_t stack_pointer = 0x40000000;
	asm volatile("mov a1, %0" :: "r" (stack_pointer));

	/* Set the program state register
	 * Name				Value	Description
	 * Interrupt level disable	0	enable all interrupt levels
	 * Exception mode		0	normal operation
	 * User vector mode		1	user vector mode, exceptions need to switch stacks
	 * Privilege level		0	Set to Ring 0
	 */
	__wsr_ps(0x20);
	asm volatile("rsync");

	/* reverse engineered from main() */
	const uint32_t uart_no = 0;
	uartAttach();
	Uart_Init(uart_no);
	ets_install_uart_printf(uart_no);

	/* reverse engineered from boot_from_something() */
	const uint16_t divlatch = uart_baudrate_detect(uart_no, 0);
	rom_uart_div_modify(uart_no, divlatch);
	UartDwnLdProc((uint8_t*)0x3fffa000, 0x2000, &user_start_fptr);

	/* reverse engineered from main() */
	if (user_start_fptr == NULL) {
		if (boot_from_flash() != 0) {
			ets_printf("boot_from_flash() failed\n");
			while (true);
		}
	}

	if (user_start_fptr) {
		user_start_fptr();
	}

	ets_printf("user code done\n");
	ets_run();
}

[[noreturn]] void esp8266RebootIntoUartDownloadMode()
{
	/* reverse engineered from system_restart_local() */
	if (system_func1(0x4) == -1) {
		clockgate_watchdog(0);
		SET_PERI_REG_MASK(PERIPHS_DPORT_18, 0xffff00ff);
		pm_open_rf();
	}

	user_uart_wait_tx_fifo_empty(0, 0x7a120);
	user_uart_wait_tx_fifo_empty(1, 0x7a120);
	ets_intr_lock();
	SET_PERI_REG_MASK(PERIPHS_DPORT_18, 0x7500);
	CLEAR_PERI_REG_MASK(PERIPHS_DPORT_18, 0x7500);
	SET_PERI_REG_MASK(PERIPHS_I2C_48, 0x2);
	CLEAR_PERI_REG_MASK(PERIPHS_I2C_48, 0x2);

	esp8266UartDownloadMode();
}

