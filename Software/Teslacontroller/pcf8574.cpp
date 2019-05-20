//pcf8574.cpp
//
//User Interface für den Teslacontroller
//Enthält alle Methoden für das Auslesen der Knöpfe und die Anzeigen auf dem Display
//
//Author: Michael Loose
//FHWS Fakultät Elektrotechnik
//
//Erstellung: 5/2019
//Stand: 13/5/2019
//////////////////////////////////////////////////////////////////////


#include "pcf8574.hpp"


set settings; //Variable zum Speichern der Einstellungen erstellen

void loadSettings() {
  //Einstellungen aus dem EEprom laden
  EEPROM.get( eeAddress, settings); //Laden der Einstellungen aus dem EEprom

  //DEBUG CODE
  //Serielle ausgabe der Einstellungen
  Serial.println("Einstellungen erfolgreich geladen!");
  Serial.print("Source: ");

  for (int i = 0; i < 4; i++)
  {
    Serial.print( settings.source[i] );
    Serial.print(" ");
  }

  Serial.println("");
  Serial.print("CoilType: ");

  for (int i = 0; i < 4; i++)
  {
    Serial.print( settings.coilType[i] );
    Serial.print(" ");
  }

}

void saveSettings() {
  EEPROM.put(eeAddress , settings);
  Serial.print("Saving Complete");
}

set getSettings(void) {
  return settings;
}

void setSettings(set locSettings){
  settings = locSettings;
}



//Initialisierung der Eingabeknöpfe
void initialiseButtons(void) {
  Wire.begin();
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), pcf8575ISR, FALLING);
  Wire.requestFrom(pcf8575adress, 1);
  Serial.println("PCF8575 erfolgreich initialisiert!");

}

//Interrupt Service Routine für Benutzereingabe
volatile bool userInput = false;

void pcf8575ISR() {
  userInput = true;
}

bool getUserInput(void) {
  return userInput;
}

void resetUserInput(void) {
  userInput = false;
}
