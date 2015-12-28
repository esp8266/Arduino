/******************************************************************************
 * Copyright 2015 Espressif Systems
 *
 * Description: A stub to make the ESP8266 debuggable by GDB over the serial
 * port.
 *
 * License: ESPRESSIF MIT License
 *******************************************************************************/

#include "gdbstub.h"
#include <stddef.h>
#include "ets_sys.h"
#include "eagle_soc.h"
#include "c_types.h"
#include "gpio.h"
#include "xtensa/corebits.h"

#include "gdbstub.h"
#include "gdbstub-entry.h"
#include "gdbstub-cfg.h"


//From xtruntime-frames.h
struct XTensa_exception_frame_s {
	uint32_t pc;
	uint32_t ps;
	uint32_t sar;
	uint32_t vpri;
	uint32_t a0;
	uint32_t a[14]; //a2..a15
//These are added manually by the exception code; the HAL doesn't set these on an exception.
	uint32_t litbase;
	uint32_t sr176;
	uint32_t sr208;
	uint32_t a1;
	 //'reason' is abused for both the debug and the exception vector: if bit 7 is set,
	//this contains an exception reason, otherwise it contains a debug vector bitmap.
	uint32_t reason;
};


struct XTensa_rtos_int_frame_s {
	uint32_t exitPtr;
	uint32_t pc;
	uint32_t ps;
	uint32_t a[16];
	uint32_t sar;
};

#if GDBSTUB_FREERTOS
/*
Definitions for FreeRTOS. This redefines some os_* functions to use their non-os* counterparts. It
also sets up some function pointers for ROM functions that aren't in the FreeRTOS ld files.
*/
#include <string.h>
#include <stdio.h>
void _xt_isr_attach(int inum, void *fn);
void _xt_isr_unmask(int inum);
void os_install_putc1(void (*p)(char c));
#define os_printf(...) printf(__VA_ARGS__)
#define os_memcpy(a,b,c) memcpy(a,b,c)
typedef void wdtfntype();
static wdtfntype *ets_wdt_disable=(wdtfntype *)0x400030f0;
static wdtfntype *ets_wdt_enable=(wdtfntype *)0x40002fa0;

#else
/*
OS-less SDK defines. Defines some headers for things that aren't in the include files, plus
the xthal stack frame struct.
*/
#include "osapi.h"
#include "user_interface.h"

void _xtos_set_exception_handler(int cause, void (exhandler)(struct XTensa_exception_frame_s *frame));
int os_printf_plus(const char *format, ...)  __attribute__ ((format (printf, 1, 2)));

#endif

#define EXCEPTION_GDB_SP_OFFSET 0x100


//We need some UART register defines.
#define ETS_UART_INUM 5
#define REG_UART_BASE( i )  (0x60000000+(i)*0xf00)
#define UART_STATUS( i )                        (REG_UART_BASE( i ) + 0x1C)
#define UART_RXFIFO_CNT 0x000000FF
#define UART_RXFIFO_CNT_S 0
#define UART_TXFIFO_CNT 0x000000FF
#define UART_TXFIFO_CNT_S                   16
#define UART_FIFO( i )                          (REG_UART_BASE( i ) + 0x0)
#define UART_INT_ENA(i)                     (REG_UART_BASE(i) + 0xC)
#define UART_INT_CLR(i)                 (REG_UART_BASE(i) + 0x10)
#define UART_RXFIFO_TOUT_INT_ENA            (BIT(8))
#define UART_RXFIFO_FULL_INT_ENA            (BIT(0))
#define UART_RXFIFO_TOUT_INT_CLR            (BIT(8))
#define UART_RXFIFO_FULL_INT_CLR            (BIT(0))




//Length of buffer used to reserve GDB commands. Has to be at least able to fit the G command, which
//implies a minimum size of about 190 bytes.
#define PBUFLEN 256
//Length of gdb stdout buffer, for console redirection
#define OBUFLEN 32

//The asm stub saves the Xtensa registers here when a debugging exception happens.
struct XTensa_exception_frame_s gdbstub_savedRegs;
#if GDBSTUB_USE_OWN_STACK
//This is the debugging exception stack.
int exceptionStack[256];
#endif

