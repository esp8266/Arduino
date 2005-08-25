#ifndef _MIDI_H
#define _MIDI_H

#define MIDI_NOTE_ON 	0x90
#define MIDI_NOTE_OFF 	0x80

// 1001cccc 0nnnnnnn 0vvvvvvv
#define MIDI_POLY_PRESSURE 	0xA0
// 1011cccc 0nnnnnnn 0vvvvvvv
#define MIDI_CONTROL_CHANGE	0xB0
// 1100cccc 0ppppppp
#define MIDI_PROGRAM_CHANGE	0xC0

#define MIDI_DATA_MASK			0x7F
#define MIDI_STATUS_MASK		0xF0
#define MIDI_CHANNEL_MASK		0x0F

#define MIDI_BAUD_RATE			31250

#include "global.h"
#include "buffer.h"

void midiInit(void);
u08 midiNoteOnOut(u08 note, u08 vel, u08 channel);
u08 midiNoteOffOut(u08 note, u08 vel, u08 channel);
u08 midiControlChangeOut(u08 controller, u08 value, u08 channel);
u08 midiProgramChangeOut(u08 program, u08 channel);


#endif
