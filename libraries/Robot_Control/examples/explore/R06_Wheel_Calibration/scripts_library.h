#include <avr/pgmspace.h>
#include <ArduinoRobot.h>

const char script1[] PROGMEM="Wheel Calibration\n";
const char script2[] PROGMEM="1. Put Robot on a flat surface\n";
const char script3[] PROGMEM="2. Adjust speed with the knob on top\n";
const char script4[] PROGMEM="3. If robot goes straight, it's done\n";
const char script5[] PROGMEM="4. Use screwdriver on the trim on bottom\n";
const char script6[] PROGMEM="Robot turns left, screw it clockwise;\n";
const char script7[] PROGMEM="Turns right, screw it ct-colockwise;\n";
const char script8[] PROGMEM="5. Repeat 4 until going straight\n";

char buffer[42];//must be longer than text

const char * const scripts[] PROGMEM = {
  script1,
  script2,
  script3,
  script4,
  script5,
  script6,
  script7,
  script8,
};

void getPGMtext(int seq){
  strcpy_P(buffer,(char*)pgm_read_word(&(scripts[seq])));
}

void writePGMtext(int seq){
  getPGMtext(seq);
  Robot.print(buffer);
}

void writeScript(int seq){
  writePGMtext(seq);
}

void writeAllScripts(){
  for(int i=0;i<8;i++){
    writeScript(i);
  }  
}
