#ifndef ui.hpp
#define ui.hpp



//#include <U8g2lib.h>
#include "lib/SPI/src/SPI.h"


#include "mediaplayer.hpp"
#include "logo.cpp"

#define xcol1 0
#define xcol2 54
#define xcol3 108
#define xcol4 162
#define xcol5 224
#define xtop1 0
#define xtop2 60
#define xtop3 120
#define xtop4 180

#define ytop 0
#define ybot 55
#define yrow1 36
#define yrow2 23

#define xtext 10
#define xcursor 0

//Methoden zum Einlesen der Knöpfe

void readUserInput(void);
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
void printCoilSetupScreen2(void);
void printCoilTestScreen(void);
void printSelfTestScreen(void);
void printCreditsScreen(void);



//Konversionsmethoden
String sourceToString (int);
String intToString(int);                                                        //Mögliche Verbesserung: String(var, DEC) benutzen





#endif
