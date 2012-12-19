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

#ifndef EMAC_H_INCLUDED
#define EMAC_H_INCLUDED

#include "../chip.h"
//#include "conf_eth.h"

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
extern "C" {
#endif
/**INDENT-ON**/
/// @endcond

/** The buffer addresses written into the descriptors must be aligned, so the
    last few bits are zero.  These bits have special meaning for the EMAC
    peripheral and cannot be used as part of the address. */
#define EMAC_RXD_ADDR_MASK      0xFFFFFFFC
#define EMAC_RXD_WRAP         (1ul << 1)  /**< Wrap bit */
#define EMAC_RXD_OWNERSHIP    (1ul << 0)  /**< Ownership bit */

#define EMAC_RXD_BROADCAST    (1ul << 31) /**< Broadcast detected */
#define EMAC_RXD_MULTIHASH    (1ul << 30) /**< Multicast hash match */
#define EMAC_RXD_UNIHASH      (1ul << 29) /**< Unicast hash match */
#define EMAC_RXD_EXTADDR      (1ul << 28) /**< External address match */
#define EMAC_RXD_ADDR1        (1ul << 26) /**< Address 1 match */
#define EMAC_RXD_ADDR2        (1ul << 25) /**< Address 2 match */
#define EMAC_RXD_ADDR3        (1ul << 24) /**< Address 3 match */
#define EMAC_RXD_ADDR4        (1ul << 23) /**< Address 4 match */
#define EMAC_RXD_TYPE         (1ul << 22) /**< Type ID match */
#define EMAC_RXD_VLAN         (1ul << 21) /**< VLAN tag detected */
#define EMAC_RXD_PRIORITY     (1ul << 20) /**< Priority tag detected */
#define EMAC_RXD_PRIORITY_MASK  (3ul << 17) /**< VLAN priority */
#define EMAC_RXD_CFI          (1ul << 16) /**< Concatenation Format Indicator only if bit 21 is set */
#define EMAC_RXD_EOF          (1ul << 15) /**< End of frame */
#define EMAC_RXD_SOF          (1ul << 14) /**< Start of frame */
#define EMAC_RXD_OFFSET_MASK                /**< Receive buffer offset */
#define EMAC_RXD_LEN_MASK       (0xFFF)     /**< Length of frame including FCS (if selected) */
#define EMAC_RXD_LENJUMBO_MASK  (0x3FFF)    /**< Jumbo frame length */

#define EMAC_TXD_USED         (1ul << 31) /**< Frame is transmitted */
#define EMAC_TXD_WRAP         (1ul << 30) /**< Last descriptor */
#define EMAC_TXD_ERROR        (1ul << 29) /**< Retry limit exceeded, error */
#define EMAC_TXD_UNDERRUN     (1ul << 28) /**< Transmit underrun */
#define EMAC_TXD_EXHAUSTED    (1ul << 27) /**< Buffer exhausted */
#define EMAC_TXD_NOCRC        (1ul << 16) /**< No CRC */
#define EMAC_TXD_LAST         (1ul << 15) /**< Last buffer in frame */
#define EMAC_TXD_LEN_MASK       (0x7FF)     /**< Length of buffer */

/** The MAC can support frame lengths up to 1536 bytes */
#define EMAC_FRAME_LENTGH_MAX       1536

#define EMAC_RX_UNITSIZE            128     /**< Fixed size for RX buffer  */
#define EMAC_TX_UNITSIZE            1518    /**< Size for ETH frame length */

/** EMAC clock speed */
#define EMAC_CLOCK_SPEED_160MHZ        (160*1000*1000)
#define EMAC_CLOCK_SPEED_80MHZ          (80*1000*1000)
#define EMAC_CLOCK_SPEED_40MHZ          (40*1000*1000)
#define EMAC_CLOCK_SPEED_20MHZ          (20*1000*1000)

/** EMAC maintain code default value*/
#define EMAC_MAN_CODE_VALUE    (10)

/** EMAC maintain start of frame default value*/
#define EMAC_MAN_SOF_VALUE     (1)

/** EMAC maintain read/write*/
#define EMAC_MAN_RW_TYPE       (2)

/** EMAC maintain read only*/
#define EMAC_MAN_READ_ONLY     (1)

/** EMAC address length */
#define EMAC_ADDR_LENGTH       (6)

/**
 * \brief Return codes for EMAC APIs.
 */
typedef enum {
	EMAC_OK = 0,         /** Operation OK */
	EMAC_TIMEOUT = 1,    /** EMAC operation timeout */
	EMAC_TX_BUSY,        /** TX in progress */
	EMAC_RX_NULL,        /** No data received */
	EMAC_SIZE_TOO_SMALL, /** Buffer size not enough */
	EMAC_PARAM,          /** Parameter error, TX packet invalid or RX size too small */
	EMAC_INVALID = 0xFF, /* Invalid */
} emac_status_t;

#if defined __ICCARM__
#pragma pack(4)    /* IAR */
#define __attribute__(...)    /* IAR */
#endif /* IAR */

/** Receive buffer descriptor struct */
typedef struct emac_rx_descriptor {
	union emac_rx_addr {
		uint32_t val;
		struct emac_rx_addr_bm {
			uint32_t b_ownership:1, /**< User clear, EMAC sets this to 1 once it has successfully written a frame to memory */
			b_wrap:1,   /**< Marks last descriptor in receive buffer */
			addr_dw:30; /**< Address in number of DW */
		} bm;
	} addr; /**< Address, Wrap & Ownership */
	union emac_rx_status {
		uint32_t val;
		struct emac_rx_status_bm {
			uint32_t len:12,       /** Length of frame including FCS */
			offset:2,              /** Receive buffer offset,  bits 13:12 of frame length for jumbo frame */
			b_sof:1,               /** Start of frame */
			b_eof:1,               /** End of frame */
			b_cfi:1,               /** Concatenation Format Indicator */
			vlan_priority:3,       /** VLAN priority (if VLAN detected) */
			b_priority_detected:1, /** Priority tag detected */
			b_vlan_detected:1,     /**< VLAN tag detected */
			b_type_id_match:1,     /**< Type ID match */
			b_addr4match:1,        /**< Address register 4 match */
			b_addr3match:1,        /**< Address register 3 match */
			b_addr2match:1,        /**< Address register 2 match */
			b_addr1match:1,        /**< Address register 1 match */
			reserved:1,
			b_ext_addr_match:1,    /**< External address match */
			b_uni_hash_match:1,    /**< Unicast hash match */
			b_multi_hash_match:1,  /**< Multicast hash match */
			b_boardcast_detect:1;  /**< Global broadcast address detected */
		} bm;
	} status;
} __attribute__ ((packed, aligned(8))) emac_rx_descriptor_t; /* GCC */

/** Transmit buffer descriptor struct */
typedef struct emac_tx_descriptor {
	uint32_t addr;
	union emac_tx_status {
		uint32_t val;
		struct emac_tx_status_bm {
			uint32_t len:11, /**< Length of buffer */
			reserved:4,
			b_last_buffer:1, /**< Last buffer (in the current frame) */
			b_no_crc:1,      /**< No CRC */
			reserved1:10,
			b_exhausted:1,   /**< Buffer exhausted in mid frame */
			b_underrun:1,    /**< Transmit underrun */
			b_error:1,       /**< Retry limit exceeded, error detected */
			b_wrap:1,        /**< Marks last descriptor in TD list */
			b_used:1;        /**< User clear, EMAC sets this to 1 once a frame has been successfully transmitted */
		} bm;
	} status;
} __attribute__ ((packed, aligned(8))) emac_tx_descriptor_t; /* GCC */

#ifdef __ICCARM__ /* IAR */
#pragma pack()
#endif

/**
 * \brief Input parameters when initializing the emac module mode.
 */
typedef struct emac_options {
	/*  Enable/Disable CopyAllFrame */
	uint8_t uc_copy_all_frame;
	/* Enable/Disable NoBroadCast */
	uint8_t uc_no_boardcast;
	/* MAC address */
	uint8_t uc_mac_addr[EMAC_ADDR_LENGTH];
} emac_options_t;

/** RX callback */
typedef void (*emac_dev_tx_cb_t) (uint32_t ul_status);
/** Wakeup callback */
typedef void (*emac_dev_wakeup_cb_t) (void);

/**
 * EMAC driver structure.
 */
typedef struct emac_device {

	/** Pointer to HW register base */
	Emac *p_hw;
	/**
	 * Pointer to allocated TX buffer.
	 * Section 3.6 of AMBA 2.0 spec states that burst should not cross
	 * 1K Boundaries.
	 * Receive buffer manager writes are burst of 2 words => 3 lsb bits
	 * of the address shall be set to 0.
	 */
	uint8_t *p_tx_buffer;
	/** Pointer to allocated RX buffer */
	uint8_t *p_rx_buffer;
	/** Pointer to Rx TDs (must be 8-byte aligned) */
	emac_rx_descriptor_t *p_rx_dscr;
	/** Pointer to Tx TDs (must be 8-byte aligned) */
	emac_tx_descriptor_t *p_tx_dscr;
	/** Optional callback to be invoked once a frame has been received */
	emac_dev_tx_cb_t func_rx_cb;
	/** Optional callback to be invoked once several TDs have been released */
	emac_dev_wakeup_cb_t func_wakeup_cb;
	/** Optional callback list to be invoked once TD has been processed */
	emac_dev_tx_cb_t *func_tx_cb_list;
	/** RX TD list size */
	uint16_t us_rx_list_size;
	/** RX index for current processing TD */
	uint16_t us_rx_idx;
	/** TX TD list size */
	uint16_t us_tx_list_size;
	/** Circular buffer head pointer by upper layer (buffer to be sent) */
	uint16_t us_tx_head;
	/** Circular buffer tail pointer incremented by handlers (buffer sent) */
	uint16_t us_tx_tail;

	/** Number of free TD before wakeup callback is invoked */
	uint8_t uc_wakeup_threshold;
} emac_device_t;

/**
 * \brief Write network control value.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param ul_ncr   Network control value.
 */
static inline void emac_network_control(Emac* p_emac, uint32_t ul_ncr)
{
	p_emac->EMAC_NCR = ul_ncr;
}

/**
 * \brief Get network control value.
 *
 * \param p_emac   Pointer to the EMAC instance.
 */

static inline uint32_t emac_get_network_control(Emac* p_emac)
{
	return p_emac->EMAC_NCR;
}

/**
 * \brief Enable/Disable EMAC receive.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_enable   0 to disable EMAC receiver, else to enable it.
 */
static inline void emac_enable_receive(Emac* p_emac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_emac->EMAC_NCR |= EMAC_NCR_RE;
	} else {
		p_emac->EMAC_NCR &= ~EMAC_NCR_RE;
	}
}

