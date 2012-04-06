/* Copyright 2009-2011 Oleg Mazurov, Circuits At Home, http://www.circuitsathome.com */
/* MAX3421E functions */
#ifndef _MAX3421E_H_
#define _MAX3421E_H_


//#include <Spi.h>
//#include <WProgram.h>
#include "Arduino.h"
#include "Max3421e_constants.h"

class MAX3421E /* : public SPI */ {
    // byte vbusState;
    public:
        MAX3421E( void );
        byte getVbusState( void );
//        void toggle( byte pin );
        static void regWr( byte, byte );
        char * bytesWr( byte, byte, char * );
        static void gpioWr( byte );
        byte regRd( byte );
        char * bytesRd( byte, byte, char * );
        byte gpioRd( void );
        boolean reset();
        boolean vbusPwr ( boolean );
        void busprobe( void );
        void powerOn();
        byte IntHandler();
        byte GpxHandler();
        byte Task();
    private:
      static void spi_init() {
        uint8_t tmp;
        // initialize SPI pins
        pinMode(SCK_PIN, OUTPUT);
        pinMode(MOSI_PIN, OUTPUT);
        pinMode(MISO_PIN, INPUT);
        pinMode(SS_PIN, OUTPUT);
        digitalWrite( SS_PIN, HIGH ); 
        /* mode 00 (CPOL=0, CPHA=0) master, fclk/2. Mode 11 (CPOL=11, CPHA=11) is also supported by MAX3421E */
        SPCR = 0x50;
        SPSR = 0x01;
        /**/
        tmp = SPSR;
        tmp = SPDR;
    }
//        void init();
    friend class Max_LCD;        
};




#endif //_MAX3421E_H_
