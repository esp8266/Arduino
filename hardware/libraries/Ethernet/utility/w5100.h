/*
@file		w5100.h
*/

#ifndef	_W5100_H_
#define	_W5100_H_


#define MR __DEF_IINCHIP_MAP_BASE__
#define IDM_OR ((__DEF_IINCHIP_MAP_BASE__ + 0x00))
#define IDM_AR0 ((__DEF_IINCHIP_MAP_BASE__ + 0x01))
#define IDM_AR1 ((__DEF_IINCHIP_MAP_BASE__ + 0x02))
#define IDM_DR ((__DEF_IINCHIP_MAP_BASE__ + 0x03))


/**
 @brief Gateway IP Register address
 */
#define GAR0				(COMMON_BASE + 0x0001)
/**
 @brief Subnet mask Register address
 */
#define SUBR0			(COMMON_BASE + 0x0005)
/**
 @brief Source MAC Register address
 */
#define SHAR0				(COMMON_BASE + 0x0009)
/**
 @brief Source IP Register address
 */
#define SIPR0				(COMMON_BASE + 0x000F)
/**
 @brief Interrupt Register
 */
#define IR					(COMMON_BASE + 0x0015)
/**
 @brief Interrupt mask register
 */
#define IMR					(COMMON_BASE + 0x0016)
/**
 @brief Timeout register address( 1 is 100us )
 */
#define RTR0				(COMMON_BASE + 0x0017)
/**
 @brief Retry count reigster
 */
#define RCR						(COMMON_BASE + 0x0019)
/**
 @brief Receive memory size reigster
 */
#define RMSR			(COMMON_BASE + 0x001A)
/**
 @brief Transmit memory size reigster
 */
#define TMSR			(COMMON_BASE + 0x001B)
/**
 @brief Authentication type register address in PPPoE mode
 */
#define PATR0					(COMMON_BASE + 0x001C)
//#define PPPALGO (COMMON_BASE + 0x001D)
#define PTIMER (COMMON_BASE + 0x0028)
#define PMAGIC (COMMON_BASE + 0x0029)

/**
 @brief Unreachable IP register address in UDP mode
 */
#define UIPR0				(COMMON_BASE + 0x002A)
/**
 @brief Unreachable Port register address in UDP mode
 */
#define UPORT0			(COMMON_BASE + 0x002E)

/**
 @brief socket register
*/
#define CH_BASE (COMMON_BASE + 0x0400)
/**
 @brief	size of each channel register map
 */
#define CH_SIZE		0x0100
/**
 @brief socket Mode register
 */
#define Sn_MR(ch)		(CH_BASE + ch * CH_SIZE + 0x0000)
/**
 @brief channel Sn_CR register
 */
#define Sn_CR(ch)				(CH_BASE + ch * CH_SIZE + 0x0001)
/**
 @brief channel interrupt register
 */
#define Sn_IR(ch)			(CH_BASE + ch * CH_SIZE + 0x0002)
/**
 @brief channel status register
 */
#define Sn_SR(ch)			(CH_BASE + ch * CH_SIZE + 0x0003)
/**
 @brief source port register
 */
#define Sn_PORT0(ch)		(CH_BASE + ch * CH_SIZE + 0x0004)
/**
 @brief Peer MAC register address
 */
#define Sn_DHAR0(ch)			(CH_BASE + ch * CH_SIZE + 0x0006)
/**
 @brief Peer IP register address
 */
#define Sn_DIPR0(ch)			(CH_BASE + ch * CH_SIZE + 0x000C)
/**
 @brief Peer port register address
 */
#define Sn_DPORT0(ch)		(CH_BASE + ch * CH_SIZE + 0x0010)
/**
 @brief Maximum Segment Size(Sn_MSSR0) register address
 */
#define Sn_MSSR0(ch)					(CH_BASE + ch * CH_SIZE + 0x0012)
/**
 @brief Protocol of IP Header field register in IP raw mode
 */
#define Sn_PROTO(ch)			(CH_BASE + ch * CH_SIZE + 0x0014)

/** 
 @brief IP Type of Service(TOS) Register 
 */
#define Sn_TOS(ch)						(CH_BASE + ch * CH_SIZE + 0x0015)
/**
 @brief IP Time to live(TTL) Register 
 */
#define Sn_TTL(ch)						(CH_BASE + ch * CH_SIZE + 0x0016)

/**
 @brief Transmit free memory size register
 */
#define Sn_TX_FSR0(ch)	(CH_BASE + ch * CH_SIZE + 0x0020)
/**
 @brief Transmit memory read pointer register address
 */
