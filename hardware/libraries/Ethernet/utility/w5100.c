/*
 * (c)COPYRIGHT
 * ALL RIGHT RESERVED
 *
 * FileName : w5100.c
 * Revision History :
 * ----------	-------		------------------------------------------------
 * 	Date			version	  	Description
 * ----------	-------  	------------------------------------------------
 * 01/25/2007	1.1			Bug is Fixed in the Indirect Mode 
 *							: Memory mapping error	
 * ----------	-------		------------------------------------------------
 * 01/08/2008	1.2			Modification of Socket Command Part
 *							: Check if the appropriately performed after writing Sn_CR
 *
 *							Modification of SPI Part
 *							: SPI code changed by adding 'spi.h'.
 *							: Change control type for SPI port from byte to bit.
 * ----------	-------		------------------------------------------------
 * 01/15/2008	1.3			Bug is Fixed in the pppinit() fuction.
 *							: do not clear interrupt value, so fixed.
 *
 *		                   			Modification of ISR
 *                   				: Do not exit ISR, if there is interrupt.
 * ----------	-------		------------------------------------------------
 * 03/21/2008	1.4			Modification of SetMR() function
 *                   				: Use IINCHIP_WRITE() function in Direct or SPI mode.
 * ----------	-------		------------------------------------------------
 */
#include <stdio.h>
#include <string.h>

#include <avr/interrupt.h>
// #include <avr/io.h> 
   
#include "types.h"
#include "socket.h"
#include "w5100.h"



#ifdef __DEF_IINCHIP_PPP__
   #include "md5.h"
#endif


#if (__DEF_IINCHIP_BUS__ == __DEF_IINCHIP_SPI_MODE__)
#include "spi.h"		//+2007113[jhpark]
#endif

static uint8 I_STATUS[MAX_SOCK_NUM];
static uint16 SMASK[MAX_SOCK_NUM]; /**< Variable for Tx buffer MASK in each channel */
static uint16 RMASK[MAX_SOCK_NUM]; /**< Variable for Rx buffer MASK in each channel */
static uint16 SSIZE[MAX_SOCK_NUM]; /**< Max Tx buffer size by each channel */
static uint16 RSIZE[MAX_SOCK_NUM]; /**< Max Rx buffer size by each channel */
static uint16 SBUFBASEADDRESS[MAX_SOCK_NUM]; /**< Tx buffer base address by each channel */
static uint16 RBUFBASEADDRESS[MAX_SOCK_NUM]; /**< Rx buffer base address by each channel */

uint8 getISR(uint8 s)
{
	return I_STATUS[s];
}

void putISR(uint8 s, uint8 val)
{
   I_STATUS[s] = val;
}

uint16 getIINCHIP_RxMAX(uint8 s)
{
   return RSIZE[s];
}
uint16 getIINCHIP_TxMAX(uint8 s)
{
   return SSIZE[s];
}
uint16 getIINCHIP_RxMASK(uint8 s)
{
   return RMASK[s];
}
uint16 getIINCHIP_TxMASK(uint8 s)
{
   return SMASK[s];
}
uint16 getIINCHIP_RxBASE(uint8 s)
{
   return RBUFBASEADDRESS[s];
}
uint16 getIINCHIP_TxBASE(uint8 s)
{
   return SBUFBASEADDRESS[s];
}

 /**
@brief	This function writes the data into W5100 registers.
*/
uint8 IINCHIP_WRITE(uint16 addr,uint8 data)
{
// DIRECT MODE I/F
#if (__DEF_IINCHIP_BUS__ == __DEF_IINCHIP_DIRECT_MODE__)
	IINCHIP_ISR_DISABLE();
	*((vuint8*)(addr)) = data;
	IINCHIP_ISR_ENABLE();
#elif(__DEF_IINCHIP_BUS__ == __DEF_IINCHIP_INDIRECT_MODE__)	/* INDIRECT MODE I/F */
	   IINCHIP_ISR_DISABLE();
	   *((vuint8*)IDM_AR0) = (uint8)((addr & 0xFF00) >> 8);
	   *((vuint8*)IDM_AR1) = (uint8)(addr & 0x00FF);
	   *((vuint8*)IDM_DR)  = data;
	   IINCHIP_ISR_ENABLE();
#elif (__DEF_IINCHIP_BUS__ == __DEF_IINCHIP_SPI_MODE__)
	   IINCHIP_ISR_DISABLE();
	IINCHIP_SpiInit();
	
	//SPI MODE I/F
	IINCHIP_CSoff();                             // CS=0, SPI start

	IINCHIP_SpiSendData(0xF0);
	IINCHIP_SpiSendData((addr & 0xFF00) >> 8);
	IINCHIP_SpiSendData(addr & 0x00FF);
	IINCHIP_SpiSendData(data);

	IINCHIP_CSon();    

	   IINCHIP_ISR_ENABLE();   
#else
	#error "unknown bus type"
#endif
	return 1;
}


/**
@brief	This function reads the value from W5100 registers.
*/
uint8 IINCHIP_READ(uint16 addr)
{
	uint8 data;

// DIRECT MODE I/F

#if (__DEF_IINCHIP_BUS__ == __DEF_IINCHIP_DIRECT_MODE__)
	IINCHIP_ISR_DISABLE();
	data = *((vuint8*)(addr));
	IINCHIP_ISR_ENABLE();
#elif(__DEF_IINCHIP_BUS__ == __DEF_IINCHIP_INDIRECT_MODE__)   
	   IINCHIP_ISR_DISABLE();
	   *((vuint8*)IDM_AR0) = (uint8)((addr & 0xFF00) >> 8);
	   *((vuint8*)IDM_AR1) = (uint8)(addr & 0x00FF);
	   data = *((vuint8*)IDM_DR);
	   IINCHIP_ISR_ENABLE();
   
#elif (__DEF_IINCHIP_BUS__ == __DEF_IINCHIP_SPI_MODE__)
	   IINCHIP_ISR_DISABLE();
	IINCHIP_SpiInit();	
	IINCHIP_CSoff();                             // CS=0, SPI start

	IINCHIP_SpiSendData(0x0F);
	IINCHIP_SpiSendData((addr & 0xFF00) >> 8);
	IINCHIP_SpiSendData(addr & 0x00FF);
	
	
	IINCHIP_SpiSendData(0);
	data = IINCHIP_SpiRecvData();

	IINCHIP_CSon();                          	// SPI end
	   IINCHIP_ISR_ENABLE();
#else
	#error "unknown bus type"
#endif
	return data;
}


