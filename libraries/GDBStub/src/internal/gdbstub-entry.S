/******************************************************************************
 * Copyright 2015 Espressif Systems
 *
 * Description: Assembly routines for the gdbstub
 *
 * License: ESPRESSIF MIT License
 *******************************************************************************/


#include "gdbstub-cfg.h"

#include <xtensa/config/specreg.h>
#include <xtensa/config/core-isa.h>
#include <xtensa/corebits.h>

#define DEBUG_PC	(EPC + XCHAL_DEBUGLEVEL)
#define DEBUG_EXCSAVE	(EXCSAVE + XCHAL_DEBUGLEVEL)
#define DEBUG_PS	(EPS + XCHAL_DEBUGLEVEL)


.global gdbstub_savedRegs

#if GDBSTUB_USE_OWN_STACK
.global gdbstub_exceptionStack
#endif

	ASATTR_GDBFN
.literal_position

	ASATTR_GDBINIT
.literal_position

	ASATTR_GDBFN
	.align	4

/*
The savedRegs struct:
	uint32_t pc;
	uint32_t ps;
	uint32_t sar;
	uint32_t vpri;
	uint32_t a0;
	uint32_t a[14]; //a2..a15
	uint32_t litbase;
	uint32_t sr176;
	uint32_t sr208;
	uint32_t a1;
	uint32_t reason;
*/

/*
This is the debugging exception routine; it's called by the debugging vector

We arrive here with all regs intact except for a2. The old contents of A2 are saved
into the DEBUG_EXCSAVE special function register. EPC is the original PC.
*/
	.type gdbstub_debug_exception_entry, @function
gdbstub_debug_exception_entry:
/*
	//Minimum no-op debug exception handler, for debug
	rsr a2,DEBUG_PC
	addi a2,a2,3
	wsr a2,DEBUG_PC
	xsr	a2, DEBUG_EXCSAVE
	rfi	XCHAL_DEBUGLEVEL
*/

//Save all regs to structure
	movi	a2, gdbstub_savedRegs
	s32i	a0, a2, 0x10
	s32i	a1, a2, 0x14
	rsr		a0, DEBUG_PS
	s32i	a0, a2, 0x04
	rsr		a0, DEBUG_EXCSAVE //was R2
	s32i	a0, a2, 0x18
	s32i	a3, a2, 0x1c
	s32i	a4, a2, 0x20
	s32i	a5, a2, 0x24
	s32i	a6, a2, 0x28
	s32i	a7, a2, 0x2c
	s32i	a8, a2, 0x30
	s32i	a9, a2, 0x34
	s32i	a10, a2, 0x38
	s32i	a11, a2, 0x3c
	s32i	a12, a2, 0x40
	s32i	a13, a2, 0x44
	s32i	a14, a2, 0x48
	s32i	a15, a2, 0x4c
	rsr		a0, SAR
	s32i	a0, a2, 0x08
	rsr		a0, LITBASE
	s32i	a0, a2, 0x50
	rsr		a0, 176
	s32i	a0, a2, 0x54
	rsr		a0, 208
	s32i	a0, a2, 0x58
	rsr		a0, DEBUGCAUSE
	s32i	a0, a2, 0x5C
	rsr		a4, DEBUG_PC
	s32i	a4, a2, 0x00

#if GDBSTUB_USE_OWN_STACK
	//Move to our own stack
	movi a1, exceptionStack+255*4
#endif

//If ICOUNT is -1, disable it by setting it to 0, otherwise we will keep triggering on the same instruction.
	rsr		a2, ICOUNT
	movi	a3, -1
	bne		a2, a3, noIcountReset
	movi	a3, 0
	wsr		a3, ICOUNT
noIcountReset:

	rsr	a2, ps
	addi	a2, a2, -PS_EXCM_MASK
	wsr	a2, ps
	rsync

//Call into the C code to do the actual handling.
	call0	gdbstub_handle_debug_exception

DebugExceptionExit:

	rsr	a2, ps
	addi	a2, a2, PS_EXCM_MASK
	wsr	a2, ps
	rsync

	//Restore registers from the gdbstub_savedRegs struct
	movi	a2, gdbstub_savedRegs
	l32i	a0, a2, 0x00
	wsr		a0, DEBUG_PC
//	l32i	a0, a2, 0x58
//	wsr		a0, 208
	l32i	a0, a2, 0x54
	//wsr		a0, 176		//Some versions of gcc do not understand this...
	.byte  0x00, 176, 0x13	//so we hand-assemble the instruction.
	l32i	a0, a2, 0x50
	wsr		a0, LITBASE
	l32i	a0, a2, 0x08
	wsr		a0, SAR
	l32i	a15, a2, 0x4c
	l32i	a14, a2, 0x48
	l32i	a13, a2, 0x44
	l32i	a12, a2, 0x40
	l32i	a11, a2, 0x3c
	l32i	a10, a2, 0x38
	l32i	a9, a2, 0x34
	l32i	a8, a2, 0x30
	l32i	a7, a2, 0x2c
	l32i	a6, a2, 0x28
	l32i	a5, a2, 0x24
	l32i	a4, a2, 0x20
	l32i	a3, a2, 0x1c
	l32i	a0, a2, 0x18
	wsr		a0, DEBUG_EXCSAVE //was R2
	l32i	a0, a2, 0x04
	wsr		a0, DEBUG_PS
	l32i	a1, a2, 0x14
	l32i	a0, a2, 0x10

	//Read back vector-saved a2 value, put back address of this routine.
	movi	a2, gdbstub_debug_exception_entry
	xsr	a2, DEBUG_EXCSAVE

	//All done. Return to where we came from.
	rfi	XCHAL_DEBUGLEVEL
	.size gdbstub_debug_exception_entry, .-gdbstub_debug_exception_entry


