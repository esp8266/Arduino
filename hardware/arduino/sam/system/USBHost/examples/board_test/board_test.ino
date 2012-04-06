/* USB Host Shield Board test routine. Runs after assembly to check board functionality */

/* USB related */
//#include <Spi.h>
#include <Max3421e.h>
#include <Max3421e_constants.h>
#include <Usb.h>

#include "board_test.h" /* Board test messages */

//#define MAX_SS 10

void setup();
void loop();

MAX3421E Max;
USB Usb;

void setup()
{
  Serial.begin( 115200 );
  //Serial.println("Start");
  //Serial.println( SCK_PIN, DEC );
  Max.powerOn();
  printProgStr( startBanner );
  printProgStr( anykey_msg );
  //Serial.print( Max.getvar(), DEC);
}

void loop()
{
  while( Serial.available() == 0 );  //wait for input
  Serial.read();                     //empty input buffer
  /* start tests */
  /* SPI short test */
  if (!revregcheck()) test_halted();
  /* GPIO test */
  if (!gpiocheck()) printProgStr(PSTR("\r\nGPIO check failed. Make sure GPIO loopback adapter is installed"));
  /* SPI long test */
  if (!spitest()) test_halted();      //test SPI for transmission errors
  if (!osctest()) printProgStr(PSTR("OSCOK test failed. Check the oscillator"));
  if (!usbtest()) printProgStr(PSTR("USB connection test failed. Check traces from USB connector to MAX3421E, as well as VBUS"));  //never gets here
    /* All tests passed */
  printProgStr( anykey_msg );
}

/* SPI short test. Checks connectivity to MAX3421E by reading REVISION register. */
/* Die rev.1 returns 0x01, rev.2 0x12, rev.3 0x13. Any other value is considered communication error */
bool revregcheck()
{
  byte tmpbyte;
  printProgStr(PSTR("\r\nReading REVISION register...Die revision "));
  tmpbyte = Max.regRd( rREVISION );
  switch( tmpbyte ) {
    case( 0x01 ):  //rev.01
      printProgStr(PSTR("01"));
      break;  
    case( 0x12 ):  //rev.02
      printProgStr(PSTR("02"));
      break;
    case( 0x13 ):  //rev.03
      printProgStr(PSTR("03"));
      break;
    default:
      printProgStr(PSTR("invalid. Value returned: "));
      print_hex( tmpbyte, 8 );
      printProgStr( testfailed_msg );
      return( false );
      break;
  }//switch( tmpbyte )...
  printProgStr( testpassed_msg );
  return( true );
}
/* SPI long test */
bool spitest()
{
  byte l = 0;
  byte k = 0;
  byte gpinpol_copy = Max.regRd( rGPINPOL );
  printProgStr(PSTR("\r\nSPI test. Each  '.' indicates 64K transferred. Stops after transferring 1MB (16 dots)\r\n"));
  /**/
  for( byte j = 0; j < 16; j++ ) {
    for( word i = 0; i < 65535; i++ ) {
      Max.regWr( rGPINPOL, k );
      l = Max.regRd( rGPINPOL);
      if( l != k ) {
        printProgStr( spitest_fail_msg );
        print_hex( k, 8);
        printProgStr(PSTR("Value read: "));
        print_hex( l, 8 );
        return( false );                  //test failed
      }
      k++;
    }//for( i = 0; i < 65535; i++
    Serial.print(".");
  }//for j = 0; j < 16...
  Max.regWr( rGPINPOL, gpinpol_copy );
  printProgStr(testpassed_msg);
  return( true );
}
/* Oscillator test */
bool osctest()
{
  printProgStr(PSTR("\r\nOscillator start/stop test."));
  printProgStr( osctest_oscstate_msg );
  check_OSCOKIRQ();                          //print OSCOK state
  printProgStr(PSTR("\r\nSetting CHIP RESET."));
  Max.regWr( rUSBCTL, bmCHIPRES );              //Chip reset. This stops the oscillator
  printProgStr( osctest_oscstate_msg );
  check_OSCOKIRQ();  //print OSCOK state
  printProgStr(PSTR("\r\nClearing CHIP RESET. "));
  Max.regWr( rUSBCTL, 0x00 );                //Chip reset release
  for( word i = 0; i < 65535; i++) {
    if( Max.regRd( rUSBIRQ ) & bmOSCOKIRQ ) {
      printProgStr(PSTR("PLL is stable. Time to stabilize - "));
      Serial.print( i, DEC );
      printProgStr(PSTR(" cycles"));
      printProgStr( testpassed_msg );
      return( true );
    }
  }//for i = 
  return(false);
}
/* Stop/start oscillator */
void check_OSCOKIRQ()
{
  if( Max.regRd( rUSBIRQ ) & bmOSCOKIRQ ) {  //checking oscillator state
    printProgStr(PSTR("ON"));
  }
  else {
    printProgStr(PSTR("OFF"));
  }
}
/* Test USB connectivity */
bool usbtest()
{
  byte rcode;
  byte usbstate;
    Max.powerOn();
    delay( 200 );
    printProgStr(PSTR("\r\nUSB Connectivity test. Waiting for device connection... "));
    while( 1 ) {
      delay( 200 );
      Max.Task();
      Usb.Task();
      usbstate = Usb.getUsbTaskState();
      switch( usbstate ) {
        case( USB_ATTACHED_SUBSTATE_RESET_DEVICE ):
          printProgStr(PSTR("\r\nDevice connected. Resetting"));
          break;
        case( USB_ATTACHED_SUBSTATE_WAIT_SOF ): 
          printProgStr(PSTR("\r\nReset complete. Waiting for the first SOF..."));
          //delay( 1000 );
          break;  
        case( USB_ATTACHED_SUBSTATE_GET_DEVICE_DESCRIPTOR_SIZE ):
          printProgStr(PSTR("\r\nSOF generation started. Enumerating device."));
          break;
        case( USB_STATE_ADDRESSING ):
          printProgStr(PSTR("\r\nSetting device address"));
          //delay( 100 );
          break;
        case( USB_STATE_CONFIGURING ):
          //delay( 1000 );
          printProgStr(PSTR("\r\nGetting device descriptor"));
          rcode = getdevdescr( 1 );
            if( rcode ) {
              printProgStr(PSTR("\r\nError reading device descriptor. Error code "));
              print_hex( rcode, 8 );
            }
            else {
              printProgStr(PSTR("\r\n\nAll tests passed. Press RESET to restart test")); 
              while(1);
            }
          break;
        case( USB_STATE_ERROR ):
          printProgStr(PSTR("\r\nUSB state machine reached error state"));
          break;
        default:
          break;
    }//switch
  }//while(1)
}
/* Get device descriptor */
byte getdevdescr( byte addr )
{
  USB_DEVICE_DESCRIPTOR buf;
  byte rcode;
  rcode = Usb.getDevDescr( addr, 0, 0x12, ( char *)&buf );
  if( rcode ) {
    return( rcode );
  }
  printProgStr(PSTR("\r\nDevice descriptor: "));
  printProgStr(PSTR("\r\nDescriptor Length:\t"));
  print_hex( buf.bLength, 8 );
  printProgStr(PSTR("\r\nDescriptor type:\t"));
  print_hex( buf.bDescriptorType, 8 );
  printProgStr(PSTR("\r\nUSB version:\t"));
  print_hex( buf.bcdUSB, 16 );
  printProgStr(PSTR("\r\nDevice class:\t"));
  print_hex( buf.bDeviceClass, 8 );
  printProgStr(PSTR("\r\nDevice Subclass:\t"));
  print_hex( buf.bDeviceSubClass, 8 );
  printProgStr(PSTR("\r\nDevice Protocol:\t"));
  print_hex( buf.bDeviceProtocol, 8 );
  printProgStr(PSTR("\r\nMax.packet size:\t"));
  print_hex( buf.bMaxPacketSize0, 8 );
  printProgStr(PSTR("\r\nVendor ID:\t"));
  print_hex( buf.idVendor, 16 );
  printProgStr(PSTR("\r\nProduct ID:\t"));
  print_hex( buf.idProduct, 16 );
  printProgStr(PSTR("\r\nRevision ID:\t"));
  print_hex( buf.bcdDevice, 16 );
  printProgStr(PSTR("\r\nMfg.string index:\t"));
  print_hex( buf.iManufacturer, 8 );
  printProgStr(PSTR("\r\nProd.string index:\t"));
  print_hex( buf.iProduct, 8 );
  printProgStr(PSTR("\r\nSerial number index:\t"));
  print_hex( buf.iSerialNumber, 8 );
  printProgStr(PSTR("\r\nNumber of conf.:\t"));
  print_hex( buf.bNumConfigurations, 8 );
  return( 0 );
}

