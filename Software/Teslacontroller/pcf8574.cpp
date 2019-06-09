//pcf8574.cpp
//
//User Interface für den Teslacontroller
//Enthält alle Methoden für das Auslesen der Knöpfe und die Anzeigen auf dem Display
//
//Author: Michael Loose
//FHWS Fakultät Elektrotechnik
//
//Erstellung: 5/2019
//Stand: 20/5/2019
//////////////////////////////////////////////////////////////////////


#include "pcf8574.hpp"


set settings; //Variable zum Speichern der Einstellungen erstellen

//Speichern die aktuelle encoder Drehrichtung
bool encplus = true;
bool encminus = true;

//Wird, sofern der Interrupt ausgelöst wurde, im normalen Durchlauf ausgelöst. Liest den PCF aus und verarbeitet den Eingang.
void readUserInput() {

  //Auslesen der Daten über i2c
  Wire.requestFrom(pcf8575adress, 1);
  byte incomingByte = Wire.read();

  //Encoder in Ruhestellung?
  if (incomingByte == 0b00000011) {
    encplus = true;
    encminus = true;
  }
  //Encoder im Uhrzeigersinn?
  if ((incomingByte == 0b00000001) && encminus) {
    encplus = false;
    onEncoderChange(true);
  }
  //Encoder im Gegenuhrzeigersinn?
  if ((incomingByte == 0b00000010) && encplus ) {
    encminus = false;
    onEncoderChange(false);
  }
  //Taste gedrückt?
  if (incomingByte & 0b00000100) onButtonClicked(6);
  if (incomingByte & 0b00001000) onButtonClicked(1);
  if (incomingByte & 0b00010000) onButtonClicked(2);
  if (incomingByte & 0b00100000) onButtonClicked(3);
  if (incomingByte & 0b01000000) onButtonClicked(4);
  if (incomingByte & 0b10000000) onButtonClicked(5);

}
void loadSettings() {
  //Einstellungen aus dem EEprom laden
  EEPROM.get(eeSettingsAddress , settings); //Laden der Einstellungen aus dem EEprom

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
  EEPROM.put(eeSettingsAddress , settings);
  Serial.print("Saving Complete");
  printNotificationScreen(1);
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
