#include <SoftwareSerial.h>                                                                 
#include <Wire.h>                                               

                      
#define DS3231_I2C_ADDRESS 104                           
#define DS3231_TIME_CAL_ADDR        0        
#define DS3231_ALARM1_ADDR          7        
#define DS3231_ALARM2_ADDR         11        
#define DS3231_CONTROL_ADDR        14        
#define DS3231_STATUS_ADDR         15        
                                               
#define DS3231_TEMPERATURE_ADDR    17        

                               
SoftwareSerial GPRS( 7, 8 );                                                        
byte buffer[ 64 ];                                                                         
int count = 0, e = 0, count2 = 0, t = 0, q;
char temp, lastCaller[13] = "blank";
boolean callIncoming = false, done;

                       
byte time[ 7 ];                                               
byte time_A1[ 5 ];                                                
byte time_A2[ 4 ];                                     
byte received[1];                                           
float temperature;                                           

                                    
char telescopeNames[6][4];

  
                                 
  

  
                                                             
  

void setPowerStateTo( int newState )
{
  if( newState != 1 && newState != 0 ) {                                                                            
    Serial.print( "Error: Invalid powerstate. Current powerstate = " );
    Serial.print( getPowerState() );
    Serial.print( "\n" );
  }
  else  {
    if( newState == getPowerState() )  {                                                                        
      Serial.print( "Powerstate = " );
      Serial.print( newState );
      Serial.print( " remains unchanged.\n" );
    }
    else  {
      powerUpOrDown();                                                          
      Serial.print( "Powerstate changed from " );
      Serial.print( 1 - newState );
      Serial.print( " to " );
      Serial.print( newState );
      Serial.print( "\n" );
    }
  }
  delay( 5000 );               
}

int getPowerState()                                                                                                                      
{
  int ret;
  if ( digitalRead(18) == 0 && digitalRead(19) == 0 )                                                                                      
    ret = 1;
  else
    ret = 0;

  return ret;
}

void powerUpOrDown()                                  
{
  pinMode( 9, OUTPUT );  
  digitalWrite( 9, LOW ); 
  delay( 1000 );
  digitalWrite( 9, HIGH );
  delay( 2000 );
  digitalWrite( 9, LOW );
  delay( 3000 );
}

  
                                
  

void clearBufferArray()                                                       
{
  for( int i = 0; i < count; i++ )
    buffer[ i ] = NULL;
}

void makeMissedCall( char num[] )
{
  int i;
  char in[ 18 ] = "ATD";
  for( i = 3; i <= 14; i++ )                                                             
    in[ i ] = num[ i - 3] ;
  in[ 15 ] = ';';
  in[ 16 ] = '\r';
  in[ 17 ] = '\0';
  GPRS.write( in );                                      
  delay( 10000 );                                                                   
  GPRS.write( "ATH\r\0" );                                        
  delay( 1000 );
}

void sendTextMessage( char number[], char messg[] )
{
  char temp[ 27 ] = "AT + CMGS = \"";
  for( q = 0; q < 12; q++ )                                                                                          
      temp[ q + 13 ] = number[ q ];
  temp[ 25 ] = '\"';
  temp[ 26 ] = '\0';

  GPRS.println( "AT+CMGF=1\r" );                                                     
  delay( 1000 );
  GPRS.println( temp );                                                             
  delay( 1000 );
  GPRS.println( messg );                                      
  delay( 1000 );
  GPRS.println( (char) 26 );                                                                                            
  delay( 1000 );
}

void analise(byte incoming[], int length)                                                                                                                                            
{
  e = 0;                                                       
  done = false;                                                       
  while( e < length && !done){                                                                   
    temp = char( incoming[e] );                                                      
    switch( temp ){                            
    case 'R': 
      {
        if( length > e + 3 && !callIncoming ) {                                
          if(char( incoming[e + 1] ) == 'I' 
            && char( incoming[e + 2] ) == 'N'
            && char( incoming[e + 3] ) == 'G'){
            GPRS.write("AT+CLCC\r");                                            
            delay(500);                                     
            GPRS.write("ATH\r");                        
            callIncoming = true;                                                                                    
            done = true;                                                             
          }
        }
      }
      break;
    case '+': 
      {
        if(char( buffer[ e + 1]) == '2' && length > e + 11 && callIncoming){                                                                                                          
          for(t = 0; t < 12; t++)                                                                                                                                                
            lastCaller[t] = char( buffer[ e + t ]);                                                        
          lastCaller[12] = '\0';
          callIncoming = false;                                      
          done = true;                                                      
        }
      }
      break;
    case 'l': 
      Serial.println(lastCaller);                                                                                                       
      break;
    }
    e++;                                
  }
}

  
                       
  


  
                        
  

byte decToBcd( byte b )                                                                   
{
  return ( b / 10 << 4 ) + b % 10;
}

boolean getBit( byte addr, int pos )                                                                       
{
  byte temp = getByte( addr );
  return boolean( (temp >> pos) & B00000001 );
}

