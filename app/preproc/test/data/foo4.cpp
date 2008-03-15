//
// Continually measures temperatures at three points using the Dallas DS18B20 on three
// separate Arduino pins.
//
// Uses the parasitic power mode
//
// Displays to a serial LCD operating at 9600 baud.
//
// Arduino Board		DS18B20
//
//			  +5 VDC
//			  |
//			 4.7K
// 8 ---------------- |----- 2 (DQ)  Note that terms 1 and 3 are grounded.
//
// 7 ---- same as above
// 6 ---- same as above
//
// Tx ---------------------------- To Serial LCD (LCD #117)
//
// Peter H Anderson, Baltimore, MD, May 5, '07


void setup()
{
  int n, dev_channel[3] = {8, 7, 6}, _1W_Pin;

  for (n=0; n<1; n++)
  {
    _1W_Pin = dev_channel[n];
    digitalWrite(_1W_Pin, LOW);
    pinMode(_1W_Pin, INPUT);	// sets the digital pin as input (logic 1)
  }
  Serial.begin(9600);
  delay(100);
  Serial.print("?B40"); // set backlight intensity
  delay(100);
}

void loop()
{
  int n, dev_channel[3] = {8, 7, 6}, _1W_Pin;
  int HighByte, LowByte, TReading, SignBit, Tc_100, Whole, Fract;

  for (n=0; n<3; n++)
  {
	_1W_Pin = dev_channel[n];
	OneWireReset(_1W_Pin);
	OneWireOutByte(_1W_Pin, 0xcc, 0);
	OneWireOutByte(_1W_Pin, 0x44, 1); // perform temperature conversion, strong pullup for one sec

	OneWireReset(_1W_Pin);
	OneWireOutByte(_1W_Pin, 0xcc, 0);
	OneWireOutByte(_1W_Pin, 0xbe, 0);

	LowByte = OneWireInByte(_1W_Pin);
	HighByte = OneWireInByte(_1W_Pin);
	TReading = (HighByte << 8) + LowByte;
	SignBit = TReading & 0x8000;  // test most sig bit
	if (SignBit) // negative
	{
	   TReading = (TReading ^ 0xffff) + 1; // 2's comp
	}
	Tc_100 = (6 * TReading) + TReading / 4;    // multiply by (100 * 0.0625) or 6.25

	Whole = Tc_100 / 100;  // separate off the whole and fractional portions
	Fract = Tc_100 % 100;

	if (n==0)  // if its the first time, clear the LCD
	{
	   Serial.print("?f");
	   delay(100);
	}

	if (SignBit) // If its negative
	{
	  Serial.print("-");
	}
	Serial.print(Whole);
	Serial.print(".");
	if (Fract < 10)
	{
	  Serial.print("0");
	}
	Serial.print(Fract);
	Serial.print("?n");
  }
  delay(5000);	// 5 second delay.  Adjust as necessary
}

void OneWireReset(int _1W_Pin) // reset.  Should improve to act as a presence pulse
{
	digitalWrite(_1W_Pin, LOW);
	pinMode(_1W_Pin, OUTPUT); // bring low for 500 us
	delayMicroseconds(500);
	pinMode(_1W_Pin, INPUT);
	delayMicroseconds(500);
}

void OneWireOutByte(int _1W_Pin, byte d, byte strong) // output byte d (least sig bit first).
{
   byte n;

   for(n=8; n!=0; n--)
   {
	if ((d & 0x01) == 1)  // test least sig bit
	{
	   digitalWrite(_1W_Pin, LOW);
	   pinMode(_1W_Pin, OUTPUT);
	   delayMicroseconds(5);
	   pinMode(_1W_Pin, INPUT);
	   delayMicroseconds(60);
	}
	else
	{
	   digitalWrite(_1W_Pin, LOW);
	   pinMode(_1W_Pin, OUTPUT);
	   delayMicroseconds(60);
	   pinMode(_1W_Pin, INPUT);
	}

	d=d>>1; // now the next bit is in the least sig bit position.
   }
   if(strong)
   {
	 digitalWrite(_1W_Pin, HIGH); // One sec of strong +5 VDC
	 pinMode(_1W_Pin, OUTPUT);
	 delay(1000);
	 pinMode(_1W_Pin, INPUT);
	 digitalWrite(_1W_Pin, LOW);
   }
}
