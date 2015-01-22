#include <ArduinoRobot.h>

void RobotControl::beginSD(){
  card.init();
  file.init(&card);
  melody.init(&card);
}

void RobotControl::_enableSD(){
  DDRB = DDRB & 0xDF; //pinMode(CS_LCD,INPUT);
  DDRB = DDRB | 0x10; //pinMode(CS_SD,OUTPUT);
}

/*
void RobotControl::sdTest(){
  file.open("Infor.txt",O_READ);
  uint8_t buf[7];
  char n;
  while ((n = file.read(buf, sizeof(buf))) > 0) {
    for (uint8_t i = 0; i < n; i++) Serial.write(buf[i]);
  }
}*/