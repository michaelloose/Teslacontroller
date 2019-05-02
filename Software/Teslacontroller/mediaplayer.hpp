#include <Arduino.h>
#include <avr/pgmspace.h>
#include "MIDI_Read.hpp"
#include <SPI.h>
#include <SD.h>

//Getter/Settermethoden
String getFileList(byte);
byte getCurrentFile(void);
bool getPlayingState(void);
void setCurrentFile(byte);

void pollMediaPlayer(void);
int initializeSD(void);
void printDirectory(File dir);
void playFile(void);
void pauseFile(void);
