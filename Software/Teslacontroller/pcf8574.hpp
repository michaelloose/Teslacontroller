#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>

//Das hier verursacht den Fehler
//#include "ui.hpp"

#define pcf8575adress 0x20 //I2c Adresse des Pin Expanders an dem die Knöpfe hängen
#define eeSettingsAddress 0 //Startadresse für das Auslesen vom EEPROM
#define eeDCAddress 128 //Startadresse für das Auslesen vom EEPROM

void initialiseButtons(void);
void pcf8575ISR();
void resetUserInput(void);
bool getUserInput(void);


//Datentyp für Einstellungen
struct set {
  int source[4];
  int coilType[4];
};



set getSettings(void);
void setSettings(set);

//Aktionen
void loadSettings();
void saveSettings();
