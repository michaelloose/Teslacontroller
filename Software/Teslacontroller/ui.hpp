//#include <U8g2lib.h>
#include "lib/SPI/src/SPI.h"


#include "mediaplayer.hpp"
#include "logo.cpp"



//Methoden zum Einlesen der Knöpfe

void pollUserInput(void);
bool getButtonPressed(void);
byte getCurrentScreen(void);

void onButtonClicked(uint8_t);
void onEncoderChange(bool);

//Methoden zur Ausgabe auf dem Display
void refreshScreen(void);
void initialiseDisplay(void);
void printStartScreen(void);

void printHomeScreen(bool[3]);
void printMenuScreen(void);
void printPlayerFileScreen(void);
void printCoilSetupScreen(void);
void printCoilTestScreen(void);
void printSelfTestScreen(void);
void printCreditsScreen(void);



//Konversionsmethoden
String sourceToString (int);
String intToString(int);                                                        //Mögliche Verbesserung: String(var, DEC) benutzen