static unsigned char cmd[PBUFLEN];		//GDB command input buffer
static char chsum;						//Running checksum of the output packet
#if GDBSTUB_REDIRECT_CONSOLE_OUTPUT
static unsigned char obuf[OBUFLEN];		//GDB stdout buffer
static int obufpos=0;					//Current position in the buffer
#endif
static int32_t singleStepPs=-1;			//Stores ps when single-stepping instruction. -1 when not in use.

//Small function to feed the hardware watchdog. Needed to stop the ESP from resetting
//due to a watchdog timeout while reading a command.
static void ATTR_GDBFN keepWDTalive() {
	uint64_t *wdtval=(uint64_t*)0x3ff21048;
	uint64_t *wdtovf=(uint64_t*)0x3ff210cc;
	int *wdtctl=(int*)0x3ff210c8;
	*wdtovf=*wdtval+1600000;
	*wdtctl|=(1<<31);
}

//Receive a char from the uart. Uses polling and feeds the watchdog.
static int ATTR_GDBFN gdbRecvChar() {
	int i;
	while (((READ_PERI_REG(UART_STATUS(0))>>UART_RXFIFO_CNT_S)&UART_RXFIFO_CNT)==0) {
		keepWDTalive();
	}
	i=READ_PERI_REG(UART_FIFO(0));
	return i;
}

//Send a char to the uart.
static void ATTR_GDBFN gdbSendChar(char c) {
	while (((READ_PERI_REG(UART_STATUS(0))>>UART_TXFIFO_CNT_S)&UART_TXFIFO_CNT)>=126) ;
	WRITE_PERI_REG(UART_FIFO(0), c);
}

//Send the start of a packet; reset checksum calculation.
static void ATTR_GDBFN gdbPacketStart() {
	chsum=0;
	gdbSendChar('$');
}

//Send a char as part of a packet
static void ATTR_GDBFN gdbPacketChar(char c) {
	if (c=='#' || c=='$' || c=='}' || c=='*') {
		gdbSendChar('}');
		gdbSendChar(c^0x20);
		chsum+=(c^0x20)+'}';
	} else {
		gdbSendChar(c);
		chsum+=c;
	}
}

//Send a string as part of a packet
static void ATTR_GDBFN gdbPacketStr(char *c) {
	while (*c!=0) {
		gdbPacketChar(*c);
		c++;
	}
}

//Send a hex val as part of a packet. 'bits'/4 dictates the number of hex chars sent.
static void ATTR_GDBFN gdbPacketHex(int val, int bits) {
	char hexChars[]="0123456789abcdef";
	int i;
	for (i=bits; i>0; i-=4) {
		gdbPacketChar(hexChars[(val>>(i-4))&0xf]);
	}
}

//Finish sending a packet.
static void ATTR_GDBFN gdbPacketEnd() {
	gdbSendChar('#');
	gdbPacketHex(chsum, 8);
}

//Error states used by the routines that grab stuff from the incoming gdb packet
#define ST_ENDPACKET -1
#define ST_ERR -2
#define ST_OK -3
#define ST_CONT -4

//Grab a hex value from the gdb packet. Ptr will get positioned on the end
//of the hex string, as far as the routine has read into it. Bits/4 indicates
//the max amount of hex chars it gobbles up. Bits can be -1 to eat up as much
//hex chars as possible.
static long ATTR_GDBFN gdbGetHexVal(unsigned char **ptr, int bits) {
	int i;
	int no;
	unsigned int v=0;
	char c;
	no=bits/4;
	if (bits==-1) no=64;
	for (i=0; i<no; i++) {
		c=**ptr;
		(*ptr)++;
		if (c>='0' && c<='9') {
			v<<=4;
			v|=(c-'0');
		} else if (c>='A' && c<='F') {
			v<<=4;
			v|=(c-'A')+10;
		} else if (c>='a' && c<='f') {
			v<<=4;
			v|=(c-'a')+10;
		} else if (c=='#') {
			if (bits==-1) {
				(*ptr)--;
				return v;
			}
			return ST_ENDPACKET;
		} else {
			if (bits==-1) {
				(*ptr)--;
				return v;
			}
			return ST_ERR;
		}
	}
	return v;
}

