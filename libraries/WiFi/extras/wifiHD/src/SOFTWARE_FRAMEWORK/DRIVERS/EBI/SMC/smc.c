/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief SMC on EBI driver for AVR32 UC3.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with a SMC module can be used.
 * - AppNote:
 *
 * \author               Atmel Corporation: http://www.atmel.com \n
 *                       Support and FAQ: http://support.atmel.no/
 *
 ******************************************************************************/

/* Copyright (c) 2009 Atmel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * 4. This software may only be redistributed and used in connection with an Atmel
 * AVR product.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE
 *
 */

#include "compiler.h"
#include "preprocessor.h"
#include "gpio.h"
#include "smc.h"

// Configure the SM Controller with SM setup and timing information for all chip select
#define SMC_CS_SETUP(ncs) { \
  U32 nwe_setup    = ((NWE_SETUP    * hsb_mhz_up + 999) / 1000); \
  U32 ncs_wr_setup = ((NCS_WR_SETUP * hsb_mhz_up + 999) / 1000); \
  U32 nrd_setup    = ((NRD_SETUP    * hsb_mhz_up + 999) / 1000); \
  U32 ncs_rd_setup = ((NCS_RD_SETUP * hsb_mhz_up + 999) / 1000); \
  U32 nwe_pulse    = ((NWE_PULSE    * hsb_mhz_up + 999) / 1000); \
  U32 ncs_wr_pulse = ((NCS_WR_PULSE * hsb_mhz_up + 999) / 1000); \
  U32 nrd_pulse    = ((NRD_PULSE    * hsb_mhz_up + 999) / 1000); \
  U32 ncs_rd_pulse = ((NCS_RD_PULSE * hsb_mhz_up + 999) / 1000); \
  U32 nwe_cycle    = ((NWE_CYCLE    * hsb_mhz_up + 999) / 1000); \
  U32 nrd_cycle    = ((NRD_CYCLE    * hsb_mhz_up + 999) / 1000); \
                                                                 \
  /* Some coherence checks...                             */     \
  /* Ensures CS is active during Rd or Wr                 */     \
  if( ncs_rd_setup + ncs_rd_pulse < nrd_setup + nrd_pulse )      \
    ncs_rd_pulse = nrd_setup + nrd_pulse - ncs_rd_setup;         \
  if( ncs_wr_setup + ncs_wr_pulse < nwe_setup + nwe_pulse )      \
    ncs_wr_pulse = nwe_setup + nwe_pulse - ncs_wr_setup;         \
                                                                 \
  /* ncs_hold = n_cycle - ncs_setup - ncs_pulse           */     \
  /* n_hold   = n_cycle - n_setup - n_pulse               */     \
  /*                                                      */     \
  /* All holds parameters must be positive or null, so:   */     \
  /* nwe_cycle shall be >= ncs_wr_setup + ncs_wr_pulse    */     \
  if( nwe_cycle < ncs_wr_setup + ncs_wr_pulse )                  \
    nwe_cycle = ncs_wr_setup + ncs_wr_pulse;                     \
                                                                 \
  /* nwe_cycle shall be >= nwe_setup + nwe_pulse          */     \
  if( nwe_cycle < nwe_setup + nwe_pulse )                        \
    nwe_cycle = nwe_setup + nwe_pulse;                           \
                                                                 \
  /* nrd_cycle shall be >= ncs_rd_setup + ncs_rd_pulse    */     \
  if( nrd_cycle < ncs_rd_setup + ncs_rd_pulse )                  \
    nrd_cycle = ncs_rd_setup + ncs_rd_pulse;                     \
                                                                 \
  /* nrd_cycle shall be >= nrd_setup + nrd_pulse          */     \
  if( nrd_cycle < nrd_setup + nrd_pulse )                        \
    nrd_cycle = nrd_setup + nrd_pulse;                           \
                                                                 \
  AVR32_SMC.cs[ncs].setup = (nwe_setup    << AVR32_SMC_SETUP0_NWE_SETUP_OFFSET) | \
                            (ncs_wr_setup << AVR32_SMC_SETUP0_NCS_WR_SETUP_OFFSET) | \
                            (nrd_setup    << AVR32_SMC_SETUP0_NRD_SETUP_OFFSET) | \
                            (ncs_rd_setup << AVR32_SMC_SETUP0_NCS_RD_SETUP_OFFSET); \
  AVR32_SMC.cs[ncs].pulse = (nwe_pulse    << AVR32_SMC_PULSE0_NWE_PULSE_OFFSET) | \
                            (ncs_wr_pulse << AVR32_SMC_PULSE0_NCS_WR_PULSE_OFFSET) | \
                            (nrd_pulse    << AVR32_SMC_PULSE0_NRD_PULSE_OFFSET) | \
                            (ncs_rd_pulse << AVR32_SMC_PULSE0_NCS_RD_PULSE_OFFSET); \
  AVR32_SMC.cs[ncs].cycle = (nwe_cycle    << AVR32_SMC_CYCLE0_NWE_CYCLE_OFFSET) | \
                            (nrd_cycle    << AVR32_SMC_CYCLE0_NRD_CYCLE_OFFSET); \
  AVR32_SMC.cs[ncs].mode = (((NCS_CONTROLLED_READ) ? AVR32_SMC_MODE0_READ_MODE_NCS_CONTROLLED : \
                           AVR32_SMC_MODE0_READ_MODE_NRD_CONTROLLED) << AVR32_SMC_MODE0_READ_MODE_OFFSET) | \
                       +    (((NCS_CONTROLLED_WRITE) ? AVR32_SMC_MODE0_WRITE_MODE_NCS_CONTROLLED : \
                           AVR32_SMC_MODE0_WRITE_MODE_NWE_CONTROLLED) << AVR32_SMC_MODE0_WRITE_MODE_OFFSET) | \
                           (NWAIT_MODE << AVR32_SMC_MODE0_EXNW_MODE_OFFSET) | \
                           (((SMC_8_BIT_CHIPS) ? AVR32_SMC_MODE0_BAT_BYTE_WRITE : \
                           AVR32_SMC_MODE0_BAT_BYTE_SELECT) << AVR32_SMC_MODE0_BAT_OFFSET) | \
                           (((SMC_DBW <= 8 ) ? AVR32_SMC_MODE0_DBW_8_BITS  : \
                           (SMC_DBW <= 16) ? AVR32_SMC_MODE0_DBW_16_BITS : \
                           AVR32_SMC_MODE0_DBW_32_BITS) << AVR32_SMC_MODE0_DBW_OFFSET) | \
                           (TDF_CYCLES << AVR32_SMC_MODE0_TDF_CYCLES_OFFSET) | \
                           (TDF_OPTIM << AVR32_SMC_MODE0_TDF_MODE_OFFSET) | \
                           (PAGE_MODE << AVR32_SMC_MODE0_PMEN_OFFSET) | \
                           (PAGE_SIZE << AVR32_SMC_MODE0_PS_OFFSET); \
  smc_tab_cs_size[ncs] = (U8)EXT_SM_SIZE; \
  }

