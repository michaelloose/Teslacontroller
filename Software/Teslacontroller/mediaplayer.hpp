#include <Arduino.h>
#include <avr/pgmspace.h>
#include "MIDI_Read.hpp"
#include <SPI.h>
#include <SdFat.h>
#include <MD_MIDIFile.h>

//Getter/Settermethoden
String getFileList(byte);
byte getCurrentFile(void);
bool getPlayingState(void);
void setCurrentFile(byte);

void pollMediaPlayer(void);
int initializeSD(void);
void printDirectory(File dir);
String fileNameAsString(File activeFile);

void midiCallback(midi_event *pev);
void sysexCallback(sysex_event *pev);
void midiSilence(void);
void playFile(void);
void pauseFile(void);