/**
@brief	This function writes into W5100 memory(Buffer)
*/ 
uint16 wiz_write_buf(uint16 addr,uint8* buf,uint16 len)
{
#if (__DEF_IINCHIP_BUS__ == __DEF_IINCHIP_DIRECT_MODE__)
	IINCHIP_ISR_DISABLE();
   	memcpy((uint8 *)addr, buf, len);
	IINCHIP_ISR_ENABLE();
#elif (__DEF_IINCHIP_BUS__ == __DEF_IINCHIP_INDIRECT_MODE__)
	   uint16 idx = 0;
	   IINCHIP_ISR_DISABLE();
	   *((vuint8*)IDM_AR0) = (uint8)((addr & 0xFF00) >> 8);
	   *((vuint8*)IDM_AR1) = (uint8)(addr & 0x00FF);
	   for (idx = 0; idx < len ; idx++) *((vuint8*)IDM_DR) = buf[idx];
	   IINCHIP_ISR_ENABLE();
#elif (__DEF_IINCHIP_BUS__ == __DEF_IINCHIP_SPI_MODE__)
	uint16 idx = 0;
	
	   IINCHIP_ISR_DISABLE();
	IINCHIP_SpiInit();
      
	//SPI MODE I/F
	for(idx=0;idx<len;idx++)
	   {
		IINCHIP_CSoff();                             // CS=0, SPI start 

		IINCHIP_SpiSendData(0xF0);
		IINCHIP_SpiSendData(((addr+idx) & 0xFF00) >> 8);
		IINCHIP_SpiSendData((addr+idx) & 0x00FF);
		IINCHIP_SpiSendData(buf[idx]);

		IINCHIP_CSon();                             // CS=0, SPI end 
	   }

	   IINCHIP_ISR_ENABLE();	   
#else
	#error "unknown bus type"
#endif
	return len;
}


/**
@brief	This function reads into W5100 memory(Buffer)
*/ 
uint16 wiz_read_buf(uint16 addr, uint8* buf,uint16 len)
{
#if (__DEF_IINCHIP_BUS__ == __DEF_IINCHIP_DIRECT_MODE__)
	IINCHIP_ISR_DISABLE();
	memcpy(buf, (uint8 *)addr, len);
	IINCHIP_ISR_ENABLE();
#elif(__DEF_IINCHIP_BUS__ == __DEF_IINCHIP_INDIRECT_MODE__)
	   uint16 idx = 0;
	   IINCHIP_ISR_DISABLE();
	   *((vuint8*)IDM_AR0) = (uint8)((addr & 0xFF00) >> 8);
	   *((vuint8*)IDM_AR1) = (uint8)(addr & 0x00FF);
	   for (idx = 0; idx < len ; idx++) buf[idx] = *((vuint8*)IDM_DR);
	   IINCHIP_ISR_ENABLE();
#elif (__DEF_IINCHIP_BUS__ == __DEF_IINCHIP_SPI_MODE__)
	uint16 idx = 0;
	   IINCHIP_ISR_DISABLE();

	IINCHIP_SpiInit();
   
	for (idx=0; idx<len; idx++)
      	   {
		IINCHIP_CSoff();                             // CS=0, SPI start 

		IINCHIP_SpiSendData(0x0F);
		IINCHIP_SpiSendData(((addr+idx) & 0xFF00) >> 8);
		IINCHIP_SpiSendData((addr+idx) & 0x00FF);


		IINCHIP_SpiSendData(0);
		buf[idx] = IINCHIP_SpiRecvData();

		IINCHIP_CSon();                             // CS=0, SPI end 	   
	   }

	   IINCHIP_ISR_ENABLE();
#else
	#error "unknown bus type"
#endif
	return len;
}


/**
@brief	Socket interrupt routine
*/ 
#ifdef __DEF_IINCHIP_INT__
ISR(INT4_vect)
{
	uint8 int_val;
	IINCHIP_ISR_DISABLE();
	int_val = IINCHIP_READ(IR);
	
	/* +200801[bj] process all of interupt */
   do {
   /*---*/
   
   	if (int_val & IR_CONFLICT)
   	{
   		printf("IP conflict : %.2x\r\n", int_val);
   	}
   	if (int_val & IR_UNREACH)
   	{
   		printf("INT Port Unreachable : %.2x\r\n", int_val);
   		printf("UIPR0 : %d.%d.%d.%d\r\n", IINCHIP_READ(UIPR0), IINCHIP_READ(UIPR0+1), IINCHIP_READ(UIPR0+2), IINCHIP_READ(UIPR0+3));
   		printf("UPORT0 : %.2x %.2x\r\n", IINCHIP_READ(UPORT0), IINCHIP_READ(UPORT0+1));
   	}
   
   	/* +200801[bj] interrupt clear */
   	IINCHIP_WRITE(IR, 0xf0); 
      /*---*/
   
   	if (int_val & IR_SOCK(0))
   	{
   	/* +-200801[bj] save interrupt value*/
   		I_STATUS[0] |= IINCHIP_READ(Sn_IR(0)); // can be come to over two times.
   		IINCHIP_WRITE(Sn_IR(0), I_STATUS[0]);
      /*---*/
   	}
   	if (int_val & IR_SOCK(1))
   	{
   	/* +-200801[bj] save interrupt value*/
   		I_STATUS[1] |= IINCHIP_READ(Sn_IR(1));
   		IINCHIP_WRITE(Sn_IR(1), I_STATUS[1]);
      /*---*/
   	}
   	if (int_val & IR_SOCK(2))
   	{
   	/* +-200801[bj] save interrupt value*/
   		I_STATUS[2] |= IINCHIP_READ(Sn_IR(2));
   		IINCHIP_WRITE(Sn_IR(2), I_STATUS[2]);
      /*---*/
   	}
   	if (int_val & IR_SOCK(3))
   	{
   	/* +-200801[bj] save interrupt value*/
   		I_STATUS[3] |= IINCHIP_READ(Sn_IR(3));
   		IINCHIP_WRITE(Sn_IR(3), I_STATUS[3]);
      /*---*/
   	}
   
   	/* +-200801[bj] re-read interrupt value*/
   	int_val = IINCHIP_READ(IR);

	/* +200801[bj] if exist, contiue to process */
   } while (int_val != 0x00);
   /*---*/

	IINCHIP_ISR_ENABLE();
}
#endif

