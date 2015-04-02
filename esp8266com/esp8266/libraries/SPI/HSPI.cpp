#include "include/HSPI.h"
#include "include/spi_register.h"

#define __min(a,b) ((a > b) ? (b):(a))

void HSPI::begin()
{
	spi_fifo = (uint32_t*)SPI_FLASH_C0(hspi_port);
	//bit9 of PERIPHS_IO_MUX should be cleared when HSPI clock doesn't equal CPU clock
	WRITE_PERI_REG(PERIPHS_IO_MUX, 0x105);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_HSPIQ_MISO);	// gpio12
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_HSPID_MOSI);	// gpio13
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_HSPI_CLK);		// gpio14

	// SPI clock=CPU clock/8
	WRITE_PERI_REG(SPI_FLASH_CLOCK(hspi_port),
		((1&SPI_CLKDIV_PRE)<<SPI_CLKDIV_PRE_S)|
		((3&SPI_CLKCNT_N)<<SPI_CLKCNT_N_S)|
		((1&SPI_CLKCNT_H)<<SPI_CLKCNT_H_S)|
		((3&SPI_CLKCNT_L)<<SPI_CLKCNT_L_S)); //clear bit 31,set SPI clock div

	uint32_t regvalue = SPI_FLASH_DOUT;
	regvalue |= SPI_DOUTDIN | SPI_CK_I_EDGE;
	regvalue &= ~(BIT2 | SPI_FLASH_USR_ADDR | SPI_FLASH_USR_DUMMY | SPI_FLASH_USR_DIN | SPI_USR_COMMAND);

	WRITE_PERI_REG(SPI_FLASH_USER(hspi_port), regvalue);
	WRITE_PERI_REG(SPI_FLASH_CTRL1(hspi_port), 0);
}

void HSPI::end()
{
}

void HSPI::setDataMode(uint8_t dataMode)
{
}

void HSPI::setBitOrder(uint8_t bitOrder)
{
	if (!bitOrder)
	{
		WRITE_PERI_REG(SPI_FLASH_CTRL(hspi_port),
				READ_PERI_REG(SPI_FLASH_CTRL(hspi_port)) & (~(SPI_WR_BIT_ODER | SPI_RD_BIT_ODER)));
	}
	else
	{
		WRITE_PERI_REG(SPI_FLASH_CTRL(hspi_port),
				READ_PERI_REG(SPI_FLASH_CTRL(hspi_port)) | (SPI_WR_BIT_ODER | SPI_RD_BIT_ODER));
	}
}

void HSPI::setClockDivider(uint8_t clockDiv)
{
	uint32_t clock_reg_val = (((clockDiv - 1) & SPI_CLKDIV_PRE) << SPI_CLKDIV_PRE_S) |
				   ((1 & SPI_CLKCNT_N) << SPI_CLKCNT_N_S) |
				   ((0 & SPI_CLKCNT_H) << SPI_CLKCNT_H_S) |
				   ((1 & SPI_CLKCNT_L) << SPI_CLKCNT_L_S);
	WRITE_PERI_REG(SPI_FLASH_CLOCK(hspi_port), clock_reg_val);
}

uint8_t HSPI::transfer(uint8_t data)
{
	hspi_wait_ready();
	hspi_prepare_txrx(1);
	*spi_fifo = data;
	hspi_start_tx();
	hspi_wait_ready();
	return *spi_fifo & 0xFF;
}

uint16_t HSPI::transfer16(uint16_t data)
{
	hspi_wait_ready();
	hspi_prepare_txrx(2);
	*spi_fifo = data;
	hspi_start_tx();
	hspi_wait_ready();
	return *spi_fifo & 0xFFFF;
}

void HSPI::transfer(void *buf, size_t count)
{
	uint32_t *_data = (uint32_t*)buf;
	uint8_t i;

	uint8_t words_to_send = __min((count + 3) / 4, hspi_fifo_size);
	hspi_prepare_tx(count);
	for(i = 0; i < words_to_send;i++)
		spi_fifo[i] = _data[i];
	hspi_start_tx();
}



