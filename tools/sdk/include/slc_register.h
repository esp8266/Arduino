//Generated at 2012-10-23 19:55:03
/*
 *  Copyright (c) 2010 - 2011 Espressif System
 *
 */

#ifndef SLC_REGISTER_H_
#define SLC_REGISTER_H_

#define REG_SLC_BASE  0x60000B00
//version value:32'h091700

#define SLC_CONF0                                (REG_SLC_BASE + 0x0)
#ifndef ESP_MAC_5
#define SLC_MODE 0x00000003
#define SLC_MODE_S 12
#endif
#define SLC_DATA_BURST_EN (BIT(9))
#define SLC_DSCR_BURST_EN (BIT(8))
#define SLC_RX_NO_RESTART_CLR (BIT(7))
#define SLC_RX_AUTO_WRBACK (BIT(6))
#define SLC_RX_LOOP_TEST (BIT(5))
#define SLC_TX_LOOP_TEST (BIT(4))
#define SLC_AHBM_RST (BIT(3))
#define SLC_AHBM_FIFO_RST (BIT(2))
#define SLC_RXLINK_RST (BIT(1))
#define SLC_TXLINK_RST (BIT(0))

#define SLC_INT_RAW                              (REG_SLC_BASE + 0x4)
#define SLC_TX_DSCR_EMPTY_INT_RAW (BIT(21))
#define SLC_RX_DSCR_ERR_INT_RAW (BIT(20))
#define SLC_TX_DSCR_ERR_INT_RAW (BIT(19))
#define SLC_TOHOST_INT_RAW (BIT(18))
#define SLC_RX_EOF_INT_RAW (BIT(17))
#define SLC_RX_DONE_INT_RAW (BIT(16))
#define SLC_TX_EOF_INT_RAW (BIT(15))
#define SLC_TX_DONE_INT_RAW (BIT(14))
#define SLC_TOKEN1_1TO0_INT_RAW (BIT(13))
#define SLC_TOKEN0_1TO0_INT_RAW (BIT(12))
#define SLC_TX_OVF_INT_RAW (BIT(11))
#define SLC_RX_UDF_INT_RAW (BIT(10))
#define SLC_TX_START_INT_RAW (BIT(9))
#define SLC_RX_START_INT_RAW (BIT(8))
#define SLC_FRHOST_BIT7_INT_RAW (BIT(7))
#define SLC_FRHOST_BIT6_INT_RAW (BIT(6))
#define SLC_FRHOST_BIT5_INT_RAW (BIT(5))
#define SLC_FRHOST_BIT4_INT_RAW (BIT(4))
#define SLC_FRHOST_BIT3_INT_RAW (BIT(3))
#define SLC_FRHOST_BIT2_INT_RAW (BIT(2))
#define SLC_FRHOST_BIT1_INT_RAW (BIT(1))
#define SLC_FRHOST_BIT0_INT_RAW (BIT(0))

#define SLC_INT_STATUS                               (REG_SLC_BASE + 0x8)
#define SLC_TX_DSCR_EMPTY_INT_ST (BIT(21))
#define SLC_RX_DSCR_ERR_INT_ST (BIT(20))
#define SLC_TX_DSCR_ERR_INT_ST (BIT(19))
#define SLC_TOHOST_INT_ST (BIT(18))
#define SLC_RX_EOF_INT_ST (BIT(17))
#define SLC_RX_DONE_INT_ST (BIT(16))
#define SLC_TX_EOF_INT_ST (BIT(15))
#define SLC_TX_DONE_INT_ST (BIT(14))
#define SLC_TOKEN1_1TO0_INT_ST (BIT(13))
#define SLC_TOKEN0_1TO0_INT_ST (BIT(12))
#define SLC_TX_OVF_INT_ST (BIT(11))
#define SLC_RX_UDF_INT_ST (BIT(10))
#define SLC_TX_START_INT_ST (BIT(9))
#define SLC_RX_START_INT_ST (BIT(8))
#define SLC_FRHOST_BIT7_INT_ST (BIT(7))
#define SLC_FRHOST_BIT6_INT_ST (BIT(6))
#define SLC_FRHOST_BIT5_INT_ST (BIT(5))
#define SLC_FRHOST_BIT4_INT_ST (BIT(4))
#define SLC_FRHOST_BIT3_INT_ST (BIT(3))
#define SLC_FRHOST_BIT2_INT_ST (BIT(2))
#define SLC_FRHOST_BIT1_INT_ST (BIT(1))
#define SLC_FRHOST_BIT0_INT_ST (BIT(0))