/**
@brief	This function is for resetting of the iinchip. Initializes the iinchip to work in whether DIRECT or INDIRECT mode
*/ 
void iinchip_init(void)
{	
	setMR( MR_RST );
#if (__DEF_IINCHIP_BUS__ == __DEF_IINCHIP_INDIRECT_MODE__)
	setMR( MR_IND | MR_AI );
#ifdef __DEF_IINCHIP_DBG__	
	printf("MR value is %d \r\n",IINCHIP_READ(MR));
#endif	
#endif
}


/**
@brief	This function set the transmit & receive buffer size as per the channels is used

Note for TMSR and RMSR bits are as follows\n
bit 1-0 : memory size of channel #0 \n
bit 3-2 : memory size of channel #1 \n
bit 5-4 : memory size of channel #2 \n
bit 7-6 : memory size of channel #3 \n\n
Maximum memory size for Tx, Rx in the W5100 is 8K Bytes,\n
In the range of 8KBytes, the memory size could be allocated dynamically by each channel.\n
Be attentive to sum of memory size shouldn't exceed 8Kbytes\n
and to data transmission and receiption from non-allocated channel may cause some problems.\n
If the 8KBytes memory is already  assigned to centain channel, \n
other 3 channels couldn't be used, for there's no available memory.\n
If two 4KBytes memory are assigned to two each channels, \n
other 2 channels couldn't be used, for there's no available memory.\n
*/ 
void sysinit(
	uint8 tx_size, 	/**< tx_size Tx memory size (00 - 1KByte, 01- 2KBtye, 10 - 4KByte, 11 - 8KByte) */
	uint8 rx_size		/**< rx_size Rx memory size (00 - 1KByte, 01- 2KBtye, 10 - 4KByte, 11 - 8KByte) */
	)
{
	int16 i;
	int16 ssum,rsum;

#ifdef __DEF_IINCHIP_DBG__
	printf("sysinit()\r\n"); 
#endif

	ssum = 0;
	rsum = 0;
	
	IINCHIP_WRITE(TMSR,tx_size); /* Set Tx memory size for each channel */
	IINCHIP_WRITE(RMSR,rx_size);	 /* Set Rx memory size for each channel */

	SBUFBASEADDRESS[0] = (uint16)(__DEF_IINCHIP_MAP_TXBUF__);		/* Set base address of Tx memory for channel #0 */
	RBUFBASEADDRESS[0] = (uint16)(__DEF_IINCHIP_MAP_RXBUF__);		/* Set base address of Rx memory for channel #0 */

#ifdef __DEF_IINCHIP_DBG__
	printf("Channel : SEND MEM SIZE : RECV MEM SIZE\r\n");
#endif

   for (i = 0 ; i < MAX_SOCK_NUM; i++)       // Set the size, masking and base address of Tx & Rx memory by each channel
	{
		SSIZE[i] = (int16)(0);
		RSIZE[i] = (int16)(0);
		if (ssum < 8192)
		{
         switch((tx_size >> i*2) & 0x03)  // Set Tx memory size
			{
			case 0:
				SSIZE[i] = (int16)(1024);
				SMASK[i] = (uint16)(0x03FF);
				break;
			case 1:
				SSIZE[i] = (int16)(2048);
				SMASK[i] = (uint16)(0x07FF);
				break;
			case 2:
				SSIZE[i] = (int16)(4096);
				SMASK[i] = (uint16)(0x0FFF);
				break;
			case 3:
				SSIZE[i] = (int16)(8192);
				SMASK[i] = (uint16)(0x1FFF);
				break;
			}
		}
		if (rsum < 8192)
		{
         switch((rx_size >> i*2) & 0x03)     // Set Rx memory size
			{
			case 0:
				RSIZE[i] = (int16)(1024);
				RMASK[i] = (uint16)(0x03FF);
				break;
			case 1:
				RSIZE[i] = (int16)(2048);
				RMASK[i] = (uint16)(0x07FF);
				break;
			case 2:
				RSIZE[i] = (int16)(4096);
				RMASK[i] = (uint16)(0x0FFF);
				break;
			case 3:
				RSIZE[i] = (int16)(8192);
				RMASK[i] = (uint16)(0x1FFF);
				break;
			}
		}
		ssum += SSIZE[i];
		rsum += RSIZE[i];

      if (i != 0)             // Sets base address of Tx and Rx memory for channel #1,#2,#3
		{
			SBUFBASEADDRESS[i] = SBUFBASEADDRESS[i-1] + SSIZE[i-1];
			RBUFBASEADDRESS[i] = RBUFBASEADDRESS[i-1] + RSIZE[i-1];
		}
#ifdef __DEF_IINCHIP_DBG__
		printf("%d : %.4x : %.4x : %.4x : %.4x\r\n", i, (uint16)SBUFBASEADDRESS[i], (uint16)RBUFBASEADDRESS[i], SSIZE[i], RSIZE[i]);
#endif
	}
}


void setMR(uint8 val)
{

#if (__DEF_IINCHIP_BUS__ == __DEF_IINCHIP_INDIRECT_MODE__)
	*((volatile uint8*)(MR)) = val;
#else
	/* 	DIRECT ACCESS	*/
	IINCHIP_WRITE(MR,val);
#endif	
}


/**
@brief	This function sets up gateway IP address.
*/ 
void setGAR(
	uint8 * addr	/**< a pointer to a 4 -byte array responsible to set the Gateway IP address. */
	)
{
	IINCHIP_WRITE((GAR0 + 0),addr[0]);
	IINCHIP_WRITE((GAR0 + 1),addr[1]);
	IINCHIP_WRITE((GAR0 + 2),addr[2]);
	IINCHIP_WRITE((GAR0 + 3),addr[3]);
}
void getGWIP(uint8 * addr)
{
	addr[0] = IINCHIP_READ((GAR0 + 0));
	addr[1] = IINCHIP_READ((GAR0 + 1));
	addr[2] = IINCHIP_READ((GAR0 + 2));
	addr[3] = IINCHIP_READ((GAR0 + 3));
}


