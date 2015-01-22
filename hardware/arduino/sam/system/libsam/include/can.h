/**
 * \file
 *
 * \brief Controller Area Network (CAN) driver module for SAM.
 *
 * Copyright (c) 2011 - 2012 Atmel Corporation. All rights reserved.
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

#ifndef CAN_H_INCLUDED
#define CAN_H_INCLUDED

#include "../chip.h"

/** @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/** @endcond */

/** Define the Mailbox mask for eight mailboxes. */
#define GLOBAL_MAILBOX_MASK           0x000000ff

/** Disable all interrupt mask */
#define CAN_DISABLE_ALL_INTERRUPT_MASK 0xffffffff

/** Define the typical baudrate for CAN communication in KHz. */
#define CAN_BPS_1000K                 1000
#define CAN_BPS_800K                  800
#define CAN_BPS_500K                  500
#define CAN_BPS_250K                  250
#define CAN_BPS_125K                  125
#define CAN_BPS_50K                   50
#define CAN_BPS_25K                   25
#define CAN_BPS_10K                   10
#define CAN_BPS_5K                    5

/** Define the mailbox mode. */
#define CAN_MB_DISABLE_MODE           0
#define CAN_MB_RX_MODE                1
#define CAN_MB_RX_OVER_WR_MODE        2
#define CAN_MB_TX_MODE                3
#define CAN_MB_CONSUMER_MODE          4
#define CAN_MB_PRODUCER_MODE          5

/** Define CAN mailbox transfer status code. */
#define CAN_MAILBOX_TRANSFER_OK       0     //! Read from or write into mailbox successfully.
#define CAN_MAILBOX_NOT_READY         0x01  //! Receiver is empty or transmitter is busy.
#define CAN_MAILBOX_RX_OVER           0x02  //! Message overwriting happens or there're messages lost in different receive modes.
#define CAN_MAILBOX_RX_NEED_RD_AGAIN  0x04  //! Application needs to re-read the data register in Receive with Overwrite mode.

/** Define the struct for CAN message mailbox. */
typedef struct {
	uint32_t ul_mb_idx;
	uint8_t uc_obj_type;  //! Mailbox object type, one of the six different objects.
	uint8_t uc_id_ver;    //! 0 stands for standard frame, 1 stands for extended frame.
	uint8_t uc_length;    //! Received data length or transmitted data length.
	uint8_t uc_tx_prio;   //! Mailbox priority, no effect in receive mode.
	uint32_t ul_status;   //! Mailbox status register value.
	uint32_t ul_id_msk;   //! No effect in transmit mode.
	uint32_t ul_id;       //! Received frame ID or the frame ID to be transmitted.
	uint32_t ul_fid;      //! Family ID.
	uint32_t ul_datal;
	uint32_t ul_datah;
} can_mb_conf_t;

/**
 * \defgroup sam_driver_can_group Controller Area Network (CAN) Driver
 *
 * See \ref sam_can_quickstart.
 *
 * \par Purpose
 *
 * The CAN controller provides all the features required to implement
 * the serial communication protocol CAN defined by Robert Bosch GmbH,
 * the CAN specification. This is a driver for configuration, enabling,
 * disabling and use of the CAN peripheral.
 *
 * @{
 */

uint32_t can_init(Can *p_can, uint32_t ul_mck, uint32_t ul_baudrate);

void can_enable(Can *p_can);
void can_disable(Can *p_can);

void can_disable_low_power_mode(Can *p_can);
void can_enable_low_power_mode(Can *p_can);

void can_disable_autobaud_listen_mode(Can *p_can);
void can_enable_autobaud_listen_mode(Can *p_can);

void can_disable_overload_frame(Can *p_can);
void can_enable_overload_frame(Can *p_can);

void can_set_timestamp_capture_point(Can *p_can, uint32_t ul_flag);

void can_disable_time_triggered_mode(Can *p_can);
void can_enable_time_triggered_mode(Can *p_can);

void can_disable_timer_freeze(Can *p_can);
void can_enable_timer_freeze(Can *p_can);

void can_disable_tx_repeat(Can *p_can);
void can_enable_tx_repeat(Can *p_can);

void can_set_rx_sync_stage(Can *p_can, uint32_t ul_stage);

