
#include <Arduino.h>

#include <unistd.h> // write

HardwareSerial Serial(UART0);

HardwareSerial::HardwareSerial (int uart_nr)
{
	if (uart_nr != 0)
		fprintf(stderr, MOCK "FIXME HardwareSerial::HardwareSerial(%d)\n", uart_nr);
}

void HardwareSerial::begin(unsigned long baud, SerialConfig config, SerialMode mode, uint8_t tx_pin)
{
	if (config != SERIAL_8N1 || mode != SERIAL_FULL || tx_pin != 1)
		fprintf(stderr, MOCK "FIXME HardwareSerial::begin(baud=%ld config=0x%x mode=0x%x)\n", baud, (int)config, (int)mode);
}

int HardwareSerial::available(void)
{
	printf(MOCK "TODO HardwareSerial::available\n");
	return 0;
}

void HardwareSerial::flush()
{
	//XXXTODO
	fflush(stdout);
}

// uart.c

extern "C"
{

size_t uart_write_char(uart_t* uart, char c)
{
	//XXXTODO
	(void)uart;
	write(1, &c, 1);
	return 1;
}

int uart_peek_char(uart_t* uart)
{
	///XXXTODO
	(void)uart;
	return -1;
}

int uart_read_char(uart_t* uart)
{
	///XXXTODO
	(void)uart;
	return -1;
}

size_t uart_write(uart_t* uart, const char* buf, size_t size)
{
	///XXXTODO
	(void)uart;
	return write(1, buf, size);
}

} // extern "C"
