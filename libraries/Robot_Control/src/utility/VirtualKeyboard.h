#ifndef VIRTUAL_KEYBOARD_H
#define VIRTUAL_KEYBOARD_H

#include <Arduino.h>
#include <ArduinoRobot.h>

class VirtualKeyboard{
	public:
		//void begin();
		void display(uint8_t top, uint16_t showColor=BLACK, uint16_t hideColor=WHITE);
		void hide();
		
		char getSelection();
		void run();
		
	private:
		uint8_t top;
		bool onOff;
		
		uint16_t showColor;
		uint16_t hideColor;
		
		int getColLin(int val);

};

extern VirtualKeyboard Vkey;
#endif