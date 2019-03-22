#include <Arduino.h>
#include <U8g2lib.h>
#include <EEPROM.h>
#include <SPI.h>
#include <avr/pgmspace.h>

#include<IoAbstraction.h>
#include<IoAbstractionWire.h>

#include "logo.cpp"

#define pcf8575adress 0x20
#define eeAddress 0 //Startadresse für das Auslesen vom EEPROM
//Datentyp für Einstellungen
struct set {
  int source[4];
  int coilType[4];
};
set getSettings(void);
//Methoden zum Einlesen der Knöpfe
void initialiseButtons(void);
bool getButtonPressed(void);
byte getCurrentScreen(void);

void onButtonClicked(uint8_t,  bool );
void onEncoderChange(int );

//Methoden zur Ausgabe auf dem Display
void refreshScreen(byte, bool[3]);
void initialiseDisplay(void);
void printStartScreen(void);
void printHomeScreen(bool[3]);
void printMenuScreen(void);

//Aktionen
void loadSettings();
void saveSettings(int, set);

//Konversionsmethoden
String sourceToString (int);
String intToString(int);