//Swap an int into the form gdb wants it
static int ATTR_GDBFN iswap(int i) {
	int r;
	r=((i>>24)&0xff);
	r|=((i>>16)&0xff)<<8;
	r|=((i>>8)&0xff)<<16;
	r|=((i>>0)&0xff)<<24;
	return r;
}

//Read a byte from the ESP8266 memory.
static unsigned char ATTR_GDBFN readbyte(unsigned int p) {
	int *i=(int*)(p&(~3));
	if (p<0x20000000 || p>=0x60000000) return -1;
	return *i>>((p&3)*8);
}

//Write a byte to the ESP8266 memory.
static void ATTR_GDBFN writeByte(unsigned int p, unsigned char d) {
	int *i=(int*)(p&(~3));
	if (p<0x20000000 || p>=0x60000000) return;
	if ((p&3)==0) *i=(*i&0xffffff00)|(d<<0);
	if ((p&3)==1) *i=(*i&0xffff00ff)|(d<<8);
	if ((p&3)==2) *i=(*i&0xff00ffff)|(d<<16);
	if ((p&3)==3) *i=(*i&0x00ffffff)|(d<<24);
}

//Returns 1 if it makes sense to write to addr p
static int ATTR_GDBFN validWrAddr(int p) {
	if (p>=0x3ff00000 && p<0x40000000) return 1;
	if (p>=0x40100000 && p<0x40140000) return 1;
	if (p>=0x60000000 && p<0x60002000) return 1;
	return 0;
}

/*
Register file in the format lx106 gdb port expects it.
Inspired by gdb/regformats/reg-xtensa.dat from
https://github.com/jcmvbkbc/crosstool-NG/blob/lx106-g%2B%2B/overlays/xtensa_lx106.tar
As decoded by Cesanta.
*/
struct regfile {
	uint32_t a[16];
	uint32_t pc;
	uint32_t sar;
	uint32_t litbase;
	uint32_t sr176;
	uint32_t sr208;
	uint32_t ps;
};


//Send the reason execution is stopped to GDB.
static void ATTR_GDBFN sendReason() {
#if 0
	char *reason=""; //default
#endif
	//exception-to-signal mapping
	char exceptionSignal[]={4,31,11,11,2,6,8,0,6,7,0,0,7,7,7,7};
	int i=0;
	gdbPacketStart();
	gdbPacketChar('T');
	if (gdbstub_savedRegs.reason==0xff) {
		gdbPacketHex(2, 8); //sigint
	} else if (gdbstub_savedRegs.reason&0x80) {
		//We stopped because of an exception. Convert exception code to a signal number and send it.
		i=gdbstub_savedRegs.reason&0x7f;
		if (i<sizeof(exceptionSignal)) gdbPacketHex(exceptionSignal[i], 8); else gdbPacketHex(11, 8);
	} else {
		//We stopped because of a debugging exception.
		gdbPacketHex(5, 8); //sigtrap
//Current Xtensa GDB versions don't seem to request this, so let's leave it off.
#if 0
		if (gdbstub_savedRegs.reason&(1<<0)) reason="break";
		if (gdbstub_savedRegs.reason&(1<<1)) reason="hwbreak";
		if (gdbstub_savedRegs.reason&(1<<2)) reason="watch";
		if (gdbstub_savedRegs.reason&(1<<3)) reason="swbreak";
		if (gdbstub_savedRegs.reason&(1<<4)) reason="swbreak";

		gdbPacketStr(reason);
		gdbPacketChar(':');
		//ToDo: watch: send address
#endif
	}
	gdbPacketEnd();
}