static U8 smc_tab_cs_size[6];

static void smc_enable_muxed_pins(void);


void smc_init(unsigned long hsb_hz)
{
  unsigned long hsb_mhz_up = (hsb_hz + 999999) / 1000000;

//! Whether to use the NCS0 pin
#ifdef SMC_USE_NCS0
  #include SMC_COMPONENT_CS0

  // Setup SMC for NCS0
  SMC_CS_SETUP(0)

  #ifdef SMC_DBW_GLOBAL
    #if (SMC_DBW_GLOBAL < SMC_DBW)
        #undef  SMC_DBW_GLOBAL
        #if     (SMC_DBW == 8)
            #define SMC_DBW_GLOBAL                  8
        #elif   (SMC_DBW == 16)
            #define SMC_DBW_GLOBAL                  16
        #elif   (SMC_DBW == 32)
            #define SMC_DBW_GLOBAL                  32
        #else
          #error  error in SMC_DBW size
        #endif
    #endif
  #else
        #if     (SMC_DBW == 8)
            #define SMC_DBW_GLOBAL                  8
        #elif   (SMC_DBW == 16)
            #define SMC_DBW_GLOBAL                  16
        #elif   (SMC_DBW == 32)
            #define SMC_DBW_GLOBAL                  32
        #else
          #error  error in SMC_DBW size
        #endif
  #endif

  #ifdef SMC_8_BIT_CHIPS_GLOBAL
    #if (SMC_8_BIT_CHIPS_GLOBAL < SMC_8_BIT)
        #undef  SMC_8_BIT_CHIPS_GLOBAL
        #if     (SMC_8_BIT_CHIPS == TRUE)
          #define SMC_8_BIT_CHIPS_GLOBAL            TRUE
        #elif   (SMC_8_BIT_CHIPS == FALSE)
          #define SMC_8_BIT_CHIPS_GLOBAL            FALSE
        #else
          #error  error in SMC_8_BIT_CHIPS size
        #endif
    #endif
  #else
      #if     (SMC_8_BIT_CHIPS == TRUE)
        #define SMC_8_BIT_CHIPS_GLOBAL            TRUE
      #elif   (SMC_8_BIT_CHIPS == FALSE)
        #define SMC_8_BIT_CHIPS_GLOBAL            FALSE
      #else
        #error  error in SMC_8_BIT_CHIPS size
      #endif
  #endif

  #ifdef NWAIT_MODE_GLOBAL
    #if (NWAIT_MODE_GLOBAL < NWAIT_MODE)
        #undef  NWAIT_MODE_GLOBAL
        #if     (NWAIT_MODE == AVR32_SMC_EXNW_MODE_DISABLED)
          #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_DISABLED
        #elif   (NWAIT_MODE == AVR32_SMC_EXNW_MODE_FROZEN)
          #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_FROZEN
        #else
          #error  error in NWAIT_MODE size
        #endif
    #endif
  #else
      #if     (NWAIT_MODE == AVR32_SMC_EXNW_MODE_DISABLED)
        #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_DISABLED
      #elif   (NWAIT_MODE == AVR32_SMC_EXNW_MODE_FROZEN)
        #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_FROZEN
      #else
        #error  error in NWAIT_MODE size
      #endif
  #endif

  #undef EXT_SM_SIZE
  #undef SMC_DBW
  #undef SMC_8_BIT_CHIPS
  #undef NWE_SETUP
  #undef NCS_WR_SETUP
  #undef NRD_SETUP
  #undef NCS_RD_SETUP
  #undef NCS_WR_PULSE
  #undef NWE_PULSE
  #undef NCS_RD_PULSE
  #undef NRD_PULSE
  #undef NCS_WR_HOLD
  #undef NWE_HOLD
  #undef NWE_CYCLE
  #undef NCS_RD_HOLD
  #undef NRD_CYCLE
  #undef TDF_CYCLES
  #undef TDF_OPTIM
  #undef PAGE_MODE
  #undef PAGE_SIZE
  #undef NCS_CONTROLLED_READ
  #undef NCS_CONTROLLED_WRITE
  #undef NWAIT_MODE
#endif


//! Whether to use the NCS1 pin
#ifdef SMC_USE_NCS1
  #include SMC_COMPONENT_CS1

  // Enable SM mode for CS1 if necessary.
  AVR32_HMATRIX.sfr[AVR32_EBI_HMATRIX_NR] &= ~(1 << AVR32_EBI_SDRAM_CS);
  AVR32_HMATRIX.sfr[AVR32_EBI_HMATRIX_NR];

  // Setup SMC for NCS1
  SMC_CS_SETUP(1)

  #ifdef SMC_DBW_GLOBAL
    #if (SMC_DBW_GLOBAL < SMC_DBW)
        #undef  SMC_DBW_GLOBAL
        #if     (SMC_DBW == 8)
            #define SMC_DBW_GLOBAL                  8
        #elif   (SMC_DBW == 16)
            #define SMC_DBW_GLOBAL                  16
        #elif   (SMC_DBW == 32)
            #define SMC_DBW_GLOBAL                  32
        #else
          #error  error in SMC_DBW size
        #endif
    #endif
  #else
        #if     (SMC_DBW == 8)
            #define SMC_DBW_GLOBAL                  8
        #elif   (SMC_DBW == 16)
            #define SMC_DBW_GLOBAL                  16
        #elif   (SMC_DBW == 32)
            #define SMC_DBW_GLOBAL                  32
        #else
          #error  error in SMC_DBW size
        #endif
  #endif

  #ifdef SMC_8_BIT_CHIPS_GLOBAL
    #if (SMC_8_BIT_CHIPS_GLOBAL < SMC_8_BIT)
        #undef  SMC_8_BIT_CHIPS_GLOBAL
        #if     (SMC_8_BIT_CHIPS == TRUE)
          #define SMC_8_BIT_CHIPS_GLOBAL            TRUE
        #elif   (SMC_8_BIT_CHIPS == FALSE)
          #define SMC_8_BIT_CHIPS_GLOBAL            FALSE
        #else
          #error  error in SMC_8_BIT_CHIPS size
        #endif
    #endif
  #else
      #if     (SMC_8_BIT_CHIPS == TRUE)
        #define SMC_8_BIT_CHIPS_GLOBAL            TRUE
      #elif   (SMC_8_BIT_CHIPS == FALSE)
        #define SMC_8_BIT_CHIPS_GLOBAL            FALSE
      #else
        #error  error in SMC_8_BIT_CHIPS size
      #endif
  #endif

  #ifdef NWAIT_MODE_GLOBAL
    #if (NWAIT_MODE_GLOBAL < NWAIT_MODE)
        #undef  NWAIT_MODE_GLOBAL
        #if     (NWAIT_MODE == AVR32_SMC_EXNW_MODE_DISABLED)
          #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_DISABLED
        #elif   (NWAIT_MODE == AVR32_SMC_EXNW_MODE_FROZEN)
          #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_FROZEN
        #else
          #error  error in NWAIT_MODE size
        #endif
    #endif
  #else
      #if     (NWAIT_MODE == AVR32_SMC_EXNW_MODE_DISABLED)
        #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_DISABLED
      #elif   (NWAIT_MODE == AVR32_SMC_EXNW_MODE_FROZEN)
        #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_FROZEN
      #else
        #error  error in NWAIT_MODE size
      #endif
  #endif

  #undef EXT_SM_SIZE
  #undef SMC_DBW
  #undef SMC_8_BIT_CHIPS
  #undef NWE_SETUP
  #undef NCS_WR_SETUP
  #undef NRD_SETUP
  #undef NCS_RD_SETUP
  #undef NCS_WR_PULSE
  #undef NWE_PULSE
  #undef NCS_RD_PULSE
  #undef NRD_PULSE
  #undef NCS_WR_HOLD
  #undef NWE_HOLD
  #undef NWE_CYCLE
  #undef NCS_RD_HOLD
  #undef NRD_CYCLE
  #undef TDF_CYCLES
  #undef TDF_OPTIM
  #undef PAGE_MODE
  #undef PAGE_SIZE
  #undef NCS_CONTROLLED_READ
  #undef NCS_CONTROLLED_WRITE
  #undef NWAIT_MODE
#endif

//! Whether to use the NCS2 pin
#ifdef SMC_USE_NCS2
  #include SMC_COMPONENT_CS2

  // Setup SMC for NCS2
  SMC_CS_SETUP(2)

  #ifdef SMC_DBW_GLOBAL
    #if (SMC_DBW_GLOBAL < SMC_DBW)
        #undef  SMC_DBW_GLOBAL
        #if     (SMC_DBW == 8)
            #define SMC_DBW_GLOBAL                  8
        #elif   (SMC_DBW == 16)
            #define SMC_DBW_GLOBAL                  16
        #elif   (SMC_DBW == 32)
            #define SMC_DBW_GLOBAL                  32
        #else
          #error  error in SMC_DBW size
        #endif
    #endif
  #else
        #if     (SMC_DBW == 8)
            #define SMC_DBW_GLOBAL                  8
        #elif   (SMC_DBW == 16)
            #define SMC_DBW_GLOBAL                  16
        #elif   (SMC_DBW == 32)
            #define SMC_DBW_GLOBAL                  32
        #else
          #error  error in SMC_DBW size
        #endif
  #endif

  #ifdef SMC_8_BIT_CHIPS_GLOBAL
    #if (SMC_8_BIT_CHIPS_GLOBAL < SMC_8_BIT)
        #undef  SMC_8_BIT_CHIPS_GLOBAL
        #if     (SMC_8_BIT_CHIPS == TRUE)
          #define SMC_8_BIT_CHIPS_GLOBAL            TRUE
        #elif   (SMC_8_BIT_CHIPS == FALSE)
          #define SMC_8_BIT_CHIPS_GLOBAL            FALSE
        #else
          #error  error in SMC_8_BIT_CHIPS size
        #endif
    #endif
  #else
      #if     (SMC_8_BIT_CHIPS == TRUE)
        #define SMC_8_BIT_CHIPS_GLOBAL            TRUE
      #elif   (SMC_8_BIT_CHIPS == FALSE)
        #define SMC_8_BIT_CHIPS_GLOBAL            FALSE
      #else
        #error  error in SMC_8_BIT_CHIPS size
      #endif
  #endif

  #ifdef NWAIT_MODE_GLOBAL
    #if (NWAIT_MODE_GLOBAL < NWAIT_MODE)
        #undef  NWAIT_MODE_GLOBAL
        #if     (NWAIT_MODE == AVR32_SMC_EXNW_MODE_DISABLED)
          #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_DISABLED
        #elif   (NWAIT_MODE == AVR32_SMC_EXNW_MODE_FROZEN)
          #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_FROZEN
        #else
          #error  error in NWAIT_MODE size
        #endif
    #endif
  #else
      #if     (NWAIT_MODE == AVR32_SMC_EXNW_MODE_DISABLED)
        #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_DISABLED
      #elif   (NWAIT_MODE == AVR32_SMC_EXNW_MODE_FROZEN)
        #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_FROZEN
      #else
        #error  error in NWAIT_MODE size
      #endif
  #endif


  #undef EXT_SM_SIZE
  #undef SMC_DBW
  #undef SMC_8_BIT_CHIPS
  #undef NWE_SETUP
  #undef NCS_WR_SETUP
  #undef NRD_SETUP
  #undef NCS_RD_SETUP
  #undef NCS_WR_PULSE
  #undef NWE_PULSE
  #undef NCS_RD_PULSE
  #undef NRD_PULSE
  #undef NCS_WR_HOLD
  #undef NWE_HOLD
  #undef NWE_CYCLE
  #undef NCS_RD_HOLD
  #undef NRD_CYCLE
  #undef TDF_CYCLES
  #undef TDF_OPTIM
  #undef PAGE_MODE
  #undef PAGE_SIZE
  #undef NCS_CONTROLLED_READ
  #undef NCS_CONTROLLED_WRITE
  #undef NWAIT_MODE
#endif

//! Whether to use the NCS3 pin
#ifdef SMC_USE_NCS3
  #include SMC_COMPONENT_CS3

  // Setup SMC for NCS3
  SMC_CS_SETUP(3)

  #ifdef SMC_DBW_GLOBAL
    #if (SMC_DBW_GLOBAL < SMC_DBW)
        #undef  SMC_DBW_GLOBAL
        #if     (SMC_DBW == 8)
            #define SMC_DBW_GLOBAL                  8
        #elif   (SMC_DBW == 16)
            #define SMC_DBW_GLOBAL                  16
        #elif   (SMC_DBW == 32)
            #define SMC_DBW_GLOBAL                  32
        #else
          #error  error in SMC_DBW size
        #endif
    #endif
  #else
        #if     (SMC_DBW == 8)
            #define SMC_DBW_GLOBAL                  8
        #elif   (SMC_DBW == 16)
            #define SMC_DBW_GLOBAL                  16
        #elif   (SMC_DBW == 32)
            #define SMC_DBW_GLOBAL                  32
        #else
          #error  error in SMC_DBW size
        #endif
  #endif

  #ifdef SMC_8_BIT_CHIPS_GLOBAL
    #if (SMC_8_BIT_CHIPS_GLOBAL < SMC_8_BIT)
        #undef  SMC_8_BIT_CHIPS_GLOBAL
        #if     (SMC_8_BIT_CHIPS == TRUE)
          #define SMC_8_BIT_CHIPS_GLOBAL            TRUE
        #elif   (SMC_8_BIT_CHIPS == FALSE)
          #define SMC_8_BIT_CHIPS_GLOBAL            FALSE
        #else
          #error  error in SMC_8_BIT_CHIPS size
        #endif
    #endif
  #else
      #if     (SMC_8_BIT_CHIPS == TRUE)
        #define SMC_8_BIT_CHIPS_GLOBAL            TRUE
      #elif   (SMC_8_BIT_CHIPS == FALSE)
        #define SMC_8_BIT_CHIPS_GLOBAL            FALSE
      #else
        #error  error in SMC_8_BIT_CHIPS size
      #endif
  #endif

  #ifdef NWAIT_MODE_GLOBAL
    #if (NWAIT_MODE_GLOBAL < NWAIT_MODE)
        #undef  NWAIT_MODE_GLOBAL
        #if     (NWAIT_MODE == AVR32_SMC_EXNW_MODE_DISABLED)
          #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_DISABLED
        #elif   (NWAIT_MODE == AVR32_SMC_EXNW_MODE_FROZEN)
          #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_FROZEN
        #else
          #error  error in NWAIT_MODE size
        #endif
    #endif
  #else
      #if     (NWAIT_MODE == AVR32_SMC_EXNW_MODE_DISABLED)
        #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_DISABLED
      #elif   (NWAIT_MODE == AVR32_SMC_EXNW_MODE_FROZEN)
        #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_FROZEN
      #else
        #error  error in NWAIT_MODE size
      #endif
  #endif


  #undef EXT_SM_SIZE
  #undef SMC_DBW
  #undef SMC_8_BIT_CHIPS
  #undef NWE_SETUP
  #undef NCS_WR_SETUP
  #undef NRD_SETUP
  #undef NCS_RD_SETUP
  #undef NCS_WR_PULSE
  #undef NWE_PULSE
  #undef NCS_RD_PULSE
  #undef NRD_PULSE
  #undef NCS_WR_HOLD
  #undef NWE_HOLD
  #undef NWE_CYCLE
  #undef NCS_RD_HOLD
  #undef NRD_CYCLE
  #undef TDF_CYCLES
  #undef TDF_OPTIM
  #undef PAGE_MODE
  #undef PAGE_SIZE
  #undef NCS_CONTROLLED_READ
  #undef NCS_CONTROLLED_WRITE
  #undef NWAIT_MODE
#endif

//! Whether to use the NCS4 pin
#ifdef SMC_USE_NCS4
  #include SMC_COMPONENT_CS4

  // Setup SMC for NCS4
  SMC_CS_SETUP(4)

  #ifdef SMC_DBW_GLOBAL
    #if (SMC_DBW_GLOBAL < SMC_DBW)
        #undef  SMC_DBW_GLOBAL
        #if     (SMC_DBW == 8)
            #define SMC_DBW_GLOBAL                  8
        #elif   (SMC_DBW == 16)
            #define SMC_DBW_GLOBAL                  16
        #elif   (SMC_DBW == 32)
            #define SMC_DBW_GLOBAL                  32
        #else
          #error  error in SMC_DBW size
        #endif
    #endif
  #else
        #if     (SMC_DBW == 8)
            #define SMC_DBW_GLOBAL                  8
        #elif   (SMC_DBW == 16)
            #define SMC_DBW_GLOBAL                  16
        #elif   (SMC_DBW == 32)
            #define SMC_DBW_GLOBAL                  32
        #else
          #error  error in SMC_DBW size
        #endif
  #endif

  #ifdef SMC_8_BIT_CHIPS_GLOBAL
    #if (SMC_8_BIT_CHIPS_GLOBAL < SMC_8_BIT)
        #undef  SMC_8_BIT_CHIPS_GLOBAL
        #if     (SMC_8_BIT_CHIPS == TRUE)
          #define SMC_8_BIT_CHIPS_GLOBAL            TRUE
        #elif   (SMC_8_BIT_CHIPS == FALSE)
          #define SMC_8_BIT_CHIPS_GLOBAL            FALSE
        #else
          #error  error in SMC_8_BIT_CHIPS size
        #endif
    #endif
  #else
      #if     (SMC_8_BIT_CHIPS == TRUE)
        #define SMC_8_BIT_CHIPS_GLOBAL            TRUE
      #elif   (SMC_8_BIT_CHIPS == FALSE)
        #define SMC_8_BIT_CHIPS_GLOBAL            FALSE
      #else
        #error  error in SMC_8_BIT_CHIPS size
      #endif
  #endif

  #ifdef NWAIT_MODE_GLOBAL
    #if (NWAIT_MODE_GLOBAL < NWAIT_MODE)
        #undef  NWAIT_MODE_GLOBAL
        #if     (NWAIT_MODE == AVR32_SMC_EXNW_MODE_DISABLED)
          #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_DISABLED
        #elif   (NWAIT_MODE == AVR32_SMC_EXNW_MODE_FROZEN)
          #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_FROZEN
        #else
          #error  error in NWAIT_MODE size
        #endif
    #endif
  #else
      #if     (NWAIT_MODE == AVR32_SMC_EXNW_MODE_DISABLED)
        #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_DISABLED
      #elif   (NWAIT_MODE == AVR32_SMC_EXNW_MODE_FROZEN)
        #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_FROZEN
      #else
        #error  error in NWAIT_MODE size
      #endif
  #endif


  #undef EXT_SM_SIZE
  #undef SMC_DBW
  #undef SMC_8_BIT_CHIPS
  #undef NWE_SETUP
  #undef NCS_WR_SETUP
  #undef NRD_SETUP
  #undef NCS_RD_SETUP
  #undef NCS_WR_PULSE
  #undef NWE_PULSE
  #undef NCS_RD_PULSE
  #undef NRD_PULSE
  #undef NCS_WR_HOLD
  #undef NWE_HOLD
  #undef NWE_CYCLE
  #undef NCS_RD_HOLD
  #undef NRD_CYCLE
  #undef TDF_CYCLES
  #undef TDF_OPTIM
  #undef PAGE_MODE
  #undef PAGE_SIZE
  #undef NCS_CONTROLLED_READ
  #undef NCS_CONTROLLED_WRITE
  #undef NWAIT_MODE
#endif

//! Whether to use the NCS5 pin
#ifdef SMC_USE_NCS5
  #include SMC_COMPONENT_CS5

  // Setup SMC for NCS5
  SMC_CS_SETUP(5)

  #ifdef SMC_DBW_GLOBAL
    #if (SMC_DBW_GLOBAL < SMC_DBW)
        #undef  SMC_DBW_GLOBAL
        #if     (SMC_DBW == 8)
            #define SMC_DBW_GLOBAL                  8
        #elif   (SMC_DBW == 16)
            #define SMC_DBW_GLOBAL                  16
        #elif   (SMC_DBW == 32)
            #define SMC_DBW_GLOBAL                  32
        #else
          #error  error in SMC_DBW size
        #endif
    #endif
  #else
        #if     (SMC_DBW == 8)
            #define SMC_DBW_GLOBAL                  8
        #elif   (SMC_DBW == 16)
            #define SMC_DBW_GLOBAL                  16
        #elif   (SMC_DBW == 32)
            #define SMC_DBW_GLOBAL                  32
        #else
          #error  error in SMC_DBW size
        #endif
  #endif

  #ifdef SMC_8_BIT_CHIPS_GLOBAL
    #if (SMC_8_BIT_CHIPS_GLOBAL < SMC_8_BIT)
        #undef  SMC_8_BIT_CHIPS_GLOBAL
        #if     (SMC_8_BIT_CHIPS == TRUE)
          #define SMC_8_BIT_CHIPS_GLOBAL            TRUE
        #elif   (SMC_8_BIT_CHIPS == FALSE)
          #define SMC_8_BIT_CHIPS_GLOBAL            FALSE
        #else
          #error  error in SMC_8_BIT_CHIPS size
        #endif
    #endif
  #else
      #if     (SMC_8_BIT_CHIPS == TRUE)
        #define SMC_8_BIT_CHIPS_GLOBAL            TRUE
      #elif   (SMC_8_BIT_CHIPS == FALSE)
        #define SMC_8_BIT_CHIPS_GLOBAL            FALSE
      #else
        #error  error in SMC_8_BIT_CHIPS size
      #endif
  #endif

  #ifdef NWAIT_MODE_GLOBAL
    #if (NWAIT_MODE_GLOBAL < NWAIT_MODE)
        #undef  NWAIT_MODE_GLOBAL
        #if     (NWAIT_MODE == AVR32_SMC_EXNW_MODE_DISABLED)
          #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_DISABLED
        #elif   (NWAIT_MODE == AVR32_SMC_EXNW_MODE_FROZEN)
          #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_FROZEN
        #else
          #error  error in NWAIT_MODE size
        #endif
    #endif
  #else
      #if     (NWAIT_MODE == AVR32_SMC_EXNW_MODE_DISABLED)
        #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_DISABLED
      #elif   (NWAIT_MODE == AVR32_SMC_EXNW_MODE_FROZEN)
        #define NWAIT_MODE_GLOBAL            AVR32_SMC_EXNW_MODE_FROZEN
      #else
        #error  error in NWAIT_MODE size
      #endif
  #endif


  #undef EXT_SM_SIZE
  #undef SMC_DBW
  #undef SMC_8_BIT_CHIPS
  #undef NWE_SETUP
  #undef NCS_WR_SETUP
  #undef NRD_SETUP
  #undef NCS_RD_SETUP
  #undef NCS_WR_PULSE
  #undef NWE_PULSE
  #undef NCS_RD_PULSE
  #undef NRD_PULSE
  #undef NCS_WR_HOLD
  #undef NWE_HOLD
  #undef NWE_CYCLE
  #undef NCS_RD_HOLD
  #undef NRD_CYCLE
  #undef TDF_CYCLES
  #undef TDF_OPTIM
  #undef PAGE_MODE
  #undef PAGE_SIZE
  #undef NCS_CONTROLLED_READ
  #undef NCS_CONTROLLED_WRITE
  #undef NWAIT_MODE
#endif
  // Put the multiplexed MCU pins used for the SM under control of the SMC.
  smc_enable_muxed_pins();
}

