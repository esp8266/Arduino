// Ladyada's logger modified by Bill Greiman to use the SdFat library 
// this is a generic logger that does checksum testing so the data written should be always good
// Assumes a sirf III chipset logger attached to pin 0 and 1

uint8_t sensorCount = 3; //number of analog pins to log

#include <SdFat.h>
#include <SdFatUtil.h>
#include <avr/pgmspace.h>

#define isdigit(x) ( x >= '0' && x <= '9')

//extern uint16_t _end;

Sd2Card card;
SdVolume volume;
SdFile root;
SdFile f;

#define led1Pin 4                // LED1 connected to digital pin 4
#define led2Pin 3                // LED2 connected to digital pin 3
#define powerpin 2               // GPS power control

// set the  RX_BUFFER_SIZE to 32!
#define BUFFSIZE 73         // we buffer one NMEA sentence at a time, 83 bytes is longer than the max length
char buffer[BUFFSIZE];      // this is the double buffer
char buffer2[12];

uint8_t bufferidx = 0;
uint32_t tmp;
#define LOG_RMC  1      // essential location data
#define RMC_ON   "$PSRF103,4,0,1,1*21\r\n"  // the command we send to turn RMC on (1 hz rate)
#define RMC_OFF  "$PSRF103,4,0,0,1*20\r\n"  // the command we send to turn RMC off

#define LOG_GGA  0      // contains fix, hdop & vdop data
#define GGA_ON   "$PSRF103,0,0,1,1*25\r\n"   // the command we send to turn GGA on (1 hz rate)
#define GGA_OFF  "$PSRF103,0,0,0,1*24\r\n"   // the command we send to turn GGA off

#define LOG_GSA 0      // satellite data
#define GSA_ON   "$PSRF103,2,0,1,1*27\r\n"   // the command we send to turn GSA on (1 hz rate)
#define GSA_OFF  "$PSRF103,2,0,0,1*26\r\n"   // the command we send to turn GSA off

#define LOG_GSV  0      // detailed satellite data
#define GSV_ON   "$PSRF103,3,0,1,1*26\r\n"  // the command we send to turn GSV on (1 hz rate)
#define GSV_OFF  "$PSRF103,3,0,0,1*27\r\n"  // the command we send to turn GSV off

#define LOG_GLL 0      // Loran-compatibility data
// this isnt output by default

#define USE_WAAS   1     // useful in US, but slower fix
#define WAAS_ON    "$PSRF151,1*3F\r\n"       // the command for turning on WAAS
#define WAAS_OFF   "$PSRF151,0*3E\r\n"       // the command for turning off WAAS

#define LOG_RMC_FIXONLY 1  // log only when we get RMC's with fix?
uint8_t fix = 0; // current fix data

// macros to use PSTR
#define putstring(str) SerialPrint_P(PSTR(str))
#define putstring_nl(str) SerialPrintln_P(PSTR(str))

// read a Hex value and return the decimal equivalent
uint8_t parseHex(char c) {
    if (c < '0')
      return 0;
    if (c <= '9')
      return c - '0';
    if (c < 'A')
       return 0;
    if (c <= 'F')
       return (c - 'A')+10;
}

uint8_t i;

// blink out an error code
void error(uint8_t errno) {
  if (card.errorCode()) {
    putstring("SD error: ");
    Serial.print(card.errorCode(), HEX);
    Serial.print(',');
    Serial.println(card.errorData(), HEX);
  }
   while(1) {
     for (i=0; i<errno; i++) {    
       digitalWrite(led1Pin, HIGH);
       digitalWrite(led2Pin, HIGH);
       delay(100);
       digitalWrite(led1Pin, LOW);   
       digitalWrite(led2Pin, LOW);   
       delay(100);
     }
     for (; i<10; i++) {
       delay(200);
     }      
   } 
}

