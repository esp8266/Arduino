/* ----------------------------------------------------------------------------
 *         SAM Software Package License
 * ----------------------------------------------------------------------------
 * Copyright (c) 2011-2012, Atmel Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following condition is met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the disclaimer below.
 *
 * Atmel's name may not be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * DISCLAIMER: THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * ----------------------------------------------------------------------------
 */

#ifndef UOTGHS_DEVICE_H_INCLUDED
#define UOTGHS_DEVICE_H_INCLUDED


#define MAX_ENDPOINTS	10
#define EP0				0
#define EP0_SIZE		64
#define EPX_SIZE		512

#define EP_SINGLE_64 (0x32UL) // EP0
#define EP_DOUBLE_64 (0x36UL) // Other endpoints

// Control Endpoint
#define EP_TYPE_CONTROL				(UOTGHS_DEVEPTCFG_EPSIZE_64_BYTE | \
									UOTGHS_DEVEPTCFG_EPTYPE_CTRL |     \
									UOTGHS_DEVEPTCFG_EPBK_1_BANK |     \
									UOTGHS_DEVEPTCFG_NBTRANS_1_TRANS | \
									UOTGHS_DEVEPTCFG_ALLOC)

// CDC Endpoints
#define EP_TYPE_BULK_IN				(UOTGHS_DEVEPTCFG_EPSIZE_512_BYTE | \
									UOTGHS_DEVEPTCFG_EPDIR_IN |         \
									UOTGHS_DEVEPTCFG_EPTYPE_BLK |       \
									UOTGHS_DEVEPTCFG_EPBK_2_BANK |      \
									UOTGHS_DEVEPTCFG_NBTRANS_1_TRANS |  \
									UOTGHS_DEVEPTCFG_ALLOC)

#define EP_TYPE_BULK_OUT          	(UOTGHS_DEVEPTCFG_EPSIZE_512_BYTE | \
									UOTGHS_DEVEPTCFG_EPTYPE_BLK |       \
									UOTGHS_DEVEPTCFG_EPBK_2_BANK |      \
									UOTGHS_DEVEPTCFG_NBTRANS_1_TRANS |  \
									UOTGHS_DEVEPTCFG_ALLOC)

#define EP_TYPE_INTERRUPT_IN      	(UOTGHS_DEVEPTCFG_EPSIZE_64_BYTE | \
									UOTGHS_DEVEPTCFG_EPDIR_IN |        \
									UOTGHS_DEVEPTCFG_EPTYPE_INTRPT |   \
									UOTGHS_DEVEPTCFG_EPBK_2_BANK |     \
									UOTGHS_DEVEPTCFG_NBTRANS_1_TRANS | \
									UOTGHS_DEVEPTCFG_ALLOC)

// HID Endpoints
#define EP_TYPE_INTERRUPT_IN_HID	(UOTGHS_DEVEPTCFG_EPSIZE_64_BYTE | \
									UOTGHS_DEVEPTCFG_EPDIR_IN |        \
									UOTGHS_DEVEPTCFG_EPTYPE_INTRPT |   \
									UOTGHS_DEVEPTCFG_EPBK_2_BANK |     \
									UOTGHS_DEVEPTCFG_NBTRANS_1_TRANS | \
									UOTGHS_DEVEPTCFG_ALLOC)

// Various definitions
#define EP_TYPE_INTERRUPT_OUT		(UOTGHS_DEVEPTCFG_EPSIZE_64_BYTE | \
									UOTGHS_DEVEPTCFG_EPTYPE_INTRPT |   \
									UOTGHS_DEVEPTCFG_EPTYPE_INTRPT |   \
									UOTGHS_DEVEPTCFG_EPBK_1_BANK |     \
									UOTGHS_DEVEPTCFG_NBTRANS_1_TRANS | \
									UOTGHS_DEVEPTCFG_ALLOC)

#define EP_TYPE_ISOCHRONOUS_IN		(UOTGHS_DEVEPTCFG_EPSIZE_1024_BYTE | \
									UOTGHS_DEVEPTCFG_EPDIR_IN |          \
									UOTGHS_DEVEPTCFG_EPTYPE_ISO |        \
									UOTGHS_DEVEPTCFG_EPBK_3_BANK |       \
									UOTGHS_DEVEPTCFG_NBTRANS_3_TRANS |   \
									UOTGHS_DEVEPTCFG_ALLOC)

#define EP_TYPE_ISOCHRONOUS_OUT		(UOTGHS_DEVEPTCFG_EPSIZE_1024_BYTE | \
									UOTGHS_DEVEPTCFG_EPTYPE_ISO |        \
									UOTGHS_DEVEPTCFG_EPBK_3_BANK |       \
									UOTGHS_DEVEPTCFG_NBTRANS_3_TRANS |   \
									UOTGHS_DEVEPTCFG_ALLOC)

//! \ingroup usb_device_group
//! \defgroup udd_group USB Device Driver (UDD)
//! UOTGHS low-level driver for USB device mode
//!
//! @{

#ifndef UOTGHS_DEVEPTCFG_EPDIR_Pos
// Bit pos is not defined in SAM header file but we need it.
# define UOTGHS_DEVEPTCFG_EPDIR_Pos 8
#endif

//! @name UOTGHS Device IP properties
//! These macros give access to IP properties
//! @{
  //! Get maximal number of endpoints
#define udd_get_endpoint_max_nbr()             (9)
#define UDD_MAX_PEP_NB                         (udd_get_endpoint_max_nbr() + 1)
  //! Get maximal number of banks of endpoints
#define udd_get_endpoint_bank_max_nbr(ep)      ((ep == 0) ? 1 : (( ep <= 2) ? 3 : 2))
  //! Get maximal size of endpoint (3X, 1024/64)
#define udd_get_endpoint_size_max(ep)          (((ep) == 0) ? 64 : 512)  // for bulk
  //! Get DMA support of endpoints
#define Is_udd_endpoint_dma_supported(ep)      ((((ep) >= 1) && ((ep) <= 6)) ? true : false)
  //! Get High Band Width support of endpoints
#define Is_udd_endpoint_high_bw_supported(ep)  (((ep) >= 2) ? true : false)
//! @}

//! @name UOTGHS Device speeds management
//! @{
  //! Enable/disable device low-speed mode
#define udd_low_speed_enable()               (Set_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_LS))
#define udd_low_speed_disable()              (Clr_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_LS))
  //! Test if device low-speed mode is forced
#define Is_udd_low_speed_enable()            (Tst_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_LS))

  //! Enable high speed mode
#define udd_high_speed_enable()          (Wr_bitfield(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_SPDCONF_Msk, 0))
  //! Disable high speed mode
#define udd_high_speed_disable()         (Wr_bitfield(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_SPDCONF_Msk, 3))
  //! Test if controller is in full speed mode
#define Is_udd_full_speed_mode()         (Rd_bitfield(UOTGHS->UOTGHS_SR, UOTGHS_SR_SPEED_Msk) == UOTGHS_SR_SPEED_FULL_SPEED)
//! @}

