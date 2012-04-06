/* MAX3421E USB Host controller get configuration descriptor */
#include <Spi.h>
#include <Max3421e.h>
#include <Usb.h>
 
#define LOBYTE(x) ((char*)(&(x)))[0]
#define HIBYTE(x) ((char*)(&(x)))[1]
#define BUFSIZE 256    //buffer size
 
void setup();
void loop();
 
MAX3421E Max;
USB Usb;
 
void setup()
{
  byte tmpdata = 0;
  Serial.begin( 9600 );
  Serial.println("Start");
  Max.powerOn();
  delay( 200 );
}
 
void loop()
{
  byte rcode;
  byte tmpbyte;
  Max.Task();
  Usb.Task();
  if( Usb.getUsbTaskState() >= USB_STATE_CONFIGURING ) {  //state configuring or higher
    /* entering configuration number to decode. Restricted to first 10 configurations */
    Serial.print("Enter configuration number: ");
    while( Serial.available() == 0 );      //wait for input
    tmpbyte = Serial.read();
    Serial.println( tmpbyte );
    tmpbyte -= 0x30;                       //convert to number
    if( tmpbyte > 9 ) {
      Serial.println("Not a number. Assuming configuration 0");
      tmpbyte = 0;
    }
    rcode = getconfdescr( 1, tmpbyte );                 //get configuration descriptor
    if( rcode ) {
      Serial.println( rcode, HEX );
    }
//    while( 1 );                          //stop
  }    
}
 
byte getconfdescr( byte addr, byte conf )
{
  char buf[ BUFSIZE ];
  char* buf_ptr = buf;
  byte rcode;
  byte descr_length;
  byte descr_type;
  unsigned int total_length;
  rcode = Usb.getConfDescr( addr, 0, 4, conf, buf );  //get total length
  LOBYTE( total_length ) = buf[ 2 ];
  HIBYTE( total_length ) = buf[ 3 ];
  if( total_length > BUFSIZE ) {    //check if total length is larger than buffer
    Serial.println("Total length truncated to 256 bytes");
    total_length = BUFSIZE;
  }
  rcode = Usb.getConfDescr( addr, 0, total_length, conf, buf ); //get the whole descriptor
  while( buf_ptr < buf + total_length ) {  //parsing descriptors
    descr_length = *( buf_ptr );
    descr_type = *( buf_ptr + 1 );
    switch( descr_type ) {
      case( USB_DESCRIPTOR_CONFIGURATION ):
        printconfdescr( buf_ptr );
        break;
      case( USB_DESCRIPTOR_INTERFACE ):
        printintfdescr( buf_ptr );
        break;
      case( USB_DESCRIPTOR_ENDPOINT ):
        printepdescr( buf_ptr );
        break;
      default:
        printunkdescr( buf_ptr );
        break;
        }//switch( descr_type
    Serial.println("");    
    buf_ptr = ( buf_ptr + descr_length );    //advance buffer pointer
  }//while( buf_ptr <=...
  return( 0 );
}
/* prints hex numbers with leading zeroes */
// copyright, Peter H Anderson, Baltimore, MD, Nov, '07
// source: http://www.phanderson.com/arduino/arduino_display.html
void print_hex(int v, int num_places)
{
  int mask=0, n, num_nibbles, digit;
 
  for (n=1; n<=num_places; n++) {
    mask = (mask << 1) | 0x0001;
  }
  v = v & mask; // truncate v to specified number of places
 
  num_nibbles = num_places / 4;
  if ((num_places % 4) != 0) {
    ++num_nibbles;
  }
  do {
    digit = ((v >> (num_nibbles-1) * 4)) & 0x0f;
    Serial.print(digit, HEX);
  } 
  while(--num_nibbles);
}
/* function to print configuration descriptor */
void printconfdescr( char* descr_ptr )
{
 USB_CONFIGURATION_DESCRIPTOR* conf_ptr = ( USB_CONFIGURATION_DESCRIPTOR* )descr_ptr;
  Serial.println("Configuration descriptor:");
  Serial.print("Total length:\t");
  print_hex( conf_ptr->wTotalLength, 16 );
  Serial.print("\r\nNum.intf:\t\t");
  print_hex( conf_ptr->bNumInterfaces, 8 );
  Serial.print("\r\nConf.value:\t");
  print_hex( conf_ptr->bConfigurationValue, 8 );
  Serial.print("\r\nConf.string:\t");
  print_hex( conf_ptr->iConfiguration, 8 );
  Serial.print("\r\nAttr.:\t\t");
  print_hex( conf_ptr->bmAttributes, 8 );
  Serial.print("\r\nMax.pwr:\t\t");
  print_hex( conf_ptr->bMaxPower, 8 );
  return;
}
/* function to print interface descriptor */
void printintfdescr( char* descr_ptr )
{
 USB_INTERFACE_DESCRIPTOR* intf_ptr = ( USB_INTERFACE_DESCRIPTOR* )descr_ptr;
  Serial.println("\r\nInterface descriptor:");
  Serial.print("Intf.number:\t");
  print_hex( intf_ptr->bInterfaceNumber, 8 );
  Serial.print("\r\nAlt.:\t\t");
  print_hex( intf_ptr->bAlternateSetting, 8 );
  Serial.print("\r\nEndpoints:\t\t");
  print_hex( intf_ptr->bNumEndpoints, 8 );
  Serial.print("\r\nClass:\t\t");
  print_hex( intf_ptr->bInterfaceClass, 8 );
  Serial.print("\r\nSubclass:\t\t");
  print_hex( intf_ptr->bInterfaceSubClass, 8 );
  Serial.print("\r\nProtocol:\t\t");
  print_hex( intf_ptr->bInterfaceProtocol, 8 );
  Serial.print("\r\nIntf.string:\t");
  print_hex( intf_ptr->iInterface, 8 );
  return;
}
/* function to print endpoint descriptor */
void printepdescr( char* descr_ptr )
{
 USB_ENDPOINT_DESCRIPTOR* ep_ptr = ( USB_ENDPOINT_DESCRIPTOR* )descr_ptr;
  Serial.println("\r\nEndpoint descriptor:");
  Serial.print("Endpoint address:\t");
  print_hex( ep_ptr->bEndpointAddress, 8 );
  Serial.print("\r\nAttr.:\t\t");
  print_hex( ep_ptr->bmAttributes, 8 );
  Serial.print("\r\nMax.pkt size:\t");
  print_hex( ep_ptr->wMaxPacketSize, 16 );
  Serial.print("\r\nPolling interval:\t");
  print_hex( ep_ptr->bInterval, 8 );
  return;
}
/*function to print unknown descriptor */
void printunkdescr( char* descr_ptr )
{
  byte length = *descr_ptr;
  byte i;
  Serial.println("\r\nUnknown descriptor:");
  Serial. print("Length:\t\t");
  print_hex( *descr_ptr, 8 );
  Serial.print("\r\nType:\t\t");
  print_hex( *(descr_ptr + 1 ), 8 );
  Serial.print("\r\nContents:\t");
  descr_ptr += 2;
  for( i = 0; i < length; i++ ) {
    print_hex( *descr_ptr, 8 );
    descr_ptr++;
  }
}
