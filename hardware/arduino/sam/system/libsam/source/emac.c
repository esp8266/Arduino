 /**
 * \file
 *
 * \brief EMAC (Ethernet MAC) driver for SAM.
 *
 * Copyright (c) 2011-2012 Atmel Corporation. All rights reserved.
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

/**
 * \defgroup emac_group Ethernet Media Access Controller
 *
 * See \ref emac_quickstart.
 *
 * Driver for the EMAC (Ethernet Media Access Controller).
 * This file contains basic functions for the EMAC, with support for all modes, settings
 * and clock speeds.
 *
 * \section dependencies Dependencies
 * This driver does not depend on other modules.
 *
 * @{
 */

#define EMAC_RX_BUFFERS                               16
#define EMAC_TX_BUFFERS                               8
#define MAC_PHY_RETRY_MAX                             1000000


/** TX descriptor lists */
#ifdef __ICCARM__ /* IAR */
#pragma data_alignment=8
#endif
static emac_tx_descriptor_t gs_tx_desc[EMAC_TX_BUFFERS];
/** TX callback lists */
static emac_dev_tx_cb_t gs_tx_callback[EMAC_TX_BUFFERS];
/** RX descriptors lists */
#ifdef __ICCARM__ /* IAR */
#pragma data_alignment=8
#endif
static emac_rx_descriptor_t gs_rx_desc[EMAC_RX_BUFFERS];
/** Send Buffer. Section 3.6 of AMBA 2.0 spec states that burst should not cross the
 * 1K Boundaries. Receive buffer manager write operations are burst of 2 words => 3 lsb bits
 * of the address shall be set to 0.
 */
#ifdef __ICCARM__ /* IAR */
#pragma data_alignment=8
#endif
static uint8_t gs_uc_tx_buffer[EMAC_TX_BUFFERS * EMAC_TX_UNITSIZE]
		__attribute__ ((aligned(8)));

#ifdef __ICCARM__ /* IAR */
#pragma data_alignment=8
#endif
/** Receive Buffer */
static uint8_t gs_uc_rx_buffer[EMAC_RX_BUFFERS * EMAC_RX_UNITSIZE]
		__attribute__ ((aligned(8)));

/**
 * EMAC device memory management struct.
 */
typedef struct emac_dev_mem {
	/* Pointer to allocated buffer for RX. The address should be 8-byte aligned
	and the size should be EMAC_RX_UNITSIZE * wRxSize. */
	uint8_t *p_rx_buffer;
	/* Pointer to allocated RX descriptor list. */
	emac_rx_descriptor_t *p_rx_dscr;
	/* RX size, in number of registered units (RX descriptors). */
	uint16_t us_rx_size;
	/* Pointer to allocated buffer for TX. The address should be 8-byte aligned
	and the size should be EMAC_TX_UNITSIZE * wTxSize. */
	uint8_t *p_tx_buffer;
	/* Pointer to allocated TX descriptor list. */
	emac_tx_descriptor_t *p_tx_dscr;
	/* TX size, in number of registered units (TX descriptors). */
	uint16_t us_tx_size;
} emac_dev_mem_t;

/** Return count in buffer */
#define CIRC_CNT(head,tail,size) (((head) - (tail)) % (size))

/*
 * Return space available, from 0 to size-1.
 * Always leave one free char as a completely full buffer that has (head == tail),
 * which is the same as empty.
 */
#define CIRC_SPACE(head,tail,size) CIRC_CNT((tail),((head)+1),(size))

/** Circular buffer is empty ? */
#define CIRC_EMPTY(head, tail)     (head == tail)
/** Clear circular buffer */
#define CIRC_CLEAR(head, tail)     (head = tail = 0)

/** Increment head or tail */
static void circ_inc(uint16_t *headortail, uint32_t size)
{
        (*headortail)++;
        if((*headortail) >= size) {
            (*headortail) = 0;
        }
}

/**
 * \brief Wait PHY operation to be completed.
 *
 * \param p_emac HW controller address.
 * \param ul_retry The retry times, 0 to wait forever until completeness.
 *
 * Return EMAC_OK if the operation is completed successfully.
 */
static uint8_t emac_wait_phy(Emac* p_emac, const uint32_t ul_retry)
{
	volatile uint32_t ul_retry_count = 0;

	while (!emac_is_phy_idle(p_emac)) {
		if (ul_retry == 0) {
			continue;
		}

		ul_retry_count++;

		if (ul_retry_count >= ul_retry) {
			return EMAC_TIMEOUT;
		}
	}
	return EMAC_OK;
}