//! @name UOTGHS Device HS test mode management
//! @{
#ifdef UOTGHS_DEVCTRL_SPDCONF_HIGH_SPEED
  //! Enable high speed test mode
# define udd_enable_hs_test_mode()        (Wr_bitfield(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_SPDCONF_Msk, 2))
# define udd_enable_hs_test_mode_j()      (Set_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_TSTJ))
# define udd_enable_hs_test_mode_k()      (Set_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_TSTK))
# define udd_enable_hs_test_mode_packet() (Set_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_TSTPCKT))
#endif
//! @}

//! @name UOTGHS Device vbus management
//! @{
#define udd_enable_vbus_interrupt()       (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_VBUSTE))
#define udd_disable_vbus_interrupt()      (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_VBUSTE))
#define Is_udd_vbus_interrupt_enabled()   (Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_VBUSTE))
#define Is_udd_vbus_high()                (Tst_bits(UOTGHS->UOTGHS_SR, UOTGHS_SR_VBUS))
#define Is_udd_vbus_low()                 (!Is_udd_vbus_high())
#define udd_ack_vbus_transition()         (UOTGHS->UOTGHS_SCR = UOTGHS_SCR_VBUSTIC)
#define udd_raise_vbus_transition()       (UOTGHS->UOTGHS_SFR = UOTGHS_SFR_VBUSTIS)
#define Is_udd_vbus_transition()          (Tst_bits(UOTGHS->UOTGHS_SR, UOTGHS_SR_VBUSTI))
//! @}


//! @name UOTGHS device attach control
//! These macros manage the UOTGHS Device attach.
//! @{
  //! Detaches from USB bus
#define udd_detach_device()               (Set_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_DETACH))
  //! Attaches to USB bus
#define udd_attach_device()               (Clr_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_DETACH))
  //! Test if the device is detached
#define Is_udd_detached()                 (Tst_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_DETACH))
//! @}


//! @name UOTGHS device bus events control
//! These macros manage the UOTGHS Device bus events.
//! @{

//! Initiates a remote wake-up event
//! @{
#define udd_initiate_remote_wake_up()     (Set_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_RMWKUP))
#define Is_udd_pending_remote_wake_up()   (Tst_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_RMWKUP))
//! @}

//! Manage upstream resume event (=remote wakeup)
//! The USB driver sends a resume signal called "Upstream Resume"
//! @{
#define udd_enable_remote_wake_up_interrupt()     (UOTGHS->UOTGHS_DEVIER = UOTGHS_DEVIER_UPRSMES)
#define udd_disable_remote_wake_up_interrupt()    (UOTGHS->UOTGHS_DEVIDR = UOTGHS_DEVIDR_UPRSMEC)
#define Is_udd_remote_wake_up_interrupt_enabled() (Tst_bits(UOTGHS->UOTGHS_DEVIMR, UOTGHS_DEVIMR_UPRSME))
#define udd_ack_remote_wake_up_start()            (UOTGHS->UOTGHS_DEVICR = UOTGHS_DEVICR_UPRSMC)
#define udd_raise_remote_wake_up_start()          (UOTGHS->UOTGHS_DEVIFR = UOTGHS_DEVIFR_UPRSMS)
#define Is_udd_remote_wake_up_start()             (Tst_bits(UOTGHS->UOTGHS_DEVISR, UOTGHS_DEVISR_UPRSM))
//! @}

//! Manage downstream resume event (=remote wakeup from host)
//! The USB controller detects a valid "End of Resume" signal initiated by the host
//! @{
#define udd_enable_resume_interrupt()             (UOTGHS->UOTGHS_DEVIER = UOTGHS_DEVIER_EORSMES)
#define udd_disable_resume_interrupt()            (UOTGHS->UOTGHS_DEVIDR = UOTGHS_DEVIDR_EORSMEC)
#define Is_udd_resume_interrupt_enabled()         (Tst_bits(UOTGHS->UOTGHS_DEVIMR, UOTGHS_DEVIMR_EORSME))
#define udd_ack_resume()                          (UOTGHS->UOTGHS_DEVICR = UOTGHS_DEVICR_EORSMC)
#define udd_raise_resume()                        (UOTGHS->UOTGHS_DEVIFR = UOTGHS_DEVIFR_EORSMS)
#define Is_udd_resume()                           (Tst_bits(UOTGHS->UOTGHS_DEVISR, UOTGHS_DEVISR_EORSM))
//! @}

//! Manage wake-up event (=usb line activity)
//! The USB controller is reactivated by a filtered non-idle signal from the lines
//! @{
#define udd_enable_wake_up_interrupt()            (UOTGHS->UOTGHS_DEVIER = UOTGHS_DEVIER_WAKEUPES)
#define udd_disable_wake_up_interrupt()           (UOTGHS->UOTGHS_DEVIDR = UOTGHS_DEVIDR_WAKEUPEC)
#define Is_udd_wake_up_interrupt_enabled()        (Tst_bits(UOTGHS->UOTGHS_DEVIMR, UOTGHS_DEVIMR_WAKEUPE))
#define udd_ack_wake_up()                         (UOTGHS->UOTGHS_DEVICR = UOTGHS_DEVICR_WAKEUPC)
#define udd_raise_wake_up()                       (UOTGHS->UOTGHS_DEVIFR = UOTGHS_DEVIFR_WAKEUPS)
#define Is_udd_wake_up()                          (Tst_bits(UOTGHS->UOTGHS_DEVISR, UOTGHS_DEVISR_WAKEUP))
//! @}

//! Manage reset event
//! Set when a USB "End of Reset" has been detected
//! @{
#define udd_enable_reset_interrupt()              (UOTGHS->UOTGHS_DEVIER = UOTGHS_DEVIER_EORSTES)
#define udd_disable_reset_interrupt()             (UOTGHS->UOTGHS_DEVIDR = UOTGHS_DEVIDR_EORSTEC)
#define Is_udd_reset_interrupt_enabled()          (Tst_bits(UOTGHS->UOTGHS_DEVIMR, UOTGHS_DEVIMR_EORSTE))
#define udd_ack_reset()                           (UOTGHS->UOTGHS_DEVICR = UOTGHS_DEVICR_EORSTC)
#define udd_raise_reset()                         (UOTGHS->UOTGHS_DEVIFR = UOTGHS_DEVIFR_EORSTS)
#define Is_udd_reset()                            (Tst_bits(UOTGHS->UOTGHS_DEVISR, UOTGHS_DEVISR_EORST))
//! @}

