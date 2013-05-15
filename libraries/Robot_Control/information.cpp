/*#include <ArduinoRobot.h>
//0 - 319: pic array, 

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


void RobotControl::getMyName(char* container){
	EEPROM_I2C::readBuffer(ADDRESS_USERNAME,(uint8_t*)container,18);
}
void RobotControl::getRobotName(char* container){
	EEPROM_I2C::readBuffer(ADDRESS_ROBOTNAME,(uint8_t*)container,18);
}
void RobotControl::getMyCity(char* container){
	EEPROM_I2C::readBuffer(ADDRESS_CITYNAME,(uint8_t*)container,18);
}
void RobotControl::getMyCountry(char* container){
	EEPROM_I2C::readBuffer(ADDRESS_COUNTRYNAME,(uint8_t*)container,18);
}

void RobotControl::setMyName(char* text){
	EEPROM_I2C::writePage(ADDRESS_USERNAME,(uint8_t*)text,18);
}
void RobotControl::setRobotName(char* text){
	EEPROM_I2C::writePage(ADDRESS_ROBOTNAME,(uint8_t*)text,18);
}
void RobotControl::setMyCity(char* text){
	EEPROM_I2C::writePage(ADDRESS_CITYNAME,(uint8_t*)text,18);
}
void RobotControl::setMyCountry(char* text){
	EEPROM_I2C::writePage(ADDRESS_COUNTRYNAME,(uint8_t*)text,18);
}
*/