/* GPIO lines check. A loopback adapter connecting GPIN to GPOUT is assumed */
bool gpiocheck()
{
 byte tmpbyte = 0;
  printProgStr(PSTR("\r\nChecking GPIO lines. Install GPIO loopback adapter and press any key to continue..."));
  while( Serial.available() == 0 );  //wait for input
  Serial.read();                     //empty input buffer  
    for( byte i = 0; i < 255; i++ ) {
      Max.gpioWr( i );
      tmpbyte = Max.gpioRd();
      if( tmpbyte != i ) {
        printProgStr(PSTR("GPIO read/write mismatch. Write: "));
        Serial.print(i, HEX);
        printProgStr(PSTR(" Read: "));
        Serial.println( tmpbyte, HEX );
        return( false );
      }//if( tmpbyte != i )
    }//for( i= 0...
    printProgStr( testpassed_msg );
    return( true );
}
/* Test halted state. Generates 0x55 to aid in SPI troubleshooting */    
void test_halted()
{
  printProgStr( test_halted_msg );
  printProgStr(PSTR("\r\nPress RESET to restart test"));
  while( 1 )  {            //System Stop. Generating pattern to keep SCLK, MISO, MOSI, SS busy
    digitalWrite(MAX_SS,LOW);
    Max.regWr( 0x55, 0x55 );
//    Spi.transfer( 0x55 ); 
    digitalWrite(MAX_SS,HIGH);
  }
}
/* given a PROGMEM string, use Serial.print() to send it out */
/* Some non-intuitive casting necessary:                           */
/* printProgStr(PSTR("Func.Mode:\t0x"));                           */
/* printProgStr((char*)pgm_read_word(&mtpopNames[(op & 0xFF)]));   */
void printProgStr(const char* str )
{
  if(!str) { 
    return;
  }
  char c;
  while((c = pgm_read_byte(str++))) {
    Serial.print(c,BYTE);
  }
}
/* prints hex numbers with leading zeroes */
// copyright, Peter H Anderson, Baltimore, MD, Nov, '07
// source: http://www.phanderson.com/arduino/arduino_display.html
void print_hex(int v, int num_places)
{
  int mask=0, n, num_nibbles, digit;

  for (n=1; n<=num_places; n++)
  {
    mask = (mask << 1) | 0x0001;
  }
  v = v & mask; // truncate v to specified number of places

  num_nibbles = num_places / 4;
  if ((num_places % 4) != 0)
  {
    ++num_nibbles;
  }

  do
  {
    digit = ((v >> (num_nibbles-1) * 4)) & 0x0f;
    Serial.print(digit, HEX);
  } 
  while(--num_nibbles);
}
