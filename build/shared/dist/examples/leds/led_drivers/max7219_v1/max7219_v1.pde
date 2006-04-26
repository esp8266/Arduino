int serialVal = 0;
int value = 0;
int i = 0;
boolean reading = false;
boolean writing = false;
int times = 0;
int serialBuff[7];

// define wiring pins
byte pin_max7219_clock  =  4;
byte pin_max7219_load   =  2;
byte pin_max7219_dataIn =  3;

// specify wiring pin i/o directions
void setPinModes()
{
  pinMode(pin_max7219_dataIn, OUTPUT);
  pinMode(pin_max7219_clock,  OUTPUT);
  pinMode(pin_max7219_load,   OUTPUT);
}

// define max7219 registers
byte max7219_reg_noop        = 0x00;
byte max7219_reg_digit0      = 0x01;
byte max7219_reg_digit1      = 0x02;
byte max7219_reg_digit2      = 0x03;
byte max7219_reg_digit3      = 0x04;
byte max7219_reg_digit4      = 0x05;
byte max7219_reg_digit5      = 0x06;
byte max7219_reg_digit6      = 0x07;
byte max7219_reg_digit7      = 0x08;
byte max7219_reg_decodeMode  = 0x09;
byte max7219_reg_intensity   = 0x0a;
byte max7219_reg_scanLimit   = 0x0b;
byte max7219_reg_shutdown    = 0x0c;
byte max7219_reg_displayTest = 0x0f;

// define max7219 as rows and cols (for nexus 8x8 displays)
byte max7219_row0 = 0x01;
byte max7219_row1 = 0x02;
byte max7219_row2 = 0x03;
byte max7219_row3 = 0x04;
byte max7219_row4 = 0x05;
byte max7219_row5 = 0x06;
byte max7219_row6 = 0x07;
byte max7219_row7 = 0x08;
byte max7219_col0 = 0x80;
byte max7219_col1 = 0x01;
byte max7219_col2 = 0x02;
byte max7219_col3 = 0x04;
byte max7219_col4 = 0x08;
byte max7219_col5 = 0x10;
byte max7219_col6 = 0x20;
byte max7219_col7 = 0x40;
 

// function to control max7219 data line
void max7219_setData(boolean value)
{
  digitalWrite(pin_max7219_dataIn, value);
}

// function to control max7219 clock line
void max7219_setClock(boolean value)
{
  digitalWrite(pin_max7219_clock, value);
}

// function to control max7219 load line
void max7219_setLoad(boolean value)
{
  digitalWrite(pin_max7219_load, value);
}

// function that puts a byte of data to the max7219
void max7219_putByte(byte data)
{
  byte i = 8;
  byte mask;
  while(i > 0) {
    mask = 0x01 << (i - 1);  // get bitmask
    max7219_setClock(LOW);   // tick
    if (data & mask){        // choose bit
      max7219_setData(HIGH); // send 1
    }else{
      max7219_setData(LOW);  // send 0
    }
    max7219_setClock(HIGH);  // tock
    --i;                     // move to lesser bit
  }
}

// function that puts a byte of data into a max7219 register
void max7219_put(byte reg, byte data)
{
  max7219_setLoad(HIGH); // begin
  max7219_putByte(reg);  // specify register
  max7219_putByte(data); // put data
  max7219_setLoad(LOW);  // latch in data
  max7219_setLoad(HIGH); // end
}

// function that sets brightness of the max7219
void max7219_setIntensity(byte intensity)
{
  // range: 0x00 to 0x0f
  max7219_put(max7219_reg_intensity, intensity & 0x0f);
}
////////////////////////////////////////////
// function that sets the same value for all registers of the max7219

void max7219_all(byte value)
{  
  max7219_put(0x01, value);
  max7219_put(0x02, value);
  max7219_put(0x03, value);
  max7219_put(0x04, value);
  max7219_put(0x05, value);
  max7219_put(0x06, value);
  max7219_put(0x07, value);
  max7219_put(0x08, value);
}

///////////////////////////////////////////
void printBuff(){
  for (i=1;i <= 8;i++){
    max7219_put(0x0+i, serialBuff[i-1]);
  }
}
////////////////////////////////////////////

void readBuff(){
  printString("into readbuff method");
  serialVal = serialRead();
  if (serialVal != -1){
    printString("information there");
    if ((serialVal == 43) && !reading){
      reading = true;
      printString("plus");
    }

    if ((serialVal == 45) && (times == 0 && reading)){
      writing = true;
      printString("minus");
    }

    if (reading && writing){
      serialBuff[times] = serialVal;
      times++;
    }

    if (times >= 7){
    printString("Print to buff");
      times = 0;
      reading = false;
      writing = false;
      printBuff();
    }

  
  }
}

//////////////////////////////////////////////////////////////

// function that initializes the max7219 to use a matrix of leds
void max7219_init()
{
  max7219_put(max7219_reg_scanLimit, 0x07);   // use all 8 columns
  max7219_put(max7219_reg_decodeMode, 0x00);  // using an led matrix (not digits)
  max7219_put(max7219_reg_shutdown, 0x01);    // not in shutdown mode
  max7219_put(max7219_reg_displayTest, 0x00); // no display test
  max7219_all(0x00);                          // empty registers
  max7219_setIntensity(0x0f);                 // set initial brightness to dim
}
 

 
// program initialization routine
void setup()
{
  setPinModes();
  max7219_init();
}

// program loop
void loop()
{
  printString("in the loop");
  //max7219_all(0x00);
  //delay(500);
  readBuff();
  
}
