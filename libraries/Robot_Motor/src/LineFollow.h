#ifndef LINE_FOLLOW_H
#define LINE_FOLLOW_H

#if ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

class LineFollow{
	public:
		LineFollow();

		void calibIRs();
		void runLineFollow();
		void config(uint8_t KP, uint8_t KD, uint8_t robotSpeed, uint8_t intergrationTime);
		
		//These are all pure virtual functions, pure VF needs pure specifier "=0"
		//virtual void motorsWrite(int speedL, int speedR)=0;
		virtual void motorsWritePct(int speedLpct, int speedRpct)=0;
		virtual void motorsStop()=0;
		virtual int _IRread(uint8_t num)=0;
	protected:
		virtual	void reportActionDone()=0;
		
	private:
		void doCalibration(int speedPct, int time);
		void ajusta_niveles();
		
		uint8_t KP;
		uint8_t KD;
		uint8_t robotSpeed; //percentage
		uint8_t intergrationTime;
		
		int lectura_sensor[5], last_error, acu;
		int sensor_blanco[5];
		int sensor_negro[5];
};

#endif