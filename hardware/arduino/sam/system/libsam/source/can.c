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

#include "../chip.h"
#include <string.h>

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

#if SAM3XA_SERIES


/** Define the timemark mask. */
#define TIMEMARK_MASK              0x0000ffff

/* CAN timeout for synchronization. */
#define CAN_TIMEOUT                100000

/** The max value for CAN baudrate prescale. */
#define CAN_BAUDRATE_MAX_DIV       128

/** Define the scope for TQ. */
#define CAN_MIN_TQ_NUM             8
#define CAN_MAX_TQ_NUM             25

/** Define the fixed bit time value. */
#define CAN_BIT_SYNC               1
#define CAN_BIT_IPT                2

typedef struct {
	uint8_t uc_tq;      //! CAN_BIT_SYNC + uc_prog + uc_phase1 + uc_phase2 = uc_tq, 8 <= uc_tq <= 25.
	uint8_t uc_prog;    //! Propagation segment, (3-bits + 1), 1~8;
	uint8_t uc_phase1;  //! Phase segment 1, (3-bits + 1), 1~8;
	uint8_t uc_phase2;  //! Phase segment 2, (3-bits + 1), 1~8, CAN_BIT_IPT <= uc_phase2;
	uint8_t uc_sjw;     //! Resynchronization jump width, (2-bits + 1), min(uc_phase1, 4);
	uint8_t uc_sp;      //! Sample point value, 0~100 in percent.
} can_bit_timing_t;

/** Values of bit time register for different baudrates, Sample point = ((1 + uc_prog + uc_phase1) / uc_tq) * 100%. */
const can_bit_timing_t can_bit_time[] = {
	{8,   (2 + 1), (1 + 1), (1 + 1), (2 + 1), 75},
	{9,   (1 + 1), (2 + 1), (2 + 1), (1 + 1), 67},
	{10,  (2 + 1), (2 + 1), (2 + 1), (2 + 1), 70},
	{11,  (3 + 1), (2 + 1), (2 + 1), (3 + 1), 72},
	{12,  (2 + 1), (3 + 1), (3 + 1), (3 + 1), 67},
	{13,  (3 + 1), (3 + 1), (3 + 1), (3 + 1), 77},
	{14,  (3 + 1), (3 + 1), (4 + 1), (3 + 1), 64},
	{15,  (3 + 1), (4 + 1), (4 + 1), (3 + 1), 67},
	{16,  (4 + 1), (4 + 1), (4 + 1), (3 + 1), 69},
	{17,  (5 + 1), (4 + 1), (4 + 1), (3 + 1), 71},
	{18,  (4 + 1), (5 + 1), (5 + 1), (3 + 1), 67},
	{19,  (5 + 1), (5 + 1), (5 + 1), (3 + 1), 68},
	{20,  (6 + 1), (5 + 1), (5 + 1), (3 + 1), 70},
	{21,  (7 + 1), (5 + 1), (5 + 1), (3 + 1), 71},
	{22,  (6 + 1), (6 + 1), (6 + 1), (3 + 1), 68},
	{23,  (7 + 1), (7 + 1), (6 + 1), (3 + 1), 70},
	{24,  (6 + 1), (7 + 1), (7 + 1), (3 + 1), 67},
	{25,  (7 + 1), (7 + 1), (7 + 1), (3 + 1), 68}
};

/**
 * \brief Configure CAN baudrate.
 *
 * \param p_can       Pointer to a CAN peripheral instance.
 * \param ul_mck      The input main clock for the CAN module.
 * \param ul_baudrate Baudrate value (kB/s), allowed values:
 *                    1000, 800, 500, 250, 125, 50, 25, 10, 5.
 *
 * \retval Set the baudrate successfully or not.
 */
