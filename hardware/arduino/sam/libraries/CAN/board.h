/**
 * \file
 *
 * \brief Standard board header file.
 *
 * This file includes the appropriate board header file according to the
 * defined board (parameter BOARD).
 *
 * Copyright (c) 2009-2012 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an
 *    Atmel microcontroller product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

#ifndef _BOARD_H_
#define _BOARD_H_

/**
 * \defgroup group_common_boards Generic board support
 *
 * The generic board support module includes board-specific definitions
 * and function prototypes, such as the board initialization function.
 *
 * \{
 */

#include "compiler.h"

#ifdef __cplusplus
extern "C" {
#endif


/*! \name Base Boards
 */
//! @{
#define EVK1100               1   //!< AT32UC3A EVK1100 board.
#define EVK1101               2   //!< AT32UC3B EVK1101 board.
#define UC3C_EK               3   //!< AT32UC3C UC3C_EK board.
#define EVK1104               4   //!< AT32UC3A3 EVK1104 board.
#define EVK1105               5   //!< AT32UC3A EVK1105 board.
#define STK600_RCUC3L0        6   //!< STK600 RCUC3L0 board.
#define UC3L_EK               7   //!< AT32UC3L-EK board.
#define XPLAIN                8   //!< ATxmega128A1 Xplain board.
#define STK600_RC064X         10  //!< ATxmega256A3 STK600 board.
#define STK600_RC100X         11  //!< ATxmega128A1 STK600 board.
#define UC3_A3_XPLAINED       13  //!< ATUC3A3 UC3-A3 Xplained board.
#define UC3_L0_XPLAINED       15  //!< ATUC3L0 UC3-L0 Xplained board.
#define STK600_RCUC3D         16  //!< STK600 RCUC3D board.
#define STK600_RCUC3C0        17  //!< STK600 RCUC3C board.
#define XMEGA_B1_XPLAINED     18  //!< ATxmega128B1 Xplained board.
#define XMEGA_A1_XPLAINED     19  //!< ATxmega128A1 Xplain-A1 board.
#define STK600_RCUC3L4        21  //!< ATUCL4 STK600 board
#define UC3_L0_XPLAINED_BC    22  //!< ATUC3L0 UC3-L0 Xplained board controller board
#define MEGA1284P_XPLAINED_BC 23  //!< ATmega1284P-Xplained board controller board
#define STK600_RC044X         24  //!< STK600 with RC044X routing card board.
#define STK600_RCUC3B0        25  //!< STK600 RCUC3B0 board.
#define UC3_L0_QT600          26  //!< QT600 UC3L0 MCU board.
#define XMEGA_A3BU_XPLAINED   27  //!< ATxmega256A3BU Xplained board.
#define STK600_RC064X_LCDX    28  //!< XMEGAB3 STK600 RC064X LCDX board.
#define STK600_RC100X_LCDX    29  //!< XMEGAB1 STK600 RC100X LCDX board.
#define UC3B_BOARD_CONTROLLER 30  //!< AT32UC3B1 board controller for Atmel boards
#define RZ600                 31  //!< AT32UC3A RZ600 MCU board
#define SAM3S_EK              32  //!< SAM3S-EK board.
#define SAM3U_EK              33  //!< SAM3U-EK board.
#define SAM3X_EK              34  //!< SAM3X-EK board.
#define SAM3N_EK              35  //!< SAM3N-EK board.
#define SAM3S_EK2             36  //!< SAM3S-EK2 board.
#define SAM4S_EK              37  //!< SAM4S-EK board.
#define STK600_RCUC3A0        38  //!< STK600 RCUC3A0 board.
#define STK600_MEGA           39  //!< STK600 MEGA board.
#define MEGA_1284P_XPLAINED   40  //!< ATmega1284P Xplained board.
#define SAM4S_XPLAINED        41  //!< SAM4S Xplained board.
#define ATXMEGA128A1_QT600    42  //!< QT600 ATXMEGA128A1 MCU board.
#define ARDUINO_DUE_X         43  //!< Arduino Due/X board.
#define STK600_RCUC3L3        44  //!< ATUCL3 STK600 board
#define SAM4L_EK              45  //!< SAM4L-EK board.
#define STK600_MEGA_RF        46  //!< STK600 MEGA RF EVK board.
#define XMEGA_C3_XPLAINED     47  //!< ATxmega384C3 Xplained board.
#define STK600_RC032X         48  //!< STK600 with RC032X routing card board.
#define SAM4S_EK2             49  //!< SAM4S-EK2 board.
#define SIMULATOR_XMEGA_A1    97  //!< Simulator for XMEGA A1 devices
#define AVR_SIMULATOR_UC3     98  //!< AVR SIMULATOR for AVR UC3 device family.
#define USER_BOARD            99  //!< User-reserved board (if any).
#define DUMMY_BOARD          100  //!< Dummy board to support board-independent applications (e.g. bootloader)
//! @}

/*! \name Extension Boards
 */
//! @{
#define EXT1102                      1  //!< AT32UC3B EXT1102 board
#define MC300                        2  //!< AT32UC3 MC300 board
#define SENSORS_XPLAINED_INERTIAL_1  3  //!< Xplained inertial sensor board 1
#define SENSORS_XPLAINED_INERTIAL_2  4  //!< Xplained inertial sensor board 2
#define SENSORS_XPLAINED_PRESSURE_1  5  //!< Xplained pressure sensor board
#define SENSORS_XPLAINED_LIGHTPROX_1 6  //!< Xplained light & proximity sensor board
#define SENSORS_XPLAINED_INERTIAL_A1 7  //!< Xplained inertial sensor board "A"
#define RZ600_AT86RF231              8  //!< AT86RF231 RF board in RZ600
#define RZ600_AT86RF230B             9  //!< AT86RF231 RF board in RZ600
#define RZ600_AT86RF212             10  //!< AT86RF231 RF board in RZ600
#define SENSORS_XPLAINED_BREADBOARD 11  //!< Xplained sensor development breadboard
#define SECURITY_XPLAINED           12  //!< Xplained ATSHA204 board
#define USER_EXT_BOARD              99  //!< User-reserved extension board (if any).
//! @}

#  include "arduino_due_x.h"
//#  include "system_sam3x.h"

#if (defined EXT_BOARD)
#  if EXT_BOARD == MC300
#    include "mc300/mc300.h"
#  elif (EXT_BOARD == SENSORS_XPLAINED_INERTIAL_1)  || \
        (EXT_BOARD == SENSORS_XPLAINED_INERTIAL_2)  || \
        (EXT_BOARD == SENSORS_XPLAINED_INERTIAL_A1) || \
        (EXT_BOARD == SENSORS_XPLAINED_PRESSURE_1)  || \
        (EXT_BOARD == SENSORS_XPLAINED_LIGHTPROX_1) || \
        (EXT_BOARD == SENSORS_XPLAINED_BREADBOARD)
#    include "sensors_xplained/sensors_xplained.h"
#  elif EXT_BOARD == RZ600_AT86RF231
#     include "at86rf231/at86rf231.h"
#  elif EXT_BOARD == RZ600_AT86RF230B
#    include "at86rf230b/at86rf230b.h"
#  elif EXT_BOARD == RZ600_AT86RF212
#    include "at86rf212/at86rf212.h"
#  elif EXT_BOARD == SECURITY_XPLAINED
#    include "security_xplained.h"
#  elif EXT_BOARD == USER_EXT_BOARD
    // User-reserved area: #include the header file of your extension board here
    // (if any).
#  endif
#endif


#if (defined(__GNUC__) && defined(__AVR32__)) || (defined(__ICCAVR32__) || defined(__AAVR32__))
#ifdef __AVR32_ABI_COMPILER__ // Automatically defined when compiling for AVR32, not when assembling.

/*! \brief This function initializes the board target resources
 *
 * This function should be called to ensure proper initialization of the target
 * board hardware connected to the part.
 */
extern void board_init(void);

#endif  // #ifdef __AVR32_ABI_COMPILER__
#else
/*! \brief This function initializes the board target resources
 *
 * This function should be called to ensure proper initialization of the target
 * board hardware connected to the part.
 */
extern void board_init(void);
#endif


#ifdef __cplusplus
}
#endif

/**
 * \}
 */

#endif  // _BOARD_H_
