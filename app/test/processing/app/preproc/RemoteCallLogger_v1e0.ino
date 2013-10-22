
#include <SoftwareSerial.h> // required to send and receive AT commands from the GPRS Shield
#include <Wire.h> // required for I2C communication with the RTC

// pin numbers for RTC
#define DS3231_I2C_ADDRESS 104 // 0x68 // Address for RTC
#define DS3231_TIME_CAL_ADDR        0 // 0x00
#define DS3231_ALARM1_ADDR          7 // 0x07
#define DS3231_ALARM2_ADDR         11 // 0x0B
#define DS3231_CONTROL_ADDR        14 // 0x0E
#define DS3231_STATUS_ADDR         15 // 0x0F
//#define DS3231_AGING_OFFSET_ADDR   16 // 0x10
#define DS3231_TEMPERATURE_ADDR    17 // 0x11

// Declarations for GPRS shield
SoftwareSerial GPRS( 7, 8 ); // A softwareSerial line is defined for the GPRS Shield
byte buffer[ 64 ]; // Buffer is used to transfer data from the GPRS line to the serial line
int count = 0, e = 0, count2 = 0, t = 0, q;
char temp, lastCaller[13] = "blank";
boolean callIncoming = false, done;

// Declarations for RTC
byte time[ 7 ]; // second, minute, hour, dow, day, month, year
byte time_A1[ 5 ]; // second_A1, minute_A1, hour_A1, day_A1, DY/DT
byte time_A2[ 4 ]; // minute_A2, hour_A2, day_A2, DY/DT
byte received[1]; // used to catch bytes sent from the clock
float temperature; // clock temperature is updated every 64 s

// Declarations for RemoteCallLogger
char telescopeNames[6][4];

/*
Code Exclusively for GPRS shield:
*/

//
//  Default set of instructions for GPRS Shield power control
//

void setPowerStateTo( int newState )
{
  if( newState != 1 && newState != 0 ) { // tests for an invalid state. In this case no change is made to powerstate
    Serial.print( "Error: Invalid powerstate. Current powerstate = " );
    Serial.print( getPowerState() );
    Serial.print( "\n" );
  }
  else  {
    if( newState == getPowerState() )  { // if the requested powerstate is already in effect, no action is taken
      Serial.print( "Powerstate = " );
      Serial.print( newState );
      Serial.print( " remains unchanged.\n" );
    }
    else  {
      powerUpOrDown();  // This is the only case where the powerstate is changed
      Serial.print( "Powerstate changed from " );
      Serial.print( 1 - newState );
      Serial.print( " to " );
      Serial.print( newState );
      Serial.print( "\n" );
    }
  }
  delay( 5000 ); // for startup
}

int getPowerState() // returns 0 if GPRS Shield is off, and 1 if GPRS Shield is on. This corresponds to the constant HIGH LOW enumeration
{
  int ret;
  if ( digitalRead(18) == 0 && digitalRead(19) == 0 ) // tests pins 18 and 19 for activity. See ExFoundImportantPins sketch to find out why
    ret = 1;
  else
    ret = 0;

  return ret;
}

void powerUpOrDown() // toggle the power of the shield
{
  pinMode( 9, OUTPUT );  
  digitalWrite( 9, LOW ); 
  delay( 1000 );
  digitalWrite( 9, HIGH );
  delay( 2000 );
  digitalWrite( 9, LOW );
  delay( 3000 );
}

//
//  End of default power control
//

void clearBufferArray() // gives each element in the buffer array a null value
{
  for( int i = 0; i < count; i++ )
    buffer[ i ] = NULL;
}

void makeMissedCall( char num[] )
{
  int i;
  char in[ 18 ] = "ATD";
  for( i = 3; i <= 14; i++ ) // AT command string containing telephone number is prepared
    in[ i ] = num[ i - 3] ;
  in[ 15 ] = ';';
  in[ 16 ] = '\r';
  in[ 17 ] = '\0';
  GPRS.write( in ); // AT command requesting call is sent
  delay( 10000 ); // enough time is given for GSM connection, and at least one ring.
  GPRS.write( "ATH\r\0" ); // AT command requesting hangup is sent
  delay( 1000 );
}

void sendTextMessage( char number[], char messg[] )
{
  char temp[ 27 ] = "AT + CMGS = \"";
  for( q = 0; q < 12; q++ )      // for-loop is used to prepare the AT command string containing the telephone number
      temp[ q + 13 ] = number[ q ];
  temp[ 25 ] = '\"';
  temp[ 26 ] = '\0';

  GPRS.println( "AT+CMGF=1\r" );    // AT command requesting SMS in text mode is sent
  delay( 1000 );
  GPRS.println( temp );            // AT command containing telephone number is sent
  delay( 1000 );
  GPRS.println( messg );          //the content of the message
  delay( 1000 );
  GPRS.println( (char) 26 );      //the ASCII code of the ctrl+z is 26. This character indicates the end of the message.
  delay( 1000 );
}

