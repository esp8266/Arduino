/* This source file is part of the ATMEL AVR-UC3-SoftwareFramework-1.7.0 Release */

/*This file is prepared for Doxygen automatic documentation generation.*/
/*! \file *********************************************************************
 *
 * \brief Management of the AT45DBX data flash controller through SPI.
 *
 * This file manages the accesses to the AT45DBX data flash components.
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

#include "compiler.h"
#include "board.h"
#include "gpio.h"
#include "spi.h"
#include "conf_at45dbx.h"
#include "at45dbx.h"


#if AT45DBX_MEM_CNT > 4
  #error AT45DBX_MEM_CNT must not exceed 4
#endif


//_____ D E F I N I T I O N S ______________________________________________

/*! \name AT45DBX Group A Commands
 */
//! @{
#define AT45DBX_CMDA_RD_PAGE              0xD2        //!< Main Memory Page Read (Serial/8-bit Mode).
#define AT45DBX_CMDA_RD_ARRAY_LEG         0xE8        //!< Continuous Array Read, Legacy Command (Serial/8-bit Mode).
#define AT45DBX_CMDA_RD_ARRAY_LF_SM       0x03        //!< Continuous Array Read, Low-Frequency Mode (Serial Mode).
#define AT45DBX_CMDA_RD_ARRAY_AF_SM       0x0B        //!< Continuous Array Read, Any-Frequency Mode (Serial Mode).
#define AT45DBX_CMDA_RD_SECTOR_PROT_REG   0x32        //!< Read Sector Protection Register (Serial/8-bit Mode).
#define AT45DBX_CMDA_RD_SECTOR_LKDN_REG   0x35        //!< Read Sector Lockdown Register (Serial/8-bit Mode).
#define AT45DBX_CMDA_RD_SECURITY_REG      0x77        //!< Read Security Register (Serial/8-bit Mode).
//! @}

/*! \name AT45DBX Group B Commands
 */
//! @{
#define AT45DBX_CMDB_ER_PAGE              0x81        //!< Page Erase (Serial/8-bit Mode).
#define AT45DBX_CMDB_ER_BLOCK             0x50        //!< Block Erase (Serial/8-bit Mode).
#define AT45DBX_CMDB_ER_SECTOR            0x7C        //!< Sector Erase (Serial/8-bit Mode).
#define AT45DBX_CMDB_ER_CHIP              0xC794809A  //!< Chip Erase (Serial/8-bit Mode).
#define AT45DBX_CMDB_XFR_PAGE_TO_BUF1     0x53        //!< Main Memory Page to Buffer 1 Transfer (Serial/8-bit Mode).
#define AT45DBX_CMDB_XFR_PAGE_TO_BUF2     0x55        //!< Main Memory Page to Buffer 2 Transfer (Serial/8-bit Mode).
#define AT45DBX_CMDB_CMP_PAGE_TO_BUF1     0x60        //!< Main Memory Page to Buffer 1 Compare (Serial/8-bit Mode).
#define AT45DBX_CMDB_CMP_PAGE_TO_BUF2     0x61        //!< Main Memory Page to Buffer 2 Compare (Serial/8-bit Mode).
#define AT45DBX_CMDB_PR_BUF1_TO_PAGE_ER   0x83        //!< Buffer 1 to Main Memory Page Program with Built-in Erase (Serial/8-bit Mode).
#define AT45DBX_CMDB_PR_BUF2_TO_PAGE_ER   0x86        //!< Buffer 2 to Main Memory Page Program with Built-in Erase (Serial/8-bit Mode).
#define AT45DBX_CMDB_PR_BUF1_TO_PAGE      0x88        //!< Buffer 1 to Main Memory Page Program without Built-in Erase (Serial/8-bit Mode).
#define AT45DBX_CMDB_PR_BUF2_TO_PAGE      0x89        //!< Buffer 2 to Main Memory Page Program without Built-in Erase (Serial/8-bit Mode).
#define AT45DBX_CMDB_PR_PAGE_TH_BUF1      0x82        //!< Main Memory Page Program through Buffer 1 (Serial/8-bit Mode).
#define AT45DBX_CMDB_PR_PAGE_TH_BUF2      0x85        //!< Main Memory Page Program through Buffer 2 (Serial/8-bit Mode).
#define AT45DBX_CMDB_RWR_PAGE_TH_BUF1     0x58        //!< Auto Page Rewrite through Buffer 1 (Serial/8-bit Mode).
#define AT45DBX_CMDB_RWR_PAGE_TH_BUF2     0x59        //!< Auto Page Rewrite through Buffer 2 (Serial/8-bit Mode).
//! @}

