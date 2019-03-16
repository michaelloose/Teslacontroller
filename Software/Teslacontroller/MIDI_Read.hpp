#include<arduino.h>

// Aktiver MIDI Kanal, 0x00 - 0x0F
#define responseChannel 0x00
// ueberschreibt die Kanalselektierung
#define respondAllChannels false

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

//leerers Dummy Byte. Wird eingesetzt sobald ein Befehl ignoriert werden soll
#define MIDI_IGNORE 0x00

// midi "state" - which data byte we are receiving
#define MIDI_STATE_BYTE1 0x00
#define MIDI_STATE_BYTE2 0x01
