/*
 * Copyright (c) 2013, WIZnet Co., Ltd.
 * Copyright (c) 2016, Nicholas Humfrey
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

// original sources: https://github.com/njh/W5500MacRaw

#ifndef W5500_H
#define W5500_H

#include <stdint.h>
#include <Arduino.h>
#include <SPI.h>



class Wiznet5500 {

public:
    /**
     * Constructor that uses the default hardware SPI pins
     * @param cs the Arduino Chip Select / Slave Select pin (default 10)
     */
    Wiznet5500(SPIClass& spi = SPI, int8_t cs = SS, int8_t intr = -1);


    /**
     * Initialise the Ethernet controller
     * Must be called before sending or receiving Ethernet frames
     *
     * @param address the local MAC address for the Ethernet interface
     * @return Returns true if setting up the Ethernet interface was successful
     */
    boolean begin(const uint8_t *address);

    /**
     * Shut down the Ethernet controlled
     */
    void end();

    /**
     * Send an Ethernet frame
     * @param data a pointer to the data to send
     * @param datalen the length of the data in the packet
     * @return the number of bytes transmitted
     */
    uint16_t sendFrame(const uint8_t *data, uint16_t datalen);

    /**
     * Read an Ethernet frame
     * @param buffer a pointer to a buffer to write the packet to
     * @param bufsize the available space in the buffer
     * @return the length of the received packet
     *         or 0 if no packet was received
     */
    uint16_t readFrame(uint8_t *buffer, uint16_t bufsize);

protected:

    /**
     * Read an Ethernet frame size
     * @return the length of data do receive
     *         or 0 if no frame was received
     */
    uint16_t readFrameSize();

    /**
     * discard an Ethernet frame
     * @param framesize readFrameSize()'s result
     */
    void discardFrame(uint16_t framesize);

    /**
     * Read an Ethernet frame data
     *     readFrameSize() must be called first,
     *     its result must be passed into framesize parameter
     * @param buffer a pointer to a buffer to write the frame to
     * @param framesize readFrameSize()'s result
     * @return the length of the received frame
     *         or 0 if a problem occured
     */
    uint16_t readFrameData(uint8_t *frame, uint16_t framesize);


