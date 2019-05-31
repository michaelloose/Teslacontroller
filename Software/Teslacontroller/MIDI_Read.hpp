#ifndef MIDI_Read_hpp
#define MIDI_Read_hpp

#include <arduino.h>
#include <Wire.h>
#include "pcf8574.hpp"
#include <avr/pgmspace.h>

#define DSP_Adress 0x34
// midi commands
#define MIDI_CMD_NOTE_OFF 0x80
#define MIDI_CMD_NOTE_ON 0x90


//Wird in dem Code zur Zeit nicht verwendet
#define MIDI_CMD_KEY_PRESSURE 0xA0
#define MIDI_CMD_CONTROLLER_CHANGE 0xB0
#define MIDI_CMD_PROGRAM_CHANGE 0xC0
#define MIDI_CMD_CHANNEL_PRESSURE 0xD0
#define MIDI_CMD_PITCH_BEND 0xE0
#define MIDI_CMD_SYSEX 0xF0




void readMidi(byte, byte, byte);
void readMidiInputOn(byte, byte, byte);
void readMidiInputOff(byte, byte, byte);
void readMidiPitchBend(byte, int);
void readMidiSysex(byte* , unsigned);


uint32_t getFrequency(byte);
void outputMidiToDSP(byte , byte, byte);
void noteOff(byte);
void noteOn(byte, uint32_t, uint32_t);
void resetPitchBend(void);


#endif