/**
 * \brief Disable transfer, reset registers and descriptor lists.
 *
 * \param p_dev Pointer to EMAC driver instance.
 *
 */
static void emac_reset_tx_mem(emac_device_t* p_dev)
{
	Emac *p_hw = p_dev->p_hw;
	uint8_t *p_tx_buff = p_dev->p_tx_buffer;
	emac_tx_descriptor_t *p_td = p_dev->p_tx_dscr;

	uint32_t ul_index;
	uint32_t ul_address;

	/* Disable TX */
	emac_enable_transmit(p_hw, 0);

	/* Set up the TX descriptors */
	CIRC_CLEAR(p_dev->us_tx_head, p_dev->us_tx_tail);
	for (ul_index = 0; ul_index < p_dev->us_tx_list_size; ul_index++) {
		ul_address = (uint32_t) (&(p_tx_buff[ul_index * EMAC_TX_UNITSIZE]));
		p_td[ul_index].addr = ul_address;
		p_td[ul_index].status.val = EMAC_TXD_USED;
	}
	p_td[p_dev->us_tx_list_size - 1].status.val =
			EMAC_TXD_USED | EMAC_TXD_WRAP;

	/* Set transmit buffer queue */
	emac_set_tx_queue(p_hw, (uint32_t) p_td);
}

/**
 * \brief Disable receiver, reset registers and descriptor list.
 *
 * \param p_drv Pointer to EMAC Driver instance.
 */
static void emac_reset_rx_mem(emac_device_t* p_dev)
{
	Emac *p_hw = p_dev->p_hw;
	uint8_t *p_rx_buff = p_dev->p_rx_buffer;
	emac_rx_descriptor_t *pRd = p_dev->p_rx_dscr;

	uint32_t ul_index;
	uint32_t ul_address;

	/* Disable RX */
	emac_enable_receive(p_hw, 0);

	/* Set up the RX descriptors */
	p_dev->us_rx_idx = 0;
	for (ul_index = 0; ul_index < p_dev->us_rx_list_size; ul_index++) {
		ul_address = (uint32_t) (&(p_rx_buff[ul_index * EMAC_RX_UNITSIZE]));
		pRd[ul_index].addr.val = ul_address & EMAC_RXD_ADDR_MASK;
		pRd[ul_index].status.val = 0;
	}
	pRd[p_dev->us_rx_list_size - 1].addr.val |= EMAC_RXD_WRAP;

	/* Set receive buffer queue */
	emac_set_rx_queue(p_hw, (uint32_t) pRd);
}


/**
 * \brief Initialize the allocated buffer lists for EMAC driver to transfer data.
 * Must be invoked after emac_dev_init() but before RX/TX starts.
 *
 * \note If input address is not 8-byte aligned, the address is automatically
 *       adjusted and the list size is reduced by one.
 *
 * \param p_emac Pointer to EMAC instance.
 * \param p_emac_dev Pointer to EMAC device instance.
 * \param p_dev_mm Pointer to the EMAC memory management control block.
 * \param p_tx_cb Pointer to allocated TX callback list.
 *
 * \return EMAC_OK or EMAC_PARAM.
 */