#define Sn_TX_RD0(ch)			(CH_BASE + ch * CH_SIZE + 0x0022)
/**
 @brief Transmit memory write pointer register address
 */
#define Sn_TX_WR0(ch)			(CH_BASE + ch * CH_SIZE + 0x0024)
/**
 @brief Received data size register
 */
#define Sn_RX_RSR0(ch)	(CH_BASE + ch * CH_SIZE + 0x0026)
/**
 @brief Read point of Receive memory
 */
#define Sn_RX_RD0(ch)			(CH_BASE + ch * CH_SIZE + 0x0028)
/**
 @brief Write point of Receive memory
 */
#define Sn_RX_WR0(ch)			(CH_BASE + ch * CH_SIZE + 0x002A)



/* MODE register values */
#define MR_RST			0x80 /**< reset */
#define MR_PB			0x10 /**< ping block */
#define MR_PPPOE		0x08 /**< enable pppoe */
#define MR_LB  		0x04 /**< little or big endian selector in indirect mode */
#define MR_AI			0x02 /**< auto-increment in indirect mode */
#define MR_IND			0x01 /**< enable indirect mode */

/* IR register values */
#define IR_CONFLICT	0x80 /**< check ip confict */
#define IR_UNREACH	0x40 /**< get the destination unreachable message in UDP sending */
#define IR_PPPoE		0x20 /**< get the PPPoE close message */
#define IR_SOCK(ch)	(0x01 << ch) /**< check socket interrupt */

/* Sn_MR values */
#define Sn_MR_CLOSE		0x00		/**< unused socket */
#define Sn_MR_TCP		0x01		/**< TCP */
#define Sn_MR_UDP		0x02		/**< UDP */
#define Sn_MR_IPRAW	0x03		/**< IP LAYER RAW SOCK */
#define Sn_MR_MACRAW	0x04		/**< MAC LAYER RAW SOCK */
#define Sn_MR_PPPOE		0x05		/**< PPPoE */
#define Sn_MR_ND		0x20		/**< No Delayed Ack(TCP) flag */
#define Sn_MR_MULTI		0x80		/**< support multicating */


/* Sn_CR values */
#define Sn_CR_OPEN		0x01		/**< initialize or open socket */
#define Sn_CR_LISTEN		0x02		/**< wait connection request in tcp mode(Server mode) */
#define Sn_CR_CONNECT	0x04		/**< send connection request in tcp mode(Client mode) */
#define Sn_CR_DISCON		0x08		/**< send closing reqeuset in tcp mode */
#define Sn_CR_CLOSE		0x10		/**< close socket */
#define Sn_CR_SEND		0x20		/**< updata txbuf pointer, send data */
#define Sn_CR_SEND_MAC	0x21		/**< send data with MAC address, so without ARP process */
#define Sn_CR_SEND_KEEP	0x22		/**<  send keep alive message */
#define Sn_CR_RECV		0x40		/**< update rxbuf pointer, recv data */

#ifdef __DEF_IINCHIP_PPP__
	#define Sn_CR_PCON				0x23		 
	#define Sn_CR_PDISCON			0x24		 
	#define Sn_CR_PCR					0x25		 
	#define Sn_CR_PCN					0x26		
	#define Sn_CR_PCJ					0x27		
#endif

/* Sn_IR values */
#ifdef __DEF_IINCHIP_PPP__
	#define Sn_IR_PRECV			0x80		
	#define Sn_IR_PFAIL			0x40		
	#define Sn_IR_PNEXT			0x20		
#endif
#define Sn_IR_SEND_OK			0x10		/**< complete sending */
#define Sn_IR_TIMEOUT			0x08		/**< assert timeout */
#define Sn_IR_RECV				0x04		/**< receiving data */
#define Sn_IR_DISCON				0x02		/**< closed socket */
#define Sn_IR_CON					0x01		/**< established connection */

/* Sn_SR values */
#define SOCK_CLOSED				0x00		/**< closed */
#define SOCK_INIT 				0x13		/**< init state */
#define SOCK_LISTEN				0x14		/**< listen state */
#define SOCK_SYNSENT	   		0x15		/**< connection state */
#define SOCK_SYNRECV		   	0x16		/**< connection state */
#define SOCK_ESTABLISHED		0x17		/**< success to connect */
#define SOCK_FIN_WAIT			0x18		/**< closing state */
#define SOCK_CLOSING		   	0x1A		/**< closing state */
#define SOCK_TIME_WAIT			0x1B		/**< closing state */
#define SOCK_CLOSE_WAIT			0x1C		/**< closing state */
#define SOCK_LAST_ACK			0x1D		/**< closing state */
#define SOCK_UDP				   0x22		/**< udp socket */
#define SOCK_IPRAW			   0x32		/**< ip raw mode socket */
#define SOCK_MACRAW			   0x42		/**< mac raw mode socket */
#define SOCK_PPPOE				0x5F		/**< pppoe socket */

