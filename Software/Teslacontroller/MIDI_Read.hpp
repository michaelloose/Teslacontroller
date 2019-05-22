#include <arduino.h>
#define DSP_Adress 0x34
#include <Wire.h>
#include "pcf8574.hpp"

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
void outputMidiToDSP(byte , byte, byte);
void noteOff(byte);
void noteOn(byte, uint32_t, uint32_t);