static uint8_t emac_init_mem(Emac* p_emac, emac_device_t* p_emac_dev,
		emac_dev_mem_t* p_dev_mm,
		emac_dev_tx_cb_t* p_tx_cb)
{
	if (p_dev_mm->us_rx_size <= 1 || p_dev_mm->us_tx_size <= 1 || p_tx_cb == NULL) {
		return EMAC_PARAM;
	}

	/* Assign RX buffers */
	if (((uint32_t) p_dev_mm->p_rx_buffer & 0x7)
			|| ((uint32_t) p_dev_mm->p_rx_dscr & 0x7)) {
		p_dev_mm->us_rx_size--;
	}
	p_emac_dev->p_rx_buffer =
			(uint8_t *) ((uint32_t) p_dev_mm->p_rx_buffer & 0xFFFFFFF8);
	p_emac_dev->p_rx_dscr =
			(emac_rx_descriptor_t *) ((uint32_t) p_dev_mm->p_rx_dscr
			& 0xFFFFFFF8);
	p_emac_dev->us_rx_list_size = p_dev_mm->us_rx_size;

	/* Assign TX buffers */
	if (((uint32_t) p_dev_mm->p_tx_buffer & 0x7)
			|| ((uint32_t) p_dev_mm->p_tx_dscr & 0x7)) {
		p_dev_mm->us_tx_size--;
	}
	p_emac_dev->p_tx_buffer =
			(uint8_t *) ((uint32_t) p_dev_mm->p_tx_buffer & 0xFFFFFFF8);
	p_emac_dev->p_tx_dscr =
			(emac_tx_descriptor_t *) ((uint32_t) p_dev_mm->p_tx_dscr
			& 0xFFFFFFF8);
	p_emac_dev->us_tx_list_size = p_dev_mm->us_tx_size;
	p_emac_dev->func_tx_cb_list = p_tx_cb;

	/* Reset TX & RX */
	emac_reset_rx_mem(p_emac_dev);
	emac_reset_tx_mem(p_emac_dev);

	/* Enable Rx and Tx, plus the statistics register */
	emac_enable_transmit(p_emac, 1);
	emac_enable_receive(p_emac, 1);
	emac_enable_statistics_write(p_emac, 1);

	/* Set up the interrupts for transmission and errors */
	emac_enable_interrupt(p_emac,
			EMAC_IER_RXUBR | /* Enable receive used bit read interrupt. */
			EMAC_IER_TUND  | /* Enable transmit underrun interrupt. */
			EMAC_IER_RLE   | /* Enable retry limit  exceeded interrupt. */
			EMAC_IER_TXERR | /* Enable transmit buffers exhausted in mid-frame interrupt. */
			EMAC_IER_TCOMP | /* Enable transmit complete interrupt. */
			EMAC_IER_ROVR  | /* Enable receive overrun interrupt. */
			EMAC_IER_HRESP | /* Enable Hresp not OK interrupt. */
			EMAC_IER_PFR   | /* Enable pause frame received interrupt. */
			EMAC_IER_PTZ);   /* Enable pause time zero interrupt. */

	return EMAC_OK;
}

/**
 * \brief Read the PHY register.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_phy_address PHY address.
 * \param uc_address Register address.
 * \param p_value Pointer to a 32-bit location to store read data.
 *
 * \Return EMAC_OK if successfully, EMAC_TIMEOUT if timeout.
 */
uint8_t emac_phy_read(Emac* p_emac, uint8_t uc_phy_address, uint8_t uc_address,
		uint32_t* p_value)
{
	emac_maintain_phy(p_emac, uc_phy_address, uc_address, 1, 0);

	if (emac_wait_phy(p_emac, MAC_PHY_RETRY_MAX) == EMAC_TIMEOUT) {
		return EMAC_TIMEOUT;
	}
	*p_value = emac_get_phy_data(p_emac);
	return EMAC_OK;
}

/**
 * \brief Write the PHY register.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_phy_address PHY Address.
 * \param uc_address Register Address.
 * \param ul_value Data to write, actually 16-bit data.
 *
 * \Return EMAC_OK if successfully, EMAC_TIMEOUT if timeout.
 */
uint8_t emac_phy_write(Emac* p_emac, uint8_t uc_phy_address,
		uint8_t uc_address, uint32_t ul_value)
{
	emac_maintain_phy(p_emac, uc_phy_address, uc_address, 0, ul_value);

	if (emac_wait_phy(p_emac, MAC_PHY_RETRY_MAX) == EMAC_TIMEOUT) {
		return EMAC_TIMEOUT;
	}
	return EMAC_OK;
}

/**
 * \brief Initialize the EMAC driver.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param p_emac_dev Pointer to the EMAC device instance.
 * \param p_opt EMAC configure options.
 */
