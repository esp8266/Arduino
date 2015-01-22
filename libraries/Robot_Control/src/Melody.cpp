#include "ArduinoRobot.h"
#include "SquawkSD.h"
#include "Fat16.h"



SQUAWK_CONSTRUCT_ISR(SQUAWK_PWM_PIN5);


void RobotControl::beginSpeaker(uint16_t frequency){
  SquawkSynth::begin(frequency);
  SquawkSynth::play();
  osc[2].vol = 0x7F;
}

void RobotControl::playNote(byte period, word length, char modifier) {
  // Modifier . makes note length 2/3
  if(modifier == '.') length = (length * 2) / 3;
  // Set up the play frequency, 352800 is [sample_rate]=44100 * [tuning]=8.0
  osc[2].freq = 352800 / period;
  // Delay, silence, delay
  delay(length);
  osc[2].freq = 0;
  delay(length);
}

void RobotControl::playMelody(char* script){
  // Find length of play string
  word length = strlen(script);
  // Set the default note time
  word time = 500;
  // Loop through each character in the play string
  for(int n = 0; n < length; n++) {
    // Fetch the character AFTER the current one - it may contain a modifier
    char modifier = script[n + 1];
    // Fetch the current character and branch accordingly
    switch(script[n]) {
      // Notes
      case 'c': playNote(214, time, modifier); break; // Play a  C
      case 'C': playNote(202, time, modifier); break; // Play a  C#
      case 'd': playNote(190, time, modifier); break; // Play a  D
      case 'D': playNote(180, time, modifier); break; // Play a  D#
      case 'e': playNote(170, time, modifier); break; // Play an E
      case 'f': playNote(160, time, modifier); break; // Play an F
      case 'F': playNote(151, time, modifier); break; // Play an F#
      case 'g': playNote(143, time, modifier); break; // Play a  G
      case 'G': playNote(135, time, modifier); break; // Play a  G#
      case 'a': playNote(127, time, modifier); break; // Play an A
      case 'A': playNote(120, time, modifier); break; // Play an A#
      case 'b': playNote(113, time, modifier); break; // Play a  B
      // Delay
      case '-': playNote(0,   time, modifier); break; // Play a quiet note
      // Note lengths
      case '1': time = 1000; break; // Full note
      case '2': time =  500; break; // Half note
      case '4': time =  250; break; // Quarter note
      case '8': time =   50; break; // Eigth note
	  // Modifier '.' makes note length 2/3

    }
  }
}

void RobotControl::beep(int beep_length){
  char scr1[]="8F";
  char scr2[]="8Fe";
  char scr3[]="1F";

  switch (beep_length)
  {
  case BEEP_SIMPLE:
  default:
    playMelody(scr1);
    break;

  case BEEP_DOUBLE:
	playMelody(scr2);
    break;

  case BEEP_LONG:
    playMelody(scr3);
  }

}

void RobotControl::tempoWrite(int tempo){
	SquawkSynthSD::tempo(tempo);
}
void RobotControl::tuneWrite(float tune){
	SquawkSynthSD::tune(tune);
}

void RobotControl::playFile(char* filename){
  melody.open(filename,O_READ);
  SquawkSynthSD::play(melody);
}

void RobotControl::stopPlayFile(){
  melody.close();
}