void setup() {                   // run once, when the sketch starts
  Serial.begin(4800);
  putstring_nl("GPSlogger");
  pinMode(led1Pin, OUTPUT);      // sets the digital pin as output
  pinMode(led2Pin, OUTPUT);      // sets the digital pin as output
  pinMode(powerpin, OUTPUT);
  digitalWrite(powerpin, LOW);
  
  // initialize the SD card at SPI_HALF_SPEED to avoid bus errors with
  // breadboards.  use SPI_FULL_SPEED for better performance.
  if (!card.init(SPI_HALF_SPEED)) {
    putstring_nl("Card init. failed!"); 
    error(1);
  }
  if (!volume.init(&card)) {
    putstring_nl("No partition!"); 
    error(2);
  }
  if (!root.openRoot(&volume)) {
        putstring_nl("Can't open root dir"); 
    error(3);
  }
  strcpy(buffer, "GPSLOG00.CSV");
  for (i = 0; i < 100; i++) {
    buffer[6] = '0' + i/10;
    buffer[7] = '0' + i%10;
    if (f.open(&root, buffer, O_CREAT | O_EXCL | O_WRITE)) break;
  }
  
  if(!f.isOpen()) {
    putstring("couldnt create "); Serial.println(buffer);
    error(3);
  }
  putstring("writing to "); Serial.println(buffer);
  putstring_nl("ready!");

  // write header
  if (sensorCount > 6) sensorCount = 6;
  strncpy_P(buffer, PSTR("time,lat,long,speed,date,sens0,sens1,sens2,sens3,sens4,sens5"), 24 + 6*sensorCount);
  Serial.println(buffer);
  // clear print error
  f.writeError = 0;
  f.println(buffer);
  if (f.writeError || !f.sync()) {
    putstring_nl("can't write header!");
    error(5);
  }
  
  delay(1000);

   putstring("\r\n");
#if USE_WAAS == 1 
   putstring(WAAS_ON); // turn on WAAS
#else
  putstring(WAAS_OFF); // turn on WAAS
#endif

#if LOG_RMC == 1
  putstring(RMC_ON); // turn on RMC
#else
  putstring(RMC_OFF); // turn off RMC
#endif

#if LOG_GSV == 1 
  putstring(GSV_ON); // turn on GSV
#else
  putstring(GSV_OFF); // turn off GSV
#endif

#if LOG_GSA == 1
  putstring(GSA_ON); // turn on GSA
#else
  putstring(GSA_OFF); // turn off GSA
#endif

#if LOG_GGA == 1
 putstring(GGA_ON); // turn on GGA
#else
 putstring(GGA_OFF); // turn off GGA
#endif
}