static uint32_t can_set_baudrate(Can *p_can, uint32_t ul_mck, uint32_t ul_baudrate)
{
	uint8_t uc_tq;
	uint8_t uc_prescale;
	uint32_t ul_mod;
	uint32_t ul_cur_mod;
	can_bit_timing_t *p_bit_time;

	/* Check whether the baudrate prescale will be greater than the max divide value. */
	if (((ul_mck + (ul_baudrate * CAN_MAX_TQ_NUM * 1000 - 1)) /
		(ul_baudrate * CAN_MAX_TQ_NUM * 1000)) > CAN_BAUDRATE_MAX_DIV) {
		return 0;
	}

	/* Check whether the input MCK is too small. */
	if (ul_mck  < ul_baudrate * CAN_MIN_TQ_NUM * 1000) {
		return 0;
	}

	/* Initialize it as the minimum Time Quantum. */
	uc_tq = CAN_MIN_TQ_NUM;

	/* Initialize the remainder as the max value. When the remainder is 0, get the right TQ number. */
	ul_mod = 0xffffffff;
	/* Find out the approximate Time Quantum according to the baudrate. */
	for (uint8_t i = CAN_MIN_TQ_NUM; i <= CAN_MAX_TQ_NUM; i++) {
		if ((ul_mck / (ul_baudrate * i * 1000)) <= CAN_BAUDRATE_MAX_DIV) {
			ul_cur_mod = ul_mck % (ul_baudrate * i * 1000);
			if (ul_cur_mod < ul_mod){
				ul_mod = ul_cur_mod;
				uc_tq = i;
				if (!ul_mod) {
					break;
				}
			}
		}
	}

	/* Calculate the baudrate prescale value. */
	uc_prescale = ul_mck / (ul_baudrate * uc_tq * 1000);

	/* Get the right CAN BIT Timing group. */
	p_bit_time = (can_bit_timing_t *)&can_bit_time[uc_tq - CAN_MIN_TQ_NUM];

	/* Before modifying the CANBR register, disable the CAN controller. */
	can_disable(p_can);

	/* Write into the CAN baudrate register. */
	p_can->CAN_BR = CAN_BR_PHASE2(p_bit_time->uc_phase2 - 1) |
					CAN_BR_PHASE1(p_bit_time->uc_phase1 - 1) |
					CAN_BR_PROPAG(p_bit_time->uc_prog - 1) |
					CAN_BR_SJW(p_bit_time->uc_sjw - 1) |
					CAN_BR_BRP(uc_prescale - 1);
	return 1;
}


/**
 * \brief Initialize CAN controller.
 *
 * \param p_can  Pointer to a CAN peripheral instance.
 * \param ul_mck CAN module input clock.
 * \param ul_baudrate CAN communication baudrate in kbs.
 *
 * \retval 0 If failed to initialize the CAN module; otherwise successful.
 *
 * \note PMC clock for CAN peripheral should be enabled before calling this function.
 */
uint32_t can_init(Can *p_can, uint32_t ul_mck, uint32_t ul_baudrate)
{
	uint32_t ul_flag;
	uint32_t ul_tick;

	/* Initialize the baudrate for CAN module. */
	ul_flag = can_set_baudrate(p_can, ul_mck, ul_baudrate);
	if (ul_flag == 0) {
		return 0;
	}

	/* Reset the CAN eight message mailbox. */
	can_reset_all_mailbox(p_can);

	/* Enable the CAN controller. */
	can_enable(p_can);

	/* Wait until the CAN is synchronized with the bus activity. */
	ul_flag = 0;
	ul_tick = 0;
	while (!(ul_flag & CAN_SR_WAKEUP) && (ul_tick < CAN_TIMEOUT)) {
		ul_flag = can_get_status(p_can);
		ul_tick++;
	}

	/* Timeout or the CAN module has been synchronized with the bus. */
	if (CAN_TIMEOUT == ul_tick) {
		return 0;
	} else {
		return 1;
	}
}

/**
 * \brief Enable CAN Controller.
 *
 * \param p_can  Pointer to a CAN peripheral instance.
 */
void can_enable(Can *p_can)
{
	p_can->CAN_MR |= CAN_MR_CANEN;
}

/**
 * \brief Disable CAN Controller.
 *
 * \param p_can  Pointer to a CAN peripheral instance.
 */