//Handle a command as received from GDB.
static int ATTR_GDBFN gdbHandleCommand(unsigned char *cmd, int len) {
	//Handle a command
	int i, j, k;
	unsigned char *data=cmd+1;
	if (cmd[0]=='g') {		//send all registers to gdb
		gdbPacketStart();
		gdbPacketHex(iswap(gdbstub_savedRegs.a0), 32);
		gdbPacketHex(iswap(gdbstub_savedRegs.a1), 32);
		for (i=2; i<16; i++) gdbPacketHex(iswap(gdbstub_savedRegs.a[i-2]), 32);
		gdbPacketHex(iswap(gdbstub_savedRegs.pc), 32);
		gdbPacketHex(iswap(gdbstub_savedRegs.sar), 32);
		gdbPacketHex(iswap(gdbstub_savedRegs.litbase), 32);
		gdbPacketHex(iswap(gdbstub_savedRegs.sr176), 32);
		gdbPacketHex(0, 32);
		gdbPacketHex(iswap(gdbstub_savedRegs.ps), 32);
		gdbPacketEnd();
	} else if (cmd[0]=='G') {	//receive content for all registers from gdb
		gdbstub_savedRegs.a0=iswap(gdbGetHexVal(&data, 32));
		gdbstub_savedRegs.a1=iswap(gdbGetHexVal(&data, 32));
		for (i=2; i<16; i++) gdbstub_savedRegs.a[i-2]=iswap(gdbGetHexVal(&data, 32));
		gdbstub_savedRegs.pc=iswap(gdbGetHexVal(&data, 32));
		gdbstub_savedRegs.sar=iswap(gdbGetHexVal(&data, 32));
		gdbstub_savedRegs.litbase=iswap(gdbGetHexVal(&data, 32));
		gdbstub_savedRegs.sr176=iswap(gdbGetHexVal(&data, 32));
		gdbGetHexVal(&data, 32);
		gdbstub_savedRegs.ps=iswap(gdbGetHexVal(&data, 32));
		gdbPacketStart();
		gdbPacketStr("OK");
		gdbPacketEnd();
	} else if (cmd[0]=='m') {	//read memory to gdb
		i=gdbGetHexVal(&data, -1);
		data++;
		j=gdbGetHexVal(&data, -1);
		gdbPacketStart();
		for (k=0; k<j; k++) {
			gdbPacketHex(readbyte(i++), 8);
		}
		gdbPacketEnd();
	} else if (cmd[0]=='M') {	//write memory from gdb
		i=gdbGetHexVal(&data, -1); //addr
		data++; //skip ,
		j=gdbGetHexVal(&data, -1); //length
		data++; //skip :
		if (validWrAddr(i) && validWrAddr(i+j)) {
			for (k=0; k<j; k++) {
				writeByte(i, gdbGetHexVal(&data, 8));
				i++;
			}
			//Make sure caches are up-to-date. Procedure according to Xtensa ISA document, ISYNC inst desc.
			asm volatile("ISYNC\nISYNC\n");
			gdbPacketStart();
			gdbPacketStr("OK");
			gdbPacketEnd();
		} else {
			//Trying to do a software breakpoint on a flash proc, perhaps?
			gdbPacketStart();
			gdbPacketStr("E01");
			gdbPacketEnd();
		}
	} else if (cmd[0]=='?') {	//Reply with stop reason
		sendReason();
//	} else if (strncmp(cmd, "vCont?", 6)==0) {
//		gdbPacketStart();
//		gdbPacketStr("vCont;c;s");
//		gdbPacketEnd();
	} else if (strncmp((char*)cmd, "vCont;c", 7)==0 || cmd[0]=='c') {	//continue execution
		return ST_CONT;
	} else if (strncmp((char*)cmd, "vCont;s", 7)==0 || cmd[0]=='s') {	//single-step instruction
		//Single-stepping can go wrong if an interrupt is pending, especially when it is e.g. a task switch:
		//the ICOUNT register will overflow in the task switch code. That is why we disable interupts when
		//doing single-instruction stepping.
		singleStepPs=gdbstub_savedRegs.ps;
		gdbstub_savedRegs.ps=(gdbstub_savedRegs.ps & ~0xf)|(XCHAL_DEBUGLEVEL-1);
		gdbstub_icount_ena_single_step();
		return ST_CONT;
	} else if (cmd[0]=='q') {	//Extended query
		if (strncmp((char*)&cmd[1], "Supported", 9)==0) { //Capabilities query
			gdbPacketStart();
			gdbPacketStr("swbreak+;hwbreak+;PacketSize=255");
			gdbPacketEnd();
		} else {
			//We don't support other queries.
			gdbPacketStart();
			gdbPacketEnd();
			return ST_ERR;
		}
	} else if (cmd[0]=='Z') {	//Set hardware break/watchpoint.
		data+=2; //skip 'x,'
		i=gdbGetHexVal(&data, -1);
		data++; //skip ','
		j=gdbGetHexVal(&data, -1);
		gdbPacketStart();
		if (cmd[1]=='1') {	//Set breakpoint
			if (gdbstub_set_hw_breakpoint(i, j)) {
				gdbPacketStr("OK");
			} else {
				gdbPacketStr("E01");
			}
		} else if (cmd[1]=='2' || cmd[1]=='3' || cmd[1]=='4') { //Set watchpoint
			int access=0;
			int mask=0;
			if (cmd[1]=='2') access=2; //write
			if (cmd[1]=='3') access=1; //read
			if (cmd[1]=='4') access=3; //access
			if (j==1) mask=0x3F;
			if (j==2) mask=0x3E;
			if (j==4) mask=0x3C;
			if (j==8) mask=0x38;
			if (j==16) mask=0x30;
			if (j==32) mask=0x20;
			if (j==64) mask=0x00;
			if (mask!=0 && gdbstub_set_hw_watchpoint(i,mask, access)) {
				gdbPacketStr("OK");
			} else {
				gdbPacketStr("E01");
			}
		}
		gdbPacketEnd();
	} else if (cmd[0]=='z') {	//Clear hardware break/watchpoint
		data+=2; //skip 'x,'
		i=gdbGetHexVal(&data, -1);
		data++; //skip ','
		j=gdbGetHexVal(&data, -1);
		gdbPacketStart();
		if (cmd[1]=='1') {	//hardware breakpoint
			if (gdbstub_del_hw_breakpoint(i)) {
				gdbPacketStr("OK");
			} else {
				gdbPacketStr("E01");
			}
		} else if (cmd[1]=='2' || cmd[1]=='3' || cmd[1]=='4') { //hardware watchpoint
			if (gdbstub_del_hw_watchpoint(i)) {
				gdbPacketStr("OK");
			} else {
				gdbPacketStr("E01");
			}
		}
		gdbPacketEnd();
	} else {
		//We don't recognize or support whatever GDB just sent us.
		gdbPacketStart();
		gdbPacketEnd();
		return ST_ERR;
	}
	return ST_OK;
}


