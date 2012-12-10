/*
  Esplora Music

  This sketch turns the Esplora in a simple musical instrument.
  Press the Switch 1 and move the slider to see how it works.

  Created on 22 november 2012
  By Enrico Gueli <enrico.gueli@gmail.com>
  modified 24 Nov 2012
  by Tom Igoe
*/


#include <Esplora.h>


const int note[] = {
262, // C
277, // C#
294, // D
311, // D#
330, // E
349, // F
370, // F#
392, // G
415, // G#
440, // A
466, // A#
494, // B
523  // C next octave
};

void setup() {
}

void loop() {
  // read the button labeled SWITCH_DOWN. If it's low,
  // then play a note:
  if (Esplora.readButton(SWITCH_DOWN) == LOW) {
    int slider = Esplora.readSlider();
    
    // use map() to map the slider's range to the 
    // range of notes you have:
    byte thisNote = map(slider, 0, 1023, 0, 13);
    // play the note corresponding to the slider's position:
    Esplora.tone(note[thisNote]);
  }
  else {
    // if the button isn't pressed, turn the note off:
    Esplora.noTone();
  }
}
