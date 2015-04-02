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
	virtual void begin();
	virtual void end();
	virtual void setBitOrder(uint8_t bitOrder);
	virtual void setDataMode(uint8_t dataMode);
	virtual void setClockDivider(uint8_t clockDiv);
	virtual uint8_t transfer(uint8_t data);
	virtual uint16_t transfer16(uint16_t data);
	virtual void transfer(void *buf, size_t count);

private:
	uint32_t _clkDiv;
	uint32_t *spi_fifo;
	const uint32_t hspi_port = 1;
	const uint32_t hspi_fifo_size = 16;

private:
	inline void hspi_wait_ready(void){while (READ_PERI_REG(SPI_FLASH_CMD(hspi_port))&SPI_FLASH_USR);}
	inline void hspi_start_tx(){SET_PERI_REG_MASK(SPI_FLASH_CMD(hspi_port), SPI_FLASH_USR);}
	inline void hspi_prepare_tx(uint32_t bytecount)
	{
		uint32_t bitcount = bytecount * 8 - 1;
		WRITE_PERI_REG(SPI_FLASH_USER1(hspi_port), (bitcount & SPI_USR_OUT_BITLEN) << SPI_USR_OUT_BITLEN_S);
	}
	inline void hspi_prepare_txrx(uint32_t bytecount)
	{
		uint32_t bitcount = bytecount * 8 - 1;
		WRITE_PERI_REG(SPI_FLASH_USER1(hspi_port), ((bitcount & SPI_USR_OUT_BITLEN) << SPI_USR_OUT_BITLEN_S) |
											   		((bitcount & SPI_USR_DIN_BITLEN) << SPI_USR_DIN_BITLEN_S));
	}
	
};



#endif//HSPI_H