//Lower layer: grab a command packet and check the checksum
//Calls gdbHandleCommand on the packet if the checksum is OK
//Returns ST_OK on success, ST_ERR when checksum fails, a
//character if it is received instead of the GDB packet
//start char.
static int ATTR_GDBFN gdbReadCommand() {
	unsigned char c;
	unsigned char chsum=0, rchsum;
	unsigned char sentchs[2];
	int p=0;
	unsigned char *ptr;
	c=gdbRecvChar();
	if (c!='$') return c;
	while(1) {
		c=gdbRecvChar();
		if (c=='#') {	//end of packet, checksum follows
			cmd[p]=0;
			break;
		}
		chsum+=c;
		if (c=='$') {
			//Wut, restart packet?
			chsum=0;
			p=0;
			continue;
		}
		if (c=='}') {		//escape the next char
			c=gdbRecvChar();
			chsum+=c;
			c^=0x20;
		}
		cmd[p++]=c;
		if (p>=PBUFLEN) return ST_ERR;
	}
	//A # has been received. Get and check the received chsum.
	sentchs[0]=gdbRecvChar();
	sentchs[1]=gdbRecvChar();
	ptr=&sentchs[0];
	rchsum=gdbGetHexVal(&ptr, 8);
//	os_printf("c %x r %x\n", chsum, rchsum);
	if (rchsum!=chsum) {
		gdbSendChar('-');
		return ST_ERR;
	} else {
		gdbSendChar('+');
		return gdbHandleCommand(cmd, p);
	}
}

//Get the value of one of the A registers
static unsigned int ATTR_GDBFN getaregval(int reg) {
	if (reg==0) return gdbstub_savedRegs.a0;
	if (reg==1) return gdbstub_savedRegs.a1;
	return gdbstub_savedRegs.a[reg-2];
}

//Set the value of one of the A registers
static void ATTR_GDBFN setaregval(int reg, unsigned int val) {
	os_printf("%x -> %x\n", val, reg);
	if (reg==0) gdbstub_savedRegs.a0=val;
	if (reg==1) gdbstub_savedRegs.a1=val;
	gdbstub_savedRegs.a[reg-2]=val;
}