//! Manage start of frame event
//! @{
#define udd_enable_sof_interrupt()                (UOTGHS->UOTGHS_DEVIER = UOTGHS_DEVIER_SOFES)
#define udd_disable_sof_interrupt()               (UOTGHS->UOTGHS_DEVIDR = UOTGHS_DEVIDR_SOFEC)
#define Is_udd_sof_interrupt_enabled()            (Tst_bits(UOTGHS->UOTGHS_DEVIMR, UOTGHS_DEVIMR_SOFE))
#define udd_ack_sof()                             (UOTGHS->UOTGHS_DEVICR = UOTGHS_DEVICR_SOFC)
#define udd_raise_sof()                           (UOTGHS->UOTGHS_DEVIFR = UOTGHS_DEVIFR_SOFS)
#define Is_udd_sof()                              (Tst_bits(UOTGHS->UOTGHS_DEVISR, UOTGHS_DEVISR_SOF))
#define udd_frame_number()                        (Rd_bitfield(UOTGHS->UOTGHS_DEVFNUM, UOTGHS_DEVFNUM_FNUM_Msk))
#define Is_udd_frame_number_crc_error()           (Tst_bits(UOTGHS->UOTGHS_DEVFNUM, UOTGHS_DEVFNUM_FNCERR))
//! @}

//! Manage Micro start of frame event (High Speed Only)
//! @{
#define udd_enable_msof_interrupt()               (UOTGHS->UOTGHS_DEVIER = UOTGHS_DEVIER_MSOFES)
#define udd_disable_msof_interrupt()              (UOTGHS->UOTGHS_DEVIDR = UOTGHS_DEVIDR_MSOFEC)
#define Is_udd_msof_interrupt_enabled()           (Tst_bits(UOTGHS->UOTGHS_DEVIMR, UOTGHS_DEVIMR_MSOFE))
#define udd_ack_msof()                            (UOTGHS->UOTGHS_DEVICR = UOTGHS_DEVIMR_MSOFE)
#define udd_raise_msof()                          (UOTGHS->UOTGHS_DEVIFR = UOTGHS_DEVIFR_MSOFS)
#define Is_udd_msof()                             (Tst_bits(UOTGHS->UOTGHS_DEVISR, UOTGHS_DEVISR_MSOF))
#define udd_micro_frame_number()                  \
	(Rd_bitfield(UOTGHS->UOTGHS_DEVFNUM, (UOTGHS_DEVFNUM_FNUM_Msk|UOTGHS_DEVFNUM_MFNUM_Msk)))
//! @}

//! Manage suspend event
//! @{
#define udd_enable_suspend_interrupt()            (UOTGHS->UOTGHS_DEVIER = UOTGHS_DEVIER_SUSPES)
#define udd_disable_suspend_interrupt()           (UOTGHS->UOTGHS_DEVIDR = UOTGHS_DEVIDR_SUSPEC)
#define Is_udd_suspend_interrupt_enabled()        (Tst_bits(UOTGHS->UOTGHS_DEVIMR, UOTGHS_DEVIMR_SUSPE))
#define udd_ack_suspend()                         (UOTGHS->UOTGHS_DEVICR = UOTGHS_DEVICR_SUSPC)
#define udd_raise_suspend()                       (UOTGHS->UOTGHS_DEVIFR = UOTGHS_DEVIFR_SUSPS)
#define Is_udd_suspend()                          (Tst_bits(UOTGHS->UOTGHS_DEVISR, UOTGHS_DEVISR_SUSP))
//! @}

//! @}

//! @name UOTGHS device address control
//! These macros manage the UOTGHS Device address.
//! @{
  //! enables USB device address
#define udd_enable_address()                      (Set_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_ADDEN))
  //! disables USB device address
#define udd_disable_address()                     (Clr_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_ADDEN))
#define Is_udd_address_enabled()                  (Tst_bits(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_ADDEN))
  //! configures the USB device address
#define udd_configure_address(addr)               (Wr_bitfield(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_UADD_Msk, addr))
  //! gets the currently configured USB device address
#define udd_get_configured_address()              (Rd_bitfield(UOTGHS->UOTGHS_DEVCTRL, UOTGHS_DEVCTRL_UADD_Msk))
//! @}


//! @name UOTGHS Device endpoint drivers
//! These macros manage the common features of the endpoints.
//! @{

//! Generic macro for UOTGHS registers that can be arrayed
//! @{
#define UOTGHS_ARRAY(reg,index)                   ((&(UOTGHS->reg))[(index)])
//! @}

//! @name UOTGHS Device endpoint configuration
//! @{
  //! enables the selected endpoint
#define udd_enable_endpoint(ep)                   (Set_bits(UOTGHS->UOTGHS_DEVEPT, UOTGHS_DEVEPT_EPEN0 << (ep)))
  //! disables the selected endpoint
#define udd_disable_endpoint(ep)                  (Clr_bits(UOTGHS->UOTGHS_DEVEPT, UOTGHS_DEVEPT_EPEN0 << (ep)))
  //! tests if the selected endpoint is enabled
#define Is_udd_endpoint_enabled(ep)               (Tst_bits(UOTGHS->UOTGHS_DEVEPT, UOTGHS_DEVEPT_EPEN0 << (ep)))
  //! resets the selected endpoint
#define udd_reset_endpoint(ep)                                         \
	do {                                                               \
		Set_bits(UOTGHS->UOTGHS_DEVEPT, UOTGHS_DEVEPT_EPRST0 << (ep)); \
		Clr_bits(UOTGHS->UOTGHS_DEVEPT, UOTGHS_DEVEPT_EPRST0 << (ep)); \
	} while (0)
  //! Tests if the selected endpoint is being reset
#define Is_udd_resetting_endpoint(ep)             (Tst_bits(UOTGHS->UOTGHS_DEVEPT, UOTGHS_DEVEPT_EPRST0 << (ep)))

  //! Configures the selected endpoint type
#define udd_configure_endpoint_type(ep, type)     (Wr_bitfield(UOTGHS_ARRAY(UOTGHS_DEVEPTCFG[0], ep), UOTGHS_DEVEPTCFG_EPTYPE_Msk, type))
  //! Gets the configured selected endpoint type
#define udd_get_endpoint_type(ep)                 (Rd_bitfield(UOTGHS_ARRAY(UOTGHS_DEVEPTCFG[0], ep), UOTGHS_DEVEPTCFG_EPTYPE_Msk))
  //! Enables the bank autoswitch for the selected endpoint
#define udd_enable_endpoint_bank_autoswitch(ep)   (Set_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTCFG[0], ep), UOTGHS_DEVEPTCFG_AUTOSW))
  //! Disables the bank autoswitch for the selected endpoint
#define udd_disable_endpoint_bank_autoswitch(ep)    (Clr_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTCFG[0], ep), UOTGHS_DEVEPTCFG_AUTOSW))
#define Is_udd_endpoint_bank_autoswitch_enabled(ep) (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTCFG[0], ep), UOTGHS_DEVEPTCFG_AUTOSW))
  //! Configures the selected endpoint direction
#define udd_configure_endpoint_direction(ep, dir) (Wr_bitfield(UOTGHS_ARRAY(UOTGHS_DEVEPTCFG[0], ep), UOTGHS_DEVEPTCFG_EPDIR, dir))
  //! Gets the configured selected endpoint direction