#if GDBSTUB_BREAK_ON_EXCEPTION

#if GDBSTUB_FREERTOS
/*
FreeRTOS exception handling code. For some reason or another, we can't just hook the main exception vector: it
seems FreeRTOS uses that for something else too (interrupts). FreeRTOS has its own fatal exception handler, and we
hook that. Unfortunately, that one is called from a few different places (eg directly in the DoubleExceptionVector)
so the precise location of the original register values are somewhat of a mystery when we arrive here...

As a 'solution', we'll just decode the most common case of the user_fatal_exception_handler being called from
the user exception handler vector:
- excsave1 - orig a0
- a1: stack frame:
	sf+16: orig a1
	sf+8: ps
	sf+4: epc
	sf+12: orig a0
	sf: magic no?
*/
	.global gdbstub_handle_user_exception
	.global gdbstub_user_exception_entry
	.type gdbstub_user_exception_entry, @function
	ASATTR_GDBFN
	.align	4
gdbstub_user_exception_entry:
//Save all regs to structure
	movi	a0, gdbstub_savedRegs
	s32i	a1, a0, 0x18 //was a2
	s32i	a3, a0, 0x1c
	s32i	a4, a0, 0x20
	s32i	a5, a0, 0x24
	s32i	a6, a0, 0x28
	s32i	a7, a0, 0x2c
	s32i	a8, a0, 0x30
	s32i	a9, a0, 0x34
	s32i	a10, a0, 0x38
	s32i	a11, a0, 0x3c
	s32i	a12, a0, 0x40
	s32i	a13, a0, 0x44
	s32i	a14, a0, 0x48
	s32i	a15, a0, 0x4c
	rsr		a2, SAR
	s32i	a2, a0, 0x08
	rsr		a2, LITBASE
	s32i	a2, a0, 0x50
	rsr		a2, 176
	s32i	a2, a0, 0x54
	rsr		a2, 208
	s32i	a2, a0, 0x58
	rsr		a2, EXCCAUSE
	s32i	a2, a0, 0x5C

//Get the rest of the regs from the stack struct
	l32i	a3, a1, 12
	s32i	a3, a0, 0x10
	l32i	a3, a1, 16
	s32i	a3, a0, 0x58
	l32i	a3, a1, 8
	s32i	a3, a0, 0x04
	l32i	a3, a1, 4
	s32i	a3, a0, 0x00

#if GDBSTUB_USE_OWN_STACK
	movi a1, exceptionStack+255*4
#endif

	rsr	a2, ps
	addi	a2, a2, -PS_EXCM_MASK
	wsr	a2, ps
	rsync

	call0	gdbstub_handle_user_exception

UserExceptionExit:

/*
Okay, from here on, it Does Not Work. There's not really any continuing from an exception in the
FreeRTOS case; there isn't any effort put in reversing the mess the exception code made yet. Maybe this
is still something we need to implement later, if there's any demand for it, or maybe we should modify
FreeRTOS to allow this in the future. (Which will then kill backwards compatibility... hmmm.)
*/
	j UserExceptionExit
	.size gdbstub_user_exception_entry, .-gdbstub_user_exception_entry


	.global gdbstub_handle_uart_int
	.global gdbstub_uart_entry
	.type gdbstub_uart_entry, @function
	ASATTR_GDBFN
	.align	4
gdbstub_uart_entry:
	//On entry, the stack frame is at SP+16.
	//This is a small stub to present that as the first arg to the gdbstub_handle_uart function.
	movi	a2, 16
	add		a2, a2, a1
	movi	a3, gdbstub_handle_uart_int
	jx		a3
	.size gdbstub_uart_entry, .-gdbstub_uart_entry

#endif

#endif



	.global gdbstub_save_extra_sfrs_for_exception
	.type gdbstub_save_extra_sfrs_for_exception, @function
	ASATTR_GDBFN
	.align 4
//The Xtensa OS HAL does not save all the special function register things. This bit of assembly
//fills the gdbstub_savedRegs struct with them.
gdbstub_save_extra_sfrs_for_exception:
	movi	a2, gdbstub_savedRegs
	rsr		a3, LITBASE
	s32i	a3, a2, 0x50
	rsr		a3, 176
	s32i	a3, a2, 0x54
	rsr		a3, 208
	s32i	a3, a2, 0x58
	rsr		a3, EXCCAUSE
	s32i	a3, a2, 0x5C
	ret
	.size gdbstub_save_extra_sfrs_for_exception, .-gdbstub_save_extra_sfrs_for_exception

	.global gdbstub_init_debug_entry
	.global _DebugExceptionVector
	.type gdbstub_init_debug_entry, @function
	ASATTR_GDBINIT
	.align	4