/**
 * \brief Enable/Disable EMAC transmit.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_enable   0 to disable EMAC transmit, else to enable it.
 */
static inline void emac_enable_transmit(Emac* p_emac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_emac->EMAC_NCR |= EMAC_NCR_TE;
	} else {
		p_emac->EMAC_NCR &= ~EMAC_NCR_TE;
	}
}

/**
 * \brief Enable/Disable EMAC management.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_enable   0 to disable EMAC management, else to enable it.
 */
static inline void emac_enable_management(Emac* p_emac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_emac->EMAC_NCR |= EMAC_NCR_MPE;
	} else {
		p_emac->EMAC_NCR &= ~EMAC_NCR_MPE;
	}
}

/**
 * \brief Clear all statistics registers.
 *
 * \param p_emac   Pointer to the EMAC instance.
 */
static inline void emac_clear_statistics(Emac* p_emac)
{
	p_emac->EMAC_NCR |= EMAC_NCR_CLRSTAT;
}

/**
 * \brief Increase all statistics registers.
 *
 * \param p_emac   Pointer to the EMAC instance.
 */
static inline void emac_increase_statistics(Emac* p_emac)
{
	p_emac->EMAC_NCR |= EMAC_NCR_INCSTAT;
}

/**
 * \brief Enable/Disable statistics registers writing.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_enable   0 to disable the statistics registers writing, else to enable it.
 */