void can_enable_interrupt(Can *p_can, uint32_t dw_mask);
void can_disable_interrupt(Can *p_can, uint32_t dw_mask);

uint32_t can_get_interrupt_mask(Can *p_can);

uint32_t can_get_status(Can *p_can);

uint32_t can_get_internal_timer_value(Can *p_can);

uint32_t can_get_timestamp_value(Can *p_can);

uint8_t can_get_tx_error_cnt(Can *p_can);
uint8_t can_get_rx_error_cnt(Can *p_can);

void can_reset_internal_timer(Can *p_can);

void can_global_send_transfer_cmd(Can *p_can, uint8_t uc_mask);
void can_global_send_abort_cmd(Can *p_can, uint8_t uc_mask);

/*
 * Mailbox functions
 */
void can_mailbox_set_timemark(Can *p_can, uint8_t uc_index, uint16_t us_cnt);
uint32_t can_mailbox_get_status(Can *p_can, uint8_t uc_index);
void can_mailbox_send_transfer_cmd(Can *p_can, uint8_t uc_index);
void can_mailbox_send_abort_cmd(Can *p_can, uint8_t uc_index);
void can_mailbox_init(Can *p_can, can_mb_conf_t *p_mailbox);
uint32_t can_mailbox_read(Can *p_can, can_mb_conf_t *p_mailbox);
uint32_t can_mailbox_write(Can *p_can, can_mb_conf_t *p_mailbox);
uint32_t can_mailbox_tx_remote_frame(Can *p_can, can_mb_conf_t *p_mailbox);
void can_reset_all_mailbox(Can *p_can);

// from wilfredo
uint32_t can_reset_mailbox_data(can_mb_conf_t *p_mailbox);

/** @} */

/** @cond 0 */
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/** @endcond */

