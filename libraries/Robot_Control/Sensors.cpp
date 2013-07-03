#include "ArduinoRobot.h"
#include "Multiplexer.h"
#include "Wire.h"
bool RobotControl::digitalRead(uint8_t port){
	uint8_t type=_getTypeCode(port);
	switch(type){
		case TYPE_TOP_TK:
			return _digitalReadTopMux(port);
			break;
		case TYPE_TOP_TKD:
			return _digitalReadTopPin(port);
			break;
		case TYPE_BOTTOM_TK:
			return _requestDigitalRead(port);
			break;
	}
}
int RobotControl::analogRead(uint8_t port){
	uint8_t type=_getTypeCode(port);
	switch(type){
		case TYPE_TOP_TK:
			return _analogReadTopMux(port);
			break;
		case TYPE_TOP_TKD:
			return _analogReadTopPin(port);
			break;
		case TYPE_BOTTOM_TK:
			return _requestAnalogRead(port);
			break;
	}
}
void RobotControl::digitalWrite(uint8_t port, bool value){
	uint8_t type=_getTypeCode(port);
	switch(type){
		case TYPE_TOP_TK:
			//Top TKs can't use digitalWrite?
			break;
		case TYPE_TOP_TKD:
			_digitalWriteTopPin(port, value);
			break;
		case TYPE_BOTTOM_TK:
			_requestDigitalWrite(port, value);
			break;
	}
}
void RobotControl::analogWrite(uint8_t port, uint8_t value){
	if(port==TKD4)
		::analogWrite(port,value);
}

uint8_t RobotControl::_getTypeCode(uint8_t port){
	switch(port){
		case TK0:
		case TK1:
		case TK2:
		case TK3:
		case TK4:
		case TK5:
		case TK6:
		case TK7:
			return TYPE_TOP_TK;
			break;
			
		case TKD0:
		case TKD1:
		case TKD2:
		case TKD3:
		case TKD4:
		case TKD5:
		case LED1:
			return TYPE_TOP_TKD;
			break;
			
		case B_TK1:
		case B_TK2:
		case B_TK3:
		case B_TK4:
			return TYPE_BOTTOM_TK;
			break;
	}
}
uint8_t RobotControl::_portToTopMux(uint8_t port){
	switch(port){
		case TK0:
			return 0;
		case TK1:
			return 1;
		case TK2:
			return 2;
		case TK3:
			return 3;
		case TK4:
			return 4;
		case TK5:
			return 5;
		case TK6:
			return 6;
		case TK7:
			return 7;
	}
}
uint8_t RobotControl::_topDPortToAPort(uint8_t port){
	switch(port){
		case TKD0:
		  return A1;
		case TKD1:
		  return A2;
		case TKD2:
		  return A3;
		case TKD3:
		  return A4;
		case TKD4:
		  return A7;
		case TKD5:
		  return A11;
	}
}
int* RobotControl::parseMBDPort(uint8_t port){
	//Serial.println(port);
	switch(port){
		case B_TK1:
			return &motorBoardData._B_TK1;
		case B_TK2:
			return &motorBoardData._B_TK2;
		case B_TK3:
			return &motorBoardData._B_TK3;
		case B_TK4:
			return &motorBoardData._B_TK4;
		
		/*		
		case B_IR0:
			return &motorBoardData._B_IR0;
		case B_IR1:
			return &motorBoardData._B_IR1;
		case B_IR2:
			return &motorBoardData._B_IR2;
		case B_IR3:
			return &motorBoardData._B_IR3;
		case B_IR4:
			return &motorBoardData._B_IR4;*/
	}
}
int RobotControl::get_motorBoardData(uint8_t port){
	return *parseMBDPort(port);
}
void RobotControl::set_motorBoardData(uint8_t port, int data){
	*parseMBDPort(port)=data;
}

bool RobotControl::_digitalReadTopMux(uint8_t port){
	uint8_t num=_portToTopMux(port);
	return Multiplexer::getDigitalValueAt(num);
}

int RobotControl::_analogReadTopMux(uint8_t port){
	uint8_t num=_portToTopMux(port);
	return Multiplexer::getAnalogValueAt(num);
}