static inline void emac_enable_statistics_write(Emac* p_emac,
		uint8_t uc_enable)
{
	if (uc_enable) {
		p_emac->EMAC_NCR |= EMAC_NCR_WESTAT;
	} else {
		p_emac->EMAC_NCR &= ~EMAC_NCR_WESTAT;
	}
}

/**
 * \brief In half-duplex mode, forces collisions on all received frames.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_enable   0 to disable the back pressure, else to enable it.
 */
static inline void emac_enable_back_pressure(Emac* p_emac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_emac->EMAC_NCR |= EMAC_NCR_BP;
	} else {
		p_emac->EMAC_NCR &= ~EMAC_NCR_BP;
	}
}

/**
 * \brief Start transmission.
 *
 * \param p_emac   Pointer to the EMAC instance.
 */
static inline void emac_start_transmission(Emac* p_emac)
{
	p_emac->EMAC_NCR |= EMAC_NCR_TSTART;
}

/**
 * \brief Halt transmission.
 *
 * \param p_emac   Pointer to the EMAC instance.
 */
static inline void emac_halt_transmission(Emac* p_emac)
{
	p_emac->EMAC_NCR |= EMAC_NCR_THALT;
}

/**
 * \brief Set up network configuration register.
 *
 * \param p_emac   Pointer to the EMAC instance.
  * \param ul_cfg   Network configuration value.
 */
static inline void emac_set_configure(Emac* p_emac, uint32_t ul_cfg)
{
	p_emac->EMAC_NCFGR = ul_cfg;
}

/**
 * \brief Get network configuration.
 *
 * \param p_emac   Pointer to the EMAC instance.
 *
 * \return Network configuration.
 */
static inline uint32_t emac_get_configure(Emac* p_emac)
{
	return p_emac->EMAC_NCFGR;
}

/**
 * \brief Set speed.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_speed 1 to indicate 100Mbps, 0 to 10Mbps.
 */
static inline void emac_set_speed(Emac* p_emac, uint8_t uc_speed)
{
	if (uc_speed) {
		p_emac->EMAC_NCFGR |= EMAC_NCFGR_SPD;
	} else {
		p_emac->EMAC_NCFGR &= ~EMAC_NCFGR_SPD;
	}
}

/**
 * \brief Enable/Disable Full-Duplex mode.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_enable   0 to disable the Full-Duplex mode, else to enable it.
 */
static inline void emac_enable_full_duplex(Emac* p_emac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_emac->EMAC_NCFGR |= EMAC_NCFGR_FD;
	} else {
		p_emac->EMAC_NCFGR &= ~EMAC_NCFGR_FD;
	}
}

/**
 * \brief Enable/Disable Copy(Receive) All Valid Frames.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_enable   0 to disable copying all valid frames, else to enable it.
 */
static inline void emac_enable_copy_all(Emac* p_emac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_emac->EMAC_NCFGR |= EMAC_NCFGR_CAF;
	} else {
		p_emac->EMAC_NCFGR &= ~EMAC_NCFGR_CAF;
	}
}

/**
 * \brief Enable/Disable jumbo frames (up to 10240 bytes).
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_enable   0 to disable the jumbo frames, else to enable it.
 */
static inline void emac_enable_jumbo_frames(Emac* p_emac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_emac->EMAC_NCFGR |= EMAC_NCFGR_JFRAME;
	} else {
		p_emac->EMAC_NCFGR &= ~EMAC_NCFGR_JFRAME;
	}
}

/**
 * \brief Disable/Enable broadcast receiving.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_enable   1 to disable the broadcast, else to enable it.
 */
