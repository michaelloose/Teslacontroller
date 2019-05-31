#ifndef mediaplayer_hpp
#define mediaplayer_hpp


#include <Arduino.h>
//#include <avr/pgmspace.h>
#include "MIDI_Read.hpp"
#include <SPI.h>
#include <SdFat.h>
#include <MD_MIDIFile.h>

//#include"pcf8574.hpp"

//Getter/Settermethoden
String getFileList(byte);
byte getCurrentFile(void);
bool getPlayingState(void);
void setCurrentFile(byte);
void setFileSelected(void);

int getNumberOfLoadedFiles(void);

void loadMidiFile(void);
bool playMidiFile(void);
void pollMediaPlayer(void);
int initializeSD(void);
void createfileList(File dir);
String fileNameAsString(File activeFile);
bool isMidi(char* filename);

void midiCallback(midi_event *pev);
void sysexCallback(sysex_event *pev);
void midiSilence(void);
void playFile(void);
void pauseFile(void);
void resetFile(void);
void selectFile(byte);


#endif