#define udd_get_endpoint_direction(ep)            (Rd_bitfield(UOTGHS_ARRAY(UOTGHS_DEVEPTCFG[0], ep), UOTGHS_DEVEPTCFG_EPDIR))
#define Is_udd_endpoint_in(ep)                    (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTCFG[0], ep), UOTGHS_DEVEPTCFG_EPDIR))
  //! Bounds given integer size to allowed range and rounds it up to the nearest
  //! available greater size, then applies register format of UOTGHS controller
  //! for endpoint size bit-field.
#define udd_format_endpoint_size(size)            (32 - clz(((uint32_t)min(max(size, 8), 1024) << 1) - 1) - 1 - 3)
  //! Configures the selected endpoint size
#define udd_configure_endpoint_size(ep, size)     (Wr_bitfield(UOTGHS_ARRAY(UOTGHS_DEVEPTCFG[0], ep), UOTGHS_DEVEPTCFG_EPSIZE_Msk, udd_format_endpoint_size(size)))
  //! Gets the configured selected endpoint size
#define udd_get_endpoint_size(ep)                 (8 << Rd_bitfield(UOTGHS_ARRAY(UOTGHS_DEVEPTCFG[0], ep), UOTGHS_DEVEPTCFG_EPSIZE_Msk))
  //! Configures the selected endpoint number of banks
#define udd_configure_endpoint_bank(ep, bank)     (Wr_bitfield(UOTGHS_ARRAY(UOTGHS_DEVEPTCFG[0], ep), UOTGHS_DEVEPTCFG_EPBK_Msk, bank))
  //! Gets the configured selected endpoint number of banks
#define udd_get_endpoint_bank(ep)                 (Rd_bitfield(UOTGHS_ARRAY(UOTGHS_DEVEPTCFG[0], ep), UOTGHS_DEVEPTCFG_EPBK_Msk)+1)
  //! Allocates the configuration selected endpoint in DPRAM memory
#define udd_allocate_memory(ep)                   (Set_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTCFG[0], ep), UOTGHS_DEVEPTCFG_ALLOC))
  //! un-allocates the configuration selected endpoint in DPRAM memory
#define udd_unallocate_memory(ep)                 (Clr_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTCFG[0], ep), UOTGHS_DEVEPTCFG_ALLOC))
#define Is_udd_memory_allocated(ep)               (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTCFG[0], ep), UOTGHS_DEVEPTCFG_ALLOC))

  //! Configures selected endpoint in one step
#define udd_configure_endpoint(ep, type, dir, size, bank) (\
	Wr_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTCFG[0], ep), UOTGHS_DEVEPTCFG_EPTYPE_Msk |\
			UOTGHS_DEVEPTCFG_EPDIR  |\
			UOTGHS_DEVEPTCFG_EPSIZE_Msk |\
			UOTGHS_DEVEPTCFG_EPBK_Msk ,   \
			(((uint32_t)(type) << UOTGHS_DEVEPTCFG_EPTYPE_Pos) & UOTGHS_DEVEPTCFG_EPTYPE_Msk) |\
			(((uint32_t)(dir ) << UOTGHS_DEVEPTCFG_EPDIR_Pos ) & UOTGHS_DEVEPTCFG_EPDIR) |\
			( (uint32_t)udd_format_endpoint_size(size) << UOTGHS_DEVEPTCFG_EPSIZE_Pos) |\
			(((uint32_t)(bank) << UOTGHS_DEVEPTCFG_EPBK_Pos) & UOTGHS_DEVEPTCFG_EPBK_Msk))\
)
  //! Tests if current endpoint is configured
#define Is_udd_endpoint_configured(ep)            (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTISR[0], ep), UOTGHS_DEVEPTISR_CFGOK))
  //! Returns the control direction
#define udd_control_direction()                   (Rd_bitfield(UOTGHS_ARRAY(UOTGHS_DEVEPTISR[0], EP_CONTROL), UOTGHS_DEVEPTISR_CTRLDIR))

  //! Resets the data toggle sequence
#define udd_reset_data_toggle(ep)                 (UOTGHS_ARRAY(UOTGHS_DEVEPTIER[0], ep) = UOTGHS_DEVEPTIER_RSTDTS)
  //! Tests if the data toggle sequence is being reset
#define Is_udd_data_toggle_reset(ep)              (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTIMR[0], ep), UOTGHS_DEVEPTIMR_RSTDT))
  //! Returns data toggle
#define udd_data_toggle(ep)                       (Rd_bitfield(UOTGHS_ARRAY(UOTGHS_DEVEPTISR[0], ep), UOTGHS_DEVEPTISR_DTSEQ_Msk))
//! @}


//! @name UOTGHS Device control endpoint
//! These macros control the endpoints.
//! @{

//! @name UOTGHS Device control endpoint interrupts
//! These macros control the endpoints interrupts.
//! @{
  //! Enables the selected endpoint interrupt
#define udd_enable_endpoint_interrupt(ep)         (UOTGHS->UOTGHS_DEVIER = UOTGHS_DEVIER_PEP_0 << (ep))
  //! Disables the selected endpoint interrupt
#define udd_disable_endpoint_interrupt(ep)        (UOTGHS->UOTGHS_DEVIDR = UOTGHS_DEVIDR_PEP_0 << (ep))
  //! Tests if the selected endpoint interrupt is enabled
#define Is_udd_endpoint_interrupt_enabled(ep)     (Tst_bits(UOTGHS->UOTGHS_DEVIMR, UOTGHS_DEVIMR_PEP_0 << (ep)))
  //! Tests if an interrupt is triggered by the selected endpoint
#define Is_udd_endpoint_interrupt(ep)             (Tst_bits(UOTGHS->UOTGHS_DEVISR, UOTGHS_DEVISR_PEP_0 << (ep)))
  //! Returns the lowest endpoint number generating an endpoint interrupt or MAX_PEP_NB if none
#define udd_get_interrupt_endpoint_number()       (ctz(((UOTGHS->UOTGHS_DEVISR >> UOTGHS_DEVISR_PEP_Pos) & \
                                                   (UOTGHS->UOTGHS_DEVIMR >> UOTGHS_DEVIMR_PEP_Pos)) |     \
                                                   (1 << MAX_PEP_NB)))
#define UOTGHS_DEVISR_PEP_Pos   12
#define UOTGHS_DEVIMR_PEP_Pos   12
//! @}

//! @name UOTGHS Device control endpoint errors
//! These macros control the endpoint errors.
//! @{
  //! Enables the STALL handshake
#define udd_enable_stall_handshake(ep)            (UOTGHS_ARRAY(UOTGHS_DEVEPTIER[0], ep) = UOTGHS_DEVEPTIER_STALLRQS)
  //! Disables the STALL handshake
#define udd_disable_stall_handshake(ep)           (UOTGHS_ARRAY(UOTGHS_DEVEPTIDR[0], ep) = UOTGHS_DEVEPTIDR_STALLRQC)
  //! Tests if STALL handshake request is running
