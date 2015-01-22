/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Abstraction layer for memory interfaces.
 *
 * This module contains the interfaces:
 *   - MEM <-> USB;
 *   - MEM <-> RAM;
 *   - MEM <-> MEM.
 *
 * This module may be configured and expanded to support the following features:
 *   - write-protected globals;
 *   - password-protected data;
 *   - specific features;
 *   - etc.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices can be used.
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

//_____ I N C L U D E S ____________________________________________________

#include "compiler.h"
#include "preprocessor.h"
#ifdef FREERTOS_USED
#include "FreeRTOS.h"
#include "semphr.h"
#endif
#include "ctrl_access.h"


//_____ D E F I N I T I O N S ______________________________________________

#ifdef FREERTOS_USED

/*! \name LUN Access Protection Macros
 */
//! @{

/*! \brief Locks accesses to LUNs.
 *
 * \return \c TRUE if the access was successfully locked, else \c FALSE.
 */
#define Ctrl_access_lock()    ctrl_access_lock()

/*! \brief Unlocks accesses to LUNs.
 */
#define Ctrl_access_unlock()  xSemaphoreGive(ctrl_access_semphr)

//! @}

//! Handle to the semaphore protecting accesses to LUNs.
static xSemaphoreHandle ctrl_access_semphr = NULL;

#else

/*! \name LUN Access Protection Macros
 */
//! @{

/*! \brief Locks accesses to LUNs.
 *
 * \return \c TRUE if the access was successfully locked, else \c FALSE.
 */
#define Ctrl_access_lock()    TRUE

/*! \brief Unlocks accesses to LUNs.
 */
#define Ctrl_access_unlock()

//! @}

#endif  // FREERTOS_USED


#if MAX_LUN

/*! \brief Initializes an entry of the LUN descriptor table.
 *
 * \param lun Logical Unit Number.
 *
 * \return LUN descriptor table entry initializer.
 */
#if ACCESS_USB == ENABLED && ACCESS_MEM_TO_RAM == ENABLED
#define Lun_desc_entry(lun) \
  {\
    TPASTE3(Lun_, lun, _test_unit_ready),\
    TPASTE3(Lun_, lun, _read_capacity),\
    TPASTE3(Lun_, lun, _wr_protect),\
    TPASTE3(Lun_, lun, _removal),\
    TPASTE3(Lun_, lun, _usb_read_10),\
    TPASTE3(Lun_, lun, _usb_write_10),\
    TPASTE3(Lun_, lun, _mem_2_ram),\
    TPASTE3(Lun_, lun, _ram_2_mem),\
    TPASTE3(LUN_, lun, _NAME)\
  }
#elif ACCESS_USB == ENABLED
#define Lun_desc_entry(lun) \
  {\
    TPASTE3(Lun_, lun, _test_unit_ready),\
    TPASTE3(Lun_, lun, _read_capacity),\
    TPASTE3(Lun_, lun, _wr_protect),\
    TPASTE3(Lun_, lun, _removal),\
    TPASTE3(Lun_, lun, _usb_read_10),\
    TPASTE3(Lun_, lun, _usb_write_10),\
    TPASTE3(LUN_, lun, _NAME)\
  }
#elif ACCESS_MEM_TO_RAM == ENABLED
#define Lun_desc_entry(lun) \
  {\
    TPASTE3(Lun_, lun, _test_unit_ready),\
    TPASTE3(Lun_, lun, _read_capacity),\
    TPASTE3(Lun_, lun, _wr_protect),\
    TPASTE3(Lun_, lun, _removal),\
    TPASTE3(Lun_, lun, _mem_2_ram),\
    TPASTE3(Lun_, lun, _ram_2_mem),\
    TPASTE3(LUN_, lun, _NAME)\
  }
#else
#define Lun_desc_entry(lun) \
  {\
    TPASTE3(Lun_, lun, _test_unit_ready),\
    TPASTE3(Lun_, lun, _read_capacity),\
    TPASTE3(Lun_, lun, _wr_protect),\
    TPASTE3(Lun_, lun, _removal),\
    TPASTE3(LUN_, lun, _NAME)\
  }
#endif