//Emulate the l32i/s32i instruction we're stopped at.
static void ATTR_GDBFN emulLdSt() {
	unsigned char i0=readbyte(gdbstub_savedRegs.pc);
	unsigned char i1=readbyte(gdbstub_savedRegs.pc+1);
	unsigned char i2=readbyte(gdbstub_savedRegs.pc+2);
	int *p;
	if ((i0&0xf)==2 && (i1&0xf0)==0x20) {
		//l32i
		p=(int*)getaregval(i1&0xf)+(i2*4);
		setaregval(i0>>4, *p);
		gdbstub_savedRegs.pc+=3;
	} else if ((i0&0xf)==0x8) {
		//l32i.n
		p=(int*)getaregval(i1&0xf)+((i1>>4)*4);
		setaregval(i0>>4, *p);
		gdbstub_savedRegs.pc+=2;
	} else if ((i0&0xf)==2 && (i1&0xf0)==0x60) {
		//s32i
		p=(int*)getaregval(i1&0xf)+(i2*4);
		*p=getaregval(i0>>4);
		gdbstub_savedRegs.pc+=3;
	} else if ((i0&0xf)==0x9) {
		//s32i.n
		p=(int*)getaregval(i1&0xf)+((i1>>4)*4);
		*p=getaregval(i0>>4);
		gdbstub_savedRegs.pc+=2;
	} else {
		os_printf("GDBSTUB: No l32i/s32i instruction: %x %x %x. Huh?", i2, i1, i0);
	}
}

//We just caught a debug exception and need to handle it. This is called from an assembly
//routine in gdbstub-entry.S
void ATTR_GDBFN gdbstub_handle_debug_exception() {
	ets_wdt_disable();

	if (singleStepPs!=-1) {
		//We come here after single-stepping an instruction. Interrupts are disabled
		//for the single step. Re-enable them here.
		gdbstub_savedRegs.ps=(gdbstub_savedRegs.ps&~0xf)|(singleStepPs&0xf);
		singleStepPs=-1;
	}

	sendReason();
	while(gdbReadCommand()!=ST_CONT);
	if ((gdbstub_savedRegs.reason&0x84)==0x4) {
		//We stopped due to a watchpoint. We can't re-execute the current instruction
		//because it will happily re-trigger the same watchpoint, so we emulate it
		//while we're still in debugger space.
		emulLdSt();
	} else if ((gdbstub_savedRegs.reason&0x88)==0x8) {
		//We stopped due to a BREAK instruction. Skip over it.
		//Check the instruction first; gdb may have replaced it with the original instruction
		//if it's one of the breakpoints it set.
		if (readbyte(gdbstub_savedRegs.pc+2)==0 &&
					(readbyte(gdbstub_savedRegs.pc+1)&0xf0)==0x40 &&
					(readbyte(gdbstub_savedRegs.pc)&0x0f)==0x00) {
			gdbstub_savedRegs.pc+=3;
		}
	} else if ((gdbstub_savedRegs.reason&0x90)==0x10) {
		//We stopped due to a BREAK.N instruction. Skip over it, after making sure the instruction
		//actually is a BREAK.N
		if ((readbyte(gdbstub_savedRegs.pc+1)&0xf0)==0xf0 &&
					readbyte(gdbstub_savedRegs.pc)==0x2d) {
			gdbstub_savedRegs.pc+=3;
		}
	}
	ets_wdt_enable();
}


#if GDBSTUB_FREERTOS
//Freetos exception. This routine is called by an assembly routine in gdbstub-entry.S
void ATTR_GDBFN gdbstub_handle_user_exception() {
	ets_wdt_disable();
	gdbstub_savedRegs.reason|=0x80; //mark as an exception reason
	sendReason();
	while(gdbReadCommand()!=ST_CONT);
	ets_wdt_enable();
}
#else