/**
 * \page sam_can_quickstart Quickstart guide for SAM CAN module.
 *
 * This is the quickstart guide for the \ref sam_drivers_can_group "SAM CAN module",
 * with step-by-step instructions on how to configure and use the drivers in a
 * selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 *
 * \section can_basic_use_case Basic use case
 * In this basic use case, as CAN module needs to work in network, two CAN modules
 * need to be configured. CAN0 mailbox 0 is configured as transmitter, and CAN1 mailbox 0
 * is configured as receiver. The communication baudrate is 1Mbit/s.
 *
 * \section can_basic_use_case_setup Setup steps
 *
 * \subsection can_basic_use_case_setup_prereq Prerequisites
 *  - \ref group_pmc "Power Management Controller driver"
 *  - \ref group_sn65hvd234_transceiver "CAN transceiver driver"
 *
 * \subsection can_basic_use_case_setup_code Example code
 * Add to application initialization:
 * \code
 *    can_mb_conf_t can0_mailbox;
 *    can_mb_conf_t can1_mailbox;
 *
 *    pmc_enable_periph_clk(ID_CAN0);
 *    pmc_enable_periph_clk(ID_CAN1);
 *
 *    can_init(CAN0, ul_sysclk, CAN_BPS_1000K);
 *    can_init(CAN1, ul_sysclk, CAN_BPS_1000K);
 *
 *    can_reset_all_mailbox(CAN0);
 *    can_reset_all_mailbox(CAN1);
 *
 *    can1_mailbox.ul_mb_idx = 0;
 *    can1_mailbox.uc_obj_type = CAN_MB_RX_MODE;
 *    can1_mailbox.ul_id_msk = CAN_MAM_MIDvA_Msk | CAN_MAM_MIDvB_Msk;
 *    can1_mailbox.ul_id = CAN_MID_MIDvA(0x07);
 *    can_mailbox_init(CAN1, &can1_mailbox);
 *
 *    can0_mailbox.ul_mb_idx = 0;
 *    can0_mailbox.uc_obj_type = CAN_MB_TX_MODE;
 *    can0_mailbox.uc_tx_prio = 15;
 *    can0_mailbox.uc_id_ver = 0;
 *    can0_mailbox.ul_id_msk = 0;
 *    can_mailbox_init(CAN0, &can0_mailbox);
 *
 *    can0_mailbox.ul_id = CAN_MID_MIDvA(0x07);
 *    can0_mailbox.ul_datal = 0x12345678;
 *    can0_mailbox.ul_datah = 0x87654321;
 *    can0_mailbox.uc_length = 8;
 *    can_mailbox_write(CAN0, &can0_mailbox);
 * \endcode
 *
 * \subsection can_basic_use_case_setup_flow Workflow
 * -# Define the CAN0 and CAN1 Transfer mailbox structure:
 *   - \code
 *    can_mb_conf_t can0_mailbox;
 *    can_mb_conf_t can1_mailbox;
 *   \endcode
 * -# Enable the module clock for CAN0 and CAN1:
 *   - \code
 *    pmc_enable_periph_clk(ID_CAN0);
 *    pmc_enable_periph_clk(ID_CAN1);
 *   \endcode
 * -# Initialize CAN0 and CAN1, baudrate is 1Mb/s:
 *   - \code
 *    can_init(CAN0, ul_sysclk, CAN_BPS_1000K);
 *    can_init(CAN1, ul_sysclk, CAN_BPS_1000K);
 *   \endcode
 *   - \note The CAN transceiver should be configured before initializing the CAN module.
 * -# Reset all CAN0 and CAN1 mailboxes:
 *   - \code
 *    can_reset_all_mailbox(CAN0);
 *    can_reset_all_mailbox(CAN1);
 *   \endcode
 * -# Initialize CAN1 mailbox 0 as receiver, frame ID is 0x07:
 *   - \code
 *    can1_mailbox.ul_mb_idx = 0;
 *    can1_mailbox.uc_obj_type = CAN_MB_RX_MODE;
 *    can1_mailbox.ul_id_msk = CAN_MAM_MIDvA_Msk | CAN_MAM_MIDvB_Msk;
 *    can1_mailbox.ul_id = CAN_MID_MIDvA(0x07);
 *    can_mailbox_init(CAN1, &can1_mailbox);
 *   \endcode
 * -# Initialize CAN0 mailbox 0 as transmitter, transmit priority is 15:
 *   - \code
 *    can0_mailbox.ul_mb_idx = 0;
 *    can0_mailbox.uc_obj_type = CAN_MB_TX_MODE;
 *    can0_mailbox.uc_tx_prio = 15;
 *    can0_mailbox.uc_id_ver = 0;
 *    can0_mailbox.ul_id_msk = 0;
 *    can_mailbox_init(CAN0, &can0_mailbox);
 *   \endcode
 * -# Prepare transmit ID, data and data length in CAN0 mailbox 0:
 *   - \code
 *    can0_mailbox.ul_id = CAN_MID_MIDvA(0x07);
 *    can0_mailbox.ul_datal = 0x12345678;
 *    can0_mailbox.ul_datah = 0x87654321;
 *    can0_mailbox.uc_length = 8;
 *    can_mailbox_write(CAN0, &can0_mailbox);
 *   \endcode
 *
 * \section can_basic_use_case_usage Usage steps
 *
 * \subsection can_basic_use_case_usage_code Example code
 * Add to, e.g., main loop in application C-file:
 * \code
 *    can_global_send_transfer_cmd(CAN0, CAN_TCR_MB0);
 *
 *    while (!(can_mailbox_get_status(CAN1, 0) & CAN_MSR_MRDY)) {
 *    }
 *
 *    can_mailbox_read(CAN1, &can1_mailbox);
 * \endcode
 *
 * \subsection can_basic_use_case_usage_flow Workflow
 * -# Send out data in CAN0 mailbox 0:
 *   - \code can_global_send_transfer_cmd(CAN0, CAN_TCR_MB0); \endcode
 * -# Wait for CAN1 mailbox 0 to receive the data:
 *   - \code
 *    while (!(can_mailbox_get_status(CAN1, 0) & CAN_MSR_MRDY)) {
 *    }
 *   \endcode
 * -# Read the received data from CAN1 mailbox 0:
 *   - \code can_mailbox_read(CAN1, &can1_mailbox); \endcode
 *
 * \section can_use_cases Advanced use cases
 * For more advanced use of the CAN driver, see the following use cases:
 * - \subpage can_use_case_1 : Two CAN modules work in PRODUCER and CONSUMER mode
 * respectively, use CAN interrupt handler to check whether the communication has been
 * completed.
 */