//! LUN descriptor table.
static const struct
{
  Ctrl_status (*test_unit_ready)(void);
  Ctrl_status (*read_capacity)(U32 *);
  Bool (*wr_protect)(void);
  Bool (*removal)(void);
#if ACCESS_USB == ENABLED
  Ctrl_status (*usb_read_10)(U32, U16);
  Ctrl_status (*usb_write_10)(U32, U16);
#endif
#if ACCESS_MEM_TO_RAM == ENABLED
  Ctrl_status (*mem_2_ram)(U32, void *);
  Ctrl_status (*ram_2_mem)(U32, const void *);
#endif
  const char *name;
} lun_desc[MAX_LUN] =
{
#if LUN_0 == ENABLE
  Lun_desc_entry(0),
#endif
#if LUN_1 == ENABLE
  Lun_desc_entry(1),
#endif
#if LUN_2 == ENABLE
  Lun_desc_entry(2),
#endif
#if LUN_3 == ENABLE
  Lun_desc_entry(3),
#endif
#if LUN_4 == ENABLE
  Lun_desc_entry(4),
#endif
#if LUN_5 == ENABLE
  Lun_desc_entry(5),
#endif
#if LUN_6 == ENABLE
  Lun_desc_entry(6),
#endif
#if LUN_7 == ENABLE
  Lun_desc_entry(7)
#endif
};

#endif


#if GLOBAL_WR_PROTECT == ENABLED
Bool g_wr_protect;
#endif


/*! \name Control Interface
 */
//! @{


#ifdef FREERTOS_USED

Bool ctrl_access_init(void)
{
  // If the handle to the protecting semaphore is not valid,
  if (!ctrl_access_semphr)
  {
    // try to create the semaphore.
    vSemaphoreCreateBinary(ctrl_access_semphr);

    // If the semaphore could not be created, there is no backup solution.
    if (!ctrl_access_semphr) return FALSE;
  }

  return TRUE;
}


/*! \brief Locks accesses to LUNs.
 *
 * \return \c TRUE if the access was successfully locked, else \c FALSE.
 */
static Bool ctrl_access_lock(void)
{
  // If the semaphore could not be created, there is no backup solution.
  if (!ctrl_access_semphr) return FALSE;

  // Wait for the semaphore.
  while (!xSemaphoreTake(ctrl_access_semphr, portMAX_DELAY));

  return TRUE;
}

#endif  // FREERTOS_USED


U8 get_nb_lun(void)
{
#if MEM_USB == ENABLE
  U8 nb_lun;

  if (!Ctrl_access_lock()) return MAX_LUN;

  nb_lun = MAX_LUN + host_get_lun();

  Ctrl_access_unlock();

  return nb_lun;
#else
  return MAX_LUN;
#endif
}


U8 get_cur_lun(void)
{
  return LUN_ID_0;
}


Ctrl_status mem_test_unit_ready(U8 lun)
{
  Ctrl_status status;

  if (!Ctrl_access_lock()) return CTRL_FAIL;

  status =
#if MAX_LUN
           (lun < MAX_LUN) ? lun_desc[lun].test_unit_ready() :
#endif
#if LUN_USB == ENABLE
                             Lun_usb_test_unit_ready(lun - LUN_ID_USB);
#else
                             CTRL_FAIL;
#endif

  Ctrl_access_unlock();

  return status;
}


Ctrl_status mem_read_capacity(U8 lun, U32 *u32_nb_sector)
{
  Ctrl_status status;

  if (!Ctrl_access_lock()) return CTRL_FAIL;

  status =
#if MAX_LUN
           (lun < MAX_LUN) ? lun_desc[lun].read_capacity(u32_nb_sector) :
#endif
#if LUN_USB == ENABLE
                             Lun_usb_read_capacity(lun - LUN_ID_USB, u32_nb_sector);
#else
                             CTRL_FAIL;
#endif

  Ctrl_access_unlock();

  return status;
}


U8 mem_sector_size(U8 lun)
{
  U8 sector_size;

  if (!Ctrl_access_lock()) return 0;

  sector_size =
#if MAX_LUN
                (lun < MAX_LUN) ? 1 :
#endif
#if LUN_USB == ENABLE
                                  Lun_usb_read_sector_size(lun - LUN_ID_USB);
#else
                                  0;
#endif

  Ctrl_access_unlock();

  return sector_size;
}


Bool mem_wr_protect(U8 lun)
{
  Bool wr_protect;

  if (!Ctrl_access_lock()) return TRUE;

  wr_protect =
#if MAX_LUN
               (lun < MAX_LUN) ? lun_desc[lun].wr_protect() :
#endif
#if LUN_USB == ENABLE
                                 Lun_usb_wr_protect(lun - LUN_ID_USB);
#else
                                 TRUE;
#endif

  Ctrl_access_unlock();

  return wr_protect;
}


