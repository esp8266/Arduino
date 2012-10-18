/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief CTRL_ACCESS interface for the AT45DBX data flash controller.
 *
 * - Compiler:           IAR EWAVR32 and GNU GCC for AVR32
 * - Supported devices:  All AVR32 devices with an SPI module can be used.
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

//_____  I N C L U D E S ___________________________________________________

#include "conf_access.h"


#if AT45DBX_MEM == ENABLE

#include "conf_at45dbx.h"
#include "at45dbx.h"
#include "at45dbx_mem.h"


//_____ D E F I N I T I O N S ______________________________________________

//! Whether to detect write accesses to the memory.
#define AT45DBX_MEM_TEST_CHANGE_STATE     ENABLED


#if (ACCESS_USB == ENABLED || ACCESS_MEM_TO_RAM == ENABLED) && AT45DBX_MEM_TEST_CHANGE_STATE == ENABLED

//! Memory data modified flag.
static volatile Bool s_b_data_modify = FALSE;

#endif


/*! \name Control Interface
 */
//! @{


Ctrl_status at45dbx_test_unit_ready(void)
{
  return (at45dbx_mem_check() == OK) ? CTRL_GOOD : CTRL_NO_PRESENT;
}


Ctrl_status at45dbx_read_capacity(U32 *u32_nb_sector)
{
  *u32_nb_sector = (AT45DBX_MEM_CNT << (AT45DBX_MEM_SIZE - AT45DBX_SECTOR_BITS)) - 1;

  return CTRL_GOOD;
}


Bool at45dbx_wr_protect(void)
{
  return FALSE;
}


Bool at45dbx_removal(void)
{
  return FALSE;
}


//! @}


#if ACCESS_USB == ENABLED

#include "usb_drv.h"
#include "scsi_decoder.h"


/*! \name MEM <-> USB Interface
 */
//! @{


Ctrl_status at45dbx_usb_read_10(U32 addr, U16 nb_sector)
{
  if (addr + nb_sector > AT45DBX_MEM_CNT << (AT45DBX_MEM_SIZE - AT45DBX_SECTOR_BITS)) return CTRL_FAIL;

  at45dbx_read_open(addr);
  at45dbx_read_multiple_sector(nb_sector);
  at45dbx_read_close();

  return CTRL_GOOD;
}


void at45dbx_read_multiple_sector_callback(const void *psector)
{
  U16 data_to_transfer = AT45DBX_SECTOR_SIZE;

  // Transfer read sector to the USB interface.
  while (data_to_transfer)
  {
    while (!Is_usb_in_ready(g_scsi_ep_ms_in))
    {
      if(!Is_usb_endpoint_enabled(g_scsi_ep_ms_in))
         return; // USB Reset
    }         

    Usb_reset_endpoint_fifo_access(g_scsi_ep_ms_in);
    data_to_transfer = usb_write_ep_txpacket(g_scsi_ep_ms_in, psector,
                                             data_to_transfer, &psector);
    Usb_ack_in_ready_send(g_scsi_ep_ms_in);
  }
}


Ctrl_status at45dbx_usb_write_10(U32 addr, U16 nb_sector)
{
  if (addr + nb_sector > AT45DBX_MEM_CNT << (AT45DBX_MEM_SIZE - AT45DBX_SECTOR_BITS)) return CTRL_FAIL;

#if AT45DBX_MEM_TEST_CHANGE_STATE == ENABLED
  if (nb_sector) s_b_data_modify = TRUE;
#endif

  at45dbx_write_open(addr);
  at45dbx_write_multiple_sector(nb_sector);
  at45dbx_write_close();

  return CTRL_GOOD;
}


void at45dbx_write_multiple_sector_callback(void *psector)
{
  U16 data_to_transfer = AT45DBX_SECTOR_SIZE;

  // Transfer sector to write from the USB interface.
  while (data_to_transfer)
  {
    while (!Is_usb_out_received(g_scsi_ep_ms_out))
    {
      if(!Is_usb_endpoint_enabled(g_scsi_ep_ms_out))
         return; // USB Reset
    }         

    Usb_reset_endpoint_fifo_access(g_scsi_ep_ms_out);
    data_to_transfer = usb_read_ep_rxpacket(g_scsi_ep_ms_out, psector,
                                            data_to_transfer, &psector);
    Usb_ack_out_received_free(g_scsi_ep_ms_out);
  }
}


//! @}

#endif  // ACCESS_USB == ENABLED


#if ACCESS_MEM_TO_RAM == ENABLED

/*! \name MEM <-> RAM Interface
 */
//! @{


Ctrl_status at45dbx_df_2_ram(U32 addr, void *ram)
{
  if (addr + 1 > AT45DBX_MEM_CNT << (AT45DBX_MEM_SIZE - AT45DBX_SECTOR_BITS)) return CTRL_FAIL;

  at45dbx_read_open(addr);
  at45dbx_read_sector_2_ram(ram);
  at45dbx_read_close();

  return CTRL_GOOD;
}


Ctrl_status at45dbx_ram_2_df(U32 addr, const void *ram)
{
  if (addr + 1 > AT45DBX_MEM_CNT << (AT45DBX_MEM_SIZE - AT45DBX_SECTOR_BITS)) return CTRL_FAIL;

#if AT45DBX_MEM_TEST_CHANGE_STATE == ENABLED
  s_b_data_modify = TRUE;
#endif

  at45dbx_write_open(addr);
  at45dbx_write_sector_from_ram(ram);
  at45dbx_write_close();

  return CTRL_GOOD;
}


//! @}

#endif  // ACCESS_MEM_TO_RAM == ENABLED


#endif  // AT45DBX_MEM == ENABLE
