#ifndef ArduinoRobot_h
#define ArduinoRobot_h

#include "Arduino_LCD.h" // Hardware-specific library
//#include "FormattedText.h"
#include "SquawkSD.h"
#include "Multiplexer.h"
#include "EasyTransfer2.h"
#include "EEPROM_I2C.h"
#include "Compass.h"
#include "Fat16.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif


#define BUTTON_NONE   -1
#define BUTTON_LEFT  0
#define BUTTON_DOWN      1
#define BUTTON_UP     2
#define BUTTON_RIGHT    3
#define BUTTON_MIDDLE     4
#define NUMBER_BUTTONS 5

//beep length
#define BEEP_SIMPLE   0
#define BEEP_DOUBLE    1
#define BEEP_LONG    2

// image locations on the EEPROM
 #define HOME_BMP	0
#define BATTERY_BMP	2048
#define COMPASS_BMP	4096
#define CONTROL_BMP	6144
#define GEARS_BMP	8192
#define LIGHT_BMP	10240
#define OSCILLO_BMP	12288
#define VOLT_BMP	14336
#define INICIO_BMP	16384  // this is a full screen splash

//Command code
#define COMMAND_SWITCH_MODE 0
#define COMMAND_RUN	10
#define COMMAND_MOTORS_STOP	11
#define COMMAND_ANALOG_WRITE	20
#define COMMAND_DIGITAL_WRITE	30
#define COMMAND_ANALOG_READ	40
#define COMMAND_ANALOG_READ_RE	41
#define COMMAND_DIGITAL_READ	50
#define COMMAND_DIGITAL_READ_RE	51
#define COMMAND_READ_IR	60
#define COMMAND_READ_IR_RE	61
#define COMMAND_ACTION_DONE 70
#define COMMAND_READ_TRIM 80
#define COMMAND_READ_TRIM_RE 81
#define COMMAND_PAUSE_MODE 90
#define COMMAND_LINE_FOLLOW_CONFIG 100

//component codename
#define CN_LEFT_MOTOR	0
#define CN_RIGHT_MOTOR	1
#define CN_IR 2

//motor board modes
#define MODE_SIMPLE 0
#define MODE_LINE_FOLLOW 1
#define MODE_ADJUST_MOTOR 2
#define MODE_IR_CONTROL 3

//port types, for R/W
#define TYPE_TOP_TK 0
#define TYPE_TOP_TKD 1
#define TYPE_BOTTOM_TK 2

//top TKs
#define TK0 100
#define TK1 101
#define TK2 102
#define TK3 103
#define TK4 104
#define TK5 105
#define TK6 106
#define TK7 107

#define M0 TK0
#define M1 TK1
#define M2 TK2
#define M3 TK3
#define M4 TK4
#define M5 TK5
#define M6 TK6
#define M7 TK7

//bottom TKs, just for communication purpose
#define B_TK1 201
#define B_TK2 202
#define B_TK3 203
#define B_TK4 204

#define D10 B_TK1
#define D9 B_TK2
#define D8 B_TK4
#define D7 B_TK3

//bottom IRs, for communication purpose
#define B_IR0 210
#define B_IR1 211
#define B_IR2 212
#define B_IR3 213
#define B_IR4 214

#ifndef LED1
#define LED1 17
#endif

//320 - 337 username, 
#define ADDRESS_USERNAME 320
//338 - 355 robotname, 
#define ADDRESS_ROBOTNAME 338
//356 - 373 cityname, 
#define ADDRESS_CITYNAME 356
 //374- 391 countryname, 
#define ADDRESS_COUNTRYNAME 374
//508-511 robot info
#define ADDRESS_ROBOTINFO 508

#define	BLACK	ILI9163C_BLACK   
#define	BLUE	ILI9163C_BLUE    
#define	RED		ILI9163C_RED     
#define	GREEN	ILI9163C_GREEN   
#define CYAN	ILI9163C_CYAN    
#define MAGENTA	ILI9163C_MAGENTA 
#define YELLOW	ILI9163C_YELLOW    
#define WHITE	ILI9163C_WHITE   
 
//A data structure for storing the current state of motor board
struct MOTOR_BOARD_DATA{
	int _B_TK1;
	int _B_TK2;
	int _B_TK3;
	int _B_TK4;
	