static inline void emac_disable_broadcast(Emac* p_emac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_emac->EMAC_NCFGR |= EMAC_NCFGR_NBC;
	} else {
		p_emac->EMAC_NCFGR &= ~EMAC_NCFGR_NBC;
	}
}

/**
 * \brief Enable/Disable multicast hash.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_enable   0 to disable the multicast hash, else to enable it.
 */
static inline void emac_enable_multicast_hash(Emac* p_emac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_emac->EMAC_NCFGR |= EMAC_NCFGR_UNI;
	} else {
		p_emac->EMAC_NCFGR &= ~EMAC_NCFGR_UNI;
	}
}

/**
 * \brief Enable/Disable big frames (over 1518, up to 1536).
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_enable   0 to disable big frames else to enable it.
 */
static inline void emac_enable_big_frame(Emac* p_emac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_emac->EMAC_NCFGR |= EMAC_NCFGR_BIG;
	} else {
		p_emac->EMAC_NCFGR &= ~EMAC_NCFGR_BIG;
	}
}

/**
 * \brief Set MDC clock divider.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param ul_mck   EMAC MCK.
 *
 * \return EMAC_OK if successfully.
 */
static inline uint8_t emac_set_clock(Emac* p_emac, uint32_t ul_mck)
{
	uint32_t ul_clk;

	if (ul_mck > EMAC_CLOCK_SPEED_160MHZ) {
		return EMAC_INVALID;
	} else if (ul_mck > EMAC_CLOCK_SPEED_80MHZ) {
		ul_clk = EMAC_NCFGR_CLK_MCK_64;
	} else if (ul_mck > EMAC_CLOCK_SPEED_40MHZ) {
		ul_clk = EMAC_NCFGR_CLK_MCK_32;
	} else if (ul_mck > EMAC_CLOCK_SPEED_20MHZ) {
		ul_clk = EMAC_NCFGR_CLK_MCK_16;
	} else {
		ul_clk = EMAC_NCFGR_CLK_MCK_8;
	}

	p_emac->EMAC_NCFGR &= ~EMAC_NCFGR_CLK_Msk;
	p_emac->EMAC_NCFGR |= ul_clk;

	return EMAC_OK;
}

/**
 * \brief Enable/Disable retry test.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_enable   0 to disable the EMAC receiver, else to enable it.
 */
static inline void emac_enable_retry_test(Emac* p_emac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_emac->EMAC_NCFGR |= EMAC_NCFGR_RTY;
	} else {
		p_emac->EMAC_NCFGR &= ~EMAC_NCFGR_RTY;
	}
}

/**
 * \brief Enable/Disable pause (when a valid pause frame is received).
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_enable   0 to disable pause frame, else to enable it.
 */
static inline void emac_enable_pause_frame(Emac* p_emac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_emac->EMAC_NCFGR |= EMAC_NCFGR_PAE;
	} else {
		p_emac->EMAC_NCFGR &= ~EMAC_NCFGR_PAE;
	}
}

/**
 * \brief Set receive buffer offset to 0 ~ 3.
 *
 * \param p_emac   Pointer to the EMAC instance.
 */
static inline void emac_set_rx_buffer_offset(Emac* p_emac, uint8_t uc_offset)
{
	p_emac->EMAC_NCFGR &= ~EMAC_NCFGR_RBOF_Msk;
	p_emac->EMAC_NCFGR |=
			(EMAC_NCFGR_RBOF_Msk & ((uc_offset) << EMAC_NCFGR_RBOF_Pos));
}

/**
 * \brief Enable/Disable receive length field checking.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_enable   0 to disable receive length field checking, else to enable it.
 */
static inline void emac_enable_rx_length_check(Emac* p_emac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_emac->EMAC_NCFGR |= EMAC_NCFGR_RLCE;
	} else {
		p_emac->EMAC_NCFGR &= ~EMAC_NCFGR_RLCE;
	}
}

/**
 * \brief Enable/Disable discarding FCS field of received frames.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_enable   0 to disable discarding FCS field of received frames, else to enable it.
 */
static inline void emac_enable_discard_fcs(Emac* p_emac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_emac->EMAC_NCFGR |= EMAC_NCFGR_DRFCS;
	} else {
		p_emac->EMAC_NCFGR &= ~EMAC_NCFGR_DRFCS;
	}
}


/**
 * \brief Enable/Disable frames to be received in half-duplex mode
 * while transmitting.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_enable   0 to disable the received in half-duplex mode, else to enable it.
 */
static inline void emac_enable_efrhd(Emac* p_emac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_emac->EMAC_NCFGR |= EMAC_NCFGR_EFRHD;
	} else {
		p_emac->EMAC_NCFGR &= ~EMAC_NCFGR_EFRHD;
	}
}

/**
 * \brief Enable/Disable ignore RX FCS.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_enable   0 to disable ignore RX FCS, else to enable it.
 */
static inline void emac_enable_ignore_rx_fcs(Emac* p_emac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_emac->EMAC_NCFGR |= EMAC_NCFGR_IRXFCS;
	} else {
		p_emac->EMAC_NCFGR &= ~EMAC_NCFGR_IRXFCS;
	}
}

/**
 * \brief Get Network Status.
 *
 * \param p_emac   Pointer to the EMAC instance.
 *
 * \return Network status.
 */