void emac_dev_init(Emac* p_emac, emac_device_t* p_emac_dev,
		emac_options_t* p_opt)
{
	emac_dev_mem_t emac_dev_mm;

	/* Disable TX & RX and more */
	emac_network_control(p_emac, 0);
	emac_disable_interrupt(p_emac, ~0u);

	emac_clear_statistics(p_emac);

	/* Clear all status bits in the receive status register. */
	emac_clear_rx_status(p_emac, EMAC_RSR_OVR | EMAC_RSR_REC | EMAC_RSR_BNA);

	/* Clear all status bits in the transmit status register */
	emac_clear_tx_status(p_emac, EMAC_TSR_UBR | EMAC_TSR_COL | EMAC_TSR_RLES
			| EMAC_TSR_BEX | EMAC_TSR_COMP | EMAC_TSR_UND);

	/* Clear interrupts */
	emac_get_interrupt_status(p_emac);

	/* Enable the copy of data into the buffers
	   ignore broadcasts, and not copy FCS. */
	emac_set_configure(p_emac,
			emac_get_configure(p_emac) | EMAC_NCFGR_DRFCS | EMAC_NCFGR_PAE);

	emac_enable_copy_all(p_emac, p_opt->uc_copy_all_frame);
	emac_disable_broadcast(p_emac, p_opt->uc_no_boardcast);

	/* Fill in EMAC device memory management */
	emac_dev_mm.p_rx_buffer = gs_uc_rx_buffer;
	emac_dev_mm.p_rx_dscr = gs_rx_desc;
	emac_dev_mm.us_rx_size = EMAC_RX_BUFFERS;

	emac_dev_mm.p_tx_buffer = gs_uc_tx_buffer;
	emac_dev_mm.p_tx_dscr = gs_tx_desc;
	emac_dev_mm.us_tx_size = EMAC_TX_BUFFERS;

	emac_init_mem(p_emac, p_emac_dev, &emac_dev_mm, gs_tx_callback);

	emac_set_address(p_emac, 0, p_opt->uc_mac_addr);

}

/**
 * \brief Frames can be read from the EMAC in multiple sections.
 * Read ul_frame_size bytes from the EMAC receive buffers to pcTo.
 * p_rcv_size is the size of the entire frame.  Generally emac_read
 * will be repeatedly called until the sum of all the ul_frame_size equals
 * the value of p_rcv_size.
 *
 * \param p_emac_dev Pointer to the EMAC device instance.
 * \param p_frame Address of the frame buffer.
 * \param ul_frame_size  Length of the frame.
 * \param p_rcv_size   Received frame size.
 *
 * \return EMAC_OK if receiving frame successfully, otherwise failed.
 */
uint32_t emac_dev_read(emac_device_t* p_emac_dev, uint8_t* p_frame,
		uint32_t ul_frame_size, uint32_t* p_rcv_size)
{
	uint16_t us_buffer_length;
	uint32_t tmp_ul_frame_size = 0;
	uint8_t *p_tmp_frame = 0;
	uint16_t us_tmp_idx = p_emac_dev->us_rx_idx;
	emac_rx_descriptor_t *p_rx_td =
			&p_emac_dev->p_rx_dscr[p_emac_dev->us_rx_idx];
	int8_t c_is_frame = 0;

	if (p_frame == NULL)
		return EMAC_PARAM;

	/* Set the default return value */
	*p_rcv_size = 0;

	/* Process received RX descriptor */
	while ((p_rx_td->addr.val & EMAC_RXD_OWNERSHIP) == EMAC_RXD_OWNERSHIP) {
		/* A start of frame has been received, discard previous fragments */
		if ((p_rx_td->status.val & EMAC_RXD_SOF) == EMAC_RXD_SOF) {
			/* Skip previous fragment */
			while (us_tmp_idx != p_emac_dev->us_rx_idx) {
				p_rx_td = &p_emac_dev->p_rx_dscr[p_emac_dev->us_rx_idx];
				p_rx_td->addr.val &= ~(EMAC_RXD_OWNERSHIP);

				circ_inc(&p_emac_dev->us_rx_idx, p_emac_dev->us_rx_list_size);
			}
			/* Reset the temporary frame pointer */
			p_tmp_frame = p_frame;
			tmp_ul_frame_size = 0;
			/* Start to gather buffers in a frame */
			c_is_frame = 1;
		}

		/* Increment the pointer */
		circ_inc(&us_tmp_idx, p_emac_dev->us_rx_list_size);

		/* Copy data in the frame buffer */
		if (c_is_frame) {
			if (us_tmp_idx == p_emac_dev->us_rx_idx) {
				do {
					p_rx_td = &p_emac_dev->p_rx_dscr[p_emac_dev->us_rx_idx];
					p_rx_td->addr.val &= ~(EMAC_RXD_OWNERSHIP);
					circ_inc(&p_emac_dev->us_rx_idx, p_emac_dev->us_rx_list_size);

				} while (us_tmp_idx != p_emac_dev->us_rx_idx);

				return EMAC_RX_NULL;
			}
			/* Copy the buffer into the application frame */
			us_buffer_length = EMAC_RX_UNITSIZE;
			if ((tmp_ul_frame_size + us_buffer_length) > ul_frame_size) {
				us_buffer_length = ul_frame_size - tmp_ul_frame_size;
			}

			memcpy(p_tmp_frame,
					(void *)(p_rx_td->addr.val & EMAC_RXD_ADDR_MASK),
					us_buffer_length);
			p_tmp_frame += us_buffer_length;
			tmp_ul_frame_size += us_buffer_length;

			/* An end of frame has been received, return the data */
			if ((p_rx_td->status.val & EMAC_RXD_EOF) == EMAC_RXD_EOF) {
				/* Frame size from the EMAC */
				*p_rcv_size = (p_rx_td->status.val & EMAC_RXD_LEN_MASK);

				/* All data have been copied in the application frame buffer => release TD */
				while (p_emac_dev->us_rx_idx != us_tmp_idx) {
					p_rx_td = &p_emac_dev->p_rx_dscr[p_emac_dev->us_rx_idx];
					p_rx_td->addr.val &= ~(EMAC_RXD_OWNERSHIP);
					circ_inc(&p_emac_dev->us_rx_idx, p_emac_dev->us_rx_list_size);
				}

				/* Application frame buffer is too small so that all data have not been copied */
				if (tmp_ul_frame_size < *p_rcv_size) {
					return EMAC_SIZE_TOO_SMALL;
				}

				return EMAC_OK;
			}
		}
		/* SOF has not been detected, skip the fragment */
		else {
			p_rx_td->addr.val &= ~(EMAC_RXD_OWNERSHIP);
			p_emac_dev->us_rx_idx = us_tmp_idx;
		}

		/* Process the next buffer */
		p_rx_td = &p_emac_dev->p_rx_dscr[us_tmp_idx];
	}

	return EMAC_RX_NULL;
}

