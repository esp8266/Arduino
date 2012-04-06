//**** ATMEL AVR - A P P L I C A T I O N   N O T E  ************************
//*
//* Title:		AVR068 - STK500 Communication Protocol
//* Filename:		command.h
//* Version:		1.0
//* Last updated:	31.01.2005
//*
//* Support E-mail:	avr@atmel.com
//*
//**************************************************************************

// *****************[ STK message constants ]***************************

#define MESSAGE_START                       0x1B        //= ESC = 27 decimal
#define TOKEN                               0x0E

// *****************[ STK general command constants ]**************************

#define CMD_SIGN_ON                         0x01
#define CMD_SET_PARAMETER                   0x02
#define CMD_GET_PARAMETER                   0x03
#define CMD_SET_DEVICE_PARAMETERS           0x04
#define CMD_OSCCAL                          0x05
#define CMD_LOAD_ADDRESS                    0x06
#define CMD_FIRMWARE_UPGRADE                0x07


// *****************[ STK ISP command constants ]******************************

#define CMD_ENTER_PROGMODE_ISP              0x10
#define CMD_LEAVE_PROGMODE_ISP              0x11
#define CMD_CHIP_ERASE_ISP                  0x12
#define CMD_PROGRAM_FLASH_ISP               0x13
#define CMD_READ_FLASH_ISP                  0x14
#define CMD_PROGRAM_EEPROM_ISP              0x15
#define CMD_READ_EEPROM_ISP                 0x16
#define CMD_PROGRAM_FUSE_ISP                0x17
#define CMD_READ_FUSE_ISP                   0x18
#define CMD_PROGRAM_LOCK_ISP                0x19
#define CMD_READ_LOCK_ISP                   0x1A
#define CMD_READ_SIGNATURE_ISP              0x1B
#define CMD_READ_OSCCAL_ISP                 0x1C
#define CMD_SPI_MULTI                       0x1D

// *****************[ STK PP command constants ]*******************************

#define CMD_ENTER_PROGMODE_PP               0x20
#define CMD_LEAVE_PROGMODE_PP               0x21
#define CMD_CHIP_ERASE_PP                   0x22
#define CMD_PROGRAM_FLASH_PP                0x23
#define CMD_READ_FLASH_PP                   0x24
#define CMD_PROGRAM_EEPROM_PP               0x25
#define CMD_READ_EEPROM_PP                  0x26
#define CMD_PROGRAM_FUSE_PP                 0x27
#define CMD_READ_FUSE_PP                    0x28
#define CMD_PROGRAM_LOCK_PP                 0x29
#define CMD_READ_LOCK_PP                    0x2A
#define CMD_READ_SIGNATURE_PP               0x2B
#define CMD_READ_OSCCAL_PP                  0x2C    

#define CMD_SET_CONTROL_STACK               0x2D

// *****************[ STK HVSP command constants ]*****************************

#define CMD_ENTER_PROGMODE_HVSP             0x30
#define CMD_LEAVE_PROGMODE_HVSP             0x31
#define CMD_CHIP_ERASE_HVSP                 0x32
#define CMD_PROGRAM_FLASH_HVSP  `     0x33
#define CMD_READ_FLASH_HVSP                 0x34
#define CMD_PROGRAM_EEPROM_HVSP             0x35
#define CMD_READ_EEPROM_HVSP                0x36
#define CMD_PROGRAM_FUSE_HVSP               0x37
#define CMD_READ_FUSE_HVSP                  0x38
#define CMD_PROGRAM_LOCK_HVSP               0x39
#define CMD_READ_LOCK_HVSP                  0x3A
#define CMD_READ_SIGNATURE_HVSP             0x3B
#define CMD_READ_OSCCAL_HVSP                0x3C

// *****************[ STK status constants ]***************************

// Success
#define STATUS_CMD_OK                       0x00

// Warnings
#define STATUS_CMD_TOUT                     0x80
#define STATUS_RDY_BSY_TOUT                 0x81
#define STATUS_SET_PARAM_MISSING            0x82

// Errors
#define STATUS_CMD_FAILED                   0xC0
#define STATUS_CKSUM_ERROR                  0xC1
#define STATUS_CMD_UNKNOWN                  0xC9

// *****************[ STK parameter constants ]***************************
#define PARAM_BUILD_NUMBER_LOW              0x80
#define PARAM_BUILD_NUMBER_HIGH             0x81
#define PARAM_HW_VER                        0x90
#define PARAM_SW_MAJOR                      0x91
#define PARAM_SW_MINOR                      0x92
#define PARAM_VTARGET                       0x94
#define PARAM_VADJUST                       0x95
#define PARAM_OSC_PSCALE                    0x96
#define PARAM_OSC_CMATCH                    0x97
#define PARAM_SCK_DURATION                  0x98
#define PARAM_TOPCARD_DETECT                0x9A
#define PARAM_STATUS                        0x9C
#define PARAM_DATA                          0x9D
#define PARAM_RESET_POLARITY                0x9E
#define PARAM_CONTROLLER_INIT               0x9F

// *****************[ STK answer constants ]***************************

#define ANSWER_CKSUM_ERROR                  0xB0