void can_disable(Can *p_can)
{
	p_can->CAN_MR &= ~CAN_MR_CANEN;
}

/**
 * \brief Disable CAN Controller low power mode.
 *
 * \param p_can  Pointer to a CAN peripheral instance.
 */
void can_disable_low_power_mode(Can *p_can)
{
	p_can->CAN_MR &= ~CAN_MR_LPM;
}

/**
 * \brief Enable CAN Controller low power mode.
 *
 * \param p_can  Pointer to a CAN peripheral instance.
 */
void can_enable_low_power_mode(Can *p_can)
{
	p_can->CAN_MR |= CAN_MR_LPM;
}

/**
 * \brief Disable CAN Controller autobaud/listen mode.
 *
 * \param p_can  Pointer to a CAN peripheral instance.
 */
void can_disable_autobaud_listen_mode(Can *p_can)
{
	p_can->CAN_MR &= ~CAN_MR_ABM;
}

/**
 * \brief Enable CAN Controller autobaud/listen mode.
 *
 * \param p_can  Pointer to a CAN peripheral instance.
 */
void can_enable_autobaud_listen_mode(Can *p_can)
{
	p_can->CAN_MR |= CAN_MR_ABM;
}

/**
 * \brief CAN Controller won't generate overload frame.
 *
 * \param p_can  Pointer to a CAN peripheral instance.
 */
void can_disable_overload_frame(Can *p_can)
{
	p_can->CAN_MR &= ~CAN_MR_OVL;
}

/**
 * \brief CAN Controller will generate an overload frame after each successful
 * reception for mailboxes configured in Receive mode, Producer and Consumer.
 *
 * \param p_can  Pointer to a CAN peripheral instance.
 */
void can_enable_overload_frame(Can *p_can)
{
	p_can->CAN_MR |= CAN_MR_OVL;
}

/**
 * \brief Configure the timestamp capture point, at the start or the end of frame.
 *
 * \param p_can   Pointer to a CAN peripheral instance.
 * \param ul_flag 0: Timestamp is captured at each start of frame;
 *                1: Timestamp is captured at each end of frame.
 */
void can_set_timestamp_capture_point(Can *p_can, uint32_t ul_flag)
{
	if (ul_flag) {
		p_can->CAN_MR |= CAN_MR_TEOF;
	} else {
		p_can->CAN_MR &= ~CAN_MR_TEOF;
	}
}

/**
 * \brief Disable CAN Controller time triggered mode.
 *
 * \param p_can  Pointer to a CAN peripheral instance.
 */
void can_disable_time_triggered_mode(Can *p_can)
{
	p_can->CAN_MR &= ~CAN_MR_TTM;
}

/**
 * \brief Enable CAN Controller time triggered mode.
 *
 * \param p_can  Pointer to a CAN peripheral instance.
 */
void can_enable_time_triggered_mode(Can *p_can)
{
	p_can->CAN_MR |= CAN_MR_TTM;
}

/**
 * \brief Disable CAN Controller timer freeze.
 *
 * \param p_can  Pointer to a CAN peripheral instance.
 */
void can_disable_timer_freeze(Can *p_can)
{
	p_can->CAN_MR &= ~CAN_MR_TIMFRZ;
}

/**
 * \brief Enable CAN Controller timer freeze.
 *
 * \param p_can  Pointer to a CAN peripheral instance.
 */
void can_enable_timer_freeze(Can *p_can)
{
	p_can->CAN_MR |= CAN_MR_TIMFRZ;
}

/**
 * \brief Disable CAN Controller transmit repeat function.
 *
 * \param p_can  Pointer to a CAN peripheral instance.
 */
void can_disable_tx_repeat(Can *p_can)
{
	p_can->CAN_MR |= CAN_MR_DRPT;
}

/**
 * \brief Enable CAN Controller transmit repeat function.
 *
 * \param p_can  Pointer to a CAN peripheral instance.
 */