/**
 * \brief Send ulLength bytes from pcFrom. This copies the buffer to one of the
 * EMAC Tx buffers, and then indicates to the EMAC that the buffer is ready.
 * If lEndOfFrame is true then the data being copied is the end of the frame
 * and the frame can be transmitted.
 *
 * \param p_emac_dev Pointer to the EMAC device instance.
 * \param p_buffer       Pointer to the data buffer.
 * \param ul_size    Length of the frame.
 * \param func_tx_cb  Transmit callback function.
 *
 * \return Length sent.
 */
uint32_t emac_dev_write(emac_device_t* p_emac_dev, void *p_buffer,
		uint32_t ul_size, emac_dev_tx_cb_t func_tx_cb)
{

	volatile emac_tx_descriptor_t *p_tx_td;
	volatile emac_dev_tx_cb_t *p_func_tx_cb;

	Emac *p_hw = p_emac_dev->p_hw;


	/* Check parameter */
	if (ul_size > EMAC_TX_UNITSIZE) {
		return EMAC_PARAM;
	}

	/* Pointers to the current transmit descriptor */
	p_tx_td = &p_emac_dev->p_tx_dscr[p_emac_dev->us_tx_head];

	/* If no free TxTd, buffer can't be sent, schedule the wakeup callback */
	if (CIRC_SPACE(p_emac_dev->us_tx_head, p_emac_dev->us_tx_tail,
					p_emac_dev->us_tx_list_size) == 0) {
		return EMAC_TX_BUSY;
	}

	/* Pointers to the current Tx callback */
	p_func_tx_cb = &p_emac_dev->func_tx_cb_list[p_emac_dev->us_tx_head];

	/* Set up/copy data to transmission buffer */
	if (p_buffer && ul_size) {
		/* Driver manages the ring buffer */
		memcpy((void *)p_tx_td->addr, p_buffer, ul_size);
	}

	/* Tx callback */
	*p_func_tx_cb = func_tx_cb;

	/* Update transmit descriptor status */

	/* The buffer size defined is the length of ethernet frame,
	   so it's always the last buffer of the frame. */
	if (p_emac_dev->us_tx_head == p_emac_dev->us_tx_list_size - 1) {
		p_tx_td->status.val =
				(ul_size & EMAC_TXD_LEN_MASK) | EMAC_TXD_LAST
				| EMAC_TXD_WRAP;
	} else {
		p_tx_td->status.val =
				(ul_size & EMAC_TXD_LEN_MASK) | EMAC_TXD_LAST;
	}

	circ_inc(&p_emac_dev->us_tx_head, p_emac_dev->us_tx_list_size);

	/* Now start to transmit if it is still not done */
	emac_start_transmission(p_hw);

	return EMAC_OK;
}

