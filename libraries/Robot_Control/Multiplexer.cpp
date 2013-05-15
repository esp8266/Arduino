#include "Multiplexer.h"

void Multiplexer::begin(uint8_t* selectors, uint8_t Z, uint8_t length){
	for(uint8_t i=0;i<length;i++){
		this->selectors[i]=selectors[i];
		pinMode(selectors[i],OUTPUT);
	}
	this->length=length;
	this->pin_Z=Z;
	pinMode(pin_Z,INPUT);
}

void Multiplexer::selectPin(uint8_t num){
	for(uint8_t i=0;i<length;i++){
		//Serial.print(bitRead(num,i));
		digitalWrite(selectors[i],bitRead(num,i));
	}
	//Serial.println("");
}

int Multiplexer::getAnalogValue(){
	return analogRead(pin_Z);
}

bool Multiplexer::getDigitalValue(){
	return digitalRead(pin_Z);
}

int Multiplexer::getAnalogValueAt(uint8_t num){
	selectPin(num);
	return getAnalogValue();
}

bool Multiplexer::getDigitalValueAt(uint8_t num){
	selectPin(num);
	return getDigitalValue();
}
