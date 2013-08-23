#ifndef ArduinoRobot_h
#define ArduinoRobot_h

#include "EasyTransfer2.h"
#include "Multiplexer.h"
#include "LineFollow.h"
//#include "IRremote.h"

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

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

//bottom TKs, just for communication purpose
#define B_TK1 201
#define B_TK2 202
#define B_TK3 203
#define B_TK4 204

/*
A message structure will be:
switch mode (2):
	byte COMMAND_SWITCH_MODE, byte mode
run (5):
	byte COMMAND_RUN, int speedL, int speedR
analogWrite (3):
	byte COMMAND_ANALOG_WRITE, byte codename, byte value;
digitalWrite (3):
	byte COMMAND_DIGITAL_WRITE, byte codename, byte value;
analogRead (2):
	byte COMMAND_ANALOG_READ, byte codename;
analogRead _return_ (4):
	byte COMMAND_ANALOG_READ_RE, byte codename, int value;
digitalRead (2):
	byte COMMAND_DIGITAL_READ, byte codename;
digitalRead _return_ (4):
	byte COMMAND_DIGITAL_READ_RE, byte codename, int value;
read IR (1):
	byte COMMAND_READ_IR;
read IR _return_ (9):
	byte COMMAND_READ_IR_RE, int valueA, int valueB, int valueC, int valueD;


*/

class RobotMotorBoard:public LineFollow{
	public:
		RobotMotorBoard();
		void begin();
		
		void process();
		
		void parseCommand();
		
		int IRread(uint8_t num);
		
		void setMode(uint8_t mode);
		void pauseMode(bool onOff);
		
		void motorsWrite(int speedL, int speedR);
		void motorsWritePct(int speedLpct, int speedRpct);//write motor values in percentage
		void motorsStop();		
	private:
		float motorAdjustment;//-1.0 ~ 1.0, whether left is lowered or right is lowered
		
		//convert codename to actual pins
		uint8_t parseCodename(uint8_t codename);
		uint8_t codenameToAPin(uint8_t codename);
		
		void stopCurrentActions();
		//void sendCommand(byte command,byte codename,int value);
		
		void _analogWrite(uint8_t codename, int value);
		void _digitalWrite(uint8_t codename, bool value);
		void _analogRead(uint8_t codename);
		void _digitalRead(uint8_t codename);
		int _IRread(uint8_t num);
		void _readIR();
		void _readTrim();
		
		void _refreshMotorAdjustment();
		
		Multiplexer IRs;
		uint8_t mode;
		uint8_t isPaused;
		EasyTransfer2 messageIn;
		EasyTransfer2 messageOut;
		
		//Line Following 
		void reportActionDone();
};

extern RobotMotorBoard RobotMotor;

#endif