void can_enable_tx_repeat(Can *p_can)
{
	p_can->CAN_MR &= ~CAN_MR_DRPT;
}

/**
 * \brief Configure CAN Controller reception synchronization stage.
 *
 * \param p_can    Pointer to a CAN peripheral instance.
 * \param ul_stage The reception stage to be configured.
 *
 * \note This is just for debug purpose only.
 */
void can_set_rx_sync_stage(Can *p_can, uint32_t ul_stage)
{
	p_can->CAN_MR = (p_can->CAN_MR & ~CAN_MR_RXSYNC_Msk) | ul_stage;
}

/**
 * \brief Enable CAN interrupt.
 *
 * \param p_can  Pointer to a CAN peripheral instance.
 * \param dw_mask Interrupt to be enabled.
 */
void can_enable_interrupt(Can *p_can, uint32_t dw_mask)
{
	p_can->CAN_IER = dw_mask;
}

/**
 * \brief Disable CAN interrupt.
 *
 * \param p_can  Pointer to a CAN peripheral instance.
 * \param dw_mask Interrupt to be disabled.
 */
void can_disable_interrupt(Can *p_can, uint32_t dw_mask)
{
	p_can->CAN_IDR = dw_mask;
}

/**
 * \brief Get CAN Interrupt Mask.
 *
 * \param p_can Pointer to a CAN peripheral instance.
 *
 * \retval CAN interrupt mask.
 */
uint32_t can_get_interrupt_mask(Can *p_can)
{
	return (p_can->CAN_IMR);
}

/**
 * \brief Get CAN status.
 *
 * \param p_can Pointer to a CAN peripheral instance.
 *
 * \retval CAN status.
 */
uint32_t can_get_status(Can *p_can)
{
	return (p_can->CAN_SR);
}

/**
 * \brief Get the 16-bit free-running internal timer count.
 *
 * \param p_can Pointer to a CAN peripheral instance.
 *
 * \retval The internal CAN free-running timer counter.
 */
uint32_t can_get_internal_timer_value(Can *p_can)
{
	return (p_can->CAN_TIM);
}

/**
 * \brief Get CAN timestamp register value.
 *
 * \param p_can Pointer to a CAN peripheral instance.
 *
 * \retval The timestamp value.
 */
uint32_t can_get_timestamp_value(Can *p_can)
{
	return (p_can->CAN_TIMESTP);
}

/**
 * \brief Get CAN transmit error counter.
 *
 * \param p_can Pointer to a CAN peripheral instance.
 *
 * \retval Transmit error counter.
 */
uint8_t can_get_tx_error_cnt(Can *p_can)
{
	return (uint8_t) (p_can->CAN_ECR >> CAN_ECR_TEC_Pos);
}

/**
 * \brief Get CAN receive error counter.
 *
 * \param p_can Pointer to a CAN peripheral instance.
 *
 * \retval Receive error counter.
 */
uint8_t can_get_rx_error_cnt(Can *p_can)
{
	return (uint8_t) (p_can->CAN_ECR >> CAN_ECR_REC_Pos);
}

/**
 * \brief Reset the internal free-running 16-bit timer.
 *
 * \param p_can Pointer to a CAN peripheral instance.
 *
 * \note If the internal timer counter is frozen, this function automatically
 * re-enables it.
 */
void can_reset_internal_timer(Can *p_can)
{
	p_can->CAN_TCR |= CAN_TCR_TIMRST;
}

/**
 * \brief Send global transfer request.
 *
 * \param p_can   Pointer to a CAN peripheral instance.
 * \param uc_mask Mask for mailboxes that are requested to transfer.
 */
void can_global_send_transfer_cmd(Can *p_can, uint8_t uc_mask)
{
	uint32_t ul_reg;

	ul_reg = p_can->CAN_TCR & ((uint32_t)~GLOBAL_MAILBOX_MASK);
	p_can->CAN_TCR = ul_reg | uc_mask;
}

/**
 * \brief Send global abort request.
 *
 * \param p_can   Pointer to a CAN peripheral instance.
 * \param uc_mask Mask for mailboxes that are requested to abort.
 */