void setBit( byte addr, int pos, boolean newBit )                                                                                                  
{
  boolean oldBit = getBit( addr, pos );                                    
  byte temp = received[ 0 ];                                                                                      
  if ( oldBit != newBit )                                                                 
  {
    if( newBit )                                                                      
        temp += (B00000001 << pos);                                                           
    else
      temp -= (B00000001 << pos);                                                                  
  }
  setByte( addr, temp );                                             
}

byte getByte( byte addr )                                                                    
{
  byte temp;
  if( getBytes( addr, 1) )                                          
    temp = received[ 0 ];                 
  else temp = -1;                              
  return temp;
}

boolean getBytes( byte addr, int amount )                                                                                                 
{                                     
  boolean wireWorked = false;
  Wire.beginTransmission( DS3231_I2C_ADDRESS );                          
  Wire.write( addr );                                          
  Wire.endTransmission();                                               
  Wire.requestFrom( DS3231_I2C_ADDRESS, amount );                                                     
  if( Wire.available() ){
    received[amount];                                                      
    for( int i = 0; i < amount; i++){
      received[ i ] = Wire.read();                                     
    }
    wireWorked = true;                              
  }
  return wireWorked;
}

void setByte( byte addr, byte newByte )                                                                           
{
  setBytes( addr, &newByte, 1);                                                          
}

void setBytes( byte addr, byte newBytes[], int amount )                                                                              
{
  Wire.beginTransmission( DS3231_I2C_ADDRESS );                          
  Wire.write( addr );                                                   
  for( int i = 0; i < amount; i++ )
    Wire.write( newBytes[ i ] );                                  
  Wire.endTransmission();                   
}

void getTime()                                                                                                   
{
  if( getBytes( DS3231_TIME_CAL_ADDR, 7) )                                                  
  {
    for(int i = 0; i < 7; i++)                                 
      time[ i ] = received[ i ];
                                                                                                       
    time[ 0 ] = ( ( time[ 0 ] & B01110000 ) >> 4 ) * 10 + ( time[ 0 ] & B00001111 );          
    time[ 1 ] = ( ( time[ 1 ] & B01110000 ) >> 4 ) * 10 + ( time[ 1 ] & B00001111 );          
    time[ 2 ] = ( ( time[ 2 ] & B00110000 ) >> 4 ) * 10 + ( time[ 2 ] & B00001111 );        
    time[ 4 ] = ( ( time[ 4 ] & B00110000 ) >> 4 ) * 10 + ( time[ 4 ] & B00001111 );                
    time[ 5 ] = ( ( time[ 5 ] & B00010000 ) >> 4 ) * 10 + ( time[ 5 ] & B00001111 );         
    time[ 6 ] = ( ( time[ 6 ] & B11110000 ) >> 4 ) * 10 + ( time[ 6 ] & B00001111 );         
  }
}

void setTime( byte newTime[ 7 ] )                                                         
{
  for(int i = 0; i < 7; i++)
    newTime[i] = decToBcd(newTime[i]);                                                                                      
  setBytes( DS3231_TIME_CAL_ADDR, newTime, 7 );                                
}

void getRTCTemperature()                                                                                    
{
                                                                
  if( getBytes( DS3231_TEMPERATURE_ADDR, 2 ) )                                                       
  {
    temperature = ( received[ 0 ] & B01111111 );                                          
    temperature += ( ( received[ 1 ] >> 6 ) * 0.25 );                                                 
  }
}

void gprsListen()
{
  if( GPRS.available() ) {                                                             
    while( GPRS.available() )  {                                      
      buffer[ count++ ] = GPRS.read();                             
      if ( count == 64 )                                                                        
        break;
    }
    Serial.write( buffer, count );                                    
    analise( buffer, count );
    clearBufferArray();                    
    count = 0;                 
  }
  if (Serial.available())                                           
    GPRS.write(Serial.read());                                       
}

void printTime()                                                       
{
  getTime();
  Serial.print( int( time[ 3 ] ) );       
  Serial.print( ' ' );
  Serial.print( int( time[ 2 ] ) );        
  Serial.print( ':' );
  Serial.print( int( time[ 1 ] ) );          
  Serial.print( ':' );
  Serial.print( int( time[ 0 ] ) );          
  Serial.print( ' ' );
  Serial.print( int( time[ 4 ] ) );       
  Serial.print( '/' );
  Serial.print( int( time[ 5 ] ) );         
  Serial.print( "/20" );
  Serial.print( int( time[ 6 ]  ) );        
  Serial.println();
}

  
               
  

void setup()
{
                             
  GPRS.begin( 9600 );
  delay(1000);
  setPowerStateTo(1);
  delay(1000);

                     
  Wire.begin();
  delay(1000);
  
  Serial.begin(9600);
  delay(1000);

}

void loop()
{
  gprsListen();                                                   
  getTime();                                                  
}