	/*int _B_IR0;
	int _B_IR1;
	int _B_IR2;
	int _B_IR3;
	int _B_IR4;*/
};

/*
A message structure will be:
switch mode:
	byte COMMAND_SWITCH_MODE, byte mode
run:
	byte COMMAND_RUN, int speedL, int speedR
analogWrite:
	byte COMMAND_ANALOG_WRITE, byte codename, byte value;
digitalWrite:
	byte COMMAND_DIGITAL_WRITE, byte codename, byte value;
analogRead:
	byte COMMAND_ANALOG_READ, byte codename;
analogRead return:
	byte COMMAND_ANALOG_READ_RE, byte codename, int value;
digitalRead return:
	byte COMMAND_DIGITAL_READ_RE, byte codename, byte value;
read IR:
	byte COMMAND_READ_IR, int valueA, int valueB, int valueC, int valueD;


*/
#define NUM_EEPROM_BMP 10
struct EEPROM_BMP{
	char name[8];
	uint8_t width;
	uint8_t height;
	uint16_t address;
};

//if you call #undef USE_SQUAWK_SYNTH_SD at the beginning of your sketch,
//it's going to remove anything regarding sound playing

class RobotControl:public Multiplexer, 
public EEPROM_I2C, 
public Compass, 
public SquawkSynthSD, 
//public FormattedText
public Arduino_LCD
{
	public:
		RobotControl();
		void begin();
		void setMode(uint8_t mode);
		
		//Read & Write, TK0 - TK7, TKD0 - TKD1, bottom TK0 - TK4
		bool digitalRead(uint8_t port);
		int analogRead(uint8_t port);
		void digitalWrite(uint8_t port, bool value);
		void analogWrite(uint8_t port, uint8_t value);//It's not available, as there's no pin can be used for analog write
		
		//IR sensors from the bottom board
		//define an array as "int arr[4];", and supply the arry name here
		uint16_t IRarray[5];
		void updateIR();
		
		//on board Potentiometor
		int knobRead();
		//Potentiometor of the motor board
		int trimRead();
		
		//on board piezo
		void beginSpeaker(uint16_t frequency=44100);
		void playMelody(char* script);
		void playFile(char* filename);
		void stopPlayFile();
		void beep(int beep_length=BEEP_SIMPLE);
		void tempoWrite(int tempo);
		void tuneWrite(float tune);

		//compass
		uint16_t compassRead();
		void drawCompass(uint16_t value);
		void drawBase();
		void drawDire(int16_t dire);
		
		//keyboard
		void keyboardCalibrate(int *vals);
		int8_t keyboardRead();//return the key that is being pressed?Has been pressed(with _processKeyboard)?
		
		//movement
		void moveForward(int speed);
		void moveBackward(int speed);
		void turnLeft(int speed);
		void turnRight(int speed);
		void motorsStop();
		void motorsWritePct(int speedLeftPct, int speedRightPct);
		
		void motorsWrite(int speedLeft,int speedRight);
		void pointTo(int degrees);//turn to an absolute angle from the compass
		void turn(int degress);//turn certain degrees from the current heading
		
		//Line Following
		void lineFollowConfig(uint8_t KP, uint8_t KD, uint8_t robotSpeed, uint8_t intergrationTime);//default 11 5 50 10
		
		//TFT LCD
		//use the same commands as Arduino_LCD
		void beginTFT(uint16_t foreGround=BLACK, uint16_t background=WHITE);
		/*void text(int value, uint8_t posX, uint8_t posY, bool EW);
		void text(long value, uint8_t posX, uint8_t posY, bool EW);
		void text(char* value, uint8_t posX, uint8_t posY, bool EW);
		void text(char value, uint8_t posX, uint8_t posY, bool EW);*/
		void debugPrint(long value, uint8_t x=0, uint8_t y=0);
		void clearScreen();
		
		void drawBMP(char* filename, uint8_t x, uint8_t y);//detect if draw with EEPROM or SD, and draw it
		void _drawBMP(uint32_t iconOffset, uint8_t x, uint8_t y, uint8_t width, uint8_t height);//draw from EEPROM
		void _drawBMP(char* filename, uint8_t x, uint8_t y);//draw from SD
		void beginBMPFromEEPROM();
		void endBMPFromEEPROM();
		
		uint16_t foreGround;//foreground color
		uint16_t backGround;//background color

		
		//SD card
		void beginSD();
		
		//Information
		void userNameRead(char* container);
		void robotNameRead(char* container);
		void cityNameRead(char* container);
		void countryNameRead(char* container);
		
		void userNameWrite(char* text);
		void robotNameWrite(char* text);
		void cityNameWrite(char* text);
		void countryNameWrite(char* text);
		
		//Others
		bool isActionDone();
		void pauseMode(uint8_t onOff);
		void displayLogos();
		void waitContinue(uint8_t key=BUTTON_MIDDLE);
		
	private:
		//Read & Write
		uint8_t _getTypeCode(uint8_t port);//different ports need different actions
		uint8_t _portToTopMux(uint8_t port);//get the number for multiplexer within top TKs
		uint8_t _topDPortToAPort(uint8_t port);//get the corrensponding analogIn pin for top TKDs
		
		bool _digitalReadTopMux(uint8_t port);//TK0 - TK7
		int _analogReadTopMux(uint8_t port);
		
		bool _digitalReadTopPin(uint8_t port);
		int _analogReadTopPin(uint8_t port);
		void _digitalWriteTopPin(uint8_t port, bool value);
		
		MOTOR_BOARD_DATA motorBoardData;
		int* parseMBDPort(uint8_t port);
		int get_motorBoardData(uint8_t port);
		void set_motorBoardData(uint8_t port, int value);
		
		bool _requestDigitalRead(uint8_t port);
		int _requestAnalogRead(uint8_t port);
		void _requestDigitalWrite(uint8_t port, uint8_t value);
				
		//LCD
		void _enableLCD();
		void _setWrite(uint8_t posX, uint8_t posY);
		void _setErase(uint8_t posX, uint8_t posY);
		
		
		//SD
		SdCard card;
		Fat16 file;
		Fat16 melody;
		void _enableSD();
		
		//keyboard
		void _processKeyboard(); //need to run in loop, detect if the key is actually pressed
		int averageAnalogInput(int pinNum);
		
		//Ultrasonic ranger
		//uint8_t pinTrigger_UR;
		//uint8_t pinEcho_UR;
		
		//Melody
		void playNote(byte period, word length, char modifier); 
		
		//Communication
		
		EasyTransfer2 messageOut;
		EasyTransfer2 messageIn;
		
		//TFT LCD
		bool _isEEPROM_BMP_Allocated;
		EEPROM_BMP * _eeprom_bmp;
		void _drawBMP_EEPROM(uint16_t address, uint8_t width, uint8_t height);
		void _drawBMP_SD(char* filename, uint8_t x, uint8_t y);

		
};

