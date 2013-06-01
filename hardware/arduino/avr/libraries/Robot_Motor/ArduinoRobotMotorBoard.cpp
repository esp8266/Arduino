#include "ArduinoRobotMotorBoard.h"
#include "EasyTransfer2.h"
#include "Multiplexer.h"
#include "LineFollow.h"

RobotMotorBoard::RobotMotorBoard(){
	//LineFollow::LineFollow();
}
/*void RobotMotorBoard::beginIRReceiver(){
	IRrecv::enableIRIn();
}*/
void RobotMotorBoard::begin(){
	//initialze communication
	Serial1.begin(9600);
	messageIn.begin(&Serial1);
	messageOut.begin(&Serial1);

	//init MUX
	uint8_t MuxPins[]={MUXA,MUXB,MUXC};
	this->IRs.begin(MuxPins,MUX_IN,3);
	pinMode(MUXI,INPUT);
	digitalWrite(MUXI,LOW);
	
	isPaused=false;
}

void RobotMotorBoard::process(){
	if(isPaused)return;//skip process if the mode is paused
	
	if(mode==MODE_SIMPLE){
		//Serial.println("s");
		//do nothing? Simple mode is just about getting commands
	}else if(mode==MODE_LINE_FOLLOW){
		//do line following stuff here.
		LineFollow::runLineFollow();
	}else if(mode==MODE_ADJUST_MOTOR){
		//Serial.println('a');
		//motorAdjustment=analogRead(POT);
		//setSpeed(255,255);
		//delay(100);
	}
}
void RobotMotorBoard::pauseMode(bool onOff){
	if(onOff){
		isPaused=true;
	}else{
		isPaused=false;
	}
	stopCurrentActions();

}
void RobotMotorBoard::parseCommand(){
	uint8_t modeName;
	uint8_t codename;
	int value;
	int speedL;
	int speedR;
	if(this->messageIn.receiveData()){
		//Serial.println("data received");
		uint8_t command=messageIn.readByte();
		//Serial.println(command);
		switch(command){
			case COMMAND_SWITCH_MODE:
				modeName=messageIn.readByte();
				setMode(modeName);
				break;
			case COMMAND_RUN:
				if(mode==MODE_LINE_FOLLOW)break;//in follow line mode, the motor does not follow commands
				speedL=messageIn.readInt();
				speedR=messageIn.readInt();
				motorsWrite(speedL,speedR);
				break;
			case COMMAND_MOTORS_STOP:
				motorsStop();
				break;
			case COMMAND_ANALOG_WRITE:
				codename=messageIn.readByte();
				value=messageIn.readInt();
				_analogWrite(codename,value);
				break;
			case COMMAND_DIGITAL_WRITE:
				codename=messageIn.readByte();
				value=messageIn.readByte();
				_digitalWrite(codename,value);
				break;
			case COMMAND_ANALOG_READ:
				codename=messageIn.readByte();
				_analogRead(codename);
				break;
			case COMMAND_DIGITAL_READ:
				codename=messageIn.readByte();
				_digitalRead(codename);
				break;
			case COMMAND_READ_IR:
				_readIR();
				break;
			case COMMAND_READ_TRIM:
				_readTrim();
				break;
			case COMMAND_PAUSE_MODE:
				pauseMode(messageIn.readByte());//onOff state
				break;
			case COMMAND_LINE_FOLLOW_CONFIG:
				LineFollow::config(
					messageIn.readByte(),	//KP
					messageIn.readByte(),	//KD
					messageIn.readByte(),	//robotSpeed
					messageIn.readByte()	//IntegrationTime
				);
				break;
		}
	}
	//delay(5);
}
uint8_t RobotMotorBoard::parseCodename(uint8_t codename){
	switch(codename){
		case B_TK1:
			return TK1;
		case B_TK2:
			return TK2;
		case B_TK3:
			return TK3;
		case B_TK4:
			return TK4;
	}
}
uint8_t RobotMotorBoard::codenameToAPin(uint8_t codename){
	switch(codename){
		case B_TK1:
			return A0;
		case B_TK2:
			return A1;
		case B_TK3:
			return A6;
		case B_TK4:
			return A11;
	}
}

