#include "ArduinoRobot.h"
#include "Wire.h"

#define BUFFPIXEL 20

bool cmp(char* str1, char* str2, uint8_t len);
uint16_t read16(Fat16& f);
uint32_t read32(Fat16& f);
//uint16_t color565(uint8_t r, uint8_t g, uint8_t b);

void RobotControl::beginTFT(uint16_t foreGround, uint16_t backGround){
	//TFT initialization
	Arduino_LCD::initB();
	Arduino_LCD::fillScreen(backGround);
	Arduino_LCD::setTextColor(foreGround);
	Arduino_LCD::setTextSize(1);
	this->foreGround=foreGround;
	this->backGround=backGround;
}
void RobotControl::_enableLCD(){
	DDRB = DDRB & 0xEF; //pinMode(CS_SD,INPUT);
	DDRB = DDRB | 0x20; //pinMode(CS_LCD,OUTPUT);
}
/*void RobotControl::_setErase(uint8_t posX, uint8_t posY){
	Arduino_LCD::setCursor(posX,posY);
	Arduino_LCD::setTextColor(backGround);
	Arduino_LCD::setTextSize(1);
}
void RobotControl::_setWrite(uint8_t posX, uint8_t posY){
	Arduino_LCD::setCursor(posX,posY);
	Arduino_LCD::setTextColor(foreGround);
	Arduino_LCD::setTextSize(1);
}*/
/*
void RobotControl::text(int value, uint8_t posX, uint8_t posY, bool EW){
	if(EW)
		_setWrite(posX,posY);
	else 
		_setErase(posX,posY);
	Arduino_LCD::print(value);
}
void RobotControl::text(long value, uint8_t posX, uint8_t posY, bool EW){
	if(EW)
		_setWrite(posX,posY);
	else 
		_setErase(posX,posY);
	Arduino_LCD::print(value);
}
void RobotControl::text(char* value, uint8_t posX, uint8_t posY, bool EW){
	if(EW)
		_setWrite(posX,posY);
	else 
		_setErase(posX,posY);
	Arduino_LCD::print(value);
}
void RobotControl::text(char value, uint8_t posX, uint8_t posY, bool EW){
	if(EW)
		_setWrite(posX,posY);
	else 
		_setErase(posX,posY);
	Arduino_LCD::print(value);
}
*/

void RobotControl::debugPrint(long value, uint8_t x, uint8_t y){
	static long oldVal=0;
	Arduino_LCD::stroke(backGround);
	text(oldVal,x,y);
	Arduino_LCD::stroke(foreGround);
	text(value,x,y);
	oldVal=value;
}

void RobotControl::clearScreen(){
	Arduino_LCD::fillScreen(backGround);
}

void RobotControl::drawBMP(char* filename, uint8_t x, uint8_t y){
	/*for(int j=0;j<NUM_EEPROM_BMP;j++){
		Serial.println(_eeprom_bmp[j].name);
		Serial.print(" ");
		Serial.print(_eeprom_bmp[j].address);
		Serial.print(" ");
		Serial.print(_eeprom_bmp[j].width);
		Serial.print(" ");
		Serial.println(_eeprom_bmp[j].height);
	}
	Serial.println();*/
	if(_isEEPROM_BMP_Allocated){
		for(int i=0;i<NUM_EEPROM_BMP;i++){
			if(cmp(_eeprom_bmp[i].name,filename,7)){
				/*Serial.println(_eeprom_bmp[i].name);
				Serial.print(" ");
				Serial.print(_eeprom_bmp[i].address);
				Serial.print(" ");
				Serial.print(_eeprom_bmp[i].width);
				Serial.print(" ");
				Serial.println(_eeprom_bmp[i].height);*/
				_drawBMP(_eeprom_bmp[i].address,x,y,_eeprom_bmp[i].width,_eeprom_bmp[i].height);
				return;
			}
		}
	}else{
		_drawBMP(filename,x,y);//goes to SD
	}
}
bool cmp(char* str1, char* str2, uint8_t len){
  for(uint8_t i=0;i<len;i++){
	if(str1[i]==' ')break;
    if(str1[i]!=str2[i])return false;
  }
  return true;
}

void RobotControl::_drawBMP(uint32_t iconOffset, uint8_t x, uint8_t y, uint8_t width, uint8_t height){
	uint8_t screenWidth=Arduino_LCD::width();
	uint8_t screenHeight=Arduino_LCD::height();
	if((x >= screenWidth) || (y >= screenHeight)) return;

	// Crop area to be loaded
	if((x+width-1) >= screenWidth)  width = screenWidth  - x;
	if((y+height-1) >= screenHeight) height = screenHeight - y;

	// Set TFT address window to clipped image bounds
	Arduino_LCD::setAddrWindow(x, y, x+width-1, y+height-1);

	// launch the reading command
	_drawBMP_EEPROM(iconOffset, width, height);
}