//Non-OS exception handler. Gets called by the Xtensa HAL.
static void ATTR_GDBFN gdb_exception_handler(struct XTensa_exception_frame_s *frame) {
	//Save the extra registers the Xtensa HAL doesn't save
	gdbstub_save_extra_sfrs_for_exception();
	//Copy registers the Xtensa HAL did save to gdbstub_savedRegs
	os_memcpy(&gdbstub_savedRegs, frame, 19*4);
	//Credits go to Cesanta for this trick. A1 seems to be destroyed, but because it
	//has a fixed offset from the address of the passed frame, we can recover it.
	gdbstub_savedRegs.a1=(uint32_t)frame+EXCEPTION_GDB_SP_OFFSET;

	gdbstub_savedRegs.reason|=0x80; //mark as an exception reason

	ets_wdt_disable();
	*((uint32_t*)UART_INT_ENA(0)) = 0;
	sendReason();
	while(gdbReadCommand()!=ST_CONT);
	ets_wdt_enable();

	//Copy any changed registers back to the frame the Xtensa HAL uses.
	os_memcpy(frame, &gdbstub_savedRegs, 19*4);
}
#endif

#if GDBSTUB_REDIRECT_CONSOLE_OUTPUT
//Replacement putchar1 routine. Instead of spitting out the character directly, it will buffer up to
//OBUFLEN characters (or up to a \n, whichever comes earlier) and send it out as a gdb stdout packet.
static void ATTR_GDBFN gdb_semihost_putchar1(char c) {
	int i;
	obuf[obufpos++]=c;
	if (c=='\n' || obufpos==OBUFLEN) {
		gdbPacketStart();
		gdbPacketChar('O');
		for (i=0; i<obufpos; i++) gdbPacketHex(obuf[i], 8);
		gdbPacketEnd();
		obufpos=0;
	}
}
#endif

#if !GDBSTUB_FREERTOS
//The OS-less SDK uses the Xtensa HAL to handle exceptions. We can use those functions to catch any
//fatal exceptions and invoke the debugger when this happens.
static void ATTR_GDBINIT install_exceptions() {
	int i;
	int exno[]={EXCCAUSE_ILLEGAL, EXCCAUSE_SYSCALL, EXCCAUSE_INSTR_ERROR, EXCCAUSE_LOAD_STORE_ERROR,
			EXCCAUSE_DIVIDE_BY_ZERO, EXCCAUSE_UNALIGNED, EXCCAUSE_INSTR_DATA_ERROR, EXCCAUSE_LOAD_STORE_DATA_ERROR,
			EXCCAUSE_INSTR_ADDR_ERROR, EXCCAUSE_LOAD_STORE_ADDR_ERROR, EXCCAUSE_INSTR_PROHIBITED,
			EXCCAUSE_LOAD_PROHIBITED, EXCCAUSE_STORE_PROHIBITED};
	for (i=0; i<(sizeof(exno)/sizeof(exno[0])); i++) {
		_xtos_set_exception_handler(exno[i], gdb_exception_handler);
	}
}
#else
//FreeRTOS doesn't use the Xtensa HAL for exceptions, but uses its own fatal exception handler.
//We use a small hack to replace that with a jump to our own handler, which then has the task of
//decyphering and re-instating the registers the FreeRTOS code left.
extern void user_fatal_exception_handler();
extern void gdbstub_user_exception_entry();

static void ATTR_GDBINIT install_exceptions() {
	//Replace the user_fatal_exception_handler by a jump to our own code
	int *ufe=(int*)user_fatal_exception_handler;
	//This mess encodes as a relative jump instruction to user_fatal_exception_handler
	*ufe=((((int)gdbstub_user_exception_entry-(int)user_fatal_exception_handler)-4)<<6)|6;
}
#endif



#if GDBSTUB_CTRLC_BREAK

#if !GDBSTUB_FREERTOS