/**
 * \page can_use_case_1 Use case #1
 *
 * In this use case, CAN0 mailbox 0 works in PRODUCER mode, and CAN1 mailbox 0
 * works in CONSUMER mode. While CAN1 mailbox 0 receives a data frame from the bus,
 * an interrupt is triggered.
 *
 * \section can_use_case_1_setup Setup steps
 *
 * \subsection can_basic_use_case_setup_prereq Prerequisites
 *  - \ref group_pmc "Power Management Controller driver"
 *  - \ref group_sn65hvd234_transceiver "CAN transceiver driver"
 *
 * \subsection can_use_case_1_setup_code Example code
 * Add to application C-file:
 * \code
 *    can_mb_conf_t can0_mailbox;
 *    can_mb_conf_t can1_mailbox;
 *    volatile uint32_t g_ul_recv_status = 0;
 * \endcode
 *
 * \code
 *    void CAN1_Handler(void)
 *    {
 *        uint32_t ul_status;
 *
 *        ul_status = can_mailbox_get_status(CAN1, 0);
 *        if ((ul_status & CAN_MSR_MRDY) == CAN_MSR_MRDY) {
 *            can1_mailbox.ul_mb_idx = 0;
 *            can1_mailbox.ul_status = ul_status;
 *            can_mailbox_read(CAN1, &can1_mailbox);
 *            g_ul_recv_status = 1;
 *        }
 *    }
 * \endcode
 *
 * \code
 *    pmc_enable_periph_clk(ID_CAN0);
 *    pmc_enable_periph_clk(ID_CAN1);
 *
 *    can_init(CAN0, ul_sysclk, CAN_BPS_1000K);
 *    can_init(CAN1, ul_sysclk, CAN_BPS_1000K);
 *
 *    can_reset_all_mailbox(CAN0);
 *    can_reset_all_mailbox(CAN1);
 *
 *    can0_mailbox.ul_mb_idx = 0;
 *    can0_mailbox.uc_obj_type = CAN_MB_PRODUCER_MODE;
 *    can0_mailbox.ul_id_msk = 0;
 *    can0_mailbox.ul_id = CAN_MID_MIDvA(0x0b);
 *    can_mailbox_init(CAN0, &can0_mailbox);
 *
 *    can0_mailbox.ul_datal = 0x11223344;
 *    can0_mailbox.ul_datah = 0x44332211;
 *    can0_mailbox.uc_length = 8;
 *    can_mailbox_write(CAN0, &can0_mailbox);
 *
 *    can1_mailbox.ul_mb_idx = 0;
 *    can1_mailbox.uc_obj_type = CAN_MB_CONSUMER_MODE;
 *    can1_mailbox.uc_tx_prio = 15;
 *    can1_mailbox.ul_id_msk = CAN_MID_MIDvA_Msk | CAN_MID_MIDvB_Msk;
 *    can1_mailbox.ul_id = CAN_MID_MIDvA(0x0b);
 *    can_mailbox_init(CAN1, &can1_mailbox);
 *
 *    can_enable_interrupt(CAN1, CAN_IER_MB0);
 *    NVIC_EnableIRQ(CAN1_IRQn);
 * \endcode
 *
 * \subsection can_use_case_1_setup_flow Workflow
 * -# Define the CAN0 and CAN1 Transfer mailbox structure:
 *   - \code
 *    can_mb_conf_t can0_mailbox;
 *    can_mb_conf_t can1_mailbox;
 *   \endcode
 * -# Define the receive flag that is changed in CAN1 ISR handler:
 *   - \code volatile uint32_t g_ul_recv_status = 0; \endcode
 * -# Define the CAN1 ISR handler in the application:
 *   - \code void CAN1_Handler(void); \endcode
 * -# In CAN1_Handler(), get CAN1 mailbox 0 status:
 *   - \code ul_status = can_mailbox_get_status(CAN1, 0); \endcode
 * -# In CAN1_Handler(), check whether the mailbox 0 has received a data frame:
 *   - \code
 *    if ((ul_status & CAN_MSR_MRDY) == CAN_MSR_MRDY) {
 *        can1_mailbox.ul_mb_idx = 0;
 *        can1_mailbox.ul_status = ul_status;
 *        can_mailbox_read(CAN1, &can1_mailbox);
 *        g_ul_recv_status = 1;
 *    }
 *   \endcode
 * -# In CAN1_Handler(), if mailbox 0 is ready, read the received data from CAN1 mailbox 0:
 *   - \code
 *    can1_mailbox.ul_mb_idx = 0;
 *    can1_mailbox.ul_status = ul_status;
 *    can_mailbox_read(CAN1, &can1_mailbox);
 *   \endcode
 * -# In CAN1_Handler(), if mailbox 0 is ready, set up the receive flag:
 *   - \code g_ul_recv_status = 1; \endcode
 * -# Enable the module clock for CAN0 and CAN1:
 *   - \code
 *    pmc_enable_periph_clk(ID_CAN0);
 *    pmc_enable_periph_clk(ID_CAN1);
 *   \endcode
 * -# Initialize CAN0 and CAN1, baudrate is 1Mb/s:
 *   - \code
 *    can_init(CAN0, ul_sysclk, CAN_BPS_1000K);
 *    can_init(CAN1, ul_sysclk, CAN_BPS_1000K);
 *   \endcode
 *   - \note The CAN transceiver should be configured before initializing the CAN module.
 * -# Reset all CAN0 and CAN1 mailboxes:
 *   - \code
 *    can_reset_all_mailbox(CAN0);
 *    can_reset_all_mailbox(CAN1);
 *   \endcode
 * -# Initialize CAN0 mailbox 0 as PRODUCER:
 *   - \code
 *    can0_mailbox.ul_mb_idx = 0;
 *    can0_mailbox.uc_obj_type = CAN_MB_PRODUCER_MODE;
 *    can0_mailbox.ul_id_msk = 0;
 *    can0_mailbox.ul_id = CAN_MID_MIDvA(0x0b);
 *    can_mailbox_init(CAN0, &can0_mailbox);
 *   \endcode
 * -# Prepare the response information when it receives a remote frame:
 *   - \code
 *    can0_mailbox.ul_datal = 0x11223344;
 *    can0_mailbox.ul_datah = 0x44332211;
 *    can0_mailbox.uc_length = 8;
 *    can_mailbox_write(CAN0, &can0_mailbox);
 *   \endcode
 * -# Initialize CAN1 mailbox 0 as CONSUMER:
 *   - \code
 *    can1_mailbox.ul_mb_idx = 0;
 *    can1_mailbox.uc_obj_type = CAN_MB_CONSUMER_MODE;
 *    can1_mailbox.uc_tx_prio = 15;
 *    can1_mailbox.ul_id_msk = CAN_MID_MIDvA_Msk | CAN_MID_MIDvB_Msk;
 *    can1_mailbox.ul_id = CAN_MID_MIDvA(0x0b);
 *    can_mailbox_init(CAN1, &can1_mailbox);
 *   \endcode
 * -# Enable the CAN1 mailbox 0 interrupt:
 *   - \code
 *    can_enable_interrupt(CAN1, CAN_IER_MB0);
 *    NVIC_EnableIRQ(CAN1_IRQn);
 *   \endcode
 *
 * \section can_use_case_1_usage Usage steps
 *
 * \subsection can_use_case_1_usage_code Example code
 * \code
 *    can_global_send_transfer_cmd(CAN0, CAN_TCR_MB0);
 *    can_global_send_transfer_cmd(CAN1, CAN_TCR_MB0);
 *
 *    while (!g_ul_recv_status) {
 *    }
 * \endcode
 *
 * \subsection can_use_case_1_usage_flow Workflow
 * -# Enable CAN0 mailbox 0 to receive remote frame and respond it:
 *   - \code can_global_send_transfer_cmd(CAN0, CAN_TCR_MB0); \endcode
 * -# Enable CAN1 mailbox 0 to send out a remote frame and then receive data frame from bus:
 *   - \code can_global_send_transfer_cmd(CAN1, CAN_TCR_MB0); \endcode
 * -# Wait for the communication to be completed.
 *   - \code
 *    while (!g_ul_recv_status) {
 *    }
 *   \endcode
 */

#endif /* CAN_H_INCLUDED */
