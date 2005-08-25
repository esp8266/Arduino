//  Midi.c
//
//  Midi output routines for the atmel atmega163 (and others)
//  
//  depends on avrlib for buffer
//  

#include "uart.h"
#include "midi.h"
#include "debug.h"


void midiInit() {
	uartInit();
	uartSetBaudRate(MIDI_BAUD_RATE);
}

u08 midiNoteOnOut(u08 note, u08 vel, u08 channel) {
	uartSendByte(MIDI_NOTE_ON | (channel & MIDI_CHANNEL_MASK));
	uartSendByte(MIDI_DATA_MASK & note);
	uartSendByte(MIDI_DATA_MASK & vel);

	return 0;
}

u08 midiNoteOffOut(u08 note, u08 vel, u08 channel) {
	uartSendByte(MIDI_NOTE_OFF | (channel & MIDI_CHANNEL_MASK));
	uartSendByte(MIDI_DATA_MASK & note);
	uartSendByte(MIDI_DATA_MASK & vel);

	return 0;
}

u08 midiControlChangeOut(u08 controller, u08 value, u08 channel) {
	uartSendByte(MIDI_CONTROL_CHANGE | (channel & MIDI_CHANNEL_MASK));
	uartSendByte(MIDI_DATA_MASK & controller);
	uartSendByte(MIDI_DATA_MASK & value);

	return 0;
}

u08 midiProgramChangeOut(u08 program, u08 channel) {
	uartSendByte(MIDI_PROGRAM_CHANGE | (channel & MIDI_CHANNEL_MASK));
	uartSendByte(MIDI_DATA_MASK & program);

	return 0;
}