void can_global_send_abort_cmd(Can *p_can, uint8_t uc_mask)
{
	uint32_t ul_reg;

	ul_reg = p_can->CAN_ACR & ((uint32_t)~GLOBAL_MAILBOX_MASK);
	p_can->CAN_ACR = ul_reg | uc_mask;
}

/**
 * \brief Configure the timemark for the mailbox.
 *
 * \param p_can    Pointer to a CAN peripheral instance.
 * \param uc_index Indicate which mailbox is to be configured.
 * \param us_cnt   The timemark to be set.
 *
 * \note The timemark is active in Time Triggered mode only.
 */
void can_mailbox_set_timemark(Can *p_can, uint8_t uc_index, uint16_t us_cnt)
{
	uint32_t ul_reg;

	ul_reg = p_can->CAN_MB[uc_index].CAN_MMR & ((uint32_t)~TIMEMARK_MASK);
	p_can->CAN_MB[uc_index].CAN_MMR = ul_reg | us_cnt;
}

/**
 * \brief Get status of the mailbox.
 *
 * \param p_can    Pointer to a CAN peripheral instance.
 * \param uc_index Indicate which mailbox is to be read.
 *
 * \retval The mailbox status.
 */
uint32_t can_mailbox_get_status(Can *p_can, uint8_t uc_index)
{
	return (p_can->CAN_MB[uc_index].CAN_MSR);
}

/**
 * \brief Send single mailbox transfer request.
 *
 * \param p_can   Pointer to a CAN peripheral instance.
 * \param uc_index Indicate which mailbox is to be configured.
 */
void can_mailbox_send_transfer_cmd(Can *p_can, uint8_t uc_index)
{
	p_can->CAN_MB[uc_index].CAN_MCR |= CAN_MCR_MTCR;
}

/**
 * \brief Send single mailbox abort request.
 *
 * \param p_can    Pointer to a CAN peripheral instance.
 * \param uc_index Indicate which mailbox is to be configured.
 */
void can_mailbox_send_abort_cmd(Can *p_can, uint8_t uc_index)
{
	p_can->CAN_MB[uc_index].CAN_MCR |= CAN_MCR_MACR;
}

/**
 * \brief Initialize the mailbox in different mode and set up related configuration.
 *
 * \param p_can    Pointer to a CAN peripheral instance.
 * \param p_mailbox Pointer to a CAN mailbox instance.
 */
void can_mailbox_init(Can *p_can, can_mb_conf_t *p_mailbox)
{
	uint8_t uc_index;

	uc_index = (uint8_t)p_mailbox->ul_mb_idx;
	/* Check the object type of the mailbox. If it's used to disable the mailbox, reset the whole mailbox. */
	if (!p_mailbox->uc_obj_type) {
		p_can->CAN_MB[uc_index].CAN_MMR = 0;
		p_can->CAN_MB[uc_index].CAN_MAM = 0;
		p_can->CAN_MB[uc_index].CAN_MID = 0;
		p_can->CAN_MB[uc_index].CAN_MDL = 0;
		p_can->CAN_MB[uc_index].CAN_MDH = 0;
		p_can->CAN_MB[uc_index].CAN_MCR = 0;
		return;
	}

	/* Set the priority in Transmit mode. */
	p_can->CAN_MB[uc_index].CAN_MMR = (p_can->CAN_MB[uc_index].CAN_MMR &
		~CAN_MMR_PRIOR_Msk) | (p_mailbox-> uc_tx_prio << CAN_MMR_PRIOR_Pos);

	/* Set the message ID and message acceptance mask for the mailbox in other modes. */
	if (p_mailbox->uc_id_ver) {
		p_can->CAN_MB[uc_index].CAN_MAM = p_mailbox->ul_id_msk | CAN_MAM_MIDE;
		p_can->CAN_MB[uc_index].CAN_MID = p_mailbox->ul_id | CAN_MAM_MIDE;
	} else {
		p_can->CAN_MB[uc_index].CAN_MAM = p_mailbox->ul_id_msk;
		p_can->CAN_MB[uc_index].CAN_MID = p_mailbox->ul_id;
	}

	/* Set up mailbox in one of the five different modes. */
	p_can->CAN_MB[uc_index].CAN_MMR = (p_can->CAN_MB[uc_index].CAN_MMR &
		~CAN_MMR_MOT_Msk) | (p_mailbox-> uc_obj_type << CAN_MMR_MOT_Pos);
}