//  Draw BMP from SD card through the filename
void RobotControl::_drawBMP(char* filename, uint8_t posX, uint8_t posY){
	uint8_t  bmpWidth, bmpHeight;   // W+H in pixels
	uint8_t  bmpDepth;              // Bit depth (currently must be 24)
	uint32_t bmpImageoffset;        // Start of image data in file
	uint32_t rowSize;               // Not always = bmpWidth; may have padding
	uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel buffer (R+G+B per pixel)
	uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
	bool  goodBmp = false;       // Set to true on valid header parse
	bool  flip    = true;        // BMP is stored bottom-to-top
	uint8_t  w, h, row, col;
	uint8_t  r, g, b;
	uint32_t pos = 0;

	// Open requested file on SD card
	if ((file.open(filename,O_READ)) == NULL) {
		return;
	}

	// Parse BMP header
	if(read16(file) == 0x4D42) { // BMP signature
		read32(file);//uint32_t aux = read32(file);
		(void)read32(file); // Read & ignore creator bytes
		bmpImageoffset = read32(file); // Start of image data
		
		// Read DIB header
		(void)read32(file);//aux = read32(file);
		bmpWidth  = read32(file);
		bmpHeight = read32(file);
		
		if(read16(file) == 1) { // # planes -- must be '1'
			bmpDepth = read16(file); // bits per pixel
			if((bmpDepth == 24) && (read32(file) == 0)) { // 0 = uncompressed
				goodBmp = true; // Supported BMP format -- proceed!

				// BMP rows are padded (if needed) to 4-byte boundary
				rowSize = (bmpWidth * 3 + 3) & ~3;

				// If bmpHeight is negative, image is in top-down order.
				// This is not canon but has been observed in the wild.
				if(bmpHeight < 0) {
					bmpHeight = -bmpHeight;
					flip      = false;
				}

				// Crop area to be loaded
				w = bmpWidth;
				h = bmpHeight;

				//  Start drawing
				//_enableLCD();
				Arduino_LCD::setAddrWindow(posX, posY, posX+bmpWidth-1, posY+bmpHeight-1);  

				for (row=0; row<h; row++) { // For each scanline...
					if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
						pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
					else     // Bitmap is stored top-to-bottom
						pos = bmpImageoffset + row * rowSize;
						
					if(file.curPosition() != pos) { // Need seek?
						//_enableSD();
						file.seekSet(pos);
						buffidx = sizeof(sdbuffer); // Force buffer reload
						//_enableLCD();
					}
					for (col=0; col<w; col++) { // For each pixel...
						// Time to read more pixel data?
						if (buffidx >= sizeof(sdbuffer)) { // Indeed
							//_enableSD();
							file.read(sdbuffer, sizeof(sdbuffer));
							buffidx = 0; // Set index to beginning
							//_enableLCD();
						}
						// Convert pixel from BMP to TFT format, push to display
						b = sdbuffer[buffidx++];
						g = sdbuffer[buffidx++];
						r = sdbuffer[buffidx++];

						int  color = Arduino_LCD::Color565(r,g,b);
						
						Arduino_LCD::pushColor(color);
					} // end pixel
				} // end scanline
				//_enableSD();
			} // end goodBmp*/
		}
	}
	file.close();
	//_enableLCD();
}
uint16_t read16(Fat16& f) {
  uint16_t result;
  f.read(&result,sizeof(result));
  return result;
}
uint32_t read32(Fat16& f) {
  uint32_t result;
  f.read(&result,sizeof(result));
  return result;
}
/*
uint16_t color565(uint8_t r, uint8_t g, uint8_t b) {
  return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}*/


void RobotControl::_drawBMP_EEPROM(uint16_t address, uint8_t width, uint8_t height){
	uint16_t u16retVal = 0;
	EEPROM_I2C::_beginTransmission(address);
	EEPROM_I2C::_endTransmission();
	/*Wire.beginTransmission(DEVICEADDRESS);
	Wire.write( (address >> 8) & 0xFF );
	Wire.write( (address >> 0) & 0xFF );
	Wire.endTransmission();*/

	long s = width * height ;
	for(long j = 0; j < (long) s >> 4; j++) { // divided by 32, times 2
		Wire.requestFrom(DEVICEADDRESS, 32);
		for(int i = 0; i < 32; i+=2) {
			u16retVal = Wire.read();
			u16retVal = (u16retVal << 8) + Wire.read();
			Arduino_LCD::pushColor(u16retVal);
		}
	}

}
void RobotControl::beginBMPFromEEPROM(){
	_eeprom_bmp=(EEPROM_BMP*)malloc(NUM_EEPROM_BMP*sizeof(EEPROM_BMP));
	EEPROM_I2C::_beginTransmission(0);
	EEPROM_I2C::_endTransmission();
	
	for(uint8_t j=0;j<NUM_EEPROM_BMP;j++){
		Wire.requestFrom(DEVICEADDRESS, sizeof(EEPROM_BMP));
		for(uint8_t i=0;i<8;i++){
			_eeprom_bmp[j].name[i]=Wire.read();//name
		}
		_eeprom_bmp[j].width=Wire.read();//width
		_eeprom_bmp[j].height=Wire.read();//height
		
		_eeprom_bmp[j].address=Wire.read();
		_eeprom_bmp[j].address=_eeprom_bmp[j].address + (Wire.read() << 8);//address
	}
	_isEEPROM_BMP_Allocated=true;
	
}
void RobotControl::endBMPFromEEPROM(){
	free(_eeprom_bmp);
	_isEEPROM_BMP_Allocated=false;
}