#define Is_udd_endpoint_stall_requested(ep)       (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTIMR[0], ep), UOTGHS_DEVEPTIMR_STALLRQ))
  //! Tests if STALL sent
#define Is_udd_stall(ep)                          (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTISR[0], ep), UOTGHS_DEVEPTISR_STALLEDI))
  //! ACKs STALL sent
#define udd_ack_stall(ep)                         (UOTGHS_ARRAY(UOTGHS_DEVEPTICR[0], ep) = UOTGHS_DEVEPTICR_STALLEDIC)
  //! Raises STALL sent
#define udd_raise_stall(ep)                       (UOTGHS_ARRAY(UOTGHS_DEVEPTIFR[0], ep) = UOTGHS_DEVEPTIFR_STALLEDIS)
  //! Enables STALL sent interrupt
#define udd_enable_stall_interrupt(ep)            (UOTGHS_ARRAY(UOTGHS_DEVEPTIER[0], ep) = UOTGHS_DEVEPTIER_STALLEDES)
  //! Disables STALL sent interrupt
#define udd_disable_stall_interrupt(ep)           (UOTGHS_ARRAY(UOTGHS_DEVEPTIDR[0], ep) = UOTGHS_DEVEPTIDR_STALLEDEC)
  //! Tests if STALL sent interrupt is enabled
#define Is_udd_stall_interrupt_enabled(ep)        (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTIMR[0], ep), UOTGHS_DEVEPTIMR_STALLEDE))

  //! Tests if NAK OUT received
#define Is_udd_nak_out(ep)                        (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTISR[0], ep), UOTGHS_DEVEPTISR_NAKOUTI))
  //! ACKs NAK OUT received
#define udd_ack_nak_out(ep)                       (UOTGHS_ARRAY(UOTGHS_DEVEPTICR[0], ep) = UOTGHS_DEVEPTICR_NAKOUTIC)
  //! Raises NAK OUT received
#define udd_raise_nak_out(ep)                     (UOTGHS_ARRAY(UOTGHS_DEVEPTIFR[0], ep) = UOTGHS_DEVEPTIFR_NAKOUTIS)
  //! Enables NAK OUT interrupt
#define udd_enable_nak_out_interrupt(ep)          (UOTGHS_ARRAY(UOTGHS_DEVEPTIER[0], ep) = UOTGHS_DEVEPTIER_NAKOUTES)
  //! Disables NAK OUT interrupt
#define udd_disable_nak_out_interrupt(ep)         (UOTGHS_ARRAY(UOTGHS_DEVEPTIDR[0], ep) = UOTGHS_DEVEPTIDR_NAKOUTEC)
  //! Tests if NAK OUT interrupt is enabled
#define Is_udd_nak_out_interrupt_enabled(ep)      (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTIMR[0], ep), UOTGHS_DEVEPTIMR_NAKOUTE))

  //! Tests if NAK IN received
#define Is_udd_nak_in(ep)                         (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTISR[0], ep), UOTGHS_DEVEPTISR_NAKINI))
  //! ACKs NAK IN received
#define udd_ack_nak_in(ep)                        (UOTGHS_ARRAY(UOTGHS_DEVEPTICR[0], ep) = UOTGHS_DEVEPTICR_NAKINIC)
  //! Raises NAK IN received
#define udd_raise_nak_in(ep)                      (UOTGHS_ARRAY(UOTGHS_DEVEPTIFR[0], ep) = UOTGHS_DEVEPTIFR_NAKINIS)
  //! Enables NAK IN interrupt
#define udd_enable_nak_in_interrupt(ep)           (UOTGHS_ARRAY(UOTGHS_DEVEPTIER[0], ep) = UOTGHS_DEVEPTIER_NAKINES)
  //! Disables NAK IN interrupt
#define udd_disable_nak_in_interrupt(ep)          (UOTGHS_ARRAY(UOTGHS_DEVEPTIDR[0], ep) = UOTGHS_DEVEPTIDR_NAKINEC)
  //! Tests if NAK IN interrupt is enabled
#define Is_udd_nak_in_interrupt_enabled(ep)       (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTIMR[0], ep), UOTGHS_DEVEPTIMR_NAKINE))

  //! ACKs endpoint isochronous overflow interrupt
#define udd_ack_overflow_interrupt(ep)            (UOTGHS_ARRAY(UOTGHS_DEVEPTICR[0], ep) = UOTGHS_DEVEPTICR_OVERFIC)
  //! Raises endpoint isochronous overflow interrupt
#define udd_raise_overflow_interrupt(ep)          (UOTGHS_ARRAY(UOTGHS_DEVEPTIFR[0], ep) = UOTGHS_DEVEPTIFR_OVERFIS)
  //! Tests if an overflow occurs
#define Is_udd_overflow(ep)                       (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTISR[0], ep), UOTGHS_DEVEPTISR_OVERFI))
  //! Enables overflow interrupt
#define udd_enable_overflow_interrupt(ep)         (UOTGHS_ARRAY(UOTGHS_DEVEPTIER[0], ep) = UOTGHS_DEVEPTIER_OVERFES)
  //! Disables overflow interrupt
#define udd_disable_overflow_interrupt(ep)        (UOTGHS_ARRAY(UOTGHS_DEVEPTIDR[0], ep) = UOTGHS_DEVEPTIDR_OVERFEC)
  //! Tests if overflow interrupt is enabled
#define Is_udd_overflow_interrupt_enabled(ep)     (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTIMR[0], ep), UOTGHS_DEVEPTIMR_OVERFE))

  //! ACKs endpoint isochronous underflow interrupt
#define udd_ack_underflow_interrupt(ep)           (UOTGHS_ARRAY(UOTGHS_DEVEPTICR[0], ep) = UOTGHS_DEVEPTICR_UNDERFIC)
  //! Raises endpoint isochronous underflow interrupt
#define udd_raise_underflow_interrupt(ep)         (UOTGHS_ARRAY(UOTGHS_DEVEPTIFR[0], ep) = UOTGHS_DEVEPTIFR_UNDERFIS)
  //! Tests if an underflow occurs
#define Is_udd_underflow(ep)                      (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTISR[0], ep), UOTGHS_DEVEPTISR_UNDERFI))
  //! Enables underflow interrupt
#define udd_enable_underflow_interrupt(ep)        (UOTGHS_ARRAY(UOTGHS_DEVEPTIER[0], ep) = UOTGHS_DEVEPTIER_UNDERFES)
  //! Disables underflow interrupt
#define udd_disable_underflow_interrupt(ep)       (UOTGHS_ARRAY(UOTGHS_DEVEPTIDR[0], ep) = UOTGHS_DEVEPTIDR_UNDERFEC)
  //! Tests if underflow interrupt is enabled
#define Is_udd_underflow_interrupt_enabled(ep)    (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTIMR[0], ep), UOTGHS_DEVEPTIMR_UNDERFE))

  //! Tests if CRC ERROR ISO OUT detected