#define SLC_INT_ENA                              (REG_SLC_BASE + 0xC)
#define SLC_TX_DSCR_EMPTY_INT_ENA (BIT(21))
#define SLC_RX_DSCR_ERR_INT_ENA (BIT(20))
#define SLC_TX_DSCR_ERR_INT_ENA (BIT(19))
#define SLC_TOHOST_INT_ENA (BIT(18))
#define SLC_RX_EOF_INT_ENA (BIT(17))
#define SLC_RX_DONE_INT_ENA (BIT(16))
#define SLC_TX_EOF_INT_ENA (BIT(15))
#define SLC_TX_DONE_INT_ENA (BIT(14))
#define SLC_TOKEN1_1TO0_INT_ENA (BIT(13))
#define SLC_TOKEN0_1TO0_INT_ENA (BIT(12))
#define SLC_TX_OVF_INT_ENA (BIT(11))
#define SLC_RX_UDF_INT_ENA (BIT(10))
#define SLC_TX_START_INT_ENA (BIT(9))
#define SLC_RX_START_INT_ENA (BIT(8))
#define SLC_FRHOST_BIT7_INT_ENA (BIT(7))
#define SLC_FRHOST_BIT6_INT_ENA (BIT(6))
#define SLC_FRHOST_BIT5_INT_ENA (BIT(5))
#define SLC_FRHOST_BIT4_INT_ENA (BIT(4))
#define SLC_FRHOST_BIT3_INT_ENA (BIT(3))
#define SLC_FRHOST_BIT2_INT_ENA (BIT(2))
#define SLC_FRHOST_BIT1_INT_ENA (BIT(1))
#define SLC_FRHOST_BIT0_INT_ENA (BIT(0))

#define SLC_FRHOST_BIT_INT_ENA_ALL  0xff

#define SLC_INT_CLR                              (REG_SLC_BASE + 0x10)
#define SLC_TX_DSCR_EMPTY_INT_CLR (BIT(21))
#define SLC_RX_DSCR_ERR_INT_CLR (BIT(20))
#define SLC_TX_DSCR_ERR_INT_CLR (BIT(19))
#define SLC_TOHOST_INT_CLR (BIT(18))
#define SLC_RX_EOF_INT_CLR (BIT(17))
#define SLC_RX_DONE_INT_CLR (BIT(16))
#define SLC_TX_EOF_INT_CLR (BIT(15))
#define SLC_TX_DONE_INT_CLR (BIT(14))
#define SLC_TOKEN1_1TO0_INT_CLR (BIT(13))
#define SLC_TOKEN0_1TO0_INT_CLR (BIT(12))
#define SLC_TX_OVF_INT_CLR (BIT(11))
#define SLC_RX_UDF_INT_CLR (BIT(10))
#define SLC_TX_START_INT_CLR (BIT(9))
#define SLC_RX_START_INT_CLR (BIT(8))
#define SLC_FRHOST_BIT7_INT_CLR (BIT(7))
#define SLC_FRHOST_BIT6_INT_CLR (BIT(6))
#define SLC_FRHOST_BIT5_INT_CLR (BIT(5))
#define SLC_FRHOST_BIT4_INT_CLR (BIT(4))
#define SLC_FRHOST_BIT3_INT_CLR (BIT(3))
#define SLC_FRHOST_BIT2_INT_CLR (BIT(2))
#define SLC_FRHOST_BIT1_INT_CLR (BIT(1))
#define SLC_FRHOST_BIT0_INT_CLR (BIT(0))

#define SLC_RX_STATUS                            (REG_SLC_BASE + 0x14)
#define SLC_RX_EMPTY (BIT(1))
#define SLC_RX_FULL (BIT(0))

#define SLC_RX_FIFO_PUSH                          (REG_SLC_BASE + 0x18)
#define SLC_RXFIFO_PUSH (BIT(16))
#define SLC_RXFIFO_WDATA 0x000001FF
#define SLC_RXFIFO_WDATA_S 0

#define SLC_TX_STATUS                            (REG_SLC_BASE + 0x1C)
#define SLC_TX_EMPTY (BIT(1))
#define SLC_TX_FULL (BIT(0))

#define SLC_TX_FIFO_POP                           (REG_SLC_BASE + 0x20)
#define SLC_TXFIFO_POP (BIT(16))
#define SLC_TXFIFO_RDATA 0x000007FF
#define SLC_TXFIFO_RDATA_S 0