/**
@brief	It sets up SubnetMask address
*/ 
void setSUBR(
	uint8 * addr	/**< a pointer to a 4 -byte array responsible to set the SubnetMask address */
	)
{
	IINCHIP_WRITE((SUBR0 + 0),addr[0]);
	IINCHIP_WRITE((SUBR0 + 1),addr[1]);
	IINCHIP_WRITE((SUBR0 + 2),addr[2]);
	IINCHIP_WRITE((SUBR0 + 3),addr[3]);
}


/**
@brief	This function sets up MAC address.
*/ 
void setSHAR(
	uint8 * addr	/**< a pointer to a 6 -byte array responsible to set the MAC address. */
	)
{
	IINCHIP_WRITE((SHAR0 + 0),addr[0]);
	IINCHIP_WRITE((SHAR0 + 1),addr[1]);
	IINCHIP_WRITE((SHAR0 + 2),addr[2]);
	IINCHIP_WRITE((SHAR0 + 3),addr[3]);
	IINCHIP_WRITE((SHAR0 + 4),addr[4]);
	IINCHIP_WRITE((SHAR0 + 5),addr[5]);
}


/**
@brief	This function sets up Source IP address.
*/
void setSIPR(
	uint8 * addr	/**< a pointer to a 4 -byte array responsible to set the Source IP address. */
	)
{
	IINCHIP_WRITE((SIPR0 + 0),addr[0]);
	IINCHIP_WRITE((SIPR0 + 1),addr[1]);
	IINCHIP_WRITE((SIPR0 + 2),addr[2]);
	IINCHIP_WRITE((SIPR0 + 3),addr[3]);
}


/**
@brief	This function gets Interrupt register in common register.
 */
uint8 getIR( void )
{
   return IINCHIP_READ(IR);
}



/**
@brief	This function sets up Retransmission time.

If there is no response from the peer or delay in response then retransmission 
will be there as per RTR (Retry Time-value Register)setting
*/
void setRTR(uint16 timeout)
{
	IINCHIP_WRITE(RTR0,(uint8)((timeout & 0xff00) >> 8));
	IINCHIP_WRITE((RTR0 + 1),(uint8)(timeout & 0x00ff));
}


/**
@brief	This function set the number of Retransmission.

If there is no response from the peer or delay in response then recorded time 
as per RTR & RCR register seeting then time out will occur.
*/
void setRCR(uint8 retry)
{
	IINCHIP_WRITE(RCR,retry);
}


/**
@brief	This function set the interrupt mask Enable/Disable appropriate Interrupt. ('1' : interrupt enable)

If any bit in IMR is set as '0' then there is not interrupt signal though the bit is
set in IR register.
*/
void setIMR(uint8 mask)
{
	IINCHIP_WRITE(IMR,mask); // must be setted 0x10.
}


/**
@brief	These below functions are used to get the Gateway, SubnetMask
		and Source Hardware Address (MAC Address) and Source IP address
*/
void getGAR(uint8 * addr)
{
	addr[0] = IINCHIP_READ(GAR0);
	addr[1] = IINCHIP_READ(GAR0+1);
	addr[2] = IINCHIP_READ(GAR0+2);
	addr[3] = IINCHIP_READ(GAR0+3);
}
void getSUBR(uint8 * addr)
{
	addr[0] = IINCHIP_READ(SUBR0);
	addr[1] = IINCHIP_READ(SUBR0+1);
	addr[2] = IINCHIP_READ(SUBR0+2);
	addr[3] = IINCHIP_READ(SUBR0+3);
}
void getSHAR(uint8 * addr)
{
	addr[0] = IINCHIP_READ(SHAR0);
	addr[1] = IINCHIP_READ(SHAR0+1);
	addr[2] = IINCHIP_READ(SHAR0+2);
	addr[3] = IINCHIP_READ(SHAR0+3);
	addr[4] = IINCHIP_READ(SHAR0+4);
	addr[5] = IINCHIP_READ(SHAR0+5);
}
void getSIPR(uint8 * addr)
{
	addr[0] = IINCHIP_READ(SIPR0);
	addr[1] = IINCHIP_READ(SIPR0+1);
	addr[2] = IINCHIP_READ(SIPR0+2);
	addr[3] = IINCHIP_READ(SIPR0+3);
}


/**
@brief	These below functions are used to get the Destination Hardware Address (MAC Address), Destination IP address and Destination Port.
*/
void getSn_DHAR(SOCKET s, uint8 * addr)
{
	addr[0] = IINCHIP_READ(Sn_DHAR0(s));
	addr[1] = IINCHIP_READ(Sn_DHAR0(s)+1);
	addr[2] = IINCHIP_READ(Sn_DHAR0(s)+2);
	addr[3] = IINCHIP_READ(Sn_DHAR0(s)+3);
	addr[4] = IINCHIP_READ(Sn_DHAR0(s)+4);
	addr[5] = IINCHIP_READ(Sn_DHAR0(s)+5);
}
void setSn_DHAR(SOCKET s, uint8 * addr)
{
	IINCHIP_WRITE((Sn_DHAR0(s) + 0),addr[0]);
	IINCHIP_WRITE((Sn_DHAR0(s) + 1),addr[1]);
	IINCHIP_WRITE((Sn_DHAR0(s) + 2),addr[2]);
	IINCHIP_WRITE((Sn_DHAR0(s) + 3),addr[3]);
	IINCHIP_WRITE((Sn_DHAR0(s) + 4),addr[4]);
	IINCHIP_WRITE((Sn_DHAR0(s) + 5),addr[5]);
}
void getSn_DIPR(SOCKET s, uint8 * addr)
{
	addr[0] = IINCHIP_READ(Sn_DIPR0(s));
	addr[1] = IINCHIP_READ(Sn_DIPR0(s)+1);
	addr[2] = IINCHIP_READ(Sn_DIPR0(s)+2);
	addr[3] = IINCHIP_READ(Sn_DIPR0(s)+3);
}
void setSn_DIPR(SOCKET s, uint8 * addr)
{
	IINCHIP_WRITE((Sn_DIPR0(s) + 0),addr[0]);
	IINCHIP_WRITE((Sn_DIPR0(s) + 1),addr[1]);
	IINCHIP_WRITE((Sn_DIPR0(s) + 2),addr[2]);
	IINCHIP_WRITE((Sn_DIPR0(s) + 3),addr[3]);
}
void getSn_DPORT(SOCKET s, uint8 * addr)
{
	addr[0] = IINCHIP_READ(Sn_DPORT0(s));
	addr[1] = IINCHIP_READ(Sn_DPORT0(s)+1);
}
void setSn_DPORT(SOCKET s, uint8 * addr)
{
	IINCHIP_WRITE((Sn_DPORT0(s) + 0),addr[0]);
	IINCHIP_WRITE((Sn_DPORT0(s) + 1),addr[1]);
}