private:

    //< SPI interface Read operation in Control Phase
    static const uint8_t AccessModeRead = (0x00 << 2);

    //< SPI interface Read operation in Control Phase
    static const uint8_t AccessModeWrite = (0x01 << 2);

    //< Common register block in Control Phase
    static const uint8_t BlockSelectCReg = (0x00 << 3);

    //< Socket 0 register block in Control Phase
    static const uint8_t BlockSelectSReg = (0x01 << 3);

    //< Socket 0 Tx buffer address block
    static const uint8_t BlockSelectTxBuf = (0x02 << 3);

    //< Socket 0 Rx buffer address block
    static const uint8_t BlockSelectRxBuf = (0x03 << 3);



    SPIClass& _spi;
    int8_t _cs;
    uint8_t _mac_address[6];

    /**
     * Default function to select chip.
     * @note This function help not to access wrong address. If you do not describe this function or register any functions,
     * null function is called.
     */
    inline void wizchip_cs_select()
    {
        digitalWrite(_cs, LOW);
    }

    /**
     * Default function to deselect chip.
     * @note This function help not to access wrong address. If you do not describe this function or register any functions,
     * null function is called.
     */
    inline void wizchip_cs_deselect()
    {
        digitalWrite(_cs, HIGH);
    }

    /**
     * Default function to read in SPI interface.
     * @note This function help not to access wrong address. If you do not describe this function or register any functions,
     * null function is called.
     */
    inline uint8_t wizchip_spi_read_byte()
    {
        return _spi.transfer(0);
    }

    /**
     * Default function to write in SPI interface.
     * @note This function help not to access wrong address. If you do not describe this function or register any functions,
     * null function is called.
     */
    inline void wizchip_spi_write_byte(uint8_t wb)
    {
        _spi.transfer(wb);
    }


    /**
     * Read a 1 byte value from a register.
     * @param address Register address
     * @return The value of register
     */
    uint8_t wizchip_read(uint8_t block, uint16_t address);

    /**
     * Reads a 2 byte value from a register.
     * @param address Register address
     * @return The value of register
     */
    uint16_t wizchip_read_word(uint8_t block, uint16_t address);

    /**
     * It reads sequence data from registers.
     * @param address Register address
     * @param pBuf Pointer buffer to read data
     * @param len Data length
     */
    void wizchip_read_buf(uint8_t block, uint16_t address, uint8_t* pBuf, uint16_t len);

    /**
     * Write a 1 byte value to a register.
     * @param address Register address
     * @param wb Write data
     * @return void
     */
    void wizchip_write(uint8_t block, uint16_t address, uint8_t wb);

    /**
     * Write a 2 byte value to a register.
     * @param address Register address
     * @param wb Write data
     * @return void
     */
    void wizchip_write_word(uint8_t block, uint16_t address, uint16_t word);

    /**
     * It writes sequence data to registers.
     * @param address Register address
     * @param pBuf Pointer buffer to write data
     * @param len Data length
     */
    void wizchip_write_buf(uint8_t block, uint16_t address, const uint8_t* pBuf, uint16_t len);

    /**
     * Get @ref Sn_TX_FSR register
     * @return uint16_t. Value of @ref Sn_TX_FSR.
     */
    uint16_t getSn_TX_FSR();

    /**
     * Get @ref Sn_RX_RSR register
     * @return uint16_t. Value of @ref Sn_RX_RSR.
     */
    uint16_t getSn_RX_RSR();


    /**
     * Reset WIZCHIP by softly.
     */
    void wizchip_sw_reset();

    /**
     * Get the link status of phy in WIZCHIP
     */
    int8_t wizphy_getphylink();

    /**
     * Get the power mode of PHY in WIZCHIP
     */
    int8_t wizphy_getphypmode();

    /**
     * Reset Phy
     */
    void wizphy_reset();

    /**
    * set the power mode of phy inside WIZCHIP. Refer to @ref PHYCFGR in W5500, @ref PHYSTATUS in W5200
    * @param pmode Settig value of power down mode.
    */
    int8_t wizphy_setphypmode(uint8_t pmode);

    /**
     * It copies data to internal TX memory
     *
     * @details This function reads the Tx write pointer register and after that,
     * it copies the <i>wizdata(pointer buffer)</i> of the length of <i>len(variable)</i> bytes to internal TX memory
     * and updates the Tx write pointer register.
     * This function is being called by send() and sendto() function also.
     *
     * @param wizdata Pointer buffer to write data
     * @param len Data length
     * @sa wizchip_recv_data()
     */
    void wizchip_send_data(const uint8_t *wizdata, uint16_t len);

    /**
     * It copies data to your buffer from internal RX memory
     *
     * @details This function read the Rx read pointer register and after that,
     * it copies the received data from internal RX memory
     * to <i>wizdata(pointer variable)</i> of the length of <i>len(variable)</i> bytes.
     * This function is being called by recv() also.
     *
     * @param wizdata Pointer buffer to read data
     * @param len Data length
     * @sa wizchip_send_data()
     */
    void wizchip_recv_data(uint8_t *wizdata, uint16_t len);

    /**
     * It discard the received data in RX memory.
     * @details It discards the data of the length of <i>len(variable)</i> bytes in internal RX memory.
     * @param len Data length
     */
    void wizchip_recv_ignore(uint16_t len);



    /** Common registers */
    enum {
        MR = 0x0000,        ///< Mode Register address (R/W)
        SHAR = 0x0009,      ///< Source MAC Register address (R/W)
        INTLEVEL = 0x0013,  ///< Set Interrupt low level timer register address (R/W)
        IR = 0x0015,        ///< Interrupt Register (R/W)
        _IMR_ = 0x0016,     ///< Interrupt mask register (R/W)
        SIR = 0x0017,       ///< Socket Interrupt Register (R/W)
        SIMR = 0x0018,      ///< Socket Interrupt Mask Register (R/W)
        _RTR_ = 0x0019,     ///< Timeout register address (1 is 100us) (R/W)
        _RCR_ = 0x001B,     ///< Retry count register (R/W)
        UIPR = 0x0028,      ///< Unreachable IP register address in UDP mode (R)
        UPORTR = 0x002C,    ///< Unreachable Port register address in UDP mode (R)
        PHYCFGR = 0x002E,   ///< PHY Status Register (R/W)
        VERSIONR = 0x0039,  ///< Chip version register address (R)
    };

    /** Socket registers */
    enum {
        Sn_MR = 0x0000,          ///< Socket Mode register (R/W)
        Sn_CR = 0x0001,          ///< Socket command register (R/W)
        Sn_IR = 0x0002,          ///< Socket interrupt register (R)
        Sn_SR = 0x0003,          ///< Socket status register (R)
        Sn_PORT = 0x0004,        ///< Source port register (R/W)
        Sn_DHAR = 0x0006,        ///< Peer MAC register address (R/W)
        Sn_DIPR = 0x000C,        ///< Peer IP register address (R/W)
        Sn_DPORT = 0x0010,       ///< Peer port register address (R/W)
        Sn_MSSR = 0x0012,        ///< Maximum Segment Size(Sn_MSSR0) register address (R/W)
        Sn_TOS = 0x0015,         ///< IP Type of Service(TOS) Register (R/W)
        Sn_TTL = 0x0016,         ///< IP Time to live(TTL) Register (R/W)
        Sn_RXBUF_SIZE = 0x001E,  ///< Receive memory size register (R/W)
        Sn_TXBUF_SIZE = 0x001F,  ///< Transmit memory size register (R/W)
        Sn_TX_FSR = 0x0020,      ///< Transmit free memory size register (R)
        Sn_TX_RD = 0x0022,       ///< Transmit memory read pointer register address (R)
        Sn_TX_WR = 0x0024,       ///< Transmit memory write pointer register address (R/W)
        Sn_RX_RSR = 0x0026,      ///< Received data size register (R)
        Sn_RX_RD = 0x0028,       ///< Read point of Receive memory (R/W)
        Sn_RX_WR = 0x002A,       ///< Write point of Receive memory (R)
        Sn_IMR = 0x002C,         ///< Socket interrupt mask register (R)
        Sn_FRAG = 0x002D,        ///< Fragment field value in IP header register (R/W)
        Sn_KPALVTR = 0x002F,     ///< Keep Alive Timer register (R/W)
    };

    /** Mode register values */
    enum {
        MR_RST = 0x80,    ///< Reset
        MR_WOL = 0x20,    ///< Wake on LAN
        MR_PB = 0x10,     ///< Ping block
        MR_PPPOE = 0x08,  ///< Enable PPPoE
        MR_FARP = 0x02,   ///< Enable UDP_FORCE_ARP CHECHK
    };

    /* Interrupt Register values */
    enum {
        IR_CONFLICT = 0x80,  ///< Check IP conflict
        IR_UNREACH = 0x40,   ///< Get the destination unreachable message in UDP sending
        IR_PPPoE = 0x20,     ///< Get the PPPoE close message
        IR_MP = 0x10,        ///< Get the magic packet interrupt
    };

    /* Interrupt Mask Register values */
    enum {
        IM_IR7 = 0x80,   ///< IP Conflict Interrupt Mask
        IM_IR6 = 0x40,   ///< Destination unreachable Interrupt Mask
        IM_IR5 = 0x20,   ///< PPPoE Close Interrupt Mask
        IM_IR4 = 0x10,   ///< Magic Packet Interrupt Mask
    };

    /** Socket Mode Register values @ref Sn_MR */
    enum {
        Sn_MR_CLOSE = 0x00,  ///< Unused socket
        Sn_MR_TCP = 0x01,    ///< TCP
        Sn_MR_UDP = 0x02,    ///< UDP
        Sn_MR_MACRAW = 0x04, ///< MAC LAYER RAW SOCK
        Sn_MR_UCASTB = 0x10, ///< Unicast Block in UDP Multicasting
        Sn_MR_ND = 0x20,     ///< No Delayed Ack(TCP), Multicast flag
        Sn_MR_BCASTB = 0x40, ///< Broadcast block in UDP Multicasting
        Sn_MR_MULTI = 0x80,  ///< Support UDP Multicasting
        Sn_MR_MIP6B = 0x10,  ///< IPv6 packet Blocking in @ref Sn_MR_MACRAW mode
        Sn_MR_MMB = 0x20,    ///< Multicast Blocking in @ref Sn_MR_MACRAW mode
        Sn_MR_MFEN = 0x80,   ///< MAC filter enable in @ref Sn_MR_MACRAW mode
    };

    /** Socket Command Register values */
    enum {
        Sn_CR_OPEN = 0x01,      ///< Initialise or open socket
        Sn_CR_LISTEN = 0x02,    ///< Wait connection request in TCP mode (Server mode)
        Sn_CR_CONNECT = 0x04,   ///< Send connection request in TCP mode (Client mode)
        Sn_CR_DISCON = 0x08,    ///< Send closing request in TCP mode
        Sn_CR_CLOSE = 0x10,     ///< Close socket
        Sn_CR_SEND = 0x20,      ///< Update TX buffer pointer and send data
        Sn_CR_SEND_MAC = 0x21,  ///< Send data with MAC address, so without ARP process
        Sn_CR_SEND_KEEP = 0x22, ///< Send keep alive message
        Sn_CR_RECV = 0x40,      ///< Update RX buffer pointer and receive data
    };

    /** Socket Interrupt register values */
    enum {
        Sn_IR_CON = 0x01,      ///< CON Interrupt
        Sn_IR_DISCON = 0x02,   ///< DISCON Interrupt
        Sn_IR_RECV = 0x04,     ///< RECV Interrupt
        Sn_IR_TIMEOUT = 0x08,  ///< TIMEOUT Interrupt
        Sn_IR_SENDOK = 0x10,   ///< SEND_OK Interrupt
    };

    /** Socket Status Register values */
    enum {
        SOCK_CLOSED = 0x00,      ///< Closed
        SOCK_INIT = 0x13,        ///< Initiate state
        SOCK_LISTEN = 0x14,      ///< Listen state
        SOCK_SYNSENT = 0x15,     ///< Connection state
        SOCK_SYNRECV = 0x16,     ///< Connection state
        SOCK_ESTABLISHED = 0x17, ///< Success to connect
        SOCK_FIN_WAIT = 0x18,    ///< Closing state
        SOCK_CLOSING = 0x1A,     ///< Closing state
        SOCK_TIME_WAIT = 0x1B,   ///< Closing state
        SOCK_CLOSE_WAIT = 0x1C,  ///< Closing state
        SOCK_LAST_ACK = 0x1D,    ///< Closing state
        SOCK_UDP = 0x22,         ///< UDP socket
        SOCK_MACRAW = 0x42,      ///< MAC raw mode socket
    };


    /* PHYCFGR register value */
    enum {
        PHYCFGR_RST = ~(1<<7),  //< For PHY reset, must operate AND mask.
        PHYCFGR_OPMD = (1<<6),   // Configre PHY with OPMDC value
        PHYCFGR_OPMDC_ALLA = (7<<3),
        PHYCFGR_OPMDC_PDOWN = (6<<3),
        PHYCFGR_OPMDC_NA = (5<<3),
        PHYCFGR_OPMDC_100FA = (4<<3),
        PHYCFGR_OPMDC_100F = (3<<3),
        PHYCFGR_OPMDC_100H = (2<<3),
        PHYCFGR_OPMDC_10F = (1<<3),
        PHYCFGR_OPMDC_10H = (0<<3),
        PHYCFGR_DPX_FULL = (1<<2),
        PHYCFGR_DPX_HALF = (0<<2),
        PHYCFGR_SPD_100 = (1<<1),
        PHYCFGR_SPD_10 = (0<<1),
        PHYCFGR_LNK_ON = (1<<0),
        PHYCFGR_LNK_OFF = (0<<0),
    };

    enum {
        PHY_SPEED_10 = 0,     ///< Link Speed 10
        PHY_SPEED_100 = 1,    ///< Link Speed 100
        PHY_DUPLEX_HALF = 0,  ///< Link Half-Duplex
        PHY_DUPLEX_FULL = 1,  ///< Link Full-Duplex
        PHY_LINK_OFF = 0,     ///< Link Off
        PHY_LINK_ON = 1,      ///< Link On
        PHY_POWER_NORM = 0,   ///< PHY power normal mode
        PHY_POWER_DOWN = 1,   ///< PHY power down mode
    };


    /**
     * Set Mode Register
     * @param (uint8_t)mr The value to be set.
     * @sa getMR()
     */
    inline void setMR(uint8_t mode) {
        wizchip_write(BlockSelectCReg, MR, mode);
    }

    /**
     * Get Mode Register
     * @return uint8_t. The value of Mode register.
     * @sa setMR()
     */
    inline uint8_t getMR() {
        return wizchip_read(BlockSelectCReg, MR);
    }

    /**
     * Set local MAC address
     * @param (uint8_t*)shar Pointer variable to set local MAC address. It should be allocated 6 bytes.
     * @sa getSHAR()
     */
    inline void setSHAR(const uint8_t* macaddr) {
        wizchip_write_buf(BlockSelectCReg, SHAR, macaddr, 6);
    }

    /**
     * Get local MAC address
     * @param (uint8_t*)shar Pointer variable to get local MAC address. It should be allocated 6 bytes.
     * @sa setSHAR()
     */
    inline void getSHAR(uint8_t* macaddr) {
        wizchip_read_buf(BlockSelectCReg, SHAR, macaddr, 6);
    }

    /**
     * Set @ref IR register
     * @param (uint8_t)ir Value to set @ref IR register.
     * @sa getIR()
     */
    inline void setIR(uint8_t ir) {
        wizchip_write(BlockSelectCReg, IR, (ir & 0xF0));
    }

    /**
     * Get @ref IR register
     * @return uint8_t. Value of @ref IR register.
     * @sa setIR()
     */
    inline uint8_t getIR() {
        return wizchip_read(BlockSelectCReg, IR) & 0xF0;
    }

    /**
     * Set @ref _IMR_ register
     * @param (uint8_t)imr Value to set @ref _IMR_ register.
     * @sa getIMR()
     */
    inline void setIMR(uint8_t imr) {
        wizchip_write(BlockSelectCReg, _IMR_, imr);
    }

    /**
     * Get @ref _IMR_ register
     * @return uint8_t. Value of @ref _IMR_ register.
     * @sa setIMR()
     */
    inline uint8_t getIMR() {
        return wizchip_read(BlockSelectCReg, _IMR_);
    }

    /**
     * Set @ref PHYCFGR register
     * @param (uint8_t)phycfgr Value to set @ref PHYCFGR register.
     * @sa getPHYCFGR()
     */
    inline void setPHYCFGR(uint8_t phycfgr) {
        wizchip_write(BlockSelectCReg, PHYCFGR, phycfgr);
    }

    /**
     * Get @ref PHYCFGR register
     * @return uint8_t. Value of @ref PHYCFGR register.
     * @sa setPHYCFGR()
     */
    inline uint8_t getPHYCFGR() {
        return wizchip_read(BlockSelectCReg, PHYCFGR);
    }

    /**
     * Get @ref VERSIONR register
     * @return uint8_t. Value of @ref VERSIONR register.
     */
    inline uint8_t getVERSIONR() {
        return wizchip_read(BlockSelectCReg, VERSIONR);
    }

    /**
     * Set @ref Sn_MR register
     * @param (uint8_t)mr Value to set @ref Sn_MR
     * @sa getSn_MR()
     */
    inline void setSn_MR(uint8_t mr) {
        wizchip_write(BlockSelectSReg, Sn_MR, mr);
    }

    /**
     * Get @ref Sn_MR register
     * @return uint8_t. Value of @ref Sn_MR.
     * @sa setSn_MR()
     */
    inline uint8_t getSn_MR() {
        return wizchip_read(BlockSelectSReg, Sn_MR);
    }

    /**
     * Set @ref Sn_CR register, then wait for the command to execute
     * @param (uint8_t)cr Value to set @ref Sn_CR
     * @sa getSn_CR()
     */
    void setSn_CR(uint8_t cr);

    /**
     * Get @ref Sn_CR register
     * @return uint8_t. Value of @ref Sn_CR.
     * @sa setSn_CR()
     */
    inline uint8_t getSn_CR() {
        return wizchip_read(BlockSelectSReg, Sn_CR);
    }

    /**
     * Set @ref Sn_IR register
     * @param (uint8_t)ir Value to set @ref Sn_IR
     * @sa getSn_IR()
     */
    inline void setSn_IR(uint8_t ir) {
        wizchip_write(BlockSelectSReg, Sn_IR, (ir & 0x1F));
    }

    /**
     * Get @ref Sn_IR register
     * @return uint8_t. Value of @ref Sn_IR.
     * @sa setSn_IR()
     */
    inline uint8_t getSn_IR() {
        return (wizchip_read(BlockSelectSReg, Sn_IR) & 0x1F);
    }

    /**
     * Set @ref Sn_IMR register
     * @param (uint8_t)imr Value to set @ref Sn_IMR
     * @sa getSn_IMR()
     */
    inline void setSn_IMR(uint8_t imr) {
        wizchip_write(BlockSelectSReg, Sn_IMR, (imr & 0x1F));
    }

    /**
     * Get @ref Sn_IMR register
     * @return uint8_t. Value of @ref Sn_IMR.
     * @sa setSn_IMR()
     */
    inline uint8_t getSn_IMR() {
        return (wizchip_read(BlockSelectSReg, Sn_IMR) & 0x1F);
    }

    /**
     * Get @ref Sn_SR register
     * @return uint8_t. Value of @ref Sn_SR.
     */
    inline uint8_t getSn_SR() {
        return wizchip_read(BlockSelectSReg, Sn_SR);
    }

    /**
     * Set @ref Sn_RXBUF_SIZE register
     * @param (uint8_t)rxbufsize Value to set @ref Sn_RXBUF_SIZE
     * @sa getSn_RXBUF_SIZE()
     */
    inline void setSn_RXBUF_SIZE(uint8_t rxbufsize) {
        wizchip_write(BlockSelectSReg, Sn_RXBUF_SIZE, rxbufsize);
    }

    /**
     * Get @ref Sn_RXBUF_SIZE register
     * @return uint8_t. Value of @ref Sn_RXBUF_SIZE.
     * @sa setSn_RXBUF_SIZE()
     */
    inline uint8_t getSn_RXBUF_SIZE() {
        return wizchip_read(BlockSelectSReg, Sn_RXBUF_SIZE);
    }

    /**
     * Set @ref Sn_TXBUF_SIZE register
     * @param (uint8_t)txbufsize Value to set @ref Sn_TXBUF_SIZE
     * @sa getSn_TXBUF_SIZE()
     */
    inline void setSn_TXBUF_SIZE(uint8_t txbufsize) {
        wizchip_write(BlockSelectSReg, Sn_TXBUF_SIZE, txbufsize);
    }

    /**
     * Get @ref Sn_TXBUF_SIZE register
     * @return uint8_t. Value of @ref Sn_TXBUF_SIZE.
     * @sa setSn_TXBUF_SIZE()
     */
    inline uint8_t getSn_TXBUF_SIZE() {
        return wizchip_read(BlockSelectSReg, Sn_TXBUF_SIZE);
    }

    /**
     * Get @ref Sn_TX_RD register
     * @return uint16_t. Value of @ref Sn_TX_RD.
     */
    inline uint16_t getSn_TX_RD() {
        return wizchip_read_word(BlockSelectSReg, Sn_TX_RD);
    }

    /**
     * Set @ref Sn_TX_WR register
     * @param (uint16_t)txwr Value to set @ref Sn_TX_WR
     * @sa GetSn_TX_WR()
     */
    inline void setSn_TX_WR(uint16_t txwr) {
        wizchip_write_word(BlockSelectSReg, Sn_TX_WR, txwr);
    }

    /**
     * Get @ref Sn_TX_WR register
     * @return uint16_t. Value of @ref Sn_TX_WR.
     * @sa setSn_TX_WR()
     */
    inline uint16_t getSn_TX_WR() {
        return wizchip_read_word(BlockSelectSReg, Sn_TX_WR);
    }

    /**
     * Set @ref Sn_RX_RD register
     * @param (uint16_t)rxrd Value to set @ref Sn_RX_RD
     * @sa getSn_RX_RD()
     */
    inline void setSn_RX_RD(uint16_t rxrd) {
        wizchip_write_word(BlockSelectSReg, Sn_RX_RD, rxrd);
    }

    /**
     * Get @ref Sn_RX_RD register
     * @return uint16_t. Value of @ref Sn_RX_RD.
     * @sa setSn_RX_RD()
     */
    inline uint16_t getSn_RX_RD() {
        return wizchip_read_word(BlockSelectSReg, Sn_RX_RD);
    }

    /**
     * Get @ref Sn_RX_WR register
     * @return uint16_t. Value of @ref Sn_RX_WR.
     */
    inline uint16_t getSn_RX_WR() {
        return wizchip_read_word(BlockSelectSReg, Sn_RX_WR);
    }
};

#endif // W5500_H
