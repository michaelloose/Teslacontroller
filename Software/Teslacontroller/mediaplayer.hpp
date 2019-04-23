#include <Arduino.h>
#include <avr/pgmspace.h>
#include "MIDI_Read.hpp"


//Getter/Settermethoden
String getFileList(byte);
byte getCurrentFile(void);
bool getPlayingState(void);
void setCurrentFile(byte);

void pollMediaPlayer(void);
int initializeSD(void);
void playFile(void);
void pauseFile(void);
