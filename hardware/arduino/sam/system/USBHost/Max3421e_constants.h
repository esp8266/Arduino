/* Copyright 2009-2011 Oleg Mazurov, Circuits At Home, http://www.circuitsathome.com */
/* MAX3421E register/bit names and bitmasks */

#ifndef _MAX3421Econstants_h_
#define _MAX3421Econstants_h_

/* SPI pins for diffrent Arduinos */

#define MEGA256_IS_ADK // Undefine this if you're using a non-ADK Mega256

// TODO: Check if the 2560 check should use `defined` too.
#if defined(__AVR_ATmega1280__) || (__AVR_ATmega2560__)
  #define SCK_PIN   52
  #define MISO_PIN  50
  #define MOSI_PIN  51
#if defined(__AVR_ATmega2560__) && defined(MEGA256_IS_ADK)
  #define TARGET_MEGA_ADK
  #define SS_PIN    53 // TODO: Handle this as an internal pin.
#else
  // TODO: Test with Mega + shield combination
  #define SS_PIN    53
#endif
#elif defined(__AVR_ATmega168__) || defined(__AVR_ATmega328P__)
  #define SCK_PIN   13
  #define MISO_PIN  12
  #define MOSI_PIN  11
  #define SS_PIN    10
#else
  #error The currently selected board needs to have its SPI pin definitions added to this file.
#endif

#if !defined(TARGET_MEGA_ADK)
  #define MAX_SS    SS_PIN
  #define MAX_INT   9
  #define MAX_GPX   8
  #define MAX_RESET 7
#else
  // Because the Arduino Mega ADK board uses "internal" pins (i.e. ones that
  // are not broken out to headers, are not in `pins_arduino.h` and can't
  // be used with functions like `pinMode()` & `digitalWrite()`) they need
  // to be defined by the low-level AVR approach.
  // These definitions and the code to use them comes from an older version
  // of the `Max3421e.cpp` file.
  // TODO: Add these internal pins to `pins_arduino.h` for the Mega ADK so
  //       we can usual the standard Arduino functions & not special-case this?
  #define INT             PE6
  #define INT_PORT        PORTE
  #define INT_DDR         DDRE
  #define INT_PIN         PINE

  #define RST             PJ2
  #define RST_PORT        PORTJ
  #define RST_DDR         DDRJ
  #define RST_PIN         PINJ

  #define GPX             PJ3
  #define GPX_PORT        PORTJ
  #define GPX_DDR         DDRJ
  #define GPX_PIN         PINJ

  #define MAX_SS          SS_PIN // TODO: Handle this as an internal pin.
#endif

/* "Breakpoint" pins for debugging */
//#define BPNT_0      3
//#define BPNT_1      2
     
//#define Select_MAX3421E     digitalWrite(MAX_SS,LOW)
//#define Deselect_MAX3421E   digitalWrite(MAX_SS,HIGH)

/* */

#define ON  true
#define OFF false

/* VBUS states */
#define SE0     0
#define SE1     1
#define FSHOST  2
#define LSHOST  3

/* MAX3421E command byte format: rrrrr0wa where 'r' is register number  */
//
// MAX3421E Registers in HOST mode. 
//
#define rRCVFIFO    0x08    //1<<3
#define rSNDFIFO    0x10    //2<<3
#define rSUDFIFO    0x20    //4<<3
#define rRCVBC      0x30    //6<<3
#define rSNDBC      0x38    //7<<3

#define rUSBIRQ     0x68    //13<<3
/* USBIRQ Bits  */
#define bmVBUSIRQ   0x40    //b6
#define bmNOVBUSIRQ 0x20    //b5
#define bmOSCOKIRQ  0x01    //b0

#define rUSBIEN     0x70    //14<<3
/* USBIEN Bits  */
#define bmVBUSIE    0x40    //b6
#define bmNOVBUSIE  0x20    //b5
#define bmOSCOKIE   0x01    //b0

#define rUSBCTL     0x78    //15<<3
/* USBCTL Bits  */
#define bmCHIPRES   0x20    //b5
#define bmPWRDOWN   0x10    //b4

#define rCPUCTL     0x80    //16<<3
/* CPUCTL Bits  */
#define bmPUSLEWID1 0x80    //b7
#define bmPULSEWID0 0x40    //b6
#define bmIE        0x01    //b0

#define rPINCTL     0x88    //17<<3
/* PINCTL Bits  */
#define bmFDUPSPI   0x10    //b4
#define bmINTLEVEL  0x08    //b3
#define bmPOSINT    0x04    //b2
#define bmGPXB      0x02    //b1
#define bmGPXA      0x01    //b0
// GPX pin selections
#define GPX_OPERATE 0x00
#define GPX_VBDET   0x01
#define GPX_BUSACT  0x02
#define GPX_SOF     0x03

#define rREVISION   0x90    //18<<3

#define rIOPINS1    0xa0    //20<<3

/* IOPINS1 Bits */
#define bmGPOUT0    0x01
#define bmGPOUT1    0x02
#define bmGPOUT2    0x04
#define bmGPOUT3    0x08
#define bmGPIN0     0x10
#define bmGPIN1     0x20
#define bmGPIN2     0x40
#define bmGPIN3     0x80

