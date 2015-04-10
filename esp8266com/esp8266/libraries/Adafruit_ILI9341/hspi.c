#include "driver\hspi.h"

/*
Pinout:
MISO GPIO12
MOSI GPIO13
CLK GPIO14
CS GPIO15
DC GPIO2
*/

#define __min(a,b) ((a > b) ? (b):(a))
uint32_t *spi_fifo;
uint32_t current_spi_port;

spi_config spi_init(uint32_t spi_port, uint32_t prescaler, spi_mode mode)
{
	spi_config config;

	WRITE_PERI_REG(PERIPHS_IO_MUX, 0x105); //clear bit9

	if (spi_port == SPI)
	{
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA0_U, 2); // SD_D0 MISO
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_DATA1_U, 2); // SD_D1 MOSI
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_SD_CLK_U, 2); // CLK
	} else if (spi_port == HSPI)
	{
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, 2); // HSPIQ MISO GPIO12
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, 2); // HSPID MOSI GPIO13
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, 2); // CLK GPIO14
		PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U, 2); // CS GPIO15
	}


	config.mode = mode;
	config.spi_port = spi_port;
	// SPI clock = CPU clock / 10 / 4
	// time length HIGHT level = (CPU clock / 10 / 2) ^ -1,
	// time length LOW level = (CPU clock / 10 / 2) ^ -1
	config.clock_reg_val = (((prescaler - 1) & SPI_CLKDIV_PRE) << SPI_CLKDIV_PRE_S) |
			   ((1 & SPI_CLKCNT_N) << SPI_CLKCNT_N_S) |
			   ((0 & SPI_CLKCNT_H) << SPI_CLKCNT_H_S) |
			   ((1 & SPI_CLKCNT_L) << SPI_CLKCNT_L_S);

	spi_reinit(&config);
	return config;
}

void spi_reinit(spi_config *config)
{
	current_spi_port = config->spi_port;
	spi_fifo = (uint32_t*)SPI_FLASH_C0(current_spi_port);

	uint32_t regvalue = SPI_FLASH_DOUT;
	WRITE_PERI_REG(SPI_FLASH_CLOCK(current_spi_port), config->clock_reg_val);
	WRITE_PERI_REG(SPI_FLASH_CTRL1(current_spi_port), 0);

	switch(config->mode)
	{
		case spi_mode_tx:
		    regvalue &= ~(BIT2 | SPI_FLASH_USR_ADDR | SPI_FLASH_USR_DUMMY | SPI_FLASH_USR_DIN | SPI_USR_COMMAND | SPI_DOUTDIN);
			break;
		case spi_mode_txrx:
			regvalue |= SPI_DOUTDIN | SPI_CK_I_EDGE;
		    regvalue &= ~(BIT2 | SPI_FLASH_USR_ADDR | SPI_FLASH_USR_DUMMY | SPI_FLASH_USR_DIN | SPI_USR_COMMAND);
			break;
	}

	WRITE_PERI_REG(SPI_FLASH_USER(current_spi_port), regvalue);
}

void spi_send_uint16_r(uint16_t data, int32_t repeats)
{
	uint32_t i;
	uint32_t word = data << 16 | data;

	while(repeats > 0)
	{
		uint16_t bytes_to_transfer = __min(repeats * sizeof(uint16_t) , SPIFIFOSIZE * sizeof(uint32_t));
		spi_wait_ready();
		spi_prepare_tx(bytes_to_transfer);
		for(i = 0; i < (bytes_to_transfer + 3) / 4;i++)
			spi_fifo[i] = word;
		spi_start_tx();
		repeats -= bytes_to_transfer / 2;
	}
}

void spi_send_data(const uint8_t * data, uint8_t datasize)
{
	uint32_t *_data = (uint32_t*)data;
	uint8_t i;

	uint8_t words_to_send = __min((datasize + 3) / 4, SPIFIFOSIZE);
	spi_prepare_tx(datasize);
	for(i = 0; i < words_to_send;i++)
		spi_fifo[i] = _data[i];
	spi_start_tx();
}

uint16_t spi_send_uint8_receive_13bits(uint8_t to_send)
{
	spi_prepare_txrx_bits(8, 13);
	*spi_fifo = to_send;
	spi_start_tx();
	spi_wait_ready();
	return (*spi_fifo) & 0x1FFF;
}