/**
@brief	This sets the maximum segment size of TCP in Active Mode), while in Passive Mode this is set by peer
*/
void setSn_MSS(SOCKET s, uint16 Sn_MSSR0)
{
	IINCHIP_WRITE(Sn_MSSR0(s),(uint8)((Sn_MSSR0 & 0xff00) >> 8));
	IINCHIP_WRITE((Sn_MSSR0(s) + 1),(uint8)(Sn_MSSR0 & 0x00ff));
}

void setSn_TTL(SOCKET s, uint8 ttl)
{
   IINCHIP_WRITE(Sn_TTL(s), ttl);
}


/**
@brief	These below function is used to setup the Protocol Field of IP Header when
		executing the IP Layer RAW mode.
*/
void setSn_PROTO(SOCKET s, uint8 proto)
{
	IINCHIP_WRITE(Sn_PROTO(s),proto);
}


/**
@brief	get socket interrupt status

These below functions are used to read the Interrupt & Soket Status register
*/
uint8 getSn_IR(SOCKET s)
{
   return IINCHIP_READ(Sn_IR(s));
}


/**
@brief	 get socket status
*/
uint8 getSn_SR(SOCKET s)
{
   return IINCHIP_READ(Sn_SR(s));
}


/**
@brief	get socket TX free buf size

This gives free buffer size of transmit buffer. This is the data size that user can transmit.
User shuold check this value first and control the size of transmitting data
*/
uint16 getSn_TX_FSR(SOCKET s)
{
	uint16 val=0,val1=0;
	do
	{
		val1 = IINCHIP_READ(Sn_TX_FSR0(s));
		val1 = (val1 << 8) + IINCHIP_READ(Sn_TX_FSR0(s) + 1);
      if (val1 != 0)
		{
   			val = IINCHIP_READ(Sn_TX_FSR0(s));
   			val = (val << 8) + IINCHIP_READ(Sn_TX_FSR0(s) + 1);
		}
	} while (val != val1);
   return val;
}


/**
@brief	 get socket RX recv buf size

This gives size of received data in receive buffer. 
*/
uint16 getSn_RX_RSR(SOCKET s)
{
	uint16 val=0,val1=0;
	do
	{
		val1 = IINCHIP_READ(Sn_RX_RSR0(s));
		val1 = (val1 << 8) + IINCHIP_READ(Sn_RX_RSR0(s) + 1);
      if(val1 != 0)
		{
   			val = IINCHIP_READ(Sn_RX_RSR0(s));
   			val = (val << 8) + IINCHIP_READ(Sn_RX_RSR0(s) + 1);
		}
	} while (val != val1);
   return val;
}


/**
@brief	 This function is being called by send() and sendto() function also. 

This function read the Tx write pointer register and after copy the data in buffer update the Tx write pointer
register. User should read upper byte first and lower byte later to get proper value.
*/
void send_data_processing(SOCKET s, uint8 *data, uint16 len)
{
	uint16 ptr;
	ptr = IINCHIP_READ(Sn_TX_WR0(s));
	ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_TX_WR0(s) + 1);
	write_data(s, data, (uint8 *)(ptr), len);
	ptr += len;
	IINCHIP_WRITE(Sn_TX_WR0(s),(uint8)((ptr & 0xff00) >> 8));
	IINCHIP_WRITE((Sn_TX_WR0(s) + 1),(uint8)(ptr & 0x00ff));
}


/**
@brief	This function is being called by recv() also.

This function read the Rx read pointer register
and after copy the data from receive buffer update the Rx write pointer register.
User should read upper byte first and lower byte later to get proper value.
*/
void recv_data_processing(SOCKET s, uint8 *data, uint16 len)
{
	uint16 ptr;
	ptr = IINCHIP_READ(Sn_RX_RD0(s));
	ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_RX_RD0(s) + 1);
#ifdef __DEF_IINCHIP_DBG__
	printf("ISR_RX: rd_ptr : %.4x\r\n", ptr);
#endif
	read_data(s, (uint8 *)ptr, data, len); // read data
	ptr += len;
	IINCHIP_WRITE(Sn_RX_RD0(s),(uint8)((ptr & 0xff00) >> 8));
	IINCHIP_WRITE((Sn_RX_RD0(s) + 1),(uint8)(ptr & 0x00ff));
}


/**
@brief	for copy the data form application buffer to Transmite buffer of the chip.

This function is being used for copy the data form application buffer to Transmite
buffer of the chip. It calculate the actual physical address where one has to write
the data in transmite buffer. Here also take care of the condition while it exceed
the Tx memory uper-bound of socket.
*/
void write_data(SOCKET s, vuint8 * src, vuint8 * dst, uint16 len)
{
	uint16 size;
	uint16 dst_mask;
	uint8 * dst_ptr;

	dst_mask = (uint16)dst & getIINCHIP_TxMASK(s);
	dst_ptr = (uint8 *)(getIINCHIP_TxBASE(s) + dst_mask);
	
	if (dst_mask + len > getIINCHIP_TxMAX(s)) 
	{
		size = getIINCHIP_TxMAX(s) - dst_mask;
		wiz_write_buf((uint16)dst_ptr, (uint8*)src, size);
		src += size;
		size = len - size;
		dst_ptr = (uint8 *)(getIINCHIP_TxBASE(s));
		wiz_write_buf((uint16)dst_ptr, (uint8*)src, size);
	} 
	else
	{
		wiz_write_buf((uint16)dst_ptr, (uint8*)src, len);
	}
}


/**
@brief	This function is being used for copy the data form Receive buffer of the chip to application buffer.

It calculate the actual physical address where one has to read
the data from Receive buffer. Here also take care of the condition while it exceed
the Rx memory uper-bound of socket.
*/
void read_data(SOCKET s, vuint8 * src, vuint8 * dst, uint16 len)
{
	uint16 size;
	uint16 src_mask;
	uint8 * src_ptr;

	src_mask = (uint16)src & getIINCHIP_RxMASK(s);
	src_ptr = (uint8 *)(getIINCHIP_RxBASE(s) + src_mask);
	
	if( (src_mask + len) > getIINCHIP_RxMAX(s) ) 
	{
		size = getIINCHIP_RxMAX(s) - src_mask;
		wiz_read_buf((uint16)src_ptr, (uint8*)dst,size);
		dst += size;
		size = len - size;
		src_ptr = (uint8 *)(getIINCHIP_RxBASE(s));
		wiz_read_buf((uint16)src_ptr, (uint8*) dst,size);
	} 
	else
	{
		wiz_read_buf((uint16)src_ptr, (uint8*) dst,len);
	}
}


#ifdef __DEF_IINCHIP_PPP__
#define PPP_OPTION_BUF_LEN 64

uint8 pppinit_in(uint8 * id, uint8 idlen, uint8 * passwd, uint8 passwdlen);


/**
@brief	make PPPoE connection
@return	1 => success to connect, 2 => Auth fail, 3 => timeout, 4 => Auth type not support

*/
uint8 pppinit(uint8 * id, uint8 idlen, uint8 * passwd, uint8 passwdlen)
{
	uint8 ret;
	uint8 isr;
	
	// PHASE0. W5100 PPPoE(ADSL) setup
	// enable pppoe mode
	printf("-- PHASE 0. W5100 PPPoE(ADSL) setup process --\r\n");
	printf("\r\n");
	IINCHIP_WRITE(MR,IINCHIP_READ(MR) | MR_PPPOE);

	// open socket in pppoe mode
	isr = IINCHIP_READ(Sn_IR(0));// first clear isr(0), W5100 at present time
	IINCHIP_WRITE(Sn_IR(0),isr);
	
	IINCHIP_WRITE(PTIMER,200); // 5sec timeout
	IINCHIP_WRITE(PMAGIC,0x01); // magic number
	IINCHIP_WRITE(Sn_MR(0),Sn_MR_PPPOE);
	IINCHIP_WRITE(Sn_CR(0),Sn_CR_OPEN);
	
	/* +20071122[chungs]:wait to process the command... */
	while( IINCHIP_READ(Sn_CR(0)) ) 
		;
	/* ------- */
	
	ret = pppinit_in(id, idlen, passwd, passwdlen);

	// close ppp connection socket
	/* +200801 (hwkim) */
	close(0);
	/* ------- */
	
	return ret;
}


uint8 pppinit_in(uint8 * id, uint8 idlen, uint8 * passwd, uint8 passwdlen)
{
	uint8 loop_idx = 0;
	uint8 isr = 0;
	uint8 buf[PPP_OPTION_BUF_LEN];
	uint16 len;
	uint8 str[PPP_OPTION_BUF_LEN];
	uint8 str_idx,dst_idx;

   // PHASE1. PPPoE Discovery
	// start to connect pppoe connection
	printf("-- PHASE 1. PPPoE Discovery process --");
	printf(" ok\r\n");
	printf("\r\n");
	IINCHIP_WRITE(Sn_CR(0),Sn_CR_PCON);
	/* +20071122[chungs]:wait to process the command... */
	while( IINCHIP_READ(Sn_CR(0)) ) 
		;
	/* ------- */
	
	wait_10ms(100);

	loop_idx = 0;
	//check whether PPPoE discovery end or not
	while (!(IINCHIP_READ(Sn_IR(0)) & Sn_IR_PNEXT))
	{
		printf(".");
		if (loop_idx++ == 10) // timeout
		{
			printf("timeout before LCP\r\n"); 
			return 3;
		}
		wait_10ms(100);
	}

   /* +200801[bj] clear interrupt value*/
   IINCHIP_WRITE(Sn_IR(0), 0xff);
   /*---*/

   // PHASE2. LCP process
	printf("-- PHASE 2. LCP process --");
		
	// send LCP Request
	{
		// Magic number option
		// option format (type value + length value + data)
	   // write magic number value
		buf[0] = 0x05; // type value
		buf[1] = 0x06; // length value
		buf[2] = 0x01; buf[3] = 0x01; buf[4] = 0x01; buf[5]= 0x01; // data
		// for MRU option, 1492 0x05d4  
		// buf[6] = 0x01; buf[7] = 0x04; buf[8] = 0x05; buf[9] = 0xD4;
	}
	send_data_processing(0, buf, 0x06);
	IINCHIP_WRITE(Sn_CR(0),Sn_CR_PCR); // send request 
	/* +20071122[chungs]:wait to process the command... */
	while( IINCHIP_READ(Sn_CR(0)) ) 
		;
	/* ------- */
		
	wait_10ms(100);

	while (!((isr = IINCHIP_READ(Sn_IR(0))) & Sn_IR_PNEXT))
	{
		if (isr & Sn_IR_PRECV) // Not support option
		{
   /* +200801[bj] clear interrupt value*/
         IINCHIP_WRITE(Sn_IR(0), Sn_IR_PRECV);
   /*---*/
			len = getSn_RX_RSR(0);
			if ( len > 0 )
			{
				recv_data_processing(0, str, len);
				IINCHIP_WRITE(Sn_CR(0),Sn_CR_RECV);
				/* +20071122[chungs]:wait to process the command... */
				while( IINCHIP_READ(Sn_CR(0)) ) 
					;
				/* ------- */
				
				// for debug
				//printf("LCP proc\r\n"); for (i = 0; i < len; i++) printf ("%02x ", str[i]); printf("\r\n");
				// get option length
				len = str[4]; len = ((len & 0x00ff) << 8) + str[5];
				len += 2;
				str_idx = 6; dst_idx = 0; // ppp header is 6 byte, so starts at 6.
				do 
				{
					if ((str[str_idx] == 0x01) || (str[str_idx] == 0x02) || (str[str_idx] == 0x03) || (str[str_idx] == 0x05))
					{
						// skip as length of support option. str_idx+1 is option's length.
						str_idx += str[str_idx+1];
					}
					else
					{
						// not support option , REJECT
						memcpy((uint8 *)(buf+dst_idx), (uint8 *)(str+str_idx), str[str_idx+1]);
						dst_idx += str[str_idx+1]; str_idx += str[str_idx+1];
					}
				} while (str_idx != len);
	   			// for debug
	   			// printf("LCP dst proc\r\n"); for (i = 0; i < dst_idx; i++) printf ("%02x ", dst[i]); printf("\r\n");
	   
	   			// send LCP REJECT packet
	   			send_data_processing(0, buf, dst_idx);
	   			IINCHIP_WRITE(Sn_CR(0),Sn_CR_PCJ);
				/* +20071122[chungs]:wait to process the command... */
				while( IINCHIP_READ(Sn_CR(0)) ) 
					;
				/* ------- */
  			}
		}
		printf(".");
		if (loop_idx++ == 10) // timeout
		{
			printf("timeout after LCP\r\n");
			return 3;
		}
		wait_10ms(100);
	}
	printf(" ok\r\n");
	printf("\r\n");

   /* +200801[bj] clear interrupt value*/
   IINCHIP_WRITE(Sn_IR(0), 0xff);
   /*---*/

	printf("-- PHASE 3. PPPoE(ADSL) Authentication mode --\r\n");
	printf("Authentication protocol : %.2x %.2x, ", IINCHIP_READ(PATR0), IINCHIP_READ(PATR0+1));

	loop_idx = 0;
	if (IINCHIP_READ(PATR0) == 0xc0 && IINCHIP_READ(PATR0+1) == 0x23)
	{
		printf("PAP\r\n"); // in case of adsl normally supports PAP.
		// send authentication data
		// copy (idlen + id + passwdlen + passwd)
		buf[loop_idx] = idlen; loop_idx++;
		memcpy((uint8 *)(buf+loop_idx), (uint8 *)(id), idlen); loop_idx += idlen;
		buf[loop_idx] = passwdlen; loop_idx++;
		memcpy((uint8 *)(buf+loop_idx), (uint8 *)(passwd), passwdlen); loop_idx += passwdlen;
		send_data_processing(0, buf, loop_idx);
		IINCHIP_WRITE(Sn_CR(0),Sn_CR_PCR);
		/* +20071122[chungs]:wait to process the command... */
		while( IINCHIP_READ(Sn_CR(0)) ) 
			;
		/* ------- */
		wait_10ms(100);
	}	
	else if (IINCHIP_READ(PATR0) == 0xc2 && IINCHIP_READ(PATR0+1) == 0x23)
	{
		uint8 chal_len;
		md5_ctx context;
		uint8  digest[16];

		len = getSn_RX_RSR(0);
		if ( len > 0 )
		{
			recv_data_processing(0, str, len);
			IINCHIP_WRITE(Sn_CR(0),Sn_CR_RECV);
			/* +20071122[chungs]:wait to process the command... */
			while( IINCHIP_READ(Sn_CR(0)) ) 
				;
			/* ------- */
#ifdef __DEF_IINCHIP_DBG__
			printf("recv CHAP\r\n");
			{
				int16 i;
				
				for (i = 0; i < 32; i++) 
					printf ("%02x ", str[i]);
			}
			printf("\r\n");
#endif
// str is C2 23 xx CHAL_ID xx xx CHAP_LEN CHAP_DATA
// index  0  1  2  3       4  5  6        7 ...

			memset(buf,0x00,64);
			buf[loop_idx] = str[3]; loop_idx++; // chal_id
			memcpy((uint8 *)(buf+loop_idx), (uint8 *)(passwd), passwdlen); loop_idx += passwdlen; //passwd
			chal_len = str[6]; // chal_id
			memcpy((uint8 *)(buf+loop_idx), (uint8 *)(str+7), chal_len); loop_idx += chal_len; //challenge
			buf[loop_idx] = 0x80;
#ifdef __DEF_IINCHIP_DBG__
			printf("CHAP proc d1\r\n");
			{
				int16 i;
				for (i = 0; i < 64; i++) 
					printf ("%02x ", buf[i]);
			}
			printf("\r\n");
#endif

			md5_init(&context);
			md5_update(&context, buf, loop_idx);
			md5_final(digest, &context);

#ifdef __DEF_IINCHIP_DBG__
			printf("CHAP proc d1\r\n");
			{
				int16 i;				
				for (i = 0; i < 16; i++) 
					printf ("%02x", digest[i]);
			}
			printf("\r\n");
#endif
			loop_idx = 0;
			buf[loop_idx] = 16; loop_idx++; // hash_len
			memcpy((uint8 *)(buf+loop_idx), (uint8 *)(digest), 16); loop_idx += 16; // hashed value
			memcpy((uint8 *)(buf+loop_idx), (uint8 *)(id), idlen); loop_idx += idlen; // id
			send_data_processing(0, buf, loop_idx);
			IINCHIP_WRITE(Sn_CR(0),Sn_CR_PCR);
			/* +20071122[chungs]:wait to process the command... */
			while( IINCHIP_READ(Sn_CR(0)) ) 
				;
			/* ------- */
			wait_10ms(100);
		}
	}
	else
	{
		printf("Not support\r\n");
#ifdef __DEF_IINCHIP_DBG__
		printf("Not support PPP Auth type: %.2x%.2x\r\n",IINCHIP_READ(PATR0), IINCHIP_READ(PATR0+1));
#endif
		return 4;
	}
	printf("\r\n");

	printf("-- Waiting for PPPoE server's admission --");
	loop_idx = 0;
	while (!((isr = IINCHIP_READ(Sn_IR(0))) & Sn_IR_PNEXT))
	{
		if (isr & Sn_IR_PFAIL)
		{
   /* +200801[bj] clear interrupt value*/
   IINCHIP_WRITE(Sn_IR(0), 0xff);
   /*---*/
			printf("failed\r\nReinput id, password..\r\n");
			return 2;
		}
		printf(".");
		if (loop_idx++ == 10) // timeout
		{
   /* +200801[bj] clear interrupt value*/
   IINCHIP_WRITE(Sn_IR(0), 0xff);
   /*---*/
			printf("timeout after PAP\r\n");
			return 3;
		}
		wait_10ms(100);
	}
   /* +200801[bj] clear interrupt value*/
   IINCHIP_WRITE(Sn_IR(0), 0xff);
   /*---*/
	printf("ok\r\n");
	printf("\r\n");
	printf("-- PHASE 4. IPCP process --");
	// IP Address
	buf[0] = 0x03; buf[1] = 0x06; buf[2] = 0x00; buf[3] = 0x00; buf[4] = 0x00; buf[5] = 0x00;
	send_data_processing(0, buf, 6);
	IINCHIP_WRITE(Sn_CR(0),Sn_CR_PCR);
	/* +20071122[chungs]:wait to process the command... */
	while( IINCHIP_READ(Sn_CR(0)) ) 
		;
	/* ------- */
	wait_10ms(100);

	loop_idx = 0;
	while (1)
	{
		if (IINCHIP_READ(Sn_IR(0)) & Sn_IR_PRECV)
		{
   /* +200801[bj] clear interrupt value*/
   IINCHIP_WRITE(Sn_IR(0), 0xff);
   /*---*/
			len = getSn_RX_RSR(0);
			if ( len > 0 )
			{
				recv_data_processing(0, str, len);
				IINCHIP_WRITE(Sn_CR(0),Sn_CR_RECV);
				/* +20071122[chungs]:wait to process the command... */
				while( IINCHIP_READ(Sn_CR(0)) ) 
					;
				/* ------- */
	   			//for debug
	   			//printf("IPCP proc\r\n"); for (i = 0; i < len; i++) printf ("%02x ", str[i]); printf("\r\n");
	   			str_idx = 6; dst_idx = 0;
	   			if (str[2] == 0x03) // in case of NAK
	   			{
	   				do 
	   				{
	   					if (str[str_idx] == 0x03) // request only ip information
	   					{
	   						memcpy((uint8 *)(buf+dst_idx), (uint8 *)(str+str_idx), str[str_idx+1]);
	   						dst_idx += str[str_idx+1]; str_idx += str[str_idx+1];
	   					}
	   					else
	   					{
	   						// skip byte
	   						str_idx += str[str_idx+1];
	   					}
	   					// for debug
	   					//printf("s: %d, d: %d, l: %d", str_idx, dst_idx, len);
	   				} while (str_idx != len);
	   				send_data_processing(0, buf, dst_idx);
	   				IINCHIP_WRITE(Sn_CR(0),Sn_CR_PCR); // send ipcp request
	   				/* +20071122[chungs]:wait to process the command... */
					while( IINCHIP_READ(Sn_CR(0)) ) 
						;
					/* ------- */
	   				wait_10ms(100);
	   				break;
	   			}
			}
		}
		printf(".");
		if (loop_idx++ == 10) // timeout
		{
			printf("timeout after IPCP\r\n");
			return 3;
		}
		wait_10ms(100);
		send_data_processing(0, buf, 6);
		IINCHIP_WRITE(Sn_CR(0),Sn_CR_PCR); //ipcp re-request
		/* +20071122[chungs]:wait to process the command... */
		while( IINCHIP_READ(Sn_CR(0)) ) 
			;
		/* ------- */
	}

	loop_idx = 0;
	while (!(IINCHIP_READ(Sn_IR(0)) & Sn_IR_PNEXT))
	{
		printf(".");
		if (loop_idx++ == 10) // timeout
		{
			printf("timeout after IPCP NAK\r\n");
			return 3;
		}
		wait_10ms(100);
		IINCHIP_WRITE(Sn_CR(0),Sn_CR_PCR); // send ipcp request
		/* +20071122[chungs]:wait to process the command... */
		while( IINCHIP_READ(Sn_CR(0)) ) 
			;
		/* ------- */
	}
   /* +200801[bj] clear interrupt value*/
   IINCHIP_WRITE(Sn_IR(0), 0xff);
   /*---*/
	printf("ok\r\n");
	printf("\r\n");
	return 1;
	// after this function, User must save the pppoe server's mac address and pppoe session id in current connection
}


