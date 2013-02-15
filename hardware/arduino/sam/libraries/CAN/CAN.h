/*
  Copyright (c) 2013 Arduino.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
  See the GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/


#ifndef _CAN_LIBRARY_
#define _CAN_LIBRARY_

#include "sn65hvd234.h"


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

class CANRaw
{
  protected:
    /* CAN peripheral, set by constructor */
    //Can* m_pCan ;

    /* CAN Transceiver */
    SSN65HVD234_Data m_Transceiver ;

    /** CAN Transfer */
    //can_mb_conf_t m_Mailbox;

  private:

  public:
    // Constructor
    //CANRawClass( Can* pCan ) ;

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

uint32_t set_baudrate(Can *p_can, uint32_t ul_mck, uint32_t ul_baudrate);
uint32_t init(Can *p_can, uint32_t ul_mck, uint32_t ul_baudrate);
void enable(Can *p_can);
void disable(Can *p_can);
void disable_low_power_mode(Can *p_can);
void enable_low_power_mode(Can *p_can);
void disable_autobaud_listen_mode(Can *p_can);
void enable_autobaud_listen_mode(Can *p_can);
void disable_overload_frame(Can *p_can);
void enable_overload_frame(Can *p_can);
void set_timestamp_capture_point(Can *p_can, uint32_t ul_flag);
void disable_time_triggered_mode(Can *p_can);
void enable_time_triggered_mode(Can *p_can);
void disable_timer_freeze(Can *p_can);
void enable_timer_freeze(Can *p_can);
void disable_tx_repeat(Can *p_can);
void enable_tx_repeat(Can *p_can);
void set_rx_sync_stage(Can *p_can, uint32_t ul_stage);
void enable_interrupt(Can *p_can, uint32_t dw_mask);
void disable_interrupt(Can *p_can, uint32_t dw_mask);
uint32_t get_interrupt_mask(Can *p_can);
uint32_t get_status(Can *p_can);
uint32_t get_internal_timer_value(Can *p_can);
uint32_t get_timestamp_value(Can *p_can);
uint8_t get_tx_error_cnt(Can *p_can);
uint8_t get_rx_error_cnt(Can *p_can);
void reset_internal_timer(Can *p_can);
void global_send_transfer_cmd(Can *p_can, uint8_t uc_mask);
void global_send_abort_cmd(Can *p_can, uint8_t uc_mask);
void mailbox_set_timemark(Can *p_can, uint8_t uc_index, uint16_t us_cnt);
uint32_t mailbox_get_status(Can *p_can, uint8_t uc_index);
void mailbox_send_transfer_cmd(Can *p_can, uint8_t uc_index);
void mailbox_send_abort_cmd(Can *p_can, uint8_t uc_index);
void mailbox_init(Can *p_can, can_mb_conf_t *p_mailbox);
uint32_t mailbox_read(Can *p_can, can_mb_conf_t *p_mailbox);
uint32_t mailbox_write(Can *p_can, can_mb_conf_t *p_mailbox);
uint32_t mailbox_tx_remote_frame(Can *p_can, can_mb_conf_t *p_mailbox);
void reset_all_mailbox(Can *p_can);
} ;


#endif // _CAN_LIBRARY_
