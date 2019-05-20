#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>
#define pcf8575adress 0x20 //I2c Adresse des Pin Expanders an dem die Knöpfe hängen
#define eeAddress 0 //Startadresse für das Auslesen vom EEPROM


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
