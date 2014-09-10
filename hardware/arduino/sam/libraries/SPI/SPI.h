/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * Copyright (c) 2014 by Paul Stoffregen <paul@pjrc.com> (Transaction API)
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef _SPI_H_INCLUDED
#define _SPI_H_INCLUDED

#include "variant.h"
#include <stdio.h>

// SPI_HAS_TRANSACTION means SPI has beginTransaction(), endTransaction(),
// usingInterrupt(), and SPISetting(clock, bitOrder, dataMode)
#define SPI_HAS_TRANSACTION 1

#define SPI_MODE0 0x02
#define SPI_MODE1 0x00
#define SPI_MODE2 0x03
#define SPI_MODE3 0x01

enum SPITransferMode {
	SPI_CONTINUE,
	SPI_LAST
};

class SPISettings {
public:
	SPISettings(uint32_t clock, BitOrder bitOrder, uint8_t dataMode) {
		if (__builtin_constant_p(clock)) {
			init_AlwaysInline(clock, bitOrder, dataMode);
		} else {
			init_MightInline(clock, bitOrder, dataMode);
		}
	}
	SPISettings() {
		init_AlwaysInline(4000000, MSBFIRST, SPI_MODE0);
	}
private:
	void init_MightInline(uint32_t clock, BitOrder bitOrder, uint8_t dataMode) {
		init_AlwaysInline(clock, bitOrder, dataMode);
	}
	void init_AlwaysInline(uint32_t clock, BitOrder bitOrder, uint8_t dataMode)
	  __attribute__((__always_inline__)) {
		uint8_t div;
		border = bitOrder;
		if (__builtin_constant_p(clock)) {
			if      (clock >= F_CPU /   2) div =   2;
			else if (clock >= F_CPU /   3) div =   3;
			else if (clock >= F_CPU /   4) div =   4;
			else if (clock >= F_CPU /   5) div =   5;
			else if (clock >= F_CPU /   6) div =   6;
			else if (clock >= F_CPU /   7) div =   7;
			else if (clock >= F_CPU /   8) div =   8;
			else if (clock >= F_CPU /   9) div =   9;
			else if (clock >= F_CPU /  10) div =  10;
			else if (clock >= F_CPU /  11) div =  11;
			else if (clock >= F_CPU /  12) div =  12;
			else if (clock >= F_CPU /  13) div =  13;
			else if (clock >= F_CPU /  14) div =  14;
			else if (clock >= F_CPU /  15) div =  15;
			else if (clock >= F_CPU /  16) div =  16;
			else if (clock >= F_CPU /  17) div =  17;
			else if (clock >= F_CPU /  18) div =  18;
			else if (clock >= F_CPU /  19) div =  19;
			else if (clock >= F_CPU /  20) div =  20;
			else if (clock >= F_CPU /  21) div =  21;
			else if (clock >= F_CPU /  22) div =  22;
			else if (clock >= F_CPU /  23) div =  23;
			else if (clock >= F_CPU /  24) div =  24;
			else if (clock >= F_CPU /  25) div =  25;
			else if (clock >= F_CPU /  26) div =  26;
			else if (clock >= F_CPU /  27) div =  27;
			else if (clock >= F_CPU /  28) div =  28;
			else if (clock >= F_CPU /  29) div =  29;
			else if (clock >= F_CPU /  30) div =  30;
			else if (clock >= F_CPU /  31) div =  31;
			else if (clock >= F_CPU /  32) div =  32;
			else if (clock >= F_CPU /  33) div =  33;
			else if (clock >= F_CPU /  34) div =  34;
			else if (clock >= F_CPU /  35) div =  35;
			else if (clock >= F_CPU /  36) div =  36;
			else if (clock >= F_CPU /  37) div =  37;
			else if (clock >= F_CPU /  38) div =  38;
			else if (clock >= F_CPU /  39) div =  39;
			else if (clock >= F_CPU /  40) div =  40;
			else if (clock >= F_CPU /  41) div =  41;
			else if (clock >= F_CPU /  42) div =  42;
			else if (clock >= F_CPU /  43) div =  43;
			else if (clock >= F_CPU /  44) div =  44;
			else if (clock >= F_CPU /  45) div =  45;
			else if (clock >= F_CPU /  46) div =  46;
			else if (clock >= F_CPU /  47) div =  47;
			else if (clock >= F_CPU /  48) div =  48;
			else if (clock >= F_CPU /  49) div =  49;
			else if (clock >= F_CPU /  50) div =  50;
			else if (clock >= F_CPU /  51) div =  51;
			else if (clock >= F_CPU /  52) div =  52;
			else if (clock >= F_CPU /  53) div =  53;
			else if (clock >= F_CPU /  54) div =  54;
			else if (clock >= F_CPU /  55) div =  55;
			else if (clock >= F_CPU /  56) div =  56;
			else if (clock >= F_CPU /  57) div =  57;
			else if (clock >= F_CPU /  58) div =  58;
			else if (clock >= F_CPU /  59) div =  59;
			else if (clock >= F_CPU /  60) div =  60;
			else if (clock >= F_CPU /  61) div =  61;
			else if (clock >= F_CPU /  62) div =  62;
			else if (clock >= F_CPU /  63) div =  63;
			else if (clock >= F_CPU /  64) div =  64;
			else if (clock >= F_CPU /  65) div =  65;
			else if (clock >= F_CPU /  66) div =  66;
			else if (clock >= F_CPU /  67) div =  67;
			else if (clock >= F_CPU /  68) div =  68;
			else if (clock >= F_CPU /  69) div =  69;
			else if (clock >= F_CPU /  70) div =  70;
			else if (clock >= F_CPU /  71) div =  71;
			else if (clock >= F_CPU /  72) div =  72;
			else if (clock >= F_CPU /  73) div =  73;
			else if (clock >= F_CPU /  74) div =  74;
			else if (clock >= F_CPU /  75) div =  75;
			else if (clock >= F_CPU /  76) div =  76;
			else if (clock >= F_CPU /  77) div =  77;
			else if (clock >= F_CPU /  78) div =  78;
			else if (clock >= F_CPU /  79) div =  79;
			else if (clock >= F_CPU /  80) div =  80;
			else if (clock >= F_CPU /  81) div =  81;
			else if (clock >= F_CPU /  82) div =  82;
			else if (clock >= F_CPU /  83) div =  83;
			else if (clock >= F_CPU /  84) div =  84;
			else if (clock >= F_CPU /  85) div =  85;
			else if (clock >= F_CPU /  86) div =  86;
			else if (clock >= F_CPU /  87) div =  87;
			else if (clock >= F_CPU /  88) div =  88;
			else if (clock >= F_CPU /  89) div =  89;
			else if (clock >= F_CPU /  90) div =  90;
			else if (clock >= F_CPU /  91) div =  91;
			else if (clock >= F_CPU /  92) div =  92;
			else if (clock >= F_CPU /  93) div =  93;
			else if (clock >= F_CPU /  94) div =  94;
			else if (clock >= F_CPU /  95) div =  95;
			else if (clock >= F_CPU /  96) div =  96;
			else if (clock >= F_CPU /  97) div =  97;
			else if (clock >= F_CPU /  98) div =  98;
			else if (clock >= F_CPU /  99) div =  99;
			else if (clock >= F_CPU / 100) div = 100;
			else if (clock >= F_CPU / 101) div = 101;
			else if (clock >= F_CPU / 102) div = 102;
			else if (clock >= F_CPU / 103) div = 103;
			else if (clock >= F_CPU / 104) div = 104;
			else if (clock >= F_CPU / 105) div = 105;
			else if (clock >= F_CPU / 106) div = 106;
			else if (clock >= F_CPU / 107) div = 107;
			else if (clock >= F_CPU / 108) div = 108;
			else if (clock >= F_CPU / 109) div = 109;
			else if (clock >= F_CPU / 110) div = 110;
			else if (clock >= F_CPU / 111) div = 111;
			else if (clock >= F_CPU / 112) div = 112;
			else if (clock >= F_CPU / 113) div = 113;
			else if (clock >= F_CPU / 114) div = 114;
			else if (clock >= F_CPU / 115) div = 115;
			else if (clock >= F_CPU / 116) div = 116;
			else if (clock >= F_CPU / 117) div = 117;
			else if (clock >= F_CPU / 118) div = 118;
			else if (clock >= F_CPU / 119) div = 119;
			else if (clock >= F_CPU / 120) div = 120;
			else if (clock >= F_CPU / 121) div = 121;
			else if (clock >= F_CPU / 122) div = 122;
			else if (clock >= F_CPU / 123) div = 123;
			else if (clock >= F_CPU / 124) div = 124;
			else if (clock >= F_CPU / 125) div = 125;
			else if (clock >= F_CPU / 126) div = 126;
			else if (clock >= F_CPU / 127) div = 127;
			else if (clock >= F_CPU / 128) div = 128;
			else if (clock >= F_CPU / 129) div = 129;
			else if (clock >= F_CPU / 130) div = 130;
			else if (clock >= F_CPU / 131) div = 131;
			else if (clock >= F_CPU / 132) div = 132;
			else if (clock >= F_CPU / 133) div = 133;
			else if (clock >= F_CPU / 134) div = 134;
			else if (clock >= F_CPU / 135) div = 135;
			else if (clock >= F_CPU / 136) div = 136;
			else if (clock >= F_CPU / 137) div = 137;
			else if (clock >= F_CPU / 138) div = 138;
			else if (clock >= F_CPU / 139) div = 139;
			else if (clock >= F_CPU / 140) div = 140;
			else if (clock >= F_CPU / 141) div = 141;
			else if (clock >= F_CPU / 142) div = 142;
			else if (clock >= F_CPU / 143) div = 143;
			else if (clock >= F_CPU / 144) div = 144;
			else if (clock >= F_CPU / 145) div = 145;
			else if (clock >= F_CPU / 146) div = 146;
			else if (clock >= F_CPU / 147) div = 147;
			else if (clock >= F_CPU / 148) div = 148;
			else if (clock >= F_CPU / 149) div = 149;
			else if (clock >= F_CPU / 150) div = 150;
			else if (clock >= F_CPU / 151) div = 151;
			else if (clock >= F_CPU / 152) div = 152;
			else if (clock >= F_CPU / 153) div = 153;
			else if (clock >= F_CPU / 154) div = 154;
			else if (clock >= F_CPU / 155) div = 155;
			else if (clock >= F_CPU / 156) div = 156;
			else if (clock >= F_CPU / 157) div = 157;
			else if (clock >= F_CPU / 158) div = 158;
			else if (clock >= F_CPU / 159) div = 159;
			else if (clock >= F_CPU / 160) div = 160;
			else if (clock >= F_CPU / 161) div = 161;
			else if (clock >= F_CPU / 162) div = 162;
			else if (clock >= F_CPU / 163) div = 163;
			else if (clock >= F_CPU / 164) div = 164;
			else if (clock >= F_CPU / 165) div = 165;
			else if (clock >= F_CPU / 166) div = 166;
			else if (clock >= F_CPU / 167) div = 167;
			else if (clock >= F_CPU / 168) div = 168;
			else if (clock >= F_CPU / 169) div = 169;
			else if (clock >= F_CPU / 170) div = 170;
			else if (clock >= F_CPU / 171) div = 171;
			else if (clock >= F_CPU / 172) div = 172;
			else if (clock >= F_CPU / 173) div = 173;
			else if (clock >= F_CPU / 174) div = 174;
			else if (clock >= F_CPU / 175) div = 175;
			else if (clock >= F_CPU / 176) div = 176;
			else if (clock >= F_CPU / 177) div = 177;
			else if (clock >= F_CPU / 178) div = 178;
			else if (clock >= F_CPU / 179) div = 179;
			else if (clock >= F_CPU / 180) div = 180;
			else if (clock >= F_CPU / 181) div = 181;
			else if (clock >= F_CPU / 182) div = 182;
			else if (clock >= F_CPU / 183) div = 183;
			else if (clock >= F_CPU / 184) div = 184;
			else if (clock >= F_CPU / 185) div = 185;
			else if (clock >= F_CPU / 186) div = 186;
			else if (clock >= F_CPU / 187) div = 187;
			else if (clock >= F_CPU / 188) div = 188;
			else if (clock >= F_CPU / 189) div = 189;
			else if (clock >= F_CPU / 190) div = 190;
			else if (clock >= F_CPU / 191) div = 191;
			else if (clock >= F_CPU / 192) div = 192;
			else if (clock >= F_CPU / 193) div = 193;
			else if (clock >= F_CPU / 194) div = 194;
			else if (clock >= F_CPU / 195) div = 195;
			else if (clock >= F_CPU / 196) div = 196;
			else if (clock >= F_CPU / 197) div = 197;
			else if (clock >= F_CPU / 198) div = 198;
			else if (clock >= F_CPU / 199) div = 199;
			else if (clock >= F_CPU / 200) div = 200;
			else if (clock >= F_CPU / 201) div = 201;
			else if (clock >= F_CPU / 202) div = 202;
			else if (clock >= F_CPU / 203) div = 203;
			else if (clock >= F_CPU / 204) div = 204;
			else if (clock >= F_CPU / 205) div = 205;
			else if (clock >= F_CPU / 206) div = 206;
			else if (clock >= F_CPU / 207) div = 207;
			else if (clock >= F_CPU / 208) div = 208;
			else if (clock >= F_CPU / 209) div = 209;
			else if (clock >= F_CPU / 210) div = 210;
			else if (clock >= F_CPU / 211) div = 211;
			else if (clock >= F_CPU / 212) div = 212;
			else if (clock >= F_CPU / 213) div = 213;
			else if (clock >= F_CPU / 214) div = 214;
			else if (clock >= F_CPU / 215) div = 215;
			else if (clock >= F_CPU / 216) div = 216;
			else if (clock >= F_CPU / 217) div = 217;
			else if (clock >= F_CPU / 218) div = 218;
			else if (clock >= F_CPU / 219) div = 219;
			else if (clock >= F_CPU / 220) div = 220;
			else if (clock >= F_CPU / 221) div = 221;
			else if (clock >= F_CPU / 222) div = 222;
			else if (clock >= F_CPU / 223) div = 223;
			else if (clock >= F_CPU / 224) div = 224;
			else if (clock >= F_CPU / 225) div = 225;
			else if (clock >= F_CPU / 226) div = 226;
			else if (clock >= F_CPU / 227) div = 227;
			else if (clock >= F_CPU / 228) div = 228;
			else if (clock >= F_CPU / 229) div = 229;
			else if (clock >= F_CPU / 230) div = 230;
			else if (clock >= F_CPU / 231) div = 231;
			else if (clock >= F_CPU / 232) div = 232;
			else if (clock >= F_CPU / 233) div = 233;
			else if (clock >= F_CPU / 234) div = 234;
			else if (clock >= F_CPU / 235) div = 235;
			else if (clock >= F_CPU / 236) div = 236;
			else if (clock >= F_CPU / 237) div = 237;
			else if (clock >= F_CPU / 238) div = 238;
			else if (clock >= F_CPU / 239) div = 239;
			else if (clock >= F_CPU / 240) div = 240;
			else if (clock >= F_CPU / 241) div = 241;
			else if (clock >= F_CPU / 242) div = 242;
			else if (clock >= F_CPU / 243) div = 243;
			else if (clock >= F_CPU / 244) div = 244;
			else if (clock >= F_CPU / 245) div = 245;
			else if (clock >= F_CPU / 246) div = 246;
			else if (clock >= F_CPU / 247) div = 247;
			else if (clock >= F_CPU / 248) div = 248;
			else if (clock >= F_CPU / 249) div = 249;
			else if (clock >= F_CPU / 250) div = 250;
			else if (clock >= F_CPU / 251) div = 251;
			else if (clock >= F_CPU / 252) div = 252;
			else if (clock >= F_CPU / 253) div = 253;
			else if (clock >= F_CPU / 254) div = 254;
			else /*  clock >= F_CPU / 255 */ div = 255;
			/*
			#! /usr/bin/perl
			for ($i=2; $i<256; $i++) {
			  printf "\t\t\telse if (clock >= F_CPU / %3d) div = %3d;\n", $i, $i;
			}
			*/
		} else {
			for (div=2; div<255; div++) {
				if (clock >= F_CPU / div) break;
			}
		}
		config = (dataMode & 3) | SPI_CSR_CSAAT | SPI_CSR_SCBR(div) | SPI_CSR_DLYBCT(1);
		//clockdiv = div;
		//datamode = dataMode;
	}
	uint32_t config;
	//uint8_t clockdiv, datamode;
	BitOrder border;
	friend class SPIClass;
};



