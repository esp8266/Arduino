//-----------------------------------------------------------------------------
//AVR Mega168 SPI HAL
#define BIT0							0x01
#define BIT1							0x02
#define BIT2							0x04
#define BIT3							0x08
#define BIT4							0x10
#define BIT5							0x20
#define BIT6							0x40
#define BIT7							0x80

#define SPI0_SS_BIT						BIT2
#define SPI0_SS_DDR						DDRB
#define SPI0_SS_PORT					PORTB

#define SPI0_SCLK_BIT					BIT5
#define SPI0_SCLK_DDR					DDRB
#define SPI0_SCLK_PORT					PORTB

#define	SPI0_MOSI_BIT					BIT3
#define SPI0_MOSI_DDR					DDRB
#define SPI0_MOSI_PORT					PORTB

#define	SPI0_MISO_BIT					BIT4
#define SPI0_MISO_DDR					DDRB
#define SPI0_MISO_PORT					PORTB


#define SPI0_WaitForReceive()				
#define SPI0_RxData()	 				(SPDR)

#define SPI0_TxData(Data)				(SPDR = Data)
#define SPI0_WaitForSend()				while( (SPSR & 0x80)==0x00 )

#define SPI0_SendByte(Data)				SPI0_TxData(Data);SPI0_WaitForSend()
#define SPI0_RecvBute()					SPI0_RxData()

// PB4(MISO), PB3(MOSI), PB5(SCK), PB2(/SS)         // CS=1, waiting for SPI start // SPI mode 0, 4MHz
#define SPI0_Init()						DDRB  |= SPI0_SS_BIT|SPI0_SCLK_BIT|SPI0_MOSI_BIT;\
										PORTB |= SPI0_SS_BIT; PORTB &= ~(SPI0_SCLK_BIT|SPI0_MOSI_BIT);\
										SPCR  = 0x50  
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//IInChip SPI HAL
#define IINCHIP_SpiInit					SPI0_Init
#define IINCHIP_SpiSendData				SPI0_SendByte	
#define IINCHIP_SpiRecvData				SPI0_RxData


#define IINCHIP_CS_BIT					BIT2
#define IINCHIP_CS_DDR					DDRB
#define IINCHIP_CS_PORT					PORTB

#define IINCHIP_CSInit()					(IINCHIP_CS_DDR |= IINCHIP_CS_BIT)
#define IINCHIP_CSon()					(IINCHIP_CS_PORT |= IINCHIP_CS_BIT)
#define IINCHIP_CSoff()					(IINCHIP_CS_PORT &= ~IINCHIP_CS_BIT)
//-----------------------------------------------------------------------------
