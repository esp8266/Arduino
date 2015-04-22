#include "ArduinoRobot.h"
#include "EasyTransfer2.h"


void RobotControl::motorsStop(){
	messageOut.writeByte(COMMAND_MOTORS_STOP);
	messageOut.sendData();
}
void RobotControl::motorsWrite(int speedLeft,int speedRight){
	messageOut.writeByte(COMMAND_RUN);
	messageOut.writeInt(speedLeft);
	messageOut.writeInt(speedRight);
	messageOut.sendData();
}
void RobotControl::motorsWritePct(int speedLeftPct, int speedRightPct){
	int16_t speedLeft=255*speedLeftPct/100.0;
	int16_t speedRight=255*speedRightPct/100.0;
	motorsWrite(speedLeft,speedRight);
}
void RobotControl::pointTo(int angle){
	int target=angle;
	uint8_t speed=80;
	target=target%360;
	if(target<0){
		target+=360;
	}
	int direction=angle;
	while(1){
		int currentAngle=compassRead();
		int diff=target-currentAngle;
		direction=180-(diff+360)%360;
		if(direction>0){
			motorsWrite(speed,-speed);//right
			delay(10);
		}else{
			motorsWrite(-speed,speed);//left
			delay(10);
		}
		//if(diff<-180) 
		//	diff += 360;
		//else if(diff> 180) 
		//	diff -= 360;
		//direction=-diff;
		
		if(abs(diff)<5){
			motorsStop();
			return;
		}
	}
}
void RobotControl::turn(int angle){
	int originalAngle=compassRead();
	int target=originalAngle+angle;
	pointTo(target);
	/*uint8_t speed=80;
	target=target%360;
	if(target<0){
		target+=360;
	}
	int direction=angle;
	while(1){
		if(direction>0){
			motorsWrite(speed,speed);//right
			delay(10);
		}else{
			motorsWrite(-speed,-speed);//left
			delay(10);
		}
		int currentAngle=compassRead();
		int diff=target-currentAngle;
		if(diff<-180) 
			diff += 360;
		else if(diff> 180) 
			diff -= 360;
		direction=-diff;
		
		if(abs(diff)<5){
			motorsWrite(0,0);
			return;
		}
	}*/
}

void RobotControl::moveForward(int speed){
	motorsWrite(speed,speed);
}
void RobotControl::moveBackward(int speed){
	motorsWrite(speed,speed);
}
void RobotControl::turnLeft(int speed){
	motorsWrite(speed,255);
}
void RobotControl::turnRight(int speed){
	motorsWrite(255,speed);
}



/*
int RobotControl::getIRrecvResult(){
	messageOut.writeByte(COMMAND_GET_IRRECV);
	messageOut.sendData();
	//delay(10);
	while(!messageIn.receiveData());
	
	if(messageIn.readByte()==COMMAND_GET_IRRECV_RE){
		return messageIn.readInt();
	}


	return -1;
}
*/