void analise(byte incoming[], int length) // this function receives and analyses all text sent from the GPRS Shield to the serial line. It stores the cell number of the last caller.
{
  e = 0;      // Counter that represents a letter in the buffer
  done = false; // Boolean that prevents unneccessary loop revolutions
  while( e < length && !done){ // while e does not surpass the last letter index of the buffer...
    temp = char( incoming[e] ); // store the character at index e in a temporary char
    switch( temp ){             // inspect temp
    case 'R': 
      {
        if( length > e + 3 && !callIncoming ) { // This case responds to "RING"
          if(char( incoming[e + 1] ) == 'I' 
            && char( incoming[e + 2] ) == 'N'
            && char( incoming[e + 3] ) == 'G'){
            GPRS.write("AT+CLCC\r");            // call information is requested
            delay(500);      // time is given for processing
            GPRS.write("ATH\r"); // GPRS shield hangs up
            callIncoming = true; // this ensures that a number cannot be stored in any other case than a missed call
            done = true;         // prevents the further operation of this while loop
          }
        }
      }
      break;
    case '+': 
      {
        if(char( buffer[ e + 1]) == '2' && length > e + 11 && callIncoming){  // this case responds to "+2", but only if the buffer contains enough characters for a valid cell number
          for(t = 0; t < 12; t++)                                              // and only if the callIncoming boolean had been triggered by a previous instance of this function
            lastCaller[t] = char( buffer[ e + t ]);    // the number of this caller is stored in lastCaller
          lastCaller[12] = '\0';
          callIncoming = false; // now we are ready for the next call
          done = true;  // prevents the further operation of this while loop
        }
      }
      break;
    case 'l': 
      Serial.println(lastCaller); // an easy way to test this function. Simply type "l" to see the value of lastCaller (default "blank")
      break;
    }
    e++;  // buffer index is incremented
  }
}

/*
End of GPRS Shield code
*/


/*
Code exclusively for RTC
*/

byte decToBcd( byte b ) // converts a byte from a decimal format to a binary-coded decimal
{
  return ( b / 10 << 4 ) + b % 10;
}

boolean getBit( byte addr, int pos ) // returns a single bit from a determined location in the RTC register
{
  byte temp = getByte( addr );
  return boolean( (temp >> pos) & B00000001 );
}

void setBit( byte addr, int pos, boolean newBit ) // ensures that a single bit from a determined location in the RTC register is a determined value
{
  boolean oldBit = getBit( addr, pos ); // bits' current state is retrieved
  byte temp = received[ 0 ]; // complete byte is retrieved. it is still left in received from the previous command
  if ( oldBit != newBit ) // change is only made if the bit isnt already the correct value
  {
    if( newBit ) // if newBit is 1, then old bit must be 0, thus we must add an amount
        temp += (B00000001 << pos); // 2 to the power of the bit position is added to the byte
    else
      temp -= (B00000001 << pos); // 2 to the power of the bit position is subtracted from the byte
  }
  setByte( addr, temp ); // the register is updated with the new byte
}

byte getByte( byte addr ) // returns a single byte from the given address in the RTC register
{
  byte temp;
  if( getBytes( addr, 1) ) // If one byte was read from the address:
    temp = received[ 0 ]; // get that byte
  else temp = -1; // -1 is returned as an error
  return temp;
}

boolean getBytes( byte addr, int amount ) // updates the byte array "received" with the given amount of bytes, read from the given address
{ // ^ returns false if reading failed
  boolean wireWorked = false;
  Wire.beginTransmission( DS3231_I2C_ADDRESS ); // We transmit to the RTC
  Wire.write( addr ); // We want to read from the given address
  Wire.endTransmission(); // We want to receive, so we stop transmitting
  Wire.requestFrom( DS3231_I2C_ADDRESS, amount ); // we request the given amount of bytes from the RTC
  if( Wire.available() ){
    received[amount]; // prepare the array for the amount of incoming bytes
    for( int i = 0; i < amount; i++){
      received[ i ] = Wire.read(); // we read the given amount of bytes
    }
    wireWorked = true; // everything went as planned
  }
  return wireWorked;
}

void setByte( byte addr, byte newByte ) // writes a given byte to a given address in the RTCs register. convenient
{
  setBytes( addr, &newByte, 1); // call the setBytes function with the default amount = 1
}