bool RobotControl::_digitalReadTopPin(uint8_t port){
	return ::digitalRead(port);
}
int RobotControl::_analogReadTopPin(uint8_t port){
	uint8_t aPin=_topDPortToAPort(port);
	return ::analogRead(aPin);
}
void RobotControl::_digitalWriteTopPin(uint8_t port, bool value){
	::digitalWrite(port, value);
}

bool RobotControl::_requestDigitalRead(uint8_t port){
	messageOut.writeByte(COMMAND_DIGITAL_READ);
	messageOut.writeByte(port);//B_TK1 - B_TK4
	messageOut.sendData();
	delay(10);
	if(messageIn.receiveData()){
		//Serial.println("*************");
		uint8_t cmd=messageIn.readByte();
		//Serial.print("cmd: ");
		//Serial.println(cmd);
		if(!(cmd==COMMAND_DIGITAL_READ_RE))
			return false;
			
		uint8_t pt=messageIn.readByte(); //Bottom TK port codename
		//Serial.print("pt: ");
		//Serial.println(pt);
		set_motorBoardData(pt,messageIn.readByte());
		return get_motorBoardData(port);
	}
}
int RobotControl::_requestAnalogRead(uint8_t port){
	messageOut.writeByte(COMMAND_ANALOG_READ);
	messageOut.writeByte(port);//B_TK1 - B_TK4
	messageOut.sendData();
	delay(10);
	if(messageIn.receiveData()){
		uint8_t cmd=messageIn.readByte();
		//Serial.println("*************");
		//Serial.print("cmd: ");
		//Serial.println(cmd);
		if(!(cmd==COMMAND_ANALOG_READ_RE))
			return false;
			
		uint8_t pt=messageIn.readByte();
		//Serial.print("pt: ");
		//Serial.println(pt);
		set_motorBoardData(pt,messageIn.readInt());
		return get_motorBoardData(port);
	}
}
void RobotControl::_requestDigitalWrite(uint8_t selector, uint8_t value){
	messageOut.writeByte(COMMAND_DIGITAL_WRITE);
	messageOut.writeByte(selector);//B_TK1 - B_TK4
	messageOut.writeByte(value);
	messageOut.sendData();
}





void RobotControl::updateIR(){
	messageOut.writeByte(COMMAND_READ_IR);
	messageOut.sendData();
	delay(10);
	if(messageIn.receiveData()){
		if(messageIn.readByte()==COMMAND_READ_IR_RE){
			for(int i=0;i<5;i++){
				IRarray[i]=messageIn.readInt();
			}
		}
	}
}

int RobotControl::knobRead(){
	return ::analogRead(POT);
}

int RobotControl::trimRead(){
	messageOut.writeByte(COMMAND_READ_TRIM);
	messageOut.sendData();
	delay(10);
	if(messageIn.receiveData()){
		uint8_t cmd=messageIn.readByte();
		if(!(cmd==COMMAND_READ_TRIM_RE))
			return false;
			
		uint16_t pt=messageIn.readInt();
		return pt;
	}
}

uint16_t RobotControl::compassRead(){
	return Compass::getReading();
}

/*
void RobotControl::beginUR(uint8_t pinTrigger, uint8_t pinEcho){
	pinTrigger_UR=pinTrigger;
	pinEcho_UR=pinEcho;
	
	pinMode(pinEcho_UR, INPUT);
	pinMode(pinTrigger_UR, OUTPUT);
}
uint16_t RobotControl::getDistance(){
	digitalWrite(pinTrigger_UR, LOW);                   // Set the trigger pin to low for 2uS
	delayMicroseconds(2);
	digitalWrite(pinTrigger_UR, HIGH);                  // Send a 10uS high to trigger ranging
	delayMicroseconds(10);
	digitalWrite(pinTrigger_UR, LOW);                   // Send pin low again
	uint16_t distance = pulseIn(pinEcho_UR, HIGH);        // Read in times pulse
	distance= distance/58;                        // Calculate distance from time of pulse
	return distance;
}*/