static inline uint32_t emac_get_status(Emac* p_emac)
{
	return p_emac->EMAC_NSR;
}

/**
 * \brief Get MDIO IN pin status.
 *
 * \param p_emac   Pointer to the EMAC instance.
 *
 * \return MDIO IN pin status.
 */
static inline uint8_t emac_get_MDIO(Emac* p_emac)
{
	return ((p_emac->EMAC_NSR & EMAC_NSR_MDIO) > 0);
}

/**
 * \brief Check if PHY is idle.
 *
 * \param p_emac   Pointer to the EMAC instance.
 *
 * \return  1 if PHY is idle.
 */
static inline uint8_t emac_is_phy_idle(Emac* p_emac)
{
	return ((p_emac->EMAC_NSR & EMAC_NSR_IDLE) > 0);
}

/**
 * \brief Return transmit status.
 *
 * \param p_emac   Pointer to the EMAC instance.
 *
 * \return  Transmit status.
 */
static inline uint32_t emac_get_tx_status(Emac* p_emac)
{
	return p_emac->EMAC_TSR;
}

/**
 * \brief Clear transmit status.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param ul_status   Transmit status.
 */
static inline void emac_clear_tx_status(Emac* p_emac, uint32_t ul_status)
{
	p_emac->EMAC_TSR = ul_status;
}

/**
 * \brief Return receive status.
 *
 * \param p_emac   Pointer to the EMAC instance.
 */
static inline uint32_t emac_get_rx_status(Emac* p_emac)
{
	return p_emac->EMAC_RSR;
}

/**
 * \brief Clear receive status.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param ul_status   Receive status.
 */
static inline void emac_clear_rx_status(Emac* p_emac, uint32_t ul_status)
{
	p_emac->EMAC_RSR = ul_status;
}

/**
 * \brief Set Rx Queue.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param ul_addr   Rx queue address.
 */
static inline void emac_set_rx_queue(Emac* p_emac, uint32_t ul_addr)
{
	p_emac->EMAC_RBQP = EMAC_RBQP_ADDR_Msk & ul_addr;
}

/**
 * \brief Get Rx Queue Address.
 *
 * \param p_emac   Pointer to the EMAC instance.
 *
 * \return  Rx queue address.
 */
static inline uint32_t emac_get_rx_queue(Emac* p_emac)
{
	return p_emac->EMAC_RBQP;
}

/**
 * \brief Set Tx Queue.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param ul_addr  Tx queue address.
 */
static inline void emac_set_tx_queue(Emac* p_emac, uint32_t ul_addr)
{
	p_emac->EMAC_TBQP = EMAC_TBQP_ADDR_Msk & ul_addr;
}

/**
 * \brief Get Tx Queue.
 *
 * \param p_emac   Pointer to the EMAC instance.
 *
 * \return  Rx queue address.
 */
static inline uint32_t emac_get_tx_queue(Emac* p_emac)
{
	return p_emac->EMAC_TBQP;
}

/**
 * \brief Enable interrupt(s).
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param ul_source   Interrupt source(s) to be enabled.
 */
static inline void emac_enable_interrupt(Emac* p_emac, uint32_t ul_source)
{
	p_emac->EMAC_IER = ul_source;
}

/**
 * \brief Disable interrupt(s).
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param ul_source   Interrupt source(s) to be disabled.
 */
static inline void emac_disable_interrupt(Emac* p_emac, uint32_t ul_source)
{
	p_emac->EMAC_IDR = ul_source;
}

/**
 * \brief Return interrupt status.
 *
 * \param p_emac   Pointer to the EMAC instance.
 *
 * \return Interrupt status.
 */
static inline uint32_t emac_get_interrupt_status(Emac* p_emac)
{
	return p_emac->EMAC_ISR;
}

/**
 * \brief Return interrupt mask.
 *
 * \param p_emac   Pointer to the EMAC instance.
 *
 * \return Interrupt mask.
 */
static inline uint32_t emac_get_interrupt_mask(Emac* p_emac)
{
	return p_emac->EMAC_IMR;
}

/**
 * \brief Execute PHY maintenance command.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_phy_addr   PHY address.
 * \param uc_reg_addr   Register address.
 * \param uc_rw   1 to Read, 0 to write.
 * \param us_data   Data to be performed, write only.
 */
static inline void emac_maintain_phy(Emac* p_emac,
		uint8_t uc_phy_addr, uint8_t uc_reg_addr, uint8_t uc_rw,
		uint16_t us_data)
{
	/* Wait until bus idle */
	while ((p_emac->EMAC_NSR & EMAC_NSR_IDLE) == 0);
	/* Write maintain register */
	p_emac->EMAC_MAN = EMAC_MAN_CODE(EMAC_MAN_CODE_VALUE)
			| EMAC_MAN_SOF(EMAC_MAN_SOF_VALUE)
			| EMAC_MAN_PHYA(uc_phy_addr)
			| EMAC_MAN_REGA(uc_reg_addr)
			| EMAC_MAN_RW((uc_rw ? EMAC_MAN_RW_TYPE : EMAC_MAN_READ_ONLY))
			| EMAC_MAN_DATA(us_data);
}

/**
 * \brief Get PHY maintenance data returned.
 *
 * \param p_emac   Pointer to the EMAC instance.
 *
 * \return Get PHY data.
 */