static void ATTR_GDBFN uart_hdlr(void *arg, void *frame) {
	int doDebug=0, fifolen=0;
	//Save the extra registers the Xtensa HAL doesn't save
	gdbstub_save_extra_sfrs_for_exception();

	fifolen=(READ_PERI_REG(UART_STATUS(0))>>UART_RXFIFO_CNT_S)&UART_RXFIFO_CNT;
	while (fifolen!=0) {
		if ((READ_PERI_REG(UART_FIFO(0)) & 0xFF)==0x3) doDebug=1; //Check if any of the chars is control-C. Throw away rest.
		fifolen--;
	}
	WRITE_PERI_REG(UART_INT_CLR(0), UART_RXFIFO_FULL_INT_CLR|UART_RXFIFO_TOUT_INT_CLR);

	if (doDebug) {
		//Copy registers the Xtensa HAL did save to gdbstub_savedRegs
		os_memcpy(&gdbstub_savedRegs, frame, 19*4);
		gdbstub_savedRegs.a1=(uint32_t)frame+EXCEPTION_GDB_SP_OFFSET;

		gdbstub_savedRegs.reason=0xff; //mark as user break reason

		ets_wdt_disable();
		sendReason();
		while(gdbReadCommand()!=ST_CONT);
		ets_wdt_enable();
		//Copy any changed registers back to the frame the Xtensa HAL uses.
		os_memcpy(frame, &gdbstub_savedRegs, 19*4);
	}
}

static void ATTR_GDBINIT install_uart_hdlr() {
	ets_isr_attach(ETS_UART_INUM, uart_hdlr, NULL);
	SET_PERI_REG_MASK(UART_INT_ENA(0), UART_RXFIFO_FULL_INT_ENA|UART_RXFIFO_TOUT_INT_ENA);
	ets_isr_unmask((1<<ETS_UART_INUM)); //enable uart interrupt
}

#else

void ATTR_GDBFN gdbstub_handle_uart_int(struct XTensa_rtos_int_frame_s *frame) {
	int doDebug=0, fifolen=0, x;

	fifolen=(READ_PERI_REG(UART_STATUS(0))>>UART_RXFIFO_CNT_S)&UART_RXFIFO_CNT;
	while (fifolen!=0) {
		if ((READ_PERI_REG(UART_FIFO(0)) & 0xFF)==0x3) doDebug=1; //Check if any of the chars is control-C. Throw away rest.
		fifolen--;
	}
	WRITE_PERI_REG(UART_INT_CLR(0), UART_RXFIFO_FULL_INT_CLR|UART_RXFIFO_TOUT_INT_CLR);

	if (doDebug) {
		//Copy registers the Xtensa HAL did save to gdbstub_savedRegs
		gdbstub_savedRegs.pc=frame->pc;
		gdbstub_savedRegs.ps=frame->ps;
		gdbstub_savedRegs.sar=frame->sar;
		gdbstub_savedRegs.a0=frame->a[0];
		gdbstub_savedRegs.a1=frame->a[1];
		for (x=2; x<16; x++) gdbstub_savedRegs.a[x-2]=frame->a[x];

//		gdbstub_savedRegs.a1=(uint32_t)frame+EXCEPTION_GDB_SP_OFFSET;

		gdbstub_savedRegs.reason=0xff; //mark as user break reason

//		ets_wdt_disable();
		sendReason();
		while(gdbReadCommand()!=ST_CONT);
//		ets_wdt_enable();
		//Copy any changed registers back to the frame the Xtensa HAL uses.
		frame->pc=gdbstub_savedRegs.pc;
		frame->ps=gdbstub_savedRegs.ps;
		frame->sar=gdbstub_savedRegs.sar;
		frame->a[0]=gdbstub_savedRegs.a0;
		frame->a[1]=gdbstub_savedRegs.a1;
		for (x=2; x<16; x++) frame->a[x]=gdbstub_savedRegs.a[x-2];
	}
}

static void ATTR_GDBINIT install_uart_hdlr() {
	_xt_isr_attach(ETS_UART_INUM, gdbstub_uart_entry);
	SET_PERI_REG_MASK(UART_INT_ENA(0), UART_RXFIFO_FULL_INT_ENA|UART_RXFIFO_TOUT_INT_ENA);
	_xt_isr_unmask((1<<ETS_UART_INUM)); //enable uart interrupt
}

#endif

#endif



//gdbstub initialization routine.
void ATTR_GDBINIT gdbstub_init() {
#if GDBSTUB_REDIRECT_CONSOLE_OUTPUT
	os_install_putc1(gdb_semihost_putchar1);
#endif
#if GDBSTUB_CTRLC_BREAK
	install_uart_hdlr();
#endif
	install_exceptions();
	gdbstub_init_debug_entry();
#if GDBSTUB_BREAK_ON_INIT
	gdbstub_do_break();
#endif
}

extern void gdb_init() __attribute__((weak, alias("gdbstub_init")));