#define Is_udd_crc_error(ep)                      (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTISR[0], ep), UOTGHS_DEVEPTISR_CRCERRI))
  //! ACKs CRC ERROR ISO OUT detected
#define udd_ack_crc_error(ep)                     (UOTGHS_ARRAY(UOTGHS_DEVEPTICR[0], ep) = UOTGHS_DEVEPTICR_CRCERRIC)
  //! Raises CRC ERROR ISO OUT detected
#define udd_raise_crc_error(ep)                   (UOTGHS_ARRAY(UOTGHS_DEVEPTIFR[0], ep) = UOTGHS_DEVEPTIFR_CRCERRIS)
  //! Enables CRC ERROR ISO OUT detected interrupt
#define udd_enable_crc_error_interrupt(ep)        (UOTGHS_ARRAY(UOTGHS_DEVEPTIER[0], ep) = UOTGHS_DEVEPTIER_CRCERRES)
  //! Disables CRC ERROR ISO OUT detected interrupt
#define udd_disable_crc_error_interrupt(ep)       (UOTGHS_ARRAY(UOTGHS_DEVEPTIDR[0], ep) = UOTGHS_DEVEPTIDR_CRCERREC)
  //! Tests if CRC ERROR ISO OUT detected interrupt is enabled
#define Is_udd_crc_error_interrupt_enabled(ep)    (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTIMR[0], ep), UOTGHS_DEVEPTIMR_CRCERRE))
//! @}

//! @name UOTGHS Device control endpoint transfer
//! These macros control the endpoint transfer.
//! @{

  //! Tests if endpoint read allowed
#define Is_udd_read_enabled(ep)                   (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTISR[0], ep), UOTGHS_DEVEPTISR_RWALL))
  //! Tests if endpoint write allowed
#define Is_udd_write_enabled(ep)                  (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTISR[0], ep), UOTGHS_DEVEPTISR_RWALL))

  //! Returns the byte count
#define udd_byte_count(ep)                        (Rd_bitfield(UOTGHS_ARRAY(UOTGHS_DEVEPTISR[0], ep), UOTGHS_DEVEPTISR_BYCT_Msk))
  //! Clears FIFOCON bit
#define udd_ack_fifocon(ep)                       (UOTGHS_ARRAY(UOTGHS_DEVEPTIDR[0], ep) = UOTGHS_DEVEPTIDR_FIFOCONC)
  //! Tests if FIFOCON bit set
#define Is_udd_fifocon(ep)                        (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTIMR[0], ep), UOTGHS_DEVEPTIMR_FIFOCON))

  //! Returns the number of busy banks
#define udd_nb_busy_bank(ep)                      (Rd_bitfield(UOTGHS_ARRAY(UOTGHS_DEVEPTISR[0], ep), UOTGHS_DEVEPTISR_NBUSYBK_Msk))
  //! Returns the number of the current bank
#define udd_current_bank(ep)                      (Rd_bitfield(UOTGHS_ARRAY(UOTGHS_DEVEPTISR[0], ep), UOTGHS_DEVEPTISR_CURRBK_Msk))
  //! Kills last bank
#define udd_kill_last_in_bank(ep)                 (UOTGHS_ARRAY(UOTGHS_DEVEPTIER[0], ep) = UOTGHS_DEVEPTIER_KILLBKS)
#define Is_udd_kill_last(ep)                      (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTIMR[0], ep), UOTGHS_DEVEPTIMR_KILLBK))
  //! Tests if last bank killed
#define Is_udd_last_in_bank_killed(ep)            (!Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTIMR[0], ep), UOTGHS_DEVEPTIMR_KILLBK))
  //! Forces all banks full (OUT) or free (IN) interrupt
#define udd_force_bank_interrupt(ep)              (UOTGHS_ARRAY(UOTGHS_DEVEPTIFR[0], ep) = UOTGHS_DEVEPTIFR_NBUSYBKS)
  //! Unforces all banks full (OUT) or free (IN) interrupt
#define udd_unforce_bank_interrupt(ep)            (UOTGHS_ARRAY(UOTGHS_DEVEPTIFR[0], ep) = UOTGHS_DEVEPTIFR_NBUSYBKS)
  //! Enables all banks full (OUT) or free (IN) interrupt
#define udd_enable_bank_interrupt(ep)             (UOTGHS_ARRAY(UOTGHS_DEVEPTIER[0], ep) = UOTGHS_DEVEPTIER_NBUSYBKES)
  //! Disables all banks full (OUT) or free (IN) interrupt
#define udd_disable_bank_interrupt(ep)            (UOTGHS_ARRAY(UOTGHS_DEVEPTIDR[0], ep) = UOTGHS_DEVEPTIDR_NBUSYBKEC)
  //! Tests if all banks full (OUT) or free (IN) interrupt enabled
#define Is_udd_bank_interrupt_enabled(ep)         (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTIMR[0], ep), UOTGHS_DEVEPTIMR_NBUSYBKE))

  //! Tests if SHORT PACKET received
#define Is_udd_short_packet(ep)                   (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTISR[0], ep), UOTGHS_DEVEPTISR_SHORTPACKET))
  //! ACKs SHORT PACKET received
#define udd_ack_short_packet(ep)                  (UOTGHS_ARRAY(UOTGHS_DEVEPTICR[0], ep) = UOTGHS_DEVEPTICR_SHORTPACKETC)
  //! Raises SHORT PACKET received
#define udd_raise_short_packet(ep)                (UOTGHS_ARRAY(UOTGHS_DEVEPTIFR[0], ep) = UOTGHS_DEVEPTIFR_SHORTPACKETS)
  //! Enables SHORT PACKET received interrupt
#define udd_enable_short_packet_interrupt(ep)     (UOTGHS_ARRAY(UOTGHS_DEVEPTIER[0], ep) = UOTGHS_DEVEPTIER_SHORTPACKETES)
  //! Disables SHORT PACKET received interrupt
#define udd_disable_short_packet_interrupt(ep)    (UOTGHS_ARRAY(UOTGHS_DEVEPTIDR[0], ep) = UOTGHS_DEVEPTIDR_SHORTPACKETEC)
  //! Tests if SHORT PACKET received interrupt is enabled
#define Is_udd_short_packet_interrupt_enabled(ep) (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTIMR[0], ep), UOTGHS_DEVEPTIMR_SHORTPACKETE))

  //! Tests if SETUP received
#define Is_udd_setup_received(ep)                    (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTISR[0], ep), UOTGHS_DEVEPTISR_RXSTPI))
  //! ACKs SETUP received
#define udd_ack_setup_received(ep)                   (UOTGHS_ARRAY(UOTGHS_DEVEPTICR[0], ep) = UOTGHS_DEVEPTICR_RXSTPIC)
  //! Raises SETUP received
#define udd_raise_setup_received(ep)                 (UOTGHS_ARRAY(UOTGHS_DEVEPTIFR[0], ep) = UOTGHS_DEVEPTIFR_RXSTPIS)
  //! Enables SETUP received interrupt