/**
 * \brief Get current load of transmit.
 *
 * \param p_emac_dev Pointer to the EMAC device instance.
 *
 * \return Current load of transmit.
 */
uint32_t emac_dev_get_tx_load(emac_device_t* p_emac_dev)
{
	uint16_t us_head = p_emac_dev->us_tx_head;
	uint16_t us_tail = p_emac_dev->us_tx_tail;
	return CIRC_CNT(us_head, us_tail, p_emac_dev->us_tx_list_size);
}

/**
 * \brief Register/Clear RX callback. Callback will be invoked after the next received
 * frame.
 *
 * When emac_dev_read() returns EMAC_RX_NULL, the application task calls
 * emac_dev_set_rx_callback() to register func_rx_cb() callback and enters suspend state.
 * The callback is in charge to resume the task once a new frame has been
 * received. The next time emac_dev_read() is called, it will be successful.
 *
 * This function is usually invoked from the RX callback itself with NULL
 * callback, to unregister. Once the callback has resumed the application task,
 * there is no need to invoke the callback again.
 *
 * \param p_emac_dev Pointer to the EMAC device instance.
 * \param func_tx_cb  Receive callback function.
 */
void emac_dev_set_rx_callback(emac_device_t* p_emac_dev,
		emac_dev_tx_cb_t func_rx_cb)
{
	Emac *p_hw = p_emac_dev->p_hw;

	if (func_rx_cb == NULL) {
		emac_disable_interrupt(p_hw, EMAC_IDR_RCOMP);
		p_emac_dev->func_rx_cb = NULL;
	} else {
		p_emac_dev->func_rx_cb = func_rx_cb;
		emac_enable_interrupt(p_hw, EMAC_IER_RCOMP);
	}
}

/**
 *  \brief Register/Clear TX wakeup callback.
 *
 * When emac_dev_write() returns EMAC_TX_BUSY (all transmit descriptor busy), the application
 * task calls emac_dev_set_tx_wakeup_callback() to register func_wakeup() callback and
 * enters suspend state. The callback is in charge to resume the task once
 * several transmit descriptors have been released. The next time emac_dev_write() will be called,
 * it shall be successful.
 *
 * This function is usually invoked with NULL callback from the TX wakeup
 * callback itself, to unregister. Once the callback has resumed the
 * application task, there is no need to invoke the callback again.
 *
 * \param p_emac_dev   Pointer to EMAC device instance.
 * \param func_wakeup    Pointer to wakeup callback function.
 * \param uc_threshold Number of free transmit descriptor before wakeup callback invoked.
 *
 * \return EMAC_OK, EMAC_PARAM on parameter error.
 */
uint8_t emac_dev_set_tx_wakeup_callback(emac_device_t* p_emac_dev,
		emac_dev_wakeup_cb_t func_wakeup_cb, uint8_t uc_threshold)
{
	if (func_wakeup_cb == NULL) {
		p_emac_dev->func_wakeup_cb = NULL;
	} else {
		if (uc_threshold <= p_emac_dev->us_tx_list_size) {
			p_emac_dev->func_wakeup_cb = func_wakeup_cb;
			p_emac_dev->uc_wakeup_threshold = uc_threshold;
		} else {
			return EMAC_PARAM;
		}
	}

	return EMAC_OK;
}


/**
 * \brief Reset TX & RX queue & statistics.
 *
 * \param p_emac_dev   Pointer to EMAC device instance.
 */
void emac_dev_reset(emac_device_t* p_emac_dev)
{
	Emac *p_hw = p_emac_dev->p_hw;

	emac_reset_rx_mem(p_emac_dev);
	emac_reset_tx_mem(p_emac_dev);
	emac_network_control(p_hw, EMAC_NCR_TE | EMAC_NCR_RE
			| EMAC_NCR_WESTAT | EMAC_NCR_CLRSTAT);
}


/**
 * \brief EMAC Interrupt handler.
 *
 * \param p_emac_dev   Pointer to EMAC device instance.
 */