void RobotMotorBoard::setMode(uint8_t mode){
	if(mode==MODE_LINE_FOLLOW){
		LineFollow::calibIRs();
	}
	/*if(mode==SET_MOTOR_ADJUSTMENT){
			save_motor_adjustment_to_EEPROM();
		}
	*/
	/*if(mode==MODE_IR_CONTROL){
		beginIRReceiver();
	}*/
	this->mode=mode;
	//stopCurrentActions();//If line following, this should stop the motors
}

void RobotMotorBoard::stopCurrentActions(){
	motorsStop();
	//motorsWrite(0,0);
}

void RobotMotorBoard::motorsWrite(int speedL, int speedR){
	/*Serial.print(speedL);
	Serial.print(" ");
	Serial.println(speedR);*/
	//motor adjustment, using percentage
	_refreshMotorAdjustment();
	
	if(motorAdjustment<0){
		speedR*=(1+motorAdjustment);
	}else{
		speedL*=(1-motorAdjustment);
	}
	
	if(speedL>0){
		analogWrite(IN_A1,speedL);
		analogWrite(IN_A2,0);
	}else{
		analogWrite(IN_A1,0);
		analogWrite(IN_A2,-speedL);
	}
	
	if(speedR>0){
		analogWrite(IN_B1,speedR);
		analogWrite(IN_B2,0);
	}else{
		analogWrite(IN_B1,0);
		analogWrite(IN_B2,-speedR);
	}
}
void RobotMotorBoard::motorsWritePct(int speedLpct, int speedRpct){
	//speedLpct, speedRpct ranges from -100 to 100
	motorsWrite(speedLpct*2.55,speedRpct*2.55);
}
void RobotMotorBoard::motorsStop(){
	analogWrite(IN_A1,255);
	analogWrite(IN_A2,255);

	analogWrite(IN_B1,255);
	analogWrite(IN_B2,255);
}


/*
*
*
*	Input and Output ports
*
*
*/
void RobotMotorBoard::_digitalWrite(uint8_t codename,bool value){
	uint8_t pin=parseCodename(codename);
	digitalWrite(pin,value);
}
void RobotMotorBoard::_analogWrite(uint8_t codename,int value){
	//There's no PWM available on motor board
}
void RobotMotorBoard::_digitalRead(uint8_t codename){
	uint8_t pin=parseCodename(codename);
	bool value=digitalRead(pin);
	messageOut.writeByte(COMMAND_DIGITAL_READ_RE);
	messageOut.writeByte(codename);
	messageOut.writeByte(value);
	messageOut.sendData();
}
void RobotMotorBoard::_analogRead(uint8_t codename){
	uint8_t pin=codenameToAPin(codename);
	int value=analogRead(pin);
	messageOut.writeByte(COMMAND_ANALOG_READ_RE);
	messageOut.writeByte(codename);
	messageOut.writeInt(value);
	messageOut.sendData();
}
int RobotMotorBoard::IRread(uint8_t num){
	IRs.selectPin(num-1); //To make consistant with the pins labeled on the board
	return IRs.getAnalogValue();
}

void RobotMotorBoard::_readIR(){
	//Serial.println("readIR");
	int value;
	messageOut.writeByte(COMMAND_READ_IR_RE);
	for(int i=1;i<6;i++){
		value=IRread(i);
		messageOut.writeInt(value);
	}
	messageOut.sendData();
}

void RobotMotorBoard::_readTrim(){
	int value=analogRead(TRIM);
	messageOut.writeByte(COMMAND_READ_TRIM_RE);
	messageOut.writeInt(value);
	messageOut.sendData();	
}

void RobotMotorBoard::_refreshMotorAdjustment(){
	motorAdjustment=map(analogRead(TRIM),0,1023,-30,30)/100.0;
}

void RobotMotorBoard::reportActionDone(){
	setMode(MODE_SIMPLE);
	messageOut.writeByte(COMMAND_ACTION_DONE);
	messageOut.sendData();
}

RobotMotorBoard RobotMotor=RobotMotorBoard();