#define udd_enable_setup_received_interrupt(ep)      (UOTGHS_ARRAY(UOTGHS_DEVEPTIER[0], ep) = UOTGHS_DEVEPTIER_RXSTPES)
  //! Disables SETUP received interrupt
#define udd_disable_setup_received_interrupt(ep)     (UOTGHS_ARRAY(UOTGHS_DEVEPTIDR[0], ep) = UOTGHS_DEVEPTIDR_RXSTPEC)
  //! Tests if SETUP received interrupt is enabled
#define Is_udd_setup_received_interrupt_enabled(ep)  (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTIMR[0], ep), UOTGHS_DEVEPTIMR_RXSTPE))

  //! Tests if OUT received
#define Is_udd_out_received(ep)                   (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTISR[0], ep), UOTGHS_DEVEPTISR_RXOUTI))
  //! ACKs OUT received
#define udd_ack_out_received(ep)                  (UOTGHS_ARRAY(UOTGHS_DEVEPTICR[0], ep) = UOTGHS_DEVEPTICR_RXOUTIC)
  //! Raises OUT received
#define udd_raise_out_received(ep)                (UOTGHS_ARRAY(UOTGHS_DEVEPTIFR[0], ep) = UOTGHS_DEVEPTIFR_RXOUTIS)
  //! Enables OUT received interrupt
#define udd_enable_out_received_interrupt(ep)     (UOTGHS_ARRAY(UOTGHS_DEVEPTIER[0], ep) = UOTGHS_DEVEPTIER_RXOUTES)
  //! Disables OUT received interrupt
#define udd_disable_out_received_interrupt(ep)    (UOTGHS_ARRAY(UOTGHS_DEVEPTIDR[0], ep) = UOTGHS_DEVEPTIDR_RXOUTEC)
  //! Tests if OUT received interrupt is enabled
#define Is_udd_out_received_interrupt_enabled(ep) (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTIMR[0], ep), UOTGHS_DEVEPTIMR_RXOUTE))

  //! Tests if IN sending
#define Is_udd_in_send(ep)                        (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTISR[0], ep), UOTGHS_DEVEPTISR_TXINI))
  //! ACKs IN sending
#define udd_ack_in_send(ep)                       (UOTGHS_ARRAY(UOTGHS_DEVEPTICR[0], ep) = UOTGHS_DEVEPTICR_TXINIC)
  //! Raises IN sending
#define udd_raise_in_send(ep)                     (UOTGHS_ARRAY(UOTGHS_DEVEPTIFR[0], ep) = UOTGHS_DEVEPTIFR_TXINIS)
  //! Enables IN sending interrupt
#define udd_enable_in_send_interrupt(ep)          (UOTGHS_ARRAY(UOTGHS_DEVEPTIER[0], ep) = UOTGHS_DEVEPTIER_TXINES)
  //! Disables IN sending interrupt
#define udd_disable_in_send_interrupt(ep)         (UOTGHS_ARRAY(UOTGHS_DEVEPTIDR[0], ep) = UOTGHS_DEVEPTIDR_TXINEC)
  //! Tests if IN sending interrupt is enabled
#define Is_udd_in_send_interrupt_enabled(ep)      (Tst_bits(UOTGHS_ARRAY(UOTGHS_DEVEPTIMR[0], ep), UOTGHS_DEVEPTIMR_TXINE))


  //! 8-bit access to FIFO data register of selected endpoint.
  //! @param ep Endpoint of which to access FIFO data register
  //! @return Volatile 8-bit data pointer to FIFO data register
  //! @warning It is up to the user of this macro to make sure that all accesses
  //! are aligned with their natural boundaries
  //! @warning It is up to the user of this macro to make sure that used HSB
  //! addresses are identical to the DPRAM internal pointer modulo 32 bits.
#define udd_get_endpoint_fifo_access8(ep) \
		(((volatile uint8_t (*)[0x8000])UOTGHS_RAM_ADDR)[(ep)])

//! @}

/*********************************************************************************************************************/

//! @name UOTGHS IP properties
//! These macros give access to IP properties (not defined in 3X)
//! @{
   //! Get IP name part 1 or 2
#define otg_get_ip_name()
#define otg_data_memory_barrier()
   //! Get IP version
#define otg_get_ip_version()
   //! Get DPRAM size (FIFO maximal size) in bytes
#define otg_get_dpram_size()
   //! Get size of USBB PB address space
#define otg_get_ip_paddress_size()
//! @}

//! @name UOTGHS OTG ID pin management
//! The ID pin come from the USB OTG connector (A and B receptable) and
//! allows to select the USB mode host or device.
//! The USBB hardware can manage it automaticaly. This feature is optional.
//! When otg_ID_PIN equals true in conf_usb_host.h, the USB_ID must be defined in board.h.
//!
//! @{
   //! PIO, PIO ID and MASK for USB_ID according to configuration from OTG_ID
#define OTG_ID_PIN                          USB_ID_GPIO
#define OTG_ID_FUNCTION                     USB_ID_FLAGS
   //! Input USB_ID from its pin
#define otg_input_id_pin() do {                     \
    pio_configure_pin(OTG_ID_PIN, OTG_ID_FUNCTION); \
} while (0)

   //! Enable external OTG_ID pin (listened to by USB)
#define otg_enable_id_pin()                 (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UIDE))
   //! Disable external OTG_ID pin (ignored by USB)
#define otg_disable_id_pin()                (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UIDE))
   //! Test if external OTG_ID pin enabled (listened to by USB)
#define Is_otg_id_pin_enabled()             (Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UIDE))
   //! Disable external OTG_ID pin and force device mode
#define otg_force_device_mode()             (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UIMOD), otg_disable_id_pin())
   //! Test if device mode is forced
#define Is_otg_device_mode_forced()         (!Is_otg_id_pin_enabled() && Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UIMOD))
   //! Disable external OTG_ID pin and force host mode
#define otg_force_host_mode()               (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UIMOD), otg_disable_id_pin())
   //! Test if host mode is forced
#define Is_otg_host_mode_forced()           (!Is_otg_id_pin_enabled() && !Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UIMOD))

//! @name UOTGHS OTG ID pin interrupt management
//! These macros manage the ID pin interrupt
//! @{
#define otg_enable_id_interrupt()           (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_IDTE))
#define otg_disable_id_interrupt()          (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_IDTE))
#define Is_otg_id_interrupt_enabled()       (Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_IDTE))
#define Is_otg_id_device()                  (Tst_bits(UOTGHS->UOTGHS_SR, UOTGHS_SR_ID))
#define Is_otg_id_host()                    (!Is_otg_id_device())
#define otg_ack_id_transition()             (UOTGHS->UOTGHS_SCR = UOTGHS_SCR_IDTIC)
#define otg_raise_id_transition()           (UOTGHS->UOTGHS_SFR = UOTGHS_SFR_IDTIS)
#define Is_otg_id_transition()              (Tst_bits(UOTGHS->UOTGHS_SR, UOTGHS_SR_IDTI))
//! @}

