#include <Arduino.h>
#include <avr/pgmspace.h>
#include "MIDI_Read.hpp"


//Getter/Settermethoden
String getFileList(int);
byte getCurrentFile(void);
void setCurrentFile(byte);


int initializeSD(void);
void playFile(void);
void pauseFile(void);