void setBytes( byte addr, byte newBytes[], int amount ) // writes a given amount of bytes in a sequence starting from a given address
{
  Wire.beginTransmission( DS3231_I2C_ADDRESS ); // We transmit to the RTC
  Wire.write( addr ); // We want to start writing from the given address
  for( int i = 0; i < amount; i++ )
    Wire.write( newBytes[ i ] ); // we write each byte in sequence
  Wire.endTransmission(); // we're done here
}

void getTime() // reads the current time from the register and updates the byte array containing the current time
{
  if( getBytes( DS3231_TIME_CAL_ADDR, 7) ) // if 7 bytes were read in from the time address:
  {
    for(int i = 0; i < 7; i++) // place each byte in it's place
      time[ i ] = received[ i ];
    // The following conversions convert the values from binary-coded decimal format to regular binary:
    time[ 0 ] = ( ( time[ 0 ] & B01110000 ) >> 4 ) * 10 + ( time[ 0 ] & B00001111 ); // second
    time[ 1 ] = ( ( time[ 1 ] & B01110000 ) >> 4 ) * 10 + ( time[ 1 ] & B00001111 ); // minute
    time[ 2 ] = ( ( time[ 2 ] & B00110000 ) >> 4 ) * 10 + ( time[ 2 ] & B00001111 ); // hour
    time[ 4 ] = ( ( time[ 4 ] & B00110000 ) >> 4 ) * 10 + ( time[ 4 ] & B00001111 ); // day of month
    time[ 5 ] = ( ( time[ 5 ] & B00010000 ) >> 4 ) * 10 + ( time[ 5 ] & B00001111 ); // month
    time[ 6 ] = ( ( time[ 6 ] & B11110000 ) >> 4 ) * 10 + ( time[ 6 ] & B00001111 ); // year 
  }
}

void setTime( byte newTime[ 7 ] ) // sets the time in the RTC register to the given values
{
  for(int i = 0; i < 7; i++)
    newTime[i] = decToBcd(newTime[i]); // the time consists of 7 bytes, each which must be converted to binary-coded decimal
  setBytes( DS3231_TIME_CAL_ADDR, newTime, 7 ); // bytes are sent to be written
}

void getRTCTemperature() // reads the temperature from the register and updates the global temperature float
{
  //temp registers (11h-12h) get updated automatically every 64s
  if( getBytes( DS3231_TEMPERATURE_ADDR, 2 ) ) // if 2 bytes were read from the temperature addresss 
  {
    temperature = ( received[ 0 ] & B01111111 ); // assign the integer part of the integer
    temperature += ( ( received[ 1 ] >> 6 ) * 0.25 ); // assign the fractional part of the temperature
  }
}

void gprsListen()
{
  if( GPRS.available() ) { // If the GPRS Shield is transmitting data to the Stalker...
    while( GPRS.available() )  {  // While there is still data left...
      buffer[ count++ ] = GPRS.read(); // get the next byte of data
      if ( count == 64 )               // we only handle a maximum of 64 bytes of data at a time
        break;
    }
    Serial.write( buffer, count ); // Send the data to the serial line
    analise( buffer, count );
    clearBufferArray(); // clear the buffer
    count = 0; // reset counter
  }
  if (Serial.available()) // if the Stalker is transmitting data....
    GPRS.write(Serial.read()); // send the data to the GPRS Shield.  
}

void printTime() // updates time, and prints it in a convenient format 
{
  getTime();
  Serial.print( int( time[ 3 ] ) ); // dow
  Serial.print( ' ' );
  Serial.print( int( time[ 2 ] ) ); // hour
  Serial.print( ':' );
  Serial.print( int( time[ 1 ] ) ); // minute
  Serial.print( ':' );
  Serial.print( int( time[ 0 ] ) ); // second
  Serial.print( ' ' );
  Serial.print( int( time[ 4 ] ) ); // day
  Serial.print( '/' );
  Serial.print( int( time[ 5 ] ) ); // month
  Serial.print( "/20" );
  Serial.print( int( time[ 6 ]  ) ); // year
  Serial.println();
}

/*
End of RTC code
*/

void setup()
{
  // GPRS Shield startup code
  GPRS.begin( 9600 );
  delay(1000);
  setPowerStateTo(1);
  delay(1000);

  // RTC Startup code
  Wire.begin();
  delay(1000);
  
  Serial.begin(9600);
  delay(1000);

}

void loop()
{
  gprsListen(); // GPRS Shield listener. Todo: replace w interrupt
  getTime();    // Updates the time. Todo: replace w interrupt
}