static inline uint16_t emac_get_phy_data(Emac* p_emac)
{
	/* Wait until bus idle */
	while ((p_emac->EMAC_NSR & EMAC_NSR_IDLE) == 0);
	/* Return data */
	return (uint16_t) (p_emac->EMAC_MAN & EMAC_MAN_DATA_Msk);
}

/**
 * \brief Set pause time.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param us_pause_time   Pause time.
 */
static inline void emac_set_pause_time(Emac* p_emac, uint16_t us_pause_time)
{
	p_emac->EMAC_PTR = us_pause_time;
}

/**
 * \brief Set Hash.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param ul_hash_top   Hash top.
 * \param ul_hash_bottom   Hash bottom.
 */
static inline void emac_set_hash(Emac* p_emac, uint32_t ul_hash_top,
		uint32_t ul_hash_bottom)
{
	p_emac->EMAC_HRB = ul_hash_bottom;
	p_emac->EMAC_HRT = ul_hash_top;
}

/**
 * \brief Set 64 bits Hash.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param ull_hash   64 bits hash value.
 */
static inline void emac_set_hash64(Emac* p_emac, uint64_t ull_hash)
{
	p_emac->EMAC_HRB = (uint32_t) ull_hash;
	p_emac->EMAC_HRT = (uint32_t) (ull_hash >> 32);
}

/**
 * \brief Set MAC Address.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_index  EMAC specific address register index.
 * \param p_mac_addr  EMAC address.
 */
static inline void emac_set_address(Emac* p_emac, uint8_t uc_index,
		uint8_t* p_mac_addr)
{
	p_emac->EMAC_SA[uc_index].EMAC_SAxB = (p_mac_addr[3] << 24)
			| (p_mac_addr[2] << 16)
			| (p_mac_addr[1] << 8)
			| (p_mac_addr[0]);
	p_emac->EMAC_SA[uc_index].EMAC_SAxT = (p_mac_addr[5] << 8)
			| (p_mac_addr[4]);
}

/**
 * \brief Set MAC Address via 2 dword.
  *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_index  EMAC specific address register index.
 * \param ul_mac_top  EMAC top address.
 * \param ul_mac_bottom  EMAC bottom address.
 */
static inline void emac_set_address32(Emac* p_emac, uint8_t uc_index,
		uint32_t ul_mac_top, uint32_t ul_mac_bottom)
{
	p_emac->EMAC_SA[uc_index].EMAC_SAxB = ul_mac_bottom;
	p_emac->EMAC_SA[uc_index].EMAC_SAxT = ul_mac_top;
}

/**
 * \brief Set MAC Address via int64.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_index  EMAC specific address register index.
 * \param ull_mac  64-bit EMAC address.
 */
static inline void emac_set_address64(Emac* p_emac, uint8_t uc_index,
		uint64_t ull_mac)
{
	p_emac->EMAC_SA[uc_index].EMAC_SAxB = (uint32_t) ull_mac;
	p_emac->EMAC_SA[uc_index].EMAC_SAxT = (uint32_t) (ull_mac >> 32);
}

/**
 * \brief Set type ID.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param us_type_id   Type to be set.
 */
static inline void emac_set_type_id(Emac* p_emac, uint16_t us_type_id)
{
	p_emac->EMAC_TID = EMAC_TID_TID(us_type_id);
}

/**
 * \brief Get type ID.
 *
 * \param p_emac   Pointer to the EMAC instance.
 *
 * \return Type ID.
 */
static inline uint16_t emac_get_type_id(Emac* p_emac)
{
	return (p_emac->EMAC_TID & EMAC_TID_TID_Msk);
}

/**
 * \brief Enable/Disable RMII.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_enable   0 to disable the RMII mode, else to enable it.
 */
static inline void emac_enable_rmii(Emac* p_emac, uint8_t uc_enable)
{
	if (uc_enable) {
		p_emac->EMAC_USRIO |= EMAC_USRIO_RMII;
	} else {
		p_emac->EMAC_USRIO &= ~EMAC_USRIO_RMII;
	}
}

/**
 * \brief Enable/Disable transceiver input clock.
 *
 * \param p_emac   Pointer to the EMAC instance.
 * \param uc_enable   0 to disable transceiver input clock, else to enable it.
 */
static inline void emac_enable_transceiver_clock(Emac* p_emac,
		uint8_t uc_enable)
{
	if (uc_enable) {
		p_emac->EMAC_USRIO |= EMAC_USRIO_CLKEN;
	} else {
		p_emac->EMAC_USRIO &= ~EMAC_USRIO_CLKEN;
	}
}

uint8_t emac_phy_read(Emac* p_emac, uint8_t uc_phy_address, uint8_t uc_address,
		uint32_t* p_value);
uint8_t emac_phy_write(Emac* p_emac, uint8_t uc_phy_address,
		uint8_t uc_address, uint32_t ul_value);
void emac_dev_init(Emac* p_emac, emac_device_t* p_emac_dev,
		emac_options_t* p_opt);
uint32_t emac_dev_read(emac_device_t* p_emac_dev, uint8_t* p_frame,
		uint32_t ul_frame_size, uint32_t* p_rcv_size);
uint32_t emac_dev_write(emac_device_t* p_emac_dev, void *p_buffer,
		uint32_t ul_size, emac_dev_tx_cb_t func_tx_cb);
