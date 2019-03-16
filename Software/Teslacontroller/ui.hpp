#include <Arduino.h>
#include <U8g2lib.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include "logo.cpp"

//Datentyp für Einstellungen
struct set {
  int source[4];
  int coilType[4];
};

//Methoden zum Einlesen der Knöpfe
void initialiseButtons(void);
void pcf8575ISR(void);
void readButtons(void);
bool getButtonPressed(void);

//Methoden zur Ausgabe auf dem Display
void initialiseDisplay(void);
void printStartScreen(void);
void printHomeScreen(set, bool[3]);
void printMenuScreen(int);

//Aktionen
void saveSettings(int, set);

//Konversionsmethoden
String sourceToString (int);
String intToString(int);