/* IP PROTOCOL */
#define IPPROTO_IP              0           /**< Dummy for IP */
#define IPPROTO_ICMP            1           /**< Control message protocol */
#define IPPROTO_IGMP            2           /**< Internet group management protocol */
#define IPPROTO_GGP             3           /**< Gateway^2 (deprecated) */
#define IPPROTO_TCP             6           /**< TCP */
#define IPPROTO_PUP             12          /**< PUP */
#define IPPROTO_UDP             17          /**< UDP */
#define IPPROTO_IDP             22          /**< XNS idp */
#define IPPROTO_ND              77          /**< UNOFFICIAL net disk protocol */
#define IPPROTO_RAW             255         /**< Raw IP packet */


/*********************************************************
* iinchip access function
*********************************************************/
extern uint8 IINCHIP_READ(uint16 addr);
extern uint8 IINCHIP_WRITE(uint16 addr,uint8 data);
extern uint16 wiz_read_buf(uint16 addr, uint8* buf,uint16 len);
extern uint16 wiz_write_buf(uint16 addr,uint8* buf,uint16 len);

extern void iinchip_init(void); // reset iinchip
extern void sysinit(uint8 tx_size, uint8 rx_size); // setting tx/rx buf size
extern uint8 getISR(uint8 s);
extern void putISR(uint8 s, uint8 val);
extern uint16 getIINCHIP_RxMAX(uint8 s);
extern uint16 getIINCHIP_TxMAX(uint8 s);
extern uint16 getIINCHIP_RxMASK(uint8 s);
extern uint16 getIINCHIP_TxMASK(uint8 s);
extern uint16 getIINCHIP_RxBASE(uint8 s);
extern uint16 getIINCHIP_TxBASE(uint8 s);
extern void setGAR(uint8 * addr); // set gateway address
extern void setSUBR(uint8 * addr); // set subnet mask address
extern void setSHAR(uint8 * addr); // set local MAC address
extern void setSIPR(uint8 * addr); // set local IP address
extern void setRTR(uint16 timeout); // set retry duration for data transmission, connection, closing ...
extern void setRCR(uint8 retry); // set retry count (above the value, assert timeout interrupt)
extern void setIMR(uint8 mask); // set interrupt mask. 
extern void getGAR(uint8 * addr);
extern void getSUBR(uint8 * addr);
extern void getSHAR(uint8 * addr);
extern void getSIPR(uint8 * addr);
extern uint8 getIR( void );
extern void setSn_MSS(SOCKET s, uint16 Sn_MSSR0); // set maximum segment size
extern void setSn_PROTO(SOCKET s, uint8 proto); // set IP Protocol value using IP-Raw mode
extern uint8 getSn_IR(SOCKET s); // get socket interrupt status
extern uint8 getSn_SR(SOCKET s); // get socket status
extern uint16 getSn_TX_FSR(SOCKET s); // get socket TX free buf size
extern uint16 getSn_RX_RSR(SOCKET s); // get socket RX recv buf size
extern void setSn_DHAR(SOCKET s, uint8 * addr);
extern void setSn_DIPR(SOCKET s, uint8 * addr);
extern void setSn_DPORT(SOCKET s, uint8 * addr);
extern void getSn_DHAR(SOCKET s, uint8 * addr);
extern void getSn_DIPR(SOCKET s, uint8 * addr);
extern void getSn_DPORT(SOCKET s, uint8 * addr);
extern void setSn_TTL(SOCKET s, uint8 ttl);
extern void setMR(uint8 val);

#ifdef __DEF_IINCHIP_PPP__
extern uint8 pppinit(uint8 *id, uint8 idlen, uint8 *passwd, uint8 passwdlen);
extern uint8 pppterm(uint8 *mac,uint8 *sessionid);
#endif

extern void send_data_processing(SOCKET s, uint8 *data, uint16 len);
extern void recv_data_processing(SOCKET s, uint8 *data, uint16 len);
extern void read_data(SOCKET s, vuint8 * src, vuint8 * dst, uint16 len);
extern void write_data(SOCKET s, vuint8 * src, vuint8 * dst, uint16 len);

#endif