void emac_handler(emac_device_t* p_emac_dev)
{
	Emac *p_hw = p_emac_dev->p_hw;

	emac_tx_descriptor_t *p_tx_td;
	emac_dev_tx_cb_t *p_tx_cb;
	volatile uint32_t ul_isr;
	volatile uint32_t ul_rsr;
	volatile uint32_t ul_tsr;
	uint32_t ul_rx_status_flag;
	uint32_t ul_tx_status_flag;

	ul_isr = emac_get_interrupt_status(p_hw);
	ul_rsr = emac_get_rx_status(p_hw);
	ul_tsr = emac_get_tx_status(p_hw);

	ul_isr &= ~(emac_get_interrupt_mask(p_hw) | 0xFFC300);

	/* RX packet */
	if ((ul_isr & EMAC_ISR_RCOMP) || (ul_rsr & EMAC_RSR_REC)) {
		ul_rx_status_flag = EMAC_RSR_REC;

		/* Check OVR */
		if (ul_rsr & EMAC_RSR_OVR) {
			ul_rx_status_flag |= EMAC_RSR_OVR;
		}
		/* Check BNA */
		if (ul_rsr & EMAC_RSR_BNA) {
			ul_rx_status_flag |= EMAC_RSR_BNA;
		}
		/* Clear status */
		emac_clear_rx_status(p_hw, ul_rx_status_flag);

		/* Invoke callbacks */
		if (p_emac_dev->func_rx_cb) {
			p_emac_dev->func_rx_cb(ul_rx_status_flag);
		}
	}

	/* TX packet */
	if ((ul_isr & EMAC_ISR_TCOMP) || (ul_tsr & EMAC_TSR_COMP)) {

		ul_tx_status_flag = EMAC_TSR_COMP;

		/* A frame transmitted */

		/* Check RLE */
		if (ul_tsr & EMAC_TSR_RLES) {
			/* Status RLE & Number of discarded buffers */
			ul_tx_status_flag = EMAC_TSR_RLES | CIRC_CNT(p_emac_dev->us_tx_head,
					p_emac_dev->us_tx_tail, p_emac_dev->us_tx_list_size);
			p_tx_cb = &p_emac_dev->func_tx_cb_list[p_emac_dev->us_tx_tail];
			emac_reset_tx_mem(p_emac_dev);
			emac_enable_transmit(p_hw, 1);
		}
		/* Check COL */
		if (ul_tsr & EMAC_TSR_COL) {
			ul_tx_status_flag |= EMAC_TSR_COL;
		}
		/* Check BEX */
		if (ul_tsr & EMAC_TSR_BEX) {
			ul_tx_status_flag |= EMAC_TSR_BEX;
		}
		/* Check UND */
		if (ul_tsr & EMAC_TSR_UND) {
			ul_tx_status_flag |= EMAC_TSR_UND;
		}
		/* Clear status */
		emac_clear_tx_status(p_hw, ul_tx_status_flag);

		if (!CIRC_EMPTY(p_emac_dev->us_tx_head, p_emac_dev->us_tx_tail)) {
			/* Check the buffers */
			do {
				p_tx_td = &p_emac_dev->p_tx_dscr[p_emac_dev->us_tx_tail];
				p_tx_cb = &p_emac_dev->func_tx_cb_list[p_emac_dev->us_tx_tail];
				/* Any error? Exit if buffer has not been sent yet */
				if ((p_tx_td->status.val & EMAC_TXD_USED) == 0) {
					break;
				}

				/* Notify upper layer that a packet has been sent */
				if (*p_tx_cb) {
					(*p_tx_cb) (ul_tx_status_flag);
				}

				circ_inc(&p_emac_dev->us_tx_tail, p_emac_dev->us_tx_list_size);
			} while (CIRC_CNT(p_emac_dev->us_tx_head, p_emac_dev->us_tx_tail,
							p_emac_dev->us_tx_list_size));
		}

		if (ul_tsr & EMAC_TSR_RLES) {
			/* Notify upper layer RLE */
			if (*p_tx_cb) {
				(*p_tx_cb) (ul_tx_status_flag);
			}
		}

		/* If a wakeup has been scheduled, notify upper layer that it can
		   send other packets, and the sending will be successful. */
		if ((CIRC_SPACE(p_emac_dev->us_tx_head, p_emac_dev->us_tx_tail,
				p_emac_dev->us_tx_list_size) >= p_emac_dev->uc_wakeup_threshold)
				&& p_emac_dev->func_wakeup_cb) {
			p_emac_dev->func_wakeup_cb();
		}
	}
}

//@}

#endif // SAM3XA_SERIES

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond
