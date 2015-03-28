#ifndef HSPI_H
#define HSPI_H

#include "SPIImpl.h"
#include "SPIdef.h"

extern "C" {
	#include "spi_register.h"
	#include "ets_sys.h"
	#include "osapi.h"
	#include "os_type.h"
}

class HSPI : public SPIImpl
{
public:
	virtual void begin()
	{
		//bit9 of PERIPHS_IO_MUX should be cleared when HSPI clock doesn't equal CPU clock
		WRITE_PERI_REG(PERIPHS_IO_MUX, 0x105); 
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_HSPIQ_MISO);	// gpio12
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_HSPID_MOSI);	// gpio13
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_HSPI_CLK);		// gpio14
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, FUNC_HSPI_CS0);		// gpio15


		SET_PERI_REG_MASK(SPI_USER(HSPI), SPI_CS_SETUP|SPI_CS_HOLD|SPI_USR_COMMAND);
		CLEAR_PERI_REG_MASK(SPI_USER(HSPI), SPI_FLASH_MODE);

		// SPI clock=CPU clock/8
		WRITE_PERI_REG(SPI_CLOCK(HSPI), 
			((1&SPI_CLKDIV_PRE)<<SPI_CLKDIV_PRE_S)|
			((3&SPI_CLKCNT_N)<<SPI_CLKCNT_N_S)|
			((1&SPI_CLKCNT_H)<<SPI_CLKCNT_H_S)|
			((3&SPI_CLKCNT_L)<<SPI_CLKCNT_L_S)); //clear bit 31,set SPI clock div


		SET_PERI_REG_MASK(SPI_USER(spi_no), SPI_CS_SETUP|SPI_CS_HOLD|SPI_USR_COMMAND|SPI_USR_MOSI);
		CLEAR_PERI_REG_MASK(SPI_USER(spi_no), SPI_FLASH_MODE);

		//clear Daul or Quad lines transmission mode
		CLEAR_PERI_REG_MASK(SPI_CTRL(spi_no), SPI_QIO_MODE|SPI_DIO_MODE|SPI_DOUT_MODE|SPI_QOUT_MODE);

		WRITE_PERI_REG(SPI_CLOCK(spi_no), 
						((3&SPI_CLKCNT_N)<<SPI_CLKCNT_N_S)|
						((1&SPI_CLKCNT_H)<<SPI_CLKCNT_H_S)|
						((3&SPI_CLKCNT_L)<<SPI_CLKCNT_L_S)); //clear bit 31,set SPI clock div

		//set 8bit output buffer length, the buffer is the low 8bit of register"SPI_FLASH_C0"
		WRITE_PERI_REG(SPI_USER1(spi_no), 
						((7&SPI_USR_MOSI_BITLEN)<<SPI_USR_MOSI_BITLEN_S)|
						((7&SPI_USR_MISO_BITLEN)<<SPI_USR_MISO_BITLEN_S));

	}

	virtual void end()
	{

	}

	virtual void setBitOrder(uint8_t bitOrder)
	{

	}

	virtual void setDataMode(uint8_t dataMode)
	{

	}

	virtual void setClockDivider(uint8_t clockDiv)
	{

	}

	virtual uint8_t transfer(uint8_t data)
	{

	}

	virtual void transfer(void *buf, size_t count)
	{

	}

private:
	uint32_t _clkDiv;

	
};



#endif//HSPI_H