/*! \name AT45DBX Group C Commands
 */
//! @{
#define AT45DBX_CMDC_RD_BUF1_LF_SM        0xD1        //!< Buffer 1 Read, Low-Frequency Mode (Serial Mode).
#define AT45DBX_CMDC_RD_BUF2_LF_SM        0xD3        //!< Buffer 2 Read, Low-Frequency Mode (Serial Mode).
#define AT45DBX_CMDC_RD_BUF1_AF_SM        0xD4        //!< Buffer 1 Read, Any-Frequency Mode (Serial Mode).
#define AT45DBX_CMDC_RD_BUF2_AF_SM        0xD6        //!< Buffer 2 Read, Any-Frequency Mode (Serial Mode).
#define AT45DBX_CMDC_RD_BUF1_AF_8M        0x54        //!< Buffer 1 Read, Any-Frequency Mode (8-bit Mode).
#define AT45DBX_CMDC_RD_BUF2_AF_8M        0x56        //!< Buffer 2 Read, Any-Frequency Mode (8-bit Mode).
#define AT45DBX_CMDC_WR_BUF1              0x84        //!< Buffer 1 Write (Serial/8-bit Mode).
#define AT45DBX_CMDC_WR_BUF2              0x87        //!< Buffer 2 Write (Serial/8-bit Mode).
#define AT45DBX_CMDC_RD_STATUS_REG        0xD7        //!< Status Register Read (Serial/8-bit Mode).
#define AT45DBX_CMDC_RD_MNFCT_DEV_ID_SM   0x9F        //!< Manufacturer and Device ID Read (Serial Mode).
//! @}

/*! \name AT45DBX Group D Commands
 */
//! @{
#define AT45DBX_CMDD_EN_SECTOR_PROT       0x3D2A7FA9  //!< Enable Sector Protection (Serial/8-bit Mode).
#define AT45DBX_CMDD_DIS_SECTOR_PROT      0x3D2A7F9A  //!< Disable Sector Protection (Serial/8-bit Mode).
#define AT45DBX_CMDD_ER_SECTOR_PROT_REG   0x3D2A7FCF  //!< Erase Sector Protection Register (Serial/8-bit Mode).
#define AT45DBX_CMDD_PR_SECTOR_PROT_REG   0x3D2A7FFC  //!< Program Sector Protection Register (Serial/8-bit Mode).
#define AT45DBX_CMDD_LKDN_SECTOR          0x3D2A7F30  //!< Sector Lockdown (Serial/8-bit Mode).
#define AT45DBX_CMDD_PR_SECURITY_REG      0x9B000000  //!< Program Security Register (Serial/8-bit Mode).
#define AT45DBX_CMDD_PR_CONF_REG          0x3D2A80A6  //!< Program Configuration Register (Serial/8-bit Mode).
#define AT45DBX_CMDD_DEEP_PWR_DN          0xB9        //!< Deep Power-down (Serial/8-bit Mode).
#define AT45DBX_CMDD_RSM_DEEP_PWR_DN      0xAB        //!< Resume from Deep Power-down (Serial/8-bit Mode).
//! @}


/*! \name Bit-Masks and Values for the Status Register
 */
//! @{
#define AT45DBX_MSK_BUSY                  0x80        //!< Busy status bit-mask.
#define AT45DBX_BUSY                      0x00        //!< Busy status value (0x00 when busy, 0x80 when ready).
#define AT45DBX_MSK_DENSITY               0x3C        //!< Device density bit-mask.
//! @}
#if AT45DBX_MEM_SIZE == AT45DBX_1MB

/*! \name AT45DB081 Memories
 */
//! @{
#define AT45DBX_DENSITY                   0x24        //!< Device density value.
#define AT45DBX_BYTE_ADDR_BITS             9          //!< Address bits for byte position within buffer.

//! @}
#elif AT45DBX_MEM_SIZE == AT45DBX_2MB

/*! \name AT45DB161 Memories
 */
//! @{
#define AT45DBX_DENSITY                   0x2C        //!< Device density value.
#define AT45DBX_BYTE_ADDR_BITS            10          //!< Address bits for byte position within buffer.
//! @}

#elif AT45DBX_MEM_SIZE == AT45DBX_4MB