/**
@brief	terminate PPPoE connection
*/
uint8 pppterm(uint8 * mac, uint8 * sessionid)
{
	uint16 i;
	uint8 isr;
#ifdef __DEF_IINCHIP_DBG__
	printf("pppterm()\r\n");
#endif
	/* Set PPPoE bit in MR(Common Mode Register) : enable socket0 pppoe */
	IINCHIP_WRITE(MR,IINCHIP_READ(MR) | MR_PPPOE);
	
	// write pppoe server's mac address and session id 
	// must be setted these value.
	for (i = 0; i < 6; i++) IINCHIP_WRITE((Sn_DHAR0(0)+i),mac[i]);
	for (i = 0; i < 2; i++) IINCHIP_WRITE((Sn_DPORT0(0)+i),sessionid[i]);
	isr = IINCHIP_READ(Sn_IR(0));
	IINCHIP_WRITE(Sn_IR(0),isr);
	
	//open socket in pppoe mode
	IINCHIP_WRITE(Sn_MR(0),Sn_MR_PPPOE);
	IINCHIP_WRITE(Sn_CR(0),Sn_CR_OPEN);
	/* +20071122[chungs]:wait to process the command... */
	while( IINCHIP_READ(Sn_CR(0)) ) 
		;
	/* ------- */
	wait_1us(1);
	// close pppoe connection
	IINCHIP_WRITE(Sn_CR(0),Sn_CR_PDISCON);
	/* +20071122[chungs]:wait to process the command... */
	while( IINCHIP_READ(Sn_CR(0)) ) 
		;
	/* ------- */
	wait_10ms(100);
	// close socket
	/* +200801 (hwkim) */
	close(0);
	/* ------- */
	

#ifdef __DEF_IINCHIP_DBG__
	printf("pppterm() end ..\r\n");
#endif

	return 1;
}
#endif