//! @name USBB OTG Vbus management
//! @{
#define otg_enable_vbus_interrupt()         (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_VBUSTE))
#define otg_disable_vbus_interrupt()        (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_VBUSTE))
#define Is_otg_vbus_interrupt_enabled()     (Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_VBUSTE))
#define Is_otg_vbus_high()                  (Tst_bits(UOTGHS->UOTGHS_SR, UOTGHS_SR_VBUS))
#define Is_otg_vbus_low()                   (!Is_otg_vbus_high())
#define otg_ack_vbus_transition()           (UOTGHS->UOTGHS_SCR = UOTGHS_SCR_VBUSTIC)
#define otg_raise_vbus_transition()         (UOTGHS->UOTGHS_SFR = UOTGHS_SFR_VBUSTIS)
#define Is_otg_vbus_transition()            (Tst_bits(UOTGHS->UOTGHS_SR, UOTGHS_SR_VBUSTI))
//! @}

//! @name UOTGHS OTG main management
//! These macros allows to enable/disable pad and UOTGHS hardware
//! @{
  //! Enable USB macro
#define otg_enable()                        (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_USBE))
  //! Disable USB macro
#define otg_disable()                       (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_USBE))
#define Is_otg_enabled()                    (Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_USBE))

  //! Enable OTG pad
#define otg_enable_pad()                    (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_OTGPADE))
  //! Disable OTG pad
#define otg_disable_pad()                   (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_OTGPADE))
#define Is_otg_pad_enabled()                (Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_OTGPADE))

  //! Check Clock Usable
  //! For parts with HS feature, this one corresponding at UTMI clock
#define Is_otg_clock_usable()               (Tst_bits(UOTGHS->UOTGHS_SR, UOTGHS_SR_CLKUSABLE))

  //! Stop (freeze) internal USB clock
#define otg_freeze_clock()                  (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_FRZCLK))
#define otg_unfreeze_clock()                (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_FRZCLK))
#define Is_otg_clock_frozen()               (Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_FRZCLK))

  //! Configure time-out of specified OTG timer
#define otg_configure_timeout(timer, timeout) (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UNLOCK),\
                                             Wr_bitfield(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_TIMPAGE_Msk, timer),\
                                             Wr_bitfield(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_TIMVALUE_Msk, timeout),\
                                             Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UNLOCK))
  //! Get configured time-out of specified OTG timer
#define otg_get_timeout(timer)              (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UNLOCK),\
                                             Wr_bitfield(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_TIMPAGE_Msk, timer),\
                                             Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_UNLOCK),\
                                             Rd_bitfield(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_TIMVALUE_Msk))


  //! Get the dual-role device state of the internal USB finite state machine of the UOTGHS controller
#define otg_get_fsm_drd_state()             (Rd_bitfield(UOTGHS->UOTGHS_FSM, UOTGHS_FSM_DRDSTATE_Msk))
//! @}

//! @name UOTGHS OTG hardware protocol
//! These macros manages the hardware OTG protocol
//! @{
  //! Initiates a Host Negociation Protocol
#define otg_device_initiate_hnp()             (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_HNPREQ))
  //! Accepts a Host Negociation Protocol
#define otg_host_accept_hnp()                 (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_HNPREQ))
  //! Rejects a Host Negociation Protocol
#define otg_host_reject_hnp()                 (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_HNPREQ))
  //! initiates a Session Request Protocol
#define otg_device_initiate_srp()             (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_SRPREQ))
  //! Selects VBus as SRP method
#define otg_select_vbus_srp_method()          (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_SRPSEL))
#define Is_otg_vbus_srp_method_selected()     (Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_SRPSEL))
  //! Selects data line as SRP method
#define otg_select_data_srp_method()          (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_SRPSEL))
#define Is_otg_data_srp_method_selected()     (!Is_otg_vbus_srp_method_selected())
  //! Tests if a HNP occurs
#define Is_otg_hnp()                          (Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_HNPREQ))
  //! Tests if a SRP from device occurs
#define Is_otg_device_srp()                   (Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_SRPREQ))

  //! Enables HNP error interrupt
#define otg_enable_hnp_error_interrupt()      (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_HNPERRE))
  //! Disables HNP error interrupt
#define otg_disable_hnp_error_interrupt()     (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_HNPERRE))
#define Is_otg_hnp_error_interrupt_enabled()  (Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_HNPERRE))
  //! ACKs HNP error interrupt
#define otg_ack_hnp_error_interrupt()         (UOTGHS->UOTGHS_SCR = UOTGHS_SCR_HNPERRIC)
  //! Raises HNP error interrupt
#define otg_raise_hnp_error_interrupt()       (UOTGHS->UOTGHS_SFR = UOTGHS_SFR_HNPERRIS)
  //! Tests if a HNP error occurs
#define Is_otg_hnp_error_interrupt()          (Tst_bits(UOTGHS->UOTGHS_SR, UOTGHS_SR_HNPERRI))

  //! Enables role exchange interrupt
#define otg_enable_role_exchange_interrupt()      (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_ROLEEXE))
  //! Disables role exchange interrupt
#define otg_disable_role_exchange_interrupt()     (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_ROLEEXE))
#define Is_otg_role_exchange_interrupt_enabled()  (Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_ROLEEXE))
  //! ACKs role exchange interrupt
#define otg_ack_role_exchange_interrupt()         (UOTGHS->UOTGHS_SCR = UOTGHS_SCR_ROLEEXIC)
  //! Raises role exchange interrupt
#define otg_raise_role_exchange_interrupt()       (UOTGHS->UOTGHS_SFR = UOTGHS_SFR_ROLEEXIS)
  //! Tests if a role exchange occurs
#define Is_otg_role_exchange_interrupt()          (Tst_bits(UOTGHS->UOTGHS_SR, UOTGHS_SR_ROLEEXI))

  //! Eenables SRP interrupt
#define otg_enable_srp_interrupt()          (Set_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_SRPE))
  //! Disables SRP interrupt
#define otg_disable_srp_interrupt()         (Clr_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_SRPE))
#define Is_otg_srp_interrupt_enabled()      (Tst_bits(UOTGHS->UOTGHS_CTRL, UOTGHS_CTRL_SRPE))
  //! ACKs SRP interrupt
#define otg_ack_srp_interrupt()             (UOTGHS->UOTGHS_SCR = UOTGHS_SCR_SRPIC)
  //! Raises SRP interrupt
#define otg_raise_srp_interrupt()           (UOTGHS->UOTGHS_SFR = UOTGHS_SFR_SRPIS)
  //! Tests if a SRP occurs
#define Is_otg_srp_interrupt()              (Tst_bits(UOTGHS->UOTGHS_SR, UOTGHS_SR_SRPI))
//! @}

//! @}


#endif /* UOTGHS_DEVICE_H_INCLUDED */