gdbstub_init_debug_entry:
//This puts the following 2 instructions into the debug exception vector:
//	xsr	a2, DEBUG_EXCSAVE
//	jx	a2
	movi	a2, _DebugExceptionVector
	movi	a3, 0xa061d220
	s32i	a3, a2, 0
	movi	a3, 0x00000002
	s32i	a3, a2, 4

//Tell the just-installed debug vector where to go.
	movi	a2, gdbstub_debug_exception_entry
	wsr		a2, DEBUG_EXCSAVE

	ret
	.size gdbstub_init_debug_entry, .-gdbstub_init_debug_entry


//Set up ICOUNT register to step one single instruction
	.global gdbstub_icount_ena_single_step
	.type gdbstub_icount_ena_single_step, @function
	ASATTR_GDBFN
	.align 4
gdbstub_icount_ena_single_step:
	movi	a3, XCHAL_DEBUGLEVEL //Only count steps in non-debug mode
	movi	a2, -2
	wsr		a3, ICOUNTLEVEL
	wsr		a2, ICOUNT
	isync
	ret
	.size gdbstub_icount_ena_single_step, .-gdbstub_icount_ena_single_step


//These routines all assume only one breakpoint and watchpoint is available, which
//is the case for the ESP8266 Xtensa core.


	.global gdbstub_set_hw_breakpoint
	.type gdbstub_set_hw_breakpoint, @function
	ASATTR_GDBFN
gdbstub_set_hw_breakpoint:
	//a2 - addr, a3 - len (unused here)
	rsr		a4, IBREAKENABLE
	bbsi	a4, 0, return_w_error
	wsr		a2, IBREAKA
	movi	a2, 1
	wsr		a2, IBREAKENABLE
	isync
	movi 	a2, 1
	ret
	.size gdbstub_set_hw_breakpoint, .-gdbstub_set_hw_breakpoint

	.global gdbstub_del_hw_breakpoint
	.type gdbstub_del_hw_breakpoint, @function
	ASATTR_GDBFN
gdbstub_del_hw_breakpoint:
	//a2 - addr
	rsr		a5, IBREAKENABLE
	bbci	a5, 0, return_w_error
	rsr		a3, IBREAKA
	bne		a3, a2, return_w_error
	movi	a2,0
	wsr		a2, IBREAKENABLE
	isync
	movi	a2, 1
	ret
	.size gdbstub_del_hw_breakpoint, .-gdbstub_del_hw_breakpoint

	.global gdbstub_set_hw_watchpoint
	.type gdbstub_set_hw_watchpoint, @function
	ASATTR_GDBFN
	//a2 - addr, a3 - mask, a4 - type (1=read, 2=write, 3=access)
gdbstub_set_hw_watchpoint:
	//Check if any of the masked address bits are set. If so, that is an error.
	movi	a5,0x0000003F
	xor		a5, a5, a3
	bany	a2, a5, return_w_error
	//Check if watchpoint already is set
	rsr		a5, DBREAKC
	movi	a6, 0xC0000000
	bany	a6, a5, return_w_error
	//Set watchpoint
	wsr		a2, DBREAKA

	//Combine type and mask
	movi	a6, 0x3F
	and		a3, a3, a6
	slli	a4, a4, 30
	or		a3, a3, a4
	wsr		a3, DBREAKC

//	movi	a2, 1
	mov		a2, a3
	isync
	ret
	.size gdbstub_set_hw_watchpoint, .-gdbstub_set_hw_watchpoint


	.global gdbstub_del_hw_watchpoint
	.type gdbstub_del_hw_watchpoint, @function
	ASATTR_GDBFN
	//a2 - addr
gdbstub_del_hw_watchpoint:
	//See if the address matches
	rsr		a3, DBREAKA
	bne		a3, a2, return_w_error
	//See if the bp actually is set
	rsr		a3, DBREAKC
	movi	a2, 0xC0000000
	bnone	a3, a2, return_w_error
	//Disable bp
	movi	a2,0
	wsr		a2,DBREAKC
	movi	a2,1
	isync
	ret

return_w_error:
	movi	a2, 0
	ret
	.size gdbstub_del_hw_watchpoint, .-gdbstub_del_hw_watchpoint


//Breakpoint, with an attempt at a functional function prologue and epilogue...
	.global gdbstub_do_break_breakpoint_addr
	.global gdbstub_do_break
	.type gdbstub_do_break, @function
	ASATTR_GDBFN
	.align	4
gdbstub_do_break:
	addi	a1, a1, -16
	s32i	a15, a1, 12
	mov		a15, a1

gdbstub_do_break_breakpoint_addr:
	break 0,0

	mov		a1, a15
	l32i	a15, a1, 12
	addi	a1, a1, 16
	ret
	.size gdbstub_do_break, .-gdbstub_do_break
