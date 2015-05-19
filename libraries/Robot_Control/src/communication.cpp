#include <ArduinoRobot.h>

bool RobotControl::isActionDone(){
	if(messageIn.receiveData()){
		if(messageIn.readByte()==COMMAND_ACTION_DONE){
			return true;
		}
	}
	return false;
}

void RobotControl::pauseMode(uint8_t onOff){
	messageOut.writeByte(COMMAND_PAUSE_MODE);
	if(onOff){
		messageOut.writeByte(true);
	}else{
		messageOut.writeByte(false);
	}
	messageOut.sendData();
}

void RobotControl::lineFollowConfig(uint8_t KP, uint8_t KD, uint8_t robotSpeed, uint8_t intergrationTime){
	messageOut.writeByte(COMMAND_LINE_FOLLOW_CONFIG);
	messageOut.writeByte(KP);
	messageOut.writeByte(KD);
	messageOut.writeByte(robotSpeed);
	messageOut.writeByte(intergrationTime);
	messageOut.sendData();
}