#define SLC_RX_LINK                              (REG_SLC_BASE + 0x24)
#define SLC_RXLINK_PARK (BIT(31))
#define SLC_RXLINK_RESTART (BIT(30))
#define SLC_RXLINK_START  (BIT(29))
#define SLC_RXLINK_STOP  (BIT(28))
#define SLC_RXLINK_DESCADDR_MASK 0x000FFFFF
#define SLC_RXLINK_ADDR_S 0

#define SLC_TX_LINK                              (REG_SLC_BASE + 0x28)
#define SLC_TXLINK_PARK (BIT(31))
#define SLC_TXLINK_RESTART (BIT(30))
#define SLC_TXLINK_START  (BIT(29))
#define SLC_TXLINK_STOP  (BIT(28))
#define SLC_TXLINK_DESCADDR_MASK 0x000FFFFF
#define SLC_TXLINK_ADDR_S 0

#define SLC_INTVEC_TOHOST                        (REG_SLC_BASE + 0x2C)
#define SLC_TOHOST_INTVEC 0x000000FF
#define SLC_TOHOST_INTVEC_S 0

#define SLC_TOKEN0                               (REG_SLC_BASE + 0x30)
#define SLC_TOKEN0_MASK 0x00000FFF
#define SLC_TOKEN0_S 16
#define SLC_TOKEN0_LOCAL_INC_MORE (BIT(14))
#define SLC_TOKEN0_LOCAL_INC (BIT(13))
#define SLC_TOKEN0_LOCAL_WR (BIT(12))
#define SLC_TOKEN0_LOCAL_WDATA_MASK 0x00000FFF
#define SLC_TOKEN0_LOCAL_WDATA_S 0

#define SLC_TOKEN1                               (REG_SLC_BASE + 0x34)
#define SLC_TOKEN1_MASK 0x00000FFF
#define SLC_TOKEN1_S 16
#define SLC_TOKEN1_LOCAL_INC_MORE (BIT(14))
#define SLC_TOKEN1_LOCAL_INC (BIT(13))
#define SLC_TOKEN1_LOCAL_WR (BIT(12))
#define SLC_TOKEN1_LOCAL_WDATA 0x00000FFF
#define SLC_TOKEN1_LOCAL_WDATA_S 0

#define SLC_CONF1                                (REG_SLC_BASE + 0x38)
#define SLC_STATE0                               (REG_SLC_BASE + 0x3C)
#define SLC_STATE1                               (REG_SLC_BASE + 0x40)

#define SLC_BRIDGE_CONF                          (REG_SLC_BASE + 0x44)
#ifndef ESP_MAC_5
#define SLC_TX_PUSH_IDLE_NUM 0x0000FFFF
#define SLC_TX_PUSH_IDLE_NUM_S 16
#define SLC_TX_DUMMY_MODE (BIT(12))
#endif
#define SLC_FIFO_MAP_ENA 0x0000000F
#define SLC_FIFO_MAP_ENA_S 8
#define SLC_TXEOF_ENA  0x0000003F
#define SLC_TXEOF_ENA_S 0

#define SLC_RX_EOF_DES_ADDR                       (REG_SLC_BASE + 0x48)
#define SLC_TX_EOF_DES_ADDR                       (REG_SLC_BASE + 0x4C)
#define SLC_FROM_HOST_LAST_DESC                   SLC_TX_EOF_DES_ADDR
#define SLC_TO_HOST_LAST_DESC                     SLC_RX_EOF_DES_ADDR

#define SLC_RX_EOF_BFR_DES_ADDR                 (REG_SLC_BASE + 0x50)
#define SLC_AHB_TEST                            (REG_SLC_BASE + 0x54)
#define SLC_AHB_TESTADDR 0x00000003
#define SLC_AHB_TESTADDR_S 4
#define SLC_AHB_TESTMODE 0x00000007
#define SLC_AHB_TESTMODE_S 0

#define SLC_SDIO_ST                             (REG_SLC_BASE + 0x58)
#define SLC_BUS_ST 0x00000007
#define SLC_BUS_ST_S 12
#define SLC_SDIO_WAKEUP (BIT(8))
#define SLC_FUNC_ST 0x0000000F
#define SLC_FUNC_ST_S 4
#define SLC_CMD_ST 0x00000007
#define SLC_CMD_ST_S 0