void loop() {                    // run over and over again

  //Serial.println(Serial.available(), DEC);
  char c;
  uint8_t sum;
  
  // read one 'line'
  if (Serial.available()) {
    c = Serial.read();
    //Serial.print(c, BYTE);
    if (bufferidx == 0) {
      while (c != '$')
        c = Serial.read(); // wait till we get a $
    }
    buffer[bufferidx] = c;

    //Serial.print(c, BYTE);
    if (c == '\n') {
      //putstring_nl("EOL");
      //Serial.print(buffer);
      buffer[bufferidx+1] = 0; // terminate it
     
      if (buffer[bufferidx-4] != '*') {
        // no checksum?
        Serial.print('*', BYTE);
        bufferidx = 0;
        return;
      }
      // get checksum
      sum = parseHex(buffer[bufferidx-3]) * 16;
      sum += parseHex(buffer[bufferidx-2]);
      
      // check checksum 
      for (i=1; i < (bufferidx-4); i++) {
        sum ^= buffer[i];
      }
      if (sum != 0) {
        //putstring_nl("Cxsum mismatch");
        Serial.print('~', BYTE);
        bufferidx = 0;
        return;
      }
      // got good data!

      if (strstr(buffer, "GPRMC")) {
        // find out if we got a fix
        char *p = buffer;
        p = strchr(p, ',')+1;
        p = strchr(p, ',')+1;       // skip to 3rd item
        
        if (p[0] == 'V') {
          digitalWrite(led1Pin, LOW);
          fix = 0;
        } else {
          digitalWrite(led1Pin, HIGH);
          fix = 1;
        }
      } else {
        // not GPRMC
        bufferidx = 0;
        return;
      }
#if LOG_RMC_FIXONLY
      if (!fix) {
          Serial.print('_', BYTE);
          bufferidx = 0;
          return;
      } 
#endif
      // rad. lets print it!
      
      Serial.print(buffer);
  
      // time to clean up the string
      // find time
      char *p = buffer;
      p = strchr(p, ',')+1;
      buffer[0] = p[0];
      buffer[1] = p[1];
      buffer[2] = ':';
      buffer[3] = p[2];
      buffer[4] = p[3];
      buffer[5] = ':';
      buffer[6] = p[4];
      buffer[7] = p[5];
      // we ignore milliseconds
      buffer[8] = ',';
      
      p = strchr(buffer+8, ',')+1;
      // skip past 'active' flag
      p = strchr(p, ',')+1;
      // find lat
      p = strchr(p, ',')+1;

      buffer[9] = '+';
      buffer[10] = p[0];
      buffer[11] = p[1];
      buffer[12] = ' ';
      strncpy(buffer+13, p+2, 7);
      buffer[20] = ',';
      
      p = strchr(buffer+21, ',')+1;
      if (p[0] == 'S')
        buffer[9] = '-';
      
      // find long
      p = strchr(p, ',')+1;
      buffer[21] = '+';
      buffer[22] = p[0];
      buffer[23] = p[1];
      buffer[24] = p[2];
      buffer[25] = ' ';
      strncpy(buffer+26, p+3, 7);
      buffer[33] = ',';
      
      p = strchr(buffer+34, ',')+1;
      if (p[0] == 'W')
        buffer[21] = '-';
      
      // find speed
      p = strchr(p, ',')+1;
      tmp = 0;
      if (p[0] != ',') {
        // ok there is some sort of speed
        while (p[0] != '.' && p[0] != ',') {
          tmp *= 10;
          tmp += p[0] - '0';
          p++;       
        }
        tmp *= 10;
        if (isdigit(p[1])) 
          tmp += p[1] - '0'; // tenths
        tmp *= 10;
        if (isdigit(p[2])) 
        tmp += p[2] - '0'; // hundredths
        
        // tmp is knots * 100
        // convert to mph (1.15 mph = 1 knot)
        tmp *= 115;
        // -OR- convert km/h 
        // tmp *= 185
      } 
      tmp /= 100;
      
      buffer[34] = (tmp / 10000) + '0';
      tmp %= 10000;
      buffer[35] = (tmp / 1000) + '0';
      tmp %= 1000;
      buffer[36] = (tmp / 100) + '0';
      tmp %= 100;
      buffer[37] = '.';
      buffer[38] = (tmp / 10) + '0';
      tmp %= 10;
      buffer[39] = tmp + '0';
       
      buffer[40] = ',';
      p = strchr(p, ',')+1;
      // skip past bearing
      p = strchr(p, ',')+1;
      //mod for bug when speed,bearing are missing (bill greiman)
      uint8_t date[6];
      for (uint8_t id = 0; id < 6; id++) date[id] = p[id];
      // get date into 2001-01-31 style 
      buffer[41] = '2';
      buffer[42] = '0';
      buffer[43] = date[4];
      buffer[44] = date[5];
      buffer[45] = '-';
      buffer[46] = date[2];
      buffer[47] = date[3];
      buffer[48] = '-';
      buffer[49] = date[0];
      buffer[50] = date[1];
      buffer[51] = 0;
      digitalWrite(led2Pin, HIGH);  
      if(f.write((uint8_t *) buffer, 51) != 51) {
         putstring_nl("can't write fix!");
	       return;
      }
      Serial.print(buffer);
      // clear print error
      f.writeError = 0;      
      // add sensor data
      for (uint8_t ia = 0; ia < sensorCount; ia++) {
        Serial.print(',');
        f.print(',');
        uint16_t data = analogRead(ia);
        Serial.print(data);
        f.print(data);
      }
      Serial.println();
      f.println();
      if (f.writeError || !f.sync()) {
         putstring_nl("can't write data!");
         error(4);
      }
      digitalWrite(led2Pin, LOW);
      bufferidx = 0;
      return;
    }
    bufferidx++;
    if (bufferidx == BUFFSIZE-1) {
       Serial.print('!', BYTE);
       bufferidx = 0;
    }
  }
}