class SPIClass {
  public:
	SPIClass(Spi *_spi, uint32_t _id, void(*_initCb)(void));

	byte transfer(uint8_t _data, SPITransferMode _mode = SPI_LAST) { return transfer(BOARD_SPI_DEFAULT_SS, _data, _mode); }
	byte transfer(byte _channel, uint8_t _data, SPITransferMode _mode = SPI_LAST);

	// Transaction Functions
	void usingInterrupt(uint8_t interruptNumber);
	void beginTransaction(SPISettings settings) { beginTransaction(BOARD_SPI_DEFAULT_SS, settings); }
	void beginTransaction(uint8_t pin, SPISettings settings);
	void endTransaction(void);

	// SPI Configuration methods
	void attachInterrupt(void);
	void detachInterrupt(void);

	void begin(void);
	void end(void);

	// Attach/Detach pin to/from SPI controller
	void begin(uint8_t _pin);
	void end(uint8_t _pin);

	// These methods sets a parameter on a single pin
	void setBitOrder(uint8_t _pin, BitOrder);
	void setDataMode(uint8_t _pin, uint8_t);
	void setClockDivider(uint8_t _pin, uint8_t);

	// These methods sets the same parameters but on default pin BOARD_SPI_DEFAULT_SS
	void setBitOrder(BitOrder _order) { setBitOrder(BOARD_SPI_DEFAULT_SS, _order); };
	void setDataMode(uint8_t _mode) { setDataMode(BOARD_SPI_DEFAULT_SS, _mode); };
	void setClockDivider(uint8_t _div) { setClockDivider(BOARD_SPI_DEFAULT_SS, _div); };

  private:
	void init();

	Spi *spi;
	uint32_t id;
	BitOrder bitOrder[SPI_CHANNELS_NUM];
	uint32_t divider[SPI_CHANNELS_NUM];
	uint32_t mode[SPI_CHANNELS_NUM];
	void (*initCb)(void);
	bool initialized;
	uint8_t interruptMode;    // 0=none, 1-15=mask, 16=global
	uint8_t interruptSave;    // temp storage, to restore state
	uint32_t interruptMask[4];
};

#if SPI_INTERFACES_COUNT > 0
extern SPIClass SPI;
#endif

// For compatibility with sketches designed for AVR @ 16 MHz
// New programs should use SPI.beginTransaction to set the SPI clock
#define SPI_CLOCK_DIV2	 11
#define SPI_CLOCK_DIV4	 21
#define SPI_CLOCK_DIV8	 42
#define SPI_CLOCK_DIV16	 84
#define SPI_CLOCK_DIV32	 168
#define SPI_CLOCK_DIV64	 255
#define SPI_CLOCK_DIV128 255

#endif
