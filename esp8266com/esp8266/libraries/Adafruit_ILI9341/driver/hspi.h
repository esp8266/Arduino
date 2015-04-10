#ifndef INCLUDE_HSPI_H_
#define INCLUDE_HSPI_H_

#include "spi_register.h"
#include <osapi.h>
#include <os_type.h>
#include <gpio.h>
#include <ets_sys.h>

#define SPI         0
#define HSPI        1

#define SPIFIFOSIZE 16 //16 words length

extern uint32_t *spi_fifo;
extern uint32_t current_spi_port;

typedef enum
{
	spi_mode_tx,
	spi_mode_txrx
} spi_mode;

typedef struct
{
	uint32_t spi_port;
	uint32_t clock_reg_val;
	spi_mode mode;
} spi_config;

spi_config spi_init(uint32_t spi_port, uint32_t prescaler, spi_mode mode);
void spi_reinit(spi_config *config);
void spi_send_data(const uint8_t * data, uint8_t datasize);
void spi_send_uint16_r(const uint16_t data, int32_t repeats);
static inline void spi_wait_ready(void){while (READ_PERI_REG(SPI_FLASH_CMD(current_spi_port))&SPI_FLASH_USR);}

static inline void spi_prepare_tx(uint32_t bytecount)
{
	uint32_t bitcount = bytecount * 8 - 1;

	WRITE_PERI_REG(SPI_FLASH_USER1(current_spi_port), (bitcount & SPI_USR_OUT_BITLEN) << SPI_USR_OUT_BITLEN_S);
}

static inline void spi_prepare_txrx_bits(uint32_t txbitcount, uint32_t rxbitcount)
{
	WRITE_PERI_REG(SPI_FLASH_USER1(current_spi_port), ((txbitcount & SPI_USR_OUT_BITLEN) << SPI_USR_OUT_BITLEN_S) |
										   ((rxbitcount & SPI_USR_DIN_BITLEN) << SPI_USR_DIN_BITLEN_S));
}

static inline void spi_start_tx()
{
	SET_PERI_REG_MASK(SPI_FLASH_CMD(current_spi_port), SPI_FLASH_USR);   // send
}

static inline void spi_send_uint8(uint8_t data)
{
	spi_prepare_tx(1);
	*spi_fifo = data;
	spi_start_tx();
}

static inline void spi_send_uint16(uint16_t data)
{
	spi_prepare_tx(2);
	*spi_fifo = data;
	spi_start_tx();
}

static inline void spi_send_uint32(uint32_t data)
{
	spi_prepare_tx(4);
	*spi_fifo = data;
	spi_start_tx();
}

/* This is needed to implement XPT2046 driver since it transmits 13 bits back*/
extern uint16_t spi_send_uint8_receive_13bits(uint8_t to_send);

#endif /* INCLUDE_HSPI_H_ */