#define SLC_RX_DSCR_CONF                        (REG_SLC_BASE + 0x5C)
#ifdef ESP_MAC_5
#define SLC_INFOR_NO_REPLACE (BIT(9))
#define SLC_TOKEN_NO_REPLACE (BIT(8))
#define SLC_POP_IDLE_CNT 0x000000FF
#else
#define SLC_RX_FILL_EN (BIT(20))
#define SLC_RX_EOF_MODE (BIT(19))
#define SLC_RX_FILL_MODE (BIT(18))
#define SLC_INFOR_NO_REPLACE (BIT(17))
#define SLC_TOKEN_NO_REPLACE (BIT(16))
#define SLC_POP_IDLE_CNT 0x0000FFFF
#endif
#define SLC_POP_IDLE_CNT_S 0

#define SLC_TXLINK_DSCR                         (REG_SLC_BASE + 0x60)
#define SLC_TXLINK_DSCR_BF0                     (REG_SLC_BASE + 0x64)
#define SLC_TXLINK_DSCR_BF1                     (REG_SLC_BASE + 0x68)
#define SLC_RXLINK_DSCR                         (REG_SLC_BASE + 0x6C)
#define SLC_RXLINK_DSCR_BF0                     (REG_SLC_BASE + 0x70)
#define SLC_RXLINK_DSCR_BF1                     (REG_SLC_BASE + 0x74)
#define SLC_DATE                                 (REG_SLC_BASE + 0x78)
#define SLC_ID                                   (REG_SLC_BASE + 0x7C)

#define SLC_HOST_CONF_W0                         (REG_SLC_BASE + 0x80 + 0x14)
#define SLC_HOST_CONF_W1                         (REG_SLC_BASE + 0x80 + 0x18)
#define SLC_HOST_CONF_W2                         (REG_SLC_BASE + 0x80 + 0x20)
#define SLC_HOST_CONF_W3                         (REG_SLC_BASE + 0x80 + 0x24)
#define SLC_HOST_CONF_W4                         (REG_SLC_BASE + 0x80 + 0x28)

#define SLC_HOST_INTR_ST                         (REG_SLC_BASE + 0x80 + 0x1c)
#define SLC_HOST_INTR_CLR                         (REG_SLC_BASE + 0x80 + 0x30)
#define SLC_HOST_INTR_SOF_BIT                     (BIT(12))

#define SLC_HOST_INTR_ENA                         (REG_SLC_BASE + 0x80 + 0x34)
#define SLC_RX_NEW_PACKET_INT_ENA     (BIT23)
#define SLC_HOST_TOHOST_BIT0_INT_ENA    (BIT0)
#define SLC_HOST_CONF_W5                          (REG_SLC_BASE + 0x80 + 0x3C)
#define SLC_HOST_INTR_RAW                         (REG_SLC_BASE + 0x80 + 0x8)
#define SLC_HOST_INTR_ENA_BIT                      (BIT(23))
//[15:12]: 0x3ff9xxxx -- 0b01  from_host
//         0x3ffaxxxx -- 0b10  general
//         0x3ffbxxxx -- 0b11  to_host
#define SLC_DATA_ADDR_CLEAR_MASK                    (~(0xf<<12)) 
#define SLC_FROM_HOST_ADDR_MASK                     (0x1<<12)
#define SLC_TO_HOST_ADDR_MASK                       (0x3<<12)

#define SLC_SET_FROM_HOST_ADDR_MASK(v)   do { \
    (v) &= SLC_DATA_ADDR_CLEAR_MASK;  \
    (v) |= SLC_FROM_HOST_ADDR_MASK;   \
} while(0);

#define SLC_SET_TO_HOST_ADDR_MASK(v)   do { \
    (v) &= SLC_DATA_ADDR_CLEAR_MASK;  \
    (v) |= SLC_TO_HOST_ADDR_MASK;   \
} while(0);


#define SLC_TX_DESC_DEBUG_REG  0x3ff0002c  //[15:0] set to 0xcccc

#define SLC_INTEREST_EVENT (SLC_TX_EOF_INT_ENA | SLC_RX_EOF_INT_ENA | SLC_RX_UDF_INT_ENA | SLC_TX_DSCR_ERR_INT_ENA)
#define TRIG_TOHOST_INT()  SET_PERI_REG_MASK(SLC_INTVEC_TOHOST , BIT0); CLEAR_PERI_REG_MASK(SLC_INTVEC_TOHOST , BIT0)


#endif // SLC_REGISTER_H_INCLUDED