/**
 * \brief Read receive information for the mailbox.
 *
 * \param p_can     Pointer to a CAN peripheral instance.
 * \param p_mailbox Pointer to a CAN mailbox instance.
 *
 * \retval Different CAN mailbox transfer status.
 *
 * \note Read the mailbox status before calling this function.
 */
uint32_t can_mailbox_read(Can *p_can, can_mb_conf_t *p_mailbox)
{
	uint32_t ul_status;
	uint8_t uc_index;
	uint32_t ul_retval;

	ul_retval = 0;
	uc_index = (uint8_t)p_mailbox->ul_mb_idx;
	ul_status = p_mailbox->ul_status;

	/* Check whether there is overwriting happening in Receive with Overwrite mode,
	   or there're messages lost in Receive mode. */
	if ((ul_status & CAN_MSR_MRDY) && (ul_status & CAN_MSR_MMI)) {
		ul_retval = CAN_MAILBOX_RX_OVER;
	}

	/* Read the message family ID. */
	p_mailbox->ul_fid = p_can->CAN_MB[uc_index].CAN_MFID & CAN_MFID_MFID_Msk;

	/* Read received data length. */
	p_mailbox->uc_length = (ul_status & CAN_MSR_MDLC_Msk) >> CAN_MSR_MDLC_Pos;

	/* Read received data. */
	p_mailbox->ul_datal = p_can->CAN_MB[uc_index].CAN_MDL;
	if (p_mailbox->uc_length > 4) {
		p_mailbox->ul_datah = p_can->CAN_MB[uc_index].CAN_MDH;
	}

	/* Read the mailbox status again to check whether the software needs to re-read mailbox data register. */
	p_mailbox->ul_status = p_can->CAN_MB[uc_index].CAN_MSR;
	ul_status = p_mailbox->ul_status;
	if (ul_status & CAN_MSR_MMI) {
		ul_retval |= CAN_MAILBOX_RX_NEED_RD_AGAIN;
	} else {
		ul_retval |= CAN_MAILBOX_TRANSFER_OK;
	}

	/* Enable next receive process. */
	can_mailbox_send_transfer_cmd(p_can, uc_index);

	return ul_retval;
}

/**
 * \brief Prepare transmit information and write them into the mailbox.
 *
 * \param p_can     Pointer to a CAN peripheral instance.
 * \param p_mailbox Pointer to a CAN mailbox instance.
 *
 * \retval CAN_MAILBOX_NOT_READY: Failed because mailbox isn't ready.
 *       CAN_MAILBOX_TRANSFER_OK: Successfully write message into mailbox.
 *
 * \note After calling this function, the mailbox message won't be sent out until
 * can_mailbox_send_transfer_cmd() is called.
 */