#define rIOPINS2    0xa8    //21<<3
/* IOPINS2 Bits */
#define bmGPOUT4    0x01
#define bmGPOUT5    0x02
#define bmGPOUT6    0x04
#define bmGPOUT7    0x08
#define bmGPIN4     0x10
#define bmGPIN5     0x20
#define bmGPIN6     0x40
#define bmGPIN7     0x80

#define rGPINIRQ    0xb0    //22<<3
/* GPINIRQ Bits */
#define bmGPINIRQ0 0x01
#define bmGPINIRQ1 0x02
#define bmGPINIRQ2 0x04
#define bmGPINIRQ3 0x08
#define bmGPINIRQ4 0x10
#define bmGPINIRQ5 0x20
#define bmGPINIRQ6 0x40
#define bmGPINIRQ7 0x80

#define rGPINIEN    0xb8    //23<<3
/* GPINIEN Bits */
#define bmGPINIEN0 0x01
#define bmGPINIEN1 0x02
#define bmGPINIEN2 0x04
#define bmGPINIEN3 0x08
#define bmGPINIEN4 0x10
#define bmGPINIEN5 0x20
#define bmGPINIEN6 0x40
#define bmGPINIEN7 0x80

#define rGPINPOL    0xc0    //24<<3
/* GPINPOL Bits */
#define bmGPINPOL0 0x01
#define bmGPINPOL1 0x02
#define bmGPINPOL2 0x04
#define bmGPINPOL3 0x08
#define bmGPINPOL4 0x10
#define bmGPINPOL5 0x20
#define bmGPINPOL6 0x40
#define bmGPINPOL7 0x80

#define rHIRQ       0xc8    //25<<3
/* HIRQ Bits */
#define bmBUSEVENTIRQ   0x01   // indicates BUS Reset Done or BUS Resume     
#define bmRWUIRQ        0x02
#define bmRCVDAVIRQ     0x04
#define bmSNDBAVIRQ     0x08
#define bmSUSDNIRQ      0x10
#define bmCONDETIRQ     0x20
#define bmFRAMEIRQ      0x40
#define bmHXFRDNIRQ     0x80

#define rHIEN       0xd0    //26<<3
/* HIEN Bits */
#define bmBUSEVENTIE    0x01
#define bmRWUIE         0x02
#define bmRCVDAVIE      0x04
#define bmSNDBAVIE      0x08
#define bmSUSDNIE       0x10
#define bmCONDETIE      0x20
#define bmFRAMEIE       0x40
#define bmHXFRDNIE      0x80

#define rMODE       0xd8    //27<<3
/* MODE Bits */
#define bmHOST          0x01
#define bmLOWSPEED      0x02
#define bmHUBPRE        0x04
#define bmSOFKAENAB     0x08
#define bmSEPIRQ        0x10
#define bmDELAYISO      0x20
#define bmDMPULLDN      0x40
#define bmDPPULLDN      0x80

#define rPERADDR    0xe0    //28<<3

#define rHCTL       0xe8    //29<<3
/* HCTL Bits */
#define bmBUSRST        0x01
#define bmFRMRST        0x02
#define bmSAMPLEBUS     0x04
#define bmSIGRSM        0x08
#define bmRCVTOG0       0x10
#define bmRCVTOG1       0x20
#define bmSNDTOG0       0x40
#define bmSNDTOG1       0x80

#define rHXFR       0xf0    //30<<3
/* Host transfer token values for writing the HXFR register (R30)   */
/* OR this bit field with the endpoint number in bits 3:0               */
#define tokSETUP  0x10  // HS=0, ISO=0, OUTNIN=0, SETUP=1
#define tokIN     0x00  // HS=0, ISO=0, OUTNIN=0, SETUP=0
#define tokOUT    0x20  // HS=0, ISO=0, OUTNIN=1, SETUP=0
#define tokINHS   0x80  // HS=1, ISO=0, OUTNIN=0, SETUP=0
#define tokOUTHS  0xA0  // HS=1, ISO=0, OUTNIN=1, SETUP=0 
#define tokISOIN  0x40  // HS=0, ISO=1, OUTNIN=0, SETUP=0
#define tokISOOUT 0x60  // HS=0, ISO=1, OUTNIN=1, SETUP=0

#define rHRSL       0xf8    //31<<3
/* HRSL Bits */
#define bmRCVTOGRD  0x10
#define bmSNDTOGRD  0x20
#define bmKSTATUS   0x40
#define bmJSTATUS   0x80
#define bmSE0       0x00    //SE0 - disconnect state
#define bmSE1       0xc0    //SE1 - illegal state       
/* Host error result codes, the 4 LSB's in the HRSL register */
#define hrSUCCESS   0x00
#define hrBUSY      0x01
#define hrBADREQ    0x02
#define hrUNDEF     0x03
#define hrNAK       0x04
#define hrSTALL     0x05
#define hrTOGERR    0x06
#define hrWRONGPID  0x07
#define hrBADBC     0x08
#define hrPIDERR    0x09
#define hrPKTERR    0x0A
#define hrCRCERR    0x0B
#define hrKERR      0x0C
#define hrJERR      0x0D
#define hrTIMEOUT   0x0E
#define hrBABBLE    0x0F

#define MODE_FS_HOST    (bmDPPULLDN|bmDMPULLDN|bmHOST|bmSOFKAENAB)
#define MODE_LS_HOST    (bmDPPULLDN|bmDMPULLDN|bmHOST|bmLOWSPEED|bmSOFKAENAB)


#endif //_MAX3421Econstants_h_