Bool mem_removal(U8 lun)
{
  Bool removal;

  if (!Ctrl_access_lock()) return TRUE;

  removal =
#if MAX_LUN
            (lun < MAX_LUN) ? lun_desc[lun].removal() :
#endif
#if LUN_USB == ENABLE
                              Lun_usb_removal();
#else
                              TRUE;
#endif

  Ctrl_access_unlock();

  return removal;
}


const char *mem_name(U8 lun)
{
  return
#if MAX_LUN
         (lun < MAX_LUN) ? lun_desc[lun].name :
#endif
#if LUN_USB == ENABLE
                           LUN_USB_NAME;
#else
                           NULL;
#endif
}


//! @}


#if ACCESS_USB == ENABLED

/*! \name MEM <-> USB Interface
 */
//! @{


Ctrl_status memory_2_usb(U8 lun, U32 addr, U16 nb_sector)
{
  Ctrl_status status;

  if (!Ctrl_access_lock()) return CTRL_FAIL;

  memory_start_read_action(nb_sector);
  status =
#if MAX_LUN
           (lun < MAX_LUN) ? lun_desc[lun].usb_read_10(addr, nb_sector) :
#endif
                             CTRL_FAIL;
  memory_stop_read_action();

  Ctrl_access_unlock();

  return status;
}


Ctrl_status usb_2_memory(U8 lun, U32 addr, U16 nb_sector)
{
  Ctrl_status status;

  if (!Ctrl_access_lock()) return CTRL_FAIL;

  memory_start_write_action(nb_sector);
  status =
#if MAX_LUN
           (lun < MAX_LUN) ? lun_desc[lun].usb_write_10(addr, nb_sector) :
#endif
                             CTRL_FAIL;
  memory_stop_write_action();

  Ctrl_access_unlock();

  return status;
}


//! @}

#endif  // ACCESS_USB == ENABLED


#if ACCESS_MEM_TO_RAM == ENABLED

/*! \name MEM <-> RAM Interface
 */
//! @{


Ctrl_status memory_2_ram(U8 lun, U32 addr, void *ram)
{
  Ctrl_status status;

  if (!Ctrl_access_lock()) return CTRL_FAIL;

  memory_start_read_action(1);
  status =
#if MAX_LUN
           (lun < MAX_LUN) ? lun_desc[lun].mem_2_ram(addr, ram) :
#endif
#if LUN_USB == ENABLE
                             Lun_usb_mem_2_ram(addr, ram);
#else
                             CTRL_FAIL;
#endif
  memory_stop_read_action();

  Ctrl_access_unlock();

  return status;
}


Ctrl_status ram_2_memory(U8 lun, U32 addr, const void *ram)
{
  Ctrl_status status;

  if (!Ctrl_access_lock()) return CTRL_FAIL;

  memory_start_write_action(1);
  status =
#if MAX_LUN
           (lun < MAX_LUN) ? lun_desc[lun].ram_2_mem(addr, ram) :
#endif
#if LUN_USB == ENABLE
                             Lun_usb_ram_2_mem(addr, ram);
#else
                             CTRL_FAIL;
#endif
  memory_stop_write_action();

  Ctrl_access_unlock();

  return status;
}


//! @}

#endif  // ACCESS_MEM_TO_RAM == ENABLED


#if ACCESS_STREAM == ENABLED

/*! \name Streaming MEM <-> MEM Interface
 */
//! @{


  #if ACCESS_MEM_TO_MEM == ENABLED

#include "fat.h"

Ctrl_status stream_mem_to_mem(U8 src_lun, U32 src_addr, U8 dest_lun, U32 dest_addr, U16 nb_sector)
{
#if (defined __GNUC__) && (defined __AVR32__)
  __attribute__((__aligned__(4)))
#elif (defined __ICCAVR32__)
  #pragma data_alignment = 4
#endif
  static U8 sector_buf[FS_512B];
  Ctrl_status status = CTRL_GOOD;

  while (nb_sector--)
  {
    if ((status = memory_2_ram(src_lun, src_addr++, sector_buf)) != CTRL_GOOD) break;
    if ((status = ram_2_memory(dest_lun, dest_addr++, sector_buf)) != CTRL_GOOD) break;
  }

  return status;
}

  #endif  // ACCESS_MEM_TO_MEM == ENABLED


Ctrl_status stream_state(U8 id)
{
  return CTRL_GOOD;
}


U16 stream_stop(U8 id)
{
  return 0;
}


//! @}

#endif  // ACCESS_STREAM == ENABLED