uint32_t can_mailbox_write(Can *p_can, can_mb_conf_t *p_mailbox)
{
	uint32_t ul_status;
	uint8_t uc_index;

	uc_index = (uint8_t)p_mailbox->ul_mb_idx;
	/* Read the mailbox status firstly to check whether the mailbox is ready or not. */
	p_mailbox->ul_status = can_mailbox_get_status(p_can, uc_index);
	ul_status = p_mailbox->ul_status;
	if (!(ul_status & CAN_MSR_MRDY)) {
		return CAN_MAILBOX_NOT_READY;
	}

	/* Write transmit identifier. */
	if (p_mailbox->uc_id_ver) {
		p_can->CAN_MB[uc_index].CAN_MID = p_mailbox->ul_id | CAN_MAM_MIDE;
	} else {
		p_can->CAN_MB[uc_index].CAN_MID = p_mailbox->ul_id;
	}

	/* Write transmit data into mailbox data register. */
	p_can->CAN_MB[uc_index].CAN_MDL = p_mailbox->ul_datal;
	if (p_mailbox->uc_length > 4) {
		p_can->CAN_MB[uc_index].CAN_MDH = p_mailbox->ul_datah;
	}

	/* Write transmit data length into mailbox control register. */
	p_can->CAN_MB[uc_index].CAN_MCR = (p_can->CAN_MB[uc_index].CAN_MCR &
			~CAN_MCR_MDLC_Msk) | CAN_MCR_MDLC(p_mailbox->uc_length);

	return CAN_MAILBOX_TRANSFER_OK;
}

/**
 * \brief Require to send out a remote frame.
 *
 * \param p_can    Pointer to a CAN peripheral instance.
 * \param p_mailbox Pointer to a CAN mailbox instance.
 *
 * \retval CAN_MAILBOX_NOT_READY: Failed because mailbox isn't ready for transmitting message.
 *       CAN_MAILBOX_TRANSFER_OK: Successfully send out a remote frame.
 */
uint32_t can_mailbox_tx_remote_frame(Can *p_can, can_mb_conf_t *p_mailbox)
{
	uint32_t ul_status;
	uint8_t uc_index;

	uc_index = (uint8_t)p_mailbox->ul_mb_idx;
	/* Read the mailbox status firstly to check whether the mailbox is ready or not. */
	p_mailbox->ul_status = p_can->CAN_MB[uc_index].CAN_MSR;
	ul_status = p_mailbox->ul_status;
	if (!(ul_status & CAN_MSR_MRDY)) {
		return CAN_MAILBOX_NOT_READY;
	}

	/* Write transmit identifier. */
	if (p_mailbox->uc_id_ver) {
		p_can->CAN_MB[uc_index].CAN_MID = p_mailbox->ul_id | CAN_MAM_MIDE;
	} else {
		p_can->CAN_MB[uc_index].CAN_MID = p_mailbox->ul_id;
	}

	/* Set the RTR bit in the sent frame. */
	p_can->CAN_MB[uc_index].CAN_MCR |= CAN_MCR_MRTR;

	/* Set the MBx bit in the Transfer Command Register to send out the remote frame. */
	can_global_send_transfer_cmd(p_can, (1 << uc_index));

	return CAN_MAILBOX_TRANSFER_OK;
}

/**
 * \brief Reset the eight mailboxes.
 *
 * \param p_can Pointer to a CAN peripheral instance.
 */
void can_reset_all_mailbox(Can *p_can)
{
	can_mb_conf_t mb_config_t;

	/* Set the mailbox object type parameter to disable the mailbox. */
	mb_config_t.uc_obj_type = CAN_MB_DISABLE_MODE;

	for (uint8_t i = 0; i < CANMB_NUMBER; i++) {
		mb_config_t.ul_mb_idx = i;
		can_mailbox_init(p_can, &mb_config_t);
	}
}

// from wilfredo
uint32_t can_reset_mailbox_data(can_mb_conf_t *p_mailbox)
{
   if ( p_mailbox == NULL )
   {
     return 1U ;
   }

#if 0
	p_mailbox->ul_mb_idx = 0;
	p_mailbox->uc_obj_type = 0;
	p_mailbox->uc_id_ver = 0;
	p_mailbox->uc_length = 0;
	p_mailbox->uc_tx_prio = 0;
	p_mailbox->ul_status = 0;
	p_mailbox->ul_id_msk = 0;
	p_mailbox->ul_id = 0;
	p_mailbox->ul_fid = 0;
	p_mailbox->ul_datal = 0;
	p_mailbox->ul_datah = 0;
#else
  memset( p_mailbox, 0, sizeof( can_mb_conf_t ) ) ;
#endif

  return 0U ;
}

#endif // SAM3XA_SERIES


/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
