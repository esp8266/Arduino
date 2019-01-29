#ifndef GDBSTUB_CFG_H
#define GDBSTUB_CFG_H

/*
Enable this define if you're using the RTOS SDK. It will use a custom exception handler instead of the HAL
and do some other magic to make everything work and compile under FreeRTOS.
*/
#ifndef GDBSTUB_FREERTOS
#define GDBSTUB_FREERTOS 0
#endif

/*
Enable this to make the exception and debugging handlers switch to a private stack. This will use
up 1K of RAM, but may be useful if you're debugging stack or stack pointer corruption problems. It's
normally disabled because not many situations need it. If for some reason the GDB communication
stops when you run into an error in your code, try enabling this.
*/
#ifndef GDBSTUB_USE_OWN_STACK
#define GDBSTUB_USE_OWN_STACK 0
#endif

/*
Enable this to cause the program to pause and wait for gdb to be connected when an exception is
encountered.
*/
#ifndef GDBSTUB_BREAK_ON_EXCEPTION
#define GDBSTUB_BREAK_ON_EXCEPTION 1
#endif

/*
If this is defined, gdbstub will break the program when you press Ctrl-C in gdb. it does this by
hooking the UART interrupt. Unfortunately, this means receiving stuff over the serial port won't
work for your program anymore. This will fail if your program sets an UART interrupt handler after
the gdbstub_init call.
*/
#ifndef GDBSTUB_CTRLC_BREAK
#define GDBSTUB_CTRLC_BREAK 1
#endif

/*
Enabling this will redirect console output to GDB. This basically means that printf/os_printf output
will show up in your gdb session, which is useful if you use gdb to do stuff. It also means that if
you use a normal terminal, you can't read the printfs anymore.
*/
#ifndef GDBSTUB_REDIRECT_CONSOLE_OUTPUT
#define GDBSTUB_REDIRECT_CONSOLE_OUTPUT 1
#endif

/*
Enable this if you want the GDB stub to wait for you to attach GDB before running. It does this by
breaking in the init routine; use the gdb 'c' command (continue) to start the program.
*/
#ifndef GDBSTUB_BREAK_ON_INIT
#define GDBSTUB_BREAK_ON_INIT 0
#endif

/*
Function attributes for function types.
Gdbstub functions are placed in flash or IRAM using attributes, as defined here. The gdbinit function
(and related) can always be in flash, because it's called in the normal code flow. The rest of the
gdbstub functions can be in flash too, but only if there's no chance of them being called when the
flash somehow is disabled (eg during SPI operations or flash write/erase operations). If the routines
are called when the flash is disabled (eg due to a Ctrl-C at the wrong time), the ESP8266 will most
likely crash.
*/
#ifndef ATTR_GDBINIT
#define ATTR_GDBINIT		ICACHE_FLASH_ATTR
#endif
#ifndef ATTR_GDBFN
#define ATTR_GDBFN			ICACHE_RAM_ATTR
#endif
#ifndef ATTR_GDBEXTERNFN
#define ATTR_GDBEXTERNFN	ICACHE_FLASH_ATTR
#endif

#ifndef ASATTR_GDBINIT
#define ASATTR_GDBINIT		.section .irom0.text
#endif
#ifndef ASATTR_GDBFN
#define ASATTR_GDBFN		.section .iram.text
#endif
#ifndef ASATTR_GDBEXTERNFN
#define ASATTR_GDBEXTERNFN	.section .irom0.text
#endif


#endif
