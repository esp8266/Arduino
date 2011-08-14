
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/boot.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

//#define MICROTOUCH

//#define WRITABLE_DIRECTORY	// undef saved 56 + 512 RAM

#define FAT_DISK_LABEL	'b','o','o','t','l','o','a','d','e','r',' '		// 11 chars (undef saves 12)
#define FAT_FILE_NAME	'F','I','R','M','W','A','R','E','B','I','N'		// 11 chars
#define MSC_DISK_SERIAL	'0','0','0','0','0','0','0','0','1','7','0','1'	// 12 chars


#define CPU_PRESCALE(n)	(CLKPR = 0x80, CLKPR = (n))
#define DISABLE_JTAG()  MCUCR = (1 << JTD) | (1 << IVCE) | (0 << PUD); MCUCR = (1 << JTD) | (0 << IVSEL) | (0 << IVCE) | (0 << PUD);

#define USB_VID 0x2341	// arduino LLC vid
#define USB_PID 0x0020	// educated guess

#define FAT_OEM_NAME	'l','e','o','n','a','r','d','o'					// 8 chars
#define BOARD_INIT()	DDRC |= (1<<7); DDRB |= (1<<0); DDRE |= (1<<6); CPU_PRESCALE(0); DISABLE_JTAG();
#define LED0			PORTC &= ~(1<<7)
#define LED1			PORTC |= (1<<7)
#define TXLED0			PORTE |= (1<<6)
#define TXLED1			PORTE &= ~(1<<6)
#define RXLED0			PORTB |= (1<<0)
#define RXLED1			PORTB &= ~(1<<0)

#define TRANSFER_PGM		0x80
#define TRANSFER_RELEASE	0x40
#define TRANSFER_ZERO		0x20

void Transfer(u8 ep, const u8* data, int len);
void Recv(u8 ep, u8* dst, u8 len);
void Program(u8 ep, u16 page, u8 count);

#define CDC_ENABLED
//#define MSC_ENABLED
//#define MSC_NO_MEDIA	// MSC is enabled but not media inserted

#include "USBCore.h"
#include "USBDesc.h"


