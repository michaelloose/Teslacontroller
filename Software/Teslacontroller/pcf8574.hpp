#ifndef pcf8574_hpp
#define pcf8574_hpp


#include <Arduino.h>
#include <EEPROM.h>
#include <Wire.h>

#include "ui.hpp"


#define pcf8575adress 0x20 //I2c Addresse des Pin Expanders an dem die Knöpfe hängen
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
  byte pitchBendRange;
  byte MIDIADChannel;
  bool autoplay;
  bool serialDebugging;
};


set getSettings(void);
void setSettings(set);

//Aktionen
void loadSettings();
void saveSettings();



#endif
