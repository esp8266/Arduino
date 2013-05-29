#include "ArduinoRobot.h"
#include "Multiplexer.h"
#include "Wire.h"
#include "EasyTransfer2.h"

//RobotControl::RobotControl(){}

RobotControl::RobotControl():Arduino_LCD(LCD_CS,DC_LCD,RST_LCD){

}

void RobotControl::begin(){
	Wire.begin();
	//Compass
	//nothing here
		
	//TK sensors
	uint8_t MuxPins[]={MUXA,MUXB,MUXC,MUXD};
	Multiplexer::begin(MuxPins,MUX_IN,4);
	
	//piezo
	pinMode(BUZZ,OUTPUT);
	
	//communication
	Serial1.begin(9600);
	messageOut.begin(&Serial1);
	messageIn.begin(&Serial1);
	
	//TFT initialization
	//Arduino_LCD::initR(INITR_GREENTAB);
}

void RobotControl::setMode(uint8_t mode){
	messageOut.writeByte(COMMAND_SWITCH_MODE);
	messageOut.writeByte(mode);
	messageOut.sendData();
}


RobotControl Robot=RobotControl();