inline void RobotControl::userNameRead(char* container){
	EEPROM_I2C::readBuffer(ADDRESS_USERNAME,(uint8_t*)container,18);
}
inline void RobotControl::robotNameRead(char* container){
	EEPROM_I2C::readBuffer(ADDRESS_ROBOTNAME,(uint8_t*)container,18);
}
inline void RobotControl::cityNameRead(char* container){
	EEPROM_I2C::readBuffer(ADDRESS_CITYNAME,(uint8_t*)container,18);
}
inline void RobotControl::countryNameRead(char* container){
	EEPROM_I2C::readBuffer(ADDRESS_COUNTRYNAME,(uint8_t*)container,18);
}

inline void RobotControl::userNameWrite(char* text){
	EEPROM_I2C::writePage(ADDRESS_USERNAME,(uint8_t*)text,18);
}
inline void RobotControl::robotNameWrite(char* text){
	EEPROM_I2C::writePage(ADDRESS_ROBOTNAME,(uint8_t*)text,18);
}
inline void RobotControl::cityNameWrite(char* text){
	EEPROM_I2C::writePage(ADDRESS_CITYNAME,(uint8_t*)text,18);
}
inline void RobotControl::countryNameWrite(char* text){
	EEPROM_I2C::writePage(ADDRESS_COUNTRYNAME,(uint8_t*)text,18);
}

extern RobotControl Robot;

#endif