uint32_t emac_dev_get_tx_load(emac_device_t* p_emac_dev);
void emac_dev_set_rx_callback(emac_device_t* p_emac_dev,
		emac_dev_tx_cb_t func_rx_cb);
uint8_t emac_dev_set_tx_wakeup_callback(emac_device_t* p_emac_dev,
		emac_dev_wakeup_cb_t func_wakeup, uint8_t uc_threshold);
void emac_dev_reset(emac_device_t* p_emac_dev);
void emac_handler(emac_device_t* p_emac_dev);

/// @cond 0
/**INDENT-OFF**/
#ifdef __cplusplus
}
#endif
/**INDENT-ON**/
/// @endcond

/**
 * \page emac_quickstart Quickstart guide for EMAC driver.
 *
 * This is the quickstart guide for the \ref emac_group "Ethernet MAC",
 * with step-by-step instructions on how to configure and use the driver in a
 * selection of use cases.
 *
 * The use cases contain several code fragments. The code fragments in the
 * steps for setup can be copied into a custom initialization function, while
 * the steps for usage can be copied into, e.g., the main application function.
 *
 * \section emac_basic_use_case Basic use case
 * In the basic use case, the EMAC driver are configured for:
 * - PHY component DM9161A is used
 * - EMAC uses RMII mode
 * - The number of receive buffer is 16
 * - The number of transfer buffer is 8
 * - MAC address is set to 00-04-25-1c-a0-02
 * - IP address is set to 192.168.0.2
 * - IP address is set to 192.168.0.2
 * - Gateway is set to 192.168.0.1
 * - Network mask is 255.255.255.0
 * - PHY operation max retry count is 1000000
 * - EMAC is configured to not support copy all frame and support broadcast
 * - The reset PIN of DM9161A is connected to the NRST of SAM3X
 * - The data will be read from the ethernet
 *
 * \section emac_basic_use_case_setup Setup steps
 *
 * \subsection emac_basic_use_case_setup_prereq Prerequisites
 * -# \ref sysclk_group "System Clock Management (sysclock)"
 * -# \ref pio_group "Parallel Input/Output Controller (pio)"
 * -# \ref pmc_group "Power Management Controller (pmc)"
 * -# \ref sam_drivers_rstc_group "Reset Controller (RSTC)"
 * -# \ref dm9161a_ethernet_phy_group "PHY component (DM9161A)"
 *
 * \subsection emac_basic_use_case_setup_code Example code
 * Content of conf_eth.h
 * \code
 * #define EMAC_RX_BUFFERS                               16
 * #define EMAC_TX_BUFFERS                               8
 * #define MAC_PHY_RETRY_MAX                             1000000
 * #define ETHERNET_CONF_ETHADDR0                        0x00
 * #define ETHERNET_CONF_ETHADDR0                        0x00
 * #define ETHERNET_CONF_ETHADDR1                        0x04
 * #define ETHERNET_CONF_ETHADDR2                        0x25
 * #define ETHERNET_CONF_ETHADDR3                        0x1C
 * #define ETHERNET_CONF_ETHADDR4                        0xA0
 * #define ETHERNET_CONF_ETHADDR5                        0x02
 * #define ETHERNET_CONF_IPADDR0                         192
 * #define ETHERNET_CONF_IPADDR1                         168
 * #define ETHERNET_CONF_IPADDR2                         0
 * #define ETHERNET_CONF_IPADDR3                         2
 * #define ETHERNET_CONF_GATEWAY_ADDR0                   192
 * #define ETHERNET_CONF_GATEWAY_ADDR1                   168
 * #define ETHERNET_CONF_GATEWAY_ADDR2                   0
 * #define ETHERNET_CONF_GATEWAY_ADDR3                   1
 * #define ETHERNET_CONF_NET_MASK0                       255
 * #define ETHERNET_CONF_NET_MASK1                       255
 * #define ETHERNET_CONF_NET_MASK2                       255
 * #define ETHERNET_CONF_NET_MASK3                       0
 * #define ETH_PHY_MODE                                  BOARD_EMAC_MODE_RMII
 * \endcode
 *
 * A specific emac device and the receive data buffer must be defined; another ul_frm_size should be defined
 * to trace the actual size of the data received.
 * \code
 * static emac_device_t gs_emac_dev;
 * static volatile uint8_t gs_uc_eth_buffer[EMAC_FRAME_LENTGH_MAX];
 *
 * uint32_t ul_frm_size;
 * \endcode
 *
 * Add to application C-file:
 * \code
 *   void emac_init(void)
 *   {
 *       sysclk_init();
 *
 *       board_init();
 *
 *       rstc_set_external_reset(RSTC, 13);
 *       rstc_reset_extern(RSTC);
 *       while (rstc_get_status(RSTC) & RSTC_SR_NRSTL) {
 *       };
 *
 *       ul_delay = sysclk_get_cpu_hz() / 1000 / 3 * 400;
 *       while (ul_delay--);
 *
 *       pmc_enable_periph_clk(ID_EMAC);
 *
 *       emac_option.uc_copy_all_frame = 0;
 *       emac_option.uc_no_boardcast = 0;
 *       memcpy(emac_option.uc_mac_addr, gs_uc_mac_address, sizeof(gs_uc_mac_address));
 *       gs_emac_dev.p_hw = EMAC;
 *
 *       emac_dev_init(EMAC, &gs_emac_dev, &emac_option);
 *
 *       NVIC_EnableIRQ(EMAC_IRQn);
 *
 *       ethernet_phy_init(EMAC, BOARD_EMAC_PHY_ADDR, sysclk_get_cpu_hz()
 *
 *       ethernet_phy_auto_negotiate(EMAC, BOARD_EMAC_PHY_ADDR
 *
 *       ethernet_phy_set_link(EMAC, BOARD_EMAC_PHY_ADDR, 1)
 * \endcode
 *
 * \subsection emac_basic_use_case_setup_flow Workflow
 * -# Ensure that conf_emac.h is present and contains the
 * following configuration symbol. This configuration file is used
 * by the driver and should not be included by the user.
 *   - \code
 *        #define EMAC_RX_BUFFERS                               16
 *        #define EMAC_TX_BUFFERS                               8
 *        #define MAC_PHY_RETRY_MAX                             1000000
 *        #define ETHERNET_CONF_ETHADDR0                        0x00
 *        #define ETHERNET_CONF_ETHADDR0                        0x00
 *        #define ETHERNET_CONF_ETHADDR1                        0x04
 *        #define ETHERNET_CONF_ETHADDR2                        0x25
 *        #define ETHERNET_CONF_ETHADDR3                        0x1C
 *        #define ETHERNET_CONF_ETHADDR4                        0xA0
 *        #define ETHERNET_CONF_ETHADDR5                        0x02
 *        #define ETHERNET_CONF_IPADDR0                         192
 *        #define ETHERNET_CONF_IPADDR1                         168
 *        #define ETHERNET_CONF_IPADDR2                         0
 *        #define ETHERNET_CONF_IPADDR3                         2
 *        #define ETHERNET_CONF_GATEWAY_ADDR0                   192
 *        #define ETHERNET_CONF_GATEWAY_ADDR1                   168
 *        #define ETHERNET_CONF_GATEWAY_ADDR2                   0
 *        #define ETHERNET_CONF_GATEWAY_ADDR3                   1
 *        #define ETHERNET_CONF_NET_MASK0                       255
 *        #define ETHERNET_CONF_NET_MASK1                       255
 *        #define ETHERNET_CONF_NET_MASK2                       255
 *        #define ETHERNET_CONF_NET_MASK3                       0
 *        #define ETH_PHY_MODE                                  BOARD_EMAC_MODE_RMII
 *   \endcode
 * -# Enable the system clock:
 *   - \code sysclk_init(); \endcode
 * -# Enable PIO configurations for EMAC:
 *   - \code board_init(); \endcode
 * -# Reset PHY; this is required by the DM9161A component:
 *   - \code
 *         rstc_set_external_reset(RSTC, 13);
 *         rstc_reset_extern(RSTC);
 *         while (rstc_get_status(RSTC) & RSTC_SR_NRSTL) {
 *         };
 * \endcode
 * -# Wait for PHY ready:
 *   - \code
 *         ul_delay = sysclk_get_cpu_hz() / 1000 / 3 * 400;
 *         while (ul_delay--);
 * \endcode
 * -# Enable PMC clock for EMAC:
 *   - \code pmc_enable_periph_clk(ID_EMAC); \endcode
 * -# Set the EMAC options; it's set to copy all frame and support broadcast:
 *   - \code
 *         emac_option.uc_copy_all_frame = 0;
 *         emac_option.uc_no_boardcast = 0;
 *         memcpy(emac_option.uc_mac_addr, gs_uc_mac_address, sizeof(gs_uc_mac_address));
 *         gs_emac_dev.p_hw = EMAC;
 * \endcode
 * -# Initialize EMAC device with the filled option:
 *   - \code
 *         emac_dev_init(EMAC, &gs_emac_dev, &emac_option);
 * \endcode
 * -# Enable the interrupt service for EMAC:
 *   - \code
 *         NVIC_EnableIRQ(EMAC_IRQn);
 * \endcode
 * -# Initialize the PHY component:
 *   - \code
 *         ethernet_phy_init(EMAC, BOARD_EMAC_PHY_ADDR, sysclk_get_cpu_hz());
 * \endcode
  * -# The link will be established based on auto negotiation.
 *   - \code
 *         ethernet_phy_auto_negotiate(EMAC, BOARD_EMAC_PHY_ADDR);
 * \endcode
 * -# Establish the ethernet link; the network can be worked from now on:
 *   - \code
 *         ethernet_phy_set_link(EMAC, BOARD_EMAC_PHY_ADDR, 1);
 * \endcode
 *
 * \section emac_basic_use_case_usage Usage steps
 * \subsection emac_basic_use_case_usage_code Example code
 * Add to, e.g., main loop in application C-file:
 * \code
 *    emac_dev_read(&gs_emac_dev, (uint8_t *) gs_uc_eth_buffer, sizeof(gs_uc_eth_buffer), &ul_frm_size));
 * \endcode
 *
 * \subsection emac_basic_use_case_usage_flow Workflow
 * -# Start reading the data from the ethernet:
 *   - \code emac_dev_read(&gs_emac_dev, (uint8_t *) gs_uc_eth_buffer, sizeof(gs_uc_eth_buffer), &ul_frm_size)); \endcode
 */

#endif /* EMAC_H_INCLUDED */