/*! \name AT45DB321 Memories
 */
//! @{
#define AT45DBX_DENSITY                   0x34        //!< Device density value.
#define AT45DBX_BYTE_ADDR_BITS            10          //!< Address bits for byte position within buffer.
//! @}

#elif AT45DBX_MEM_SIZE == AT45DBX_8MB

/*! \name AT45DB642 Memories
 */
//! @{
#define AT45DBX_DENSITY                   0x3C        //!< Device density value.
#define AT45DBX_BYTE_ADDR_BITS            11          //!< Address bits for byte position within buffer.
//! @}

#else
  #error AT45DBX_MEM_SIZE is not defined to a supported value
#endif

//! Address bits for page selection.
#define AT45DBX_PAGE_ADDR_BITS            (AT45DBX_MEM_SIZE - AT45DBX_PAGE_BITS)

//! Number of bits for addresses within pages.
#define AT45DBX_PAGE_BITS                 (AT45DBX_BYTE_ADDR_BITS - 1)

//! Page size in bytes.
#define AT45DBX_PAGE_SIZE                 (1 << AT45DBX_PAGE_BITS)

//! Bit-mask for byte position within buffer in \ref gl_ptr_mem.
#define AT45DBX_MSK_PTR_BYTE              ((1 << AT45DBX_PAGE_BITS) - 1)

//! Bit-mask for page selection in \ref gl_ptr_mem.
#define AT45DBX_MSK_PTR_PAGE              (((1 << AT45DBX_PAGE_ADDR_BITS) - 1) << AT45DBX_PAGE_BITS)

//! Bit-mask for byte position within sector in \ref gl_ptr_mem.
#define AT45DBX_MSK_PTR_SECTOR            ((1 << AT45DBX_SECTOR_BITS) - 1)


/*! \brief Sends a dummy byte through SPI.
 */
#define spi_write_dummy()                 spi_write(AT45DBX_SPI, 0xFF)


//! Boolean indicating whether memory is in busy state.
static Bool at45dbx_busy;

//! Memory data pointer.
static U32 gl_ptr_mem;

//! Sector buffer.
static U8 sector_buf[AT45DBX_SECTOR_SIZE];


/*! \name Control Functions
 */
//! @{


Bool at45dbx_init(spi_options_t spiOptions, unsigned int pba_hz)
{
  // Setup SPI registers according to spiOptions.
  for (spiOptions.reg = AT45DBX_SPI_FIRST_NPCS;
       spiOptions.reg < AT45DBX_SPI_FIRST_NPCS + AT45DBX_MEM_CNT;
       spiOptions.reg++)
  {
    if (spi_setupChipReg(AT45DBX_SPI, &spiOptions, pba_hz) != SPI_OK) return KO;
  }

  // Memory ready.
  at45dbx_busy = FALSE;

  return OK;
}


/*! \brief Selects or unselects a DF memory.
 *
 * \param memidx  Memory ID of DF to select or unselect.
 * \param bSelect Boolean indicating whether the DF memory has to be selected.
 */
static void at45dbx_chipselect_df(U8 memidx, Bool bSelect)
{
  if (bSelect)
  {
    // Select SPI chip.
    spi_selectChip(AT45DBX_SPI, AT45DBX_SPI_FIRST_NPCS + memidx);
  }
  else
  {
    // Unselect SPI chip.
    spi_unselectChip(AT45DBX_SPI, AT45DBX_SPI_FIRST_NPCS + memidx);
  }
}


Bool at45dbx_mem_check(void)
{
  U8 df;
  U16 status = 0;

  // DF memory check.
  for (df = 0; df < AT45DBX_MEM_CNT; df++)
  {
    // Select the DF memory to check.
    at45dbx_chipselect_df(df, TRUE);

    // Send the Status Register Read command.
    spi_write(AT45DBX_SPI, AT45DBX_CMDC_RD_STATUS_REG);

    // Send a dummy byte to read the status register.
    spi_write_dummy();
    spi_read(AT45DBX_SPI, &status);

    // Unselect the checked DF memory.
    at45dbx_chipselect_df(df, FALSE);

    // Unexpected device density value.
    if ((status & AT45DBX_MSK_DENSITY) < AT45DBX_DENSITY) return KO;
  }

  return OK;
}


/*! \brief Waits until the DF is ready.
 */
static void at45dbx_wait_ready(void)
{
  U16 status;

  // Select the DF memory gl_ptr_mem points to.
  at45dbx_chipselect_df(gl_ptr_mem >> AT45DBX_MEM_SIZE, TRUE);

  // Send the Status Register Read command.
  spi_write(AT45DBX_SPI, AT45DBX_CMDC_RD_STATUS_REG);

  // Read the status register until the DF is ready.
  do
  {
    // Send a dummy byte to read the status register.
    spi_write_dummy();
    spi_read(AT45DBX_SPI, &status);
  } while ((status & AT45DBX_MSK_BUSY) == AT45DBX_BUSY);

  // Unselect the DF memory gl_ptr_mem points to.
  at45dbx_chipselect_df(gl_ptr_mem >> AT45DBX_MEM_SIZE, FALSE);
}


Bool at45dbx_read_open(U32 sector)
{
  U32 addr;

  // Set the global memory pointer to a byte address.
  gl_ptr_mem = sector << AT45DBX_SECTOR_BITS; // gl_ptr_mem = sector * AT45DBX_SECTOR_SIZE.

  // If the DF memory is busy, wait until it's ready.
  if (at45dbx_busy) at45dbx_wait_ready();
  at45dbx_busy = FALSE;

  // Select the DF memory gl_ptr_mem points to.
  at45dbx_chipselect_df(gl_ptr_mem >> AT45DBX_MEM_SIZE, TRUE);

  // Initiate a page read at a given sector.

  // Send the Main Memory Page Read command.
  spi_write(AT45DBX_SPI, AT45DBX_CMDA_RD_PAGE);

  // Send the three address bytes, which comprise:
  //  - (24 - (AT45DBX_PAGE_ADDR_BITS + AT45DBX_BYTE_ADDR_BITS)) reserved bits;
  //  - then AT45DBX_PAGE_ADDR_BITS bits specifying the page in main memory to be read;
  //  - then AT45DBX_BYTE_ADDR_BITS bits specifying the starting byte address within that page.
  // NOTE: The bits of gl_ptr_mem above the AT45DBX_MEM_SIZE bits are useless for the local
  // DF addressing. They are used for DF discrimination when there are several DFs.
  addr = (Rd_bitfield(gl_ptr_mem, AT45DBX_MSK_PTR_PAGE) << AT45DBX_BYTE_ADDR_BITS) |
         Rd_bitfield(gl_ptr_mem, AT45DBX_MSK_PTR_BYTE);
  spi_write(AT45DBX_SPI, LSB2W(addr));
  spi_write(AT45DBX_SPI, LSB1W(addr));
  spi_write(AT45DBX_SPI, LSB0W(addr));

  // Send 32 don't care clock cycles to initialize the read operation.
  spi_write_dummy();
  spi_write_dummy();
  spi_write_dummy();
  spi_write_dummy();

  return OK;
}


void at45dbx_read_close(void)
{
  // Unselect the DF memory gl_ptr_mem points to.
  at45dbx_chipselect_df(gl_ptr_mem >> AT45DBX_MEM_SIZE, FALSE);

  // Memory ready.
  at45dbx_busy = FALSE;
}


Bool at45dbx_write_open(U32 sector)
{
  U32 addr;

  // Set the global memory pointer to a byte address.
  gl_ptr_mem = sector << AT45DBX_SECTOR_BITS; // gl_ptr_mem = sector * AT45DBX_SECTOR_SIZE.

  // If the DF memory is busy, wait until it's ready.
  if (at45dbx_busy) at45dbx_wait_ready();
  at45dbx_busy = FALSE;

#if AT45DBX_PAGE_SIZE > AT45DBX_SECTOR_SIZE
  // Select the DF memory gl_ptr_mem points to.
  at45dbx_chipselect_df(gl_ptr_mem >> AT45DBX_MEM_SIZE, TRUE);

  // Transfer the content of the current page to buffer 1.

  // Send the Main Memory Page to Buffer 1 Transfer command.
  spi_write(AT45DBX_SPI, AT45DBX_CMDB_XFR_PAGE_TO_BUF1);

  // Send the three address bytes, which comprise:
  //  - (24 - (AT45DBX_PAGE_ADDR_BITS + AT45DBX_BYTE_ADDR_BITS)) reserved bits;
  //  - then AT45DBX_PAGE_ADDR_BITS bits specifying the page in main memory to be read;
  //  - then AT45DBX_BYTE_ADDR_BITS don't care bits.
  // NOTE: The bits of gl_ptr_mem above the AT45DBX_MEM_SIZE bits are useless for the local
  // DF addressing. They are used for DF discrimination when there are several DFs.
  addr = Rd_bitfield(gl_ptr_mem, AT45DBX_MSK_PTR_PAGE) << AT45DBX_BYTE_ADDR_BITS;
  spi_write(AT45DBX_SPI, LSB2W(addr));
  spi_write(AT45DBX_SPI, LSB1W(addr));
  spi_write(AT45DBX_SPI, LSB0W(addr));

  // Unselect the DF memory gl_ptr_mem points to.
  at45dbx_chipselect_df(gl_ptr_mem >> AT45DBX_MEM_SIZE, FALSE);

  // Wait for end of page transfer.
  at45dbx_wait_ready();
#endif

  // Select the DF memory gl_ptr_mem points to.
  at45dbx_chipselect_df(gl_ptr_mem >> AT45DBX_MEM_SIZE, TRUE);

  // Initiate a page write at a given sector.

  // Send the Main Memory Page Program through Buffer 1 command.
  spi_write(AT45DBX_SPI, AT45DBX_CMDB_PR_PAGE_TH_BUF1);

  // Send the three address bytes, which comprise:
  //  - (24 - (AT45DBX_PAGE_ADDR_BITS + AT45DBX_BYTE_ADDR_BITS)) reserved bits;
  //  - then AT45DBX_PAGE_ADDR_BITS bits specifying the page in main memory to be written;
  //  - then AT45DBX_BYTE_ADDR_BITS bits specifying the starting byte address within that page.
  // NOTE: The bits of gl_ptr_mem above the AT45DBX_MEM_SIZE bits are useless for the local
  // DF addressing. They are used for DF discrimination when there are several DFs.
  addr = (Rd_bitfield(gl_ptr_mem, AT45DBX_MSK_PTR_PAGE) << AT45DBX_BYTE_ADDR_BITS) |
         Rd_bitfield(gl_ptr_mem, AT45DBX_MSK_PTR_BYTE);
  spi_write(AT45DBX_SPI, LSB2W(addr));
  spi_write(AT45DBX_SPI, LSB1W(addr));
  spi_write(AT45DBX_SPI, LSB0W(addr));

  return OK;
}


void at45dbx_write_close(void)
{
  // While end of logical sector not reached, zero-fill remaining memory bytes.
  while (Rd_bitfield(gl_ptr_mem, AT45DBX_MSK_PTR_SECTOR))
  {
    spi_write(AT45DBX_SPI, 0x00);
    gl_ptr_mem++;
  }

  // Unselect the DF memory gl_ptr_mem points to.
  at45dbx_chipselect_df(gl_ptr_mem >> AT45DBX_MEM_SIZE, FALSE);

  // Memory busy.
  at45dbx_busy = TRUE;
}


//! @}


/*! \name Single-Byte Access Functions
 */
//! @{


U8 at45dbx_read_byte(void)
{
  U16 data;

  // Memory busy.
  if (at45dbx_busy)
  {
    // Being here, we know that we previously finished a page read.
    // => We have to access the next page.

    // Memory ready.
    at45dbx_busy = FALSE;

    // Eventually select the next DF and open the next page.
    // NOTE: at45dbx_read_open input parameter is a sector.
    at45dbx_read_open(gl_ptr_mem >> AT45DBX_SECTOR_BITS); // gl_ptr_mem / AT45DBX_SECTOR_SIZE.
  }

  // Send a dummy byte to read the next data byte.
  spi_write_dummy();
  spi_read(AT45DBX_SPI, &data);
  gl_ptr_mem++;

  // If end of page reached,
  if (!Rd_bitfield(gl_ptr_mem, AT45DBX_MSK_PTR_BYTE))
  {
    // unselect the DF memory gl_ptr_mem points to.
    at45dbx_chipselect_df(gl_ptr_mem >> AT45DBX_MEM_SIZE, FALSE);

    // Memory busy.
    at45dbx_busy = TRUE;
  }

  return data;
}


Bool at45dbx_write_byte(U8 b)
{
  // Memory busy.
  if (at45dbx_busy)
  {
    // Being here, we know that we previously launched a page programming.
    // => We have to access the next page.

    // Eventually select the next DF and open the next page.
    // NOTE: at45dbx_write_open input parameter is a sector.
    at45dbx_write_open(gl_ptr_mem >> AT45DBX_SECTOR_BITS); // gl_ptr_mem / AT45DBX_SECTOR_SIZE.
  }

  // Write the next data byte.
  spi_write(AT45DBX_SPI, b);
  gl_ptr_mem++;

  // If end of page reached,
  if (!Rd_bitfield(gl_ptr_mem, AT45DBX_MSK_PTR_BYTE))
  {
    // unselect the DF memory gl_ptr_mem points to in order to program the page.
    at45dbx_chipselect_df(gl_ptr_mem >> AT45DBX_MEM_SIZE, FALSE);

    // Memory busy.
    at45dbx_busy = TRUE;
  }

  return OK;
}


//! @}


/*! \name Multiple-Sector Access Functions
 */
//! @{


Bool at45dbx_read_multiple_sector(U16 nb_sector)
{
  while (nb_sector--)
  {
    // Read the next sector.
    at45dbx_read_sector_2_ram(sector_buf);
    at45dbx_read_multiple_sector_callback(sector_buf);
  }

  return OK;
}


Bool at45dbx_write_multiple_sector(U16 nb_sector)
{
  while (nb_sector--)
  {
    // Write the next sector.
    at45dbx_write_multiple_sector_callback(sector_buf);
    at45dbx_write_sector_from_ram(sector_buf);
  }

  return OK;
}


//! @}


/*! \name Single-Sector Access Functions
 */
//! @{


Bool at45dbx_read_sector_2_ram(void *ram)
{
  U8 *_ram = ram;
  U16 i;
  U16 data;

  // Memory busy.
  if (at45dbx_busy)
  {
    // Being here, we know that we previously finished a page read.
    // => We have to access the next page.

    // Memory ready.
    at45dbx_busy = FALSE;

    // Eventually select the next DF and open the next page.
    // NOTE: at45dbx_read_open input parameter is a sector.
    at45dbx_read_open(gl_ptr_mem >> AT45DBX_SECTOR_BITS); // gl_ptr_mem / AT45DBX_SECTOR_SIZE.
  }

  // Read the next sector.
  for (i = AT45DBX_SECTOR_SIZE; i; i--)
  {
    // Send a dummy byte to read the next data byte.
    spi_write_dummy();
    spi_read(AT45DBX_SPI, &data);
    *_ram++ = data;
  }

  // Update the memory pointer.
  gl_ptr_mem += AT45DBX_SECTOR_SIZE;

#if AT45DBX_PAGE_SIZE > AT45DBX_SECTOR_SIZE
  // If end of page reached,
  if (!Rd_bitfield(gl_ptr_mem, AT45DBX_MSK_PTR_BYTE))
#endif
  {
    // unselect the DF memory gl_ptr_mem points to.
    at45dbx_chipselect_df(gl_ptr_mem >> AT45DBX_MEM_SIZE, FALSE);

    // Memory busy.
    at45dbx_busy = TRUE;
  }

  return OK;
}


Bool at45dbx_write_sector_from_ram(const void *ram)
{
  const U8 *_ram = ram;
  U16 i;

  // Memory busy.
  if (at45dbx_busy)
  {
    // Being here, we know that we previously launched a page programming.
    // => We have to access the next page.

    // Eventually select the next DF and open the next page.
    // NOTE: at45dbx_write_open input parameter is a sector.
    at45dbx_write_open(gl_ptr_mem >> AT45DBX_SECTOR_BITS); // gl_ptr_mem / AT45DBX_SECTOR_SIZE.
  }

  // Write the next sector.
  for (i = AT45DBX_SECTOR_SIZE; i; i--)
  {
    // Write the next data byte.
    spi_write(AT45DBX_SPI, *_ram++);
  }

  // Update the memory pointer.
  gl_ptr_mem += AT45DBX_SECTOR_SIZE;

#if AT45DBX_PAGE_SIZE > AT45DBX_SECTOR_SIZE
  // If end of page reached,
  if (!Rd_bitfield(gl_ptr_mem, AT45DBX_MSK_PTR_BYTE))
#endif
  {
    // unselect the DF memory gl_ptr_mem points to in order to program the page.
    at45dbx_chipselect_df(gl_ptr_mem >> AT45DBX_MEM_SIZE, FALSE);

    // Memory busy.
    at45dbx_busy = TRUE;
  }

  return OK;
}


//! @}


#endif  // AT45DBX_MEM == ENABLE
