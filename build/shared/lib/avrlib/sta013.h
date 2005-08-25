/*! \file sta013.h \brief STMicroelectronics STA013 MP3 player driver. */
//*****************************************************************************
//
// File Name	: 'sta013.h'
// Title		: STMicroelectronics STA013 MP3 player driver
// Author		: Pascal Stang
// Created		: 10/22/2000
// Revised		: 12/04/2000
// Version		: 0.3
// Target MCU	: ATmega103 (should work for Atmel AVR Series)
// Editor Tabs	: 4
//
// NOTE: This code is currently below version 1.0, and therefore is considered
// to be lacking in some functionality or documentation, or may not be fully
// tested.  Nonetheless, you can expect most functions to work.
//
// This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//
//*****************************************************************************


#ifndef STA013_H
#define STA013_H

#include "global.h"

// include project-dependent configuration
#include "sta013conf.h"

// STA013 I2C address
#define STA_I2C_DEV						0x86
#define STA_IDENT						0xAC

// STA013 register (sub)address
#define STA_REG_VERSION                0x00
#define STA_REG_IDENT                  0x01
#define STA_REG_PLLCTL_1               0x05
#define STA_REG_PLLCTL_2               0x06
#define STA_REG_PLLCTL_3               0x07
#define STA_REG_REQ_POL                0x0c
#define STA_REG_SCLK_POL               0x0d
#define STA_REG_ERROR_CODE             0x0f
#define STA_REG_SOFT_RESET             0x10
#define STA_REG_PLAY                   0x13
#define STA_REG_MUTE                   0x14
#define STA_REG_CMD_INTERRUPT          0x16
#define STA_REG_DATA_REQ_ENABLE        0x18
#define STA_REG_SYNCSTATUS             0x40
#define STA_REG_ANCCOUNT_L             0x41
#define STA_REG_ANCCOUNT_H             0x42
#define STA_REG_HEAD_H                 0x43
#define STA_REG_HEAD_M                 0x44
#define STA_REG_HEAD_L                 0x45
#define STA_REG_DLA                    0x46
#define STA_REG_DLB                    0x47
#define STA_REG_DRA                    0x48
#define STA_REG_DRB                    0x49
#define STA_REG_MFSDF_441              0x50
#define STA_REG_PLLFRAC_441_L          0x51
#define STA_REG_PLLFRAC_441_H          0x52
#define STA_REG_PCMDIVIDER             0x54
#define STA_REG_PCMCONF                0x55
#define STA_REG_PCMCROSS               0x56
#define STA_REG_ANC_DATA_1             0x59
#define STA_REG_ANC_DATA_2             0x5a
#define STA_REG_ANC_DATA_3             0x5b
#define STA_REG_ANC_DATA_4             0x5c
#define STA_REG_ANC_DATA_5             0x5d
#define STA_REG_MFSDF                  0x61
#define STA_REG_DAC_CLK_MODE           0x63
#define STA_REG_PLLFRAC_L              0x64
#define STA_REG_PLLFRAC_H              0x65
#define STA_REG_FRAME_CNT_L            0x67
#define STA_REG_FRAME_CNT_M            0x68
#define STA_REG_FRAME_CNT_H            0x69
#define STA_REG_AVERAGE_BITRATE        0x6a
#define STA_REG_SOFTVERSION            0x71
#define STA_REG_RUN                    0x72
#define STA_REG_TREBLE_FREQUENCY_LOW   0x77
#define STA_REG_TREBLE_FREQUENCY_HIGH  0x78
#define STA_REG_BASS_FREQUENCY_LOW     0x79
#define STA_REG_BASS_FREQUENCY_HIGH    0x7a
#define STA_REG_TREBLE_ENHANCE         0x7b
#define STA_REG_BASS_ENHANCE           0x7c
#define STA_REG_TONE_ATTEN             0x7d

#define MIN_VOLUME_ATTENUATION			0
#define MAX_VOLUME_ATTENUATION			96
#define MIN_TONE_ATTENUATION			0
#define MAX_TONE_ATTENUATION			96
#define MIN_BASS_FREQUENCY				100
#define MAX_BASS_FREQUENCY				500
#define MIN_BASS_ENHANCE				-12	// -18dB in 1.5 dB steps
#define MAX_BASS_ENHANCE				+12	// +18dB in 1.5 dB steps
#define MIN_TREBLE_FREQUENCY			1000
#define MAX_TREBLE_FREQUENCY			5000
#define MIN_TREBLE_ENHANCE				-12	// -18dB in 1.5 dB steps
#define MAX_TREBLE_ENHANCE				+12	// +18dB in 1.5 dB steps
#define SOFTMUTE_VOLUME_CHANGE			20

// global variables
//u16 Sta013UpdateIndex;

// prototypes
void sta013HWReset(void);
u08  sta013ReadReg(u08 reg);
void sta013WriteReg(u08 reg, u08 data);
void sta013DownloadUpdate(void);
u08  sta013Init(void);

void sta013StartDecoder(void);
void sta013StopDecoder(void);
void sta013PauseDecoder(void);
void sta013ResumeDecoder(void);

void sta013GetMP3Info(u16 *bitrate, u08 *sampFreq, u08 *mode);
u16 sta013GetAverageBitrate(void);

void sta013SetVolume(u08 volume, s08 balance);
void sta013SetTone(s08 bassEnh, u16 bassFreq, s08 trebleEnh, u16 trebleFreq);


u08  sta013Demand(void);

#endif