/*! \brief Puts the multiplexed MCU pins used for the SMC
 *
 */
static void smc_enable_muxed_pins(void)
{
  static const gpio_map_t SMC_EBI_GPIO_MAP =
  {
    // Enable data pins.
#ifdef EBI_DATA_0
    {ATPASTE2(EBI_DATA_0,_PIN),ATPASTE2(EBI_DATA_0,_FUNCTION)},
#endif
#ifdef EBI_DATA_1
    {ATPASTE2(EBI_DATA_1,_PIN),ATPASTE2(EBI_DATA_1,_FUNCTION)},
#endif
#ifdef EBI_DATA_2
    {ATPASTE2(EBI_DATA_2,_PIN),ATPASTE2(EBI_DATA_2,_FUNCTION)},
#endif
#ifdef EBI_DATA_3
    {ATPASTE2(EBI_DATA_3,_PIN),ATPASTE2(EBI_DATA_3,_FUNCTION)},
#endif
#ifdef EBI_DATA_4
    {ATPASTE2(EBI_DATA_4,_PIN),ATPASTE2(EBI_DATA_4,_FUNCTION)},
#endif
#ifdef EBI_DATA_5
    {ATPASTE2(EBI_DATA_5,_PIN),ATPASTE2(EBI_DATA_5,_FUNCTION)},
#endif
#ifdef EBI_DATA_6
    {ATPASTE2(EBI_DATA_6,_PIN),ATPASTE2(EBI_DATA_6,_FUNCTION)},
#endif
#ifdef EBI_DATA_7
    {ATPASTE2(EBI_DATA_7,_PIN),ATPASTE2(EBI_DATA_7,_FUNCTION)},
#endif
#ifdef EBI_DATA_8
    {ATPASTE2(EBI_DATA_8,_PIN),ATPASTE2(EBI_DATA_8,_FUNCTION)},
#endif
#ifdef EBI_DATA_9
    {ATPASTE2(EBI_DATA_9,_PIN),ATPASTE2(EBI_DATA_9,_FUNCTION)},
#endif
#ifdef EBI_DATA_10
    {ATPASTE2(EBI_DATA_10,_PIN),ATPASTE2(EBI_DATA_10,_FUNCTION)},
#endif
#ifdef EBI_DATA_11
    {ATPASTE2(EBI_DATA_11,_PIN),ATPASTE2(EBI_DATA_11,_FUNCTION)},
#endif
#ifdef EBI_DATA_12
    {ATPASTE2(EBI_DATA_12,_PIN),ATPASTE2(EBI_DATA_12,_FUNCTION)},
#endif
#ifdef EBI_DATA_13
    {ATPASTE2(EBI_DATA_13,_PIN),ATPASTE2(EBI_DATA_13,_FUNCTION)},
#endif
#ifdef EBI_DATA_14
    {ATPASTE2(EBI_DATA_14,_PIN),ATPASTE2(EBI_DATA_14,_FUNCTION)},
#endif
#ifdef EBI_DATA_15
    {ATPASTE2(EBI_DATA_15,_PIN),ATPASTE2(EBI_DATA_15,_FUNCTION)},
#endif
#ifdef EBI_DATA_16
    {ATPASTE2(EBI_DATA_16,_PIN),ATPASTE2(EBI_DATA_16,_FUNCTION)},
#endif
#ifdef EBI_DATA_17
    {ATPASTE2(EBI_DATA_17,_PIN),ATPASTE2(EBI_DATA_17,_FUNCTION)},
#endif
#ifdef EBI_DATA_18
    {ATPASTE2(EBI_DATA_18,_PIN),ATPASTE2(EBI_DATA_18,_FUNCTION)},
#endif
#ifdef EBI_DATA_19
    {ATPASTE2(EBI_DATA_19,_PIN),ATPASTE2(EBI_DATA_19,_FUNCTION)},
#endif
#ifdef EBI_DATA_20
    {ATPASTE2(EBI_DATA_20,_PIN),ATPASTE2(EBI_DATA_20,_FUNCTION)},
#endif
#ifdef EBI_DATA_21
    {ATPASTE2(EBI_DATA_21,_PIN),ATPASTE2(EBI_DATA_21,_FUNCTION)},
#endif
#ifdef EBI_DATA_22
    {ATPASTE2(EBI_DATA_22,_PIN),ATPASTE2(EBI_DATA_22,_FUNCTION)},
#endif
#ifdef EBI_DATA_23
    {ATPASTE2(EBI_DATA_23,_PIN),ATPASTE2(EBI_DATA_23,_FUNCTION)},
#endif
#ifdef EBI_DATA_24
    {ATPASTE2(EBI_DATA_24,_PIN),ATPASTE2(EBI_DATA_24,_FUNCTION)},
#endif
#ifdef EBI_DATA_25
    {ATPASTE2(EBI_DATA_25,_PIN),ATPASTE2(EBI_DATA_25,_FUNCTION)},
#endif
#ifdef EBI_DATA_26
    {ATPASTE2(EBI_DATA_26,_PIN),ATPASTE2(EBI_DATA_26,_FUNCTION)},
#endif
#ifdef EBI_DATA_27
    {ATPASTE2(EBI_DATA_27,_PIN),ATPASTE2(EBI_DATA_27,_FUNCTION)},
#endif
#ifdef EBI_DATA_28
    {ATPASTE2(EBI_DATA_28,_PIN),ATPASTE2(EBI_DATA_28,_FUNCTION)},
#endif
#ifdef EBI_DATA_29
    {ATPASTE2(EBI_DATA_29,_PIN),ATPASTE2(EBI_DATA_29,_FUNCTION)},
#endif
#ifdef EBI_DATA_30
    {ATPASTE2(EBI_DATA_30,_PIN),ATPASTE2(EBI_DATA_30,_FUNCTION)},
#endif
#ifdef EBI_DATA_31
    {ATPASTE2(EBI_DATA_31,_PIN),ATPASTE2(EBI_DATA_31,_FUNCTION)},
#endif

    // Enable address pins.
#if SMC_DBW_GLOBAL <= 8
#ifdef EBI_ADDR_0
    {ATPASTE2(EBI_ADDR_0,_PIN),ATPASTE2(EBI_ADDR_0,_FUNCTION)},
#endif
#endif
#if SMC_DBW_GLOBAL <= 16
#ifdef EBI_ADDR_1
    {ATPASTE2(EBI_ADDR_1,_PIN),ATPASTE2(EBI_ADDR_1,_FUNCTION)},
#endif
#endif

#ifdef EBI_ADDR_2
    {ATPASTE2(EBI_ADDR_2,_PIN),ATPASTE2(EBI_ADDR_2,_FUNCTION)},
#endif
#ifdef EBI_ADDR_3
    {ATPASTE2(EBI_ADDR_3,_PIN),ATPASTE2(EBI_ADDR_3,_FUNCTION)},
#endif
#ifdef EBI_ADDR_4
    {ATPASTE2(EBI_ADDR_4,_PIN),ATPASTE2(EBI_ADDR_4,_FUNCTION)},
#endif
#ifdef EBI_ADDR_5
    {ATPASTE2(EBI_ADDR_5,_PIN),ATPASTE2(EBI_ADDR_5,_FUNCTION)},
#endif
#ifdef EBI_ADDR_6
    {ATPASTE2(EBI_ADDR_6,_PIN),ATPASTE2(EBI_ADDR_6,_FUNCTION)},
#endif
#ifdef EBI_ADDR_7
    {ATPASTE2(EBI_ADDR_7,_PIN),ATPASTE2(EBI_ADDR_7,_FUNCTION)},
#endif
#ifdef EBI_ADDR_8
    {ATPASTE2(EBI_ADDR_8,_PIN),ATPASTE2(EBI_ADDR_8,_FUNCTION)},
#endif
#ifdef EBI_ADDR_9
    {ATPASTE2(EBI_ADDR_9,_PIN),ATPASTE2(EBI_ADDR_9,_FUNCTION)},
#endif
#ifdef EBI_ADDR_10
    {ATPASTE2(EBI_ADDR_10,_PIN),ATPASTE2(EBI_ADDR_10,_FUNCTION)},
#endif
#ifdef EBI_ADDR_11
    {ATPASTE2(EBI_ADDR_11,_PIN),ATPASTE2(EBI_ADDR_11,_FUNCTION)},
#endif
#ifdef EBI_ADDR_12
    {ATPASTE2(EBI_ADDR_12,_PIN),ATPASTE2(EBI_ADDR_12,_FUNCTION)},
#endif
#ifdef EBI_ADDR_13
    {ATPASTE2(EBI_ADDR_13,_PIN),ATPASTE2(EBI_ADDR_13,_FUNCTION)},
#endif
#ifdef EBI_ADDR_14
    {ATPASTE2(EBI_ADDR_14,_PIN),ATPASTE2(EBI_ADDR_14,_FUNCTION)},
#endif
#ifdef EBI_ADDR_15
    {ATPASTE2(EBI_ADDR_15,_PIN),ATPASTE2(EBI_ADDR_15,_FUNCTION)},
#endif
#ifdef EBI_ADDR_16
    {ATPASTE2(EBI_ADDR_16,_PIN),ATPASTE2(EBI_ADDR_16,_FUNCTION)},
#endif
#ifdef EBI_ADDR_17
    {ATPASTE2(EBI_ADDR_17,_PIN),ATPASTE2(EBI_ADDR_17,_FUNCTION)},
#endif
#ifdef EBI_ADDR_18
    {ATPASTE2(EBI_ADDR_18,_PIN),ATPASTE2(EBI_ADDR_18,_FUNCTION)},
#endif
#ifdef EBI_ADDR_19
    {ATPASTE2(EBI_ADDR_19,_PIN),ATPASTE2(EBI_ADDR_19,_FUNCTION)},
#endif
#ifdef EBI_ADDR_20
    {ATPASTE2(EBI_ADDR_20,_PIN),ATPASTE2(EBI_ADDR_20,_FUNCTION)},
#endif
#ifdef EBI_ADDR_21
    {ATPASTE2(EBI_ADDR_21,_PIN),ATPASTE2(EBI_ADDR_21,_FUNCTION)},
#endif
#ifdef EBI_ADDR_22
    {ATPASTE2(EBI_ADDR_22,_PIN),ATPASTE2(EBI_ADDR_22,_FUNCTION)},
#endif
#ifdef EBI_ADDR_23
    {ATPASTE2(EBI_ADDR_23,_PIN),ATPASTE2(EBI_ADDR_23,_FUNCTION)},
#endif

#if SMC_DBW_GLOBAL <= 8
  #undef SMC_8_BIT_CHIPS
  #define SMC_8_BIT_CHIPS   TRUE
#endif

    // Enable data mask pins.
#if !SMC_8_BIT_CHIPS_GLOBAL
#ifdef EBI_ADDR_0
    {ATPASTE2(EBI_ADDR_0,_PIN),ATPASTE2(EBI_ADDR_0,_FUNCTION)},
#endif
#endif
#ifdef EBI_NWE0
    {ATPASTE2(EBI_NWE0,_PIN),ATPASTE2(EBI_NWE0,_FUNCTION)},
#endif

#if SMC_DBW_GLOBAL >= 16
  #ifdef EBI_NWE1
      {ATPASTE2(EBI_NWE1,_PIN),ATPASTE2(EBI_NWE1,_FUNCTION)},
  #endif
  #if SMC_DBW_GLOBAL >= 32
    #ifdef EBI_ADDR_1
        {ATPASTE2(EBI_ADDR_1,_PIN),ATPASTE2(EBI_ADDR_1,_FUNCTION)},
    #endif
    #ifdef EBI_NWE3
        {ATPASTE2(EBI_NWE3,_PIN),ATPASTE2(EBI_NWE3,_FUNCTION)},
    #endif
  #endif
#endif
    #ifdef EBI_NRD
        {ATPASTE2(EBI_NRD,_PIN),ATPASTE2(EBI_NRD,_FUNCTION)},
    #endif

    // Enable control pins.
#if NWAIT_MODE_GLOBAL != AVR32_SMC_EXNW_MODE_DISABLED
    #ifdef EBI_NWAIT
        {ATPASTE2(EBI_NWAIT,_PIN),ATPASTE2(EBI_NWAIT,_FUNCTION)},
    #endif
#endif
#ifdef SMC_USE_NCS0
    #ifdef EBI_NCS_0
        {ATPASTE2(EBI_NCS_0,_PIN),ATPASTE2(EBI_NCS_0,_FUNCTION)},
    #endif
#endif
#ifdef SMC_USE_NCS1
    #ifdef EBI_NCS_1
        {ATPASTE2(EBI_NCS_1,_PIN),ATPASTE2(EBI_NCS_1,_FUNCTION)},
    #endif
#endif
#ifdef SMC_USE_NCS2
    #ifdef EBI_NCS_2
        {ATPASTE2(EBI_NCS_2,_PIN),ATPASTE2(EBI_NCS_2,_FUNCTION)},
    #endif
#endif
#ifdef SMC_USE_NCS3
    #ifdef EBI_NCS_3
        {ATPASTE2(EBI_NCS_3,_PIN),ATPASTE2(EBI_NCS_3,_FUNCTION)},
    #endif
#endif
#ifdef SMC_USE_NCS4
    #ifdef EBI_NCS_4
        {ATPASTE2(EBI_NCS_4,_PIN),ATPASTE2(EBI_NCS_4,_FUNCTION)},
    #endif
#endif
#ifdef SMC_USE_NCS5
    #ifdef EBI_NCS_5
        {ATPASTE2(EBI_NCS_5,_PIN),ATPASTE2(EBI_NCS_5,_FUNCTION)},
    #endif
#endif
 };

  gpio_enable_module(SMC_EBI_GPIO_MAP, sizeof(SMC_EBI_GPIO_MAP) / sizeof(SMC_EBI_GPIO_MAP[0]));
}

unsigned char smc_get_cs_size(unsigned char cs)
{
  return smc_tab_cs_size[cs];
}
