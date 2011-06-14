#ifndef _BOARD_
#define _BOARD_

/*----------------------------------------------------------------------------
 *        Headers
 *----------------------------------------------------------------------------*/

#include "chip.h"

#include "include/bitbanding.h"
#include "include/board_lowlevel.h"
#include "include/timetick.h"

/**
 * Libc porting layers
 */
#if defined (  __GNUC__  ) /* GCC CS3 */
#    include "include/syscalls.h" /** RedHat Newlib minimal stub */
#endif

/*----------------------------------------------------------------------------
 *        Definitions
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/

/** Name of the board */
#define BOARD_NAME "SAM3S-EK"

/*
#define BOARD_REV_A
*/
#define BOARD_REV_B

/** Frequency of the board main oscillator */
#define BOARD_MAINOSC           12000000

/** Master clock frequency (when using board_lowlevel.c) */
#define BOARD_MCK               64000000

#endif /* #ifndef _BOARD_ */

