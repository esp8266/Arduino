/* Copyright 2009-2011 Oleg Mazurov, Circuits At Home, http://www.circuitsathome.com */
/* MAX3421E USB host controller support */

#include "Max3421e.h"
// #include "Max3421e_constants.h"

static byte vbusState;

/* Functions    */

/* Constructor */
MAX3421E::MAX3421E()
{
    spi_init();  
#if !defined(TARGET_MEGA_ADK)
    pinMode( MAX_INT, INPUT);
    pinMode( MAX_GPX, INPUT );
#else
    // "Internal" pins so can't use Arduino helper functions.
    INT_DDR &= ~_BV(INT);
    GPX_DDR &= ~_BV(GPX);
#endif
    pinMode( MAX_SS, OUTPUT );
    digitalWrite(MAX_SS,HIGH);   
#if !defined(TARGET_MEGA_ADK)
    pinMode( MAX_RESET, OUTPUT );
    digitalWrite( MAX_RESET, HIGH );  //release MAX3421E from reset
#else
    // "Internal" pins so can't use Arduino helper functions.
    RST_DDR |= _BV(RST);
    RST_PORT |= _BV(RST); // Equivalent to setRST(HIGH) from old code.
#endif
}

byte MAX3421E::getVbusState( void )
{ 
    return( vbusState );
}
/* initialization */
//void MAX3421E::init()
//{
//    /* setup pins */
//    pinMode( MAX_INT, INPUT);
//    pinMode( MAX_GPX, INPUT );
//    pinMode( MAX_SS, OUTPUT );
//    //pinMode( BPNT_0, OUTPUT );
//    //pinMode( BPNT_1, OUTPUT );
//    //digitalWrite( BPNT_0, LOW );
//    //digitalWrite( BPNT_1, LOW );
//    Deselect_MAX3421E;              
//    pinMode( MAX_RESET, OUTPUT );
//    digitalWrite( MAX_RESET, HIGH );  //release MAX3421E from reset
//}
//byte MAX3421E::getVbusState( void )
//{
//    return( vbusState );
//}
//void MAX3421E::toggle( byte pin )
//{
//    digitalWrite( pin, HIGH );
//    digitalWrite( pin, LOW );
//}
/* Single host register write   */
void MAX3421E::regWr( byte reg, byte val)
{
      digitalWrite(MAX_SS,LOW);
      SPDR = ( reg | 0x02 );
      while(!( SPSR & ( 1 << SPIF )));
      SPDR = val;
      while(!( SPSR & ( 1 << SPIF )));
      digitalWrite(MAX_SS,HIGH);
      return;
}
/* multiple-byte write */
/* returns a pointer to a memory position after last written */
char * MAX3421E::bytesWr( byte reg, byte nbytes, char * data )
{
    digitalWrite(MAX_SS,LOW);
    SPDR = ( reg | 0x02 );
    while( nbytes-- ) {
      while(!( SPSR & ( 1 << SPIF )));  //check if previous byte was sent
      SPDR = ( *data );               // send next data byte
      data++;                         // advance data pointer
    }
    while(!( SPSR & ( 1 << SPIF )));
    digitalWrite(MAX_SS,HIGH);
    return( data );
}
/* GPIO write. GPIO byte is split between 2 registers, so two writes are needed to write one byte */
/* GPOUT bits are in the low nibble. 0-3 in IOPINS1, 4-7 in IOPINS2 */
/* upper 4 bits of IOPINS1, IOPINS2 are read-only, so no masking is necessary */
void MAX3421E::gpioWr( byte val )
{
    regWr( rIOPINS1, val );
    val = val >>4;
    regWr( rIOPINS2, val );
    
    return;     
}
/* Single host register read        */
byte MAX3421E::regRd( byte reg )    
{
  byte tmp;
    digitalWrite(MAX_SS,LOW);
    SPDR = reg;
    while(!( SPSR & ( 1 << SPIF )));
    SPDR = 0; //send empty byte
    while(!( SPSR & ( 1 << SPIF )));
    digitalWrite(MAX_SS,HIGH); 
    return( SPDR );
}
/* multiple-bytes register read                             */
/* returns a pointer to a memory position after last read   */
char * MAX3421E::bytesRd ( byte reg, byte nbytes, char  * data )
{
    digitalWrite(MAX_SS,LOW);
    SPDR = reg;      
    while(!( SPSR & ( 1 << SPIF )));    //wait
    while( nbytes ) {
      SPDR = 0; //send empty byte
      nbytes--;
      while(!( SPSR & ( 1 << SPIF )));
      *data = SPDR;
      data++;
    }
    digitalWrite(MAX_SS,HIGH);
    return( data );   
}
/* GPIO read. See gpioWr for explanation */
/* GPIN pins are in high nibbles of IOPINS1, IOPINS2    */
byte MAX3421E::gpioRd( void )
{
 byte tmpbyte = 0;
    tmpbyte = regRd( rIOPINS2 );            //pins 4-7
    tmpbyte &= 0xf0;                        //clean lower nibble
    tmpbyte |= ( regRd( rIOPINS1 ) >>4 ) ;  //shift low bits and OR with upper from previous operation. Upper nibble zeroes during shift, at least with this compiler
    return( tmpbyte );
}
/* reset MAX3421E using chip reset bit. SPI configuration is not affected   */
boolean MAX3421E::reset()
{
  unsigned short tmp = 0;
    regWr( rUSBCTL, bmCHIPRES );                        //Chip reset. This stops the oscillator
    regWr( rUSBCTL, 0x00 );                             //Remove the reset
    while(!(regRd( rUSBIRQ ) & bmOSCOKIRQ )) {          //wait until the PLL is stable
        tmp++;                                          //timeout after 256 attempts
        if( tmp == 0 ) {
            return( false );
        }
    }
    return( true );
}
/* turn USB power on/off                                                */
/* does nothing, returns TRUE. Left for compatibility with old sketches               */
/* will be deleted eventually                                           */
///* ON pin of VBUS switch (MAX4793 or similar) is connected to GPOUT7    */
///* OVERLOAD pin of Vbus switch is connected to GPIN7                    */
///* OVERLOAD state low. NO OVERLOAD or VBUS OFF state high.              */
boolean MAX3421E::vbusPwr ( boolean action )
{
//  byte tmp;
//    tmp = regRd( rIOPINS2 );                //copy of IOPINS2
//    if( action ) {                          //turn on by setting GPOUT7
//        tmp |= bmGPOUT7;
//    }
//    else {                                  //turn off by clearing GPOUT7
//        tmp &= ~bmGPOUT7;
//    }
//    regWr( rIOPINS2, tmp );                 //send GPOUT7
//    if( action ) {
//        delay( 60 );
//    }
//    if (( regRd( rIOPINS2 ) & bmGPIN7 ) == 0 ) {     // check if overload is present. MAX4793 /FLAG ( pin 4 ) goes low if overload
//        return( false );
//    }                      
    return( true );                                             // power on/off successful                       
}
/* probe bus to determine device presense and speed and switch host to this speed */
void MAX3421E::busprobe( void )
{
 byte bus_sample;
    bus_sample = regRd( rHRSL );            //Get J,K status
    bus_sample &= ( bmJSTATUS|bmKSTATUS );      //zero the rest of the byte
    switch( bus_sample ) {                          //start full-speed or low-speed host 
        case( bmJSTATUS ):
            if(( regRd( rMODE ) & bmLOWSPEED ) == 0 ) {
                regWr( rMODE, MODE_FS_HOST );       //start full-speed host
                vbusState = FSHOST;
            }
            else {
                regWr( rMODE, MODE_LS_HOST);        //start low-speed host
                vbusState = LSHOST;
            }
            break;
        case( bmKSTATUS ):
            if(( regRd( rMODE ) & bmLOWSPEED ) == 0 ) {
                regWr( rMODE, MODE_LS_HOST );       //start low-speed host
                vbusState = LSHOST;
            }
            else {
                regWr( rMODE, MODE_FS_HOST );       //start full-speed host
                vbusState = FSHOST;
            }
            break;
        case( bmSE1 ):              //illegal state
            vbusState = SE1;
            break;
        case( bmSE0 ):              //disconnected state
            regWr( rMODE, bmDPPULLDN|bmDMPULLDN|bmHOST|bmSEPIRQ);
            vbusState = SE0;
            break;
        }//end switch( bus_sample )
}
/* MAX3421E initialization after power-on   */
void MAX3421E::powerOn()
{
    /* Configure full-duplex SPI, interrupt pulse   */
    regWr( rPINCTL,( bmFDUPSPI + bmINTLEVEL + bmGPXB ));    //Full-duplex SPI, level interrupt, GPX
    if( reset() == false ) {                                //stop/start the oscillator
        Serial.println("Error: OSCOKIRQ failed to assert");
    }

    /* configure host operation */
    regWr( rMODE, bmDPPULLDN|bmDMPULLDN|bmHOST|bmSEPIRQ );      // set pull-downs, Host, Separate GPIN IRQ on GPX
    regWr( rHIEN, bmCONDETIE|bmFRAMEIE );                                             //connection detection
    /* check if device is connected */
    regWr( rHCTL,bmSAMPLEBUS );                                             // sample USB bus
    while(!(regRd( rHCTL ) & bmSAMPLEBUS ));                                //wait for sample operation to finish
    busprobe();                                                             //check if anything is connected
    regWr( rHIRQ, bmCONDETIRQ );                                            //clear connection detect interrupt                 
    regWr( rCPUCTL, 0x01 );                                                 //enable interrupt pin
}
/* MAX3421 state change task and interrupt handler */
byte MAX3421E::Task( void )
{
 byte rcode = 0;
 byte pinvalue;
    //Serial.print("Vbus state: ");
    //Serial.println( vbusState, HEX );

#if !defined(TARGET_MEGA_ADK)
    pinvalue = digitalRead( MAX_INT );    
#else
    // "Internal" pin so can't use Arduino helper functions.
    pinvalue = INT_PIN & _BV(INT) ? HIGH : LOW; // from old `readINT()`
#endif
    if( pinvalue  == LOW ) {
        rcode = IntHandler();
    }

#if !defined(TARGET_MEGA_ADK)
    pinvalue = digitalRead( MAX_GPX );
#else
    // "Internal" pin so can't use Arduino helper functions.
    pinvalue = GPX_PIN & _BV(GPX) ? HIGH : LOW; // from old `readGPX()`
#endif
    if( pinvalue == LOW ) {
        GpxHandler();
    }
//    usbSM();                                //USB state machine                            
    return( rcode );   
}   
byte MAX3421E::IntHandler()
{
 byte HIRQ;
 byte HIRQ_sendback = 0x00;
    HIRQ = regRd( rHIRQ );                  //determine interrupt source
    //if( HIRQ & bmFRAMEIRQ ) {               //->1ms SOF interrupt handler
    //    HIRQ_sendback |= bmFRAMEIRQ;
    //}//end FRAMEIRQ handling
    if( HIRQ & bmCONDETIRQ ) {
        busprobe();
        HIRQ_sendback |= bmCONDETIRQ;
    }
    /* End HIRQ interrupts handling, clear serviced IRQs    */
    regWr( rHIRQ, HIRQ_sendback );
    return( HIRQ_sendback );
}
byte MAX3421E::GpxHandler()
{
 byte GPINIRQ = regRd( rGPINIRQ );          //read GPIN IRQ register
//    if( GPINIRQ & bmGPINIRQ7 ) {            //vbus overload
//        vbusPwr( OFF );                     //attempt powercycle
//        delay( 1000 );
//        vbusPwr( ON );
//        regWr( rGPINIRQ, bmGPINIRQ7 );
//    }       
    return( GPINIRQ );
}

//void MAX3421E::usbSM( void )                //USB state machine
//{
//    
//
//}
